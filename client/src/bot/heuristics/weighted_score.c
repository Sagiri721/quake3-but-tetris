/**
 * @file        weighted_score.c
 * @brief       This algorithm evaluates the board using a weighted scoring system
 *              It takes into account:
 *                  - Total height
 *                  - Cleared lines
 *                  - Holes
 *                  - Bumpiness
 */

#include "../ai.h"

#include <math.h>
#include "../ai_utils.h"

const float weights[4] = {
    -0.510066f, // Total height
     0.760666f, // Lines cleared
    -0.35663f,  // Holes
    -0.184483f  // Bumpiness
};

/**
 * Sum of the distance from the bottom to the highest filled cell in each column
 */
void calculate_total_height(tetris_board* board, char* temp_board, int* height) {
    *height = 0;
    for (size_t i = 0; i < board->cols; i++) {
        
        for (size_t j = 0; j < board->rows; j++) {
            if (temp_board[i * board->rows + j] != 0) {
                *height += (board->rows - j);
                break;
            }
        }
    }
}

/**
 * A hole is an empty space with atleast one tile in the same column above it
 */
void count_holes(tetris_board* board, char* temp_board, int* holes) {
    *holes = 0;
    for (size_t i = 0; i < board->cols; i++) {
        char block_found = 0;
        for (size_t j = 0; j < board->rows; j++) {
            if (temp_board[i * board->rows + j] != 0) {
                block_found = 1;
            } else {
                if (block_found) {
                    (*holes)++;
                }
            }
        }
    }
}

/**
 * The bumpiness is the sum of the absolute difference in height between all 
 * two consecutive columns. The bumpiness measures the variation of its column heights.
 */
void calculate_bumpiness(tetris_board* board, char* temp_board, int* bumpiness) {

    int heights[board->cols];
    for (size_t i = 0; i < board->cols; i++)
    {
        heights[i] = 0;
        for (size_t j = 0; j < board->rows; j++)
        {
            if (temp_board[i * board->rows + j] != 0) {
                heights[i] = board->rows - j;
                break;
            }
        }
    }
    
    *bumpiness = 0;
    for (size_t i = 0; i < board->cols - 1; i++)
    {
        *bumpiness += abs(heights[i] - heights[i + 1]);
    }
}

/**
 * Shoutouts https://perso.esiee.fr/~chierchg/optimization/content/03/intro.html
 */
void weighted_score(tetris_board* board, move_candidate* candidate) {
    
    // Fake place the piece
    char temp_board[board->rows * board->cols];
    make_pseudo_board(board, temp_board, candidate);
    
    // Find total height
    int total_height = 0;
    calculate_total_height(board, temp_board, &total_height);

    // Find number of complete lines
    int lines_cleared = 0;
    pseudo_check_for_clears(temp_board, board->rows, board->cols, &lines_cleared);

    // Find number of holes
    int holes = 0;
    count_holes(board, temp_board, &holes);

    // Find bumpiness
    int bumpiness = 0;
    calculate_bumpiness(board, temp_board, &bumpiness);

    candidate->score = weights[0] * (float) total_height +
                       weights[1] * (float) lines_cleared +
                       weights[2] * (float) holes +
                       weights[3] * (float) bumpiness;
}