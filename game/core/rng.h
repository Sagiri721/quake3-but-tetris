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
unsigned int rng_step(rng_table* table);

/**
 * Peek ahead on the rng table
 */
unsigned int rng_peek(rng_table* table, unsigned int steps_ahead);

#endif