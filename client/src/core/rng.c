/**
 * @file        rng.c
 * @brief       Random number generation
 */

#include "rng.h"

void rng_init(rng_table *table, unsigned int seed) {
    table->seed = seed;
}

void rng_step(rng_table *table, unsigned int *out_value) {
    // Simple linear congruential generator (LCG)
    // TODO: better
    int t = (table->seed * 1664525 + 1013904223) % 0xFFFFFFFF;
    table->seed = t;
    *out_value = t;
}