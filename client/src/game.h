/**
 * @file        game.h
 * @brief       Game logic
 */

#ifndef GAME_H
#define GAME_H

struct sapp_event;

void setup_game();
void cleanup_game();
void event_game(const struct sapp_event* event);
void update_game();

#endif