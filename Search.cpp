#include "Search.h"
#include "MoveGenerator.h"
#include "Evaluator.h"
#include <vector>

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
