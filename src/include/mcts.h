#ifndef MCTS_H
#define MCTS_H

#include <vector>
#include <cmath>
#include <limits>
#include <memory>
#include <torch/torch.h>
#include <random>
#include <numeric>
#include <unordered_map>

#include "board.h"
#include "moveMapping.h"
#include "resNet.h"
#include "moveGenerator.h"

using namespace std;
extern MoveMap moveMap;

class Node{
public:
    Node(string state, float c, Node *parent = nullptr, 
         string moveTaken = "", float prior = 0, int visits = 0);
    bool isFullyExpanded();
    Node* selectChild();
    void expand(vector<float> &policy);
    void backpropagate(float value);
    float getPUCT(Node *child);

public:
    string state;
    float c;
    Node *parent;
    string moveTaken;
    float prior;
    int visits;
    float value;

    vector<shared_ptr<Node>> children;
};

class MCTS{
public:
    MCTS(unordered_map<string, float> &args, ResNet &model);
    vector<float> search(string state);
    vector<float> getStateVec(string state);

private:
    torch::Tensor getStateTensor(string state);
    torch::Tensor BBToTensor(uint64_t bitboard);
    torch::Tensor intToTensor(int value);
    torch::Tensor floatToTensor(float value, float max);

    vector<float> generateDirichletNoise(int numMoves);
    torch::Tensor getLegalMovesTensor(Board board);

private:
    unordered_map<string, float> args;
    ResNet model;
};

#endif