#include "include/moveGenerator.h"

MoveGenerator::MoveGenerator(Board &board) : board(board) {
    generateLegalMoves();
}

void MoveGenerator::generateLegalMoves(){
    generatePseudoLegalMoves();
}

void MoveGenerator::generatePseudoLegalMoves(){
    if(board.isWhiteToPlay()){
        generatePawnMoves(board.getWhitePawnsBB(), board.getBlackBB(), board.getEmptyBB());
        generateRookMoves(board.getWhiteRooksBB(), board.getBlackBB(), board.getAllBB());
        generateKnightMoves(board.getWhiteKnightsBB(), board.getWhiteBB());
        generateBishopMoves(board.getWhiteBishopsBB(), board.getBlackBB(), board.getAllBB());
        generateQueenMoves(board.getWhiteQueensBB(), board.getBlackBB(), board.getAllBB());
        generateKingMoves(board.getWhiteKingBB(), board.getBlackBB(), board.getAllBB());
    }
    else {
        generatePawnMoves(board.getBlackPawnsBB(), board.getWhiteBB(), board.getEmptyBB());
        generateRookMoves(board.getBlackRooksBB(), board.getWhiteBB(), board.getAllBB());
        generateKnightMoves(board.getBlackKnightsBB(), board.getBlackBB());   
        generateBishopMoves(board.getBlackBishopsBB(), board.getWhiteBB(), board.getAllBB());
        generateQueenMoves(board.getBlackQueensBB(), board.getWhiteBB(), board.getAllBB());
        generateKingMoves(board.getBlackKingBB(), board.getWhiteBB(), board.getAllBB());
    }

    cout << "Number of pseudo legal moves generated: " << pseudoLegalMoves.size() << endl;

    // for(auto move : pseudoLegalMoves){
    //     move.printMove();
    // }
}

void MoveGenerator::generatePawnMoves(uint64_t friendlyPawns, uint64_t enemyPieces, uint64_t emptySquares){
    uint64_t startingRank = (board.isWhiteToPlay()) ? 0x00FF000000000000 : 0x000000000000FF00;
    uint64_t leftMask = (board.isWhiteToPlay()) ? 0x0101010101010101 : 0x8080808080808080; // Relative
    uint64_t rightMask = (board.isWhiteToPlay()) ? 0x8080808080808080 : 0x0101010101010101;
    int direction = (board.isWhiteToPlay()) ? 8 : -8;
    
    // Single push
    uint64_t singlePush;
    if(board.isWhiteToPlay())
        singlePush = (friendlyPawns >> 8) & emptySquares;
    else
        singlePush = (friendlyPawns << 8) & emptySquares;

    // Add the single push to the pseudo legal moves
    int toSquare = 0;
    while(singlePush){
        if(singlePush & 1)
            pseudoLegalMoves.push_back(Move(toSquare + direction, toSquare, board));
        
        singlePush >>= 1;
        toSquare++;
    }

    // Double push
    uint64_t doublePush;
    if(board.isWhiteToPlay())
        doublePush = ((friendlyPawns & startingRank) >> 16) & (emptySquares >> 8) & emptySquares;
    else
        doublePush = ((friendlyPawns & startingRank) << 16) & (emptySquares >> 8) & emptySquares;

    // Add the double push to the pseudo legal moves
    toSquare = 0;
    while(doublePush){
        if(doublePush & 1)
            pseudoLegalMoves.push_back(Move(toSquare + (2 * direction), toSquare, board));

        doublePush >>= 1;
        toSquare++;
    }

    // Left captures
    uint64_t leftCaptures;
    if(board.isWhiteToPlay())
        leftCaptures = (friendlyPawns & ~leftMask) >> 9 & enemyPieces;
    else
        leftCaptures = (friendlyPawns & ~leftMask) << 9 & enemyPieces;

    // Add the left capture to the pseudo legal moves
    toSquare = 0;
    while(leftCaptures){
        if(leftCaptures & 1)
            pseudoLegalMoves.push_back(Move(toSquare + direction + (direction % 7), toSquare, board));
        
        leftCaptures >>= 1;
        toSquare++;
    }

    // Right captures
    uint64_t rightCaptures;
    if(board.isWhiteToPlay())
        rightCaptures = (friendlyPawns & ~rightMask) >> 7 & enemyPieces;
    else
        rightCaptures = (friendlyPawns & ~rightMask) << 7 & enemyPieces;

    // Add the right capture to the pseudo legal moves
    toSquare = 0;
    while(rightCaptures){
        if(rightCaptures & 1)
            pseudoLegalMoves.push_back(Move(toSquare + direction - (direction % 7), toSquare, board));

        rightCaptures >>= 1;
        toSquare++;
    }

    // En passant capture
    if(board.getEnPassantSquare() != -1){
        int enPassantSquare = board.getEnPassantSquare();
        if(board.isWhiteToPlay()){
            // Left capture
            if((friendlyPawns & ~leftMask) >> 9 & (1ULL << enPassantSquare))
                pseudoLegalMoves.push_back(Move(enPassantSquare + 9, enPassantSquare, board));
            // Right capture
            if((friendlyPawns & ~rightMask) >> 7 & (1ULL << enPassantSquare))
                pseudoLegalMoves.push_back(Move(enPassantSquare + 7, enPassantSquare, board));
        }
        else {
            // Left capture
            if((friendlyPawns & ~leftMask) << 9 & (1ULL << enPassantSquare))
                pseudoLegalMoves.push_back(Move(enPassantSquare - 9, enPassantSquare, board));
            // Right capture
            if((friendlyPawns & ~rightMask) << 7 & (1ULL << enPassantSquare))
                pseudoLegalMoves.push_back(Move(enPassantSquare - 7, enPassantSquare, board));
        }
    }
}

void MoveGenerator::generateRookMoves(uint64_t friendlyRooks, uint64_t enemyPieces, uint64_t allPieces){
    uint64_t fileA = 0x0101010101010101;
    uint64_t fileH = 0x8080808080808080;

    int fromSquare = 0;
    while(friendlyRooks){
        if(friendlyRooks & 1){
            uint64_t rookMoves = 0ULL;
            uint8_t direction = 0b1111; 
            /*
                0b0001 = Up, 
                0b0010 = Down, 
                0b0100 = Left, 
                0b1000 = Right
            */ 
            int distance = 1;
            uint64_t currentMove;
            while(direction){
                // Up moves
                if(direction & 0b0001){
                    currentMove = (1ULL << fromSquare) >> (8 * distance);
                    rookMoves |= currentMove & ~allPieces;

                    if(currentMove & enemyPieces){
                        rookMoves |= currentMove & enemyPieces;
                        direction &= ~0b0001;
                    }
                    else if(currentMove & allPieces)
                        direction &= ~0b0001;
                }

                // Down moves
                if(direction & 0b0010){
                    currentMove = (1ULL << fromSquare) << (8 * distance);
                    rookMoves |= currentMove & ~allPieces;

                    if(currentMove & enemyPieces){
                        rookMoves |= currentMove & enemyPieces;
                        direction &= ~0b0010;
                    }
                    else if(currentMove & allPieces)
                        direction &= ~0b0010;
                }

                // Left moves
                if(direction & 0b0100){
                    if(fromSquare % 8 == 0)
                        direction &= ~0b0100;
                    else{
                        currentMove = (1ULL << fromSquare) >> distance;
                        rookMoves |= currentMove & ~allPieces;

                        if(currentMove & enemyPieces){
                            rookMoves |= currentMove & enemyPieces;
                            direction &= ~0b0100;
                        }
                        else if(currentMove & allPieces)
                            direction &= ~0b0100;
                        else if(currentMove & fileA)
                            direction &= ~0b0100;
                    }
                }

                // Right moves
                if(direction & 0b1000){
                    if(fromSquare % 8 == 7)
                        direction &= ~0b1000;
                    else{
                        currentMove = (1ULL << fromSquare) << distance;
                        rookMoves |= currentMove & ~allPieces;

                        if(currentMove & enemyPieces){
                            rookMoves |= currentMove & enemyPieces;
                            direction &= ~0b1000;
                        }
                        else if(currentMove & allPieces)
                            direction &= ~0b1000;
                        else if(currentMove & fileH)
                            direction &= ~0b1000;
                    }
                }

                distance++;
            }        

            addBBToMoves(fromSquare, rookMoves);
        }

        friendlyRooks >>= 1;
        fromSquare++;
    }
}

void MoveGenerator::generateKnightMoves(uint64_t friendlyKnights, uint64_t friendlyPieces){
    uint64_t notFileA = 0xFEFEFEFEFEFEFEFE;
    uint64_t notFileAB = 0xFCFCFCFCFCFCFCFC;
    uint64_t notFileH = 0x7F7F7F7F7F7F7F7F;
    uint64_t notFileGH = 0x3F3F3F3F3F3F3F3F;

    /*
       EXAMPLE BOARD
    ===================
    - x - x -   Top Top
    x - - - x   Top Mid
    - - K - -
    x - - - x   Bot Mid
    - x - x -   Bot Bot    
    */

    // Loop through knights
    int fromSquare = 0;

    while(friendlyKnights){
        if(friendlyKnights & 1){
            uint64_t knightMoves = 0ULL;
            // Top Top moves
            knightMoves |= ((1ULL << fromSquare) & notFileA) >> 17;
            knightMoves |= ((1ULL << fromSquare) & notFileH) >> 15;
            // Mid moves
            knightMoves |= ((1ULL << fromSquare) & notFileAB) >> 10;
            knightMoves |= ((1ULL << fromSquare) & notFileGH) >> 6;
            knightMoves |= ((1ULL << fromSquare) & notFileAB) << 6;
            knightMoves |= ((1ULL << fromSquare) & notFileGH) << 10;
            // Bottom Bottom moves
            knightMoves |= ((1ULL << fromSquare) & notFileA) << 15;
            knightMoves |= ((1ULL << fromSquare) & notFileH) << 17;

            knightMoves &= ~friendlyPieces;

            // Add the moves to the pseudo legal moves
            addBBToMoves(fromSquare, knightMoves);
        }

        friendlyKnights >>= 1;
        fromSquare++;
    }
}

void MoveGenerator::generateBishopMoves(uint64_t friendlyBishops, uint64_t enemyPieces, uint64_t allPieces){
    uint64_t fileA = 0x0101010101010101;
    uint64_t fileH = 0x8080808080808080;
    uint64_t rank1 = 0xFF00000000000000;
    uint64_t rank8 = 0x00000000000000FF;

    uint64_t allMoves = 0ULL;

    int fromSquare = 0;
    while(friendlyBishops){
        if(friendlyBishops & 1){
            uint64_t bishopMoves = 0ULL;
            uint8_t direction = 0b1111; 
            /*
                0b0001 = Up-Left, 
                0b0010 = Up-Right, 
                0b0100 = Down-Left,
                0b1000 = Down-Right
            */ 
            int distance = 1;
            uint64_t currentMove;
            while(direction){
                // Up-Left moves
                if(direction & 0b0001){
                    if(fromSquare % 8 == 0 || (1ULL << fromSquare) & rank8)
                        direction &= ~0b0001;
                    else{
                        currentMove = (1ULL << fromSquare) >> (9 * distance);
                        bishopMoves |= currentMove & ~allPieces;

                        if(currentMove & enemyPieces){
                            bishopMoves |= currentMove & enemyPieces;
                            direction &= ~0b0001;
                        }
                        else if(currentMove & allPieces || currentMove & fileA || currentMove & rank8)
                            direction &= ~0b0001;

                    }

                }

                // Up-Right moves
                if(direction & 0b0010){
                    if(fromSquare % 8 == 7 || (1ULL << fromSquare) & rank8)
                        direction &= ~0b0010;
                    else{
                        currentMove = (1ULL << fromSquare) >> (7 * distance);
                        bishopMoves |= currentMove & ~allPieces;

                        if(currentMove & enemyPieces){
                            bishopMoves |= currentMove & enemyPieces;
                            direction &= ~0b0010;
                        }
                        else if(currentMove & allPieces || currentMove & fileH || currentMove & rank8)
                            direction &= ~0b0010;
                    }
                }

                // Down-Left moves
                if(direction & 0b0100){
                    if(fromSquare % 8 == 0 || (1ULL << fromSquare) & rank1)
                        direction &= ~0b0100;
                    else{
                        currentMove = (1ULL << fromSquare) << (7 * distance);
                        bishopMoves |= currentMove & ~allPieces;

                        if(currentMove & enemyPieces){
                            bishopMoves |= currentMove & enemyPieces;
                            direction &= ~0b0100;
                        }
                        else if(currentMove & allPieces || currentMove & fileA || currentMove & rank1)
                            direction &= ~0b0100;
                    }                    
                }

                // Down-Right moves
                if(direction & 0b1000){
                    if(fromSquare % 8 == 7 || (1ULL << fromSquare) & rank1)
                        direction &= ~0b1000;
                    else{
                        currentMove = (1ULL << fromSquare) << (9 * distance);
                        bishopMoves |= currentMove & ~allPieces;

                        if(currentMove & enemyPieces){
                            bishopMoves |= currentMove & enemyPieces;
                            direction &= ~0b1000;
                        }
                        else if(currentMove & allPieces || currentMove & fileH || currentMove & rank1)
                            direction &= ~0b1000;
                    }
                }

                distance++;
            }       
            
            // Add the moves to the pseudo legal moves
            allMoves |= bishopMoves;
            addBBToMoves(fromSquare, bishopMoves);
        }

        friendlyBishops >>= 1;
        fromSquare++;
    }

    cout << "Bishop moves: " << endl;
    board.printBitBoard(allMoves);
}

void MoveGenerator::generateQueenMoves(uint64_t friendlyQueens, uint64_t enemyPieces, uint64_t allPieces){
    generateRookMoves(friendlyQueens, enemyPieces, allPieces);
    generateBishopMoves(friendlyQueens, enemyPieces, allPieces);
}

void MoveGenerator::generateKingMoves(uint64_t friendlyKing, uint64_t enemyPieces, uint64_t allPieces){
    uint64_t fileA = 0x0101010101010101;
    uint64_t fileH = 0x8080808080808080;    
    uint64_t kingMoves = 0ULL;

    uint64_t availableSquares = enemyPieces | ~allPieces;

    int fromSquare = 0;
    // Find King's position
    while(friendlyKing){
        if(friendlyKing & 1){
            uint64_t position = 1ULL << fromSquare;

            // Left moves, top to bottom
            kingMoves |= ((position & ~fileA) >> 9) & availableSquares;
            kingMoves |= ((position & ~fileA) >> 1) & availableSquares;
            kingMoves |= ((position & ~fileA) << 7) & availableSquares;

            // Vertical mocves
            kingMoves |= (position >> 8) & availableSquares;
            kingMoves |= (position << 8) & availableSquares;

            // Right moves, top to bottom
            kingMoves |= ((position & ~fileH) >> 7) & availableSquares;
            kingMoves |= ((position & ~fileH) << 1) & availableSquares;
            kingMoves |= ((position & ~fileH) << 9) & availableSquares;

            // Add moves to pseudo legal moves
            addBBToMoves(fromSquare, kingMoves);

            return; // There should only be one king on the board
        }

        fromSquare++;
        friendlyKing >>= 1;
    }
}

void MoveGenerator::addBBToMoves(int fromSquare, uint64_t movesBB){
    int toSquare = 0;
    while(movesBB){
        if(movesBB & 1)
            pseudoLegalMoves.push_back(Move(fromSquare, toSquare, board));
        movesBB >>= 1;
        toSquare++;
    }
}