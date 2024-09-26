#include <stdio.h>
#include "include/chess.h"
// #include "include/board.h"
// #include "include/piece.h"
// #include "include/move.h"

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

        cout << "Turn: " << chess.getPlayerTurn() << endl;
        cout << "Move: ";
        cin >> uci;
    }

    return 0;
}