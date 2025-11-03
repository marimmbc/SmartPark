#include "gui.h"
#include "game.h"
#include "raylib.h"
#include "sprites.h"
#include <math.h>
#include <stdio.h>

static Vector2 sector_pos_fixed(SectorType t){
    switch (t){
        case SECTOR_ESCORREGA: return (Vector2){660, 420};
        case SECTOR_SORVETE:   return (Vector2){450, 270};
        case SECTOR_BALANCO:   return (Vector2){230, 420};
        default:               return (Vector2){450, 550};
    }
}
static Vector2 sector_pos_idx(int idx){
    if (idx==0) return sector_pos_fixed(SECTOR_ESCORREGA);
    if (idx==1) return sector_pos_fixed(SECTOR_SORVETE);
    return sector_pos_fixed(SECTOR_BALANCO);
}

static Vector2 queue_base_pos(SectorType t){
    if (t==SECTOR_ESCORREGA) return (Vector2){660, 500};
    if (t==SECTOR_SORVETE)   return (Vector2){450, 340};
    return (Vector2){230, 500};
}

static const float LIMIT_MIN_X = 120.0f;
static const float LIMIT_MAX_X = 780.0f;
static const float LIMIT_MIN_Y = 200.0f;
static const float LIMIT_MAX_Y = 560.0f;

void gui_draw(const GameState* gs_ro, int win_w, int win_h){
    GameState* gs = (GameState*)gs_ro;

    static int initialized = 0;
    static Sprites spr;
    static FaceDir player_dir = FACE_FRONT;

    if (!initialized){
        if (!IsWindowReady()) return;
        if (!sprites_load(&spr, "assets")) {
            BeginDrawing();
            ClearBackground((Color){18,18,24,255});
            DrawText("Failed to load sprites from ./assets", 30, 30, 20, RED);
            EndDrawing();
            return;
        }
        initialized = 1;
    }

    const float SPEED = 4.0f;
    float vx = 0.0f, vy = 0.0f;
    int walking_front = 0;
    int any_key = 0;

    if (IsKeyDown(KEY_W)) { vy -= SPEED; player_dir = FACE_BACK;  any_key = 1; }
    if (IsKeyDown(KEY_S)) { vy += SPEED; player_dir = FACE_FRONT; any_key = 1; walking_front = 1; }
    if (IsKeyDown(KEY_A)) { vx -= SPEED; player_dir = FACE_LEFT;  any_key = 1; }
    if (IsKeyDown(KEY_D)) { vx += SPEED; player_dir = FACE_RIGHT; any_key = 1; }

    if (any_key){
        gs->player_x += vx;
        gs->player_y += vy;
        if (gs->player_x < LIMIT_MIN_X) gs->player_x = LIMIT_MIN_X;
        if (gs->player_x > LIMIT_MAX_X) gs->player_x = LIMIT_MAX_X;
        if (gs->player_y < LIMIT_MIN_Y) gs->player_y = LIMIT_MIN_Y;
        if (gs->player_y > LIMIT_MAX_Y) gs->player_y = LIMIT_MAX_Y;
        gs->last_input_ms = gs->now_ms;
    } else {
        if (gs->now_ms - gs->last_input_ms >= 2000ULL){
            player_dir = FACE_FRONT;
            walking_front = 0;
        }
    }

    BeginDrawing();

    if (spr.background.id){
        DrawTexturePro(
            spr.background,
            (Rectangle){0,0,(float)spr.background.width,(float)spr.background.height},
            (Rectangle){0,0,(float)win_w,(float)win_h},
            (Vector2){0,0}, 0.0f, WHITE
        );
    } else {
        ClearBackground((Color){18,18,24,255});
    }

    for(int i=0;i<SECTOR_COUNT;i++){
        Vector2 p=sector_pos_idx(i);
        DrawCircleLines((int)p.x,(int)p.y,48,(Color){90,140,255,180});
        const char* name=sector_name((SectorType)i);
        int tw=MeasureText(name, 18);
        DrawText(name, (int)(p.x - tw/2), (int)(p.y-9), 18, RAYWHITE);
    }

    int alvo_idx = -1;
    Event topo;
    if (gs->queue.size > 0 && queue_peek(&gs->queue, &topo)){
        alvo_idx = (int)topo.sector;
        Vector2 ap = sector_pos_fixed(topo.sector);
        float t = (float)(gs->now_ms % 1000) / 1000.0f;
        float pulse = 6.0f + 4.0f * sinf(t * 2.0f * PI);
        Color hi = (Color){255,215,0,220};
        DrawRing(ap, 54.0f, 54.0f + pulse, 0, 360, 64, hi);
    }

    Texture2D ptex = sprites_pick(&spr.player, player_dir, walking_front);
    if (ptex.id){
        float scale = 0.75f;
        float w = (float)ptex.width*scale, h=(float)ptex.height*scale;
        DrawTexturePro(ptex, (Rectangle){0,0,(float)ptex.width,(float)ptex.height},
                            (Rectangle){gs->player_x - w/2, gs->player_y - h/1.2f, w, h},
                            (Vector2){0,0}, 0.0f, WHITE);
    }

    for(int i=0;i<gs->npc_count;i++){
        const Npc* n = &gs->npcs[i];
        if (!n->current_sector) continue;

        Vector2 p;
        if (n->in_queue){
            Vector2 base = queue_base_pos(n->current_sector->type);
            p = (Vector2){ base.x, base.y + n->queue_slot * 24.0f };
        } else {
            p = sector_pos_fixed(n->current_sector->type);
        }

        Texture2D ntex = sprites_pick(&spr.npc, FACE_FRONT, 0);
        if (ntex.id){
            float scale = 0.72f;
            float w = (float)ntex.width*scale, h=(float)ntex.height*scale;
            DrawTexturePro(ntex, (Rectangle){0,0,(float)ntex.width,(float)ntex.height},
                                (Rectangle){p.x - w/2, p.y - h/1.2f, w, h},
                                (Vector2){0,0}, 0.0f, WHITE);
        }
    }

    int x=24,y=24;
    DrawText("SmartPark", x,y,22, RAYWHITE);
    y+=30;
    char buf[200];
    unsigned long tsec=(unsigned long)((gs->now_ms - gs->start_ms)/1000);
    snprintf(buf,sizeof(buf),"Tempo: %lus  Score: %d  Penalidades: %d/%d",
             tsec, gs->score, gs->penalties_soft, gs->penalties_hard);
    DrawText(buf,x,y,18,(Color){220,220,240,255});
    y+=30;

    DrawText("Fila (topo ->):",x,y,18,(Color){200,200,220,255});
    y+=26;
    for(int i=0;i<gs->queue.size;i++){
        const Event* e=&gs->queue.buf[i];
        const char* en=event_name(e->type);
        const char* sn=sector_name(e->sector);
        snprintf(buf,sizeof(buf),"%c %s [%s]", i==0?'>':' ', en, sn);
        DrawText(buf,x,y,16,(Color){220,220,240,255});
        y+=22;
    }
    y+=8;

    if (alvo_idx >= 0){
        snprintf(buf,sizeof(buf),"Objetivo: va ate [%s] e pressione H",
                 sector_name((SectorType)alvo_idx));
        DrawText(buf,x,y,16,(Color){255,235,140,255});
        y+=22;
    }

    DrawText("[W/A/S/D] mover  [H] resolver (perto do setor)  [U] desfazer  [Q] sair",
             x,y,16,(Color){160,200,255,255});

    EndDrawing();

    if (WindowShouldClose()){
        sprites_unload(&spr);
        initialized = 0;
    }
}
