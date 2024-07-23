#include <stdio.h>
#include "include/chess.h"
// #include "include/board.h"
// #include "include/piece.h"
// #include "include/move.h"

using namespace std;

int main(){
    Chess chess;
    chess.move("e2e4");
    chess.move("e7e5");

    // Board board;

    // Move move("e2e4", board);
    // board.printBoard();
    return 0;
}
