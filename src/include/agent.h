#ifndef _AGENT_H_
#define _AGENT_H_

#include <unordered_map>
#include <algorithm>


#include "resNet.h"
#include "mcts.h"
#include "board.h"

using namespace std;

class Agent{
public:
    // Agent(bool isWhite, unordered_map<string, float> &args);
    Agent(bool isWhite, unordered_map<string, float> &args, ResNet &model);
    string getMove(Board board);

private:
    bool isWhite;
    unordered_map<string, float> args;
    ResNet model;
    MCTS mcts;

private:
    string getBestMove(unordered_map<string, float> &policy);
};

#endif