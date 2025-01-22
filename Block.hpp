#ifndef BLOCK_H
#define BLOCK_H

#include <fstream>
#include "Person.hpp"

using namespace std;

struct Block {
    int numBlock;
    int count;
    int nextBlock;
    Person data[50];

    Block() : count(0), nextBlock(-1) {};
};

#endif
