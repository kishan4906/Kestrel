#include "Search.h"
#include "MoveGenerator.h"
#include "Evaluator.h"
#include <vector>
#include <algorithm>

// Same piece values as the evaluator, duplicated here for MVV-LVA scoring —
// kept local since move ordering only needs relative magnitude, not the
// exact same constants as evaluation.
static int pieceWeight(PieceType type) {
    switch (type) {
        case PAWN:   return 100;
        case KNIGHT: return 320;
        case BISHOP: return 330;
        case ROOK:   return 500;
        case QUEEN:  return 900;
        case KING:   return 20000;
        default:     return 0;
    }
}

// MVV-LVA: score = value of captured piece minus value of the capturing
// piece (scaled down so it never outweighs the victim's value). A pawn
// taking a queen scores much higher than a queen taking a pawn — both are
// good, but the pawn recapture is objectively the safer/better order to
// try first since it doesn't risk the queen for nothing if it's defended.
static int moveOrderScore(const Board& board, const Move& m) {
    if (!m.isCapture()) return 0;

    PieceType attacker = board.at(m.from).type;
    PieceType victim;
    if (m.flag == EN_PASSANT) {
        victim = PAWN; // en passant always captures a pawn
    } else {
        victim = board.at(m.to).type;
    }

    return pieceWeight(victim) * 10 - pieceWeight(attacker);
}

void Search::orderMoves(const Board& board, std::vector<Move>& moves) {
    std::sort(moves.begin(), moves.end(), [&board](const Move& a, const Move& b) {
        return moveOrderScore(board, a) > moveOrderScore(board, b);
    });
}

int Search::alphaBeta(const Board& board, int depth, int alpha, int beta, bool maximizing) {
    std::vector<Move> legalMoves = MoveGenerator::generateLegalMoves(board);

    // No legal moves: either checkmate or stalemate — both are terminal,
    // no need to recurse further.
    if (legalMoves.empty()) {
        bool inCheck = MoveGenerator::isInCheck(board, board.sideToMove);
        if (!inCheck) return 0; // stalemate — draw

        // Checkmate: score it as extremely bad for whoever is mated, but
        // scaled by depth so a *closer* mate scores better than a farther
        // one (the engine prefers mating in 2 over mating in 5).
        int mateScore = INF - (1000 - depth); // depth pushed in as a tiebreaker
        return maximizing ? -mateScore : mateScore;
    }

    if (depth == 0) {
        return Evaluator::evaluate(board);
    }

    orderMoves(board, legalMoves);

    if (maximizing) {
        int best = -INF;
        for (const Move& m : legalMoves) {
            Board copy = board;
            copy.makeMove(m);
            int score = alphaBeta(copy, depth - 1, alpha, beta, false);
            best = std::max(best, score);
            alpha = std::max(alpha, best);
            if (beta <= alpha) break; // beta cutoff — opponent won't allow this branch
        }
        return best;
    } else {
        int best = INF;
        for (const Move& m : legalMoves) {
            Board copy = board;
            copy.makeMove(m);
            int score = alphaBeta(copy, depth - 1, alpha, beta, true);
            best = std::min(best, score);
            beta = std::min(beta, best);
            if (beta <= alpha) break; // alpha cutoff
        }
        return best;
    }
}

SearchResult Search::findBestMove(const Board& board, int depth) {
    std::vector<Move> legalMoves = MoveGenerator::generateLegalMoves(board);
    SearchResult result;

    if (legalMoves.empty()) {
        return result; // no legal moves — caller should check for checkmate/stalemate separately
    }

    bool maximizing = (board.sideToMove == WHITE);
    int bestScore = maximizing ? -INF : INF;

    orderMoves(board, legalMoves);
    Move bestMove = legalMoves[0]; // fallback in case everything ties

    for (const Move& m : legalMoves) {
        Board copy = board;
        copy.makeMove(m);
        int score = alphaBeta(copy, depth - 1, -INF, INF, !maximizing);

        if (maximizing && score > bestScore) {
            bestScore = score;
            bestMove = m;
        } else if (!maximizing && score < bestScore) {
            bestScore = score;
            bestMove = m;
        }
    }

    result.bestMove = bestMove;
    result.score = bestScore;
    return result;
}
