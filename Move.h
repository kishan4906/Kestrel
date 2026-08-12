#pragma once
#include "Board.h"

enum MoveFlag : uint8_t {
    QUIET = 0,
    CAPTURE,
    DOUBLE_PAWN_PUSH,
    EN_PASSANT,
    CASTLE_KINGSIDE,
    CASTLE_QUEENSIDE,
    PROMOTION,
    PROMOTION_CAPTURE
};

struct Move {
    Square from = -1;
    Square to = -1;
    MoveFlag flag = QUIET;
    PieceType promotion = NONE; // only set when flag is PROMOTION / PROMOTION_CAPTURE

    bool isCapture() const {
        return flag == CAPTURE || flag == EN_PASSANT || flag == PROMOTION_CAPTURE;
    }
};

std::string moveToString(const Move& m); // e.g. "e2e4", "e7e8q" for promotion
