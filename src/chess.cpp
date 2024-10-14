#include "include/chess.h"

Chess::Chess(const string &fen)
    : board(fen), moves(board){}

void Chess::printBoard(){
    board.printBoard();
}

void Chess::move(string lan){
    Move move(lan, board);

    if(move.isMoveValid(board)){
        move.makeMove(board);
        moves.updateBoard(board);
    }
    else
        cout << "Invalid move: " << lan << endl;
}

string Chess::getSan(string &lan){
    Move move(lan, board);
    return move.lanToSan(moves);
}

bool Chess::isGameOver(){
    // cout << "Num pieces: " << board.getNumPieces() 
    //      << ", Half move: " << board.getHalfMoveClock() 
    //      << ", Full move counter:" << board.getFullMoveNumber() << endl;

    if(board.getNumPieces() == 2 || board.getHalfMoveClock() >= 50){
        result = "Draw";
        return true;
    }

    bool isTerminalState = moves.isTerminalState();
    if(isTerminalState){
        if(moves.isLoss())
            result = (board.isWhiteToPlay()) ? "Black" : "White";
        else
            result = "Draw";

        return true;
    }

    return isTerminalState;
}

void Chess::printGeneratedMoves(){
    if(moves.getMoves().size() == 0){
        cout << "No legal moves" << endl;
        return;
    }
    moves.printGeneratedMoves();
}