#include "raylib.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include "game.h"
#include "ranking.h"

int main(void){
    const int win_w = 900, win_h = 600;
    InitWindow(win_w, win_h, "SmartPark - GUI");
    SetTargetFPS(60);

    GameState gs;
    game_init(&gs);

    while (!WindowShouldClose()){
        if ( (gs.now_ms - gs.start_ms) >= (uint64_t)GAME_TOTAL_SECONDS*1000ULL ) break;

        game_tick(&gs);

        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_A)) player_move_prev(&gs);
        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_D)) player_move_next(&gs);
        if (IsKeyPressed(KEY_H)) player_handle_top(&gs);
        if (IsKeyPressed(KEY_U)) player_undo(&gs);
        if (IsKeyPressed(KEY_Q)) break;

        gui_draw(&gs, win_w, win_h);
    }

    CloseWindow();

    printf("\nFIM! Score: %d | Penalidades: %d/%d\n", gs.score, gs.penalties_soft, gs.penalties_hard);

    Score cur; memset(&cur,0,sizeof(cur));
    strncpy(cur.name,"player",sizeof(cur.name)-1);
    cur.score = gs.score;
    cur.penalties_soft = gs.penalties_soft;
    cur.penalties_hard = gs.penalties_hard;
    ranking_save_append(&cur);

    Score arr[512];
    int n = ranking_load(arr, 512);
    if (n>0){
        quicksort_scores(arr, 0, n-1);
        printf("\nRanking:\n");
        ranking_print_top(arr, n);
    }

    game_shutdown(&gs);
    return 0;
}
