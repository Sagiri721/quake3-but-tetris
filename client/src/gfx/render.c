/**
 * @file        render.h
 * @brief       Game rendering implementation
 */

#include "render.h"

#include "../sokol_gp/thirdparty/sokol_gfx.h"
#include "../sokol_gp/sokol_gp.h"
#include "../sokol_gp/thirdparty/sokol_app.h"
#include "../sokol_gp/thirdparty/sokol_glue.h"
#include "../core/tetris.h"
#include "bitmap_text.h"
#include "image.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CLEAR_COLOUR_R 0.1f
#define CLEAR_COLOUR_G 0.1f
#define CLEAR_COLOUR_B 0.1f

#define CELL_SIZE 32

float COLOURS[NUM_TETROMINOS][3] = {
    {1, 0, 0},          // Red
    {0, 1, 0},          // Green
    {0, 0, 1},          // Blue
    {1, 1, 0},          // Yellow
    {1, 0.5, 0},        // Orange
    {0.5f, 0, 0.5f},    // Purple
    {0, 1, 1}           // Cyan
};

// KC85 font character set
const char* KC85_LETTER_TABLE = " !#$%&()*+,-./0123456789:;<=>?ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]_abcdefghijklmnopqrstuvwxyz";

int width, height;

// Font information
bitmap_font kc85_font;

void render_init() {

    sg_image kc85_img = create_sg_image_from_file("res/fonts/kc85.png");

    // Make font
    if(!bitmap_font_init(&kc85_font, (bitmap_desc) {
        .img = kc85_img,
        .img_width_pixels = 570,
        .img_height_pixels = 150,
        .char_width_pixels = 30,
        .char_height_pixels = 30,
        .char_padding_x_pixels = 0,
        .char_padding_y_pixels = 0,
        .chars = KC85_LETTER_TABLE,
        .num_chars = strlen(KC85_LETTER_TABLE),
    })) {
        fprintf(stderr, "Failed to initialize bitmap font\n");
        exit(EXIT_FAILURE);
    }
}

void render_destroy() {
    bitmap_font_destroy(&kc85_font);
}

void render_begin() {
    // Get current window size.
    width = sapp_width(), height = sapp_height();

    // Begin recording draw commands for a frame buffer of size (width, height).
    sgp_begin(width, height);
    // Set frame buffer drawing region to (0,0,width,height).
    sgp_viewport(0, 0, width, height);
    // Set drawing coordinate space to top-left origin with pixel units.
    sgp_project(0, width, 0, height); // origin at top-left

    // Clear the frame buffer.
    sgp_set_color(CLEAR_COLOUR_R, CLEAR_COLOUR_G, CLEAR_COLOUR_B, 1.0f);
    sgp_clear();
}

void render_end() {
    // Finish a draw command queue, clearing it.
    sgp_end();
    // End render pass.
    sg_end_pass();
    // Commit Sokol render.
    sg_commit();
}

/**
 * @brief Render a single cell at (x, y) with given type
 */
void render_cell(float x, float y, char type) {
    
    // Get color based on tetromino type
    float r = COLOURS[type][0];
    float g = COLOURS[type][1];
    float b = COLOURS[type][2];

    sgp_set_color(r, g, b, 1.0f);
    sgp_draw_filled_rect(x, y, CELL_SIZE - 1, CELL_SIZE - 1);
}

/**
 * @brief Render a tetromino piece
 */
void render_tetromino(tetromino t, float board_x, float board_y) {

    // float board_x = (width - (10 * CELL_SIZE)) / 2.0f;
    // float board_y = (height - (20 * CELL_SIZE)) / 2.0f;

    for (int i = 0; i < TETRIS; i++) {

        position cell_position = TETROMINOS[t.type][t.rot][i];
        position p = (position) {
            .x = cell_position.x + t.pos.x,
            .y = cell_position.y + t.pos.y
        };

        float cell_x = board_x + p.x * CELL_SIZE;
        float cell_y = board_y + p.y * CELL_SIZE;

        render_cell(cell_x, cell_y, t.type);
    }
}

void render_ui(tetris_board* game) {
    
    sgp_set_image(0, kc85_font.desc.img);
    sgp_set_blend_mode(SGP_BLENDMODE_ADD);
    sgp_set_color(1.0f, 1.0f, 1.0f, 1.0f);
    bitmap_draw_string(&kc85_font, "Hello, beautiful world!", 23, (sgp_rect){
        .x = 10,
        .y = 10,
        .w = 15,
        .h = 15
    });

    sgp_flush();
}

void render_game(tetris_board* game) {
    
    // Draw field background
    float board_width = game->cols * CELL_SIZE;
    float board_height = game->rows * CELL_SIZE;

    float board_x = (width - board_width) / 2.0f;
    float board_y = (height - board_height) / 2.0f;

    sgp_set_color(0.6f, 0.6f, 0.6f, 1.0f);
    sgp_draw_filled_rect(board_x, board_y, board_width, board_height);

    // Draw cells
    for (int x = 0; x < game->cols; x++) {
        for (int y = 0; y < game->rows; y++) {
            int cell;
            if ((cell = index_cell(game, x, y)) != 0) {

                // Draw filled cell
                float cell_x = board_x + x * CELL_SIZE;
                float cell_y = board_y + y * CELL_SIZE;

                render_cell(cell_x, cell_y, cell - 1);
            }
        }
    }

    // Draw current piece
    tetromino current = game->current;
    render_tetromino(current, board_x, board_y);

    // Draw next piece
    render_tetromino((tetromino){
        .type = game->next.type,
        .rot = 0,
        .pos = (position){.x = game->cols + 1, .y = 1}
    }, board_x, board_y);

    // Draw hold piece
    if (game->has_hold) {
        render_tetromino((tetromino){
            .type = game->hold.type,
            .rot = 0,
            .pos = (position){.x = game->cols + 1, .y = 6}
        }, board_x, board_y);
    }

    // Begin a render pass.
    sg_pass pass = {.swapchain = sglue_swapchain()};
    sg_begin_pass(&pass);
    // Dispatch all draw commands to Sokol GFX.
    sgp_flush();
}