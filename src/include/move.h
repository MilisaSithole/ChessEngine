#ifndef MOVE_H
#define MOVE_H

#include <iostream>
#include <cstdint>
#include <string>

#include "piece.h"
#include "board.h"

using namespace std;

struct Square{
    uint8_t rank;
    uint8_t file;

    Square() : rank(0), file(0) {}
    Square(uint8_t rank, uint8_t file) : rank(rank), file(file) {}
};

class Move{
public:
    Move(string lan, Board &board);
    Move(int moveFrom, int moveTo, Board &board);
    int algebraicToIndex(string &square);
    string indexToAlgebraic(int &idx);
    bool isMoveValid();
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