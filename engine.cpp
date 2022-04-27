#include "thc.h"
#include "engine.h"
#include <stdlib.h>
#include <chrono>
#include <cfloat>

enum {
    PAWN = 0,
    ROOK = 1,
    KNIGHT = 2,
    BISHOP = 3,
    QUEEN = 4,
    KING = 5,
    NUM_PIECES = 6,
};

// debug variables
unsigned long move_count;

static void getMaterialCount(thc::ChessRules &cr, uint8_t *whitePieces, uint8_t *blackPieces)
{
    for (int i = 0; i < 64; i++)
    {
        switch (cr.squares[i]) {
            case 'p':
                ++blackPieces[PAWN];
                break;
            case 'r':
                ++blackPieces[ROOK];
                break;
            case 'n':
                ++blackPieces[KNIGHT];
                break;
            case 'b':
                ++blackPieces[BISHOP];
                break;
            case 'k':
                ++blackPieces[KING];
                break;
            case 'q':
                ++blackPieces[QUEEN];
                break;

            case 'P':
                ++whitePieces[PAWN];
                break;
            case 'R':
                ++whitePieces[ROOK];
                break;
            case 'N':
                ++whitePieces[KNIGHT];
                break;
            case 'B':
                ++whitePieces[BISHOP];
                break;
            case 'K':
                ++whitePieces[KING];
                break;
            case 'Q':
                ++whitePieces[QUEEN];
                break;
        }
    }
}

// evaluation functions from thc library are very complicated
// simple material evaluation, doesn't consider positional features at all
static double evaluatePositionFast(thc::ChessRules &cr)
{
    // count material, scaled by type of piece
    uint8_t whitePieces[NUM_PIECES] = {0};
    uint8_t blackPieces[NUM_PIECES] = {0};
    getMaterialCount(cr, whitePieces, blackPieces);

    double score = 0;
    score += 200 * (whitePieces[KING] - blackPieces[KING]);
    score += 9 * (whitePieces[QUEEN] - blackPieces[QUEEN]);
    score += 5 * (whitePieces[ROOK] - blackPieces[ROOK]);
    score += 3.1 * (whitePieces[BISHOP] - blackPieces[BISHOP]);
    score += 3 * (whitePieces[KNIGHT] - blackPieces[KNIGHT]);
    score += 1 * (whitePieces[PAWN] - blackPieces[PAWN]);

    // could consider piece mobility, but not cheap to compute

    return score;
}

void getRandomMove(thc::ChessRules &cr, thc::Move &best_move, int max_depth, bool white)
{
    thc::MOVELIST legal_moves;
    cr.GenLegalMoveList(&legal_moves);
    int best_move_idx = 0;
    if (legal_moves.count > 0)
        best_move_idx = rand() % legal_moves.count;
    best_move = legal_moves.moves[best_move_idx];
    printf("Using random move %d out of %d\n", best_move_idx, legal_moves.count);
}

double getGreedyMove(thc::ChessRules &cr, thc::Move &best_move, int max_depth, double alpha, double beta, bool white)
{
    thc::MOVELIST legal_moves;
    cr.GenLegalMoveList(&legal_moves);
    int best_move_idx = 0;
    if (legal_moves.count > 0)
        best_move_idx = rand() % legal_moves.count;
    double best_score = white ? -1000 : 1000;
    double curr_score;
    for (int i = 0; i < legal_moves.count; i++)
    {
        cr.PushMove(legal_moves.moves[i]);
        curr_score = evaluatePositionFast(cr);
        cr.PopMove(legal_moves.moves[i]);
        if (white && (best_score < curr_score))
        {
            best_move_idx = i;
            best_score = curr_score;
            alpha = (best_score > alpha) ? best_score : alpha;
        }
        if (!white && (best_score > curr_score))
        {
            best_move_idx = i;
            best_score = curr_score;
            beta = (best_score < beta) ? best_score : beta;
        }
        if (beta <= alpha)
        {
            break;
        }
    }
    best_move = legal_moves.moves[best_move_idx];
    move_count += legal_moves.count;
    return best_score;
}

double minMaxIteration(thc::ChessRules &cr, thc::Move &best_move, int curr_depth, double alpha, double beta, bool white)
{
    if (curr_depth == 0)
    {
        return getGreedyMove(cr, best_move, 0, alpha, beta, white);
    }

    // printf("Curr depth: %d\n", curr_depth);

    thc::MOVELIST legal_moves;
    cr.GenLegalMoveList(&legal_moves);
    int best_move_idx = 0;
    if (legal_moves.count > 0)
        best_move_idx = rand() % legal_moves.count;
    double best_score = white ? -1000 : 1000;
    double curr_score;

    for (int i = 0; i < legal_moves.count; i++)
    {
        cr.PushMove(legal_moves.moves[i]);
        curr_score = minMaxIteration(cr, best_move, curr_depth - 1, alpha, beta, !white);
        cr.PopMove(legal_moves.moves[i]);
        if (white && (best_score < curr_score))
        {
            best_move_idx = i;
            best_score = curr_score;
            alpha = (best_score > alpha) ? best_score : alpha;
        }
        if (!white && (best_score > curr_score))
        {
            best_move_idx = i;
            best_score = curr_score;
            beta = (best_score < beta) ? best_score : beta;
        }
        if (beta <= alpha)
        {
            // printf("Depth %d: Pruning with beta=%lf, alpha=%lf\n", curr_depth, beta, alpha);
            break;
        }
    }

    // not certain if this is correct because of pass by reference stuff
    best_move = legal_moves.moves[best_move_idx];
    return best_score;
}

void getMinMaxMove(thc::ChessRules &cr, thc::Move &best_move, int max_depth, bool white)
{
    
    double best_eval = minMaxIteration(cr, best_move, max_depth, DBL_MIN, DBL_MAX, white);
    printf("Best move for %s has evaluation %lf\n", (white) ? "WHITE": "BLACK", best_eval);
}

void getBestMove(thc::ChessRules &cr, thc::Move &best_move, int engine_mode, int max_depth, bool white)
{
    using namespace std::chrono;
    auto start = high_resolution_clock::now();
    move_count = 0;
    if (engine_mode == RANDOM_MOVE)
        getRandomMove(cr, best_move, max_depth, white);
    else if (engine_mode == GREEDY_MOVE)
        getGreedyMove(cr, best_move, max_depth, DBL_MIN, DBL_MAX, white);
    else
        getMinMaxMove(cr, best_move, max_depth, white);

    auto stop = high_resolution_clock::now();
    printf("Considered %lu moves in %ldms\n", move_count, duration_cast<milliseconds>(stop - start).count());
}