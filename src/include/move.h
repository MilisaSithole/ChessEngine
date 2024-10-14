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

class MoveGenerator;

class Move{
public:
    Move(string lan, Board &board);
    Move(int moveFrom, int moveTo, Board &board, string promotion = "");
    int algebraicToIndex(string &square);
    string indexToAlgebraic(int &idx);
    bool isMoveValid(Board &board);
    void makeMove(Board &board);
    bool isCastle();
    void printMove();
    int getFromSquare(){return moveFrom;};
    int getToSquare(){return moveTo;};
    Piece getMovedPiece(){return movedPiece;};
    bool isPawnMovedOrCaptured();
    string getLan();

    string lanToSan(MoveGenerator &moves);

private:
    int moveFrom;
    int moveTo;
    Piece movedPiece;
    Piece capturedPiece;
    bool isWhitesTurn;
    string promotion;

private:
    bool isCapture(){return capturedPiece.getType() != PieceType::None;};
};

#endif