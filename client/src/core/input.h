/**
 * @file        input.h
 * @brief       User input definition
 */

#ifndef INPUT_H
#define INPUT_H

#include "../queue/queue.h"

struct tetris_board;
struct sapp_event;


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

    char ui_down;
    char ui_up;
    char ui_select;
    char ui_back;
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

    char ui_down_edge;
    char ui_up_edge;
    char ui_select_edge;
    char ui_back_edge;
} input_edge_table;

// Input tables
extern input_table g_input;
extern input_edge_table fired;

/** 
 * Following Carmack's philosophy of input, i like the idea
 * of having gravity as an input, as it is an input triggered by time
 */
typedef enum {
    IE_GRAVITY, IE_MOVE_LEFT, IE_MOVE_RIGHT, IE_DROP, IE_ROTATE_LEFT, IE_ROTATE_RIGHT, IE_HARD_DROP, IE_HOLD, IE_RESET
} input_event_type;

/**
 * Who implements the input
 */
typedef enum {
    INPUT_PROVIDER_KEYBOARD,
    INPUT_PROVIDER_CPU,
} input_provider_type;

// Function signature for input providers
typedef void (*input_provider_func)(struct tetris_board* game);

// Input provider structure
typedef struct {
    input_provider_type type;
    input_provider_func process_fn;
} input_provider;

// Keyboard-specific functions
void handle_kb_input_event(const struct sapp_event* event);

char is_edge_pressed(char raw, char* fired_flag);

// Dispatch input processing to the appropriate input provider
void process_input(struct tetris_board* game);
// Register an input event into the input queue
void register_input(int action, queue* input_queue);

#endif