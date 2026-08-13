#pragma once
#include "Board.h"
#include "Move.h"
#include <vector>

class MoveGenerator {
public:
    // Generates pseudo-legal moves for the side to move (doesn't yet check
    // whether the move leaves the mover's own king in check — that's added
    // in the legality-filtering step later).
    static std::vector<Move> generatePseudoLegalMoves(const Board& board);

private:
    static void generateKnightMoves(const Board& board, Square from, std::vector<Move>& moves);
    static void generateKingMoves(const Board& board, Square from, std::vector<Move>& moves);
    static void generateSlidingMoves(const Board& board, Square from,
                                      const int deltas[4][2], int numDirections,
                                      std::vector<Move>& moves);
};
