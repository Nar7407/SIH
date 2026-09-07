#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX 100

typedef enum {
    TELEMETRY = 1,
    SSTV_IMAGE = 2,
    VOICE_DATA = 3,
    SCIENTIFIC_PAYLOAD = 4
} PacketType;

typedef struct {
    int id;
    PacketType type;
    char data[256];
    int priority;
} Packet;

typedef struct {
    Packet items[MAX];
    int size;
} PriorityQueue;

static int g_nextId = 1;

char* getPacketTypeName(PacketType type) {
    switch (type) {
        case TELEMETRY:           return "Telemetry (TT&C)";
        case SSTV_IMAGE:          return "SSTV Image";
        case VOICE_DATA:          return "Voice Data (Codec2/M17)";
        case SCIENTIFIC_PAYLOAD:  return "Scientific Payload";
        default:                  return "Unknown";
    }
}

int getPriorityForType(PacketType t) {
    switch (t) {
        case TELEMETRY:           return 1;
        case VOICE_DATA:          return 2;
        case SSTV_IMAGE:          return 3;
        case SCIENTIFIC_PAYLOAD:  return 4;
        default:                  return 5;
    }
}

Packet createPacket(PacketType type, const char* data) {
    Packet p;
    p.id = g_nextId++;
    p.type = type;
    p.priority = getPriorityForType(type);
    strncpy(p.data, data, sizeof(p.data) - 1);
    p.data[sizeof(p.data) - 1] = '\0';
    return p;
}

void initPQ(PriorityQueue *pq) {
    pq->size = 0;
}

int pqIsEmpty(PriorityQueue *pq) {
    return pq->size == 0;
}

int pqIsFull(PriorityQueue *pq) {
    return pq->size == MAX;
}

void pqEnqueue(PriorityQueue *pq, Packet p) {
    if (pqIsFull(pq)) {
        printf("[PQ] Overflow! Cannot enqueue packet %d.\n", p.id);
        return;
    }
    int pos = pq->size;
    while (pos > 0 && pq->items[pos - 1].priority > p.priority) {
        pq->items[pos] = pq->items[pos - 1];
        pos--;
    }
    pq->items[pos] = p;
    pq->size++;
    printf("[PQ] Packet %d enqueued (priority %d). Size: %d/%d\n",
           p.id, p.priority, pq->size, MAX);
}

Packet pqDequeue(PriorityQueue *pq) {
    Packet empty = {0, TELEMETRY, "", 0};
    if (pqIsEmpty(pq)) {
        printf("[PQ] Underflow! Nothing to dequeue.\n");
        return empty;
    }
    Packet item = pq->items[0];
    for (int i = 1; i < pq->size; i++)
        pq->items[i - 1] = pq->items[i];
    pq->size--;
    printf("[PQ] Packet %d dequeued (priority %d). Size: %d/%d\n",
           item.id, item.priority, pq->size, MAX);
    return item;
}

Packet pqPeek(PriorityQueue *pq) {
    Packet empty = {0, TELEMETRY, "", 0};
    if (pqIsEmpty(pq)) {
        printf("[PQ] Queue is empty.\n");
        return empty;
    }
    return pq->items[0];
}

void pqDisplay(PriorityQueue *pq) {
    if (pqIsEmpty(pq)) {
        printf("[PQ] Queue is empty.\n");
        return;
    }
    printf("\n=== PRIORITY QUEUE (%d/%d packets) ===\n", pq->size, MAX);
    printf("%-4s %-8s %-25s %-6s\n", "Pos", "ID", "Type", "Prio");
    printf("----------------------------------------\n");
    for (int i = 0; i < pq->size; i++) {
        printf("%-4d %-8d %-25s %-6d\n",
               i + 1, pq->items[i].id,
               getPacketTypeName(pq->items[i].type),
               pq->items[i].priority);
    }
    printf("========================================\n");
    printf("Front -> [Highest priority, transmitted first]\n");
    printf("Rear  -> [Lowest priority]\n");
}

int main() {
    PriorityQueue pq;
    initPQ(&pq);

    pqEnqueue(&pq, createPacket(TELEMETRY, "alt=400km vel=7.66km/s batt=87%"));
    pqEnqueue(&pq, createPacket(SCIENTIFIC_PAYLOAD, "dosimeter=0.12mSv temp=22.4C"));
    pqEnqueue(&pq, createPacket(VOICE_DATA, "M17 codec2 1200bps SNR=18dB"));
    pqEnqueue(&pq, createPacket(SSTV_IMAGE, "frame_0x3A sync_ok lines=240"));
    pqEnqueue(&pq, createPacket(TELEMETRY, "alt=405km vel=7.65km/s batt=85%"));

    pqDisplay(&pq);

    printf("\n--- Dequeue all packets (priority order) ---\n");
    while (!pqIsEmpty(&pq)) {
        Packet p = pqDequeue(&pq);
        printf("Transmitted: ID=%d Type=%s Priority=%d Data=%s\n",
               p.id, getPacketTypeName(p.type), p.priority, p.data);
    }

    return 0;
}