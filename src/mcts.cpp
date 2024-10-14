#include "include/mcts.h"

MoveMap moveMap;

Node::Node(string state, float c, Node *parent, string moveTaken, float prior, int visits){
    this->state = state;
    this->c = c;
    this->parent = parent;
    this->moveTaken = moveTaken;
    this->prior = prior;
    this->visits = visits;
    this->value = 0;
}

bool Node::isFullyExpanded(){
    return !children.empty();
}

Node* Node::selectChild(){
    Node* bestChild = nullptr;
    float bestPUCT = -numeric_limits<float>::infinity();

    for(auto& child: children){
        float puct = getPUCT(child.get());
        if(puct > bestPUCT){
            bestChild = child.get();
            bestPUCT = puct;
        }
    }

    return bestChild;
}

float Node::getPUCT(Node *child){
    float Q = (child->visits == 0) ? 0 : 1 - ((child->value / child->visits) + 1) / 2;
    return Q + c * sqrt(visits / (child->visits + 1)) * child->prior;
}

void Node::expand(vector<float> &policy){
    for(size_t a = 0; a < policy.size(); ++a){
        if(policy[a] > 0){
            Board currBoard(state);
            string action = moveMap.getMove(a);

            currBoard.makeMove(action);
            string childState = currBoard.getFen();

            children.push_back(make_shared<Node>(childState, c, this, action, policy[a]));
        }
    }
}

void Node::backpropagate(float value){
    visits++;
    this->value += value;

    // Flip value for opponent
    if(parent != nullptr)
        parent->backpropagate(-value);
}

MCTS::MCTS(unordered_map<string, float> &args, ResNet &model)
    : args(args), model(model){}

unordered_map<string, float> MCTS::search(string state){
    Node root(state, args["c"], nullptr, "", 0, 1);

    torch::Tensor stateTensor = getStateTensor(state);
    torch::Tensor policy = model.getPolicy(stateTensor);

    Board board(state);
    MoveGenerator moves(board);
    int numMoves = moves.getMoves().size();

    // Dirichlet noise
    torch::Tensor dirichletNoise = torch::tensor(generateDirichletNoise(TOTAL_MOVES));
    policy = (1 - args["dirichletEps"]) * policy + (args["dirichletEps"] * dirichletNoise);

    // Filter out illegal moves
    torch::Tensor legalMovesTensor = getLegalMovesTensor(board);    
    policy *= legalMovesTensor;
    policy /= policy.sum();

    // Expand using the policy
    vector<float> policyVec(policy.data_ptr<float>(), policy.data_ptr<float>() + TOTAL_MOVES);
    root.expand(policyVec);

    // Perform searches
    for(int search = 0; search < static_cast<int>(args["numSearches"]); ++search){
        Node* node = &root;

        // Selection
        while(node->isFullyExpanded())
            node = node->selectChild();

        bool isTerminal = moves.isTerminalState();
        float value = moves.getValue();

        if(!isTerminal){
            stateTensor = getStateTensor(node->state);
            auto modelOutput = model.getOutput(stateTensor).toTuple();
            policy = modelOutput->elements()[0].toTensor().squeeze();
            value = modelOutput->elements()[1].toTensor().item<float>();

            // Filter out illegal moves
            legalMovesTensor = getLegalMovesTensor(Board(node->state));
            policy *= legalMovesTensor;
            policy /= policy.sum();

            // Expand using the policy
            policyVec = vector<float>(policy.data_ptr<float>(), policy.data_ptr<float>() + TOTAL_MOVES);
            node->expand(policyVec);
        }

        // Backprop
        node->backpropagate(value);
    }

    // torch::Tensor actionProbs = torch::zeros({numMoves});
    unordered_map<string, float> actionProbs(numMoves);
    float probSum = 0;
    for(const auto &child: root.children){
        actionProbs[child->moveTaken] = child->visits;
        probSum += child->visits;
    }

    for(auto &pair: actionProbs)
        pair.second /= probSum;
    return actionProbs;
}

torch::Tensor MCTS::getStateTensor(string state){
    Board board(state);

    torch::Tensor stateTensor = torch::zeros({19, 8, 8});

    // [0 - 11] Pieces
    stateTensor[0] = BBToTensor(board.getWhitePawnsBB());
    stateTensor[1] = BBToTensor(board.getWhiteRooksBB());
    stateTensor[2] = BBToTensor(board.getWhiteKnightsBB());
    stateTensor[3] = BBToTensor(board.getWhiteBishopsBB());
    stateTensor[4] = BBToTensor(board.getWhiteQueensBB());
    stateTensor[5] = BBToTensor(board.getWhiteKingBB());
    stateTensor[6] = BBToTensor(board.getBlackPawnsBB());
    stateTensor[7] = BBToTensor(board.getBlackRooksBB());
    stateTensor[8] = BBToTensor(board.getBlackKnightsBB());
    stateTensor[9] = BBToTensor(board.getBlackBishopsBB());
    stateTensor[10] = BBToTensor(board.getBlackQueensBB());
    stateTensor[11] = BBToTensor(board.getBlackKingBB());

    // [12] Turn
    stateTensor[12] = intToTensor(board.isWhiteToPlay() ? 1 : -1);

    // [13 - 16] Castling rights
    uint64_t castlingRights = board.getCastlingRights();
    stateTensor[13] = intToTensor(castlingRights & 0b0001);
    stateTensor[14] = intToTensor(castlingRights & 0b0010);
    stateTensor[15] = intToTensor(castlingRights & 0b0100);
    stateTensor[16] = intToTensor(castlingRights & 0b1000);

    // [17] enPassant square
    uint64_t enPassantSquare = 1ULL << board.getEnPassantSquare();
    stateTensor[17] = BBToTensor(enPassantSquare);

    // [18] Halfmove clock
    stateTensor[18] = floatToTensor(board.getHalfMoveClock(), 50);

    stateTensor = stateTensor.unsqueeze(0);

    return stateTensor;
}

torch::Tensor MCTS::BBToTensor(uint64_t bitboard){
    torch::Tensor tensor = torch::zeros({8, 8});

    for(int i = 0; i < 64; i++){
        if(bitboard & (1ULL << i)){
            int row = i / 8;
            int col = i % 8;
            tensor[row][col] = 1;
        }
    }

    return tensor;
}

torch::Tensor MCTS::intToTensor(int value){
    return torch::ones({8, 8}) * value;
}

torch::Tensor MCTS::floatToTensor(float value, float maxValue){
    return torch::ones({8, 8}) * (value / maxValue);
}



vector<float> MCTS::generateDirichletNoise(int numMoves){
    random_device rd;
    mt19937 gen(rd());
    gamma_distribution<float> dirichletDist(args["dirichletAlpha"], 1.0);

    vector<float> dirichletNoise(numMoves);
    float sum = 0;
    for(float &val: dirichletNoise){
        val = dirichletDist(gen);
        sum += val;
    }

    for(float &val: dirichletNoise)
        val /= sum;

    return dirichletNoise;
}

torch::Tensor MCTS::getLegalMovesTensor(Board board){
    torch::Tensor legalMovesTensor = torch::zeros({TOTAL_MOVES});
    MoveGenerator moves(board);
    for(string move: moves.getMoves())
        legalMovesTensor[moveMap.getIdx(move)] = 1;

    return legalMovesTensor;
}