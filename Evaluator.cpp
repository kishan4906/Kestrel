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

// Standard "simplified evaluation" piece-square tables (centipawns), one
// per piece type. Written top-down as they're normally published — row 0
// is rank 8, row 7 is rank 1 — since that's how they're checked by eye
// against a printed board. Lookup converts between that layout and our
// rank-0-is-rank-1 square indexing (see pstValue below).
static const int PAWN_PST[8][8] = {
    {  0,  0,  0,  0,  0,  0,  0,  0 },
    { 50, 50, 50, 50, 50, 50, 50, 50 },
    { 10, 10, 20, 30, 30, 20, 10, 10 },
    {  5,  5, 10, 25, 25, 10,  5,  5 },
    {  0,  0,  0, 20, 20,  0,  0,  0 },
    {  5, -5,-10,  0,  0,-10, -5,  5 },
    {  5, 10, 10,-20,-20, 10, 10,  5 },
    {  0,  0,  0,  0,  0,  0,  0,  0 },
};

static const int KNIGHT_PST[8][8] = {
    { -50,-40,-30,-30,-30,-30,-40,-50 },
    { -40,-20,  0,  0,  0,  0,-20,-40 },
    { -30,  0, 10, 15, 15, 10,  0,-30 },
    { -30,  5, 15, 20, 20, 15,  5,-30 },
    { -30,  0, 15, 20, 20, 15,  0,-30 },
    { -30,  5, 10, 15, 15, 10,  5,-30 },
    { -40,-20,  0,  5,  5,  0,-20,-40 },
    { -50,-40,-30,-30,-30,-30,-40,-50 },
};

static const int BISHOP_PST[8][8] = {
    { -20,-10,-10,-10,-10,-10,-10,-20 },
    { -10,  0,  0,  0,  0,  0,  0,-10 },
    { -10,  0,  5, 10, 10,  5,  0,-10 },
    { -10,  5,  5, 10, 10,  5,  5,-10 },
    { -10,  0, 10, 10, 10, 10,  0,-10 },
    { -10, 10, 10, 10, 10, 10, 10,-10 },
    { -10,  5,  0,  0,  0,  0,  5,-10 },
    { -20,-10,-10,-10,-10,-10,-10,-20 },
};

static const int ROOK_PST[8][8] = {
    {  0,  0,  0,  0,  0,  0,  0,  0 },
    {  5, 10, 10, 10, 10, 10, 10,  5 },
    { -5,  0,  0,  0,  0,  0,  0, -5 },
    { -5,  0,  0,  0,  0,  0,  0, -5 },
    { -5,  0,  0,  0,  0,  0,  0, -5 },
    { -5,  0,  0,  0,  0,  0,  0, -5 },
    { -5,  0,  0,  0,  0,  0,  0, -5 },
    {  0,  0,  0,  5,  5,  0,  0,  0 },
};

static const int QUEEN_PST[8][8] = {
    { -20,-10,-10, -5, -5,-10,-10,-20 },
    { -10,  0,  0,  0,  0,  0,  0,-10 },
    { -10,  0,  5,  5,  5,  5,  0,-10 },
    {  -5,  0,  5,  5,  5,  5,  0, -5 },
    {   0,  0,  5,  5,  5,  5,  0, -5 },
    { -10,  5,  5,  5,  5,  5,  0,-10 },
    { -10,  0,  5,  0,  0,  0,  0,-10 },
    { -20,-10,-10, -5, -5,-10,-10,-20 },
};

// Middlegame king table — rewards staying tucked behind pawn cover
// (castled corners) and heavily penalizes wandering into the center,
// where the king is exposed to attack in the middlegame.
static const int KING_PST[8][8] = {
    { -30,-40,-40,-50,-50,-40,-40,-30 },
    { -30,-40,-40,-50,-50,-40,-40,-30 },
    { -30,-40,-40,-50,-50,-40,-40,-30 },
    { -30,-40,-40,-50,-50,-40,-40,-30 },
    { -20,-30,-30,-40,-40,-30,-30,-20 },
    { -10,-20,-20,-20,-20,-20,-20,-10 },
    {  20, 20,  0,  0,  0,  0, 20, 20 },
    {  20, 30, 10,  0,  0, 10, 30, 20 },
};

// Looks up the positional bonus for a piece of the given type/color sitting
// on `sq`. Tables above are written rank8-first, so White reads them
// "flipped" (table[7-rank]) while Black reads them directly (table[rank])
// — this is the standard trick for mirroring a table across colors instead
// of maintaining two separate tables per piece.
static int pstValue(PieceType type, Color color, Square sq) {
    int file = fileOf(sq);
    int rank = rankOf(sq);
    int row = (color == WHITE) ? (7 - rank) : rank;

    switch (type) {
        case PAWN:   return PAWN_PST[row][file];
        case KNIGHT: return KNIGHT_PST[row][file];
        case BISHOP: return BISHOP_PST[row][file];
        case ROOK:   return ROOK_PST[row][file];
        case QUEEN:  return QUEEN_PST[row][file];
        case KING:   return KING_PST[row][file];
        default:     return 0;
    }
}

int Evaluator::positionalScore(const Board& board) {
    int score = 0;
    for (Square sq = 0; sq < 64; sq++) {
        Piece p = board.at(sq);
        if (p.isEmpty()) continue;

        int bonus = pstValue(p.type, p.color, sq);
        score += (p.color == WHITE) ? bonus : -bonus;
    }
    return score;
}

int Evaluator::evaluate(const Board& board) {
    return materialScore(board) + positionalScore(board);
}
