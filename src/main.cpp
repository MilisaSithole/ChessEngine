#include <stdio.h>
#include "include/chess.h"
// #include "include/board.h"
// #include "include/piece.h"
// #include "include/move.h"
#include <iostream>
#include <filesystem>

using namespace std;

std::string replaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
    return str;
}

int main(){
    // string relativePath;
    // cin >> relativePath;

    // while(relativePath != "exit"){
    //     string path = filesystem::current_path().parent_path().string() + "/" + relativePath;
    //     cout << "Path: " << path << endl;
    //     if(filesystem::exists(path)){
    //         cout << "File exists" << endl;
    //     }
    //     else{
    //         cout << "File does not exist" << endl;
    //     }
    //     cout << "Enter relative path (or 'exit' to quit): ";
    //     cin >> relativePath;
    // }
    // return 0;

    // cout << "Parent directory: " << filesystem::current_path().parent_path() << endl;
    // cout << "Current executable path: " << filesystem::current_path() << endl;
    // cout << "Directory contents:" << endl;
    // for (const auto & entry : filesystem::directory_iterator(filesystem::current_path())) {
    //     cout << '\t' << entry.path().filename() << endl;
    // }
    // return 0;


    Chess chess("r3k2r/8/8/8/5pP1/8/8/R3K2R b KQkq g3 0 1");
    // Chess chess;
    chess.printBoard();

    string uci;
    cout << "Turn: " << chess.getPlayerTurn() << endl;
    cout << "Move: ";
    cin >> uci;

    while(uci != "m"){
        chess.move(uci);
        cout << chess.getFen() << endl;
        chess.printBoard();

        cout << "Turn: " << chess.getPlayerTurn() << endl;
        cout << "Move: ";
        cin >> uci;
    }

    return 0;
}