/**
 * @file        tetris.c
 * @brief       Implementation of the tetris game
 */

#include "tetris.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

position TETROMINOS[NUM_TETROMINOS][NUM_ORIENTATIONS][TETRIS] = {
  // I
  {{{1, 0}, {1, 1}, {1, 2}, {1, 3}},
   {{0, 2}, {1, 2}, {2, 2}, {3, 2}},
   {{3, 0}, {3, 1}, {3, 2}, {3, 3}},
   {{0, 1}, {1, 1}, {2, 1}, {3, 1}}},
  // J
  {{{0, 0}, {1, 0}, {1, 1}, {1, 2}},
   {{0, 1}, {0, 2}, {1, 1}, {2, 1}},
   {{1, 0}, {1, 1}, {1, 2}, {2, 2}},
   {{0, 1}, {1, 1}, {2, 0}, {2, 1}}},
  // L
  {{{0, 2}, {1, 0}, {1, 1}, {1, 2}},
   {{0, 1}, {1, 1}, {2, 1}, {2, 2}},
   {{1, 0}, {1, 1}, {1, 2}, {2, 0}},
   {{0, 0}, {0, 1}, {1, 1}, {2, 1}}},
  // O
  {{{0, 1}, {0, 2}, {1, 1}, {1, 2}},
   {{0, 1}, {0, 2}, {1, 1}, {1, 2}},
   {{0, 1}, {0, 2}, {1, 1}, {1, 2}},
   {{0, 1}, {0, 2}, {1, 1}, {1, 2}}},
  // S
  {{{0, 1}, {0, 2}, {1, 0}, {1, 1}},
   {{0, 1}, {1, 1}, {1, 2}, {2, 2}},
   {{1, 1}, {1, 2}, {2, 0}, {2, 1}},
   {{0, 0}, {1, 0}, {1, 1}, {2, 1}}},
  // T
  {{{0, 1}, {1, 0}, {1, 1}, {1, 2}},
   {{0, 1}, {1, 1}, {1, 2}, {2, 1}},
   {{1, 0}, {1, 1}, {1, 2}, {2, 1}},
   {{0, 1}, {1, 0}, {1, 1}, {2, 1}}},
  // Z
  {{{0, 0}, {0, 1}, {1, 1}, {1, 2}},
   {{0, 2}, {1, 1}, {1, 2}, {2, 1}},
   {{1, 0}, {1, 1}, {2, 1}, {2, 2}},
   {{0, 1}, {1, 0}, {1, 1}, {2, 0}}},
};

/**
 * @brief Retrieve piece with RNG
 */
tetromino get_random_piece() {
    int r = rand() % NUM_TETROMINOS;
    return (tetromino) {
        .rot = 0,
        .type = r
    };
}

/**
 * @brief Place a tetromino at the top of the screen
 */
void place_piece_at_top(tetris_board* game, tetromino* piece) {
    piece->pos.x = floor(game->cols / 2.0) - 1;
    piece->pos.y = 0;
}

/**
 * @brief Retrieve the next piece and place it at the top
 */
void retrieve_next_piece(tetris_board* game) {
    game->current = game->next;
    game->next = get_random_piece();
    place_piece_at_top(game, &game->current);
}

/**
 * @brief Clear the tetris board
 */
void clear_board(tetris_board* game) {

    assert(game->board != NULL);

    for (int x = 0; x < game->rows; x++) {
        for (int y = 0; y < game->cols; y++) {
            game->board[x * game->cols + y] = 0;
        }
    }
}

/**
 * @brief Clear a row and move all rows above down by one
 */
void clear_row(tetris_board* game, int row) {

    assert(game->board != NULL);
    assert(row >= 0 && row < game->rows);

    // Clear the row
    for (int x = 0; x < game->rows; x++) {
        game->board[x * game->rows + row] = 0;
    }

    // Move all rows above down by one
    for (int y = row; y > 0; y--) {
        for (int x = 0; x < game->rows; x++) {
            game->board[x * game->rows + y] = game->board[x * game->rows + (y - 1)];
        }
    }
}

/**
 * @brief Check for full rows and clear them
 */
void check_for_clears(tetris_board* game) {

    for (int y = 0; y < game->rows; y++) {
        char full = 1;

        // Check if row is full
        for (int x = 0; x < game->cols; x++) {
            if (index_cell(game, x, y) == 0) {
                full = 0;
            }
        }

        if (full) {
            clear_row(game, y);
            game->points += 100;
        }
    }
}

/**
 * @brief Move a tetromino by dx, dy
 * @return 1 if move successful, 0 if ready to lock
 */
char move_tetromino(tetris_board* game, tetromino* piece, int dx, int dy) {
    int xx = piece->pos.x + dx;
    int yy = piece->pos.y + dy;

    // Check for collisions with existing blocks
    for (int i = 0; i < TETRIS; i++) {
        position cell_position = TETROMINOS[piece->type][piece->rot][i];
        int cell_x = cell_position.x + xx;
        int cell_y = cell_position.y + yy;

        // Check bounds
        if (cell_x < 0 || cell_x >= game->cols || cell_y < 0)
            return -1;

        // Check collision with existing blocks
        if (index_cell(game, cell_x, cell_y) != 0 || cell_y >= game->rows)
            return 0;
    }

    // No collisions, move piece
    piece->pos.x = xx;
    piece->pos.y = yy;

    return 1;
}

/**
 * @brief Lock a piece into the board and retrieve the next piece
 */
void lock_piece(tetris_board* game, tetromino* piece) {

    // Lock piece into the board
    for (int i = 0; i < TETRIS; i++) {
        position cell_position = TETROMINOS[piece->type][piece->rot][i];
        int cell_x = cell_position.x + piece->pos.x;
        int cell_y = cell_position.y + piece->pos.y;

        //game->board[cell_x * game->cols + cell_y] = piece->type + 1;
        game->board[cell_x * game->rows + cell_y] = piece->type + 1;
    }

    check_for_clears(game);
    retrieve_next_piece(game);
}

/**
 * @brief Apply gravity to the current piece
 */
void tetris_apply_gravity(tetris_board* game) {

    static char fail_counter = 0;

    // Move current piece down by one
    if(!move_tetromino(game, &game->current, 0, 1)) {

        fail_counter++;

        // Piece placement grace
        if (fail_counter >= 2) {
            fail_counter = 0;
            lock_piece(game, &game->current);
        }
    }
}

void tetris_init(tetris_board* game, int rows, int cols) {
    game->rows = rows;
    game->cols = cols;

    game->points = 0;
    game->level = 1;

    game->has_hold = 0;
    game->current = get_random_piece();
    game->next = get_random_piece();

    game->speed = 0.2f;

    place_piece_at_top(game, &game->current);

    // Initialize field to empty
    game->board = malloc(rows * cols * sizeof(char*));
    if (game->board == NULL) {
        fprintf(stderr, "Failed to allocate memory for tetris board\n");
        exit(EXIT_FAILURE);
    }

    clear_board(game);
}

void tetris_update(tetris_board* game, float dt) {

    static float timer = 0.0f;
    timer += dt;

    if (timer > game->speed) {

        tetris_apply_gravity(game);

        // Reset timer
        timer = 0.0f;
    }

}

char index_cell(tetris_board* game, int x, int y) {
    
    assert(game->board != NULL);
    assert(x >= 0 && x <= game->cols);
    assert(y >= 0 && y <= game->rows);

    return game->board[x * game->rows + y];
}

void tetris_destroy(tetris_board *game) {

    assert(game->board != NULL);
    free(game->board);
}

// Tetris events
#define MOVE_COOLDOWN 0.08f
#define DROP_COOLDOWN 0.03f

void tetris_move(tetris_board* game, int direction, float dt) {
    static float move_timer = 0.0f;
    move_timer += dt;

    if (move_timer < MOVE_COOLDOWN) return;

    move_tetromino(game, &game->current, direction, 0);
    move_timer = 0.0f;
}

void tetris_rotate(tetris_board* game, float dt) {

    // Keep track of how many rotations we've tried
    // Only matters if we hit a wall while rotating
    static char rotations_tried = 0;
    assert(rotations_tried >= 0 && rotations_tried <= NUM_ORIENTATIONS);

    tetromino* piece = &game->current;
    static int old_rot = 0;

    // Store old rotation on first try only
    if (rotations_tried == 0) 
        old_rot = piece->rot;
    
    piece->rot = (piece->rot + NUM_ORIENTATIONS + 1) % NUM_ORIENTATIONS;

    assert(piece->rot >= 0 && piece->rot < NUM_ORIENTATIONS);

    // Check for collisions
    if (move_tetromino(game, piece, 0, 0) < 1) {
        // Attempt next rotation tro see if it works
        if (rotations_tried < NUM_ORIENTATIONS) {
            rotations_tried++;
            tetris_rotate(game, dt);
        } else {
            // Revert rotation
            piece->rot = old_rot;
            rotations_tried = 0;
        }
    }
}

void tetris_drop(tetris_board* game, float dt) {

    static float drop_timer = 0.0f;
    drop_timer += dt;

    if (drop_timer < DROP_COOLDOWN) return;

    tetris_apply_gravity(game);
    drop_timer = 0.0f;
}