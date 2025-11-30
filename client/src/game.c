/**
 * @file        game.h
 * @brief       Game loop implementation
 */

#include "game.h"

#include "gfx/render.h"
#include "core/input.h"
#include "core/tetris.h"

#include "sokol_gp/thirdparty/sokol_app.h"

#define ROWS 20
#define COLS 10

tetris_board game;

void setup_game() {
    render_init();
    input_init();
    tetris_init(&game, ROWS, COLS, 0);
}

void event_game(const sapp_event* event) {
    // Handle input events
    handle_input_event(event);
}

void cleanup_game() {
    tetris_destroy(&game);
    input_destroy();
    render_destroy();
}

void update_game() {
    
    float time = sapp_frame_duration();

    // Update the game state
    process_input(&game, time);
    tetris_update(&game, time);

    // Render the game
    render_begin();
    render_game(&game);
    render_ui(&game);
    render_end();
}