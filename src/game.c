#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include "game.h"
#include "eds.h"
#include "ia.h"

static uint64_t now_ms(void){
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return (uint64_t)tv.tv_sec*1000ULL + tv.tv_usec/1000ULL;
}

static void sleep_ms(int ms){ usleep(ms*1000); }

static int rnd_range(int a, int b){ return a + (rand() % (b - a + 1)); }

const char* sector_name(SectorType s){
    switch(s){
        case SECTOR_BALANCO: return "BALANCO";
        case SECTOR_SORVETE: return "SORVETE";
        case SECTOR_ESCORREGA: return "ESCORREGA";
        default: return "?";
    }
}

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

static EventType pick_event_for(SectorType s){
    if (s==SECTOR_BALANCO)   return EVT_BALANCO_MANUTENCAO;
    if (s==SECTOR_SORVETE)   return EVT_SORVETE_REABASTECER;
    return EVT_ESCORREGA_MOLHADO;
}

static void enqueue_sector_event_if_possible(GameState* gs, SectorType s){
    Event e; e.type=pick_event_for(s); e.sector=s; e.enq_ms=gs->now_ms;
    if (gs->queue.size < EVENT_QUEUE_CAP) queue_enqueue(&gs->queue, e); // agora sempre no fim da fila
}

static void maybe_generate_events(GameState* gs){
    if (!gs->events_initialized){
        for (int i=0; i<SECTOR_COUNT; i++)
            gs->next_event_ms[i] = gs->now_ms + (uint64_t)rnd_range(2000, 7000);
        gs->events_initialized = true;
    }

    float soma_paciencia = 0.0f;
    int total_npcs = 0;
    for (int k=0; k<gs->npc_count; k++){
        soma_paciencia += gs->npcs[k].patience;
        total_npcs++;
    }

    float media_paciencia = (total_npcs > 0) ? (soma_paciencia / (float)total_npcs) : 0.7f;
    float fator_humor;
    if      (media_paciencia < 0.30f) fator_humor = 0.50f;
    else if (media_paciencia < 0.50f) fator_humor = 0.80f;
    else if (media_paciencia < 0.70f) fator_humor = 1.00f;
    else if (media_paciencia < 0.85f) fator_humor = 1.15f;
    else                              fator_humor = 1.30f;

    for (int i=0; i<SECTOR_COUNT; i++){
        if (gs->now_ms >= gs->next_event_ms[i]){
            enqueue_sector_event_if_possible(gs, (SectorType)i);
            int base_min = 2000;
            int base_max = 7000;
            int intervalo = base_max - base_min;
            int aleatorio = rnd_range(0, intervalo);
            uint64_t delay = (uint64_t)((base_min + aleatorio) / fator_humor);
            if (delay < 800) delay = 800;
            gs->next_event_ms[i] = gs->now_ms + delay;
        }
    }
}

static void check_delay_penalties(GameState* gs){
    Event top;
    if(!queue_peek(&gs->queue,&top)) return;
    uint64_t waited = gs->now_ms - top.enq_ms;
    if (waited >= PENALTY_HARD_MS){
        gs->penalties_hard++;
        Event d; queue_dequeue(&gs->queue,&d);
        printf("PENALIDADE FORTE: '%s' atrasado.\n", event_name(top.type));
    } else if (waited >= PENALTY_SOFT_MS){
        gs->penalties_soft++;
        gs->queue.buf[0].enq_ms = gs->now_ms;
        printf("Penalidade leve: '%s' demorando.\n", event_name(top.type));
    }
}

void game_init(GameState* gs){
    gs->score=0;
    gs->penalties_soft=0;
    gs->penalties_hard=0;
    gs->start_ms=now_ms();
    gs->now_ms=gs->start_ms;
    srand((unsigned)gs->start_ms);
    gs->map_head=map_build_3();
    gs->player_pos=gs->map_head;
    queue_init(&gs->queue);
    gs->undo=stack_new(64);
    gs->player_x=450.0f;
    gs->player_y=500.0f;
    gs->last_input_ms=gs->now_ms;
    for (int i=0;i<SECTOR_COUNT;i++){
        gs->last_resolve_ms[i]=0;
        gs->next_event_ms[i]=0;
    }
    gs->events_initialized = false;
    npcs_init(gs, 6);
}

void game_shutdown(GameState* gs){
    stack_free(gs->undo);
    if(gs->map_head){
        Sector *a=gs->map_head,*p=a->next;
        for(int i=0;i<SECTOR_COUNT-1;i++){ Sector* n=p->next; free(p); p=n; }
        free(a);
    }
}

bool player_handle_top(GameState* gs){
    Event top;
    if(!queue_peek(&gs->queue,&top)) return false;
    Snapshot s; save_snapshot(gs,&s); stack_push_snap(gs->undo,&s);
    uint64_t waited = gs->now_ms - top.enq_ms;
    int base=10;
    if(waited<5000) gs->score += base+5;
    else if(waited<10000) gs->score += base+2;
    else gs->score += base;
    Event d; queue_dequeue(&gs->queue,&d);
    gs->last_resolve_ms[top.sector] = gs->now_ms;
    return true;
}

static int sector_has_active_event(const GameState* gs, SectorType t){
    for(int i=0;i<gs->queue.size;i++){
        if (gs->queue.buf[i].sector == t) return 1;
    }
    return 0;
}

static int count_waiting_at_sector(const GameState* gs, SectorType t){
    int c=0;
    for(int i=0;i<gs->npc_count;i++){
        const Npc* n=&gs->npcs[i];
        if (n->in_queue && n->current_sector && n->current_sector->type==t) c++;
    }
    return c;
}

void npcs_init(GameState* gs, int count){
    if (count > NPC_MAX) count = NPC_MAX;
    gs->npc_count = count;
    for(int i=0;i<count;i++){
        gs->npcs[i].current_sector = gs->map_head;
        gs->npcs[i].arrival_ms = gs->now_ms;
        gs->npcs[i].in_queue = 0;
        gs->npcs[i].queue_slot = 0;
        gs->npcs[i].patience = 0.70f;
        gs->npcs[i].wait_start_ms = 0;
        gs->npcs[i].preferred = (SectorType)(i % SECTOR_COUNT);
    }
}

static void npc_tick_one(GameState* gs, Npc* n){
    if (!n->current_sector) return;
    int preferido = (n->current_sector->type == n->preferred) ? 1 : 0;

    if (sector_has_active_event(gs, n->current_sector->type)){
        if (!n->in_queue){
            n->in_queue = 1;
            n->queue_slot = count_waiting_at_sector(gs, n->current_sector->type);
            n->wait_start_ms = gs->now_ms;
        } else {
            float delta_s = (float)(GAME_TICK_MS / 1000.0f);
            int same_sector_as_player = (gs->player_pos && gs->player_pos->type == n->current_sector->type);
            uint64_t last_resolve = gs->last_resolve_ms[n->current_sector->type];
            int interaction_recent = same_sector_as_player && (gs->now_ms - last_resolve <= 1200);
            if (interaction_recent) delta_s = 0.0f;

            float pred = ia_predict_paciencia(n->patience, preferido, delta_s, 0.0f);
            n->patience = pred;
            if (n->patience < 0.25f){
                n->in_queue = 0;
                n->current_sector = step_next(n->current_sector);
                n->arrival_ms = gs->now_ms;
                n->patience += 0.10f;
                if (n->patience > 1.0f) n->patience = 1.0f;
                return;
            }
        }
        return;
    }

    if (n->in_queue){
        n->in_queue = 0;
        n->arrival_ms = gs->now_ms;
        n->patience = ia_predict_paciencia(n->patience, preferido, 0.0f, +0.05f);
        return;
    }

    if (gs->now_ms - n->arrival_ms >= NPC_STOP_MS){
        n->current_sector = step_next(n->current_sector);
        n->arrival_ms = gs->now_ms;
        n->patience = ia_predict_paciencia(n->patience, preferido, 0.0f, +0.02f);
    }
}

void npcs_tick(GameState* gs){
    for(int i=0;i<gs->npc_count;i++){
        npc_tick_one(gs, &gs->npcs[i]);
    }
}

void game_tick(GameState* gs){
    gs->now_ms=now_ms();
    maybe_generate_events(gs);
    check_delay_penalties(gs);
    npcs_tick(gs);
    sleep_ms(10);
}

bool player_undo(GameState* gs){
    Snapshot s;
    if(!stack_pop_snap(gs->undo,&s)) return false;
    restore_snapshot(gs,&s);
    return true;
}