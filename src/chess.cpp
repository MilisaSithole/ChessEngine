#include "include/chess.h"

Chess::Chess(unordered_map<string, float> &args, const string &fen)
    : board(fen), moves(board), args(args){}

bool Chess::isValidMove(string lan){
    Move move(lan, board);

    return move.isMoveValid(board);
}

void Chess::move(string lan){
    Move move(lan, board);

    if(move.isMoveValid(board)){
        move.makeMove(board);
        moves.updateBoard(board);
        moveHistory[board.getFenNoClock()]++;

        if(moveHistory[board.getFenNoClock()] > 2)
            result = "Draw";
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
    // if(result != ""){
    //     cout << "Threefold repetition" << endl;
    //     return true;
    // }

    if(board.getNumPieces() == 2 || 
       board.getFenCount(board.getFenNoClock()) > 2 ||
       board.getHalfMoveClock() >= static_cast<int>(args["_50MoveRule"]) ||
       board.getFullMoveNumber() >= static_cast<int>(args["maxMoves"]) / 2){
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