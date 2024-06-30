#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <vector>
#include "piece.h"

using namespace std;

class Board{
public:
    Board();
    void initBoard();
    void printBoard();

private:
    Piece board[8][8];
};

#endif