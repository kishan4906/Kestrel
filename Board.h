#pragma once
#include <array>
#include <string>
#include <cstdint>

// ---- Piece encoding ----
// Piece = color | type, packed into a single byte enum.
enum PieceType : uint8_t {
    NONE = 0, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
};

enum Color : uint8_t {
    WHITE = 0, BLACK = 1, NO_COLOR = 2
};

struct Piece {
    PieceType type = NONE;
    Color color = NO_COLOR;

    bool isEmpty() const { return type == NONE; }
};

// Square index: 0 = a1, 7 = h1, 8 = a2, ... 63 = h8 (little-endian rank-file)
using Square = int;

inline int fileOf(Square sq) { return sq % 8; }   // 0=a ... 7=h
inline int rankOf(Square sq) { return sq / 8; }   // 0=rank1 ... 7=rank8
inline Square makeSquare(int file, int rank) { return rank * 8 + file; }
std::string squareToString(Square sq); // e.g. 27 -> "d4"

struct CastlingRights {
    bool whiteKingSide  = true;
    bool whiteQueenSide = true;
    bool blackKingSide  = true;
    bool blackQueenSide = true;
};

class Board {
public:
    Board();

    void loadFEN(const std::string& fen);
    std::string toFEN() const;
    void print() const;

    Piece at(Square sq) const { return squares[sq]; }
    void  set(Square sq, Piece p) { squares[sq] = p; }

    Color sideToMove = WHITE;
    CastlingRights castling;
    Square enPassantTarget = -1;   // -1 = none
    int halfmoveClock = 0;
    int fullmoveNumber = 1;

private:
    std::array<Piece, 64> squares;
};
