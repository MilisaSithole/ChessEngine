#include "include/agent.h"

Agent::Agent(bool isWhite, unordered_map<string, float> &args, ResNet &model, bool isRandom, int threadID)
    : isWhite(isWhite), args(args), model(model), mcts(args, model, threadID), isRandom(isRandom){}

string Agent::getMove(Board board){
    if(isRandom){
        return getRandomMove(board);
    }

    vector<float> policy = mcts.search(board.getFen());
    policies.push_back(policy);

    float sum = 0;
    for(auto &p: policy){
        p = pow(p, 1.0 / args["temperature"]);
        sum += p;
    }
    for(auto &p: policy)
        p /= sum;

    auto maxIdx = max_element(policy.begin(), policy.end());
    int idx;

    // Stochastic move selection
    if(args["temperature"] > 1.0){
        auto seed = chrono::high_resolution_clock::now().time_since_epoch().count(); // + (threadID * 100);
        mt19937 gen(seed);
        discrete_distribution<> dist(policy.begin(), policy.end());
        idx = dist(gen);
    }
    else
        idx = distance(policy.begin(), maxIdx);

    return moveMap.getMove(idx);
}

string Agent::getRandomMove(Board board){
    MoveGenerator moves(board);
    vector<string> movesVec = moves.getMoves();

    if(movesVec.size() == 1)
        return movesVec[0];

    int idx = rand() % movesVec.size();
    return movesVec[idx];
}

void Agent::updateStatesRecord(string fen, float captureReward){
    states.push_back(fen);
    values.push_back(captureReward * captureBias);
}

void Agent::saveGame(string result, int threadID){
    // Init values vector
    int numMoves = states.size();

    int value = 0;
    if(result == "White")
        value = 1;
    if(result == "Black")
        value = -1;

    if(!isWhite)
        value *= -1;
    
    if(value != 0)
        for(int i = 0; i < numMoves; i++){
            values[i] += value * terminalBias;
            value *= -1;
        }

    string fileName = (threadID == -1) ? 
                      "../src/games/games_" + to_string(model.getVersion()) + ".csv" : 
                      "../src/games/games_" + to_string(model.getVersion()) + to_string(threadID) + ".csv";
    ofstream outFile(fileName, ios::app);

    // Check file open
    if(!outFile.is_open()){
        cout << "Error: Could not open file " << fileName << endl;
        return;
    }

    // Write to file
    for(int i = 0; i < numMoves; i++){
        // Write encoded state
        vector<float> stateVec = mcts.getStateVec(states[i]);

        for(size_t j = 0; j < stateVec.size(); j++){
            outFile << stateVec[j];
            if(j != stateVec.size() - 1)
                outFile << ",";
        }
        outFile << "\n";

        // Write policy
        for(size_t j = 0; j < policies[i].size(); j++){
            outFile << policies[i][j];
            if(j != policies[i].size() - 1)
                outFile << ",";
        }
        outFile << "\n";

        // Write value
        outFile << values[i] << "\n";
    }

    outFile.close();
    
    // Clear memory
    states.clear();
    policies.clear();
}