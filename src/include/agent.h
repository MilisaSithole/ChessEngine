#ifndef _AGENT_H_
#define _AGENT_H_

#include <unordered_map>
#include <algorithm>
#include <vector>
#include <fstream>
#include <iostream>

#include "moveMapping.h"
#include "resNet.h"
#include "mcts.h"
#include "board.h"

using namespace std;

class Agent{
public:
    // Agent(bool isWhite, unordered_map<string, float> &args);
    Agent(bool isWhite, unordered_map<string, float> &args, ResNet &model);
    string getMove(Board board);
    string getRandomMove(Board board);
    void updateStatesRecord(string fen);
    void saveGame(string result);

private:
    bool isWhite;
    unordered_map<string, float> args;
    ResNet model;
    MCTS mcts;
    MoveMap moveMap;

    vector<string> states;
    vector<vector<float>> policies;
};

#endif