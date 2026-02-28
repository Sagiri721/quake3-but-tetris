/**
 * @file        input.c
 * @brief       CPU input handling
 */

#include "tetris.h"
#include "input.h"

#include "../../bot/ai.h"
#include "../../bot/ai.h"

#include <stdio.h>

static move_candidate current_plan = {0};
static char has_plan = 0;

void execute_plan(tetris_board *game) {

    // Check if should hold for the first time
    if (!game->has_hold && game->current.type != current_plan.t.type) {
        register_input(IE_HOLD, &game->input_queue);
        return;
    } 

    // Rotate towards target rotation
    if (game->current.rot != current_plan.t.rot) {
        register_input(IE_ROTATE_RIGHT, &game->input_queue);
        return;
    }

    // Move towards target x position
    if (game->current.pos.x < current_plan.t.pos.x) {
        register_input(IE_MOVE_RIGHT, &game->input_queue);
        return;
    } else if (game->current.pos.x > current_plan.t.pos.x) {
        register_input(IE_MOVE_LEFT, &game->input_queue);
        return;
    }

    // Drop piece
    register_input(IE_HARD_DROP, &game->input_queue);

    // Plan executed
    has_plan = 0;
}

void process_cpu_input(tetris_board *game) {

    // Ok i wont play anymore fine
    if (game->game_over) return;

    if (!has_plan) {
        // Get new plan
        current_plan = decide_next_move(game);
        has_plan = 1;

        printf("CPU decided on move: x=%d rot=%d score=%.2f\n", current_plan.t.pos.x, current_plan.t.rot, current_plan.score);
    }

    execute_plan(game);
}

void init_cpu_provider(input_provider *provider) {

    provider->type = INPUT_PROVIDER_CPU;
    provider->process_fn = process_cpu_input;
}