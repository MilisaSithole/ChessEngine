#ifndef MOVE_H
#define MOVE_H

#include <iostream>
#include <cstdint>
#include <string>
#include <vector>

#include "piece.h"
#include "board.h"
#include "moveGenerator.h"

using namespace std;

class Move{
public:
    Move(string uci, Board &board);
    Move(int moveFrom, int moveTo, Board &board);
    int algebraicToIndex(string &square);
    string indexToAlgebraic(int &idx);
    bool isMoveValid(Board &board);
    void makeMove(Board &board);
    void printMove();
    int getFromSquare();
    int getToSquare();
    Piece getMovedPiece();
    bool isPawnMovedOrCaptured();
    string getUci();

private:
    int moveFrom;
    int moveTo;
    Piece movedPiece;
    Piece capturedPiece;
    bool isWhitesTurn;
};

#endif