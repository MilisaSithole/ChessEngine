#ifndef MOVE_H
#define MOVE_H

#include <iostream>
#include "piece.h"
#include <string>

class Move{
public:
    Move(std::string moveFrom, std::string moveTo, Piece movedPiece);

private:
    std::string moveFrom;
    std::string moveTo;
    Piece movedPiece;
};

#endif