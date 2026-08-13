#include "Board.h"
#include "MoveGenerator.h"
#include "Evaluator.h"
#include "Search.h"
#include <iostream>
#include <chrono>

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

    // Evaluation: starting position should be exactly 0 (perfectly symmetric material)
    std::cout << "\n--- Eval test: starting position ---\n";
    board.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    board.print();
    std::cout << "Evaluation: " << Evaluator::evaluate(board) << " (expect 0)\n";

    // Evaluation: white is up a queen — should be strongly positive (+900)
    std::cout << "\n--- Eval test: white up a queen ---\n";
    board.loadFEN("4k3/8/8/8/8/8/8/3QK3 w - - 0 1");
    board.print();
    std::cout << "Evaluation: " << Evaluator::evaluate(board) << " (expect +900)\n";

    // Evaluation: black is up a rook — should be negative (-500)
    std::cout << "\n--- Eval test: black up a rook ---\n";
    board.loadFEN("3rk3/8/8/8/8/8/8/4K3 w - - 0 1");
    board.print();
    std::cout << "Evaluation: " << Evaluator::evaluate(board) << " (expect -500)\n";

    // Search test: white queen can capture a free/undefended black rook.
    // At depth 2, the engine should find and take it.
    std::cout << "\n--- Search test: white queen can capture a free rook ---\n";
    board.loadFEN("4k3/8/8/3r4/8/8/8/3QK3 w - - 0 1");
    board.print();
    {
        SearchResult r = Search::findBestMove(board, 2);
        std::cout << "Best move: " << moveToString(r.bestMove)
                   << " (expect d1d5, capturing the rook), score: " << r.score << "\n";
    }

    // Search test: mate in 1 — white queen delivers back-rank mate on e8.
    // Black king on h8 is boxed in by its own pawns on g7/h7.
    std::cout << "\n--- Search test: mate in 1 ---\n";
    board.loadFEN("6k1/6pp/8/8/8/8/8/4Q1K1 w - - 0 1");
    board.print();
    {
        SearchResult r = Search::findBestMove(board, 2);
        std::cout << "Best move: " << moveToString(r.bestMove)
                   << " (expect e1e8, checkmate), score: " << r.score << "\n";
    }

    // Move-ordering / depth test: search the starting position at depth 4
    // with move ordering on. This is mainly a speed/sanity check — at
    // depth 4 a naive unordered search takes noticeably longer.
    std::cout << "\n--- Search test: starting position, depth 4 (timed) ---\n";
    board.loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    {
        auto start = std::chrono::steady_clock::now();
        SearchResult r = Search::findBestMove(board, 4);
        auto end = std::chrono::steady_clock::now();
        double seconds = std::chrono::duration<double>(end - start).count();

        std::cout << "Best move: " << moveToString(r.bestMove) << ", score: " << r.score << "\n";
        std::cout << "Time taken: " << seconds << "s\n";
    }

    std::cout << "\n--- Search test: starting position, depth 5 (timed) ---\n";
    {
        auto start = std::chrono::steady_clock::now();
        SearchResult r = Search::findBestMove(board, 5);
        auto end = std::chrono::steady_clock::now();
        double seconds = std::chrono::duration<double>(end - start).count();

        std::cout << "Best move: " << moveToString(r.bestMove) << ", score: " << r.score << "\n";
        std::cout << "Time taken: " << seconds << "s\n";
    }

    // Iterative deepening test: give it a 1-second budget instead of a
    // fixed depth. It should search depth 1, 2, 3... as far as it can get
    // within that time, then return the best move from the last completed depth.
    std::cout << "\n--- Iterative deepening test: starting position, 1000ms budget ---\n";
    {
        auto start = std::chrono::steady_clock::now();
        SearchResult r = Search::findBestMoveTimed(board, 20, 1000);
        auto end = std::chrono::steady_clock::now();
        double seconds = std::chrono::duration<double>(end - start).count();

        std::cout << "Best move: " << moveToString(r.bestMove) << ", score: " << r.score << "\n";
        std::cout << "Depth reached: " << r.depthReached << ", nodes searched: " << r.nodes << "\n";
        std::cout << "Time taken: " << seconds << "s (budget was 1.0s)\n";
    }

    return 0;
}
