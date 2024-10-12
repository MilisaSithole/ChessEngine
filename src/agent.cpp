#include "include/agent.h"

Agent::Agent(bool isWhite, unordered_map<string, float> &args, ResNet &model)
    : isWhite(isWhite), args(args), model(model), mcts(args, model){}

string Agent::getMove(Board board){
    unordered_map<string, float> policy = mcts.search(board.getFen());
    return getBestMove(policy);
}

string Agent::getBestMove(unordered_map<string, float> &policy){
    auto maxElement = max_element(
        policy.begin(), policy.end(),
        [](const auto &a, const auto &b){
            return a.second < b.second;
        }
    );

    return maxElement->first;
}