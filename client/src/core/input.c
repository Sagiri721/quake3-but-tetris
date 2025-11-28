/**
 * @file        input.c
 * @brief       User input handling
 */

#include "input.h"
#include "../sokol_gp/thirdparty/sokol_app.h"
#include "tetris.h"

input_table g_input = {0};

void process_input(tetris_board* game, float dt) {
    
    if (g_input.left)  tetris_move(game, -1, dt);
    if (g_input.right) tetris_move(game, 1, dt);
    if (g_input.down)  tetris_drop(game, dt);
    
    if (g_input.rotate) {
        tetris_rotate(game, dt);
        g_input.rotate = false;  // consume the press
    }
}

/**
 * @brief Fill the input table based on events
 */
void handle_input_event(const sapp_event* event) {

    if (event->type == SAPP_EVENTTYPE_KEY_DOWN || event->type == SAPP_EVENTTYPE_KEY_UP) {
        bool pressed = (event->type == SAPP_EVENTTYPE_KEY_DOWN);

        switch (event->key_code) {
            case SAPP_KEYCODE_A: g_input.left   = pressed; break;
            case SAPP_KEYCODE_D: g_input.right  = pressed; break;
            case SAPP_KEYCODE_S: g_input.down   = pressed; break;
            case SAPP_KEYCODE_W: g_input.rotate = pressed; break;
        default: break;
        }
    }
}