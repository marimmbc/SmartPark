#include "gui.h"
#include "game.h"
#include "types.h"
#include "raylib.h"
#include "sprites.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

static int loaded = 0;
static Sprites spr;
static int sprites_ready = 0;
static FaceDir player_dir = FACE_FRONT;

static Vector2 sector_pos(int idx, int w, int h){
    float cx=w*0.5f, cy=h*0.5f, r=(w<h?w:h)*0.28f;
    float ang=(float)idx*(2.0f*PI/3.0f)-PI/2.0f;
    return (Vector2){ cx + r*(float)cosf(ang), cy + r*(float)sinf(ang) };
}

static int sector_index_from_ptr(const GameState* gs, const Sector* s){
    const Sector* it=gs->map_head;
    for(int i=0;i<SECTOR_COUNT;i++){
        if(it==s) return i;
        it=it->next;
    }
    return 0;
}

static int player_sector_index(const GameState* gs){
    const Sector* s=gs->map_head;
    for(int i=0;i<SECTOR_COUNT;i++){
        if(s==gs->player_pos) return i;
        s=s->next;
    }
    return 0;
}

void gui_load(void){
    if (loaded) return;
    if (!sprites_load(&spr, "assets")) {
        sprites_ready = 0;
    } else {
        sprites_ready = 1;
    }
    loaded = 1;
}

void gui_unload(void){
    if (!loaded) return;
    if (sprites_ready) {
        sprites_unload(&spr);
        sprites_ready = 0;
    }
    loaded = 0;
}

void gui_draw(GameState* gs, int win_w, int win_h){
    if (!loaded) gui_load();

    if (!sprites_ready){
        DrawText("Falha ao carregar sprites de ./assets", 30, 30, 20, RED);
        return;
    }

    bool held = false;
    if (IsKeyDown(KEY_W)) { player_dir = FACE_BACK;  held = true; }
    if (IsKeyDown(KEY_S)) { player_dir = FACE_FRONT; held = true; }
    if (IsKeyDown(KEY_A)) { player_dir = FACE_LEFT;  held = true; }
    if (IsKeyDown(KEY_D)) { player_dir = FACE_RIGHT; held = true; }
    if (!held) player_dir = FACE_FRONT;

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
    Texture2D ptex = sprites_pick(&spr.player, player_dir, 0);
    if (ptex.id){
        float scale = 0.75f;
        float w = (float)ptex.width*scale, h=(float)ptex.height*scale;
        DrawTexturePro(ptex, (Rectangle){0,0,(float)ptex.width,(float)ptex.height},
                            (Rectangle){pp.x - w/2, pp.y - h/1.2f, w, h},
                            (Vector2){0,0}, 0.0f, WHITE);
    }

    for(int i=0;i<gs->npc_count;i++){
        const Npc* n=&gs->npcs[i];
        const Sector* cur = n->current_sector;
        int nidx = cur ? sector_index_from_ptr(gs, cur) : 0;
        Vector2 np = sector_pos(nidx, win_w, win_h);
        float offsetY = n->in_queue ? (18.0f + n->queue_slot*10.0f) : 0.0f;

        FaceDir npc_dir = FACE_FRONT;
        Texture2D ntex = sprites_pick(&spr.npc, npc_dir, 0);
        if (ntex.id){
            float scale = 0.72f;
            float w = (float)ntex.width*scale, h=(float)ntex.height*scale;
            float drawX = np.x - w/2;
            float drawY = np.y - h/1.2f + offsetY;
            DrawTexturePro(ntex, (Rectangle){0,0,(float)ntex.width,(float)ntex.height},
                                (Rectangle){drawX, drawY, w, h},
                                (Vector2){0,0}, 0.0f, WHITE);

            float patience = n->patience;
            if (patience < 0.0f) patience = 0.0f;
            if (patience > 1.0f) patience = 1.0f;

            Color barColor;
            if (patience >= 0.8f)      barColor = (Color){0,220,0,255};
            else if (patience >= 0.6f) barColor = (Color){120,255,120,255};
            else if (patience >= 0.4f) barColor = (Color){255,220,0,255};
            else                       barColor = (Color){255,60,60,255};

            float barWidth = 40.0f;
            float barHeight = 6.0f;
            float barX = np.x - barWidth/2.0f;
            float barY = drawY - 12.0f;
            float fillWidth = barWidth * patience;

            DrawRectangle((int)barX, (int)barY, (int)barWidth, (int)barHeight, (Color){40,40,40,180});
            DrawRectangle((int)barX, (int)barY, (int)fillWidth, (int)barHeight, barColor);
            DrawRectangleLines((int)barX, (int)barY, (int)barWidth, (int)barHeight, (Color){10,10,10,220});
        }
    }

    int x=24,y=24;
    DrawText("SmartPark", x,y,22, RAYWHITE);
    y+=30;
    char buf[160];
    unsigned long tsec=(unsigned long)((gs->now_ms - gs->start_ms)/1000);
    snprintf(buf,sizeof(buf),"Tempo: %lus  Score: %d  Penalidades: %d/%d", tsec, gs->score, gs->penalties_soft, gs->penalties_hard);
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
    DrawText("[W/A] setor anterior  [S/D] setor seguinte  [H] resolver  [U] desfazer  [Q] sair", x,y,16,(Color){160,200,255,255});
}