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

    // Rook in the center of an empty board — should see 14 squares (7 each direction)
    std::cout << "\n--- Rook test: white rook on d4, empty board ---\n";
    board.loadFEN("4k3/8/8/8/3R4/8/8/4K3 w - - 0 1");
    board.print();
    printMoves(MoveGenerator::generatePseudoLegalMoves(board));

    // Bishop blocked by own pawn one square away, enemy pawn blocking another diagonal
    std::cout << "\n--- Bishop test: blocked by own pawn, captures enemy pawn ---\n";
    board.loadFEN("4k3/8/8/8/8/2P5/1B6/4K1p1 w - - 0 1");
    board.print();
    printMoves(MoveGenerator::generatePseudoLegalMoves(board));

    // Queen in the center of an empty board — should see 27 squares (8 directions)
    std::cout << "\n--- Queen test: white queen on d4, empty board ---\n";
    board.loadFEN("4k3/8/8/8/3Q4/8/8/4K3 w - - 0 1");
    board.print();
    printMoves(MoveGenerator::generatePseudoLegalMoves(board));

    // Pawn on start rank: single push, double push, and a diagonal capture
    std::cout << "\n--- Pawn test: white pawn on e2, black pawn to capture on d3 ---\n";
    board.loadFEN("4k3/8/8/8/8/3p4/4P3/4K3 w - - 0 1");
    board.print();
    printMoves(MoveGenerator::generatePseudoLegalMoves(board));

    // Pawn NOT on start rank: only single push allowed, no double push
    std::cout << "\n--- Pawn test: white pawn on e3 (already moved) ---\n";
    board.loadFEN("4k3/8/8/8/8/4P3/8/4K3 w - - 0 1");
    board.print();
    printMoves(MoveGenerator::generatePseudoLegalMoves(board));

    // En passant: white pawn on e5, black just double-pushed d7-d5, ep target d6
    std::cout << "\n--- En passant test: white pawn e5, black pawn just played d7-d5 ---\n";
    board.loadFEN("4k3/8/8/3pP3/8/8/8/4K3 w - d6 0 1");
    board.print();
    printMoves(MoveGenerator::generatePseudoLegalMoves(board));

    // Promotion: white pawn on e7, empty square ahead — should yield 4 promotion moves
    std::cout << "\n--- Promotion test: white pawn on e7, empty e8 ---\n";
    board.loadFEN("7k/4P3/8/8/8/8/8/4K3 w - - 0 1");
    board.print();
    printMoves(MoveGenerator::generatePseudoLegalMoves(board));

    // Pinned piece: white knight on d2 is pinned to the king by a black rook
    // on d-file. Pseudo-legal moves would let it jump away; legal moves must not.
    std::cout << "\n--- Pin test: white knight on d2 pinned by rook on d8 ---\n";
    board.loadFEN("3r3k/8/8/8/8/8/3N4/3K4 w - - 0 1");
    board.print();
    std::cout << "Pseudo-legal: ";
    printMoves(MoveGenerator::generatePseudoLegalMoves(board));
    std::cout << "Legal:        ";
    printMoves(MoveGenerator::generateLegalMoves(board));
    std::cout << "In check? " << (MoveGenerator::isInCheck(board, WHITE) ? "yes" : "no") << "\n";

    // King in check: white king on e1 in check from black rook on e8.
    // Only moves that escape check should remain legal.
    std::cout << "\n--- Check test: white king on e1 in check from rook on e8 ---\n";
    board.loadFEN("4r2k/8/8/8/8/8/8/4K3 w - - 0 1");
    board.print();
    std::cout << "In check? " << (MoveGenerator::isInCheck(board, WHITE) ? "yes" : "no") << "\n";
    std::cout << "Pseudo-legal: ";
    printMoves(MoveGenerator::generatePseudoLegalMoves(board));
    std::cout << "Legal:        ";
    printMoves(MoveGenerator::generateLegalMoves(board));

    return 0;
}
