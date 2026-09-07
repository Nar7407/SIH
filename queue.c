
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
    int front;
    int rear;
    int size;
} Queue;

typedef struct {
    Packet items[MAX];
    int size;
} PriorityQueue;

typedef struct {
    Packet items[MAX];
    int front;
    int rear;
    int size;
    int overwritten;  
} CircularBuffer;

typedef struct {
    Packet items[MAX];
    int front;
    int rear;
    int size;
} Deque;

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

PacketType getPacketType(void) {
    int type;
    printf("\nSelect Packet Type:\n");
    printf("  1. Telemetry (TT&C)\n");
    printf("  2. SSTV Image\n");
    printf("  3. Voice Data (Codec2/M17)\n");
    printf("  4. Scientific Payload\n");
    printf("Enter packet type (1-4): ");
    scanf("%d", &type);
    if (type < TELEMETRY || type > SCIENTIFIC_PAYLOAD) {
        printf("Invalid type, defaulting to Telemetry.\n");
        type = TELEMETRY;
    }
    return (PacketType)type;
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

Packet createPacketManual(void) {
    Packet p;
    p.id = g_nextId++;
    p.type = getPacketType();
    p.priority = getPriorityForType(p.type);

    printf("Enter data payload: ");
    getchar();  
    fgets(p.data, sizeof(p.data), stdin);
    p.data[strcspn(p.data, "\n")] = 0;
    return p;
}

Packet createPacketRandom(void) {
    Packet p;
    p.id = g_nextId++;
    p.type = (PacketType)(rand() % 4 + 1);
    p.priority = getPriorityForType(p.type);

    const char *payloads[] = {
        "TT&C: alt=400km vel=7.66km/s batt=87%",
        "SSTV: frame_0x3A sync_ok lines=240",
        "VOICE: M17 codec2 1200bps SNR=18dB",
        "SCI: dosimeter=0.12mSv temp=22.4C"
    };
    strncpy(p.data, payloads[p.type - 1], sizeof(p.data) - 1);
    p.data[sizeof(p.data) - 1] = '\0';
    return p;
}

void printPacket(const Packet *p, const char *prefix) {
    printf("%sPacket ID    : %d\n", prefix, p->id);
    printf("%sPacket Type  : %s\n", prefix, getPacketTypeName(p->type));
    printf("%sData         : %s\n", prefix, p->data);
    printf("%sPriority     : %d\n", prefix, p->priority);
}

void initQueue(Queue *q)           { q->front = 0; q->rear = -1; q->size = 0; }
int  queueIsEmpty(Queue *q)        { return q->size == 0; }
int  queueIsFull(Queue *q)         { return q->size == MAX; }

void queueEnqueue(Queue *q, Packet p) {
    if (queueIsFull(q)) {
        printf("\n[FIFO] Queue Overflow! Cannot enqueue packet %d.\n", p.id);
        return;
    }
    q->rear = (q->rear + 1) % MAX;
    q->items[q->rear] = p;
    q->size++;
    printf("[FIFO] Packet %d enqueued. Size: %d/%d\n", p.id, q->size, MAX);
}

Packet queueDequeue(Queue *q) {
    Packet empty = {0, TELEMETRY, "", 0};
    if (queueIsEmpty(q)) {
        printf("\n[FIFO] Queue Underflow! Nothing to dequeue.\n");
        return empty;
    }
    Packet item = q->items[q->front];
    q->front = (q->front + 1) % MAX;
    q->size--;
    printf("[FIFO] Packet %d dequeued. Size: %d/%d\n", item.id, q->size, MAX);
    return item;
}

Packet queuePeek(Queue *q) {
    Packet empty = {0, TELEMETRY, "", 0};
    if (queueIsEmpty(q)) {
        printf("\n[FIFO] Queue is empty.\n");
        return empty;
    }
    return q->items[q->front];
}

void queueDisplay(Queue *q) {
    if (queueIsEmpty(q)) {
        printf("\n[FIFO] Queue is empty.\n");
        return;
    }
    printf("\n========================================\n");
    printf("       FIFO QUEUE  (%d/%d packets)\n", q->size, MAX);
    printf("========================================\n");
    printf("%-4s %-8s %-25s %-6s\n", "Pos", "ID", "Type", "Prio");
    printf("----------------------------------------\n");
    for (int i = 0; i < q->size; i++) {
        int idx = (q->front + i) % MAX;
        printf("%-4d %-8d %-25s %-6d\n",
               i + 1, q->items[idx].id,
               getPacketTypeName(q->items[idx].type),
               q->items[idx].priority);
    }
    printf("========================================\n");
    printf("Front -> [Next to transmit]\n");
    printf("Rear  -> [Most recently added]\n");
}

void initPQ(PriorityQueue *pq)     { pq->size = 0; }
int  pqIsEmpty(PriorityQueue *pq)  { return pq->size == 0; }
int  pqIsFull(PriorityQueue *pq)   { return pq->size == MAX; }

void pqEnqueue(PriorityQueue *pq, Packet p) {
    if (pqIsFull(pq)) {
        printf("\n[PQ] Priority Queue Overflow! Cannot enqueue packet %d.\n", p.id);
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
        printf("\n[PQ] Priority Queue Underflow! Nothing to dequeue.\n");
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
        printf("\n[PQ] Priority Queue is empty.\n");
        return empty;
    }
    return pq->items[0];
}

void pqDisplay(PriorityQueue *pq) {
    if (pqIsEmpty(pq)) {
        printf("\n[PQ] Priority Queue is empty.\n");
        return;
    }
    printf("\n========================================\n");
    printf("    PRIORITY QUEUE  (%d/%d packets)\n", pq->size, MAX);
    printf("========================================\n");
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

void initCB(CircularBuffer *cb)    { cb->front = 0; cb->rear = -1; cb->size = 0; cb->overwritten = 0; }
int  cbIsEmpty(CircularBuffer *cb) { return cb->size == 0; }

void cbEnqueue(CircularBuffer *cb, Packet p) {
    if (cb->size == MAX) {
        printf("[CB] Buffer FULL! Overwriting oldest packet %d (%s).\n",
               cb->items[cb->front].id,
               getPacketTypeName(cb->items[cb->front].type));
        cb->items[cb->front] = p;
        cb->front = (cb->front + 1) % MAX;
        cb->rear = (cb->rear + 1) % MAX;
        cb->overwritten++;
    } else {
        cb->rear = (cb->rear + 1) % MAX;
        cb->items[cb->rear] = p;
        cb->size++;
    }
    printf("[CB] Packet %d enqueued. Size: %d/%d  (overwritten so far: %d)\n",
           p.id, cb->size, MAX, cb->overwritten);
}

Packet cbDequeue(CircularBuffer *cb) {
    Packet empty = {0, TELEMETRY, "", 0};
    if (cbIsEmpty(cb)) {
        printf("\n[CB] Circular Buffer Underflow! Nothing to dequeue.\n");
        return empty;
    }
    Packet item = cb->items[cb->front];
    cb->front = (cb->front + 1) % MAX;
    cb->size--;
    printf("[CB] Packet %d dequeued. Size: %d/%d\n", item.id, cb->size, MAX);
    return item;
}

Packet cbPeek(CircularBuffer *cb) {
    Packet empty = {0, TELEMETRY, "", 0};
    if (cbIsEmpty(cb)) {
        printf("\n[CB] Circular Buffer is empty.\n");
        return empty;
    }
    return cb->items[cb->front];
}

void cbDisplay(CircularBuffer *cb) {
    if (cbIsEmpty(cb)) {
        printf("\n[CB] Circular Buffer is empty.\n");
        return;
    }
    printf("\n========================================\n");
    printf("  CIRCULAR BUFFER  (%d/%d packets)\n", cb->size, MAX);
    printf("  Overwritten so far: %d\n", cb->overwritten);
    printf("========================================\n");
    printf("%-4s %-8s %-25s %-6s\n", "Pos", "ID", "Type", "Prio");
    printf("----------------------------------------\n");
    for (int i = 0; i < cb->size; i++) {
        int idx = (cb->front + i) % MAX;
        printf("%-4d %-8d %-25s %-6d\n",
               i + 1, cb->items[idx].id,
               getPacketTypeName(cb->items[idx].type),
               cb->items[idx].priority);
    }
    printf("========================================\n");
    printf("Front -> [Oldest surviving packet]\n");
    printf("Rear  -> [Newest packet]\n");
}

void initDeque(Deque *dq) { dq->front = 0; dq->rear = -1; dq->size = 0; }
int  dequeIsEmpty(Deque *dq) { return dq->size == 0; }
int  dequeIsFull(Deque *dq)  { return dq->size == MAX; }

void dequeInsertFront(Deque *dq, Packet p) {
    if (dequeIsFull(dq)) {
        printf("\n[Deque] Overflow! Cannot insert front packet %d.\n", p.id);
        return;
    }
    dq->front = (dq->front - 1 + MAX) % MAX;
    dq->items[dq->front] = p;
    dq->size++;
    printf("[Deque] Packet %d inserted at FRONT (priority %d). Size: %d/%d\n",
           p.id, p.priority, dq->size, MAX);
}

void dequeInsertRear(Deque *dq, Packet p) {
    if (dequeIsFull(dq)) {
        printf("\n[Deque] Overflow! Cannot insert rear packet %d.\n", p.id);
        return;
    }
    dq->rear = (dq->rear + 1) % MAX;
    dq->items[dq->rear] = p;
    dq->size++;
    printf("[Deque] Packet %d inserted at REAR (priority %d). Size: %d/%d\n",
           p.id, p.priority, dq->size, MAX);
}

Packet dequeDeleteFront(Deque *dq) {
    Packet empty = {0, TELEMETRY, "", 0};
    if (dequeIsEmpty(dq)) {
        printf("\n[Deque] Underflow! Nothing to delete from front.\n");
        return empty;
    }
    Packet item = dq->items[dq->front];
    dq->front = (dq->front + 1) % MAX;
    dq->size--;
    printf("[Deque] Packet %d deleted from FRONT. Size: %d/%d\n", item.id, dq->size, MAX);
    return item;
}

Packet dequeDeleteRear(Deque *dq) {
    Packet empty = {0, TELEMETRY, "", 0};
    if (dequeIsEmpty(dq)) {
        printf("\n[Deque] Underflow! Nothing to delete from rear.\n");
        return empty;
    }
    Packet item = dq->items[dq->rear];
    dq->rear = (dq->rear - 1 + MAX) % MAX;
    dq->size--;
    printf("[Deque] Packet %d deleted from REAR. Size: %d/%d\n", item.id, dq->size, MAX);
    return item;
}

Packet dequePeekFront(Deque *dq) {
    Packet empty = {0, TELEMETRY, "", 0};
    if (dequeIsEmpty(dq)) {
        printf("\n[Deque] Deque is empty.\n");
        return empty;
    }
    return dq->items[dq->front];
}

Packet dequePeekRear(Deque *dq) {
    Packet empty = {0, TELEMETRY, "", 0};
    if (dequeIsEmpty(dq)) {
        printf("\n[Deque] Deque is empty.\n");
        return empty;
    }
    return dq->items[dq->rear];
}

void dequeDisplay(Deque *dq) {
    if (dequeIsEmpty(dq)) {
        printf("\n[Deque] Deque is empty.\n");
        return;
    }
    printf("\n========================================\n");
    printf("       DEQUE  (%d/%d packets)\n", dq->size, MAX);
    printf("========================================\n");
    printf("%-4s %-8s %-25s %-6s\n", "Pos", "ID", "Type", "Prio");
    printf("----------------------------------------\n");
    for (int i = 0; i < dq->size; i++) {
        int idx = (dq->front + i) % MAX;
        printf("%-4d %-8d %-25s %-6d\n",
               i + 1, dq->items[idx].id,
               getPacketTypeName(dq->items[idx].type),
               dq->items[idx].priority);
    }
    printf("========================================\n");
    printf("Front -> [Emergency insert/remove]\n");
    printf("Rear  -> [Normal insert/remove]\n");
}

void batchSimulation(Queue *fifoQ, PriorityQueue *pq, CircularBuffer *cb, Deque *dq, int mode) {
    int count;
    printf("\nEnter number of packets to generate (1-%d): ", MAX);
    scanf("%d", &count);
    if (count < 1 || count > MAX) {
        printf("Invalid count. Using 10.\n");
        count = 10;
    }

    printf("\n--- Batch Simulation: Generating %d random packets ---\n\n", count);
    printf("%-4s %-8s %-25s %-6s\n", "#", "ID", "Type", "Prio");
    printf("----------------------------------------------\n");

    for (int i = 0; i < count; i++) {
        Packet p = createPacketRandom();
        printf("%-4d %-8d %-25s %-6d\n",
               i + 1, p.id, getPacketTypeName(p.type), p.priority);

        if (mode == 1)      queueEnqueue(fifoQ, p);
        else if (mode == 2) pqEnqueue(pq, p);
        else if (mode == 3) cbEnqueue(cb, p);
        else if (mode == 4) dequeInsertRear(dq, p);
    }

    printf("\n--- Batch complete. %d packets generated. ---\n", count);
    printf("Use Display (option 5) to view the queue.\n");
}

int selectMode(void) {
    int mode;
    printf("\n========================================\n");
    printf("   SELECT QUEUE IMPLEMENTATION\n");
    printf("========================================\n");
    printf("1. FIFO Queue (Standard first-in-first-out)\n");
    printf("2. Priority Queue (Highest priority transmitted first)\n");
    printf("3. Circular Buffer (Overwrites oldest when full)\n");
    printf("4. Deque - Double Ended Queue (Emergency insert/remove)\n");
    printf("========================================\n");
    printf("Enter choice (1-4): ");
    scanf("%d", &mode);
    if (mode < 1 || mode > 4) {
        printf("Invalid choice. Defaulting to FIFO Queue.\n");
        mode = 1;
    }
    return mode;
}

int main() {
    srand((unsigned)time(NULL));

    Queue          fifoQ;
    PriorityQueue  pq;
    CircularBuffer cb;
    Deque          dq;

    initQueue(&fifoQ);
    initPQ(&pq);
    initCB(&cb);
    initDeque(&dq);

    int mode = selectMode();
    const char *modeNames[] = { "", "FIFO Queue", "Priority Queue", "Circular Buffer", "Deque" };

    int choice;

    printf("\n========================================================\n");
    printf("   SomaiyaSat PocketQube Communication Queue\n");
    printf("   Satellite Data Buffer Management System\n");
    printf("   Mode: %s\n", modeNames[mode]);
    printf("========================================================\n");

    do {
        printf("\n========= SATELLITE QUEUE MENU =========\n");
        printf("1. Create Node (Generate New Packet)\n");
        if (mode == 4) {
            printf("2. Insert Emergency Packet at FRONT (Deque only)\n");
            printf("3. Insert Normal Packet at REAR (Deque only)\n");
        } else {
            printf("2. Enqueue Packet\n");
        }
        printf("4. Dequeue Packet\n");
        printf("5. Peek Front Packet\n");
        printf("6. Display Queue\n");
        printf("7. Check Queue Status\n");
        printf("8. Batch Simulation (Auto-generate packets)\n");
        printf("9. Switch Queue Mode\n");
        printf("10. Exit\n");
        printf("==========================================\n");
        printf("Current mode: %s\n", modeNames[mode]);
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
        
        case 1: {
            Packet p = createPacketManual();
            if (mode == 1)      queueEnqueue(&fifoQ, p);
            else if (mode == 2) pqEnqueue(&pq, p);
            else if (mode == 3) cbEnqueue(&cb, p);
            else if (mode == 4) dequeInsertRear(&dq, p);
            printf("\n--- Node Created & Enqueued ---\n");
            printPacket(&p, "");
            break;
        }

        
        case 2: {
            if (mode == 4) {
                Packet p = createPacketManual();
                dequeInsertFront(&dq, p);
                printf("\n--- Emergency Packet Inserted at FRONT ---\n");
                printPacket(&p, "");
            } else {
                printf("\nUse option 1 (Create Node) to add packets.\n");
            }
            break;
        }

        
        case 3: {
            if (mode == 4) {
                Packet p = createPacketManual();
                dequeInsertRear(&dq, p);
                printf("\n--- Normal Packet Inserted at REAR ---\n");
                printPacket(&p, "");
            } else {
                printf("\nUse option 1 (Create Node) to add packets.\n");
            }
            break;
        }

        
        case 4: {
            Packet p;
            if (mode == 1)      p = queueDequeue(&fifoQ);
            else if (mode == 2) p = pqDequeue(&pq);
            else if (mode == 3) p = cbDequeue(&cb);
            else                p = dequeDeleteFront(&dq);
            if (p.id != 0) {
                printf("\n--- Packet Transmitted ---\n");
                printPacket(&p, "");
            }
            break;
        }

        
        case 5: {
            Packet p;
            if (mode == 1)      p = queuePeek(&fifoQ);
            else if (mode == 2) p = pqPeek(&pq);
            else if (mode == 3) p = cbPeek(&cb);
            else                p = dequePeekFront(&dq);
            if (p.id != 0) {
                printf("\n--- Front Packet (Next to transmit) ---\n");
                printPacket(&p, "");
            }
            break;
        }

        
        case 6:
            if (mode == 1)      queueDisplay(&fifoQ);
            else if (mode == 2) pqDisplay(&pq);
            else if (mode == 3) cbDisplay(&cb);
            else                dequeDisplay(&dq);
            break;

        
        case 7: {
            int sz = 0, mx = MAX;
            int ow = 0;
            if (mode == 1)      { sz = fifoQ.size; }
            else if (mode == 2) { sz = pq.size; }
            else if (mode == 3) { sz = cb.size; ow = cb.overwritten; }
            else                { sz = dq.size; }
            printf("\n--- Queue Status ---\n");
            printf("Mode         : %s\n", modeNames[mode]);
            printf("Queue Size   : %d / %d\n", sz, mx);
            printf("Status       : %s\n",
                   sz == 0 ? "Empty" : (sz == mx ? "Full" : "Has packets"));
            if (mode == 3)
                printf("Overwritten  : %d packets lost\n", ow);
            break;
        }

        
        case 8:
            batchSimulation(&fifoQ, &pq, &cb, &dq, mode);
            break;

        
        case 9:
            mode = selectMode();
            printf("Switched to: %s\n", modeNames[mode]);
            break;

        
        case 10:
            printf("\nShutting down satellite communication queue...\n");
            printf("Goodbye!\n");
            break;

        default:
            printf("\nInvalid choice! Please enter 1-10.\n");
        }
    } while (choice != 10);

    return 0;
}
