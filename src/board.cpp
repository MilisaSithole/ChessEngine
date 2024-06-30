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
            std::cout << board[i][j].symbol() << " ";
        std::cout << std::endl;
    }
}