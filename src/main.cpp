#include <stdio.h>
#include <iostream>
#include <filesystem>
#include <unordered_map>
#include <chrono>
#include <string>
#include <unistd.h>

#include "include/chess.h"
#include "include/agent.h"
#include "include/resNet.h"

using namespace std;
int main(){
    ResNet resNet;
    unordered_map<string, float> args{
        {"c", 2},
        {"numSearches", 250},
        {"numIterations", 3},
        {"numSelfPlayIterations", 20},
        {"numEpochs", 4},
        {"batchSize", 64},
        {"temperature", 1.25},
        {"dirichletEps", 0.25},
        {"dirichletAlpha", 0.3}
    };

    Agent agent(true, args, resNet);

    for(int i = 0; i < static_cast<int>(args["numSelfPlayIterations"]); i++){
        cout << "Self play iteration: " << i+1 << " of " << args["numSelfPlayIterations"] << endl;

        auto start = chrono::high_resolution_clock::now();

        // Chess chess("1Qb5/1R4r1/3pp1kp/2pP2B1/4B1PP/2P4q/5P2/N3K2R b - - 2 51");
        Chess chess;
        bool isGameOver = false;
        string prevFen = "-";
        string lan;
        int numMoves = 0;
        while(!isGameOver){

            if(prevFen == chess.getFen()){
                lan = agent.getRandomMove(chess.getBoard());
                cout << "Used invalid move. Random move: " << lan << endl;
            }
            else{
                lan = agent.getMove(chess.getBoard());
                cout << "Moves played: " << ++numMoves << "\r" << flush;
            }

            prevFen = chess.getFen();
            chess.move(lan);
            agent.updateStatesRecord(chess.getFen());
            isGameOver = chess.isGameOver();
        }

        cout << endl; // For moves played print
        auto end = chrono::high_resolution_clock::now();
        cout << "Time taken: " << chrono::duration_cast<std::chrono::seconds>(end - start).count() << " seconds" << endl;
        cout << "Result: " << chess.getResult() << endl;
        agent.saveGame(chess.getResult());
    }

    return 0;

    // Chess chess;
    // chess.printBoard();
    // cout << "Turn: " << chess.getPlayerTurn() << endl;

    // string lan;

    // bool isGameOver = false;
    // while(!isGameOver){
    //     // cout << "[Enter] continue | [m] get moves | [q] quit: ";
    //     // char inp = getchar();
    //     // if(inp == 'q')
    //     //     return 0;
    //     // if(inp == 'm'){
    //     //     chess.printGeneratedMoves();
    //     //     continue;
    //     // }

    //     cout  << "Move: ";
    //     lan = agent.getMove(chess.getBoard());
    //     cout << lan << " | " << chess.getSan(lan) << endl;

    //     chess.move(lan);
    //     agent.updateStatesRecord(chess.getFen());
    //     cout << chess.getFen() << endl;
    //     chess.printBoard();
    //     cout << "Turn: " << chess.getPlayerTurn() << endl;

    //     isGameOver = chess.isGameOver();
    // }
    // cout << "Game Over: ";
    // string result = chess.getResult();
    // if(result == "Draw")
    //     cout << "Draw" << endl;
    // else
    //     cout << result << " wins" << endl;
    // agent.saveGame(result);

    return 0;
}