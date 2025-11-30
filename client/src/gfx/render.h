/**
 * @file        render.h
 * @brief       Game rendering abstraction
 */

#ifndef RENDER_H
#define RENDER_H

#include "../core/tetris.h"

// The piece colours
extern float COLOURS[NUM_TETROMINOS][3];

void render_init();
void render_destroy();

void render_begin();
void render_end();

void render_game(tetris_board* game, unsigned int offset, unsigned int boards);
void render_ui(tetris_board* game, unsigned int offset, unsigned int boards);

#endif