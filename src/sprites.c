#include "sprites.h"
#include <stdio.h>

static Texture2D load_tex(const char* path){
    Image img = LoadImage(path);
    Texture2D tx = (Texture2D){0};
    if (img.data){ tx = LoadTextureFromImage(img); UnloadImage(img); }
    return tx;
}
static int ok_tex(Texture2D t){ return t.id != 0; }

int sprites_load(Sprites* sp, const char* base_path){
    char p[256];

    snprintf(p,sizeof(p),"%s/bg.png", base_path);
    sp->background = load_tex(p);

    snprintf(p,sizeof(p),"%s/player_front.png",      base_path);  sp->player.front      = load_tex(p);
    snprintf(p,sizeof(p),"%s/player_front_walk.png", base_path);  sp->player.front_walk = load_tex(p);
    snprintf(p,sizeof(p),"%s/player_back.png",       base_path);  sp->player.back       = load_tex(p);
    snprintf(p,sizeof(p),"%s/player_left.png",       base_path);  sp->player.left       = load_tex(p);
    snprintf(p,sizeof(p),"%s/player_right.png",      base_path);  sp->player.right      = load_tex(p);

    snprintf(p,sizeof(p),"%s/npc_front.png", base_path);     sp->npc.front     = load_tex(p);
    snprintf(p,sizeof(p),"%s/npc_back.png",  base_path);     sp->npc.back      = load_tex(p);
    snprintf(p,sizeof(p),"%s/npc_left.png",  base_path);     sp->npc.left      = load_tex(p);
    snprintf(p,sizeof(p),"%s/npc_right.png", base_path);     sp->npc.right     = load_tex(p);

    int ok_player = ok_tex(sp->player.front) && ok_tex(sp->player.front_walk) &&
                    ok_tex(sp->player.back)  && ok_tex(sp->player.left) &&
                    ok_tex(sp->player.right);
    int ok_npc    = ok_tex(sp->npc.front)    && ok_tex(sp->npc.back) &&
                    ok_tex(sp->npc.left)     && ok_tex(sp->npc.right);

    return (ok_player && ok_npc) ? 1 : 0;
}

void sprites_unload(Sprites* sp){
    if (sp->background.id)       UnloadTexture(sp->background);
    if (sp->player.front.id)     UnloadTexture(sp->player.front);
    if (sp->player.front_walk.id)UnloadTexture(sp->player.front_walk);
    if (sp->player.back.id)      UnloadTexture(sp->player.back);
    if (sp->player.left.id)      UnloadTexture(sp->player.left);
    if (sp->player.right.id)     UnloadTexture(sp->player.right);
    if (sp->npc.front.id)        UnloadTexture(sp->npc.front);
    if (sp->npc.back.id)         UnloadTexture(sp->npc.back);
    if (sp->npc.left.id)         UnloadTexture(sp->npc.left);
    if (sp->npc.right.id)        UnloadTexture(sp->npc.right);
}

Texture2D sprites_pick(const SpriteSet* set, FaceDir dir, int walking_front){
    if (dir == FACE_FRONT && walking_front) return set->front_walk;
    switch(dir){
        case FACE_BACK:  return set->back;
        case FACE_LEFT:  return set->left;
        case FACE_RIGHT: return set->right;
        case FACE_FRONT:
        default:         return set->front;
    }
}
