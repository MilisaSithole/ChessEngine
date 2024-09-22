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

    uint64_t myPawns, myRooks, myKnights, myBishops, myQueens, myKing;
    uint64_t enemyPawns, enemyRooks, enemyKnights, enemyBishops, enemyQueens, enemyKing;
    uint64_t myPieces, enemyPieces;
    uint64_t emptySquares, allPieces;

private:
    void generatePawnMoves(uint64_t friendlyPawns, uint64_t oppPieces, uint64_t emptySquares);
    void generateRookMoves(uint64_t friendlyRooks, uint64_t oppPieces, uint64_t allPieces);
    void generateKnightMoves(uint64_t friendlyKnights, uint64_t friendlyPieces);
    void generateBishopMoves(uint64_t friendlyBishops, uint64_t oppPieces, uint64_t allPieces);
    void generateQueenMoves(uint64_t friendlyQueens, uint64_t oppPieces, uint64_t allPieces);
    void generateKingMoves(uint64_t friendlyKing, uint64_t oppPieces, uint64_t allPieces);

    void addBBToMoves(int fromSquare, uint64_t movesBB);

    uint64_t getAttackedSquares();
    // TODO: refactor these funcs
    uint64_t getPawnAttacks(uint64_t oppPawns);
    uint64_t getRookAttacks(uint64_t oppRooks);
    uint64_t getKnightAttacks(uint64_t oppKnights);
    uint64_t getBishopAttacks(uint64_t oppBishops);
    uint64_t getQueenAttacks(uint64_t oppQueens);
    uint64_t getKingAttacks(uint64_t oppKing);
};

#endif