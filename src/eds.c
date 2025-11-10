#include <stdlib.h>
#include <string.h>
#include "eds.h"

static Sector* new_node(SectorType t){
  Sector* n = (Sector*)malloc(sizeof(Sector));
  n->type = t;
  n->next = NULL;
  return n;
}

Sector* map_build_3(void){
  Sector *a=new_node(SECTOR_BALANCO), *b=new_node(SECTOR_SORVETE), *c=new_node(SECTOR_ESCORREGA);
  a->next=b; b->next=c; c->next=a;
  return a;
}

Sector* step_next(Sector* s){
  return s ? s->next : NULL;
}

Sector* step_prev(Sector* s, Sector* head){
  if (!s || !head) return NULL;
  Sector* p = head;
  for (int k=0; k<SECTOR_COUNT; k++){
    if (p->next == s) return p;
    p = p->next;
  }
  return NULL;
}

void queue_init(EventQueue* q){ q->size=0; }
bool queue_is_full(const EventQueue* q){ return q->size>=EVENT_QUEUE_CAP; }
bool queue_is_empty(const EventQueue* q){ return q->size==0; }

bool queue_peek(const EventQueue* q, Event* out){
  if(!q->size) return false;
  if(out) *out=q->buf[0];
  return true;
}

bool queue_dequeue(EventQueue* q, Event* out){
  if(!q->size) return false;
  if(out) *out=q->buf[0];
  if (q->size > 1){
    memmove(&q->buf[0], &q->buf[1], sizeof(Event)*(q->size-1));
  }
  q->size--;
  return true;
}

bool queue_enqueue(EventQueue* q, Event e){
  if (queue_is_full(q)) return false;
  q->buf[q->size] = e;
  q->size++;
  return true;
}

Stack* stack_new(int cap){
  Stack* s=(Stack*)malloc(sizeof(Stack));
  s->top=-1; s->cap=cap; s->data=(void**)malloc(sizeof(void*)*cap);
  return s;
}

void stack_free(Stack* s){
  if(!s) return;
  for(int i=0;i<=s->top;i++) free(s->data[i]);
  free(s->data);
  free(s);
}

bool stack_push_snap(Stack* s, const Snapshot* snap){
  if(s->top+1>=s->cap) return false;
  Snapshot* c=(Snapshot*)malloc(sizeof(Snapshot));
  *c=*snap;
  s->data[++s->top]=c;
  return true;
}

bool stack_pop_snap(Stack* s, Snapshot* out){
  if(s->top<0) return false;
  Snapshot* c=(Snapshot*)s->data[s->top--];
  if(out) *out=*c;
  free(c);
  return true;
}

void save_snapshot(const GameState* gs, Snapshot* out){
  out->score=gs->score;
  out->penalties_soft=gs->penalties_soft;
  out->penalties_hard=gs->penalties_hard;
  out->queue=gs->queue;
  out->player_sector=gs->player_pos->type;
}

void restore_snapshot(GameState* gs, const Snapshot* snap){
  gs->score=snap->score;
  gs->penalties_soft=snap->penalties_soft;
  gs->penalties_hard=snap->penalties_hard;
  gs->queue=snap->queue;
  Sector* p=gs->map_head;
  for(int i=0;i<SECTOR_COUNT;i++){
    if(p->type==snap->player_sector){ gs->player_pos=p; return; }
    p=p->next;
  }
  gs->player_pos = gs->map_head;
}
