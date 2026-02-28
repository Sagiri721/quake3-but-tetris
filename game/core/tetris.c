/**
 * @file        tetris.c
 * @brief       Implementation of the tetris game
 */

#include "tetris.h"
#include "input.h"
#include "rng.h"

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
tetromino get_random_piece(tetris_board* game) {

    int r = rng_step(&game->rng) % NUM_TETROMINOS;
    return (tetromino) {
        .rot = 0,
        .type = r
    };
}

/**
 * @brief Peek at the next n-th tetromino type
 */
tetromino_type tetris_peek_next(tetris_board *game, unsigned int n) {
    return rng_peek(&game->rng, n) % NUM_TETROMINOS;
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
 * @brief Attribute score based on lines cleared
 */
void attribute_score(tetris_board* game, int lines_cleared) {

    int points = 0;
    switch (lines_cleared) {
        case 1:
            points = 100 * (game->level + 1);
            break;
        case 2:
            points = 300 * (game->level + 1);
            break;
        case 3:
            points = 400 * (game->level + 1);
            break;
        case 4:
            points = 800 * (game->level + 1);
            break;
        default:
            break;
    }

    game->stats.lines_cleared += lines_cleared;
    game->points += points;

    // Check for level up
    unsigned int new_level = game->stats.lines_cleared / game->level_goal;
    if (new_level > game->level)
        game->level = new_level;
}

/**
 * @brief Go to a specific level
 */
void tetris_goto_level(tetris_board* game, unsigned level) {

    game->level = level;
    game->stats.lines_cleared = level * game->level_goal;
}

/**
 * @brief Place a tetromino at the top of the screen
 */
void place_piece_at_top(tetris_board* game, tetromino* piece) {
    piece->pos.x = floor(game->cols / 2.0) - 1;
    piece->pos.y = 0;

    // If spawn killed, that's all she wrote
    if (!move_tetromino(game, piece, 0, 0)) {
        game->game_over = 1;
    }
}

/**
 * @brief Retrieve the next piece and place it at the top
 */
void retrieve_next_piece(tetris_board* game) {

    assert(game->board != NULL);

    game->current = game->next;
    game->next = get_random_piece(game);
    place_piece_at_top(game, &game->current);

    game->lock_grace_counter = 0;
}

/**
 * @brief Clear the tetris board
 */
void clear_board(tetris_board* game) {

    assert(game->board != NULL);

    for (size_t x = 0; x < game->rows; x++) {
        for (size_t y = 0; y < game->cols; y++) {
            game->board[x * game->cols + y] = 0;
        }
    }
}

/**
 * @brief Clear a row and move all rows above down by one
 */
void clear_row(tetris_board* game, unsigned int row) {

    assert(game->board != NULL);
    assert(row >= 0 && row < game->rows);

    // Clear the row
    for (size_t x = 0; x < game->rows; x++) {
        game->board[x * game->rows + row] = 0;
    }

    // Move all rows above down by one
    for (size_t y = row; y > 0; y--) {
        for (size_t x = 0; x < game->rows; x++) {
            game->board[x * game->rows + y] = game->board[x * game->rows + (y - 1)];
        }
    }
}

/**
 * @brief Check for full rows and clear them
 */
void check_for_clears(tetris_board* game) {

    int lines_cleared = 0;
    for (size_t y = 0; y < game->rows; y++) {
        char full = 1;

        // Check if row is full
        for (size_t x = 0; x < game->cols; x++) {
            if (index_cell(game, x, y) == 0) {
                full = 0;
            }
        }

        if (full) {
            clear_row(game, y);
            lines_cleared++;
        }
    }

    attribute_score(game, lines_cleared);
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
        unsigned int cell_x = cell_position.x + xx;
        unsigned int cell_y = cell_position.y + yy;

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

void push_line_to_bottom(tetris_board* game, char* line) {

    assert(game->board != NULL);

    // Move all rows up by one
    for (size_t y = 0; y < game->rows - 1; y++) {
        for (size_t x = 0; x < game->cols; x++) {
            game->board[x * game->rows + y] = game->board[x * game->rows + (y + 1)];
        }
    }

    // Insert new line at the bottom
    for (size_t x = 0; x < game->cols; x++) {
        game->board[x * game->rows + (game->rows - 1)] = line[x];
    }
}

/**
 * @brief Add garbage lines at the bottom of the board
 */
void add_garbage(tetris_board* game, unsigned int lines, rng_table* rng) {

    assert(game->board != NULL);
    assert(lines < game->rows && lines > 0);

    for (size_t i = 0; i < lines; i++) {

        char garbage[game->cols];
        unsigned int hole_index = rng_step(rng) % game->cols;

        for (size_t j = 0; j < game->cols; j++) {
            if (j == hole_index) garbage[j] = 0;
            else garbage[j] = TET_GARBAGE;
        }

        push_line_to_bottom(game, garbage);
    }
}

void tetris_init(tetris_board* game, int rows, int cols, unsigned int seed, char* name) {

    // Initialize RNG
    rng_init(&game->rng, seed);

    game->name = name;

    // No one is doomed from the start :)
    game->game_over = 0;

    game->rows = rows;
    game->cols = cols;

    game->points = 0;
    game->level_goal = 10;
    game->level = 1;

    game->has_hold = 0;
    game->current = get_random_piece(game);
    game->next = get_random_piece(game);

    game->lock_grace_counter = 0;

    game->counters.gravity_timer = 0.0f;
    game->counters.move_timer = 0.0f;
    game->counters.drop_timer = 0.0f;
    game->counters.rotations_tried = 0;
    game->counters.old_rot = 0;

    // Initialize settings
    game->settings.preview_count = 3;

    // Initialize input queue
    queue_init(&game->input_queue);

    // Initialize field to empty
    game->board = malloc(rows * cols * sizeof(char*));
    if (game->board == NULL) {
        fprintf(stderr, "Failed to allocate memory for tetris board\n");
        exit(EXIT_FAILURE);
    }

    clear_board(game);

    // Start game with current piece at top
    place_piece_at_top(game, &game->current);
}

void tetris_process_input_queue(tetris_board* game, float dt) {
    int action;
    
    while (!is_empty(&game->input_queue)) {
        
        if(!dequeue(&game->input_queue, &action)){
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

    // If game over, nothing to do
    if (game->game_over) return;

    game->counters.gravity_timer += dt;

    float speed = sample_speed_table(game);
    if (game->counters.gravity_timer > speed) {

        //tetris_apply_gravity(game);
        register_input(IE_GRAVITY, &game->input_queue);

        // Reset timer
        game->counters.gravity_timer = 0.0f;
    }

    // Process input queue
    tetris_process_input_queue(game, dt);
}

char index_cell(const tetris_board* game, unsigned int x, unsigned int y) {
    
    assert(game->board != NULL);
    assert(x <= game->cols);
    assert(y <= game->rows);

    return game->board[x * game->rows + y];
}

void tetris_destroy(tetris_board *game) {

    assert(game->board != NULL);
    free(game->board);
}

position calculate_drop_preview(tetromino* piece, tetris_board* game) {

    tetromino preview = *piece;
    position original_pos = preview.pos;

    // Drop piece until it collides
    while (move_tetromino(game, &preview, 0, 1));
    position drop_pos = preview.pos;

    // Return to original position
    preview.pos = original_pos;

    return drop_pos;
}

// Tetris events
#define MOVE_COOLDOWN 0.08f
#define DROP_COOLDOWN 0.03f

void tetris_move(tetris_board* game, int direction, float dt) {
    game->counters.move_timer += dt;

    if (game->counters.move_timer < MOVE_COOLDOWN) return;

    move_tetromino(game, &game->current, direction, 0);
    game->counters.move_timer = 0.0f;
}

void tetris_rotate(tetris_board* game, rot_dir dir, float dt) {

    // Keep track of how many rotations we've tried
    // Only matters if we hit a wall while rotating
    assert(game->counters.rotations_tried >= 0 && game->counters.rotations_tried <= NUM_ORIENTATIONS);

    tetromino* piece = &game->current;

    // Store old rotation on first try only
    if (game->counters.rotations_tried == 0) 
        game->counters.old_rot = piece->rot;
    
    piece->rot = (dir == R_RIGHT) ? 
        (piece->rot - 1 + NUM_ORIENTATIONS) % NUM_ORIENTATIONS : (piece->rot + 1) % NUM_ORIENTATIONS;

    assert(piece->rot >= 0 && piece->rot < NUM_ORIENTATIONS);

    // Check for collisions
    if (move_tetromino(game, piece, 0, 0) < 1) {
        // Attempt next rotation tro see if it works
        if (game->counters.rotations_tried < NUM_ORIENTATIONS) {
            game->counters.rotations_tried++;
            tetris_rotate(game, dir, dt);
        } else {
            // Revert rotation
            piece->rot = game->counters.old_rot;
            game->counters.rotations_tried = 0;
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

    game->counters.drop_timer += dt;

    if (game->counters.drop_timer < DROP_COOLDOWN) return;

    // We check for collision just to make sure the grace period is still applied
    if (move_tetromino(game, &game->current, 0, 1)) {
        tetris_apply_gravity(game);
    }

    game->counters.drop_timer = 0.0f;
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

    rng_init(&game->rng, game->rng.seed);

    game->points = 0;
    game->level = 0;

    game->has_hold = 0;
    game->current = get_random_piece(game);
    game->next = get_random_piece(game);

    place_piece_at_top(game, &game->current);

    clear_board(game);
}