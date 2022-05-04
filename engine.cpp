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
unsigned long greedy_call_count;
unsigned long minmax_call_count;
unsigned long parallel_call_count;

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

static void display_position( thc::ChessRules &cr, const std::string &description )
{
    std::string fen = cr.ForsythPublish();
    std::string s = cr.ToDebugStr();
    printf( "%s\n", description.c_str() );
    printf( "FEN (Forsyth Edwards Notation) = %s\n", fen.c_str() );
    printf( "Position = %s\n", s.c_str() );
}

double getGreedyMove(thc::ChessRules &cr, thc::Move &best_move, int max_depth, bool white)
{
    thc::MOVELIST legal_moves;
    cr.GenLegalMoveList(&legal_moves);
    int best_move_idx = 0;
    if (legal_moves.count > 0)
        best_move_idx = rand() % legal_moves.count;
    double best_score = white ? -1000 : 1000;
    double curr_score;

    // display_position(cr, "Position passed to getGreedyMove");
    int i;
    for (i = 0; i < legal_moves.count; i++)
    {
        cr.PushMove(legal_moves.moves[i]);
        curr_score = evaluatePositionFast(cr);
        cr.PopMove(legal_moves.moves[i]);
        if (white)
        {   
            if (best_score < curr_score)
            {
                best_move_idx = i;
                best_score = curr_score;
            }
        }
        if (!white && (best_score > curr_score))
        {
            best_move_idx = i;
            best_score = curr_score;
        }
    }
    best_move = legal_moves.moves[best_move_idx];
    #pragma omp critical
    {
        // printf("Adding %d moves to count\n", legal_moves.count);
        move_count += i;
        greedy_call_count += 1;
    }
    return best_score;
}

double minMaxIteration(thc::ChessRules &cr, thc::Move &best_move, int curr_depth, double alpha, double beta, bool white)
{
    minmax_call_count += 1;
    if (curr_depth == 0)
    {
        return getGreedyMove(cr, best_move, 0, white);
    }

    thc::MOVELIST legal_moves;
    cr.GenLegalMoveList(&legal_moves);
    int best_move_idx = 0;
    if (legal_moves.count > 0)
        best_move_idx = rand() % legal_moves.count;
    double best_score = white ? -1000 : 1000;
    double curr_score;

    // printf("Curr depth: %d, %d legal moves\n", curr_depth, legal_moves.count);

    if (white) // max
    {
        for (int i = 0; i < legal_moves.count; i++)
        {
            cr.PushMove(legal_moves.moves[i]);
            curr_score = minMaxIteration(cr, best_move, curr_depth - 1, alpha, beta, !white);
            cr.PopMove(legal_moves.moves[i]);
            if (best_score < curr_score)
            {
                best_move_idx = i;
                best_score = curr_score;
                alpha = (best_score > alpha) ? best_score : alpha;
            }
            if (best_score >= beta)
                break;
        }
    }
    else // min
    {
        for (int i = 0; i < legal_moves.count; i++)
        {
            cr.PushMove(legal_moves.moves[i]);
            curr_score = minMaxIteration(cr, best_move, curr_depth - 1, alpha, beta, !white);
            cr.PopMove(legal_moves.moves[i]);
            if (best_score > curr_score)
            {
                best_move_idx = i;
                best_score = curr_score;
                beta = (best_score < beta) ? best_score : beta;
            }
            if (best_score <= alpha)
                break;
        }
    }

    best_move = legal_moves.moves[best_move_idx];
    return best_score;
}

void getMinMaxMove(thc::ChessRules &cr, thc::Move &best_move, int max_depth, bool white)
{
    double best_eval = minMaxIteration(cr, best_move, max_depth, DBL_MIN, DBL_MAX, white);
    printf("Best move for %s has evaluation %lf\n", (white) ? "WHITE": "BLACK", best_eval);
}


double getGreedyMoveParallel(thc::ChessRules &cr, thc::Move &best_move, int max_depth, int n_threads, bool white)
{
    thc::MOVELIST legal_moves;
    cr.GenLegalMoveList(&legal_moves);
    int best_move_idx = 0;
    if (legal_moves.count > 0)
        best_move_idx = rand() % legal_moves.count;
    double best_score = white ? -1000 : 1000;

    // display_position(cr, "Position passed to getGreedyMove");

    volatile bool flag = false;
    #pragma omp parallel for default(shared) num_threads(2)
    for (int i = 0; i < legal_moves.count; i++)
    {
        if (flag) continue;

        thc::ChessRules local_cr = cr;
        local_cr.PushMove(legal_moves.moves[i]);
        double curr_score = evaluatePositionFast(local_cr);
        local_cr.PopMove(legal_moves.moves[i]);
        #pragma omp critical
        {
            if (white && (best_score < curr_score))
            {
                best_move_idx = i;
                best_score = curr_score;
            }
            if (!white && (best_score > curr_score))
            {
                best_move_idx = i;
                best_score = curr_score;
            }
        }
    }

    best_move = legal_moves.moves[best_move_idx];
    #pragma omp critical
    {
        // printf("Adding %d moves to count\n", legal_moves.count);
        move_count += legal_moves.count;
        greedy_call_count += 1;
    }
    return best_score;
}


double minMaxIterationParallel(thc::ChessRules cr, thc::Move &best_move, int curr_depth, double alpha, double beta, int n_threads, bool white)
{
    parallel_call_count += 1;
    if (curr_depth == 0)
    {
        return getGreedyMoveParallel(cr, best_move, 0, 2, white);
    }

    thc::MOVELIST legal_moves;
    cr.GenLegalMoveList(&legal_moves);
    int best_move_idx = 0;
    if (legal_moves.count > 0)
        best_move_idx = rand() % legal_moves.count;
    double best_score = white ? -1000 : 1000;

    // display_position(cr, "Position passed to getGreedyMove");

    thc::Move local_best_move = best_move;
    volatile bool flag = false;
    int n_workers = 1;
    if (curr_depth == 1) {
        n_workers = n_threads / 2;
    }
    if (white) // max
    {
        #pragma omp parallel for default(shared) private(local_best_move) num_threads(n_workers)
        for (int i = 0; i < legal_moves.count; i++)
        {
            if (flag)
                continue;

            thc::ChessRules local_cr = cr;
            local_cr.PushMove(legal_moves.moves[i]);
            double curr_score = minMaxIteration(local_cr, local_best_move, curr_depth - 1, alpha, beta, !white);
            local_cr.PopMove(legal_moves.moves[i]);
            #pragma omp critical
            {
                if (best_score < curr_score)
                {
                    best_move_idx = i;
                    best_score = curr_score;
                    alpha = (best_score > alpha) ? best_score : alpha;
                }
                if (best_score >= beta)
                    flag = true;
            }
        }
    }
    else // min
    {
        #pragma omp parallel for default(shared) private(local_best_move) num_threads(n_workers)
        for (int i = 0; i < legal_moves.count; i++)
        {
            if (flag)
                continue;

            thc::ChessRules local_cr = cr;
            local_cr.PushMove(legal_moves.moves[i]);
            double curr_score = minMaxIteration(local_cr, local_best_move, curr_depth - 1, alpha, beta, !white);
            local_cr.PopMove(legal_moves.moves[i]);
            #pragma omp critical
            {
                if (best_score > curr_score)
                {
                    best_move_idx = i;
                    best_score = curr_score;
                    beta = (best_score < beta) ? best_score : beta;
                }
                if (best_score <= alpha)
                    flag = true;
            }
        }
    }

    best_move = legal_moves.moves[best_move_idx];
    #pragma omp critical
    {
        // printf("Adding %d moves to count\n", legal_moves.count);
        move_count += legal_moves.count;
        greedy_call_count += 1;
    }
    return best_score;
}

void getMinMaxMoveParallel(thc::ChessRules &cr, thc::Move &best_move, int max_depth, int n_threads, bool white)
{
    double best_eval = minMaxIterationParallel(cr, best_move, max_depth, DBL_MIN, DBL_MAX, n_threads, white);
    printf("Best move for %s has evaluation %lf\n", (white) ? "WHITE": "BLACK", best_eval);
}

void getBestMove(thc::ChessRules &cr, thc::Move &best_move, int engine_mode, int max_depth, int n_threads, bool white)
{
    using namespace std::chrono;
    auto start = high_resolution_clock::now();
    
    move_count = 0;
    greedy_call_count = 0;
    minmax_call_count = 0;
    parallel_call_count = 0;
    
    if (engine_mode == RANDOM_MOVE)
        getRandomMove(cr, best_move, max_depth, white);
    else if (engine_mode == GREEDY_MOVE)
        getGreedyMove(cr, best_move, max_depth, white);
    else if (engine_mode == MINMAX_MOVE)
        getMinMaxMove(cr, best_move, max_depth, white);
    else
        getMinMaxMoveParallel(cr, best_move, max_depth, n_threads, white);

    auto stop = high_resolution_clock::now();
    printf("Considered %lu moves in %ldms\n", move_count, duration_cast<milliseconds>(stop - start).count());
    printf("Greedy call count: %lu\n", greedy_call_count);
    printf("Minmax call count: %lu\n", minmax_call_count);
    printf("Parallel call count: %lu\n", parallel_call_count);
}