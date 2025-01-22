#ifndef SEQUENCESET_H
#define SEQUENCESET_H

#include "Header.hpp" 
#include "Person.hpp"
#include "Block.hpp"

using namespace std;

class SequenceSet {
private:
    fstream file;
    Header header;

    Block readBlock(int position);
    void writeBlock(const Block &block, int position);
    void writeHeader(fstream &file, const Header &header);
    Header readHeader(fstream &file);

public:
    SequenceSet(const string &fileName);
    ~SequenceSet();
    Person searchPersonById(int id);
    void insertPerson(const Person &person);
    void removePerson(const Person &pToRemove);
    void insertFromCSV(const string &csvFileName, bool keepFile);
    void saveToCSV(const string &csvFileName);
    void printPerson(const Person &p);
    void printFile(bool readRegisters);
};

#endif
