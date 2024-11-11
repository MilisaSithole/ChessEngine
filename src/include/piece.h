#ifndef PIECE_H
#define PIECE_H

#include <cctype>
#include <iostream>
#include <string>

enum class PieceType {
    Pawn,
    Rook,
    Knight,
    Bishop,
    Queen,
    King,
    None
};
enum class Colour {
    White,
    Black,
    None
};

class Piece{
public:
    Piece();
    Piece(char symbol);

    PieceType getType() const;
    Colour getColour() const;
    float getValue();

    void setType(PieceType type);
    void setColour(Colour colour);

    char symbol();
    bool isEmpty() const;
    bool isWhite() const;

private:
    PieceType type;
    Colour colour;
};

std::string getPieceTypeName(PieceType type);

#endif