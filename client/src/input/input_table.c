/**
 * @file        input_table.c
 * @brief       Implementation of input handling for the game.
 */

#include "input_table.h"

#include "../sokol_gp/thirdparty/sokol_app.h"
#include <assert.h>

input_table g_input = {0};
input_edge_table fired = {0};

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
 * @brief Fill the input table based on events
 */
void handle_kb_input_event(const sapp_event* event) {

    if (event->type == SAPP_EVENTTYPE_KEY_DOWN || event->type == SAPP_EVENTTYPE_KEY_UP) {
        bool pressed = (event->type == SAPP_EVENTTYPE_KEY_DOWN);

        switch (event->key_code) {
            case SAPP_KEYCODE_A:
            case SAPP_KEYCODE_LEFT: 
                g_input.left   = pressed; 
                g_input.ui_left = pressed;
                break;
            case SAPP_KEYCODE_D: 
            case SAPP_KEYCODE_RIGHT: 
                g_input.right  = pressed; 
                g_input.ui_right  = pressed; 
                break;
            case SAPP_KEYCODE_S: 
            case SAPP_KEYCODE_DOWN: 
                g_input.down   = pressed; 
                g_input.ui_down = pressed;
                break;
            case SAPP_KEYCODE_Z: 
                g_input.lrotate  = pressed; 
                g_input.ui_select = pressed;
                break;
            case SAPP_KEYCODE_LEFT_CONTROL:
            case SAPP_KEYCODE_X: 
                g_input.rrotate  = pressed; 
                g_input.ui_back  = pressed;
                break;
            case SAPP_KEYCODE_R: g_input.reset  = pressed; break;
            case SAPP_KEYCODE_UP:
            case SAPP_KEYCODE_W: 
                g_input.drop   = pressed; 
                g_input.ui_up = pressed;
                break;
            case SAPP_KEYCODE_C:
            case SAPP_KEYCODE_SPACE:
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
