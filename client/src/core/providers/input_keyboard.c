/**
 * @file        input.c
 * @brief       User input handling
 */

#include "input_keyboard.h"
#include "../../sokol_gp/thirdparty/sokol_app.h"
#include "../tetris.h"

/**
 * @brief Process the current input state into game actions
 */
void process_game_keyboard_state(tetris_board* game) {
    
    if (g_input.left)  register_input(IE_MOVE_LEFT, &game->input_queue);
    if (g_input.right) register_input(IE_MOVE_RIGHT, &game->input_queue);
    if (g_input.down)  register_input(IE_DROP, &game->input_queue);

    // one-shots
    if (is_edge_pressed(g_input.lrotate, &fired.lrotate_edge))
        register_input(IE_ROTATE_LEFT, &game->input_queue);

    if (is_edge_pressed(g_input.rrotate, &fired.rrotate_edge))
        register_input(IE_ROTATE_RIGHT, &game->input_queue);

    if (is_edge_pressed(g_input.hold, &fired.hold_edge))
        register_input(IE_HOLD, &game->input_queue);

    if (is_edge_pressed(g_input.reset, &fired.reset_edge))
        register_input(IE_RESET, &game->input_queue);

    if (is_edge_pressed(g_input.drop, &fired.drop_edge))
        register_input(IE_HARD_DROP, &game->input_queue);
}

void init_keyboard_provider(input_provider *provider) {

    provider->type = INPUT_PROVIDER_KEYBOARD;
    provider->process_fn = process_game_keyboard_state;
}

void cleanup_keyboard_provider(input_provider* provider) {
    // pass
}