/**
 * @file        input_table.h
 * @brief       keyboard input table
 */

#ifndef INPUT_TABLE_H
#define INPUT_TABLE_H

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
    char ui_left;
    char ui_right;
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
    char ui_left_edge;
    char ui_right_edge;
    char ui_select_edge;
    char ui_back_edge;
} input_edge_table;

// Input tables
extern input_table g_input;
extern input_edge_table fired;

// Keyboard-specific functions
void handle_kb_input_event(const struct sapp_event* event);

char is_edge_pressed(char raw, char* fired_flag);


#endif