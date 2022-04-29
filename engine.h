#ifndef ENGINE_H
#define ENGINE_H
#include "thc.h"

#define GREEDY_MOVE   0
#define RANDOM_MOVE   1
#define MINMAX_MOVE   2
#define PARALLEL_MOVE 3

/*
    getLegalMoves(game) // implemented by the API but might want to parallelize
    getBestMove(game, legal_moves)
 */

void getBestMove(thc::ChessRules &cr, thc::Move &best_move, int engine_mode, int max_depth, int n_threads, bool white);

#endif