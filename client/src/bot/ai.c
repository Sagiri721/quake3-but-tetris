/**
 * @file        ai.c
 * @brief       AI bot handling
 */

#include "ai.h"

#include <math.h>

// Get all possible positions for a tetromino on the board
// Given the current rotation of the piece
void get_all_possible_positions_for_rotation(tetris_board* game, tetromino* piece, evaluation_function eval, move_candidate* result) {

    // Copy original piece
    tetromino test_piece = *piece;

    int farLeft = -4;
    test_piece.pos.x = farLeft;

    // Find the furthest left position
    while (move_tetromino(game, &test_piece, 0, 0) != 1)
        test_piece.pos.x += 1;

    // Now try all positions from left to right
    for (;;) {

        position testing_position = calculate_drop_preview(&test_piece, game);

        // Evaluate this position
        move_candidate current = {
            .pos = testing_position,
            .rot = test_piece.rot,
            .score = 0.0f
        };

        eval(game, &current);

        // If this position is better, store it
        if (current.score > result->score) {
            *result = current;
        }

        if (move_tetromino(game, &test_piece, 1, 0) != 1)
            break;
    }
}

void get_all_possible_positions(tetris_board* game, evaluation_function eval, move_candidate* result) {

    tetromino piece = game->current;

    // Test all 4 rotations
    for (int i = 0; i < 4; i++) {

        get_all_possible_positions_for_rotation(game, &piece, eval, result);
        piece.rot = (piece.rot + 1) % 4;
    }
}

move_candidate decide_next_move(tetris_board* game) {

    move_candidate best = {
        .pos = {0, 0},
        .rot = 0,
        .score = -INFINITY
    };

    get_all_possible_positions(game, &weighted_score, &best);
    return best;
}