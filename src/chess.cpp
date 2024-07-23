#include "include/chess.h"

Chess::Chess(){
    Board board;
    isWhitesTurn = true;
}

void Chess::move(std::string lan){
    Move move(lan, board, isWhitesTurn);

    if(move.isMoveValid()){
        move.makeMove(board);
        board.printBoard();
        isWhitesTurn = !isWhitesTurn;
    } else {
        std::cout << "Invalid move" << std::endl;
    }

    std::cout << getPlayerTurn() << "'s turn" << std::endl;
}

std::string Chess::getPlayerTurn(){
    return (isWhitesTurn) ? "White" : "Black";
}