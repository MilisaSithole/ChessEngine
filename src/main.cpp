#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <unordered_map>
#include <ctime>
#include <string>
#include <unistd.h>
#include <sstream>
#include <vector>
#include <deque>
#include <thread>
#include <mutex>
#include <atomic>
#include <random>

#include "include/chess.h"
#include "include/agent.h"
#include "include/resNet.h"

using namespace std;

atomic<int> totalGames(0);
atomic<int> totalMatches(0);
vector<string> startingPositions;
auto seed = chrono::high_resolution_clock::now().time_since_epoch().count();
mt19937 gen(seed);

// For recording games
string nameWhite, nameBlack, startingFen, gameResult;
vector<string> moves;

string getTimeSince(time_t start);
int timeToInt(time_t startTime, time_t endTime);
string intToTimeStr(int seconds);
void selfPlay(int threadID, unordered_map<string, float> &args, 
              ResNet &resNet, int &totalMoves, time_t &totalStartTime, int results[], 
              mutex &updateMtx);
void writeToMainFile(int numThreads, int modelVersion = 0);
void playGame(unordered_map<string, float> args, ResNet &resNet);
void playRandom(unordered_map<string, float> args, ResNet &resNet);
void agentVsAgent(int threadID, unordered_map<string, float> args, ResNet &resNet1, ResNet &resNet2);
void keepLastNLines(int n, int modelVersion = 0);
void loadStartingPositions();
string getRandomStartingPos();
void saveGame();
void saveThreadGame(int threadID, string white, string black, string startingFen, vector<string> moves, string result);
void combineThreadGames(int numThreads);


int main(){
    mutex updateMtx;

    int modelVersion = 1;
    ResNet resNet(modelVersion);
    unordered_map<string, float> args{
        {"c", 1.14},
        {"temperature", 0.99},
        {"numSearches", 100},
        {"numSelfPlayIterations", 128},
        {"maxMoves", 100},
        {"_50MoveRule", 25},
        {"numMovesToKeep", 3000},
        {"numMatches", 50}
    };
    loadStartingPositions();
    bool play = true;

    ResNet resNet1(0);
    ResNet resNet2(1);
    char playAgain = 'y';
    // while(play && playAgain == 'y'){
    //     // agentVsAgent(args, resNet1, resNet2);
    //     playRandom(args, resNet);

    //     cout << "Play again? (y/n): ";
    //     cin >> playAgain;
    // }

    // for(int i = 0; i < 100; i++)
    //     playRandom(args, resNet);
    // if(play) return 0;

    auto totalStartTime = time(nullptr);
    int totalSeconds = 0;
    int totalMoves = 0;
    int results[] = {0, 0, 0}; //White - Draw - Black
    
    int numThreads = thread::hardware_concurrency();
    numThreads = numThreads * 8 / 10;

    vector<thread> threads;

    cout << "Recording " << static_cast<int>(args["numMatches"]) << " matches..." << endl;
    for(int i = 0; i < numThreads; i++)
        threads.push_back(thread(agentVsAgent, i, args, ref(resNet1), ref(resNet2)));

    // // Self Play
    // for(int i = 0; i < numThreads; i++)
    //     threads.push_back(thread(selfPlay, i, ref(args), ref(resNet), ref(totalMoves), ref(totalStartTime), results, ref(updateMtx)));

    for(auto &thread: threads)
        thread.join();

    combineThreadGames(numThreads);
    return 0;

    cout << "=============================================================" << endl;
    cout << "White: " << results[0] << " | " << "Draw: " << results[1] << " | "
         << "Black: " << results[2] << endl;
    cout << "Total Moves: " << totalMoves << " | Total time taken: " << getTimeSince(totalStartTime) << "\n" << endl;
    cout << "=============================================================" << endl;

    totalSeconds = timeToInt(totalStartTime, time(nullptr));
    auto startWritingTime = time(nullptr);
    keepLastNLines(static_cast<int>(args["numMovesToKeep"]), modelVersion);
    writeToMainFile(numThreads, modelVersion);
    cout << "Time taken to write to file: " << getTimeSince(startWritingTime) << endl;
    cout << "=============================================================" << endl;

    // Write stats into file
    string fileName = "../stats/stats.csv";
    ofstream statsFile(fileName, ios::app);

    // Check file not open
    if(!statsFile.is_open()){
        cout << "Error, couldn't open stats file: " << fileName << endl;
        return 0;
    }

    // Write to file
    statsFile << totalMoves << ","
              << totalSeconds << ","
              << results[0] << "," << results[1] << "," << results[2] << "\n";
    statsFile.close();

    return 0;
}

string getTimeSince(time_t start_time){
    time_t current_time = std::time(nullptr);
    int elapsed_time = static_cast<int>(current_time - start_time);

    // Calculate hours, minutes and seconds
    int hrs = elapsed_time / 3600;
    int mins = (elapsed_time % 3600) / 60;
    int secs = elapsed_time % 60;

    string out = "";
    if(hrs > 0){
        out += to_string(hrs) + ":";
        out += (mins < 10 ? "0" : "") + to_string(mins) + ":";
        out += (secs < 10 ? "0" : "") + to_string(secs);
    }
    else{
        out += (mins < 10 ? "0" : "") + to_string(mins) + ":";
        out += (secs < 10 ? "0" : "") + to_string(secs);
    }
    
    return out;
}

int timeToInt(time_t startTime, time_t endTime){
    return static_cast<int>(endTime - startTime);
}

string intToTimeStr(int seconds){
    // Calculate hours, minutes and seconds
    int hrs = seconds / 3600;
    int mins = (seconds % 3600) / 60;
    int secs = seconds % 60;

    string out = "";
    if(hrs > 0){
        out += to_string(hrs) + ":";
        out += (mins < 10 ? "0" : "") + to_string(mins) + ":";
        out += (secs < 10 ? "0" : "") + to_string(secs);
    }
    else{
        out += (mins < 10 ? "0" : "") + to_string(mins) + ":";
        out += (secs < 10 ? "0" : "") + to_string(secs);
    }
    
    return out;
}

void selfPlay(int threadID, unordered_map<string, float> &args, ResNet &resNet, int &totalMoves, time_t &totalStartTime, int results[], mutex &updateMtx){
    Agent agent(true, args, resNet, threadID);

    while(totalGames < args["numSelfPlayIterations"]){
        Chess chess(args, getRandomStartingPos());
        // Chess chess(args);
        agent.setIsWhite((chess.getPlayerTurn() == "White") ? true : false);
        bool isGameOver = false;
        string prevFen = "";
        string lan;
        int numMoves = 0;
        int currGame = totalGames.load();
        totalGames++;

        auto startTime = time(nullptr);
        cout << "[" << threadID << "]\t Self play game: " << currGame+1 << " of " << args["numSelfPlayIterations"] << " " << chess.getFen() << endl;

        // Game loop
        while(!isGameOver){
            if(prevFen == chess.getFen())
                lan = agent.getRandomMove(chess.getBoard());
            else
                lan = agent.getMove(chess.getBoard());

            prevFen = chess.getFen();

            if(chess.isValidMove(lan)){
                chess.move(lan);
                agent.updateStatesRecord(chess.getFen(), chess.getCaptureReward());
                ++numMoves;
                isGameOver = chess.isGameOver();
            }
        }

        string result = chess.getResult();
        agent.saveGame(result, threadID);
        cout << "[" << threadID << "] \t Result: " << result 
             << " | " << numMoves << " | " << getTimeSince(startTime) 
             << " | " << getTimeSince(totalStartTime) << endl;

        // Update gloabal variables
        {
            lock_guard<mutex> lock(updateMtx);
            totalMoves += numMoves;
            if(result == "White") results[0]++;
            else if(result == "Draw") results[1]++;
            else results[2]++;
        }
    }

    cout << "[" << threadID << "]\t Done!" << endl;
}

void writeToMainFile(int numThreads, int modelVersion){
    string path = "../src/games/games_" + to_string(modelVersion);

    string mainFilePath = "../src/games/games_" + to_string(modelVersion) + ".csv";
    ofstream mainFile(mainFilePath, ios::app);

    for(int i = 0; i < numThreads; i++){
        string inFileName = path + to_string(i) + ".csv";
        ifstream inFile(inFileName);

        if(inFile.is_open()){
            string line;
            while(getline(inFile, line))
                mainFile << line << "\n";
            inFile.close();

            // Delete file
            if(remove(inFileName.c_str()) != 0)
                cout << "Error deleting file" << endl;
        }
    }
}

void keepLastNLines(int n, int modelVersion){
    string mainFilePath = "../src/games/games_" + to_string(modelVersion) + ".csv";
    ifstream inFile(mainFilePath);
    deque<vector<string>> lines;
    string line;

    ofstream winlossFile("../src/games/winloss.csv", ios::app);

    vector<string> currLines(3);
    int lineIdx = 0;
    while(getline(inFile, line)){
        currLines[lineIdx % 3] = line;
        lineIdx++;

        if(lineIdx % 3 == 0){
            lines.push_back(currLines);

            int currTerminalVal = stoi(currLines[2]);
            if(currTerminalVal != 0){
                winlossFile << currLines[0] << "\n";
                winlossFile << currLines[1] << "\n";
                winlossFile << currLines[2] << "\n";
            }

            if(lines.size() > n){
                int terminalVal = stoi(currLines[2]);

                if(terminalVal == 0)
                    lines.pop_back();
                else
                    lines.pop_front();
            }
        }
    }
    inFile.close();
    winlossFile.close();

    if(lines.size() < n)
        return;

    ofstream outFile(mainFilePath);
    for(const auto &record: lines)
        for(const auto &l: record)
            outFile << l << "\n";
    outFile.close();
}

void playGame(unordered_map<string, float> args, ResNet &resNet){
    args["maxMoves"] = 6000;
    args["_50MoveRule"] = 50;
    
    Agent agent(true, args, resNet);
    Chess chess(args);
    bool isGameOver = false;
    string prevFen = "";
    string lan;

    cout << chess.getFen() << endl;
    chess.printBoard();

    // Game loop
    char inp;
    bool autoPlay = false;
    while(!isGameOver){
        if(!autoPlay){
            inp = getchar();

            if(inp == 'q')
                return;
            if(inp == 'm'){
                chess.printGeneratedMoves();
                continue;
            }
            if(inp == 'k'){
                chess.printKingDanger();
                continue;
            }
            if(inp == 'a')
                autoPlay = true;
        }
        lan = "";

        if(prevFen == chess.getFen())
            lan = agent.getRandomMove(chess.getBoard());
        else
            lan = agent.getMove(chess.getBoard());

        // if(autoPlay && (chess.getSan(lan).find("#") || chess.getSan(lan).find("+")))
        //     autoPlay = false;

        prevFen = chess.getFen();
        if(chess.isValidMove(lan)){
            cout << chess.getPlayerTurn() << " | " << lan << " | " << chess.getSan(lan) << endl;
            chess.move(lan);
            cout << chess.getFen() << endl;
            chess.printBoard();
            isGameOver = chess.isGameOver();
        }
    }
    cout << "Result: " << chess.getResult() << endl;
}

void playRandom(unordered_map<string, float> args, ResNet &resNet){
    args["maxMoves"] = 6000;
    args["_50MoveRule"] = 50;

    // Randomly assign agent color
    srand(time(0));
    bool agentIsWhite = rand() % 2 == 0;
    Agent agent(agentIsWhite, args, resNet);
    // Chess chess(args, getRandomStartingPos());
    Chess chess(args);

    if(agentIsWhite){
        nameWhite = "Agent";
        nameBlack = "Random";
    }
    else{
        nameWhite = "Random";
        nameBlack = "Agent";
    }
    startingFen = chess.getFen();
    moves.clear();

    bool isGameOver = false;
    string prevFen = "";
    string lan;

    // cout << chess.getFen() << endl;
    // cout << "Agent: " << ((agentIsWhite) ? "White" : "Black") << endl;
    // chess.printBoard();

    // Game loop
    while(!isGameOver){
        lan = "";

        // If it's the agent's turn
        if(agentIsWhite == (chess.getPlayerTurn() == "White")){
            if(prevFen == chess.getFen())
                lan = agent.getRandomMove(chess.getBoard());
            else
                lan = agent.getMove(chess.getBoard());
        }
        // If it's the opponent's turn (random moves)
        else {
            // sleep(2);
            lan = agent.getRandomMove(chess.getBoard());
        }

        prevFen = chess.getFen();
        if(chess.isValidMove(lan)){
            // cout << chess.getPlayerTurn() << " | " << lan << " | " << chess.getSan(lan) << endl;
            chess.move(lan);
            // cout << chess.getFen() << endl;
            // chess.printBoard();
            isGameOver = chess.isGameOver();

            moves.push_back(lan);
        }
    }
    cout << "Result: " << chess.getResult() << endl;
    gameResult = chess.getResult();
    saveGame();
}

void agentVsAgent(int threadID, unordered_map<string, float> args, ResNet &resNet1, ResNet &resNet2){
    args["maxMoves"] = 6000;
    args["_50MoveRule"] = 50;

    Agent agent1(false, args, resNet2);
    Agent agent2(true, args, resNet1, true);
    nameWhite = "Improved";
    nameBlack = "Random";

    while(totalMatches < args["numMatches"]){
        vector<string> moves;
        Chess chess(args, getRandomStartingPos());
        string startingFen = chess.getFen();
        cout << "[ " << threadID << " ]\t" << startingFen << endl;
        int currGame = totalMatches.load();
        totalMatches++;
        
        bool isGameOver = false;
        string prevFen = "";
        string lan;

        while(!isGameOver){
            lan = "";

            if(chess.getPlayerTurn() == "White"){
                if(prevFen == chess.getFen())
                    lan = agent1.getRandomMove(chess.getBoard());
                else
                    lan = agent1.getMove(chess.getBoard());
            }
            else {
                if(prevFen == chess.getFen())
                    lan = agent2.getRandomMove(chess.getBoard());
                else
                    lan = agent2.getMove(chess.getBoard());
            }

            prevFen = chess.getFen();
            if(chess.isValidMove(lan)){
                chess.move(lan);
                isGameOver = chess.isGameOver();

                moves.push_back(lan);
            }
        }

        cout << "[ " << threadID << " ]\t" << currGame+1 << " | Result: " << chess.getResult() << endl;
        saveThreadGame(threadID, nameWhite, nameBlack, startingFen, moves, chess.getResult());

    }
}

/*
void agentVsAgent(unordered_map<string, float> args, ResNet &resNet1, ResNet &resNet2){
    args["maxMoves"] = 6000;
    args["_50MoveRule"] = 50;

    Agent agent1(true, args, resNet1);
    Agent agent2(false, args, resNet2);
    // Chess chess(args, getRandomStartingPos());
    Chess chess(args);

    nameWhite = "Base";
    nameBlack = "Improved";
    startingFen = chess.getFen();
    moves.clear();

    bool isGameOver = false;
    string prevFen = "";
    string lan;

    cout << chess.getFen() << endl;
    chess.printBoard();

    // Game loop
    while(!isGameOver){
        lan = "";

        // If it's the agent's turn
        if(chess.getPlayerTurn() == "White"){
            if(prevFen == chess.getFen())
                lan = agent1.getRandomMove(chess.getBoard());
            else
                lan = agent1.getMove(chess.getBoard());
        }
        // If it's the opponent's turn (random moves)
        else {
            if(prevFen == chess.getFen())
                lan = agent2.getRandomMove(chess.getBoard());
            else
                lan = agent2.getMove(chess.getBoard());
        }

        prevFen = chess.getFen();
        if(chess.isValidMove(lan)){
            cout << chess.getPlayerTurn() << " | " << lan << " | " << chess.getSan(lan) << endl;
            chess.move(lan);
            cout << chess.getFen() << endl;
            chess.printBoard();
            isGameOver = chess.isGameOver();

            moves.push_back(lan);
        }
    }
    cout << "Result: " << chess.getResult() << endl;
    gameResult = chess.getResult();
    saveGame();
}
*/

void loadStartingPositions(){
    // ifstream posFile("../puzzles/GeneratedEndgames.txt");
    ifstream posFile("../puzzles/equalFens.txt");
    string line;
    while(getline(posFile, line))
        startingPositions.push_back(line);
    posFile.close();
}

string getRandomStartingPos(){
    // auto seed = chrono::high_resolution_clock::now().time_since_epoch().count();
    // mt19937 gen(seed);
    uniform_int_distribution<> dist(0, startingPositions.size()-1);
    return startingPositions[dist(gen)];
}

void saveGame(){
    ofstream gameFile("../src/games/game.txt", ios::app);
    gameFile << nameWhite << "," << nameBlack << endl;
    gameFile << startingFen << endl;

    for(string lan : moves)
        gameFile << lan << " ";
    gameFile << endl;
    gameFile << gameResult << endl;
    gameFile.close();
}

void saveThreadGame(int threadID, string white, string black, string startingFen, vector<string> moves, string result){
    ofstream threadGameFile("../src/games/game" + to_string(threadID) + ".txt", ios::app);
    threadGameFile << white << "," << black << endl;
    threadGameFile << startingFen << endl;

    for(string lan : moves)
        threadGameFile << lan << " ";
    threadGameFile << endl;
    threadGameFile << result << endl;
    threadGameFile.close();

}

void combineThreadGames(int numThreads){
    ofstream combinedGameFile("../src/games/combinedGame.txt", ios::app);
    string path = "../src/games/game";

    for(int i=0; i<numThreads; i++){
        string filePath = path + to_string(i) + ".txt";
        ifstream threadGameFile(filePath);

        string line;
        while(getline(threadGameFile, line))
            combinedGameFile << line << endl;
        threadGameFile.close();
        remove(filePath.c_str());
    }
    combinedGameFile.close();
}