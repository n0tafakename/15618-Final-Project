#include "thc.h"
#include "engine.h"
#include <stdlib.h>

#define RANDOM_MOVE 0

enum {
    PAWN = 0,
    ROOK = 1,
    KNIGHT = 2,
    BISHOP = 3,
    QUEEN = 4,
    KING = 5,
    NUM_PIECES = 6,
};

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
    score += 3 * (whitePieces[PAWN] - blackPieces[PAWN]);

    return score;
}

#if RANDOM_MOVE == 1
void getBestMove(thc::ChessRules &cr, thc::Move &best_move, int max_time)
{
    thc::MOVELIST legal_moves;
    cr.GenLegalMoveList(&legal_moves);
    int best_move_idx = rand() % legal_moves.count;
    best_move = legal_moves.moves[best_move_idx];
    printf("Using random move %d out of %d\n", best_move_idx, legal_moves.count);
}
#else
void getBestMove(thc::ChessRules &cr, thc::Move &best_move, int max_time, bool white)
{
    thc::MOVELIST legal_moves;
    cr.GenLegalMoveList(&legal_moves);
    int best_move_idx = 0;
    double best_score = white ? -1000 : 1000;
    double curr_score;
    for (int i = 0; i < legal_moves.count; i++)
    {
        cr.PushMove(legal_moves.moves[i]);
        curr_score = evaluatePositionFast(cr);
        if ((white && (best_score < curr_score)) || (!white && (best_score > curr_score)))
        {
            best_move_idx = i;
            best_score = curr_score;
        }
        cr.PopMove(legal_moves.moves[i]);
    }
    best_move = legal_moves.moves[best_move_idx];
}
#endif