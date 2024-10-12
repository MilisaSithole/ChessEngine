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

void Chess::printGeneratedMoves(){
    MoveGenerator moves(board);
    if(moves.getMoves().size() == 0){
        cout << "Checkmate" << endl;
        return;
    }
    moves.printGeneratedMoves();
}