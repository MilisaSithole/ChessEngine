#ifndef MOVEGENERATOR_H
#define MOVEGENERATOR_H

#include <vector>

#include "board.h"
#include "move.h"
#include "piece.h"

class MoveGenerator{
public:
    virtual std::vector<Move> generateMoves(const Board &board, int rank, int file) = 0;
    virtual ~MoveGenerator() = default;
};

class PawnMoveGenerator : public MoveGenerator{
public:
    std::vector<Move> generateMoves(const Board &board, int rank, int file) override;
};

class RookMoveGenerator : public MoveGenerator{
public:
    std::vector<Move> generateMoves(const Board &board, int rank, int file) override;
};

class KnightMoveGenerator : public MoveGenerator{
public:
    std::vector<Move> generateMoves(const Board &board, int rank, int file) override;
};

class BishopMoveGenerator : public MoveGenerator{
public:
    std::vector<Move> generateMoves(const Board &board, int rank, int file) override;
};

class QueenMoveGenerator : public MoveGenerator{
public:
    std::vector<Move> generateMoves(const Board &board, int rank, int file) override;
};

class KingMoveGenerator : public MoveGenerator{
public:
    std::vector<Move> generateMoves(const Board &board, int rank, int file) override;
};

#endif