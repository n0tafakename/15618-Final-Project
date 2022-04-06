#ifndef ENGINE_H
#define ENGINE_H
#include "thc.h"

/*
    getLegalMoves(game) // implemented by the API but might want to parallelize
    getBestMove(game, legal_moves)
 */

void getBestMove(thc::ChessRules &cr, thc::Move &best_move, int max_time, bool white);

#endif