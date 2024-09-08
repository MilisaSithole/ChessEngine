#include "include/move.h"

Move::Move(string lan, Board &board){
    string moveFromStr = lan.substr(0, 2);
    string moveToStr = lan.substr(2, 2);
    this->moveFrom = algebraicToIndex(moveFromStr);
    this->moveTo = algebraicToIndex(moveToStr);
    this->movedPiece = board.getPieceAt(moveFrom);
    this->capturedPiece = board.getPieceAt(moveTo);
    this->isWhitesTurn = board.isWhiteToPlay();
}

Move::Move(int moveFrom, int moveTo, Board &board){
    this->moveFrom = moveFrom;
    this->moveTo = moveTo;
    this->movedPiece = board.getPieceAt(moveFrom);
    this->capturedPiece = board.getPieceAt(moveTo);
    this->isWhitesTurn = board.isWhiteToPlay();
}

int Move::algebraicToIndex(string &square){
    return int(square[0] - 'a') + ((7 - int(square[1] - '1')) * 8);
}

string Move::indexToAlgebraic(int &idx){
    return string(1, 'a' + idx % 8) + to_string(8 - idx / 8);
}

bool Move::isMoveValid(){
    if(!movedPiece.isEmpty() && movedPiece.isWhite() == isWhitesTurn && 
       (capturedPiece.isEmpty() || capturedPiece.getColour() != movedPiece.getColour()))
        return true;
    return false;
}

void Move::makeMove(Board &board){
    board.makeMove(moveFrom, moveTo);
}

void Move::printMove(){
    std::cout << indexToAlgebraic(moveFrom) << " -> " << indexToAlgebraic(moveTo) << std::endl;
}

int Move::getFromSquare(){
    return moveFrom;
}

int Move::getToSquare(){
    return moveTo;
}

Piece Move::getMovedPiece(){
    return movedPiece;
}

bool Move::isPawnMovedOrCaptured(){
    return (movedPiece.getType() == PieceType::Pawn) || (capturedPiece.getType() == PieceType::Pawn);
}