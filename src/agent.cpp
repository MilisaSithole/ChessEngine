#include "include/agent.h"

Agent::Agent(bool isWhite, unordered_map<string, float> &args, ResNet &model)
    : isWhite(isWhite), args(args), model(model), mcts(args, model){}

string Agent::getMove(Board board){
    vector<float> policy = mcts.search(board.getFen());
    policies.push_back(policy);

    auto maxIdx = max_element(policy.begin(), policy.end());
    int idx = distance(policy.begin(), maxIdx);

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

void Agent::updateStatesRecord(string fen){
    states.push_back(fen);
}

void Agent::saveGame(string result){
    // Init values vector
    int numMoves = states.size();
    vector<float> values(numMoves, 0);

    int value = 0;
    if(result == "White")
        value = 1;
    if(result == "Black")
        value = -1;
    
    if(value != 0)
        for(int i = 0; i < numMoves; i++){
            values[i] = value;
            value *= -1;
        }

    string fileName = "../src/games/games.csv";
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
    cout << "Game saved \n" << endl;

    // Clear memory
    states.clear();
    policies.clear();
}