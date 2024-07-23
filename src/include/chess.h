#ifndef CHESS_H
#define CHESS_H

#include "board.h"
#include "move.h"

class Chess{
public:
    Chess();
    void move(std::string lan);
    std::string getPlayerTurn();

private:
    Board board;
    bool isWhitesTurn;
};

#endif