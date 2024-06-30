#include "include/board.h"

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

    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            PieceType type = PieceType::None;
            Colour colour = Colour::None;

            if(initBoard[i][j] != '.'){
                colour = std::isupper(initBoard[i][j]) ? Colour::White : Colour::Black;
                switch (std::tolower(initBoard[i][j])) {
                    case 'p':
                        type = PieceType::Pawn;
                        break;            
                    case 'r':
                        type = PieceType::Rook;
                        break;    
                    case 'n':
                        type = PieceType::Knight;
                        break;    
                    case 'b':
                        type = PieceType::Bishop;
                        break;    
                    case 'q':
                        type = PieceType::Queen;
                        break;    
                    case 'k':
                        type = PieceType::King;
                        break;   
                    default:
                        type = PieceType::None;
                        break;                        
                }
            }
            board[i][j] = Piece(type, colour);
        }
    }
}

void Board::printBoard(){
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++)
            std::cout << board[i][j].toChar() << " ";
        std::cout << std::endl;
    }
}