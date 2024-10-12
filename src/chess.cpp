#include "include/chess.h"

Chess::Chess(const string &fen){
    board = Board(fen);
}

void Chess::printBoard(){
    board.printBoard();
}

void Chess::move(string lan){
    Move move(lan, board);

    if(move.isMoveValid(board))
        move.makeMove(board);
    else
        cout << "Invalid move: " << endl;
}

string Chess::getFen(){
    return board.getFen();
}

string Chess::getPlayerTurn(){
    return (board.isWhiteToPlay()) ? "White" : "Black";
}

bool Chess::isGameOver(){
    cout << "Num pieces: " << board.getNumPieces() 
         << ", Half move: " << board.getHalfMoveClock() 
         << ", Full move counter:" << board.getFullMoveNumber() << endl;
    if(board.getNumPieces() == 2 || board.getHalfMoveClock() >= 50){
        result = "Draw";
        return true;
    }

    MoveGenerator moves(board);
    bool isTerminalState = moves.isTerminalState();
    if(isTerminalState){
        if(moves.isLoss())
            result = (board.isWhiteToPlay()) ? "Black" : "White";
        else
            result = "Draw";
    }

    return isTerminalState;
}

void Chess::printGeneratedMoves(){
    MoveGenerator moves(board);
    if(moves.getMoves().size() == 0){
        cout << "Checkmate" << endl;
        return;
    }
    moves.printGeneratedMoves();
}