#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <cstdint>
#include <bitset>

#include "piece.h"

using namespace std;

// Casting rights masks
const uint8_t WHITE_KINGSIDE_CASTLE = 0x01;
const uint8_t WHITE_QUEENSIDE_CASTLE = 0x02;
const uint8_t BLACK_KINGSIDE_CASTLE = 0x04;
const uint8_t BLACK_QUEENSIDE_CASTLE = 0x08;

class Board{
public:
    Board(const string &fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    void initBoard(const string &fen);
    void printBoard();
    void parseFen(const string &fen);
    string getFen();
    Piece getPieceAt(int square);
    void makeMove(int fromSquare, int toSquare);
    int algebraicToIndex(string &square);
    string idxToAlgebraic(int &idx);
    bool isWhiteToPlay();
    void moveUpdate(int fromSquare, int toSquare);
    int getEnPassantSquare(){return enPassantIdx;};
    int getPrevEnPassantSquare(){return prevEnPassantIdx;};

    void setBitBoards();
    void updateBitBoards(int idxFrom, int idxTo);
    void printBitBoard(uint64_t bitboard);

    uint64_t getWhiteBB();
    uint64_t getBlackBB();
    uint64_t getAllBB(){return getWhiteBB() | getBlackBB();};
    uint64_t getEmptyBB(){return ~getAllBB();};
    uint64_t getWhitePawnsBB(){return whitePawnsBB;};
    uint64_t getBlackPawnsBB(){return blackPawnsBB;};
    uint64_t getWhiteRooksBB(){return whiteRooksBB;};
    uint64_t getBlackRooksBB(){return blackRooksBB;};
    uint64_t getWhiteKnightsBB(){return whiteKnightsBB;};
    uint64_t getBlackKnightsBB(){return blackKnightsBB;};
    uint64_t getWhiteBishopsBB(){return whiteBishopsBB;};
    uint64_t getBlackBishopsBB(){return blackBishopsBB;};
    uint64_t getWhiteQueensBB(){return whiteQueensBB;};
    uint64_t getBlackQueensBB(){return blackQueensBB;};
    uint64_t getWhiteKingBB(){return whiteKingBB;};
    uint64_t getBlackKingBB(){return blackKingBB;};

private:
    // Piece board[8][8];
    Piece board[64];
    bool isWhitesTurn;
    uint8_t castlingRights;
    int enPassantIdx, prevEnPassantIdx;
    int halfMoveClock;
    int fullMoveNumber;

    uint64_t whitePawnsBB   = 0ULL;
    uint64_t blackPawnsBB   = 0ULL;
    uint64_t whiteRooksBB   = 0ULL;
    uint64_t blackRooksBB   = 0ULL;
    uint64_t whiteKnightsBB = 0ULL;
    uint64_t blackKnightsBB = 0ULL;
    uint64_t whiteBishopsBB = 0ULL;
    uint64_t blackBishopsBB = 0ULL;
    uint64_t whiteQueensBB  = 0ULL;
    uint64_t blackQueensBB  = 0ULL;
    uint64_t whiteKingBB    = 0ULL;
    uint64_t blackKingBB    = 0ULL;
};

#endif