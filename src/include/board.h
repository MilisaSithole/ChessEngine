#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include "piece.h"

using namespace std;

class Board{
public:
    Board();
    void initBoard();
    void printBoard();
    Piece getPieceAt(int r, int c);
    void makeMove(int fromR, int fromC, int toR, int toC);

private:
    Piece board[8][8];
};

#endif