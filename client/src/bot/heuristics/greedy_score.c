/**
 * @file        greedy_score.c
 * @brief       Try to get as many lines cleared as possible this turn
 */

#include "../ai.h"

#include <math.h>
#include "../ai_utils.h"

void greedy_score(tetris_board* board, move_candidate* candidate) {
    
    // Fake place the piece
    char temp_board[board->rows * board->cols];
    make_pseudo_board(board, temp_board, candidate);
    
    // Evaluate the board
    int lines_cleared = 0;
    pseudo_check_for_clears(temp_board, board->rows, board->cols, &lines_cleared);
    
    // Prefer moves that clear more lines
    // But include y level so that if there are 
    // no possible line clears this turn, play something that maybe 
    // gets you closer to a line clear next turn
    candidate->score = (float) lines_cleared * 1000.0f + (float) candidate->pos.y;
}