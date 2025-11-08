#include "raylib.h"
#include <string.h>
#include <stdio.h>
#include "game.h"
#include "gui.h"
#include "ranking.h"
#include "ia.h"

typedef enum { STATE_MENU=0, STATE_NAME=1, STATE_GAME=2, STATE_RANKING=3, STATE_POSTGAME=4, STATE_EXIT=5 } AppState;

static const int WIN_W=900, WIN_H=600;
static const char* options[]={"iniciar o jogo","ver o ranking do jogo","sair do jogo"};
static const int optionsCount=3;
static int optionIndex=0;

static Texture2D bgTex;
static RenderTexture2D rtLow;
static Font pixFont;

static int finalPos=0;
static int finalScore=0;
static AppState postNext=STATE_MENU;
static char finalName[16]="";
static int gameAssetsLoaded=0;

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

static int draw_menu_and_handle_input(void){
    int btnW=420, btnH=64, gap=24;
    int totalH=optionsCount*btnH+(optionsCount-1)*gap;
    int startX=(WIN_W-btnW)/2;
    int startY=(WIN_H-totalH)/2 + 30;

    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) optionIndex=(optionIndex-1+optionsCount)%optionsCount;
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) optionIndex=(optionIndex+1)%optionsCount;

    Vector2 mp=GetMousePosition();
    int hover=-1;
    for(int i=0;i<optionsCount;i++){
        Rectangle r=(Rectangle){(float)startX,(float)(startY+i*(btnH+gap)),(float)btnW,(float)btnH};
        if (CheckCollisionPointRec(mp,r)) hover=i;
    }
    if (hover!=-1) optionIndex=hover;

    int chosen=-1;
    if (IsKeyPressed(KEY_ENTER)) chosen=optionIndex;
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && hover!=-1) chosen=hover;

    draw_blurred_bg();
    const char* title="SmartPark";
    int tw=MeasureTextEx(pixFont,title,48,1).x;
    DrawTextEx(pixFont,title,(Vector2){(float)(WIN_W/2 - tw/2), 100},48,1,RAYWHITE);

    for(int i=0;i<optionsCount;i++){
        Rectangle r=(Rectangle){(float)startX,(float)(startY+i*(btnH+gap)),(float)btnW,(float)btnH};
        draw_pixel_button(r,options[i], i==optionIndex);
    }

    const char* tip="use W/S ou setas para navegar, Enter ou clique para selecionar";
    int ttw=MeasureTextEx(pixFont,tip,16,1).x;
    DrawTextEx(pixFont,tip,(Vector2){(float)(WIN_W/2-ttw/2),(float)(WIN_H-60)},16,1,(Color){160,200,255,255});

    return chosen;
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

static int compute_position_by_score_only(const Score* arr, int n, int myScore){
    int pos=1;
    for(int i=0;i<n;i++){
        if (arr[i].score > myScore) pos++;
    }
    return pos;
}

static void draw_postgame_screen(int pos, int score){
    draw_blurred_bg();
    int btnW=420, btnH=64;
    Rectangle r=(Rectangle){(float)((WIN_W-btnW)/2), (float)((WIN_H-btnH)/2), (float)btnW, (float)btnH};
    Color borderCol=(Color){0,220,110,255};
    Color fillLight=(Color){110,240,170,255};
    Color fillDark=(Color){80,200,140,255};
    float mid=r.height*0.55f;
    DrawRectangle((int)r.x,(int)r.y,(int)r.width,(int)mid,fillLight);
    DrawRectangle((int)r.x,(int)(r.y+mid),(int)r.width,(int)(r.height-mid),fillDark);
    DrawRectangleLinesEx(r,4.0f,borderCol);
    Rectangle inner=(Rectangle){r.x+6,r.y+6,r.width-12,r.height-12};
    DrawRectangleLinesEx(inner,2.0f,borderCol);
    char text[64];
    snprintf(text,sizeof(text),"%d - %d",pos,score);
    int fs=32;
    int tw=MeasureTextEx(pixFont,text,(float)fs,1).x;
    int tx=(int)(r.x+(r.width-tw)/2);
    int ty=(int)(r.y+(r.height-fs)/2);
    DrawTextEx(pixFont,text,(Vector2){(float)tx,(float)ty},(float)fs,1,BLACK);
    const char* tip="Enter/ESC volta ao menu";
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
                if (!gameAssetsLoaded){ gui_load_assets(); gameAssetsLoaded=1; }
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
            int time_up = ((gs.now_ms - gs.start_ms) >= (uint64_t)GAME_TOTAL_SECONDS*1000ULL);
            int user_quit = IsKeyPressed(KEY_Q);
            if (time_up || user_quit){
                Score cur; memset(&cur,0,sizeof(cur));
                strncpy(cur.name,playerName,sizeof(cur.name)-1);
                cur.score=gs.score;
                cur.penalties_soft=gs.penalties_soft;
                cur.penalties_hard=gs.penalties_hard;
                Score arr[512];
                int n=ranking_load(arr,512);
                finalPos = compute_position_by_score_only(arr,n,cur.score);
                finalScore = cur.score;
                strncpy(finalName, cur.name, sizeof(finalName)-1);
                finalName[sizeof(finalName)-1]='\0';
                postNext = time_up ? STATE_MENU : STATE_EXIT;
                game_shutdown(&gs);
                ia_shutdown();
                if (gameAssetsLoaded){ gui_unload_assets(); gameAssetsLoaded=0; }
                state=STATE_POSTGAME;
                EndDrawing();
                continue;
            }
            game_tick(&gs);
            if (IsKeyPressed(KEY_H)) player_handle_top(&gs);
            if (IsKeyPressed(KEY_U)) player_undo(&gs);
            gui_draw(&gs, WIN_W, WIN_H);
            EndDrawing();
            continue;
        }
        if (state==STATE_POSTGAME){
            draw_postgame_screen(finalPos, finalScore);
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                Score toSave; memset(&toSave,0,sizeof(toSave));
                strncpy(toSave.name,finalName,sizeof(toSave.name)-1);
                toSave.score=finalScore;
                toSave.penalties_soft=0;
                toSave.penalties_hard=0;
                ranking_save_append(&toSave);
                state=STATE_MENU;
            }
            EndDrawing();
            continue;
        }
        EndDrawing();
    }
    if (state==STATE_GAME){
        game_shutdown(&gs);
        ia_shutdown();
        if (gameAssetsLoaded){ gui_unload_assets(); gameAssetsLoaded=0; }
    }
    unload_menu_assets();
    CloseWindow();
    return 0;
}
