#include "include/chess.h"

Chess::Chess(const string &fen){
    board = Board(fen);
}

void Chess::printBoard(){
    board.printBoard();
}

void Chess::move(string uci){
    Move move(uci, board);

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

void Chess::loadMoveMaps(){
    ifstream movesFile("../src/files/moves.csv");

    if(!movesFile.is_open()){
        cout << "Error opening moves file" << endl;
        return;
    }
    else{
        cout << "Moves file loaded" << endl;
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