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

void run_benchmark(thc::ChessRules &cr, thc::Move &mv, int depth, bool white)
{
    getBestMove(cr, mv, MINMAX_MOVE, depth, white);
}

int main(int argc, char *argv[])
{
    thc::ChessRules cr;
    thc::Move mv;
    int test = 0;
    bool white = true;
    bool skip_sequential = false; // doesn't do anything yet
    int depth = 4;

    int opt = 0;
    do {
        opt = getopt(argc, argv, "t:d:s:");
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
        default:
            white = true;
            break;
    }
    printf("Running benchmark %d at depth %d\n", test, depth);
    display_position(cr, "Starting Position");

    using namespace std::chrono;
    auto start = high_resolution_clock::now();
    
    run_benchmark(cr, mv, depth, white);

    auto stop = high_resolution_clock::now();
    printf("Found next move '%s' in %ldms\n", mv.NaturalOut(&cr).c_str(), duration_cast<milliseconds>(stop - start).count());
}