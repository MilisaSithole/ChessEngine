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
    Agent(bool isWhite, unordered_map<string, float> &args, ResNet &model, bool isRandom = false, int threadID = 0);
    string getMove(Board board);
    string getRandomMove(Board board);
    void updateStatesRecord(string fen, float captureReward);
    void saveGame(string result, int threadID = -1);
    void setIsWhite(bool isWhite){this->isWhite = isWhite;};

private:
    bool isWhite;
    bool isRandom;
    unordered_map<string, float> args;
    ResNet model;
    MCTS mcts;
    MoveMap moveMap;

    float captureBias = 0.2;
    float terminalBias = 0.8;
    vector<string> states;
    vector<float> values;
    vector<vector<float>> policies;
};

#endif