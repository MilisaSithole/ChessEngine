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
        {"numSearches", 1000},
        {"numIterations", 3},
        {"numSelfPlayIterations", 500},
        {"numEpochs", 4},
        {"batchSize", 64},
        {"temperature", 1.25},
        {"dirichletEps", 0.25},
        {"dirichletAlpha", 0.3}
    };
    bool agentIsWhite = true;
    Agent agent(agentIsWhite, args, resNet);

    // Chess chess("r7/3P4/8/2k5/4p3/8/8/R3K2R w KQ - 0 1");
    Chess chess;
    chess.printBoard();

    string uci;
    bool playerTurn = chess.getPlayerTurn() == "White";
    cout << "Turn: " << playerTurn << endl;
    cout << "Move: ";
    cin >> uci;

    while(uci != "q"){
        if(playerTurn)
            uci = agent.getMove(chess.getBoard());

        if(uci == "m"){
            chess.printGeneratedMoves();
            cout << "Move: ";
            cin >> uci;
            continue;
        }

        chess.move(uci);
        cout << chess.getFen() << endl;
        chess.printBoard();

        cout << "Turn: " << chess.getPlayerTurn() << endl;
        cout << "Move: ";
        cin >> uci;
    }

    return 0;
}