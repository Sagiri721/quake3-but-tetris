/**
 * @file        ai_utils.c
 * @brief       AI utility functions
 */
#include "ai_utils.h"

#include <string.h>

void pseudo_check_for_clears(char* game, int rows, int cols, int* lines_cleared) {

    *lines_cleared = 0;
    for (int y = 0; y < rows; y++) {
        char full = 1;

        // Check if row is full
        for (int x = 0; x < cols; x++) {
            if (game[x * rows + y] == 0) {
                full = 0;
            }
        }

        if (full) (*lines_cleared)++;
    }
}

void pseudo_place_tetromino(char* board, int rows, int cols, tetromino* piece) {

    for (int i = 0; i < TETRIS; i++) {
        position cell_position = TETROMINOS[piece->type][piece->rot][i];
        int cell_x = cell_position.x + piece->pos.x;
        int cell_y = cell_position.y + piece->pos.y;

        //game->board[cell_x * game->cols + cell_y] = piece->type + 1;
        board[cell_x * rows + cell_y] = piece->type + 1;
    }
}

void make_pseudo_board(tetris_board* board, char* temp_board, move_candidate* candidate) {

    memcpy(temp_board, board->board, board->rows * board->cols);

    tetromino test_piece = board->current;
    test_piece.pos = candidate->pos;
    test_piece.rot = candidate->rot;
    pseudo_place_tetromino(temp_board, board->rows, board->cols, &test_piece);
}