#include <stdio.h>
#include "screen.h"
#include "types.h"

extern const char* sector_name(SectorType);
extern const char* event_name(EventType);

void screen_draw(const GameState* gs){
    printf("\n=== t=%lus ===\n",(unsigned long)((gs->now_ms-gs->start_ms)/1000));
    printf("Local: %s | Score: %d | Penalidades: %d leves, %d fortes\n",
        sector_name(gs->player_pos->type), gs->score, gs->penalties_soft, gs->penalties_hard);
    printf("Fila (%d/%d):\n", gs->queue.size, EVENT_QUEUE_CAP);
    for(int i=0;i<gs->queue.size;i++){
        const Event* e=&gs->queue.buf[i];
        printf("  %c %s [%s]\n", i==0?'>':' ', event_name(e->type), sector_name(e->sector));
    }
    printf("[W/A/S/D] mover  [H] resolver topo  [U] desfazer  [Q] sair\n");
}
