#pragma once
#include "Board.h"

// Implements enough of the UCI (Universal Chess Interface) protocol for
// a GUI (Arena, Cute Chess, ChessBase, or a custom web frontend) to drive
// this engine over stdin/stdout. UCI is plain text — one command per line.
class UCI {
public:
    // Reads commands from stdin in a loop until "quit" is received.
    static void loop();

private:
    static void handleUCI();
    static void handleIsReady();
    static void handlePosition(const std::string& line);
    static void handleGo(const std::string& line);

    static Board board; // the engine's current position, updated by "position"
};
