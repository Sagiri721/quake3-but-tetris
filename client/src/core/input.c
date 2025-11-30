/**
 * @file        input.c
 * @brief       User input handling
 */

#include "input.h"
#include "../sokol_gp/thirdparty/sokol_app.h"
#include "../queue/queue.h"
#include "tetris.h"

static input_table g_input = {0};
static input_edge_table fired = {0};

char is_edge_pressed(char raw, char* fired_flag) {
    
    if (raw && !(*fired_flag)) {
        *fired_flag = 1;
        return 1;
    } else if (!raw) {
        *fired_flag = 0;
    }

    return 0;
}

/**
 * @brief Register an input action
 * later we can make a history of inputs through here
 */
void register_input(int action, tetris_board* game) {
    if(!enqueue(&game->input_queue, action)){
        //pass
    }
}

/**
 * @brief Process the current input state into game actions
 */
void process_input(tetris_board* game, float dt) {
    
    if (g_input.left)  register_input(IE_MOVE_LEFT, game);
    if (g_input.right) register_input(IE_MOVE_RIGHT, game);
    if (g_input.down)  register_input(IE_DROP, game);

    // one-shots
    if (is_edge_pressed(g_input.lrotate, &fired.lrotate_edge))
        register_input(IE_ROTATE_LEFT, game);

    if (is_edge_pressed(g_input.rrotate, &fired.rrotate_edge))
        register_input(IE_ROTATE_RIGHT, game);

    if (is_edge_pressed(g_input.hold, &fired.hold_edge))
        register_input(IE_HOLD, game);

    if (is_edge_pressed(g_input.reset, &fired.reset_edge))
        register_input(IE_RESET, game);

    if (is_edge_pressed(g_input.drop, &fired.drop_edge)){
        register_input(IE_HARD_DROP, game);
    }
}

/**
 * @brief Fill the input table based on events
 */
void handle_input_event(const sapp_event* event) {

    if (event->type == SAPP_EVENTTYPE_KEY_DOWN || event->type == SAPP_EVENTTYPE_KEY_UP) {
        bool pressed = (event->type == SAPP_EVENTTYPE_KEY_DOWN);

        switch (event->key_code) {
            case SAPP_KEYCODE_A:
            case SAPP_KEYCODE_LEFT: g_input.left   = pressed; break;
            case SAPP_KEYCODE_D: 
            case SAPP_KEYCODE_RIGHT: g_input.right  = pressed; break;
            case SAPP_KEYCODE_S: 
            case SAPP_KEYCODE_DOWN: g_input.down   = pressed; break;
            case SAPP_KEYCODE_Z: g_input.lrotate  = pressed; break;
            case SAPP_KEYCODE_LEFT_CONTROL:
            case SAPP_KEYCODE_W: 
            case SAPP_KEYCODE_X: 
            case SAPP_KEYCODE_UP: g_input.rrotate  = pressed; break;
            case SAPP_KEYCODE_R: g_input.reset  = pressed; break;
            case SAPP_KEYCODE_SPACE: g_input.drop   = pressed; break;
            case SAPP_KEYCODE_C:
            case SAPP_KEYCODE_LEFT_SHIFT: g_input.hold   = pressed; break;
            case SAPP_KEYCODE_ESCAPE:
                if (pressed) {
                    sapp_request_quit();
                }
                break;
        default: break;
        }
    }
}