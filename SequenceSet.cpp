#include "SequenceSet.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

using namespace std;

const int BLOCK_SIZE = 50;

// Salva um bloco no arquivo calculando a posicao correta baseada no sistema de posicoes da estrutura.
void SequenceSet::writeBlock(const Block &block, int position)
{
    file.seekp(sizeof(Header) + position * sizeof(Block), ios::beg);
    file.write(reinterpret_cast<const char *>(&block), sizeof(Block));
}

// Le um bloco no arquivo, se baseia na posicao do bloco para fazer a leitura.
Block SequenceSet::readBlock(int position)
{
    Block block;
    file.seekg(sizeof(Header) + position * sizeof(Block), ios::beg);
    file.read(reinterpret_cast<char *>(&block), sizeof(Block));
    return block;
}

// Salva o cabecalho no arquivo, usado sempre para salvar as alteracoes feitas.
void SequenceSet::writeHeader(fstream &file, const Header &header)
{
    file.seekp(0, ios::beg);
    file.write(reinterpret_cast<const char *>(&header), sizeof(Header));
}

// Le o cabecalho do arquivo, usado para carregar as propriedades atuais do arquivo.
Header SequenceSet::readHeader(fstream &file)
{
    Header header;
    file.seekg(0, ios::beg);
    file.read(reinterpret_cast<char *>(&header), sizeof(Header));
    return header;
}

SequenceSet::SequenceSet(const string &fileName)
{
    file.open(fileName, ios::in | ios::out | ios::binary);

    if (!file.is_open())
    {
        file.open(fileName, ios::out | ios::binary);

        if (!file.is_open())
        {
            cerr << "Erro: Não foi possível criar o arquivo." << endl;
            throw runtime_error("Erro ao criar arquivo binário.");
        }

        file.close();
        file.open(fileName, ios::in | ios::out | ios::binary);
        writeHeader(file, header);
    }
    else
    {
        file.seekg(0, ios::end);

        if (file.tellg() == 0)
            writeHeader(file, header);

        header = readHeader(file);
    }
}

// Destrutor da estrutura, fecha o arquivo e salva o ultimo estado do cabecalho.
SequenceSet::~SequenceSet()
{
    if (file.is_open())
    {
        file.seekp(0, ios::beg);
        file.write(reinterpret_cast<const char *>(&header), sizeof(Header));
        file.close();
    }
}

// Percorre os blocos comparando seus menores e maiores elementos com o elementos de busca para otimizar as iteracoes ate encontra-lo ou percorrer todos os blocos.
// Caso nao encontre retorna um registro vazio com um ID especifico para esses casos.
Person SequenceSet::searchPersonById(int id)
{
    if (!file.is_open())
    {
        cerr << "Arquivo não está aberto." << endl;
        throw runtime_error("Arquivo não está aberto.");
    }

    int currentBlockPos = header.firstBlock;

    while (currentBlockPos != -1)
    {
        Block currentBlock = readBlock(currentBlockPos);

        if (id >= currentBlock.data[0].id && id <= currentBlock.data[currentBlock.count - 1].id)
        {
            for (int i = 0; i < currentBlock.count; i++)
            {
                if (currentBlock.data[i].id == id)
                {
                    return currentBlock.data[i];
                }
            }
        }

        currentBlockPos = currentBlock.nextBlock;
    }

    Person e;
    return e;
}

// Insere um registro no arquivo, verifica se o arquivo esta vazio, cheio ou entao procura o bloco e a posicao correta.
void SequenceSet::insertPerson(const Person &person)
{
    {
        if (!file.is_open())
        {
            cerr << "Arquivo nao pode ser encontrado." << endl;
            throw runtime_error("Arquivo fechado.");
        }

        if (searchPersonById(person.id).id != -1)
        {
            system("clear||cls");
            cerr << "Erro: ID ja cadastrado. Cada ID deve ser unico!" << endl;
            return;
        }

        // Logica para caso o arquivo esteja vazio.
        if (header.firstBlock == -1)
        {
            Block newBlock;
            newBlock.numBlock = 0;
            newBlock.data[0] = person;
            newBlock.count = 1;
            newBlock.nextBlock = -1;

            header.firstBlock = 0;
            header.blockCount = 1;
            header.nextFreeBlock = 1;

            writeBlock(newBlock, 0);
            writeHeader(file, header);
            return;
        }

        int currentBlockPos = header.firstBlock;
        int previousBlockPos = -1;

        // Logica para procurar pelo arquivo o bloco correto, achando o bloco procura-se entao a posicao corretam ajusta os demais registros.
        while (currentBlockPos != -1)
        {
            Block currentBlock = readBlock(currentBlockPos);
            if (person.id <= currentBlock.data[currentBlock.count - 1].id || currentBlock.nextBlock == -1)
            {
                if (currentBlock.count == BLOCK_SIZE)
                {
                    Block newBlock;
                    newBlock.numBlock = header.nextFreeBlock;
                    newBlock.nextBlock = currentBlock.nextBlock;

                    // Move metade dos registros para o novo bloco.
                    int halfSize = BLOCK_SIZE / 2;
                    int newBlockIndex = 0;
                    for (int j = halfSize; j < BLOCK_SIZE; j++)
                        newBlock.data[newBlockIndex++] = currentBlock.data[j];

                    newBlock.count = newBlockIndex;
                    currentBlock.count = halfSize;

                    currentBlock.nextBlock = newBlock.numBlock;

                    header.blockCount++;
                    header.nextFreeBlock++;
                    writeBlock(currentBlock, currentBlockPos);
                    writeBlock(newBlock, newBlock.numBlock);
                    writeHeader(file, header);

                    if (person.id > currentBlock.data[currentBlock.count - 1].id)
                    {
                        currentBlockPos = newBlock.numBlock;
                        currentBlock = newBlock;
                    }
                }

                int i;
                for (i = currentBlock.count - 1; i >= 0 && currentBlock.data[i].id > person.id; i--)
                {
                    currentBlock.data[i + 1] = currentBlock.data[i];
                }
                currentBlock.data[i + 1] = person;
                currentBlock.count++;

                writeBlock(currentBlock, currentBlockPos);
                return;
            }

            previousBlockPos = currentBlockPos;
            currentBlockPos = currentBlock.nextBlock;
        }

        // Insere no final do último bloco ou cria um novo caso o ultimo esteja cheio tambem
        Block lastBlock = readBlock(previousBlockPos);

        if (lastBlock.count < BLOCK_SIZE)
        {
            lastBlock.data[lastBlock.count] = person;
            lastBlock.count++;
            writeBlock(lastBlock, previousBlockPos);
        }
        else
        {
            Block newBlock;
            newBlock.numBlock = header.nextFreeBlock;
            newBlock.data[0] = person;
            newBlock.count = 1;
            newBlock.nextBlock = -1;

            lastBlock.nextBlock = newBlock.numBlock;

            header.blockCount++;
            header.nextFreeBlock++;

            writeBlock(lastBlock, previousBlockPos);
            writeBlock(newBlock, newBlock.numBlock);
            writeHeader(file, header);
        }
    }
}

// Remove um registro baseado no id
void SequenceSet::removePerson(const Person &pToRemove)
{
    {
        if (!file.is_open())
        {
            cerr << "Arquivo nao pode ser encontrado." << endl;
            throw runtime_error("Arquivo fechado.");
        }

        header = readHeader(file);

        if (header.firstBlock == -1)
        {
            cerr << "Arquivo vazio." << endl;
            return;
        }

        int currentBlockPos = header.firstBlock;
        int previousBlockPos = -1;
        bool found = false;

        while (currentBlockPos != -1 && !found)
        {
            Block currentBlock = readBlock(currentBlockPos);

            int indexToRemove = -1;
            int i = 0;

            while (i < currentBlock.count)
            {
                if (currentBlock.data[i].id == pToRemove.id)
                {
                    indexToRemove = i;
                    found = true;
                }
                i++;
            }

            if (indexToRemove != -1)
            {
                for (int i = indexToRemove; i < currentBlock.count - 1; i++)
                    currentBlock.data[i] = currentBlock.data[i + 1];

                currentBlock.count--;

                // Caso o bloco fique vazio ele sera indicado como o proximo a ser usado.
                if (currentBlock.count == 0)
                {
                    if (previousBlockPos == -1)
                    {
                        header.firstBlock = currentBlock.nextBlock;
                    }
                    else
                    {
                        // Atualiza o bloco anterior para pular o bloco vazio
                        Block previousBlock = readBlock(previousBlockPos);
                        previousBlock.nextBlock = currentBlock.nextBlock;
                        writeBlock(previousBlock, previousBlockPos);
                    }

                    // Marca o bloco como reutilizável
                    currentBlock.nextBlock = header.nextFreeBlock;
                    header.nextFreeBlock = currentBlockPos;

                    // Atualiza a contagem total de blocos
                    header.blockCount--;
                }
                else
                {
                    // Apenas atualiza o bloco com o registro removido
                    writeBlock(currentBlock, currentBlockPos);
                }

                // Atualiza o cabeçalho no arquivo
                writeHeader(file, header);
                readHeader(file);

                system("clear||cls");
                cout << "Registro removido com sucesso." << endl;
                return;
            }

            // Avança para o próximo bloco
            previousBlockPos = currentBlockPos;
            currentBlockPos = currentBlock.nextBlock;
        }

        system("clear||cls");
        cerr << "Registro nao encontrado." << endl;
    }
}

// Percorre registros em um arquivo CSV e os salva na estrutura de duas formas diferentes
// keepFile = true: os registros sao inseridos mantendo a estrutura anterior
// keepFile = false: os registros anteriores sao sobrescritos pelos registros do arquivo, substituindo a estrutura anterior.
void SequenceSet::insertFromCSV(const string &csvFileName, bool keepFile)
{
    if (!keepFile)
    {
        Header h;
        writeHeader(file, h);
        header = readHeader(file);
    }

    ifstream csvFile(csvFileName);
    if (!csvFile.is_open())
    {
        cerr << "Erro: Não foi possível abrir o arquivo CSV." << endl;
        return;
    }

    string line;
    getline(csvFile, line);

    while (getline(csvFile, line))
    {
        stringstream ss(line);

        int id;
        char personName[MAX_NAME_LENGTH] = {};
        char jobTitle[MAX_JOB_LENGTH] = {};
        double basePay, overtimePay, otherPay, benefits, totalPay, totalPayBenefits;
        int year;

        ss >> id;
        ss.ignore(1);
        ss.getline(personName, MAX_NAME_LENGTH, ',');
        ss.getline(jobTitle, MAX_JOB_LENGTH, ',');
        ss >> basePay;
        ss.ignore(1);
        ss >> overtimePay;
        ss.ignore(1);
        ss >> otherPay;
        ss.ignore(1);
        ss >> benefits;
        ss.ignore(1);
        ss >> totalPay;
        ss.ignore(1);
        ss >> totalPayBenefits;
        ss.ignore(1);
        ss >> year;

        Person p(id, personName, jobTitle, basePay, overtimePay, otherPay, benefits, totalPay, totalPayBenefits, year);
        insertPerson(p);
    }

    writeHeader(file, header);
    readHeader(file);

    csvFile.close();
}

// Escreve os registros da estrutura no formatos CSV padrao da base de dados.
/*
void SequenceSet::saveToCSV(const string &csvFileName)
{
    ofstream csvFile(csvFileName);
    if (!csvFile.is_open())
    {
        system("clear||cls");
        cerr << "Erro: Não foi possível criar o arquivo CSV." << endl;
        return;
    }

    csvFile << "Id,Employee Name,Job Title,Base Pay,Overtime Pay,Other Pay,Benefits,Total Pay,Total Pay & Benefits,Year\n";

    Person p;
    Block block;

    int i = 0;
    int iterator = header.firstBlock;

    while (i < header.blockCount)
    {
        file.seekg(sizeof(Header) + iterator * sizeof(Block), ios::beg);
        file.read(reinterpret_cast<char *>(&block), sizeof(Block));

        for (int j = 0; j < block.count; j++)
        {
            p = block.data[j];

            csvFile << p.id << ","
                    << p.personName << ","
                    << p.jobTitle << ","
                    << p.basePay << ","
                    << p.overtimePay << ","
                    << p.otherPay << ","
                    << p.benefits << ","
                    << p.totalPay << ","
                    << p.totalPayBenefits << ","
                    << p.year << "\n";
        }

        iterator = block.nextBlock;
        i++;
    }

    system("clear||cls");
    cout << "Arquivo salvo com sucesso!" << endl;
    csvFile.close();
}*/

// Metodo para facilitar a manutencao do codigo, imprime no console um registro formatado.
void SequenceSet::printPerson(const Person &p)
{
    cout << "ID: " << p.id
         << ", Nome: " << p.personName
         << ", Cargo: " << p.jobTitle
         << ", BasePay: " << p.basePay
         << ", Ano: " << p.year << endl;
}

// Imprime todos os registros console.
void SequenceSet::printFile(bool readRegisters)
{
    header = readHeader(file);

    if (header.blockCount == 0)
    {
        cout << "Nenhum registro encontrado!" << endl;
        return;
    }

    cout << "===== Cabeçalho =====" << endl;
    cout << "Quantidade de Blocos: " << header.blockCount << endl;
    cout << "Primeiro Bloco: " << header.firstBlock << endl;
    cout << "Próximo Bloco Livre: " << header.nextFreeBlock << endl;
    cout << "=====================" << endl;

    Block block;
    int i = 0;
    int iterator = header.firstBlock;

    while (i < header.blockCount)
    {
        file.seekg(sizeof(Header) + iterator * sizeof(Block), ios::beg);
        file.read(reinterpret_cast<char *>(&block), sizeof(Block));

        cout << "===== Bloco " << block.numBlock << " =====" << endl;
        cout << "Contagem de Registros: " << block.count << endl;
        cout << "Próximo Bloco: " << block.nextBlock << endl;

        if (readRegisters)
        {
            for (int j = 0; j < block.count; j++)
            {
                printPerson(block.data[j]);
            }
        }

        iterator = block.nextBlock;
        i++;
    }
}
