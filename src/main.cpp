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
    // string fen = "8/4k3/3P4/5N2/4R2B/8/8/4K3 b - - 1 31";
    // string fen = "8/4k3/8/8/4R3/8/8/4K3 b - - 1 31";
    // string fen = "4k3/3qr3/8/8/Q7/4Q3/8/2K5 b - - 1 31";
    // string fen = "4k3/8/6n1/4R3/8/8/8/2K5 b - d3 1 31";
    // string fen = "8/8/8/8/k2Pp2Q/8/8/3K4 b - d3 1 31";
    string fen = "2kr3r/p1p1qpb1/bn2pnp1/3PN3/1p2P3/2N5/PPPBBPPP/R3K2R w KQ - 0 3";
    // string fen = "4k3/8/4r3/8/4Q3/8/8/K7 b KQkq - 3 2";
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