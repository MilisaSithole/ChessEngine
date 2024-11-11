#include "include/piece.h"

Piece::Piece(){
    this->type = PieceType::None;
    this->colour = Colour::None;
}

Piece::Piece(char symbol){
    type = PieceType::None;
    colour = (std::isupper(symbol)) ? Colour::White : Colour::Black;

    switch(std::tolower(symbol)){
        case 'p':
            type = PieceType::Pawn;
            break;
        case 'r':
            type = PieceType::Rook;
            break;
        case 'n':
            type = PieceType::Knight;
            break;
        case 'b':
            type = PieceType::Bishop;
            break;
        case 'q':
            type = PieceType::Queen;
            break;
        case 'k':
            type = PieceType::King;
            break;
        default:
            type = PieceType::None;
            break;
    }

    this->type = type;
    this->colour = colour;
}

PieceType Piece::getType() const{
    return type;
}

Colour Piece::getColour() const{
    return colour;
}

float Piece::getValue(){
    switch(type){
        case PieceType::Pawn:
            return 1.0f;
        case PieceType::Rook:
            return 5.0f;
        case PieceType::Knight:
            return 3.0f;
        case PieceType::Bishop:
            return 3.0f;
        case PieceType::Queen:
            return 9.0f;
        case PieceType::King:
            return 20.0f;
        default:
            return 0.0f;
    }
}

void Piece::setType(PieceType type){
    this->type = type;
}

void Piece::setColour(Colour colour){
    this->colour = colour;
}

char Piece::symbol(){
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

bool Piece::isWhite() const{
    return colour == Colour::White;
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