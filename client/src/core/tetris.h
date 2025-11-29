/**
 * @file        tetris.h
 * @brief       Tetris game header
 */

#ifndef TETRIS_H
#define TETRIS_H

#define TETRIS 4
#define NUM_TETROMINOS 7
#define NUM_ORIENTATIONS 4

#define NUM_LEVELS 19 + 1

typedef enum {
    TET_I, TET_J, TET_L, TET_O, TET_S, TET_T, TET_Z
} tetromino_type;

typedef enum {
    R_LEFT, R_RIGHT
} rot_dir;

typedef struct {
    int x;
    int y;
} position;

// A tetris piece
typedef struct
{
    tetromino_type type;
    int rot;
    position pos;  
} tetromino;

// Game board
typedef struct {

    unsigned int seed; // Game seed

    unsigned int rows;
    unsigned int cols;

    unsigned int points;
    unsigned int level;

    tetromino current; // Currently falling piece
    tetromino next; // Next piece
    
    tetromino hold; // Holding piece
    char has_held; // Has the player used hold this turn?
    char has_hold; // Is the player holding anything?

    short lock_grace_counter; // Lock grace period counter

    // The current board state
    char* board;

} tetris_board;

/*
  This array stores all necessary information about the cells that are filled by
  each tetromino.  The first index is the type of the tetromino (i.e. shape,
  e.g. I, J, Z, etc.).  The next index is the orientation (0-3).  The final
  array contains 4 tetris_location objects, each mapping to an offset from a
  point on the upper left that is the tetromino "origin".
 */
extern position TETROMINOS[NUM_TETROMINOS][NUM_ORIENTATIONS][TETRIS];

/**
 * The speed at which pieces gravity ticks at each levels
 * This speed is given in seconds
 */
extern float LEVEL_SPEED[NUM_LEVELS];

void tetris_init(tetris_board* game, int rows, int cols, unsigned int seed); // Start a tetris board
void tetris_update(tetris_board* game, float dt);
void tetris_destroy(tetris_board* game);

// Index a board cell
char index_cell(tetris_board* game, int x, int y);

// Events
void tetris_apply_gravity(tetris_board* game);
void tetris_move(tetris_board* game, int dx, float dt);
void tetris_drop(tetris_board* game, float dt);
void tetris_rotate(tetris_board* game, rot_dir dir, float dt);
void tetris_hard_drop(tetris_board* game);
void tetris_hold(tetris_board* game);

void tetris_reset(tetris_board* game);

#endif