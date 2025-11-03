#ifndef EDS_H
#define EDS_H
#include "types.h"

Sector* map_build_3(void);
Sector* step_next(Sector* s);
Sector* step_prev(Sector* s, Sector* head);

void  queue_init(EventQueue* q);
bool  queue_is_full(const EventQueue* q);
bool  queue_is_empty(const EventQueue* q);
bool  queue_peek(const EventQueue* q, Event* out);
bool  queue_dequeue(EventQueue* q, Event* out);
bool  queue_enqueue(EventQueue* q, Event e);

int   event_priority(EventType t);

Stack* stack_new(int cap);
void   stack_free(Stack* s);
bool   stack_push_snap(Stack* s, const Snapshot* snap);
bool   stack_pop_snap(Stack* s, Snapshot* out);

void save_snapshot(const GameState* gs, Snapshot* out);
void restore_snapshot(GameState* gs, const Snapshot* snap);

#endif
