#ifndef GUI_H
#define GUI_H
#include "types.h"
#include "raylib.h"

void gui_load(void);
void gui_unload(void);
void gui_draw(GameState* gs, int win_w, int win_h);

#endif
