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
#include "../core/tetris.h"

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

// Tile texture
sg_image tile_texture;

void render_init() {

    sg_image kc85_img = create_sg_image_from_file("res/fonts/kc85.png");
    if (!kc85_img.id) {
        fprintf(stderr, "Failed to load font image\n");
        exit(EXIT_FAILURE);
    }

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

    tile_texture = create_sg_image_from_file("res/tetris/tile_01.png");
    if (!tile_texture.id) {
        fprintf(stderr, "Failed to load tile texture image\n");
        exit(EXIT_FAILURE);
    }
}

void render_destroy() {
    sg_destroy_image(kc85_font.desc.img);
    bitmap_font_destroy(&kc85_font);
    sg_destroy_image(tile_texture);
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

    // Begin a render pass.
    sg_pass pass = {.swapchain = sglue_swapchain()};
    sg_begin_pass(&pass);
    
    // Dispatch all draw commands to Sokol GFX.
    sgp_flush();

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
void render_cell(float x, float y, char type, float alpha) {
    
    // Get color based on tetromino type
    float r = COLOURS[type][0];
    float g = COLOURS[type][1];
    float b = COLOURS[type][2];

    sgp_set_color(r, g, b, alpha);
    sgp_draw_filled_rect(x, y, CELL_SIZE, CELL_SIZE);
}

/**
 * @brief Render a tetromino piece
 */
void render_tetromino(tetromino t, float board_x, float board_y, float alpha) {

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

        render_cell(cell_x, cell_y, t.type, alpha);
    }
}

void render_ui(tetris_board* game, unsigned int offset, unsigned int boards) {
    
    float board_width = game->cols * CELL_SIZE;
    float board_height = game->rows * CELL_SIZE;

    float board_x = (width - board_width) / 2.0f;
    float board_y = (height - board_height) / 2.0f;

    // Apply offset if more than one game is being rendered
    int pivot = width * (2*offset + 1) / (2*boards);
    board_x = pivot - board_width / 2;
    
    sgp_set_image(0, kc85_font.desc.img);
    sgp_set_blend_mode(SGP_BLENDMODE_ADD);
    sgp_set_color(1.0f, 1.0f, 1.0f, 1.0f);
    // bitmap_draw_string(&kc85_font, "Hello, beautiful world!", 23, (sgp_rect){
    //     .x = 10,
    //     .y = 10,
    //     .w = 15,
    //     .h = 15
    // });

    // Draw player name
    bitmap_draw_string(&kc85_font, game->name, strlen(game->name), (sgp_rect){
        .x = board_x + board_width / 2 - (strlen(game->name) * CELL_SIZE) / 2,
        .y = board_y - CELL_SIZE,
        .w = CELL_SIZE,
        .h = CELL_SIZE
    });

    char score_text[32];
    sprintf(score_text, "%06d", game->points);
    bitmap_draw_string(&kc85_font, score_text, strlen(score_text), (sgp_rect){
        .x = board_x + board_width + CELL_SIZE,
        .y = board_y + CELL_SIZE * 10 + CELL_SIZE,
        .w = CELL_SIZE,
        .h = CELL_SIZE
    });

    char level_text[32];
    sprintf(level_text, "%02d", game->level);
    bitmap_draw_string(&kc85_font, level_text, strlen(level_text), (sgp_rect){
        .x = board_x + board_width + CELL_SIZE,
        .y = board_y + CELL_SIZE * 12 + CELL_SIZE,
        .w = CELL_SIZE,
        .h = CELL_SIZE
    });

    // Draw game over text
    if (game->game_over) {
        const char* game_over_text = "GAME OVER";
        bitmap_draw_string(&kc85_font, game_over_text, strlen(game_over_text), (sgp_rect){
            .x = board_x + board_width / 2 - (strlen(game_over_text) * CELL_SIZE) / 2,
            .y = board_y + board_height / 2 - CELL_SIZE,
            .w = CELL_SIZE,
            .h = CELL_SIZE
        });
    }
}

void render_game(tetris_board* game, unsigned int offset, unsigned int boards) {
    
    // Draw field background
    float board_width = game->cols * CELL_SIZE;
    float board_height = game->rows * CELL_SIZE;

    float board_x = (width - board_width) / 2.0f;
    float board_y = (height - board_height) / 2.0f;

    // Apply offset if more than one game is being rendered
    int pivot = width * (2*offset + 1) / (2*boards);
    board_x = pivot - board_width / 2;

    sgp_reset_blend_mode();
    sgp_reset_image(0);

    sgp_set_color(0.2f, 0.2f, 0.2f, 1.0f);
    sgp_draw_filled_rect(board_x, board_y, board_width, board_height);

    // Draw cells
    for (int x = 0; x < game->cols; x++) {
        for (int y = 0; y < game->rows; y++) {
            int cell;
            if ((cell = index_cell(game, x, y)) != 0) {
                
                sgp_set_image(0, tile_texture);

                // Draw filled cell
                float cell_x = board_x + x * CELL_SIZE;
                float cell_y = board_y + y * CELL_SIZE;

                render_cell(cell_x, cell_y, cell - 1, 1.0f);
            } else {

                // Unbind texture
                sgp_reset_image(0);

                // Draw empty cell border
                float cell_x = board_x + x * CELL_SIZE;
                float cell_y = board_y + y * CELL_SIZE;

                sgp_set_color(0.f, 0.f, 0.f, 1.0f);
                sgp_draw_filled_rect(cell_x + 1, cell_y + 1, CELL_SIZE - 2, CELL_SIZE - 2);
            }
        }
    }

    // Draw current piece
    sgp_set_image(0, tile_texture);

    tetromino current = game->current;
    render_tetromino(current, board_x, board_y, 1.0f);

    // Draw drop phantom
    tetromino phantom = current;
    position drop_pos = calculate_drop_preview(&game->current, game);
    phantom.pos = drop_pos;

    sgp_set_blend_mode(SGP_BLENDMODE_ADD);
    render_tetromino(phantom, board_x, board_y, 0.3f);

    // Draw next piece
    render_tetromino((tetromino){
        .type = game->next.type,
        .rot = 0,
        .pos = (position){.x = game->cols + 1, .y = 1}
    }, board_x, board_y, 1.0f);

    sgp_set_blend_mode(SGP_BLENDMODE_NONE);

    // Draw hold piece
    if (game->has_hold) {
        render_tetromino((tetromino){
            .type = game->hold.type,
            .rot = 0,
            .pos = (position){.x = game->cols + 1, .y = 6}
        }, board_x, board_y, 1.0f);
    }
}