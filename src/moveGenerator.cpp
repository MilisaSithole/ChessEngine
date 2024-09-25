#include "include/moveGenerator.h"

MoveGenerator::MoveGenerator(Board &board) : board(board) {
    // Set bitboards
    if(board.isWhiteToPlay()){
        myPawns = board.getWhitePawnsBB();
        myRooks = board.getWhiteRooksBB();
        myKnights = board.getWhiteKnightsBB();
        myBishops = board.getWhiteBishopsBB();
        myQueens = board.getWhiteQueensBB();
        myKing = board.getWhiteKingBB();
        myPieces = board.getWhiteBB();

        enemyPawns = board.getBlackPawnsBB();
        enemyRooks = board.getBlackRooksBB();
        enemyKnights = board.getBlackKnightsBB();
        enemyBishops = board.getBlackBishopsBB();
        enemyQueens = board.getBlackQueensBB();
        enemyKing = board.getBlackKingBB();
        enemyPieces = board.getBlackBB();
    }
    else{
        myPawns = board.getBlackPawnsBB();
        myRooks = board.getBlackRooksBB();
        myKnights = board.getBlackKnightsBB();
        myBishops = board.getBlackBishopsBB();
        myQueens = board.getBlackQueensBB();
        myKing = board.getBlackKingBB();
        myPieces = board.getBlackBB();

        enemyPawns = board.getWhitePawnsBB();
        enemyRooks = board.getWhiteRooksBB();
        enemyKnights = board.getWhiteKnightsBB();
        enemyBishops = board.getWhiteBishopsBB();
        enemyQueens = board.getWhiteQueensBB();
        enemyKing = board.getWhiteKingBB();
        enemyPieces = board.getWhiteBB();
    }
    allPieces = board.getAllBB();
    emptySquares = ~allPieces;
    generatedMoves = 0ULL;

    pinnedPieces = getPinnedPieces(myKing);

    pinners = getPinners(pinnedPieces, myKing);

    if(!pinners)
        pinnedPieces = 0ULL;

    cout << "Pinned pieces:" << endl;
    board.printBitBoard(pinnedPieces);
    cout << "Pinners:" << endl;
    board.printBitBoard(pinners);

    // Generate pseudo legal moves
    generateLegalMoves();

    cout << "Generated moves:" << endl;
    board.printBitBoard(generatedMoves);
}

void MoveGenerator::generateLegalMoves(){
    kingDangerSquares = getAttackedSquares(true);
    attackedSquares = getAttackedSquares(false);
    
    checkers = getAttackers(myKing);

    updateNumCheckers();
    updateCheckMasks();

    generatePseudoLegalMoves();
}

void MoveGenerator::generatePseudoLegalMoves(){
    generateKingMoves(myKing, enemyPieces, allPieces);
    if(numCheckers < 2){
        generatePawnMoves(myPawns, enemyPieces, emptySquares);
        generateRookMoves(myRooks, enemyPieces, allPieces);
        generateKnightMoves(myKnights, myPieces);
        generateBishopMoves(myBishops, enemyPieces, allPieces);
        generateQueenMoves(myQueens, enemyPieces, allPieces);
    }

    cout << "Number of pseudo legal moves generated: " << pseudoLegalMoves.size() << endl;
    for(auto move : pseudoLegalMoves){
        move.printMove();
    }
}

void MoveGenerator::generatePawnMoves(uint64_t friendlyPawns, uint64_t enemyPieces, uint64_t emptySquares){
    uint64_t startingRank = (board.isWhiteToPlay()) ? 0x00FF000000000000 : 0x000000000000FF00;
    uint64_t leftMask = (board.isWhiteToPlay()) ? 0x0101010101010101 : 0x8080808080808080; // Relative
    uint64_t rightMask = (board.isWhiteToPlay()) ? 0x8080808080808080 : 0x0101010101010101;
    int direction = (board.isWhiteToPlay()) ? 8 : -8;
    
    // NON PINNED PAWNS
    // Single push
    uint64_t singlePush;
    if(board.isWhiteToPlay())
        singlePush = ((friendlyPawns & ~pinnedPieces) >> 8) & emptySquares;
    else
        singlePush = ((friendlyPawns & ~pinnedPieces) << 8) & emptySquares;

    singlePush &= (captureMask | blockMask); // In case in check
    generatedMoves |= singlePush;

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
        doublePush = (((friendlyPawns & ~pinnedPieces) & startingRank) >> 16) & (emptySquares >> 8) & emptySquares;
    else
        doublePush = (((friendlyPawns & ~pinnedPieces) & startingRank) << 16) & (emptySquares >> 8) & emptySquares;

    doublePush &= (captureMask | blockMask); // In case in check
    generatedMoves |= doublePush;

    cout << "Before double push" << endl;
    board.printBitBoard(friendlyPawns & ~pinnedPieces & startingRank);
    cout << "After double push" << endl;
    board.printBitBoard(doublePush);

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
        leftCaptures = ((friendlyPawns & ~pinnedPieces) & ~leftMask) >> 9 & enemyPieces;
    else
        leftCaptures = ((friendlyPawns & ~pinnedPieces) & ~leftMask) << 9 & enemyPieces;

    leftCaptures &= (captureMask | blockMask); // In caase in check
    generatedMoves |= leftCaptures;

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
        rightCaptures = ((friendlyPawns & ~pinnedPieces) & ~rightMask) >> 7 & enemyPieces;
    else
        rightCaptures = ((friendlyPawns & ~pinnedPieces) & ~rightMask) << 7 & enemyPieces;

    rightCaptures &= (captureMask | blockMask); // In case in check
    generatedMoves |= rightCaptures;

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
            if(((friendlyPawns & ~pinnedPieces) & ~leftMask) >> 9 & (1ULL << enPassantSquare) & (captureMask | blockMask))
                if(!checkForEnPassantDiscoveredCheck(1ULL << (enPassantSquare + 9), 1ULL << (enPassantSquare + 8))){
                    pseudoLegalMoves.push_back(Move(enPassantSquare + 9, enPassantSquare, board));
                    generatedMoves |= ((friendlyPawns & ~pinnedPieces) & ~leftMask) >> 9 & (1ULL << enPassantSquare) & (captureMask | blockMask);
                }
            // Right capture
            if(((friendlyPawns & ~pinnedPieces) & ~rightMask) >> 7 & (1ULL << enPassantSquare) & (captureMask | blockMask))
                if(!checkForEnPassantDiscoveredCheck(1ULL << (enPassantSquare + 7), 1ULL << (enPassantSquare + 8))){
                    pseudoLegalMoves.push_back(Move(enPassantSquare + 7, enPassantSquare, board));
                    generatedMoves |= ((friendlyPawns & ~pinnedPieces) & ~rightMask) >> 7 & (1ULL << enPassantSquare) & (captureMask | blockMask);
                }
        }
        else {
            // Left capture
            if(((friendlyPawns & ~pinnedPieces) & ~leftMask) << 9 & (1ULL << enPassantSquare) & (captureMask | blockMask))
                if(!checkForEnPassantDiscoveredCheck(1ULL << (enPassantSquare - 9), 1ULL << (enPassantSquare - 8))){
                    pseudoLegalMoves.push_back(Move(enPassantSquare - 9, enPassantSquare, board));
                    generatedMoves |= ((friendlyPawns & ~pinnedPieces) & ~leftMask) << 9 & (1ULL << enPassantSquare) & (captureMask | blockMask);
                }
            // Right capture
            if(((friendlyPawns & ~pinnedPieces) & ~rightMask) << 7 & (1ULL << enPassantSquare) & (captureMask | blockMask))
                if(!checkForEnPassantDiscoveredCheck(1ULL << (enPassantSquare - 7), 1ULL << (enPassantSquare - 8))){
                    pseudoLegalMoves.push_back(Move(enPassantSquare - 7, enPassantSquare, board));
                    generatedMoves |= ((friendlyPawns & ~pinnedPieces) & ~rightMask) << 7 & (1ULL << enPassantSquare) & (captureMask | blockMask);
                }
        }
    }

    // PINNED PAWNS
    if(friendlyPawns & pinnedPieces){
        uint64_t pinnedPawns = friendlyPawns & pinnedPieces;
        int pinnedIdx = 0;

        while(pinnedPawns){
            if(pinnedPawns & 1){
                // Get pinned ray
                uint64_t pinnedRay = getPinnedRays(1ULL << pinnedIdx, pinners, myKing);

                // Single push
                if(board.isWhiteToPlay())
                    singlePush = ((1ULL << pinnedIdx) >> 8) & emptySquares;
                else
                    singlePush = ((1ULL << pinnedIdx) << 8) & emptySquares;

                pseudoLegalMoves.push_back(Move(pinnedIdx, pinnedIdx - direction, board));
                generatedMoves |= singlePush & pinnedRay;

                // Double push
                if(board.isWhiteToPlay())
                    doublePush = (((1ULL << pinnedIdx) & startingRank) >> 16) & (emptySquares >> 8) & emptySquares;
                else
                    doublePush = (((1ULL << pinnedIdx) & startingRank) << 16) & (emptySquares >> 8) & emptySquares;

                pseudoLegalMoves.push_back(Move(pinnedIdx, pinnedIdx - (2 * direction), board));
                generatedMoves |= doublePush & pinnedRay;

                // Left capture
                if(board.isWhiteToPlay())
                    leftCaptures = ((1ULL << pinnedIdx) & ~leftMask) >> 9 & enemyPieces;
                else
                    leftCaptures = ((1ULL << pinnedIdx) & ~leftMask) << 9 & enemyPieces;

                pseudoLegalMoves.push_back(Move(pinnedIdx, pinnedIdx - direction - (direction % 7), board));
                generatedMoves |= leftCaptures & pinnedRay;

                // Right capture
                if(board.isWhiteToPlay())
                    rightCaptures = ((1ULL << pinnedIdx) & ~rightMask) >> 7 & enemyPieces;
                else
                    rightCaptures = ((1ULL << pinnedIdx) & ~rightMask) << 7 & enemyPieces;

                pseudoLegalMoves.push_back(Move(pinnedIdx, pinnedIdx - direction + (direction % 7), board));
                generatedMoves |= rightCaptures; // & pinnedRay;

                // En passant capture
                if(board.getEnPassantSquare() != -1){
                    int enPassantSquare = board.getEnPassantSquare();
                    if(board.isWhiteToPlay()){
                        // Left capture
                        if(((1ULL << pinnedIdx) & ~leftMask) >> 9 & (1ULL << enPassantSquare) & pinnedRay)
                            pseudoLegalMoves.push_back(Move(enPassantSquare + 9, enPassantSquare, board));
                        // Right capture
                        if(((1ULL << pinnedIdx) & ~rightMask) >> 7 & (1ULL << enPassantSquare) & pinnedRay)
                            pseudoLegalMoves.push_back(Move(enPassantSquare + 7, enPassantSquare, board));

                        generatedMoves |= ((1ULL << pinnedIdx) & ~leftMask) >> 9 & (1ULL << enPassantSquare) & pinnedRay;
                        generatedMoves |= ((1ULL << pinnedIdx) & ~rightMask) >> 7 & (1ULL << enPassantSquare) & pinnedRay;
                    }
                    else {
                        // Left capture
                        if(((1ULL << pinnedIdx) & ~leftMask) << 9 & (1ULL << enPassantSquare) & pinnedRay)
                            pseudoLegalMoves.push_back(Move(enPassantSquare - 9, enPassantSquare, board));
                        // Right capture
                        if(((1ULL << pinnedIdx) & ~rightMask) << 7 & (1ULL << enPassantSquare) & pinnedRay)
                            pseudoLegalMoves.push_back(Move(enPassantSquare - 7, enPassantSquare, board));

                        generatedMoves |= ((1ULL << pinnedIdx) & ~leftMask) << 9 & (1ULL << enPassantSquare) & pinnedRay;
                        generatedMoves |= ((1ULL << pinnedIdx) & ~rightMask) << 7 & (1ULL << enPassantSquare) & pinnedRay;
                    }
                }
            }

            pinnedPawns >>= 1;
            pinnedIdx++;
        }

    }
}

bool MoveGenerator::checkForEnPassantDiscoveredCheck(uint64_t movingPawn, uint64_t capturedPawn){
    // TODO
    uint64_t fileA = 0x0101010101010101;
    uint64_t fileH = 0x8080808080808080;
    uint64_t emptyFilter = emptySquares | movingPawn | capturedPawn;

    int distance = 0;
 
    // Check left of king
    if(!(myKing & fileA)){
        do{
            distance++;
            if((myKing >> distance) & (enemyQueens | enemyRooks))
                return true;
            else if(!((myKing >> distance) & emptyFilter))
                break;
        } while(!((myKing >> distance) & fileA));
    }
    distance = 0;
    // Check right of king
    if(!(myKing & fileH)){
        do{
            distance++;
            if((myKing << distance) & (enemyQueens | enemyRooks))
                return true;
            else if(!((myKing << distance) & emptyFilter))
                break;
        } while(!((myKing << distance) & fileH));
    }

    return false;

}

void MoveGenerator::generateRookMoves(uint64_t friendlyRooks, uint64_t enemyPieces, uint64_t allPieces){
    uint64_t fileA = 0x0101010101010101;
    uint64_t fileH = 0x8080808080808080;

    int fromSquare = 0;
    while(friendlyRooks){
        if(friendlyRooks & 1){
            bool isPinned = (1ULL << fromSquare) & pinnedPieces;
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

            if(isPinned){
                uint64_t pinnedRay = getPinnedRays(1ULL << fromSquare, pinners, myKing);
                rookMoves &= pinnedRay;
            }
            else
                // In case in check
                rookMoves &= (captureMask | blockMask);

            // Add moves to pseudoLegalMoves
            generatedMoves |= rookMoves;
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
            bool isPinned = (1ULL << fromSquare) & pinnedPieces;
            if(!isPinned){
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
                // Remove moves that would capture friendly pieces
                knightMoves &= ~friendlyPieces;

                // In case in check
                knightMoves &= (captureMask | blockMask);

                // Add the moves to the pseudo legal moves
                generatedMoves |= knightMoves;
                addBBToMoves(fromSquare, knightMoves);

            }
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

    int fromSquare = 0;
    while(friendlyBishops){
        if(friendlyBishops & 1){
            bool isPinned = (1ULL << fromSquare) & pinnedPieces;
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

            if(isPinned){
                uint64_t pinnedRay = getPinnedRays(1ULL << fromSquare, pinners, myKing);
                bishopMoves &= pinnedRay;
            }
            else
                // In case in check
                bishopMoves &= (captureMask | blockMask);
            
            // Add the moves to the pseudo legal moves
            generatedMoves |= bishopMoves;
            addBBToMoves(fromSquare, bishopMoves);
        }

        friendlyBishops >>= 1;
        fromSquare++;
    }
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

            // Vertical moves
            kingMoves |= (position >> 8) & availableSquares;
            kingMoves |= (position << 8) & availableSquares;

            // Right moves, top to bottom
            kingMoves |= ((position & ~fileH) >> 7) & availableSquares;
            kingMoves |= ((position & ~fileH) << 1) & availableSquares;
            kingMoves |= ((position & ~fileH) << 9) & availableSquares;

            // Remove attacked squares rom moves
            kingMoves &= ~kingDangerSquares;

            // Add moves to pseudo legal moves
            generatedMoves |= kingMoves;
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



uint64_t MoveGenerator::getAttackedSquares(bool kingDanger){
    uint64_t attackedSquares = 0ULL;

    attackedSquares |= getPawnAttacks(enemyPawns);
    attackedSquares |= getRookAttacks(enemyRooks, kingDanger);
    attackedSquares |= getKnightAttacks(enemyKnights);
    attackedSquares |= getBishopAttacks(enemyBishops, kingDanger);
    attackedSquares |= getQueenAttacks(enemyQueens, kingDanger);
    attackedSquares |= getKingAttacks(enemyKing);

    return attackedSquares;
}

uint64_t MoveGenerator::getPawnAttacks(uint64_t oppPawns){
    uint64_t leftMask = (!board.isWhiteToPlay()) ? 0x0101010101010101 : 0x8080808080808080;
    uint64_t rightMask = (!board.isWhiteToPlay()) ? 0x8080808080808080 : 0x0101010101010101;

    uint64_t pawnAttacks = 0ULL;

    // Left captures
    if(board.isWhiteToPlay())
        pawnAttacks |= (oppPawns & ~leftMask) << 9 & (enemyPieces | emptySquares);
    else
        pawnAttacks |= (oppPawns & ~leftMask) >> 9 & (enemyPieces | emptySquares);

    // Right captures
    if(board.isWhiteToPlay())
        pawnAttacks |= (oppPawns & ~rightMask) << 7 & (enemyPieces | emptySquares);
    else
        pawnAttacks |= (oppPawns & ~rightMask) >> 7 & (enemyPieces | emptySquares);

    // En Passant captures
    int enPassantSquare = board.getPrevEnPassantSquare();
    if(enPassantSquare != -1){
        if(board.isWhiteToPlay()){
            pawnAttacks |= (oppPawns & ~leftMask) << 9 & (1ULL << enPassantSquare);
            pawnAttacks |= (oppPawns & ~rightMask) << 7 & (1ULL << enPassantSquare);
        }
        else{
            pawnAttacks |= (oppPawns & ~leftMask) >> 9 & (1ULL << enPassantSquare);
            pawnAttacks |= (oppPawns & ~rightMask) >> 7 & (1ULL << enPassantSquare);
        }
    }

    return pawnAttacks;
}

uint64_t MoveGenerator::getRookAttacks(uint64_t oppRooks, bool kingDanger, uint64_t filterOut){
    uint64_t fileA = 0x0101010101010101;
    uint64_t fileH = 0x8080808080808080;

    uint64_t emptyAndKing = emptySquares | filterOut;
    uint64_t allButKing = allPieces & ~filterOut;
    uint64_t mineButKing = myPieces & ~filterOut;
    if(kingDanger){
        emptyAndKing |= myKing;
        allButKing &= ~myKing;
        mineButKing &= ~myKing;
    }

    uint64_t rookAttacks = 0ULL;

    int fromSquare = 0;
    while(oppRooks){
        if(oppRooks & 1){
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
                    rookAttacks |= currentMove & emptyAndKing;

                    if(currentMove & (mineButKing | enemyPieces)){
                        rookAttacks |= currentMove & (mineButKing | enemyPieces);
                        direction &= ~0b0001;
                    }
                    else if(currentMove & allButKing)
                        direction &= ~0b0001;
                }

                // Down moves
                if(direction & 0b0010){
                    currentMove = (1ULL << fromSquare) << (8 * distance);
                    rookAttacks |= currentMove & emptyAndKing;

                    if(currentMove & (mineButKing | enemyPieces)){
                        rookAttacks |= currentMove & (mineButKing | enemyPieces);
                        direction &= ~0b0010;
                    }
                    else if(currentMove & allButKing)
                        direction &= ~0b0010;
                }

                // Left moves
                if(direction & 0b0100){
                    if(fromSquare % 8 == 0)
                        direction &= ~0b0100;
                    else{
                        currentMove = (1ULL << fromSquare) >> distance;
                        rookAttacks |= currentMove & emptyAndKing;

                        if(currentMove & (mineButKing | enemyPieces)){
                            rookAttacks |= currentMove & (mineButKing | enemyPieces);
                            direction &= ~0b0100;
                        }
                        else if(currentMove & allButKing)
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
                        rookAttacks |= currentMove & emptyAndKing;

                        if(currentMove & (mineButKing | enemyPieces)){
                            rookAttacks |= currentMove & (mineButKing | enemyPieces);
                            direction &= ~0b1000;
                        }
                        else if(currentMove & allButKing)
                            direction &= ~0b1000;
                        else if(currentMove & fileH)
                            direction &= ~0b1000;
                    }
                }
                distance++;
            }
            rookAttacks ^= 1ULL << fromSquare;
        }

        oppRooks >>= 1;
        fromSquare++;
    }

    return rookAttacks;
}

uint64_t MoveGenerator::getKnightAttacks(uint64_t oppKnights){
    uint64_t notFileA = 0xFEFEFEFEFEFEFEFE;
    uint64_t notFileAB = 0xFCFCFCFCFCFCFCFC;
    uint64_t notFileH = 0x7F7F7F7F7F7F7F7F;
    uint64_t notFileGH = 0x3F3F3F3F3F3F3F3F;

    uint64_t knightAttacks = 0ULL;

    // Top Top moves
    knightAttacks |= (oppKnights & notFileA) >> 17;
    knightAttacks |= (oppKnights & notFileH) >> 15;
    // Mid moves
    knightAttacks |= (oppKnights & notFileAB) >> 10;
    knightAttacks |= (oppKnights & notFileGH) >> 6;
    knightAttacks |= (oppKnights & notFileAB) << 6;
    knightAttacks |= (oppKnights & notFileGH) << 10;
    // Bottom Bottom moves
    knightAttacks |= (oppKnights & notFileA) << 15;
    knightAttacks |= (oppKnights & notFileH) << 17;

    return knightAttacks;
}

uint64_t MoveGenerator::getBishopAttacks(uint64_t oppBishops, bool kingDanger, uint64_t filterOut){
    uint64_t fileA = 0x0101010101010101;
    uint64_t fileH = 0x8080808080808080;
    uint64_t rank1 = 0xFF00000000000000;
    uint64_t rank8 = 0x00000000000000FF;

    uint64_t emptyAndKing = emptySquares | filterOut;
    uint64_t allButKing = allPieces & ~filterOut;
    uint64_t mineButKing = myPieces & ~filterOut;
    if(kingDanger){
        emptyAndKing |= myKing;
        allButKing &= ~myKing;
        mineButKing &= ~myKing;
    }

    uint64_t bishopAttacks = 0ULL;

    int fromSquare = 0;
    while(oppBishops){
        if(oppBishops & 1){
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
                        bishopAttacks |= currentMove & emptyAndKing;

                        if(currentMove & (mineButKing | enemyPieces)){
                            bishopAttacks |= currentMove & (mineButKing | enemyPieces);
                            direction &= ~0b0001;
                        }
                        else if(currentMove & allButKing || currentMove & fileA || currentMove & rank8)
                            direction &= ~0b0001;
                    }
                }

                // Up-Right moves
                if(direction & 0b0010){
                    if(fromSquare % 8 == 7 || (1ULL << fromSquare) & rank8)
                        direction &= ~0b0010;
                    else{
                        currentMove = (1ULL << fromSquare) >> (7 * distance);
                        bishopAttacks |= currentMove & emptyAndKing;

                        if(currentMove & (mineButKing | enemyPieces)){
                            bishopAttacks |= currentMove & (mineButKing | enemyPieces);
                            direction &= ~0b0010;
                        }
                        else if(currentMove & allButKing || currentMove & fileH || currentMove & rank8)
                            direction &= ~0b0010;
                    }
                }

                // Down-Left moves
                if(direction & 0b0100){
                    if(fromSquare % 8 == 0 || (1ULL << fromSquare) & rank1)
                        direction &= ~0b0100;
                    else{
                        currentMove = (1ULL << fromSquare) << (7 * distance);
                        bishopAttacks |= currentMove & emptyAndKing;

                        if(currentMove & (mineButKing | enemyPieces)){
                            bishopAttacks |= currentMove & (mineButKing | enemyPieces);
                            direction &= ~0b0100;
                        }
                        else if(currentMove & allButKing || currentMove & fileA || currentMove & rank1)
                            direction &= ~0b0100;
                    }                    
                }

                // Down-Right moves
                if(direction & 0b1000){
                    if(fromSquare % 8 == 7 || (1ULL << fromSquare) & rank1)
                        direction &= ~0b1000;
                    else{
                        currentMove = (1ULL << fromSquare) << (9 * distance);
                        bishopAttacks |= currentMove & emptyAndKing;

                        if(currentMove & (mineButKing | enemyPieces)){
                            bishopAttacks |= currentMove & (mineButKing | enemyPieces);
                            direction &= ~0b1000;
                        }
                        else if(currentMove & allButKing || currentMove & fileH || currentMove & rank1)
                            direction &= ~0b1000;
                    }
                }
                distance++;
            }
        }

        oppBishops >>= 1;
        fromSquare++;
    }

    return bishopAttacks;
}

uint64_t MoveGenerator::getQueenAttacks(uint64_t oppQueens, bool kingDanger, uint64_t filterOut){
    uint64_t queenAttacks = 0ULL;

    queenAttacks |= getRookAttacks(oppQueens, kingDanger, filterOut);
    queenAttacks |= getBishopAttacks(oppQueens, kingDanger, filterOut);

    return queenAttacks;
}

uint64_t MoveGenerator::getKingAttacks(uint64_t oppKing){
    uint64_t fileA = 0x0101010101010101;
    uint64_t fileH = 0x8080808080808080;
    
    uint64_t kingAttacks = 0ULL;

    // Left moves, top to bottom
    kingAttacks |= (oppKing & ~fileA) >> 9;
    kingAttacks |= (oppKing & ~fileA) >> 1;
    kingAttacks |= (oppKing & ~fileA) << 7;

    // Vertical moves
    kingAttacks |= oppKing >> 8;
    kingAttacks |= oppKing << 8;

    // Right moves, top to bottom
    kingAttacks |= (oppKing & ~fileH) >> 7;
    kingAttacks |= (oppKing & ~fileH) << 1;
    kingAttacks |= (oppKing & ~fileH) << 9;

    return kingAttacks;
}



uint64_t MoveGenerator::getAttackers(uint64_t piece){
    uint64_t attackers = 0ULL;

    // Pretend to be the piece
    attackers |= getPawnAttackers(piece);
    attackers |= getRookAttacks(piece, false) & enemyRooks;
    attackers |= getKnightAttacks(piece) & enemyKnights;
    attackers |= getBishopAttacks(piece, false) & enemyBishops;
    attackers |= getQueenAttacks(piece, false) & enemyQueens;

    return attackers;
}

uint64_t MoveGenerator::getPawnAttackers(uint64_t piece){
    uint64_t leftMask = (board.isWhiteToPlay()) ? 0x0101010101010101 : 0x8080808080808080; // Relative
    uint64_t rightMask = (board.isWhiteToPlay()) ? 0x8080808080808080 : 0x0101010101010101;

    uint64_t attackers = 0ULL;

    if(board.isWhiteToPlay()){
        attackers |= (piece & ~leftMask) >> 9;
        attackers |= (piece & ~rightMask) >> 7;
    }
    else{
        attackers |= (piece & ~leftMask) << 9;
        attackers |= (piece & ~rightMask) << 7;
    }

    attackers &= enemyPawns;

    return attackers;
}

uint64_t MoveGenerator::getAttackerRay(uint64_t piece, uint64_t attackType){
    uint64_t ray = 0ULL;

    ray |= getRookAttackerRay(piece, enemyRooks, attackType);
    ray |= getRookAttackerRay(piece, enemyQueens, attackType);
    ray |= getBishopAttackerRay(piece, enemyBishops, attackType);
    ray |= getBishopAttackerRay(piece, enemyQueens, attackType);

    return ray;
}

uint64_t MoveGenerator::getRookAttackerRay(uint64_t piece, uint64_t attackers, uint64_t attackType){
    uint64_t attacker = attackers;
    if(attackType == checkers)
        attacker &= checkers;
    uint64_t ray = 0ULL;
    
    if(attacker){
        // Get attacker and piece squares
        int attackerSquare = -1;
        int pieceSquare = -1;

        uint64_t attackerCopy = attacker;
        while(attackerCopy){
            attackerSquare++;
            attackerCopy >>= 1;
        }

        uint64_t pieceCopy = piece;
        while(pieceCopy){
            pieceSquare++;
            pieceCopy >>= 1;
        }

        // Check if in same file or rank
        if((attackerSquare % 8 != pieceSquare % 8) && (attackerSquare / 8 != pieceSquare / 8))
            return ray;

        int distance = 1;

        // Check up
        if(piece <= (attacker >> 8)){
            while((ray & (piece << 8)) == 0){
                ray |= attacker >> (8 * distance);
                distance++;
            }
        }
        // Check down
        else if(piece >= (attacker << 8)){
            while((ray & (piece >> 8)) == 0){
                ray |= attacker << (8 * distance);
                distance++;
            }
        }
        // Check left
        else if(piece <= (attacker >> 1)){
            while((ray & (attacker >> 1)) == 0){
                ray |= piece << distance;
                distance++;
            }
        }
        // Check right
        else if(piece >= (attacker << 1)){
            while((ray & (attacker << 1)) == 0){
                ray |= piece >> distance;
                distance++;
            }
        }
    }

    return ray;
}

uint64_t MoveGenerator::getBishopAttackerRay(uint64_t piece, uint64_t attackers, uint64_t attackType){
    uint64_t attacker = attackType & attackers;
    uint64_t ray = 0ULL;

    if(attacker){
        int attackerSquare = -1;
        int pieceSquare = -1;

        uint64_t attackerCopy = attacker;
        while(attackerCopy){
            attackerSquare++;
            attackerCopy >>= 1;
        }

        uint64_t pieceCopy = piece;
        while(pieceCopy){
            pieceSquare++;
            pieceCopy >>= 1;
        }

        // Check if in same diagonal
        if(abs(attackerSquare % 8 - pieceSquare % 8) != abs(attackerSquare / 8 - pieceSquare / 8))
            return ray;

        int distance = 1;

        // Check up left
        if(piece < attacker && (pieceSquare % 8 < attackerSquare % 8)){
            while((ray & (attacker >> 9)) == 0){
                ray |= piece << (9 * distance);
                distance++;
            }
        }
        // Check up right
        else if(piece < attacker && (pieceSquare % 8 > attackerSquare % 8)){
            while((ray & (attacker >> 7)) == 0){
                ray |= piece << (7 * distance);
                distance++;
            }
        }
        // Check down left
        else if(piece > attacker && (pieceSquare % 8 < attackerSquare % 8)){
            while((ray & (attacker << 7)) == 0){
                ray |= piece >> (7 * distance);
                distance++;
            }
        }
        // Check down right
        else if(piece > attacker && (pieceSquare % 8 > attackerSquare % 8)){
            while((ray & (attacker << 9)) == 0){
                ray |= piece >> (9 * distance);
                distance++;
            }
        }        
    }
        
    return ray;
}



uint64_t MoveGenerator::getPinnedPieces(uint64_t piece){
    uint64_t pieceRays = 0ULL;
    pieceRays |= getQueenAttacks(piece, false);

    cout << "King rays for pins:" << endl;
    board.printBitBoard(pieceRays);

    uint64_t sliderAttackerRays = 0ULL;
    sliderAttackerRays |= getRookAttacks(enemyRooks, false);
    sliderAttackerRays |= getBishopAttacks(enemyBishops, false);
    sliderAttackerRays |= getQueenAttacks(enemyQueens, false);

    cout << "Slider attacker rays for pins:" << endl;
    board.printBitBoard(sliderAttackerRays);

    return pieceRays & sliderAttackerRays & myPieces;
}

uint64_t MoveGenerator::getPinners(uint64_t pinned, uint64_t piece){
    if(!pinned)
        return 0ULL;

    uint64_t pinners = 0ULL;

    // Use 'Piece' a slider, filtering out pinned pieces to get attack moves
    pinners |= getQueenAttacks(piece, false, pinned);

    // And with opp sliders
    pinners &= enemyRooks | enemyBishops | enemyQueens;

    return pinners;
}

uint64_t MoveGenerator::getPinnedRays(uint64_t pinned, uint64_t pinners, uint64_t piece){
    if(!pinned) 
        return 0xFFFFFFFFFFFFFFFF;

    uint64_t pinnedRay = 0ULL;
    uint64_t pinnersCopy = pinners;
    
    // Loop through pinners
    int fromSquare = 0;
    while(pinnersCopy){
        if(pinnersCopy & 1){
            // Get pinner ray to piece
            if((1ULL << fromSquare) & enemyRooks)
                pinnedRay |= getRookAttackerRay(piece, 1ULL << fromSquare, pinners);
            else if((1ULL << fromSquare) & enemyBishops)
                pinnedRay |= getBishopAttackerRay(piece, 1ULL << fromSquare, pinners);
            else if((1ULL << fromSquare) & enemyQueens){
                pinnedRay |= getRookAttackerRay(piece, 1ULL << fromSquare, pinners);
                pinnedRay |= getBishopAttackerRay(piece, 1ULL << fromSquare, pinners);
            }

            if(pinnedRay & pinned)
                return pinnedRay | (1ULL << fromSquare);
            else
                pinnedRay = 0ULL;
        }

        pinnersCopy >>= 1;
        fromSquare++;
    }

    return 0xFFFFFFFFFFFFFFFF;
}

void MoveGenerator::updateNumCheckers(){
    numCheckers = 0;

    uint64_t attackers = checkers;

    while(attackers){
        numCheckers += attackers & 1;
        attackers >>= 1;
    }
}

void MoveGenerator::updateCheckMasks(){
    if(numCheckers == 0){
        captureMask = 0xFFFFFFFFFFFFFFFF;
        blockMask = 0xFFFFFFFFFFFFFFFF;
    }
    else if(numCheckers == 1){
        captureMask = checkers;
        if(board.getEnPassantSquare() != -1){
            captureMask |= (1ULL << board.getEnPassantSquare());
        }
        blockMask = getAttackerRay(myKing, checkers);
    }
    else{
        captureMask = 0ULL;
        blockMask = 0ULL;
    }
}