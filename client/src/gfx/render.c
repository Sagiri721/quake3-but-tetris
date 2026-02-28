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
#include "menu.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CLEAR_COLOUR_R 0.0f
#define CLEAR_COLOUR_G 0.0f
#define CLEAR_COLOUR_B 0.0f

#define CELL_SIZE 32

float COLOURS[NUM_TETROMINOS + 1][3] = {
    {1, 0, 0},          // Red      (I)
    {0, 1, 0},          // Green    (J)
    {0, 0, 1},          // Blue     (L)
    {1, 1, 0},          // Yellow   (O)
    {1, 0.5, 0},        // Orange   (Z)
    {0.5f, 0, 0.5f},    // Purple   (T)
    {0, 1, 1},          // Cyan     (S)
    {0.3, 0.3, 0.3}     // Grey     (Garbage)
};

// KC85 font character set
const char* KC85_LETTER_TABLE = " !#$%&()*+,-./0123456789:;<=>?ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]_abcdefghijklmnopqrstuvwxyz";
const char* SEGMENT_LETTER_TABLE = " BCDEFGHIJKLMNOPQRSTUVWXY-1234567890";

int width, height;

// Font information
bitmap_font kc85_font, seg_font;

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

    sg_image seg_img = create_sg_image_from_file("res/fonts/7seg.png");
    if (!seg_img.id) {
        fprintf(stderr, "Failed to load 7-segment font image\n");
        exit(EXIT_FAILURE);
    }

    if(!bitmap_font_init(&seg_font, (bitmap_desc) {
        .img = seg_img,
        .img_width_pixels = 120,
        .img_height_pixels = 54,
        .char_width_pixels = 10,
        .char_height_pixels = 18,
        .char_padding_x_pixels = 0,
        .char_padding_y_pixels = 0,
        .chars = SEGMENT_LETTER_TABLE,
        .num_chars = strlen(SEGMENT_LETTER_TABLE),
    })) {
        fprintf(stderr, "Failed to initialize 7-segment bitmap font\n");
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
void render_cell(float x, float y, int type, float alpha) {
    
    // Get color based on tetromino type
    if (type == 8)
        printf("Rendering cell of type 8 at (%f, %f)\n", x, y);
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

void render_ui(const tetris_board* game, unsigned int offset, unsigned int boards) {
    
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
        .x = board_x + board_width / 2.0f - (strlen(game->name) * CELL_SIZE) / 2.0f,
        .y = board_y - CELL_SIZE,
        .w = CELL_SIZE,
        .h = CELL_SIZE
    });

    char score_text[32];
    sprintf(score_text, "%06d", game->points);
    sgp_set_image(0, seg_font.desc.img);
    bitmap_draw_string(&seg_font, score_text, strlen(score_text), (sgp_rect){
        .x = board_x + board_width + CELL_SIZE,
        .y = CELL_SIZE,
        .w = 10 * 4,
        .h = 18 * 4
    });

    char level_text[32];
    sprintf(level_text, "%02d", game->level);
    sgp_set_image(0, kc85_font.desc.img);
    bitmap_draw_string(&kc85_font, level_text, strlen(level_text), (sgp_rect){
        .x = board_x + board_width + CELL_SIZE,
        .y = CELL_SIZE * 4,
        .w = CELL_SIZE,
        .h = CELL_SIZE
    });

    // Draw game over text
    if (game->game_over) {
        const char* game_over_text = "GAME OVER";
        bitmap_draw_string(&kc85_font, game_over_text, strlen(game_over_text), (sgp_rect){
            .x = board_x + board_width / 2.0f - (strlen(game_over_text) * CELL_SIZE) / 2.0f,
            .y = board_y + board_height / 2 - CELL_SIZE,
            .w = CELL_SIZE,
            .h = CELL_SIZE
        });
    }

    sgp_reset_color();
    sgp_reset_blend_mode();
    sgp_reset_image(0);
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
    for (size_t x = 0; x < game->cols; x++) {
        for (size_t y = 0; y < game->rows; y++) {
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

    // Draw next piece(s)
    for (size_t i = 0; i < game->settings.preview_count; i++) {
        unsigned int next_type = tetris_peek_next(game, i);
        render_tetromino((tetromino){
            .type = next_type,
            .rot = 0,
            .pos = (position){.x = -3, .y = 1 + (i * 4)}
        }, board_x, board_y, 1.0f);
    }

    sgp_set_blend_mode(SGP_BLENDMODE_NONE);

    // Draw hold piece
    if (game->has_hold) {
        render_tetromino((tetromino){
            .type = game->hold.type,
            .rot = 0,
            .pos = (position){.x = game->cols + 1, .y = 5}
        }, board_x, board_y, 1.0f);
    }

    sgp_reset_color();
    sgp_reset_blend_mode();
    sgp_reset_image(0);
}

void render_menu(const menu *m) {
    
    sgp_set_image(0, kc85_font.desc.img);
    sgp_set_blend_mode(SGP_BLENDMODE_ADD);
    sgp_set_color(1.0f, 1.0f, 1.0f, 1.0f);

    // Render each menu item
    for (size_t i = 0; i < m->item_count; i++) {
        
        menu_item item = m->items[i];
        char render_text[MAX_LABEL_LENGTH + 3 + 16];

        switch (m->items[i].type)
        {
            case MA_SUBMENU:
            case MA_CALLBACK:
                // Cursor indicator
                if (i == m->selected_index) sprintf(render_text, "=> %s", item.label);
                else sprintf(render_text, "%s", item.label);
                
                break;

            case MA_NUMBER:
                sprintf(render_text, "%s", item.label);

                // Display number as well
                char number_text[16];

                if (m->items[i].action.number->printer == NULL) {
                    
                    if (i == m->selected_index) sprintf(number_text, ": >%d<", *(m->items[i].action.number->value));
                    else sprintf(number_text, ": %d", *(m->items[i].action.number->value));

                } else {
                    if (i == m->selected_index) {
                        m->items[i].action.number->printer(number_text, *(m->items[i].action.number->value));
                        char temp[16];
                        sprintf(temp, ": >%s<", number_text);
                        strcpy_s(number_text, sizeof(number_text), temp);
                    }
                    else {
                        m->items[i].action.number->printer(number_text, *(m->items[i].action.number->value));
                        char temp[16];
                        sprintf(temp, ": %s", number_text);
                        strcpy_s(number_text, sizeof(number_text), temp);
                    }
                }
                    
                //strcat(render_text, number_text);
                strcat_s(render_text, sizeof(render_text), number_text);
                break;
        }

        size_t len = strlen(render_text);
        bitmap_draw_string(&kc85_font, render_text, len, (sgp_rect){
            .x = (width - (len * CELL_SIZE)) / 2.0f,
            .y = (height / 2.0f) + (i * CELL_SIZE),
            .w = CELL_SIZE,
            .h = CELL_SIZE
        });
    }

    sgp_reset_color();
    sgp_reset_blend_mode();
    sgp_reset_image(0);
}

void render_titlescreen(float dt) {

    static char title_string[] = "NETRIS NETRIS NETRIS NETRIS NETRIS NETRIS NETRIS";
    static size_t len = 48;
    static float x = 0;

    sgp_set_image(0, kc85_font.desc.img);
    sgp_set_blend_mode(SGP_BLENDMODE_ADD_PREMULTIPLIED);

    for (size_t i = 0; i <= 2; i++) {

        if (i == 0) sgp_set_color(1.0f, 0.0f, 0.0f, 1.0f);
        else if (i == 1) sgp_set_color(1.0f, 0.5f, 0.5f, 1.0f);
        else sgp_set_color(1.0f, 1.0f, 1.0f, 1.0f);

        bitmap_draw_string(&kc85_font, title_string, len, (sgp_rect){
            .x = x - CELL_SIZE,
            .y = (height / 4.0f) - i * 2.0f,
            .w = CELL_SIZE,
            .h = CELL_SIZE
        });

        bitmap_draw_string(&kc85_font, title_string, len, (sgp_rect){
            .x = x + 1532,
            .y = (height / 4.0f) - i * 2.0f,
            .w = CELL_SIZE,
            .h = CELL_SIZE
        });
    }

    x -= dt * 130;
    if (x < -1532) x = CELL_SIZE;

    sgp_reset_color();
    sgp_reset_blend_mode();
    sgp_reset_image(0);
}