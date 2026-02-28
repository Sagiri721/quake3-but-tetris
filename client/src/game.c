/**
 * @file        game.h
 * @brief       Game loop implementation
 */

#include "game.h"

#include "rng.h"
#include "tetris.h"

#include "audio/ogg_player.h"
#include "input/providers/input_cpu.h"
#include "input/providers/input_keyboard.h"
#include "gfx/menu.h"
#include "gfx/render.h"
#include "input/input_table.h"

#include "sokol_gp/thirdparty/sokol_app.h"
#include <math.h>
#include <time.h>

#define ROWS 20
#define COLS 10

typedef enum {
    GM_MARATHON,
    GM_VERSUS,
    GM_CHALLENGE
} game_mode;

static game_mode current_game_mode;

tetris_board games[2];
input_provider providers[2];

ogg_audio_player player;

void start_versus() {

    current_game_mode = GM_VERSUS;
    
    tetris_init(&games[0], ROWS, COLS, 0, "Sagiri");
    init_keyboard_provider(&providers[0]);
    games[0].input_provider = &providers[0];

    tetris_init(&games[1], ROWS, COLS, 0, "CPU");
    init_cpu_provider(&providers[1]);
    games[1].input_provider = &providers[1];

    menu_clear_stack();
}

static int start_level = 1;
void start_marathon() {

    current_game_mode = GM_MARATHON;
    
    tetris_init(&games[0], ROWS, COLS, 0, "Sagiri");
    tetris_goto_level(&games[0], start_level);

    init_keyboard_provider(&providers[0]);
    games[0].input_provider = &providers[0];

    menu_clear_stack();
}

static int garbage_level = 1;
void start_challenge() {

    current_game_mode = GM_CHALLENGE;
    
    tetris_init(&games[0], ROWS, COLS, 0, "Sagiri");

    // Add some garbage lines
    rng_table t;
    rng_init(&t, (unsigned int)time(NULL));
    add_garbage(&games[0], garbage_level, &t);

    init_keyboard_provider(&providers[0]);
    games[0].input_provider = &providers[0];

    menu_clear_stack();
}

number_action_desc level_input = {
    .value = &start_level,
    .lower = 1,
    .upper = INFINITY,
    .increment = 1,

    .on_change = NULL,
    .printer = NULL,
};

menu marathon_settings_menu = {
    .items = (menu_item[]) {
        { "Start level",    MA_NUMBER,          .action.number = &level_input },
        { "Go",             MA_CALLBACK,        .action.callback = start_marathon },
    },
    .item_count = 2,
    .selected_index = 0,
};

number_action_desc garbage_input = {
    .value = &garbage_level,
    .lower = 1,
    .upper = 15,
    .increment = 1,

    .on_change = NULL,
    .printer = NULL,
};

menu challenge_settings_menu = {
    .items = (menu_item[]) {
        { "Garbage",        MA_NUMBER,          .action.number = &garbage_input},
        { "Go",             MA_CALLBACK,        .action.callback = start_challenge}
    },
    .item_count = 2,
    .selected_index = 0,
};

menu game_menu = {
    .items = (menu_item[]) {
        { "Marathon",       MA_SUBMENU,     .action.submenu = &marathon_settings_menu },
        { "Versus",         MA_CALLBACK,    .action.callback = start_versus },
        { "Challenge",      MA_SUBMENU,     .action.submenu = &challenge_settings_menu },
    },
    .item_count = 3,
    .selected_index = 0,
};

static const char* tracks[] = {
    "res/audio/theme.ogg",
    "res/audio/theme2.ogg",
    "res/audio/theme3.ogg",
    "res/audio/theme4.ogg",
};

void swap_track(int value) { push_ogg_file(&player, tracks[value]); }
void print_track(char* buffer, int value) {
    
    static const char* track_names[] = {
        "Classic",
        "SMB3",
        "Last Bible",
        "Touhou",
    };

    snprintf(buffer, 32, "%s", track_names[value]);
}

int current_track = 1;
number_action_desc track_input = {
    .value = &current_track,
    .lower = 0,
    .upper = 3,
    .increment = 1,

    .on_change = &swap_track,
    .printer = &print_track,
};

void set_volume(int value) {
    audio_set_volume(&player, (float) value / 10.0f);
}

int volume = 10;
number_action_desc volume_number = {
    .value = &volume,
    .lower = 0,
    .upper = 10,
    .increment = 1,
    
    .on_change = &set_volume,
    .printer = NULL,
};

menu audio_settings = {
    .items = (menu_item[]) {
        { "Track",          MA_NUMBER,     .action.number = &track_input },
        { "Volume",         MA_NUMBER,     .action.number = &volume_number },
    },
    .item_count = 2,
    .selected_index = 0,
};

menu settings = {
    .items = (menu_item[]) {
        { "Audio",          MA_SUBMENU,     .action.submenu = &audio_settings },
        { "Video",          MA_SUBMENU,     .action.submenu = NULL }, // TODO
    },
    .item_count = 2,
    .selected_index = 0,
};

menu main_menu = {
    .items = (menu_item[]) {
        { "Start",          MA_SUBMENU,      .action.submenu = &game_menu },
        { "Settings",       MA_SUBMENU,      .action.submenu = &settings },
        { "Quit",           MA_CALLBACK,     .action.callback = sapp_request_quit },
    },
    .item_count = 3,
    .selected_index = 0,
};

void setup_game() {

    audio_init(&player, 100, 1);
    push_ogg_file(&player, "res/audio/theme2.ogg");

    render_init();
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
        if (game->board)
            tetris_destroy(game);
    }

    render_destroy();
    audio_destroy(&player);
}

void update_game() {
    
    float time = sapp_frame_duration();

    if (menu_opened()) {

        menu* m = menu_current();

        render_begin();
        render_titlescreen(time);

        menu_update(m);
        render_menu(m);
        render_end();
        return;
    }

    switch (current_game_mode)
    {
        case GM_CHALLENGE:
        case GM_MARATHON:

            // Update the game state
            tetris_board* game = &games[0];
            tetris_update(game, time);
            pump_input(game);

            // Render the game
            render_begin();
            render_game(game, 0, 1);
            render_ui(game, 0, 1);
            render_end();

        break;
        case GM_VERSUS:
            // Update the game state
            for (int i = 0; i < 2; i++) {
                tetris_board* game = &games[i];
                tetris_update(game, time);
                pump_input(game);
            }

            // Render the game
            render_begin();
            for (int i = 0; i < 2; i++) {
                tetris_board* game = &games[i];
                render_game(game, i, 2);
                render_ui(game, i, 2);
            }
            render_end();
        default:
            break;
    }
}