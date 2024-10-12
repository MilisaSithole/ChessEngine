#ifndef MOVEGENERATOR_H
#define MOVEGENERATOR_H

#include <vector>
#include <algorithm>

#include "board.h"
#include "move.h"
#include "piece.h"

using namespace std;
class Move;

class MoveGenerator{
public:
    MoveGenerator(Board &board);
    vector<string> getMoves(){return moves;};
    void generateMoves();
    void printGeneratedMoves();
    bool isTerminalState();
    float getValue();

private:
    Board &board;
    vector<string> moves;

    uint64_t myPawns, myRooks, myKnights, myBishops, myQueens, myKing;
    uint64_t enemyPawns, enemyRooks, enemyKnights, enemyBishops, enemyQueens, enemyKing;
    uint64_t myPieces, enemyPieces;
    uint64_t emptySquares, allPieces;
    uint64_t kingDangerSquares; // Slider pieces see through king
    uint64_t attackedSquares, checkers;
    uint64_t captureMask, blockMask; // For single checks
    uint64_t pinnedPieces, pinners, pinnedRays;
    uint64_t generatedMoves;

    int numCheckers;

private:
    void generatePawnMoves(uint64_t friendlyPawns, uint64_t oppPieces, uint64_t emptySquares);
    bool checkForEnPassantDiscoveredCheck(uint64_t movingPawn, uint64_t capturedPawn);
    void generateRookMoves(uint64_t friendlyRooks, uint64_t oppPieces, uint64_t allPieces);
    void generateKnightMoves(uint64_t friendlyKnights, uint64_t friendlyPieces);
    void generateBishopMoves(uint64_t friendlyBishops, uint64_t oppPieces, uint64_t allPieces);
    void generateQueenMoves(uint64_t friendlyQueens, uint64_t oppPieces, uint64_t allPieces);
    void generateKingMoves(uint64_t friendlyKing, uint64_t oppPieces, uint64_t allPieces);

    void addBBToMoves(int fromSquare, uint64_t movesBB);

    uint64_t getAttackedSquares(bool kingDanger);
    uint64_t getPawnAttacks(uint64_t oppPawns);
    uint64_t getRookAttacks(uint64_t oppRooks, bool kingDanger, uint64_t filterOut = 0ULL);
    uint64_t getKnightAttacks(uint64_t oppKnights);
    uint64_t getBishopAttacks(uint64_t oppBishops, bool kingDanger, uint64_t filterOut = 0ULL);
    uint64_t getQueenAttacks(uint64_t oppQueens, bool kingDanger, uint64_t filterOut = 0ULL);
    uint64_t getKingAttacks(uint64_t oppKing);

    uint64_t getAttackers(uint64_t piece);
    uint64_t getPawnAttackers(uint64_t piece);
    uint64_t getAttackerRay(uint64_t piece, uint64_t attackType);
    uint64_t getRookAttackerRay(uint64_t piece, uint64_t attackers, uint64_t attackType);
    uint64_t getBishopAttackerRay(uint64_t piece, uint64_t attackers, uint64_t attackType);

    uint64_t getPinnedPieces(uint64_t piece);
    uint64_t getPinners(uint64_t pinned, uint64_t piece);
    uint64_t getPinnedRays(uint64_t pinned, uint64_t pinners, uint64_t piece);

    void updateNumCheckers();
    void updateCheckMasks();
};

#endif