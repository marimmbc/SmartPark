#include "gui.h"
#include "game.h"
#include "raylib.h"
#include "sprites.h"
#include <math.h>
#include <stdio.h>

static Vector2 sector_pos(int idx, int w, int h){
    float cx=w*0.5f, cy=h*0.5f, r=(w<h?w:h)*0.28f;
    float ang=(float)idx*(2.0f*PI/3.0f)-PI/2.0f;
    return (Vector2){ cx + r*(float)cosf(ang), cy + r*(float)sinf(ang) };
}

static int player_sector_index(const GameState* gs){
    int idx=0; Sector* s=gs->map_head;
    for(int i=0;i<SECTOR_COUNT;i++){ if(s==gs->player_pos){ idx=i; break; } s=s->next; }
    return idx;
}

void gui_draw(const GameState* gs, int win_w, int win_h){
    static int initialized = 0;
    static Sprites spr;
    static FaceDir player_dir = FACE_FRONT;
    static FaceDir npc_dir    = FACE_FRONT;

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

    bool moving = false;
    if (IsKeyDown(KEY_W)) { player_dir = FACE_BACK;  moving = true; }
    if (IsKeyDown(KEY_S)) { player_dir = FACE_FRONT; moving = true; }
    if (IsKeyDown(KEY_A)) { player_dir = FACE_LEFT;  moving = true; }
    if (IsKeyDown(KEY_D)) { player_dir = FACE_RIGHT; moving = true; }
    if (!moving) player_dir = FACE_FRONT;

    npc_dir = FACE_FRONT;

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
        Vector2 p=sector_pos(i,win_w,win_h);
        DrawCircleLines((int)p.x,(int)p.y,48,(Color){90,140,255,180});
        const char* name=sector_name((SectorType)i);
        int tw=MeasureText(name, 18);
        DrawText(name, (int)(p.x - tw/2), (int)(p.y-9), 18, RAYWHITE);
    }

    int pidx = player_sector_index(gs);
    Vector2 pp = sector_pos(pidx, win_w, win_h);
    Texture2D ptex = sprites_pick(&spr.player, player_dir);
    if (ptex.id){
        float scale = 0.75f;
        float w = (float)ptex.width*scale, h=(float)ptex.height*scale;
        DrawTexturePro(ptex, (Rectangle){0,0,(float)ptex.width,(float)ptex.height},
                            (Rectangle){pp.x - w/2, pp.y - h/1.2f, w, h},
                            (Vector2){0,0}, 0.0f, WHITE);
    }

    int nidx = (pidx + 1) % SECTOR_COUNT;
    Vector2 np = sector_pos(nidx, win_w, win_h);
    Texture2D ntex = sprites_pick(&spr.npc, npc_dir);
    if (ntex.id){
        float scale = 0.72f;
        float w = (float)ntex.width*scale, h=(float)ntex.height*scale;
        DrawTexturePro(ntex, (Rectangle){0,0,(float)ntex.width,(float)ntex.height},
                            (Rectangle){np.x - w/2, np.y - h/1.2f, w, h},
                            (Vector2){0,0}, 0.0f, WHITE);
    }

    int x=24,y=24;
    DrawText("SmartPark", x,y,22, RAYWHITE);
    y+=30;
    char buf[128];
    unsigned long tsec=(unsigned long)((gs->now_ms - gs->start_ms)/1000);
    snprintf(buf,sizeof(buf),"Time: %lus  Score: %d  Pen: %d/%d", tsec, gs->score, gs->penalties_soft, gs->penalties_hard);
    DrawText(buf,x,y,18,(Color){220,220,240,255});
    y+=30;
    DrawText("Queue (top ->):",x,y,18,(Color){200,200,220,255});
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
    DrawText("[A] prev  [D] next  [W/S] face  [H] handle  [U] undo  [Q] quit", x,y,16,(Color){160,200,255,255});

    EndDrawing();

    if (WindowShouldClose()){
        sprites_unload(&spr);
        initialized = 0;
    }
}
