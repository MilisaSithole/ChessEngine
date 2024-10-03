#include "include/chess.h"

Chess::Chess(const string &fen){
    cout << "Initial fen: " << fen << endl;
    // Initialize board
    board = Board(fen);

    // Load move maps
    loadMoveMaps();
}

void Chess::printBoard(){
    board.printBoard();
}

void Chess::move(string uci){
    Move move(uci, board);

    if(move.isMoveValid(board)){
        move.makeMove(board);
        board.printBoard();
    } else {
        cout << "Invalid move" << endl;
    }
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

void Chess::loadMoveMaps(){
    ifstream movesFile("moves.csv");

    if(!movesFile.is_open()){
        cout << "Error opening moves file" << endl;
        return;
    }

    string line;

    int idx;
    string move;
    while(getline(movesFile, line)){
        size_t commaPos = line.find(',');
        idx = stoi(line.substr(0, commaPos));
        move = line.substr(commaPos + 2);
        
        idxToMove[idx] = move;
        moveToIdx[move] = idx;
    }
}