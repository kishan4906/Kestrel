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

    // Filters pseudo-legal moves down to truly legal ones: simulates each
    // move and discards it if the mover's own king ends up in check.
    static std::vector<Move> generateLegalMoves(const Board& board);

    // Is `sq` attacked by any piece of color `byColor`? Used both for
    // check detection (attack on the king's square) and later for
    // castling legality (king can't pass through an attacked square).
    static bool isSquareAttacked(const Board& board, Square sq, Color byColor);

    // Is the given color's king currently in check?
    static bool isInCheck(const Board& board, Color color);

private:
    static void generateKnightMoves(const Board& board, Square from, std::vector<Move>& moves);
    static void generateKingMoves(const Board& board, Square from, std::vector<Move>& moves);
    static void generateSlidingMoves(const Board& board, Square from,
                                      const int deltas[4][2], int numDirections,
                                      std::vector<Move>& moves);
    static void generatePawnMoves(const Board& board, Square from, std::vector<Move>& moves);
    static Square findKing(const Board& board, Color color);
};
