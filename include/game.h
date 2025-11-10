#ifndef GAME_H
#define GAME_H
#include "types.h"

#define GAME_TOTAL_SECONDS 90 

void game_init(GameState* gs);
void game_shutdown(GameState* gs);
void game_tick(GameState* gs);

void player_move_next(GameState* gs);
void player_move_prev(GameState* gs);  
bool player_handle_top(GameState* gs);
bool player_undo(GameState* gs);

const char* sector_name(SectorType s);
const char* event_name(EventType e);

void npcs_init(GameState* gs, int count);
void npcs_tick(GameState* gs);

#endif
