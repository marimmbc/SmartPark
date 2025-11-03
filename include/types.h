#ifndef TYPES_H
#define TYPES_H
#include <stdbool.h>
#include <stdint.h>

#define GAME_TOTAL_SECONDS   180
#define GAME_TICK_MS         500
#define NPC_ACTION_MS        5000
#define EVENT_QUEUE_CAP      3
#define PENALTY_SOFT_MS      15000
#define PENALTY_HARD_MS      20000

typedef enum { SECTOR_BALANCO, SECTOR_SORVETE, SECTOR_ESCORREGA, SECTOR_COUNT } SectorType;
typedef enum {
  EVT_BALANCO_QUER_BRINCAR, EVT_BALANCO_MANUTENCAO,
  EVT_SORVETE_FILA, EVT_SORVETE_REABASTECER,
  EVT_ESCORREGA_QUER, EVT_ESCORREGA_MOLHADO, EVT_COUNT
} EventType;

typedef struct { EventType type; SectorType sector; uint64_t enq_ms; } Event;

typedef struct Sector { SectorType type; struct Sector* next; } Sector;           // lista circular
typedef struct { Event buf[EVENT_QUEUE_CAP]; int size; } EventQueue;              // fila cap 3
typedef struct { int top, cap; void** data; } Stack;                              // pilha genérica

typedef struct {
  int score, penalties_soft, penalties_hard;
  uint64_t start_ms, now_ms;
  Sector *map_head, *player_pos;
  EventQueue queue;
  Stack *undo;
} GameState;

typedef struct { int score, penalties_soft, penalties_hard; EventQueue queue; SectorType player_sector; } Snapshot;

#endif
