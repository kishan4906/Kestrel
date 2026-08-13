#pragma once
#include "Board.h"
#include "Move.h"
#include <limits>
#include <vector>
#include <chrono>

// A move paired with the score the search assigned to it.
struct SearchResult {
    Move bestMove;
    int score = 0;
    int depthReached = 0;   // how many plies iterative deepening completed
    long nodes = 0;         // positions examined, for reporting search speed
};

class Search {
public:
    // Searches `depth` plies ahead and returns the best move for the side
    // to move in `board`, along with its evaluation.
    static SearchResult findBestMove(const Board& board, int depth);

    // Iterative deepening: searches depth 1, then 2, then 3... up to
    // maxDepth, stopping early if timeLimitMs elapses. Always returns the
    // best move found by the last FULLY completed depth — a depth that
    // gets cut off partway through is discarded, since a partial search
    // isn't trustworthy (it may not have looked at every root move).
    static SearchResult findBestMoveTimed(const Board& board, int maxDepth, int timeLimitMs);

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

    // Deadline for the current timed search, and a node counter for stats.
    // Both are reset at the start of every findBestMoveTimed call.
    static std::chrono::steady_clock::time_point deadline;
    static long nodeCount;
    static bool timeIsUp();
};

// Thrown from deep inside alphaBeta when the time budget runs out, so the
// whole in-progress depth can unwind immediately instead of finishing a
// search whose result we're about to discard anyway.
struct SearchTimeout {};
