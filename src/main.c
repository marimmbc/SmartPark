#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "game.h"
#include "screen.h"
#include "keyboard.h"
#include "ranking.h"

int main(void){
  GameState gs; game_init(&gs);
  keyboard_init();
  while( (gs.now_ms - gs.start_ms) < (uint64_t)GAME_TOTAL_SECONDS*1000ULL ){
    game_tick(&gs);
    screen_draw(&gs);
    for(;;){
      int ch = getch_now();
      if (ch == -1) break;
      ch = tolower(ch);
      if (ch=='w' || ch=='a' || ch=='s' || ch=='d') player_move_next(&gs);
      else if (ch=='h'){ if(!player_handle_top(&gs)) printf("Nada para resolver.\n"); }
      else if (ch=='u'){ if(!player_undo(&gs)) printf("Nada para desfazer.\n"); }
      else if (ch=='q'){ goto end; }
    }
  }
end:
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

  keyboard_shutdown();
  game_shutdown(&gs);
  return 0;
}
