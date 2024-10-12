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
    Move(string lan, Board &board);
    Move(int moveFrom, int moveTo, Board &board, string promotion = "");
    int algebraicToIndex(string &square);
    string indexToAlgebraic(int &idx);
    bool isMoveValid(Board &board);
    void makeMove(Board &board);
    void printMove();
    int getFromSquare();
    int getToSquare();
    Piece getMovedPiece();
    bool isPawnMovedOrCaptured();
    string getLan();

private:
    int moveFrom;
    int moveTo;
    Piece movedPiece;
    Piece capturedPiece;
    bool isWhitesTurn;
    string promotion;
};

#endif