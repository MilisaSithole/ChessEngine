#include "include/board.h"
#include <vector>

Board::Board(){
    initBoard();
}

void Board::initBoard(){
    vector<vector<char>> initBoard = {
        {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
        {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
        {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}
    };

    for(int i = 0; i < 8; i++)
        for(int j = 0; j < 8; j++)
            board[i][j] = Piece(initBoard[i][j]);
}

void Board::printBoard(){
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++)
            cout << board[i][j].symbol() << " ";
        cout << endl;
    }
}

Piece Board::getPieceAt(int i, int j){
    return board[i][j];
}

void Board::makeMove(int fromR, int fromC, int toR, int toC){
    board[toR][toC] = board[fromR][fromC];
    board[fromR][fromC] = Piece();
}