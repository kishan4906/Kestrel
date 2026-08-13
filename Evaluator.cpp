#include "Evaluator.h"

// Standard piece values in centipawns. King has no material value since
// it can never be captured — its "value" comes from king safety terms,
// added later.
static int pieceValue(PieceType type) {
    switch (type) {
        case PAWN:   return 100;
        case KNIGHT: return 320;
        case BISHOP: return 330;
        case ROOK:   return 500;
        case QUEEN:  return 900;
        default:     return 0;
    }
}

int Evaluator::materialScore(const Board& board) {
    int score = 0;
    for (Square sq = 0; sq < 64; sq++) {
        Piece p = board.at(sq);
        if (p.isEmpty()) continue;

        int value = pieceValue(p.type);
        score += (p.color == WHITE) ? value : -value;
    }
    return score;
}

int Evaluator::evaluate(const Board& board) {
    // Material only for now — piece-square tables (positional bonuses)
    // get layered in next.
    return materialScore(board);
}
