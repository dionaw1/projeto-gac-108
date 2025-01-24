#include "App.hpp"
#include "Person.cpp"
#include "SequenceSet.cpp"
#include <iostream>

// Interface basica pra funcionamento do sistema.
void printMenu()
{
    cout << "=========== Menu ===========" << endl
         << "[1] Procurar registro (ID)." << endl
         << "[2] Inserir novo registro no terminal." << endl
         << "[3] Importar dados de um arquivo CSV." << endl
         << "[4] Exibir todos os blocos." << endl
         << "[5] Exibir todos os registros." << endl
         << "[6] Remover registro." << endl
         << "[0] Finalizar o programa." << endl;
}

// Utilidade para lidar com entradas indesejadas.
void invalidInput()
{
    throw runtime_error("Erro: entrada invalida.");
}

// Utilidade para receber um registro pelo terminal do usuario.
Person loadFromTerminal()
{
    Person p;
    cout << "Digite o ID: ";
    cin >> p.id;
    cin.ignore();
    cout << "Digite o Nome: ";
    cin.getline(p.personName, MAX_NAME_LENGTH);
    cout << "Digite o Cargo: ";
    cin.getline(p.jobTitle, MAX_JOB_LENGTH);
    cout << "Digite o BasePay: ";
    cin >> p.basePay;
    cout << "Digite o OvertimePay: ";
    cin >> p.overtimePay;
    cout << "Digite o OtherPay: ";
    cin >> p.otherPay;
    cout << "Digite os Benefícios: ";
    cin >> p.benefits;
    cout << "Digite o TotalPay: ";
    cin >> p.totalPay;
    cout << "Digite o TotalPayBenefits: ";
    cin >> p.totalPayBenefits;
    cout << "Digite o Ano: ";
    cin >> p.year;
    return p;
}

// Procedimento principal para interacao sistema-usuario.
void app()
{
    string binFile = "data.bin";
    string csvFileName;
    Person tp;
    int searchId;

    SequenceSet set(binFile);
    int option = -1;

    while (option != 0)
    {
        printMenu();
        cin >> option;
        if (cin.fail())
            invalidInput();

        switch (option)
        {
        case 1:

            cout << "Informe o ID para busca: ";
            cin >> searchId;
            if (cin.fail())
                invalidInput();

            tp = set.searchPersonById(searchId);
            if (!(tp.id == -1)){
                system("clear||cls");
                set.printPerson(tp);
            }
            else{
                system("clear||cls");
                cerr << "Nenhum registro com ID: " << searchId << " pode ser encontrado. " << endl;
            }
            break;

        case 2:
            set.insertPerson(loadFromTerminal());
            break;

        case 3:
            system("clear||cls");
            cout << "Informe o nome do arquivo .CSV: ";
            cin >> csvFileName;
            csvFileName = csvFileName + ".csv";
            set.insertFromCSV(csvFileName, false);
            break;

        case 4:
			      system("clear||cls");
            set.printFile(false);
            break;

        case 5:
			system("clear||cls");
            set.printFile(true);
            break;

        case 6:
            cout << "Informe o ID para remocao: ";
            cin >> searchId;
            if (cin.fail())
                invalidInput();

            set.removePerson(set.searchPersonById(searchId));
            break;

        case 0:
            break;

        default:
            cout << "Entrada nao reconhecida" << endl;
            break;
        }
    }
    cout << "Programa finalizado!";
}

int main()
{
    app();

    return 0;
}
