#include <stdio.h>
#include "include/chess.h"
// #include "include/board.h"
// #include "include/piece.h"
// #include "include/move.h"
#include <iostream>
#include <filesystem>
#include <unordered_map>

#include "include/agent.h"
#include "include/resNet.h"

using namespace std;
int main(){
    ResNet resNet;
    unordered_map<string, float> args{
        {"c", 2},
        {"numSearches", 250},
        {"numIterations", 3},
        {"numSelfPlayIterations", 500},
        {"numEpochs", 4},
        {"batchSize", 64},
        {"temperature", 1.25},
        {"dirichletEps", 0.25},
        {"dirichletAlpha", 0.3}
    };

    Agent agent(true, args, resNet);

    // Chess chess("rnb1kbr1/p2Q1p2/7p/q3p3/1pp1n3/P1PPKpPN/1P5P/RNB2B1R b - - 0 15");
    Chess chess("rnb1kbr1/p3Rp2/7p/q3p3/1pp1n3/P1PPKpPN/1P5P/RNB2B1R b - - 0 15");
    chess.printBoard();
    cout << "Turn: " << chess.getPlayerTurn() << endl;

    string lan;

    bool isGameOver = false;
    while(!isGameOver){
        cout << "[Enter] continue | [m] get moves | [q] quit: ";
        char inp = getchar();
        if(inp == 'q')
            return 0;
        if(inp == 'm'){
            chess.printGeneratedMoves();
            continue;
        }

        cout  << "Move: ";
        lan = agent.getMove(chess.getBoard());
        cout << lan << " | " << chess.getSan(lan) << endl;

        chess.move(lan);
        cout << chess.getFen() << endl;
        chess.printBoard();
        cout << "Turn: " << chess.getPlayerTurn() << endl;

        isGameOver = chess.isGameOver();
    }
    cout << "Game Over: ";
    if(chess.getResult() == "Draw")
        cout << "Draw" << endl;
    else
        cout << chess.getResult() << " wins" << endl;
        
    // bool playerTurn = chess.getPlayerTurn() == "White";
    // cout << "Turn: " << playerTurn << endl;
    // cout << "Move: ";
    // cin >> lan;

    // while(lan != "q"){
    //     if(lan == "m"){
    //         chess.printGeneratedMoves();
    //         cout << "Move: ";
    //         cin >> lan;
    //         continue;
    //     }

    //     lan = agent.getMove(chess.getBoard());

    //     chess.move(lan);
    //     cout << chess.getFen() << endl;
    //     chess.printBoard();

    //     cout << "Turn: " << chess.getPlayerTurn() << endl;
    //     cout << "Move: ";
    //     cin >> lan;
    // }

    return 0;
}