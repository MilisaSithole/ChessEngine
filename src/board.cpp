#include "include/board.h"
#include <vector>

Board::Board(const string &fen){
    initBoard(fen);
    setBitBoards();
}

void Board::initBoard(const string &fen){
    parseFen(fen);
}

void Board::printBoard(){
    for(int i = 0; i < 64; i++){
        cout << board[i].symbol() << " ";
        if(i % 8 == 7)
            cout << endl;
    }
}

void Board::parseFen(const string &fen){
    // Split fen into tokens
    vector<string> fenTokens;
    string fenToken;
    istringstream fenStream(fen);

    while(getline(fenStream, fenToken, ' '))
        fenTokens.push_back(fenToken);

    // Set board to its fen state
    int boardIdx = 0;
    for(char c: fenTokens[0]){
        if(isalpha(c)){
            board[boardIdx] = Piece(c);
            boardIdx++;
        } 
        else if(isdigit(c))
            boardIdx += c - '0';
        else
            continue;
    }

    // Set player turn
    isWhitesTurn = fenTokens[1] == "w";

    // Set castling rights
    castlingRights = 0;
    if(fenTokens[2].find('K') != string::npos)
        castlingRights |= WHITE_KINGSIDE_CASTLE;
    if(fenTokens[2].find('Q') != string::npos)
        castlingRights |= WHITE_QUEENSIDE_CASTLE;
    if(fenTokens[2].find('k') != string::npos)
        castlingRights |= BLACK_KINGSIDE_CASTLE;
    if(fenTokens[2].find('q') != string::npos)
        castlingRights |= BLACK_QUEENSIDE_CASTLE;

    // Set en passant square
    enPassantIdx = (fenTokens[3] == "-") ? -1 : algebraicToIndex(fenTokens[3]);

    // Set halfmove clock
    halfMoveClock = stoi(fenTokens[4]);

    // Set fullmove number
    fullMoveNumber = stoi(fenTokens[5]);
    
}

string Board::getFen(){
    string fen = "";

    // Piece placements 
    for(int i = 0; i < 64; i++){
        int emptyCellCOunt = 0;

        if(board[i].isEmpty()){
            emptyCellCOunt++;
            continue;
        }

        if(emptyCellCOunt > 0){
            fen += to_string(emptyCellCOunt);
            emptyCellCOunt = 0;
        }

        fen += board[i].symbol();

        if(i % 8 == 7){
            if(emptyCellCOunt > 0){
                fen += to_string(emptyCellCOunt);
                emptyCellCOunt = 0;
            }

            if(i != 63)
                fen += "/";
        }
    }

    // Player turn
    fen += (isWhitesTurn) ? " w" : " b";

    // Castling rights
    if(castlingRights == 0)
        fen += " -";
    else{
        fen += " ";
        if(castlingRights & WHITE_KINGSIDE_CASTLE)
            fen += "K";
        if(castlingRights & WHITE_QUEENSIDE_CASTLE)
            fen += "Q";
        if(castlingRights & BLACK_KINGSIDE_CASTLE)
            fen += "k";
        if(castlingRights & BLACK_QUEENSIDE_CASTLE)
            fen += "q";
    }

    // En passant square
    if(enPassantIdx == -1)
        fen += " -";
    else
        fen += " " + idxToAlgebraic(enPassantIdx);

    // Halfmove clock
    fen += " " + to_string(halfMoveClock);

    // Fullmove number
    fen += " " + to_string(fullMoveNumber);

    return fen;
}

Piece Board::getPieceAt(int square){
    return board[square];
}

void Board::makeMove(int fromSquare, int toSquare){
    // Update bit boards
    updateBitBoards(fromSquare, toSquare);

    // Check for double pawn push
    if(board[fromSquare].getType() == PieceType::Pawn && abs(fromSquare - toSquare) == 16){
        enPassantIdx = (fromSquare + toSquare) / 2;
    }
    else
        enPassantIdx = -1;

    board[toSquare] = board[fromSquare];
    board[fromSquare] = Piece();

}

int Board::algebraicToIndex(string &square){
    return int(square[0] - 'a') + ((7 - int(square[1] - '1')) * 8);
}

string Board::idxToAlgebraic(int &idx){
    return string(1, 'a' + idx % 8) + to_string(8 - idx / 8);
}

bool  Board::isWhiteToPlay(){
    return isWhitesTurn;
}

void Board::moveUpdate(int fromSquare, int toSquare){

    if(board[fromSquare].getType() == PieceType::Pawn || 
       board[toSquare].getType() == PieceType::Pawn)
        halfMoveClock = 0;
    else
        halfMoveClock++;

    if(!isWhitesTurn)
        fullMoveNumber++;

    isWhitesTurn = !isWhitesTurn;
}

void Board::setBitBoards(){
    for(int square = 0; square < 64; square++){

        if(board[square].isEmpty()) continue;
        Piece currPiece = board[square];
        
        if(currPiece.getType() == PieceType::Pawn){
            if(currPiece.isWhite())
                whitePawnsBB |= (1ULL << square);
            else
                blackPawnsBB |= (1ULL << square);
        }
        else if(currPiece.getType() == PieceType::Rook){
            if(currPiece.isWhite())
                whiteRooksBB |= (1ULL << square);
            else
                blackRooksBB |= (1ULL << square);
        }
        else if(currPiece.getType() == PieceType::Knight){
            if(currPiece.isWhite())
                whiteKnightsBB |= (1ULL << square);
            else
                blackKnightsBB |= (1ULL << square);
        }
        else if(currPiece.getType() == PieceType::Bishop){
            if(currPiece.isWhite())
                whiteBishopsBB |= (1ULL << square);
            else
                blackBishopsBB |= (1ULL << square);
        }
        else if(currPiece.getType() == PieceType::Queen){
            if(currPiece.isWhite())
                whiteQueensBB |= (1ULL << square);
            else
                blackQueensBB |= (1ULL << square);
        }
        else if(currPiece.getType() == PieceType::King){
            if(currPiece.isWhite())
                whiteKingBB |= (1ULL << square);
            else
                blackKingBB |= (1ULL << square);
        }
    }
}

void Board::updateBitBoards(int fromSquare, int toSquare){

    Piece movingPiece = board[fromSquare];
    Piece capturedPiece = board[toSquare];

    // Move moving piece
    if(movingPiece.getType() == PieceType::Pawn){
        if(movingPiece.isWhite()){
            whitePawnsBB &= ~(1ULL << fromSquare);
            whitePawnsBB |= (1ULL << toSquare);
        }
        else{
            blackPawnsBB &= ~(1ULL << fromSquare);
            blackPawnsBB |= (1ULL << toSquare);
        }
    }
    else if(movingPiece.getType() == PieceType::Rook){
        if(movingPiece.isWhite()){
            whiteRooksBB &= ~(1ULL << fromSquare);
            whiteRooksBB |= (1ULL << toSquare);
        }
        else{
            blackRooksBB &= ~(1ULL << fromSquare);
            blackRooksBB |= (1ULL << toSquare);
        }
    }
    else if(movingPiece.getType() == PieceType::Knight){
        if(movingPiece.isWhite()){
            whiteKnightsBB &= ~(1ULL << fromSquare);
            whiteKnightsBB |= (1ULL << toSquare);
        }
        else{
            blackKnightsBB &= ~(1ULL << fromSquare);
            blackKnightsBB |= (1ULL << toSquare);
        }
    }
    else if(movingPiece.getType() == PieceType::Bishop){
        if(movingPiece.isWhite()){
            whiteBishopsBB &= ~(1ULL << fromSquare);
            whiteBishopsBB |= (1ULL << toSquare);
        }
        else{
            blackBishopsBB &= ~(1ULL << fromSquare);
            blackBishopsBB |= (1ULL << toSquare);
        }
    }
    else if(movingPiece.getType() == PieceType::Queen){
        if(movingPiece.isWhite()){
            whiteQueensBB &= ~(1ULL << fromSquare);
            whiteQueensBB |= (1ULL << toSquare);
        }
        else{
            blackQueensBB &= ~(1ULL << fromSquare);
            blackQueensBB |= (1ULL << toSquare);
        }
    }
    else if(movingPiece.getType() == PieceType::King){
        if(movingPiece.isWhite()){
            whiteKingBB &= ~(1ULL << fromSquare);
            whiteKingBB |= (1ULL << toSquare);
        }
        else{
            blackKingBB &= ~(1ULL << fromSquare);
            blackKingBB |= (1ULL << toSquare);
        }
    }

    // Remove captured piece
    if(!capturedPiece.isEmpty()){
        if(capturedPiece.getType() == PieceType::Pawn){
            if(capturedPiece.isWhite())
                whitePawnsBB &= ~(1ULL << toSquare);
            else
                blackPawnsBB &= ~(1ULL << toSquare);
        }
        else if(capturedPiece.getType() == PieceType::Rook){
            if(capturedPiece.isWhite())
                whiteRooksBB &= ~(1ULL << toSquare);
            else
                blackRooksBB &= ~(1ULL << toSquare);
        }
        else if(capturedPiece.getType() == PieceType::Knight){
            if(capturedPiece.isWhite()){
                whiteKnightsBB &= ~(1ULL << toSquare);
            }
            else{
                blackKnightsBB &= ~(1ULL << toSquare);
            }
        }
        else if(capturedPiece.getType() == PieceType::Bishop){
            if(capturedPiece.isWhite()){
                whiteBishopsBB &= ~(1ULL << toSquare);
            }
            else{
                blackBishopsBB &= ~(1ULL << toSquare);
            }
        }
        else if(capturedPiece.getType() == PieceType::Queen){
            if(capturedPiece.isWhite()){
                whiteQueensBB &= ~(1ULL << toSquare);
            }
            else{
                blackQueensBB &= ~(1ULL << toSquare);
            }
        }
        else if(capturedPiece.getType() == PieceType::King){
            if(capturedPiece.isWhite()){
                whiteKingBB &= ~(1ULL << toSquare);
            }
            else{
                blackKingBB &= ~(1ULL << toSquare);
            }
        }
    }
}

void Board::printBitBoard(uint64_t bitboard){
    for(int i = 0; i < 64; i++){
        cout << ((bitboard >> i) & 1) << " ";
        if(i % 8 == 7)
            cout << "\n";
    }
    cout << endl;
}

uint64_t Board::getWhiteBB(){
    return whitePawnsBB | whiteRooksBB | whiteKnightsBB | whiteBishopsBB | whiteQueensBB | whiteKingBB;
}

uint64_t Board::getBlackBB(){
    return blackPawnsBB | blackRooksBB | blackKnightsBB | blackBishopsBB | blackQueensBB | blackKingBB;
}