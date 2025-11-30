/**
 * @file        tetris.c
 * @brief       Implementation of the tetris game
 */

#include "tetris.h"
#include "input.h"

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

// Values from https://listfist.com/list-of-tetris-levels-by-speed-nes-ntsc-vs-pal
// Keep a constant 20 as the values are pulled from seconds until bottom
// So regardless of row size we keep the same speed scaling intentionally
float LEVEL_SPEED[NUM_LEVELS] = {
    14.398f / 20.0f, // Level 0
    12.798f / 20.0f, // Level 1
    11.598f / 20.0f, // Level 2
    9.999f / 20.0f, // Level 3
    8.799f / 20.0f, // Level 4
    7.199f / 20.0f, // Level 5
    5.999f / 20.0f, // Level 6
    4.399f / 20.0f, // Level 7
    2.800f / 20.0f, // Level 8
    2.000f / 20.0f, // Level 9
    1.600f / 20.0f, // Level 10
    1.600f / 20.0f, // Level 11
    1.600f / 20.0f, // Level 12
    1.200f / 20.0f, // Level 13
    1.200f / 20.0f, // Level 14
    1.200f / 20.0f, // Level 15
    0.800f / 20.0f, // Level 16
    0.800f / 20.0f, // Level 17
    0.800f / 20.0f, // Level 18
    0.400f / 20.0f, // Level 19
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
 * @brief Sample speed table based on current level
 */
float sample_speed_table(tetris_board* game) {
 
    unsigned int level = game->level;
    if (level >= NUM_LEVELS) // Level cap
        level = NUM_LEVELS - 1;
 
    return LEVEL_SPEED[level];
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

    game->lock_grace_counter = 0;
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

    // Release hold ability
    game->has_held = 0;
}

void tetris_init(tetris_board* game, int rows, int cols, unsigned int seed) {

    game->seed = seed;
    srand(seed);

    game->rows = rows;
    game->cols = cols;

    game->points = 0;
    game->level = 0;

    game->has_hold = 0;
    game->current = get_random_piece();
    game->next = get_random_piece();

    game->lock_grace_counter = 0;

    place_piece_at_top(game, &game->current);

    // Initialize field to empty
    game->board = malloc(rows * cols * sizeof(char*));
    if (game->board == NULL) {
        fprintf(stderr, "Failed to allocate memory for tetris board\n");
        exit(EXIT_FAILURE);
    }

    clear_board(game);
}

void tetris_process_input_queue(tetris_board* game, float dt) {
    int action;
    
    while (!is_empty(&input_queue)) {
        
        if(!dequeue(&input_queue, &action)){
            break;
        }

        switch (action) {
            case IE_MOVE_LEFT:
                tetris_move(game, -1, dt);
                break;
            case IE_MOVE_RIGHT:
                tetris_move(game, 1, dt);
                break;
            case IE_ROTATE_RIGHT:
                tetris_rotate(game, R_RIGHT, dt);
                break;
            case IE_ROTATE_LEFT:
                tetris_rotate(game, R_LEFT, dt);
                break;
            case IE_DROP:
                tetris_drop(game, dt);
                break;
            case IE_GRAVITY:
                tetris_apply_gravity(game);
                break;
            case IE_RESET:
                tetris_reset(game);
                break;
            case IE_HARD_DROP:
                tetris_hard_drop(game);
                break;
            case IE_HOLD:
                tetris_hold(game);
                break;
            default:
                break;
        }
    }
}

void tetris_update(tetris_board* game, float dt) {

    static float timer = 0.0f;
    timer += dt;

    float speed = sample_speed_table(game);
    if (timer > speed) {

        //tetris_apply_gravity(game);
        register_input(IE_GRAVITY);

        // Reset timer
        timer = 0.0f;
    }

    // Process input queue
    tetris_process_input_queue(game, dt);
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

void tetris_rotate(tetris_board* game, rot_dir dir, float dt) {

    // Keep track of how many rotations we've tried
    // Only matters if we hit a wall while rotating
    static char rotations_tried = 0;
    assert(rotations_tried >= 0 && rotations_tried <= NUM_ORIENTATIONS);

    tetromino* piece = &game->current;
    static int old_rot = 0;

    // Store old rotation on first try only
    if (rotations_tried == 0) 
        old_rot = piece->rot;
    
    piece->rot = (dir == R_RIGHT) ? 
        (piece->rot - 1 + NUM_ORIENTATIONS) % NUM_ORIENTATIONS : (piece->rot + 1) % NUM_ORIENTATIONS;

    assert(piece->rot >= 0 && piece->rot < NUM_ORIENTATIONS);

    // Check for collisions
    if (move_tetromino(game, piece, 0, 0) < 1) {
        // Attempt next rotation tro see if it works
        if (rotations_tried < NUM_ORIENTATIONS) {
            rotations_tried++;
            tetris_rotate(game, dir, dt);
        } else {
            // Revert rotation
            piece->rot = old_rot;
            rotations_tried = 0;
        }
    }
}

void tetris_apply_gravity(tetris_board* game) {

    // Move current piece down by one
    if(!move_tetromino(game, &game->current, 0, 1)) {

        game->lock_grace_counter++;

        // Piece placement grace
        if (game->lock_grace_counter >= 2) {
            lock_piece(game, &game->current);
            game->lock_grace_counter = 0;
        }
    } else {
        // On successful down move reset grace counter
        game->lock_grace_counter = 0;
    }
}

void tetris_drop(tetris_board* game, float dt) {

    static float drop_timer = 0.0f;
    drop_timer += dt;

    if (drop_timer < DROP_COOLDOWN) return;

    tetris_apply_gravity(game);
    drop_timer = 0.0f;
}

void tetris_hold(tetris_board *game) {
    
    // Cannot hold multiple times in a row
    if (game->has_held) return;
    game->has_held = 1;

    if (game->has_hold) {
        
        // Swap held piece with current piece
        tetromino temp = game->current;
        game->current = game->hold;
        game->hold = temp;
    } else {
        // First time holding
        game->hold = game->current;
        game->has_hold = 1;
        retrieve_next_piece(game);
    }

    place_piece_at_top(game, &game->current);
    game->lock_grace_counter = 0;
}

void tetris_hard_drop(tetris_board* game) {

    // Drop piece until it locks
    while (move_tetromino(game, &game->current, 0, 1)) {
        tetris_apply_gravity(game);
    }

    lock_piece(game, &game->current);
    game->lock_grace_counter = 0;
}

void tetris_reset(tetris_board* game) {

    srand(game->seed);

    game->points = 0;
    game->level = 0;

    game->has_hold = 0;
    game->current = get_random_piece();
    game->next = get_random_piece();

    place_piece_at_top(game, &game->current);

    clear_board(game);
}