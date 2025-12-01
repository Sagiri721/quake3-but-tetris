/**
 * @file        input.c
 * @brief       CPU input handling
 */

#include "input_cpu.h"
#include "../tetris.h"
#include "../../bot/ai.h"
#include "../../bot/ai.h"
#include <stdio.h>

static move_candidate current_plan = {0};
static char has_plan = 0;

void execute_plane(tetris_board *game) {

    // Rotate towards target rotation
    if (game->current.rot != current_plan.rot) {
        register_input(IE_ROTATE_RIGHT, game);
        return;
    }

    // Move towards target x position
    if (game->current.pos.x < current_plan.pos.x) {
        register_input(IE_MOVE_RIGHT, game);
        return;
    } else if (game->current.pos.x > current_plan.pos.x) {
        register_input(IE_MOVE_LEFT, game);
        return;
    }

    // Drop piece
    register_input(IE_HARD_DROP, game);

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

        printf("CPU decided on move: x=%d rot=%d score=%.2f\n", current_plan.pos.x, current_plan.rot, current_plan.score);
    }

    execute_plane(game);
}

void init_cpu_provider(input_provider *provider) {

    provider->type = INPUT_PROVIDER_CPU;
    provider->process_fn = process_cpu_input;
}