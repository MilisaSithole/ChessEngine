#include "include/chess.h"

Chess::Chess(const string &fen){
    cout << "Initial fen: " << fen << endl;
    // Initialize board
    board = Board(fen);
}

void Chess::printBoard(){
    board.printBoard();
}

void Chess::move(string uci){
    Move move(uci, board);

    if(move.isMoveValid()){
        move.makeMove(board);
        board.printBoard();
    } else {
        cout << "Invalid move" << endl;
    }

    cout << getPlayerTurn() << "'s turn" << endl;
}

string Chess::getFen(){
    return board.getFen();
}

string Chess::getPlayerTurn(){
    return (board.isWhiteToPlay()) ? "White" : "Black";
}

void Chess::printGeneratedMoves(){
    MoveGenerator moves(board);
}