/**
 * @file        ai_utils.h
 * @brief       Utility functions for Tetris AI operations.
 */

#ifndef AI_UTILS_H
#define AI_UTILS_H

#include "../core/tetris.h"
#include "ai.h"

void pseudo_check_for_clears(char* game, int rows, int cols, int* lines_cleared);
void pseudo_place_tetromino(char* board, int rows, int cols, tetromino* piece);
void make_pseudo_board(tetris_board* board, char* temp_board, move_candidate* candidate);
#endif