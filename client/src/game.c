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

tetris_board games[2];

void setup_game() {
    render_init();

    for (int i = 0; i < 2; i++) {
        tetris_board* game = &games[i];
        tetris_init(game, ROWS, COLS, 0);
    }
}

void event_game(const sapp_event* event) {
    // Handle input events
    handle_input_event(event);
}

void cleanup_game() {
    for (int i = 0; i < 2; i++) {
        tetris_board* game = &games[i];
        tetris_destroy(game);
    }

    render_destroy();
}

void update_game() {
    
    float time = sapp_frame_duration();

    // Update the game state
    process_input(&games[0], time);
    
    for (int i = 0; i < 2; i++) {
        tetris_board* game = &games[i];
        tetris_update(game, time);
    }

    // Render the game
    render_begin();
    for (int i = 0; i < 2; i++) {
        tetris_board* game = &games[i];
        render_game(game, i, 2);
        render_ui(game, i, 2);
    }
    render_end();
}