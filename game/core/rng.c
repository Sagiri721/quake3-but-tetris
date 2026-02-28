/**
 * @file        rng.c
 * @brief       Random number generation
 */

#include "rng.h"

void rng_init(rng_table *table, unsigned int seed) {
    table->seed = seed;
}

unsigned int calculate_next_step(unsigned int s) {
    return (s * 1664525 + 1013904223) % 0xFFFFFFFF;
}

unsigned int rng_step(rng_table *table) {
    // Simple linear congruential generator (LCG)
    // TODO: better
    unsigned int t = calculate_next_step(table->seed);
    table->seed = t;

    return t;
}

unsigned int rng_peek(rng_table *table, unsigned int steps_ahead) {

    unsigned int fake_seed = table->seed;
    for (unsigned int i = 0; i < steps_ahead; i++) {
        fake_seed = calculate_next_step(fake_seed);
    }

    return fake_seed;
}