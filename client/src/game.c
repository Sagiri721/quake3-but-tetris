/**
 * @file        game.h
 * @brief       Game loop implementation
 */

#include "game.h"

#include "core/providers/input_cpu.h"
#include "core/providers/input_keyboard.h"
#include "gfx/menu.h"
#include "gfx/render.h"
#include "core/input.h"
#include "core/tetris.h"

#include "sokol_gp/thirdparty/sokol_app.h"
#include <time.h>

#define ROWS 20
#define COLS 10

void start_game() {
    menu_clear_stack();
}

menu main_menu = {
    .items = (menu_item[]) {
        { "Start", MA_SUBMENU, .action.submenu = &game_menu },
        { "Quit",  MA_CALLBACK,  .action.callback = sapp_request_quit },
    },
    .item_count = 2,
    .selected_index = 0,
};

menu game_menu = {
    .items = (menu_item[]) {
        { "Marathon", MA_CALLBACK, .action.callback = start_game },
        { "Versus",       MA_CALLBACK, .action.callback = NULL },
        { "Challenge",    MA_CALLBACK, .action.callback = NULL },
    },
    .item_count = 3,
    .selected_index = 0,
};

tetris_board games[2];
input_provider providers[2];

void setup_game() {

    render_init();

    tetris_init(&games[0], ROWS, COLS, 0, "Sagiri");
    init_keyboard_provider(&providers[0]);
    games[0].input_provider = &providers[0];

    tetris_init(&games[1], ROWS, COLS, 0, "CPU");
    init_cpu_provider(&providers[1]);
    games[1].input_provider = &providers[1];

    menu_push(&main_menu);
}

void event_game(const sapp_event* event) {
    // Handle input events
    //handle_input_event(event);
    handle_kb_input_event(event);
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

    if (menu_opened()) {

        menu* m = menu_current();

        render_begin();
        menu_update(m);
        render_menu(m);
        render_end();
        return;
    }

     // Update the game state
    for (int i = 0; i < 2; i++) {
        tetris_board* game = &games[i];
        tetris_update(game, time);
        process_input(game);
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