/**
 * @file        input.c    
 * @brief       Implementation of input registration and processing for Tetris game.
 */

#include "input.h"
#include "queue/queue.h"
#include "tetris.h"

void register_input(int action, queue* input_queue) {

    if(!enqueue(input_queue, action)) {
        // Handle this maybe?
    }
}

// Dispatch input processing to the appropriate input provider
void pump_input(tetris_board* game) {
    if (game && game->input_provider && game->input_provider->process_fn) {
        game->input_provider->process_fn(game);
    }
}