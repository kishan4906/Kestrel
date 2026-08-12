#include "Board.h"
#include "MoveGenerator.h"
#include <iostream>

static void printMoves(const std::vector<Move>& moves) {
    std::cout << "Found " << moves.size() << " moves: ";
    for (const auto& m : moves) std::cout << moveToString(m) << " ";
    std::cout << "\n";
}

int main() {
    Board board;
    std::cout << "Starting position:\n";
    board.print();
    std::cout << "\nFEN round-trip: " << board.toFEN() << "\n";

    std::cout << "\nLoading a custom FEN (after 1.e4 e5 2.Nf3):\n";
    board.loadFEN("rnbqkbnr/pppp1ppp/8/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2");
    board.print();
    std::cout << "\nFEN round-trip: " << board.toFEN() << "\n";

    // Knight in the corner (a1) — should have exactly 2 legal jumps
    std::cout << "\n--- Knight-only test: white knight on a1, kings only ---\n";
    board.loadFEN("4k3/8/8/8/8/8/8/N3K3 w - - 0 1");
    board.print();
    printMoves(MoveGenerator::generatePseudoLegalMoves(board));

    // Knight in the center (d4) — should have all 8 jumps available
    std::cout << "\n--- Knight-only test: white knight on d4 ---\n";
    board.loadFEN("4k3/8/8/8/3N4/8/8/4K3 w - - 0 1");
    board.print();
    printMoves(MoveGenerator::generatePseudoLegalMoves(board));

    // King mobility + captures: white king surrounded by black pawns to capture
    std::cout << "\n--- King test: white king on e4 surrounded by black pawns ---\n";
    board.loadFEN("4k3/8/8/3ppp2/3pKp2/3ppp2/8/8 w - - 0 1");
    board.print();
    printMoves(MoveGenerator::generatePseudoLegalMoves(board));

    return 0;
}
