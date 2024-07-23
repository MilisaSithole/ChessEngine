#include "include/move.h"

Move::Move(std::string lan, Board &board, bool isWhitesTurn){
    std::string moveFromStr = lan.substr(0, 2);
    std::string moveToStr = lan.substr(2, 2);
    this->moveFrom = algebraicToIndecies(moveFromStr);
    this->moveTo = algebraicToIndecies(moveToStr);
    this->movedPiece = board.getPieceAt(moveFrom.rank, moveFrom.file);
    this->capturedPiece = board.getPieceAt(moveTo.rank, moveTo.file);
    this->isWhitesTurn = isWhitesTurn;
}

Square Move::algebraicToIndecies(std::string &square){
    Square s = Square(7 - uint8_t(square[1] - '1'), uint8_t(square[0] - 'a'));
    return s;
}

bool Move::isMoveValid(){
    if(!movedPiece.isEmpty() && movedPiece.isWhite() == isWhitesTurn && (capturedPiece.isEmpty() || capturedPiece.getColour() != movedPiece.getColour()))
        return true;
    return false;
}

void Move::makeMove(Board &board){
    board.makeMove(moveFrom.rank, moveFrom.file, moveTo.rank, moveTo.file);
}

void Move::printMove(){
    std::cout << "Moved from " << char('a' + moveFrom.file) << int(8 - moveFrom.rank) << " to " << char('a' + moveTo.file) << int(8 - moveTo.rank) << std::endl;
}

int Move::getFromRank(){
    return int(moveFrom.rank);
}

int Move::getFromFile(){
    return int(moveFrom.file);
}

int Move::getToRank(){
    return int(moveTo.rank);
}

int Move::getToFile(){
    return int(moveTo.file);
}

Piece Move::getMovedPiece(){
    return movedPiece;
}