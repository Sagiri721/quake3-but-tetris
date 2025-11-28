/**
 * @file        input.h
 * @brief       User input definition
 */

#ifndef INPUT_H
#define INPUT_H

#include "tetris.h"

typedef struct {
    char left;
    char right;
    char down;
    char rotate;
} input_table;

struct sapp_event;

void handle_input_event(const struct sapp_event* event);
void process_input(tetris_board* game, float dt);

#endif