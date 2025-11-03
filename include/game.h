#ifndef GAME_H
#define GAME_H
#include "types.h"

void game_init(GameState* gs);
void game_shutdown(GameState* gs);
void game_tick(GameState* gs);

void player_move_next(GameState* gs);
bool player_handle_top(GameState* gs);
bool player_undo(GameState* gs);

const char* sector_name(SectorType s);
const char* event_name(EventType e);

#endif
