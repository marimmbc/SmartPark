#include "raylib.h"
#include <string.h>
#include <stdio.h>
#include "game.h"
#include "gui.h"
#include "ranking.h"
#include "ia.h"

typedef enum { STATE_MENU=0, STATE_NAME=1, STATE_GAME=2, STATE_RANKING=3, STATE_EXIT=4 } AppState;

static const int WIN_W=900, WIN_H=600;
static const char* options[]={"iniciar o jogo","ver o ranking do jogo","sair do jogo"};
static const int optionsCount=3;
static int optionIndex=0;

static Texture2D bgTex;
static RenderTexture2D rtLow;
static Font pixFont;

static void init_menu_assets(void){
    bgTex = LoadTexture("assets/bg.png");
    rtLow = LoadRenderTexture(225, 150);
    pixFont = GetFontDefault();
    SetTextureFilter(pixFont.texture, TEXTURE_FILTER_POINT);
}

static void unload_menu_assets(void){
    if (bgTex.id) UnloadTexture(bgTex);
    if (rtLow.id) UnloadRenderTexture(rtLow);
}

static void draw_blurred_bg(void){
    BeginTextureMode(rtLow);
    ClearBackground(BLACK);
    if (bgTex.id){
        float sx=(float)bgTex.width, sy=(float)bgTex.height;
        float tx=(float)rtLow.texture.width, ty=(float)rtLow.texture.height;
        DrawTexturePro(bgTex,(Rectangle){0,0,sx,sy},(Rectangle){0,0,tx,ty},(Vector2){0,0},0,WHITE);
    }
    EndTextureMode();
    SetTextureFilter(rtLow.texture, TEXTURE_FILTER_BILINEAR);
    DrawTexturePro(rtLow.texture,(Rectangle){0,0,(float)rtLow.texture.width,(float)-rtLow.texture.height},(Rectangle){0,0,(float)WIN_W,(float)WIN_H},(Vector2){0,0},0,WHITE);
    DrawRectangle(0,0,WIN_W,WIN_H,(Color){0,0,0,80});
}

static void draw_pixel_button(Rectangle r,const char* text,bool focused){
    Color textCol= focused ? (Color){0,255,128,255} : (Color){0,220,110,255};
    Color borderCol=textCol;
    Color fillLight= focused ? (Color){140,255,190,255} : (Color){110,240,170,255};
    Color fillDark= focused ? (Color){90,220,150,255} : (Color){80,200,140,255};
    float mid=r.height*0.55f;
    DrawRectangle((int)r.x,(int)r.y,(int)r.width,(int)mid,fillLight);
    DrawRectangle((int)r.x,(int)(r.y+mid),(int)r.width,(int)(r.height-mid),fillDark);
    DrawRectangleLinesEx(r,4.0f,borderCol);
    Rectangle inner=(Rectangle){r.x+6,r.y+6,r.width-12,r.height-12};
    DrawRectangleLinesEx(inner,2.0f,borderCol);
    if (focused) DrawRectangle((int)r.x,(int)(r.y+r.height),(int)r.width,4,(Color){0,60,40,120});
    int fs=28;
    int tw=MeasureTextEx(pixFont,text,(float)fs,1).x;
    int tx=(int)(r.x+(r.width-tw)/2);
    int ty=(int)(r.y+(r.height-fs)/2);
    DrawTextEx(pixFont,text,(Vector2){(float)tx,(float)ty},(float)fs,1,textCol);
}

static void draw_name_input(char* nameBuf){
    draw_blurred_bg();
    const char* msg = "Digite seu nome:";
    int mw = MeasureTextEx(pixFont, msg, 32, 1).x;
    DrawTextEx(pixFont, msg, (Vector2){(float)(WIN_W/2 - mw/2), 200}, 32, 1, RAYWHITE);
    int boxW = 420, boxH = 60;
    Rectangle box = (Rectangle){(float)(WIN_W/2 - boxW/2), 260, (float)boxW, (float)boxH};
    DrawRectangleRec(box, WHITE);
    DrawRectangleLinesEx(box, 4.0f, (Color){0,255,128,255});
    char temp[16];
    strncpy(temp, nameBuf, 15);
    temp[15] = '\0';
    int fs = 28;
    int tw = MeasureTextEx(pixFont, temp, fs, 1).x;
    DrawTextEx(pixFont, temp, (Vector2){box.x + (box.width - tw)/2, box.y + (box.height - fs)/2}, fs, 1, BLACK);
    const char* tip = "pressione Enter para continuar";
    int ttw = MeasureTextEx(pixFont, tip, 16, 1).x;
    DrawTextEx(pixFont, tip, (Vector2){(float)(WIN_W/2 - ttw/2), (float)(WIN_H - 60)}, 16, 1, (Color){160,200,255,255});
}

static void draw_ranking_screen(void){
    draw_blurred_bg();
    const char* title="Ranking";
    int tw=MeasureTextEx(pixFont,title,44,1).x;
    DrawTextEx(pixFont,title,(Vector2){(float)(WIN_W/2 - tw/2), 60},44,1,RAYWHITE);
    Score arr[512];
    int n=ranking_load(arr,512);
    if (n>0){
        quicksort_scores(arr,0,n-1);
        int x=150,y=140;
        DrawTextEx(pixFont,"pos",(Vector2){(float)x,(float)y},20,1,(Color){200,200,220,255});
        DrawTextEx(pixFont,"nome",(Vector2){(float)(x+80),(float)y},20,1,(Color){200,200,220,255});
        DrawTextEx(pixFont,"score",(Vector2){(float)(x+320),(float)y},20,1,(Color){200,200,220,255});
        DrawTextEx(pixFont,"penal leves/fortes",(Vector2){(float)(x+420),(float)y},20,1,(Color){200,200,220,255});
        y+=30;
        char buf[256];
        int show=n<10?n:10;
        for(int i=0;i<show;i++){
            snprintf(buf,sizeof(buf),"%d",i+1);
            DrawTextEx(pixFont,buf,(Vector2){(float)x,(float)y},20,1,RAYWHITE);
            DrawTextEx(pixFont,arr[i].name,(Vector2){(float)(x+80),(float)y},20,1,RAYWHITE);
            snprintf(buf,sizeof(buf),"%d",arr[i].score);
            DrawTextEx(pixFont,buf,(Vector2){(float)(x+320),(float)y},20,1,RAYWHITE);
            snprintf(buf,sizeof(buf),"%d / %d",arr[i].penalties_soft,arr[i].penalties_hard);
            DrawTextEx(pixFont,buf,(Vector2){(float)(x+420),(float)y},20,1,RAYWHITE);
            y+=28;
        }
    } else {
        const char* msg="sem partidas registradas ainda";
        int mw=MeasureTextEx(pixFont,msg,22,1).x;
        DrawTextEx(pixFont,msg,(Vector2){(float)(WIN_W/2 - mw/2),160},22,1,(Color){220,220,240,255});
    }
    const char* tip="ESC volta ao menu";
    int ttw=MeasureTextEx(pixFont,tip,16,1).x;
    DrawTextEx(pixFont,tip,(Vector2){(float)(WIN_W/2-ttw/2),(float)(WIN_H-60)},16,1,(Color){160,200,255,255});
}

int main(void){
    InitWindow(WIN_W, WIN_H, "SmartPark");
    SetTargetFPS(60);
    init_menu_assets();
    AppState state=STATE_MENU;
    GameState gs; memset(&gs,0,sizeof(gs));
    char playerName[16]="";
    while(state!=STATE_EXIT && !WindowShouldClose()){
        BeginDrawing();
        ClearBackground(BLACK);
        if (state==STATE_MENU){
            int chosen=draw_menu_and_handle_input();
            if (chosen==0) state=STATE_NAME;
            else if (chosen==1) state=STATE_RANKING;
            else if (chosen==2) state=STATE_EXIT;
            EndDrawing();
            continue;
        }
        if (state==STATE_NAME){
            int key=GetCharPressed();
            while(key>0){
                if ((key>=32) && (key<=125) && (int)strlen(playerName)<15){
                    int len=(int)strlen(playerName);
                    playerName[len]=(char)key;
                    playerName[len+1]='\0';
                }
                key=GetCharPressed();
            }
            if (IsKeyPressed(KEY_BACKSPACE) && strlen(playerName)>0) playerName[strlen(playerName)-1]='\0';
            if (IsKeyPressed(KEY_ENTER) && strlen(playerName)>0){
                ia_init();
                game_init(&gs);
                state=STATE_GAME;
            }
            draw_name_input(playerName);
            EndDrawing();
            continue;
        }
        if (state==STATE_RANKING){
            if (IsKeyPressed(KEY_ESCAPE)) state=STATE_MENU;
            draw_ranking_screen();
            EndDrawing();
            continue;
        }
        if (state==STATE_GAME){
            if ((gs.now_ms - gs.start_ms) >= (uint64_t)GAME_TOTAL_SECONDS*1000ULL){
                Score cur; memset(&cur,0,sizeof(cur));
                strncpy(cur.name,playerName,sizeof(cur.name)-1);
                cur.score=gs.score;
                cur.penalties_soft=gs.penalties_soft;
                cur.penalties_hard=gs.penalties_hard;
                ranking_save_append(&cur);
                game_shutdown(&gs);
                ia_shutdown();
                state=STATE_MENU;
                EndDrawing();
                continue;
            }
            game_tick(&gs);
            if (IsKeyPressed(KEY_H)) player_handle_top(&gs);
            if (IsKeyPressed(KEY_U)) player_undo(&gs);
            if (IsKeyPressed(KEY_Q)) {
                Score cur; memset(&cur,0,sizeof(cur));
                strncpy(cur.name,playerName,sizeof(cur.name)-1);
                cur.score=gs.score;
                cur.penalties_soft=gs.penalties_soft;
                cur.penalties_hard=gs.penalties_hard;
                ranking_save_append(&cur);
                game_shutdown(&gs);
                ia_shutdown();
                state=STATE_EXIT;
                EndDrawing();
                continue;
            }
            gui_draw(&gs, WIN_W, WIN_H);
            EndDrawing();
            continue;
        }
        EndDrawing();
    }
    if (state==STATE_GAME){
        game_shutdown(&gs);
        ia_shutdown();
    }
    unload_menu_assets();
    CloseWindow();
    return 0;
}
