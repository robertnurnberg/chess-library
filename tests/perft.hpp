#pragma once

#include <chrono>
#include <iomanip>
#include <sstream>
#include <tuple>

#include "../src/chess.hpp"

using namespace chess;
using namespace std::chrono;

class Perft {
   public:
    uint64_t perft(int depth) {
        Movelist moves;
        movegen::legalmoves(moves, board_);

        if (depth == 1) {
            return moves.size();
        }

        U64 nodes = 0;

        for (const auto& move : moves) {
            board_.makeMove(move);
            nodes += perft(depth - 1);
            board_.unmakeMove(move);
        }

        return nodes;
    }

    void benchPerft(Board& board, int depth, uint64_t expected_node_count) {
        board_ = board;

        const auto t1 = high_resolution_clock::now();
        const auto nodes = perft(depth);
        const auto t2 = high_resolution_clock::now();
        const auto ms = duration_cast<milliseconds>(t2 - t1).count();

        std::stringstream ss;
        // clang-format off
        ss << "depth " << std::left << std::setw(2) << depth 
           << " time " << std::setw(5) << ms
           << " nodes " << std::setw(12) << nodes 
           << " nps " << std::setw(9) << (nodes * 1000) / (ms + 1) 
           << " fen " << std::setw(87) << board_.getFen();
        // clang-format on

        if (nodes != expected_node_count) {
            std::cout << ss.str() << std::endl;

            throw std::runtime_error("Wrong node count, expected " +
                                     std::to_string(expected_node_count) + " got " +
                                     std::to_string(nodes));
        }

        std::cout << ss.str() << std::endl;
    }

   private:
    Board board_;
};

struct Test {
    std::string fen;
    uint64_t expected_node_count;
    int depth;
};

int runPerftTest() {
    const Test test_positions[] = {
        {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 3195901860, 7},
        {"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ", 193690690, 5},
        {"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ", 178633661, 7},
        {"r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 706045033, 6},
        {"rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 89941194, 5},
        {"r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 1", 164075551, 5}};

    const Test test_positions_960[] = {
        {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w AHah - 0 1", 119060324ull, 6},
        {"1rqbkrbn/1ppppp1p/1n6/p1N3p1/8/2P4P/PP1PPPP1/1RQBKRBN w FBfb - 0 9", 191762235ull, 6},
        {"rbbqn1kr/pp2p1pp/6n1/2pp1p2/2P4P/P7/BP1PPPP1/R1BQNNKR w HAha - 0 9", 924181432ull, 6},
        {"rqbbknr1/1ppp2pp/p5n1/4pp2/P7/1PP5/1Q1PPPPP/R1BBKNRN w GAga - 0 9", 308553169ull, 6},
        {"4rrb1/1kp3b1/1p1p4/pP1Pn2p/5p2/1PR2P2/2P1NB1P/2KR1B2 w D - 0 21", 872323796ull, 6},
        {"1rkr3b/1ppn3p/3pB1n1/6q1/R2P4/4N1P1/1P5P/2KRQ1B1 b Dbd - 0 14", 2678022813ull, 6}};

    Perft perft;

    for (const auto& test : test_positions) {
        Board board(test.fen);
        perft.benchPerft(board, test.depth, test.expected_node_count);
    }

    for (const auto& test : test_positions_960) {
        Board board(test.fen);
        board.set960(true);

        perft.benchPerft(board, test.depth, test.expected_node_count);
    }

    return 0;
}