#include "Move.h"

static char promoChar(PieceType t) {
    switch (t) {
        case QUEEN:  return 'q';
        case ROOK:   return 'r';
        case BISHOP: return 'b';
        case KNIGHT: return 'n';
        default:     return '\0';
    }
}

std::string moveToString(const Move& m) {
    std::string s = squareToString(m.from) + squareToString(m.to);
    if (m.flag == PROMOTION || m.flag == PROMOTION_CAPTURE) {
        s += promoChar(m.promotion);
    }
    return s;
}
