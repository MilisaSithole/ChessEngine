#ifndef CHESS_H
#define CHESS_H

#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

#include "board.h"
#include "move.h"
#include "moveGenerator.h"
#include "resNet.h"

using namespace std;

class Chess{
public:
    Chess(const string &fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    void printBoard();
    void move(string lan);
    string getFen(){return board.getFen();};
    string getPlayerTurn(){return (board.isWhiteToPlay()) ? "White" : "Black";};
    Board getBoard(){return board;};
    string getSan(string &lan);
    bool isGameOver();
    string getResult(){return result;};

    void printGeneratedMoves();

private:
    Board board;
    MoveGenerator moves;
    ResNet resNet;
    string result;
};

#endif