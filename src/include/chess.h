#ifndef CHESS_H
#define CHESS_H

#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

#include "board.h"
#include "move.h"
#include "moveGenerator.h"
#include "resNet.h"

using namespace std;

class Chess{
public:
    Chess(const string &fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    void printBoard();
    void move(string uci);
    string getFen();
    string getPlayerTurn();

    void printGeneratedMoves();
    void loadMoveMaps();

private:
    Board board;
    unordered_map<int, string> idxToMove;
    unordered_map<string, int> moveToIdx;
    ResNet resNet;
};

#endif