#include "Board.h"
#include <iostream>

int main() {
    Board board;
    std::cout << "Starting position:\n";
    board.print();

    std::cout << "\nFEN round-trip: " << board.toFEN() << "\n";

    std::cout << "\nLoading a custom FEN (after 1.e4 e5 2.Nf3):\n";
    board.loadFEN("rnbqkbnr/pppp1ppp/8/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2");
    board.print();
    std::cout << "\nFEN round-trip: " << board.toFEN() << "\n";

    return 0;
}
