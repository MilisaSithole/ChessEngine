#include "include/moveMapping.h"

MoveMap::MoveMap(){
    moveToIdx.reserve(TOTAL_MOVES);

    ifstream movesFile("../src/files/moves.csv");

    if(!movesFile.is_open()){
        cout << "Error opening moves file" << endl;
        return;
    }
    else{
        cout << "Moves file loaded" << endl;
    }

    int idx;
    string move;
    string line;

    int lineCount = 0;
    while(getline(movesFile, line)){
        lineCount++;

        size_t delimPos = line.find(',');
        idx = stoi(line.substr(0, delimPos));
        move = line.substr(delimPos + 2);
        move = move.substr(0, move.length()-1);

        idxToMove[idx] = move;
        moveToIdx[move] = idx;
    }
}