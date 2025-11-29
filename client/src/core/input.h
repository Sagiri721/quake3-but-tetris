/**
 * @file        input.h
 * @brief       User input definition
 */

#ifndef INPUT_H
#define INPUT_H

#include "tetris.h"
#include "../queue/queue.h"

/**
 * An input queue should make sure no inputs are dropped 
 * and also useful to build a Quake3 inspired input history
 * To store replays
 */
extern queue input_queue;

/** 
 * Following Carmack's philosophy of input, i like the idea
 * of having gravity as an input, as it is an input triggered by time
 */
typedef enum {
    IE_GRAVITY, IE_MOVE_LEFT, IE_MOVE_RIGHT, IE_DROP, IE_ROTATE_LEFT, IE_ROTATE_RIGHT, IE_HARD_DROP, IE_HOLD, IE_RESET
} input_event_type;

/**
 * Raw input table mapping
 * If key is pressed, the corresponding field is set to 1
 * if key is not pressed, the corresponding field is set to 0
 */
typedef struct {
    char left;
    char right;
    char down;
    char lrotate;
    char rrotate;
    char reset;
    char drop;
    char hold;
} input_table;

/**
 * Input edge table mapping
 * If key is pressed, the corresponding field is set to 1 for one check
 * and only resets to 0 when key is released
 */
typedef struct {
    char left_edge;
    char right_edge;
    char down_edge;
    char lrotate_edge;
    char rrotate_edge;
    char reset_edge;
    char drop_edge;
    char hold_edge;
} input_edge_table;

struct sapp_event;

void input_init();

// Process the input table and make the required events
void handle_input_event(const struct sapp_event* event);
// Process raw sokol events
void process_input(tetris_board* game, float dt);
// Register an input event into the input queue
void register_input(int action);

void input_cleanup();

#endif