/**
 * @file        ai.h
 * @brief       AI module for bot behavior and decision-making.
 */

#ifndef BOT_AI_H
#define BOT_AI_H

#include "../core/tetris.h"

// A potential candidate for the best move
typedef struct {
    position pos;
    int rot;
    float score;
} move_candidate;

typedef void (*evaluation_function)(tetris_board*, move_candidate*);

/**
 * @brief Decides the next move for the bot based on the current board state and piece to place.
 */
move_candidate decide_next_move(tetris_board* game);

// Decision algorithms
/**
 * @brief Evaluates a move candidate by checking for immediate line clears
 */
void greedy_score(tetris_board* board, move_candidate* candidate);

/**
 * @brief Evaluates a move candidate using positive weights for various board features
 */
void weighted_score(tetris_board* board, move_candidate* candidate);

#endif