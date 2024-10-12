#ifndef MOVE_MAPPING_H
#define MOVE_MAPPING_H

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <array>

using namespace std;

const int TOTAL_MOVES = 1968;

class MoveMap{
public:
    MoveMap();
    string getMove(int idx) {return idxToMove[idx];};
    int getIdx(string move) {return moveToIdx[move];};

private:
    string idxToMove[TOTAL_MOVES];
    unordered_map<string, int> moveToIdx;
};

#endif