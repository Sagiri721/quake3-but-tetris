/**
 * @file        input_cpu.h
 * @brief       CPU input provider
 */
#ifndef INPUT_CPU_H
#define INPUT_CPU_H
#include "input.h"

// Cpu-specific functions
void init_cpu_provider(input_provider* provider);
void cleanup_cpu_provider(input_provider* provider);

#endif