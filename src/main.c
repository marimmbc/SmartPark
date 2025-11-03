#include <stdio.h>
#include <ctype.h>
#include "game.h"

static void draw(const GameState* gs){
  printf("\n=== t=%lus ===\n", (unsigned long)((gs->now_ms - gs->start_ms)/1000));
  printf("Local: %s | Score: %d | Penalidades: %d leves, %d fortes\n",
    sector_name(gs->player_pos->type), gs->score, gs->penalties_soft, gs->penalties_hard);
  printf("Fila (%d/%d):\n", gs->queue.size, EVENT_QUEUE_CAP);
  for(int i=0;i<gs->queue.size;i++){
    const Event* e=&gs->queue.buf[i];
    printf("  %c %s [%s]\n", i==0?'>':' ', event_name(e->type), sector_name(e->sector));
  }
  printf("[M]over  [H]andle topo  [U]ndo  [Q]uit\n");
}

int main(void){
  GameState gs; game_init(&gs);
  while( (gs.now_ms - gs.start_ms) < (uint64_t)GAME_TOTAL_SECONDS*1000ULL ){
    game_tick(&gs); draw(&gs);
    int ch=fgetc(stdin); if(ch==EOF) continue; ch=tolower(ch);
    if(ch=='m') player_move_next(&gs);
    else if(ch=='h'){ if(!player_handle_top(&gs)) printf("Nada para resolver.\n"); }
    else if(ch=='u'){ if(!player_undo(&gs)) printf("Nada para desfazer.\n"); }
    else if(ch=='q') break;
  }
  printf("\nFIM! Score: %d | Penalidades: %d/%d\n", gs.score, gs.penalties_soft, gs.penalties_hard);
  game_shutdown(&gs); return 0;
}
