#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include "game.h"
#include "eds.h"

static uint64_t now_ms(void){ struct timeval tv; gettimeofday(&tv,NULL); return (uint64_t)tv.tv_sec*1000ULL + tv.tv_usec/1000ULL; }
static void     sleep_ms(int ms){ usleep(ms*1000); }

/* nomes */
const char* sector_name(SectorType s){ return (s==SECTOR_BALANCO)?"BALANCO":(s==SECTOR_SORVETE)?"SORVETE":"ESCORREGA"; }
const char* event_name(EventType e){
  switch(e){
    case EVT_BALANCO_QUER_BRINCAR: return "crianca quer brincar no balanco";
    case EVT_BALANCO_MANUTENCAO:   return "corrente soltou (manutencao rapida)";
    case EVT_SORVETE_FILA:         return "fila do sorvete crescendo";
    case EVT_SORVETE_REABASTECER:  return "acabou cobertura; reabastecer";
    case EVT_ESCORREGA_QUER:       return "crianca quer escorregar";
    case EVT_ESCORREGA_MOLHADO:    return "escorregador molhado; secar";
    default: return "?";
  }
}

/* geração de eventos */
static EventType pick_event_for(SectorType s){
  int bit = (int)((now_ms()/1000) % 2);
  if (s==SECTOR_BALANCO)   return bit?EVT_BALANCO_QUER_BRINCAR:EVT_BALANCO_MANUTENCAO;
  if (s==SECTOR_SORVETE)   return bit?EVT_SORVETE_FILA:EVT_SORVETE_REABASTECER;
  return bit?EVT_ESCORREGA_QUER:EVT_ESCORREGA_MOLHADO;
}
static void enqueue_sector_event_if_possible(GameState* gs, SectorType s){
  Event e={ .type=pick_event_for(s), .sector=s, .enq_ms=gs->now_ms };
  (void)queue_ordered_insert(&gs->queue, e); // se cheio, ignora (sem penalidade)
}
static void maybe_generate_events(GameState* gs){
  static uint64_t last_ms=0; if(!last_ms) last_ms=gs->now_ms;
  if (gs->now_ms - last_ms >= NPC_ACTION_MS){ for(int i=0;i<SECTOR_COUNT;i++) enqueue_sector_event_if_possible(gs,(SectorType)i); last_ms=gs->now_ms; }
}

/* penalidades por atraso no topo */
static void check_delay_penalties(GameState* gs){
  Event top; if(!queue_peek(&gs->queue,&top)) return;
  uint64_t waited = gs->now_ms - top.enq_ms;
  if (waited >= PENALTY_HARD_MS){
    gs->penalties_hard++; Event d; queue_dequeue(&gs->queue,&d);
    printf("PENALIDADE FORTE: '%s' atrasado.\n", event_name(top.type));
  } else if (waited >= PENALTY_SOFT_MS){
    gs->penalties_soft++; gs->queue.buf[0].enq_ms = gs->now_ms; // evita repetir leve em loop
    printf("Penalidade leve: '%s' demorando.\n", event_name(top.type));
  }
}

/* API pública */
void game_init(GameState* gs){
  gs->score=gs->penalties_soft=gs->penalties_hard=0;
  gs->start_ms=gs->now_ms=now_ms();
  gs->map_head=map_build_3(); gs->player_pos=gs->map_head;
  queue_init(&gs->queue);
  gs->undo=stack_new(64);
}
void game_shutdown(GameState* gs){
  stack_free(gs->undo);
  if(gs->map_head){ Sector *a=gs->map_head,*p=a->next; for(int i=0;i<SECTOR_COUNT-1;i++){ Sector* n=p->next; free(p); p=n;} free(a); }
}
void game_tick(GameState* gs){ gs->now_ms=now_ms(); maybe_generate_events(gs); check_delay_penalties(gs); }

void player_move_next(GameState* gs){ Snapshot s; save_snapshot(gs,&s); stack_push_snap(gs->undo,&s); gs->player_pos=step_next(gs->player_pos); }
bool player_handle_top(GameState* gs){
  Event top; if(!queue_peek(&gs->queue,&top)) return false;
  Snapshot s; save_snapshot(gs,&s); stack_push_snap(gs->undo,&s);
  uint64_t waited = gs->now_ms - top.enq_ms; int base=10;
  gs->score += (waited<5000)? base+5 : (waited<10000)? base+2 : base;
  Event d; queue_dequeue(&gs->queue,&d); return true;
}
bool player_undo(GameState* gs){ Snapshot s; if(!stack_pop_snap(gs->undo,&s)) return false; restore_snapshot(gs,&s); return true; }

/* (main de console aqui só para teste rápido — o seu pode ficar separado em src/main.c)
   Mantemos em src/main.c para organização. */
