#include "MoveGenerator.h"

// Knight offsets expressed as (file delta, rank delta) — using deltas instead
// of raw square-index offsets avoids the classic bug where a move wraps
// around the board edge (e.g. a knight on h-file "jumping" to the a-file).
static const int KNIGHT_DELTAS[8][2] = {
    {1, 2}, {2, 1}, {2, -1}, {1, -2},
    {-1, -2}, {-2, -1}, {-2, 1}, {-1, 2}
};

static const int KING_DELTAS[8][2] = {
    {1, 0}, {1, 1}, {0, 1}, {-1, 1},
    {-1, 0}, {-1, -1}, {0, -1}, {1, -1}
};

static bool onBoard(int file, int rank) {
    return file >= 0 && file < 8 && rank >= 0 && rank < 8;
}

// Shared logic for any fixed-offset piece (knight, king): try each delta,
// skip off-board squares, skip squares occupied by your own piece, and
// tag the move CAPTURE or QUIET based on what's there.
static void generateOffsetMoves(const Board& board, Square from,
                                 const int deltas[8][2],
                                 std::vector<Move>& moves) {
    Color us = board.at(from).color;
    int fromFile = fileOf(from);
    int fromRank = rankOf(from);

    for (int i = 0; i < 8; i++) {
        int toFile = fromFile + deltas[i][0];
        int toRank = fromRank + deltas[i][1];
        if (!onBoard(toFile, toRank)) continue;

        Square to = makeSquare(toFile, toRank);
        Piece target = board.at(to);

        if (target.isEmpty()) {
            moves.push_back({from, to, QUIET});
        } else if (target.color != us) {
            moves.push_back({from, to, CAPTURE});
        }
        // else: own piece occupies the square, illegal, skip
    }
}

void MoveGenerator::generateKnightMoves(const Board& board, Square from, std::vector<Move>& moves) {
    generateOffsetMoves(board, from, KNIGHT_DELTAS, moves);
}

void MoveGenerator::generateKingMoves(const Board& board, Square from, std::vector<Move>& moves) {
    generateOffsetMoves(board, from, KING_DELTAS, moves);
    // Castling is added later once we're checking king safety —
    // castling through/into check requires the check-detection logic first.
}

std::vector<Move> MoveGenerator::generatePseudoLegalMoves(const Board& board) {
    std::vector<Move> moves;
    Color us = board.sideToMove;

    for (Square sq = 0; sq < 64; sq++) {
        Piece p = board.at(sq);
        if (p.isEmpty() || p.color != us) continue;

        switch (p.type) {
            case KNIGHT:
                generateKnightMoves(board, sq, moves);
                break;
            case KING:
                generateKingMoves(board, sq, moves);
                break;
            // PAWN, BISHOP, ROOK, QUEEN come in the next steps
            default:
                break;
        }
    }

    return moves;
}
