#ifndef CHESS_H
#define CHESS_H

#include <vector>
#include <sstream>

#include "board.h"
#include "move.h"

using namespace std;

class Chess{
public:
    Chess(const string &fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    void printBoard();
    void move(string lan);
    string getFen();
    string getPlayerTurn();

private:
    Board board;
};

#endif