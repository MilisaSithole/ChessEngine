#ifndef MOVE_H
#define MOVE_H

#include <iostream>
#include <cstdint>
#include "piece.h"
#include "board.h"
#include <string>

struct Square{
    uint8_t rank;
    uint8_t file;

    Square() : rank(0), file(0) {}
    Square(uint8_t rank, uint8_t file) : rank(rank), file(file) {}
};

class Move{
public:
    Move(std::string lan, Board &board, bool isWhitesTurn);
    Square algebraicToIndecies(std::string &square);
    bool isMoveValid();
    void makeMove(Board &board);
    void printMove();
    int getFromRank();
    int getFromFile();
    int getToRank();
    int getToFile();
    Piece getMovedPiece();

private:
    Square moveFrom;
    Square moveTo;
    Piece movedPiece;
    Piece capturedPiece;
    bool isWhitesTurn;
};

#endif