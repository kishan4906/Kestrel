#include "Board.h"
#include <sstream>
#include <cctype>
#include <iostream>

std::string squareToString(Square sq) {
    std::string s;
    s += char('a' + fileOf(sq));
    s += char('1' + rankOf(sq));
    return s;
}

Board::Board() {
    loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

static Piece pieceFromChar(char c) {
    Piece p;
    p.color = std::isupper(c) ? WHITE : BLACK;
    switch (std::tolower(c)) {
        case 'p': p.type = PAWN;   break;
        case 'n': p.type = KNIGHT; break;
        case 'b': p.type = BISHOP; break;
        case 'r': p.type = ROOK;   break;
        case 'q': p.type = QUEEN;  break;
        case 'k': p.type = KING;   break;
        default:  p.type = NONE;   p.color = NO_COLOR; break;
    }
    return p;
}

static char pieceToChar(Piece p) {
    char c;
    switch (p.type) {
        case PAWN:   c = 'p'; break;
        case KNIGHT: c = 'n'; break;
        case BISHOP: c = 'b'; break;
        case ROOK:   c = 'r'; break;
        case QUEEN:  c = 'q'; break;
        case KING:   c = 'k'; break;
        default: return '.';
    }
    return p.color == WHITE ? std::toupper(c) : c;
}

void Board::loadFEN(const std::string& fen) {
    squares.fill(Piece{});
    std::istringstream iss(fen);
    std::string boardPart, sidePart, castlePart, epPart;
    int half = 0, full = 1;
    iss >> boardPart >> sidePart >> castlePart >> epPart >> half >> full;

    // Board part: ranks 8 -> 1, separated by '/'
    int rank = 7, file = 0;
    for (char c : boardPart) {
        if (c == '/') { rank--; file = 0; }
        else if (std::isdigit(c)) { file += c - '0'; }
        else { set(makeSquare(file, rank), pieceFromChar(c)); file++; }
    }

    sideToMove = (sidePart == "w") ? WHITE : BLACK;

    castling = CastlingRights{false, false, false, false};
    for (char c : castlePart) {
        if (c == 'K') castling.whiteKingSide = true;
        if (c == 'Q') castling.whiteQueenSide = true;
        if (c == 'k') castling.blackKingSide = true;
        if (c == 'q') castling.blackQueenSide = true;
    }

    if (epPart != "-" && epPart.size() == 2) {
        int f = epPart[0] - 'a';
        int r = epPart[1] - '1';
        enPassantTarget = makeSquare(f, r);
    } else {
        enPassantTarget = -1;
    }

    halfmoveClock = half;
    fullmoveNumber = full;
}

std::string Board::toFEN() const {
    std::ostringstream oss;
    for (int rank = 7; rank >= 0; rank--) {
        int emptyCount = 0;
        for (int file = 0; file < 8; file++) {
            Piece p = at(makeSquare(file, rank));
            if (p.isEmpty()) {
                emptyCount++;
            } else {
                if (emptyCount > 0) { oss << emptyCount; emptyCount = 0; }
                oss << pieceToChar(p);
            }
        }
        if (emptyCount > 0) oss << emptyCount;
        if (rank > 0) oss << '/';
    }

    oss << ' ' << (sideToMove == WHITE ? 'w' : 'b') << ' ';

    std::string rights;
    if (castling.whiteKingSide)  rights += 'K';
    if (castling.whiteQueenSide) rights += 'Q';
    if (castling.blackKingSide)  rights += 'k';
    if (castling.blackQueenSide) rights += 'q';
    oss << (rights.empty() ? "-" : rights) << ' ';

    oss << (enPassantTarget == -1 ? "-" : squareToString(enPassantTarget)) << ' ';
    oss << halfmoveClock << ' ' << fullmoveNumber;

    return oss.str();
}

void Board::print() const {
    for (int rank = 7; rank >= 0; rank--) {
        std::cout << (rank + 1) << " ";
        for (int file = 0; file < 8; file++) {
            Piece p = at(makeSquare(file, rank));
            std::cout << pieceToChar(p) << ' ';
        }
        std::cout << '\n';
    }
    std::cout << "  a b c d e f g h\n";
    std::cout << "Side to move: " << (sideToMove == WHITE ? "White" : "Black") << "\n";
}
