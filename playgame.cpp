/*

    Simple chess game driver

    Arguments
    -w = True if white player is a human (else bot)
    -b = True if black player is a human (else bot)
    -e = engine type for bots (might want to split into seperate flags for each player)
    -t  = max amount of time for engine to think per move

 */

#include <stdio.h>
#include <unistd.h>
#include <string>
#include <vector>
#include "thc.h"
#include "engine.h"
#include <iostream>

#define HUMAN  0
#define ENGINE 1

void display_position( thc::ChessRules &cr, const std::string &description )
{
    std::string fen = cr.ForsythPublish();
    std::string s = cr.ToDebugStr();
    printf( "%s\n", description.c_str() );
    printf( "FEN (Forsyth Edwards Notation) = %s\n", fen.c_str() );
    printf( "Position = %s\n", s.c_str() );
}

int main(int argc, char *argv[]) {
    int white_player = HUMAN;
    int black_player = ENGINE;
    int engine_type = 0;
    int time_per_move = 10;
    int opt = 0;
    do {
        opt = getopt(argc, argv, "w:b:e:t");
        switch (opt) {
            case 'w':
                white_player = atoi(optarg);
                break;
            case 'b':
                black_player = atoi(optarg);
                break;
            case 'e':
                engine_type = atoi(optarg);
                break;
            case 't':
                time_per_move = atoi(optarg);
                break;
            default:
                break;
        }
    } while (opt != -1);

    thc::ChessRules cr;
    thc::Move mv;
    std::string mv_txt;
    std::string board_desc;
    thc::TERMINAL eval_penultimate_position;
    int move_was_legal = 0;
    display_position(cr, "");
    do {
        if (white_player == HUMAN)
        {
            // get human move
            do {
                std::cout << "White to Move: " << std::endl;
                std::cin >> mv_txt;
                move_was_legal = mv.NaturalIn(&cr, mv_txt.c_str());
                if (!move_was_legal)
                {
                    std::cout << mv_txt + " is not a legal move" << std::endl;
                }
            } while (!move_was_legal);
        }
        else
        {
            // get engine move
            printf("Unimplemented\n");
        }

        // play move, print board
        cr.PlayMove(mv);
        display_position(cr, "");
        cr.Evaluate(eval_penultimate_position);
        if (eval_penultimate_position != thc::NOT_TERMINAL)
            break;

        if (black_player == HUMAN) {
            // get human move
            do {
                std::cout << "Black to Move: " << std::endl;
                std::cin >> mv_txt;
                move_was_legal = mv.NaturalIn(&cr, mv_txt.c_str());
                if (!move_was_legal)
                {
                    std::cout << mv_txt + " is not a legal move" << std::endl;
                }
            } while (!move_was_legal);
        } else {
            // get engine move
            printf("Unimplemented\n");
        }

        // input move
        cr.PlayMove(mv);
        display_position(cr, "");
        cr.Evaluate(eval_penultimate_position);
        if (eval_penultimate_position != thc::NOT_TERMINAL)
            break;

    } while (eval_penultimate_position == thc::NOT_TERMINAL);
    std::cout << "Game finished" << std::endl;
    switch (eval_penultimate_position) {
        case thc::TERMINAL_BCHECKMATE:
            std::cout << "White wins by checkmate" << std::endl;
            break;
        case thc::TERMINAL_WCHECKMATE:
            std::cout << "Black wins by checkmate" << std::endl;
            break;
        case thc::TERMINAL_WSTALEMATE:
            std::cout << "White is stalemated" << std::endl;
            break;
        case thc::TERMINAL_BSTALEMATE:
            std::cout << "Black is stalemated" << std::endl;
            break;
    }
}