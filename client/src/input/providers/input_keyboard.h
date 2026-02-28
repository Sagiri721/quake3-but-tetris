/**
 * @file        input_keyboard.h
 * @brief       Keyboard input provider
 */
#ifndef INPUT_KEYBOARD_H
#define INPUT_KEYBOARD_H
#include "input.h"

void init_keyboard_provider(input_provider* provider);
void cleanup_keyboard_provider(input_provider* provider);

#endif