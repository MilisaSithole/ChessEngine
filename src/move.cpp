#include "include/move.h"

Move::Move(string uci, Board &board){
    string moveFromStr = uci.substr(0, 2);
    string moveToStr = uci.substr(2, 2);
    if(uci.length() > 4)
        this->promotion = uci.substr(4, 1);
    this->moveFrom = algebraicToIndex(moveFromStr);
    this->moveTo = algebraicToIndex(moveToStr);
    this->movedPiece = board.getPieceAt(moveFrom);
    this->capturedPiece = board.getPieceAt(moveTo);
    this->isWhitesTurn = board.isWhiteToPlay();
}

Move::Move(int moveFrom, int moveTo, Board &board, string promotion){
    this->moveFrom = moveFrom;
    this->moveTo = moveTo;
    this->movedPiece = board.getPieceAt(moveFrom);
    this->capturedPiece = board.getPieceAt(moveTo);
    this->isWhitesTurn = board.isWhiteToPlay();
    this->promotion = promotion;
}

int Move::algebraicToIndex(string &square){
    return int(square[0] - 'a') + ((7 - int(square[1] - '1')) * 8);
}

string Move::indexToAlgebraic(int &idx){
    return string(1, 'a' + idx % 8) + to_string(8 - idx / 8);
}

bool Move::isMoveValid(Board &board){
    MoveGenerator moveGenerator(board);
    // vector<Move> moves = moveGenerator.getMoves();

    for(string move : moveGenerator.getMoves()){
        if(getUci() == move)
            return true;
    }
    return false;
}

void Move::makeMove(Board &board){
    board.makeMove(moveFrom, moveTo, promotion);
}

void Move::printMove(){
    cout << indexToAlgebraic(moveFrom) << " -> " << indexToAlgebraic(moveTo) << promotion << endl;
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

string Move::getUci(){
    return indexToAlgebraic(moveFrom) + indexToAlgebraic(moveTo) + promotion;
}