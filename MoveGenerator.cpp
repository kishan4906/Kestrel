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

// Directions for sliding pieces. Bishop moves diagonally, rook orthogonally;
// queen is just both combined at the call site.
static const int BISHOP_DIRS[4][2] = { {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };
static const int ROOK_DIRS[4][2]   = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} };

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

// Walks each direction one square at a time, stopping at the first piece hit
// (capturing it if it's an enemy) or at the board edge. This is the same
// idea for bishop, rook, and queen — only the direction set differs.
void MoveGenerator::generateSlidingMoves(const Board& board, Square from,
                                          const int deltas[4][2], int numDirections,
                                          std::vector<Move>& moves) {
    Color us = board.at(from).color;
    int fromFile = fileOf(from);
    int fromRank = rankOf(from);

    for (int d = 0; d < numDirections; d++) {
        int file = fromFile;
        int rank = fromRank;

        while (true) {
            file += deltas[d][0];
            rank += deltas[d][1];
            if (!onBoard(file, rank)) break;

            Square to = makeSquare(file, rank);
            Piece target = board.at(to);

            if (target.isEmpty()) {
                moves.push_back({from, to, QUIET});
                continue; // keep walking in this direction
            }
            if (target.color != us) {
                moves.push_back({from, to, CAPTURE});
            }
            break; // blocked either way — own piece or a captured enemy piece
        }
    }
}

// Adds a pawn push/capture, expanding into 4 promotion moves if it lands
// on the far rank (rank 8 for white, rank 1 for black).
static void addPawnMove(Square from, Square to, bool isCapture, bool isPromotion,
                         std::vector<Move>& moves) {
    if (isPromotion) {
        MoveFlag flag = isCapture ? PROMOTION_CAPTURE : PROMOTION;
        for (PieceType promo : { QUEEN, ROOK, BISHOP, KNIGHT }) {
            moves.push_back({from, to, flag, promo});
        }
    } else {
        moves.push_back({from, to, isCapture ? CAPTURE : QUIET});
    }
}

void MoveGenerator::generatePawnMoves(const Board& board, Square from, std::vector<Move>& moves) {
    Color us = board.at(from).color;
    int file = fileOf(from);
    int rank = rankOf(from);

    // White moves up the board (+1 rank), black moves down (-1 rank).
    int dir = (us == WHITE) ? 1 : -1;
    int startRank = (us == WHITE) ? 1 : 6;   // rank 2 for white, rank 7 for black (0-indexed)
    int promoRank = (us == WHITE) ? 7 : 0;   // landing on rank 8 / rank 1 promotes

    // --- Single push ---
    int oneRank = rank + dir;
    if (onBoard(file, oneRank)) {
        Square oneSq = makeSquare(file, oneRank);
        if (board.at(oneSq).isEmpty()) {
            bool promotes = (oneRank == promoRank);
            addPawnMove(from, oneSq, false, promotes, moves);

            // --- Double push (only from the start rank, only if both squares are clear) ---
            if (rank == startRank) {
                int twoRank = rank + 2 * dir;
                Square twoSq = makeSquare(file, twoRank);
                if (board.at(twoSq).isEmpty()) {
                    moves.push_back({from, twoSq, DOUBLE_PAWN_PUSH});
                }
            }
        }
    }

    // --- Diagonal captures (including en passant) ---
    for (int df : { -1, 1 }) {
        int captFile = file + df;
        int captRank = rank + dir;
        if (!onBoard(captFile, captRank)) continue;

        Square captSq = makeSquare(captFile, captRank);
        Piece target = board.at(captSq);

        if (!target.isEmpty() && target.color != us) {
            bool promotes = (captRank == promoRank);
            addPawnMove(from, captSq, true, promotes, moves);
        } else if (target.isEmpty() && captSq == board.enPassantTarget) {
            // En passant: the captured pawn sits beside us, not on captSq itself.
            moves.push_back({from, captSq, EN_PASSANT});
        }
    }
}

std::vector<Move> MoveGenerator::generatePseudoLegalMoves(const Board& board) {
    std::vector<Move> moves;
    Color us = board.sideToMove;

    for (Square sq = 0; sq < 64; sq++) {
        Piece p = board.at(sq);
        if (p.isEmpty() || p.color != us) continue;

        switch (p.type) {
            case PAWN:
                generatePawnMoves(board, sq, moves);
                break;
            case KNIGHT:
                generateKnightMoves(board, sq, moves);
                break;
            case KING:
                generateKingMoves(board, sq, moves);
                break;
            case BISHOP:
                generateSlidingMoves(board, sq, BISHOP_DIRS, 4, moves);
                break;
            case ROOK:
                generateSlidingMoves(board, sq, ROOK_DIRS, 4, moves);
                break;
            case QUEEN:
                generateSlidingMoves(board, sq, BISHOP_DIRS, 4, moves);
                generateSlidingMoves(board, sq, ROOK_DIRS, 4, moves);
                break;
            default:
                break;
        }
    }

    return moves;
}
