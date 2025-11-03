#ifndef SPRITES_H
#define SPRITES_H
#include "raylib.h"

typedef enum { FACE_FRONT=0, FACE_BACK=1, FACE_LEFT=2, FACE_RIGHT=3 } FaceDir;

typedef struct {
    Texture2D front;
    Texture2D back;
    Texture2D left;
    Texture2D right;
} SpriteSet;

typedef struct {
    SpriteSet player;
    SpriteSet npc;
    Texture2D background;
} Sprites;

int  sprites_load(Sprites* sp, const char* base_path);
void sprites_unload(Sprites* sp);
Texture2D sprites_pick(const SpriteSet* set, FaceDir dir);

#endif
