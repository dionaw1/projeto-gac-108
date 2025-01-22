#ifndef HEADER_H
#define HEADER_H

#include <fstream>

using namespace std;

struct Header {
    int blockCount;
    int firstBlock;
    int nextFreeBlock;

    Header() : blockCount(0), firstBlock(-1), nextFreeBlock(0) {}
};

#endif
