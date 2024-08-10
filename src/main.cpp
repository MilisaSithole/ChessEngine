#include <stdio.h>
#include "include/chess.h"
// #include "include/board.h"
// #include "include/piece.h"
// #include "include/move.h"

using namespace std;

int main(){
    // Chess chess("rnbqkbnr/pp2pppp/3p4/8/3NP3/8/PPP2PPP/RNBQKB1R b KQkq - 0 4");
    Chess chess;
    chess.move("e2e4");
    // cout << "\n" << chess.getFen() << endl;
    chess.move("e7e5");
    // cout << "\n" << chess.getFen() << endl;
    // chess.move("b8a6");

    return 0;
}
