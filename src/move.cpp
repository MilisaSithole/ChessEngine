#include "include/move.h"

Move::Move(string lan, Board &board){
    string moveFromStr = lan.substr(0, 2);
    string moveToStr = lan.substr(2, 2);
    if(lan.length() > 4)
        this->promotion = lan.substr(4, 1);
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
        if(getLan() == move)
            return true;
    }
    return false;
}

void Move::makeMove(Board &board){
    board.makeMove(moveFrom, moveTo, promotion);
}

bool Move::isCastle(){
    return (movedPiece.getType() == PieceType::King) && (abs(moveFrom - moveTo) == 2);
}

void Move::printMove(){
    cout << indexToAlgebraic(moveFrom) << " -> " << indexToAlgebraic(moveTo) << promotion << endl;
}

bool Move::isPawnMovedOrCaptured(){
    return (movedPiece.getType() == PieceType::Pawn) || (capturedPiece.getType() == PieceType::Pawn);
}

string Move::getLan(){
    return indexToAlgebraic(moveFrom) + indexToAlgebraic(moveTo) + promotion;
}



string Move::lanToSan(MoveGenerator &moves){
    // If move is a castle
    if(isCastle()){
        if(moveTo > moveFrom)
            return "O-O";
        else
            return "O-O-O";
    }

    string san = "";

    // if move is pawn capture
    if(movedPiece.getType() == PieceType::Pawn){
        if(isCapture())
            san += indexToAlgebraic(moveFrom)[0];
    }
    else{
        // TODO: ambiguity checks
        // If not a pawn move
        san += movedPiece.symbol();
    }

    // Check for capture
    if(isCapture())
        san += "x";

    // Add to square
    san += indexToAlgebraic(moveTo);

    // Check for promotion
    if(promotion != "")
        san += "=" + promotion;

    // Check for check
    string lan = getLan();
    moves.doMove(lan);
    if(moves.isCheck())
        san += (moves.isLoss()) ? "+" : "#";
    moves.undoMoves();

    return san;
}