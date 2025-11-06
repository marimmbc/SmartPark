#ifndef TYPES_H
#define TYPES_H
#include <stdbool.h>
#include <stdint.h>

#define GAME_TOTAL_SECONDS 180
#define GAME_TICK_MS 500
#define NPC_ACTION_MS 5000
#define EVENT_QUEUE_CAP 3
#define PENALTY_SOFT_MS 15000
#define PENALTY_HARD_MS 20000
#define NPC_MAX 6
#define NPC_STOP_MS 5000

typedef enum { SECTOR_BALANCO, SECTOR_SORVETE, SECTOR_ESCORREGA, SECTOR_COUNT } SectorType;

typedef enum {
  EVT_BALANCO_QUER_BRINCAR,
  EVT_BALANCO_MANUTENCAO,
  EVT_SORVETE_FILA,
  EVT_SORVETE_REABASTECER,
  EVT_ESCORREGA_QUER,
  EVT_ESCORREGA_MOLHADO,
  EVT_COUNT
} EventType;

typedef struct { EventType type; SectorType sector; uint64_t enq_ms; } Event;

typedef struct Sector { SectorType type; struct Sector* next; } Sector;

typedef struct { Event buf[EVENT_QUEUE_CAP]; int size; } EventQueue;

typedef struct { int top, cap; void** data; } Stack;

typedef struct Npc {
    Sector*   current_sector;
    uint64_t  arrival_ms;
    int       in_queue;
    int       queue_slot;

    float     patience;          
    SectorType preferred;        
    uint64_t  wait_start_ms;     
} Npc;

typedef struct {
  int score;
  int penalties_soft;
  int penalties_hard;
  uint64_t start_ms;
  uint64_t now_ms;
  Sector* map_head;
  Sector* player_pos;
  float player_x;
  float player_y;
  uint64_t last_input_ms;
  EventQueue queue;
  Stack* undo;
  Npc npcs[NPC_MAX];
  int npc_count;
} GameState;

typedef struct {
  int score;
  int penalties_soft;
  int penalties_hard;
  EventQueue queue;
  SectorType player_sector;
} Snapshot;

typedef struct {
  char name[32];
  int score;
  int penalties_soft;
  int penalties_hard;
} Score;

#endif
