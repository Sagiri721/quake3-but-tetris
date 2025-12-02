/**
 * @file        rng.h
 * @brief       Random Number Generator Interface
 */

#ifndef RNG_H_
#define RNG_H_

typedef struct {
    unsigned int seed;
} rng_table;

void rng_init(rng_table* table, unsigned int seed);

/**
 * Get the current random number from the RNG table
 */
void rng_step(rng_table* table, unsigned int* out_value);

#endif