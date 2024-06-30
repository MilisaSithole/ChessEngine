#include "include/piece.h"

Piece::Piece(PieceType type, Colour colour) : type(type), colour(colour) {}

PieceType Piece::getType() const{
    return type;
}

Colour Piece::getColour() const{
    return colour;
}

void Piece::setType(PieceType type){
    this->type = type;
}

void Piece::setColour(Colour colour){
    this->colour = colour;
}

char Piece::toChar(){
    if(isEmpty()){
        return '.';
    }
    
    char pieceChar;
    switch(type){
        case PieceType::Pawn:
            pieceChar = 'p';
            break;
        case PieceType::Rook:
            pieceChar = 'r';
            break;
        case PieceType::Knight:
            pieceChar = 'n';
            break;
        case PieceType::Bishop:
            pieceChar = 'b';
            break;
        case PieceType::Queen:
            pieceChar = 'q';
            break;
        case PieceType::King:
            pieceChar = 'k';
            break;
        default:
            pieceChar = '.';
            break;
    }

    return (colour == Colour::White) ? std::toupper(pieceChar) : pieceChar;
}

bool Piece::isEmpty() const{
    return type == PieceType::None;
}

std::string getPieceTypeName(PieceType type){
    switch(type){
        case PieceType::Pawn:
            return "Pawn";
        case PieceType::Rook:
            return "Rook";
        case PieceType::Knight:
            return "Knight";
        case PieceType::Bishop:
            return "Bishop";
        case PieceType::Queen:
            return "Queen";
        case PieceType::King:
            return "King";
        default:
            return "None";
    }
}