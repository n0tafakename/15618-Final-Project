#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include "thc.h"
#include "engine.h"
#include <chrono>
#include <cfloat>

/*
    TODO - Instantiate engine and board
    Manipulate board into a fixed position
    Measure time taken to compute the next move
*/
// test 0: No moves
// test 1: Najdorf
void display_position( thc::ChessRules &cr, const std::string &description )
{
    std::string fen = cr.ForsythPublish();
    std::string s = cr.ToDebugStr();
    printf( "%s\n", description.c_str() );
    printf( "FEN (Forsyth Edwards Notation) = %s\n", fen.c_str() );
    printf( "Position = %s\n", s.c_str() );
}

void setup_benchmark_najdorf(thc::ChessRules &cr)
{
    thc::Move mv;
    mv.NaturalIn(&cr, "e4");
    cr.PlayMove(mv);
    mv.NaturalIn(&cr, "c5");
    cr.PlayMove(mv);
    mv.NaturalIn(&cr, "Nf3");
    cr.PlayMove(mv);
    mv.NaturalIn(&cr, "d6");
    cr.PlayMove(mv);
    mv.NaturalIn(&cr, "d4");
    cr.PlayMove(mv);
    mv.NaturalIn(&cr, "cxd4");
    cr.PlayMove(mv);
    mv.NaturalIn(&cr, "Nxd4");
    cr.PlayMove(mv);
    mv.NaturalIn(&cr, "Nf6");
    cr.PlayMove(mv);
    mv.NaturalIn(&cr, "Nc3");
    cr.PlayMove(mv);
    mv.NaturalIn(&cr, "a6");
    cr.PlayMove(mv);
}

void setup_benchmark_puzzle(thc::ChessRules &cr)
{
    thc::Move mv;
    mv.NaturalIn(&cr, "g3");
    cr.PlayMove(mv);
    mv.NaturalIn(&cr, "e5");
    cr.PlayMove(mv);
    mv.NaturalIn(&cr, "Bg2");
    cr.PlayMove(mv);
    mv.NaturalIn(&cr, "d5");
    cr.PlayMove(mv);
    mv.NaturalIn(&cr, "e3");
    cr.PlayMove(mv);
    mv.NaturalIn(&cr, "c5");
    cr.PlayMove(mv);
    mv.NaturalIn(&cr, "c3");
    cr.PlayMove(mv);
    mv.NaturalIn(&cr, "f5");
    cr.PlayMove(mv);
    mv.NaturalIn(&cr, "Ne2");
    cr.PlayMove(mv);
    mv.NaturalIn(&cr, "Nc6");
    cr.PlayMove(mv);
    mv.NaturalIn(&cr, "O-O");
    cr.PlayMove(mv);
    mv.NaturalIn(&cr, "Nf6");
    cr.PlayMove(mv);
    mv.NaturalIn(&cr, "d4");
    cr.PlayMove(mv);
    mv.NaturalIn(&cr, "cxd4");
    cr.PlayMove(mv);
    mv.NaturalIn(&cr, "exd4");
    cr.PlayMove(mv);
    mv.NaturalIn(&cr, "e4");
    cr.PlayMove(mv);
    mv.NaturalIn(&cr, "f3");
    cr.PlayMove(mv);
    mv.NaturalIn(&cr, "Bd6");
    cr.PlayMove(mv);
    mv.NaturalIn(&cr, "Nf4");
    cr.PlayMove(mv);
    mv.NaturalIn(&cr, "Qc7");
    cr.PlayMove(mv);
    mv.NaturalIn(&cr, "fxe4");
    cr.PlayMove(mv);
    mv.NaturalIn(&cr, "fxe4");
    cr.PlayMove(mv);
}

void run_benchmark_sequential(thc::ChessRules &cr, thc::Move &mv, int depth, bool white)
{
    getBestMove(cr, mv, MINMAX_MOVE, depth, 1, white);
}

void run_benchmark_parallel(thc::ChessRules &cr, thc::Move &mv, int depth, int n_threads, bool white)
{
    getBestMove(cr, mv, PARALLEL_MOVE, depth, n_threads, white);
}

int main(int argc, char *argv[])
{
    thc::ChessRules cr;
    thc::Move mv;
    int test = 0;
    bool white = true;
    bool skip_sequential = false; // doesn't do anything yet
    int depth = 4;
    int n_threads = 1;

    int opt = 0;
    do {
        opt = getopt(argc, argv, "t:d:n:s");
        switch (opt) {
            case 't':
                test = std::stoi(optarg);
                break;
            case 'd':
                depth = std::stoi(optarg);
                break;
            case 's':
                skip_sequential = true;
                break;
            case 'n':
                n_threads = std::stoi(optarg);
            default:
                break;
        }
    } while (opt != -1);

    // setup test
    switch (test)
    {
        case 1:
            white = true;
            setup_benchmark_najdorf(cr);
            break;
        case 2:
            white = true;
            setup_benchmark_puzzle(cr);
            // correct move should be Nxd5
            break;
        default:
            white = true;
            break;
    }
    printf("Running benchmark %d at depth %d with %d threads\n", test, depth, n_threads);
    display_position(cr, "Starting Position");

    using namespace std::chrono;
    long sequential_dur;
    if (skip_sequential == false)
    {
        auto start = high_resolution_clock::now();
        run_benchmark_sequential(cr, mv, depth, white);
        auto stop = high_resolution_clock::now();
        sequential_dur = duration_cast<milliseconds>(stop - start).count();
        printf("Sequential: Found next move '%s' in %ldms\n\n", mv.NaturalOut(&cr).c_str(), sequential_dur);
    }

    auto start = high_resolution_clock::now();
    run_benchmark_parallel(cr, mv, depth, n_threads, white);
    auto stop = high_resolution_clock::now();
    long parallel_dur = duration_cast<milliseconds>(stop - start).count();
    printf("Parallel: Found next move '%s' in %ldms\n\n", mv.NaturalOut(&cr).c_str(), parallel_dur);

    if (skip_sequential == false)
        printf("Speedup: %lf\n", (double)((double)sequential_dur / (double)parallel_dur));
}