#include "include/board.h"
#include <vector>

Board::Board(string fen){
    initBoard(fen);
    setBitBoards();
}

void Board::initBoard(string fen){
    parseFen(fen);
}

void Board::printBoard(){
    for(int i = 0; i < 64; i++){
        if(i == lastMoveIdx)
            cout << "□ ";
        else
            cout << board[i].symbol() << " ";
        if(i % 8 == 7)
            cout << endl;
    }
}

void Board::parseFen(string fen){
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
    int emptyCellCount = 0;
    for(int i = 0; i < 64; i++){

        

        if(board[i].isEmpty()){
            emptyCellCount++;
        }
        else{
            if(emptyCellCount > 0){
                fen += to_string(emptyCellCount);
                emptyCellCount = 0;
            }

            fen += board[i].symbol();
        }

        if(i % 8 == 7){
            if(emptyCellCount > 0){
                fen += to_string(emptyCellCount);
                emptyCellCount = 0;
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

void Board::makeMove(int fromSquare, int toSquare, string promotion){
    lastMoveIdx = fromSquare;
    moveUpdate(fromSquare, toSquare);
    updateBitBoards(fromSquare, toSquare, promotion);
    updateBoard(fromSquare, toSquare, promotion);
}

void Board::makeMove(string lan){
    string fromSquareStr = lan.substr(0, 2);
    string toSquareStr = lan.substr(2, 2);
    int fromSquare = algebraicToIndex(fromSquareStr);
    int toSquare = algebraicToIndex(toSquareStr);
    string promotion = "";
    if(lan.length() > 4)
        promotion = lan.substr(4, 1);

    makeMove(fromSquare, toSquare, promotion);
}

void Board::updateBoard(int fromSquare, int toSquare, string promotion){
    // Promotion
    if(promotion != "")
        return;
    else {
        board[toSquare] = board[fromSquare];
        board[fromSquare] = Piece();
    }
}

int Board::algebraicToIndex(string &square){
    return int(square[0] - 'a') + ((7 - int(square[1] - '1')) * 8);
}

string Board::idxToAlgebraic(int &idx){
    return string(1, 'a' + idx % 8) + to_string(8 - idx / 8);
}

bool Board::isWhiteToPlay(){
    return isWhitesTurn;
}

/// <summary>
/// Updates the game state and bitboards for a move.
/// </summary>
/// <param name="fromSquare">The square the piece is moving from.</param>
/// <param name="toSquare">The square the piece is moving to.</param>
void Board::moveUpdate(int fromSquare, int toSquare){
    // En passant capture [Makes the move]
    if(board[fromSquare].getType() == PieceType::Pawn && toSquare == enPassantIdx){
        int direction = (isWhitesTurn) ? 8 : -8;
        
        board[toSquare + direction] = Piece();
        updateBitBoards(toSquare + direction, -1);
    }

    // Castling [Makes the move]
    if(board[fromSquare].getType() == PieceType::King){
        if(isWhitesTurn){
            if((fromSquare == 60) && (toSquare == 62) && (castlingRights & 0b0001)){
                updateBitBoards(63, 61);
                board[61] = board[63];
                board[63] = Piece();
                castlingRights &= 0b1100;
            }
            else if((fromSquare == 60) && (toSquare == 58) && (castlingRights & 0b0010)){
                updateBitBoards(56, 59);
                board[59] = board[56];
                board[56] = Piece();
                castlingRights &= 0b1100;   
            }
        }
        else{
            if((fromSquare == 4) && (toSquare == 6) && (castlingRights & 0b0001)){
                updateBitBoards(7, 5);
                board[5] = board[7];
                board[7] = Piece();
                castlingRights &= 0b0011;
            }
            else if((fromSquare == 4) && (toSquare == 2) && (castlingRights & 0b0010)){
                updateBitBoards(0, 3);
                board[3] = board[0];
                board[0] = Piece();
                castlingRights &= 0b0011;
            }
        }
    }

    // Check if rooks moved or captured to update castling rights
    if((fromSquare == 0 || fromSquare == 7) && board[fromSquare].getType() == PieceType::Rook && !board[fromSquare].isWhite())
        castlingRights &= 0b0011;
    if((toSquare == 0 || toSquare == 7) && board[toSquare].getType() == PieceType::Rook && !board[toSquare].isWhite())
        castlingRights &= 0b0011;
    if((fromSquare == 56 || fromSquare == 63) && board[fromSquare].getType() == PieceType::Rook && board[fromSquare].isWhite())
        castlingRights &= 0b1100;
    if((toSquare == 56 || toSquare == 63) && board[toSquare].getType() == PieceType::Rook && board[toSquare].isWhite())
        castlingRights &= 0b1100;

    // Update en passant
    prevEnPassantIdx = enPassantIdx;
    enPassantIdx = -1;
    if(board[fromSquare].getType() == PieceType::Pawn)
        if(abs((fromSquare / 8) - (toSquare / 8)) == 2)
            enPassantIdx = (fromSquare + toSquare) / 2;
        

    // Update half move clock
    if(board[fromSquare].getType() == PieceType::Pawn || 
       board[toSquare].getType() != PieceType::None)
        halfMoveClock = 0;
    else
        halfMoveClock++;

    // Full move counter
    if(!isWhitesTurn)
        fullMoveNumber++;

    // Change turn
    isWhitesTurn = !isWhitesTurn;
}

int Board::getNumPieces(){
    int num = 0;
    for(int square = 0; square < 64; square++){
        if(board[square].getType() != PieceType::None)
            num++;
    }
    return num;
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

void Board::updateBitBoards(int fromSquare, int toSquare, string promotion){
    // For en passant capture
    if(toSquare == -1){
        if(board[fromSquare].isWhite())
            whitePawnsBB &= ~(1ULL << fromSquare);
        else
            blackPawnsBB &= ~(1ULL << fromSquare);

        return;
    }


    Piece movingPiece = board[fromSquare];
    Piece capturedPiece = board[toSquare];

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

    // Move moving piece
    if(promotion != ""){
        if(movingPiece.isWhite())
            whitePawnsBB &= ~(1ULL << fromSquare);
        else
            blackPawnsBB &= ~(1ULL << fromSquare);
            
        switch(promotion[0]){
            case 'R':
                if(movingPiece.isWhite()){
                    board[toSquare] = Piece('R');
                    whiteRooksBB |= (1ULL << toSquare);
                }
                else{
                    board[toSquare] = Piece('r');
                    blackRooksBB |= (1ULL << toSquare);
                }
                break;
            case 'N':
                if(movingPiece.isWhite()){
                    board[toSquare] = Piece('N');
                    whiteKnightsBB |= (1ULL << toSquare);
                }
                else{
                    board[toSquare] = Piece('n');
                    blackKnightsBB |= (1ULL << toSquare);
                }
                break;
            case 'B':
                if(movingPiece.isWhite()){
                    board[toSquare] = Piece('B');
                    whiteBishopsBB |= (1ULL << toSquare);
                }
                else{
                    board[toSquare] = Piece('b');
                    blackBishopsBB |= (1ULL << toSquare);
                }
                break;
            case 'Q':
                if(movingPiece.isWhite()){
                    board[toSquare] = Piece('Q');
                    whiteQueensBB |= (1ULL << toSquare);
                }
                else{
                    board[toSquare] = Piece('q');
                    blackQueensBB |= (1ULL << toSquare);
                }
                break;
        }
        board[fromSquare] = Piece();
        return;
    }

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