#pragma once
#include "Board.h"

// Score convention: positive favors White, negative favors Black,
// measured in centipawns (100 = one pawn's worth of advantage).
class Evaluator {
public:
    static int evaluate(const Board& board);

private:
    static int materialScore(const Board& board);
};
