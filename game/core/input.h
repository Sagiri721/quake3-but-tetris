/**
 * @file        input.h
 * @brief       Input definition
 */

#ifndef INPUT_H
#define INPUT_H

#include "queue/queue.h"

struct tetris_board;

/** 
 * Following Carmack's philosophy of input, i like the idea
 * of having gravity as an input, as it is an input triggered by time
 */
typedef enum {
    IE_GRAVITY, IE_MOVE_LEFT, IE_MOVE_RIGHT, IE_DROP, IE_ROTATE_LEFT, IE_ROTATE_RIGHT, IE_HARD_DROP, IE_HOLD, IE_RESET
} input_event_type;

/**
 * Who implements the dang input
 */
typedef enum {
    INPUT_PROVIDER_KEYBOARD,
    INPUT_PROVIDER_CPU,
    //INPUT_PROVIDER_NETWORK,
} input_provider_type;

// Function signature for input providers
typedef void (*input_provider_func)(struct tetris_board* game);

// Input provider structure
// This is not strictly necessary
typedef struct {
    input_provider_type type;
    input_provider_func process_fn;
} input_provider;

// Dispatch input processing to the appropriate input provider
void pump_input(struct tetris_board* game);
// Register an input event into the input queue
void register_input(int action, queue* input_queue);

#endif