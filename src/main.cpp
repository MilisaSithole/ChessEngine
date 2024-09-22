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
    // string fen = "3r4/5pbk/P2P1n1p/q5p1/2Q2p2/5N1P/5PP1/4RBK1 w - - 1 31";
    string fen = "8/4k3/8/8/4R3/8/8/4K3 b - - 1 31";
    // fen = replaceAll(fen, "P", "1");
    // fen = replaceAll(fen, "R", "1");
    // fen = replaceAll(fen, "N", "1");
    // fen = replaceAll(fen, "B", "1");
    // fen = replaceAll(fen, "Q", "1");
    // fen = replaceAll(fen, "K", "1");
    cout << fen << endl;

    Chess chess(fen);
    // Chess chess("rnbqkbnr/pp2pppp/3p4/8/3NP3/8/PPP2PPP/RNBQKB1R w KQkq - 0 4");
    // Chess chess("r7/p1kp4/2p2Pp1/1p6/4n3/1P2R1P1/P3b1P1/2KRN3 b - - 4 23");
    // Chess chess("8/8/8/3Q4/8/8/8/8 w KQkq - 0 4");
    // Chess chess;
    chess.printBoard();

    chess.printGeneratedMoves();

    return 0;
}