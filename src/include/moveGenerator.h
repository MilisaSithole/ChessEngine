#ifndef MOVEGENERATOR_H
#define MOVEGENERATOR_H

#include <vector>

#include "board.h"
#include "move.h"
#include "piece.h"

using namespace std;

class MoveGenerator{
public:
    MoveGenerator(Board &board);
    void generateLegalMoves();
    void generatePseudoLegalMoves();
    bool isMoveLegal(Move &move);
    bool isInCheck();

private:
    Board &board;
    vector<Move> pseudoLegalMoves;
    vector<Move> legalMoves;

private:
    void generatePawnMoves(uint64_t friendlyPawns, uint64_t enemyPieces, uint64_t emptySquares);
    void generateRookMoves(uint64_t friendlyRooks, uint64_t enemyPieces, uint64_t allPieces);
    void generateKnightMoves(uint64_t friendlyKnights, uint64_t friendlyPieces);
    void generateBishopMoves(uint64_t friendlyBishops, uint64_t enemyPieces, uint64_t allPieces);
    void generateQueenMoves(uint64_t friendlyQueens, uint64_t enemyPieces, uint64_t allPieces);
    void generateKingMoves(uint64_t friendlyKing, uint64_t enemyPieces, uint64_t allPieces);

    void addBBToMoves(int fromSquare, uint64_t movesBB);
};

#endif