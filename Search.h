#pragma once
#include "Board.h"
#include "Move.h"
#include <limits>
#include <vector>

// A move paired with the score the search assigned to it.
struct SearchResult {
    Move bestMove;
    int score = 0;
};

class Search {
public:
    // Searches `depth` plies ahead and returns the best move for the side
    // to move in `board`, along with its evaluation.
    static SearchResult findBestMove(const Board& board, int depth);

    static constexpr int INF = std::numeric_limits<int>::max() / 2; // avoid overflow when negated

private:
    // Standard alpha-beta minimax. Returns the score of `board` from
    // White's perspective (positive = good for White), searched `depth`
    // plies deep. alpha/beta bound the range of scores worth exploring —
    // any branch that falls outside gets pruned without full evaluation.
    static int alphaBeta(const Board& board, int depth, int alpha, int beta, bool maximizing);

    // Reorders moves in place so captures (especially good ones — capturing
    // a queen with a pawn) are searched before quiet moves. Alpha-beta
    // prunes far more when strong moves are found early, since a beta
    // cutoff is more likely to trigger sooner.
    static void orderMoves(const Board& board, std::vector<Move>& moves);
};
