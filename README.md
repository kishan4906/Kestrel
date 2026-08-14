# ♟️ Kestrel — C++17 Chess Engine

**Kestrel** is a chess engine built from scratch in **C++17**.

It is designed around classic chess-engine techniques such as **alpha-beta search, position evaluation, move generation, and iterative/depth-limited analysis**, and communicates through the **Universal Chess Interface (UCI)** protocol.

Kestrel can be used as a standalone chess engine or integrated into other applications.

**Kestrel powers [Corvus](https://github.com/kishan4906/Corvus), an AI chess coaching application that explains Kestrel's analysis in plain English.**

## ✨ Features

* ♟️ Complete chess position representation
* 🔄 Legal move generation
* 🧠 Alpha-beta search
* 📊 Position evaluation
* ⏱️ Time-controlled search
* 🔎 Depth-limited analysis
* 🌳 Node counting
* 🎯 Best-move calculation
* 🔌 UCI protocol support
* 💻 C++17 implementation
* 🤖 Integration with external applications such as **Corvus AI Chess Coach**

## 🏗️ Architecture

```text
                    ┌──────────────────────┐
                    │      Kestrel         │
                    │    Chess Engine      │
                    │       C++17          │
                    └──────────┬───────────┘
                               │
                 ┌─────────────┼─────────────┐
                 │             │             │
                 ▼             ▼             ▼
          Move Generation   Evaluation   Search
                 │             │             │
                 └─────────────┼─────────────┘
                               ▼
                       Alpha-Beta Search
                               │
                               ▼
                         Best Move
                               │
                               ▼
                         UCI Interface
```

## 🧠 Search

Kestrel uses **alpha-beta pruning** to efficiently search the game tree.

Conceptually:

```text
Position
   │
   ▼
Generate legal moves
   │
   ▼
Search candidate moves
   │
   ▼
Evaluate resulting positions
   │
   ▼
Alpha-Beta pruning
   │
   ▼
Select best move
```

The engine can perform time-controlled searches, allowing it to stop searching when the allocated thinking time has expired.

## 📈 Evaluation

Kestrel evaluates chess positions using a custom evaluation system.

The evaluation produces a **centipawn score**, where positive values generally favor White and negative values favor Black.

For example:

```text
+100  → approximately one pawn advantage for White
+50   → small White advantage
  0   → approximately equal
-50   → small Black advantage
-100  → approximately one pawn advantage for Black
```

The evaluation is then used by the search algorithm to determine which moves lead to stronger positions.

## 🔌 UCI Support

Kestrel communicates using the **Universal Chess Interface (UCI)** protocol.

This allows external programs to control the engine and retrieve analysis.

Typical communication looks like:

```text
GUI / Application
       │
       │ position ...
       ▼
    Kestrel
       │
       │ go ...
       ▼
    Search
       │
       ▼
bestmove e2e4
```

This architecture also makes Kestrel easy to integrate into other chess applications.

## 🤖 Corvus Integration

Kestrel powers **Corvus**, a separate AI-powered chess coaching application.

**Corvus** uses Kestrel for chess analysis and combines the engine's output with an LLM to provide explanations in plain English.

```text
              KESTREL
          C++17 Chess Engine
                  │
                  │ UCI
                  ▼
              CORVUS
          AI Chess Coach
                  │
                  │ Analysis
                  ▼
              Groq LLM
                  │
                  ▼
       Human-readable explanation
```

For example, Kestrel may provide:

```text
Best move: e2e4
Evaluation: +0.25
Depth: 6
Nodes: 45213
```

Corvus can then turn that engine output into an explanation such as:

> **e4 is a strong move because it immediately fights for the center, opens lines for the pieces, and helps White develop with tempo.**

👉 **[View Corvus — AI Chess Coach](https://github.com/kishan4906/Corvus)**

## 🛠️ Tech Stack

| Component   | Technology               |
| ----------- | ------------------------ |
| Language    | C++17                    |
| Search      | Alpha-Beta               |
| Interface   | UCI                      |
| Build       | C++ compiler             |
| Integration | Kestrel subprocess / UCI |
| AI Coaching | Corvus + Groq            |

## 📂 Project Structure

```text
Kestrel/
│
├── Board.cpp / Board.h
├── Move.cpp / Move.h
├── Search.cpp / Search.h
├── Evaluation.cpp / Evaluation.h
├── UCI.cpp / UCI.h
├── uci_main.cpp
│
├── CMakeLists.txt
├── Makefile
└── README.md
```

> File names may vary depending on the current implementation.

## 🚀 Getting Started

### Requirements

You need:

* A C++17-compatible compiler
* CMake (if using the CMake build)
* Git

### Clone the Repository

```bash
git clone https://github.com/kishan4906/Kestrel.git
cd Kestrel
```

### Build

If using CMake:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

Alternatively, use the project's provided build configuration if you are building with another toolchain.

## ♟️ Running Kestrel

Once compiled, launch the engine executable.

For example:

```bash
./kestrel
```

On Windows:

```powershell
.\kestrel.exe
```

Kestrel communicates through standard input/output using UCI commands.

Example:

```text
uci
isready
position startpos
go depth 6
```

The engine will eventually return something similar to:

```text
bestmove e2e4
```

## 🧪 Example UCI Session

```text
> uci

< id name Kestrel
< uciok

> isready

< readyok

> position startpos

> go depth 6

< info depth 6 ...
< bestmove e2e4
```

## 🎯 Project Goals

Kestrel was created to explore the fundamentals of chess-engine development and high-performance game-tree search.

The main goals are:

* Understand chess-engine architecture
* Implement a chess position and move system
* Build a search algorithm from scratch
* Experiment with evaluation techniques
* Learn the UCI protocol
* Create an engine that can be integrated into other applications

## 🛣️ Future Improvements

Potential areas for further development:

* [ ] Transposition tables
* [ ] Zobrist hashing
* [ ] Improved move ordering
* [ ] Quiescence search
* [ ] Principal variation search
* [ ] Opening book
* [ ] Endgame tablebase support
* [ ] Improved evaluation heuristics
* [ ] Multi-threaded search
* [ ] Performance benchmarking
* [ ] Strength testing through engine matches

## 🔗 Related Project

### Corvus — AI Chess Coach

Kestrel provides the **chess analysis layer**, while Corvus provides the **AI coaching layer**.

Together:

```text
Kestrel
  ↓
Chess calculation
  ↓
Position evaluation
  ↓
Best move
  ↓
Corvus
  ↓
LLM explanation
  ↓
Human-friendly chess coaching
```

👉 **[Corvus GitHub Repository](https://github.com/kishan4906/Corvus)**

## 👨‍💻 Author

**Kishan Kushwaha**

Kestrel is a from-scratch C++17 chess-engine project focused on search, evaluation, and UCI-based engine integration.

---

⭐ If you find Kestrel interesting, consider giving the repository a star.
