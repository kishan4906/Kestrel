#include "UCI.h"
#include "MoveGenerator.h"
#include "Search.h"
#include <iostream>
#include <sstream>
#include <vector>

Board UCI::board;

static const std::string START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

static Square squareFromString(const std::string& s) {
    int file = s[0] - 'a';
    int rank = s[1] - '1';
    return makeSquare(file, rank);
}

static PieceType promoFromChar(char c) {
    switch (c) {
        case 'q': return QUEEN;
        case 'r': return ROOK;
        case 'b': return BISHOP;
        case 'n': return KNIGHT;
        default:  return NONE;
    }
}

// Parses a UCI move string like "e2e4" or "e7e8q" and finds the matching
// LEGAL move (with its correct flag — capture, en passant, castle, promotion)
// from the current position. Returns false if no legal move matches, which
// signals a bug upstream (GUI desyncing) or a genuinely illegal move sent in.
static bool findMatchingMove(const Board& board, const std::string& uciMove, Move& out) {
    Square from = squareFromString(uciMove.substr(0, 2));
    Square to = squareFromString(uciMove.substr(2, 2));
    PieceType promo = (uciMove.size() >= 5) ? promoFromChar(uciMove[4]) : NONE;

    for (const Move& m : MoveGenerator::generateLegalMoves(board)) {
        if (m.from == from && m.to == to) {
            bool isPromo = (m.flag == PROMOTION || m.flag == PROMOTION_CAPTURE);
            if (isPromo && m.promotion != promo) continue; // wrong promotion piece, keep looking
            out = m;
            return true;
        }
    }
    return false;
}

void UCI::handleUCI() {
    std::cout << "id name Kestrel\n";
    std::cout << "id author Kishan Kushwaha\n";
    std::cout << "uciok\n" << std::flush;
}

void UCI::handleIsReady() {
    std::cout << "readyok\n" << std::flush;
}

void UCI::handlePosition(const std::string& line) {
    std::istringstream iss(line);
    std::string token;
    iss >> token; // consume "position"
    iss >> token; // "startpos" or "fen"

    if (token == "startpos") {
        board.loadFEN(START_FEN);
    } else if (token == "fen") {
        // FEN is 6 space-separated fields — reassemble them from the stream.
        std::string fen, field;
        for (int i = 0; i < 6 && iss >> field; i++) {
            fen += field;
            if (i < 5) fen += ' ';
        }
        board.loadFEN(fen);
    }

    // Optional trailing "moves e2e4 e7e5 ..." — replay each move onto the board.
    if (iss >> token && token == "moves") {
        std::string moveStr;
        while (iss >> moveStr) {
            Move m;
            if (findMatchingMove(board, moveStr, m)) {
                board.makeMove(m);
            }
            // If a move doesn't match, silently skip it rather than crash —
            // a desynced GUI shouldn't take down the engine process.
        }
    }
}

void UCI::handleGo(const std::string& line) {
    std::istringstream iss(line);
    std::string token;
    iss >> token; // consume "go"

    int movetime = -1;
    int depth = -1;
    int wtime = -1, btime = -1;

    while (iss >> token) {
        if (token == "movetime") iss >> movetime;
        else if (token == "depth") iss >> depth;
        else if (token == "wtime") iss >> wtime;
        else if (token == "btime") iss >> btime;
        // winc/binc/moves-to-go and other tokens are read past but not used yet.
    }

    SearchResult result;

    if (depth > 0) {
        // Fixed depth — used mainly for engine-strength testing, no time limit.
        result = Search::findBestMove(board, depth);
    } else if (movetime > 0) {
        result = Search::findBestMoveTimed(board, 20, movetime);
    } else if (wtime > 0 || btime > 0) {
        // Simple time management: spend roughly 1/30th of our remaining
        // clock on this move. Not tournament-grade, but keeps the engine
        // from flagging (running out of time) in a timed GUI game.
        int myTime = (board.sideToMove == WHITE) ? wtime : btime;
        int budget = (myTime > 0) ? myTime / 30 : 2000;
        result = Search::findBestMoveTimed(board, 20, budget);
    } else {
        // No time info given at all — fall back to a fixed 2-second think.
        result = Search::findBestMoveTimed(board, 20, 2000);
    }

    std::cout << "bestmove " << moveToString(result.bestMove) << "\n" << std::flush;
}

void UCI::loop() {
    board.loadFEN(START_FEN);
    std::string line;

    while (std::getline(std::cin, line)) {
        if (line == "uci") {
            handleUCI();
        } else if (line == "isready") {
            handleIsReady();
        } else if (line == "ucinewgame") {
            board.loadFEN(START_FEN);
        } else if (line.rfind("position", 0) == 0) {
            handlePosition(line);
        } else if (line.rfind("go", 0) == 0) {
            handleGo(line);
        } else if (line == "quit") {
            break;
        }
        // Unrecognized commands are ignored, per UCI convention — engines
        // should never crash on an unknown token, just skip it.
    }
}
