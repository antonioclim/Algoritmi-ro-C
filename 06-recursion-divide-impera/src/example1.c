/**
 * =============================================================================
 * SĂPTĂMÂNA 6: COZI (QUEUES)
 * Exemplu Complet Demonstrativ
 * =============================================================================
 *
 * Acest exemplu demonstrează:
 *   1. Implementarea unui circular buffer (array-based queue)
 *   2. Implementarea unei queue cu linked list
 *   3. Operațiile fundamentale: enqueue, dequeue, peek, isEmpty, isFull
 *   4. Double-ended queue (deque) simplificată
 *   5. Priority queue cu nivele multiple
 *   6. Aplicație practică: BFS pe un graf
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -o example1 example1.c
 * Utilizare: ./example1
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

/* =============================================================================
 * CONSTANTE ȘI MACRO-URI
 * =============================================================================
 */

#define QUEUE_CAPACITY 10
#define MAX_NODES 20
#define NUM_PRIORITIES 4

/* Culori pentru output în terminal */
#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_RED     "\033[31m"
#define COLOR_MAGENTA "\033[35m"

/* =============================================================================
 * PART 1: CIRCULAR BUFFER QUEUE (ARRAY-BASED)
 * =============================================================================
 */

/**
 * Structura pentru o queue implementată cu circular buffer
 * 
 * Avantaje:
 * - Toate operațiile O(1)
 * - Memorie contiguă (cache-friendly)
 * - Fără alocare dinamică per operație
 *
 * Dezavantaje:
 * - Capacitate fixă
 * - Trebuie știută dimensiunea maximă la compilare/inițializare
 */
typedef struct {
    int data[QUEUE_CAPACITY];   /* Array-ul de stocare */
    int front;                   /* Index primul element */
    int rear;                    /* Index următoarea poziție liberă */
    int count;                   /* Număr de elemente curente */
} CircularQueue;

/**
 * Inițializează o queue circulară
 * @param q Pointer la queue
 */
void cqueue_init(CircularQueue *q) {
    q->front = 0;
    q->rear = 0;
    q->count = 0;
    /* Inițializăm array-ul cu 0 pentru debugging mai ușor */
    memset(q->data, 0, sizeof(q->data));
}

/**
 * Verifică dacă queue-ul circular este gol
 * @param q Pointer la queue
 * @return true dacă este gol, false altfel
 */
bool cqueue_is_empty(const CircularQueue *q) {
    return q->count == 0;
}

/**
 * Verifică dacă queue-ul circular este plin
 * @param q Pointer la queue
 * @return true dacă este plin, false altfel
 */
bool cqueue_is_full(const CircularQueue *q) {
    return q->count >= QUEUE_CAPACITY;
}

/**
 * Returnează numărul de elemente din queue
 * @param q Pointer la queue
 * @return Numărul de elemente
 */
int cqueue_size(const CircularQueue *q) {
    return q->count;
}

/**
 * Adaugă un element la sfârșitul queue-ului (enqueue)
 * @param q Pointer la queue
 * @param item Elementul de adăugat
 * @return true dacă operația a reușit, false dacă queue-ul este plin
 */
bool cqueue_enqueue(CircularQueue *q, int item) {
    if (cqueue_is_full(q)) {
        fprintf(stderr, COLOR_RED "Eroare: Queue overflow! Capacitate maximă: %d\n" COLOR_RESET, 
                QUEUE_CAPACITY);
        return false;
    }
    
    /* Adăugăm elementul la poziția rear */
    q->data[q->rear] = item;
    
    /* Avansăm rear circular folosind operatorul modulo */
    q->rear = (q->rear + 1) % QUEUE_CAPACITY;
    
    /* Incrementăm contorul */
    q->count++;
    
    return true;
}

/**
 * Elimină și returnează elementul din fața queue-ului (dequeue)
 * @param q Pointer la queue
 * @param result Pointer pentru stocarea rezultatului
 * @return true dacă operația a reușit, false dacă queue-ul este gol
 */
bool cqueue_dequeue(CircularQueue *q, int *result) {
    if (cqueue_is_empty(q)) {
        fprintf(stderr, COLOR_RED "Eroare: Queue underflow! Queue-ul este gol.\n" COLOR_RESET);
        return false;
    }
    
    /* Salvăm elementul din front */
    *result = q->data[q->front];
    
    /* Avansăm front circular */
    q->front = (q->front + 1) % QUEUE_CAPACITY;
    
    /* Decrementăm contorul */
    q->count--;
    
    return true;
}

/**
 * Returnează elementul din față fără a-l elimina (peek)
 * @param q Pointer la queue
 * @param result Pointer pentru stocarea rezultatului
 * @return true dacă operația a reușit, false dacă queue-ul este gol
 */
bool cqueue_peek(const CircularQueue *q, int *result) {
    if (cqueue_is_empty(q)) {
        return false;
    }
    *result = q->data[q->front];
    return true;
}

/**
 * Afișează conținutul queue-ului circular (pentru debugging)
 * @param q Pointer la queue
 */
void cqueue_print(const CircularQueue *q) {
    printf("Queue [size=%d, front=%d, rear=%d]: ", q->count, q->front, q->rear);
    
    if (cqueue_is_empty(q)) {
        printf("(goală)\n");
        return;
    }
    
    printf("{ ");
    int idx = q->front;
    for (int i = 0; i < q->count; i++) {
        printf("%d", q->data[idx]);
        if (i < q->count - 1) printf(" → ");
        idx = (idx + 1) % QUEUE_CAPACITY;
    }
    printf(" }\n");
}

/**
 * Demonstrează circular buffer queue
 */
void demo_circular_queue(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║      " COLOR_CYAN "PART 1: CIRCULAR BUFFER QUEUE" COLOR_RESET "                              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n\n");
    
    CircularQueue q;
    cqueue_init(&q);
    
    printf(COLOR_GREEN "✓ Queue inițializată\n" COLOR_RESET);
    cqueue_print(&q);
    
    /* Adăugăm elemente */
    printf("\n" COLOR_YELLOW "→ Adăugăm elementele 10, 20, 30, 40, 50:\n" COLOR_RESET);
    int values[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++) {
        cqueue_enqueue(&q, values[i]);
        printf("  enqueue(%d): ", values[i]);
        cqueue_print(&q);
    }
    
    /* Extragem câteva elemente */
    printf("\n" COLOR_YELLOW "→ Extragem 2 elemente:\n" COLOR_RESET);
    int result;
    for (int i = 0; i < 2; i++) {
        cqueue_dequeue(&q, &result);
        printf("  dequeue() → %d: ", result);
        cqueue_print(&q);
    }
    
    /* Demonstrăm wrap-around */
    printf("\n" COLOR_YELLOW "→ Demonstrăm wrap-around (adăugăm 60, 70, 80, 90):\n" COLOR_RESET);
    int more[] = {60, 70, 80, 90};
    for (int i = 0; i < 4; i++) {
        cqueue_enqueue(&q, more[i]);
        printf("  enqueue(%d): ", more[i]);
        cqueue_print(&q);
    }
    
    /* Peek */
    int front_val;
    if (cqueue_peek(&q, &front_val)) {
        printf("\n" COLOR_GREEN "✓ peek() → %d (elementul din față, fără eliminare)\n" COLOR_RESET, front_val);
    }
    
    printf("\n" COLOR_GREEN "✓ Demonstrație completă!\n" COLOR_RESET);
}

/* =============================================================================
 * PART 2: LINKED LIST QUEUE
 * =============================================================================
 */

/**
 * Structura pentru un nod în linked list queue
 */
typedef struct LLNode {
    int data;
    struct LLNode *next;
} LLNode;

/**
 * Structura pentru o queue implementată cu linked list
 *
 * Avantaje:
 * - Dimensiune dinamică
 * - Nu risipește memorie pentru elemente neutilizate
 *
 * Dezavantaje:
 * - Overhead de memorie pentru pointeri
 * - Alocare dinamică per operație
 * - Cache locality slabă
 */
typedef struct {
    LLNode *front;   /* Pointer la primul nod */
    LLNode *rear;    /* Pointer la ultimul nod */
    int count;       /* Număr de elemente */
} LinkedQueue;

/**
 * Inițializează o linked list queue
 */
void lqueue_init(LinkedQueue *q) {
    q->front = NULL;
    q->rear = NULL;
    q->count = 0;
}

/**
 * Eliberează toată memoria unei linked list queue
 */
void lqueue_destroy(LinkedQueue *q) {
    LLNode *current = q->front;
    while (current != NULL) {
        LLNode *temp = current;
        current = current->next;
        free(temp);
    }
    q->front = NULL;
    q->rear = NULL;
    q->count = 0;
}

/**
 * Verifică dacă linked queue este goală
 */
bool lqueue_is_empty(const LinkedQueue *q) {
    return q->front == NULL;
}

/**
 * Returnează dimensiunea linked queue
 */
int lqueue_size(const LinkedQueue *q) {
    return q->count;
}

/**
 * Adaugă un element la linked queue
 */
bool lqueue_enqueue(LinkedQueue *q, int item) {
    /* Alocăm un nou nod */
    LLNode *newNode = (LLNode *)malloc(sizeof(LLNode));
    if (newNode == NULL) {
        fprintf(stderr, COLOR_RED "Eroare: Alocare memorie eșuată!\n" COLOR_RESET);
        return false;
    }
    
    newNode->data = item;
    newNode->next = NULL;
    
    if (q->rear == NULL) {
        /* Queue goală - noul nod devine și front și rear */
        q->front = newNode;
        q->rear = newNode;
    } else {
        /* Adăugăm la sfârșitul listei */
        q->rear->next = newNode;
        q->rear = newNode;
    }
    
    q->count++;
    return true;
}

/**
 * Elimină și returnează elementul din față
 */
bool lqueue_dequeue(LinkedQueue *q, int *result) {
    if (lqueue_is_empty(q)) {
        fprintf(stderr, COLOR_RED "Eroare: Queue goală!\n" COLOR_RESET);
        return false;
    }
    
    LLNode *temp = q->front;
    *result = temp->data;
    
    q->front = q->front->next;
    
    /* Dacă queue devine goală, actualizăm și rear */
    if (q->front == NULL) {
        q->rear = NULL;
    }
    
    free(temp);
    q->count--;
    
    return true;
}

/**
 * Afișează linked queue
 */
void lqueue_print(const LinkedQueue *q) {
    printf("LinkedQueue [size=%d]: ", q->count);
    
    if (lqueue_is_empty(q)) {
        printf("(goală)\n");
        return;
    }
    
    printf("front → ");
    LLNode *current = q->front;
    while (current != NULL) {
        printf("[%d]", current->data);
        if (current->next != NULL) printf(" → ");
        current = current->next;
    }
    printf(" ← rear\n");
}

/**
 * Demonstrează linked list queue
 */
void demo_linked_queue(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║      " COLOR_CYAN "PART 2: LINKED LIST QUEUE" COLOR_RESET "                                  ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n\n");
    
    LinkedQueue q;
    lqueue_init(&q);
    
    printf(COLOR_GREEN "✓ LinkedQueue inițializată\n" COLOR_RESET);
    lqueue_print(&q);
    
    /* Adăugăm elemente */
    printf("\n" COLOR_YELLOW "→ Adăugăm elementele A(65), B(66), C(67), D(68):\n" COLOR_RESET);
    for (int c = 'A'; c <= 'D'; c++) {
        lqueue_enqueue(&q, c);
        printf("  enqueue('%c'=%d): ", c, c);
        lqueue_print(&q);
    }
    
    /* Extragem elemente */
    printf("\n" COLOR_YELLOW "→ Extragem toate elementele:\n" COLOR_RESET);
    int result;
    while (!lqueue_is_empty(&q)) {
        lqueue_dequeue(&q, &result);
        printf("  dequeue() → '%c'=%d: ", result, result);
        lqueue_print(&q);
    }
    
    /* Curățăm memoria */
    lqueue_destroy(&q);
    printf("\n" COLOR_GREEN "✓ Memoria eliberată corect!\n" COLOR_RESET);
}

/* =============================================================================
 * PART 3: DOUBLE-ENDED QUEUE (DEQUE) SIMPLIFICATĂ
 * =============================================================================
 */

/**
 * Structura pentru un nod doubly-linked (pentru deque)
 */
typedef struct DNode {
    int data;
    struct DNode *prev;
    struct DNode *next;
} DNode;

/**
 * Structura pentru deque
 */
typedef struct {
    DNode *front;
    DNode *rear;
    int count;
} Deque;

/**
 * Inițializează un deque
 */
void deque_init(Deque *dq) {
    dq->front = NULL;
    dq->rear = NULL;
    dq->count = 0;
}

/**
 * Eliberează memoria deque-ului
 */
void deque_destroy(Deque *dq) {
    DNode *current = dq->front;
    while (current != NULL) {
        DNode *temp = current;
        current = current->next;
        free(temp);
    }
    dq->front = NULL;
    dq->rear = NULL;
    dq->count = 0;
}

/**
 * Adaugă element la front
 */
bool deque_push_front(Deque *dq, int item) {
    DNode *newNode = (DNode *)malloc(sizeof(DNode));
    if (newNode == NULL) return false;
    
    newNode->data = item;
    newNode->prev = NULL;
    newNode->next = dq->front;
    
    if (dq->front == NULL) {
        dq->rear = newNode;
    } else {
        dq->front->prev = newNode;
    }
    dq->front = newNode;
    dq->count++;
    
    return true;
}

/**
 * Adaugă element la rear
 */
bool deque_push_rear(Deque *dq, int item) {
    DNode *newNode = (DNode *)malloc(sizeof(DNode));
    if (newNode == NULL) return false;
    
    newNode->data = item;
    newNode->next = NULL;
    newNode->prev = dq->rear;
    
    if (dq->rear == NULL) {
        dq->front = newNode;
    } else {
        dq->rear->next = newNode;
    }
    dq->rear = newNode;
    dq->count++;
    
    return true;
}

/**
 * Elimină element de la front
 */
bool deque_pop_front(Deque *dq, int *result) {
    if (dq->front == NULL) return false;
    
    DNode *temp = dq->front;
    *result = temp->data;
    
    dq->front = dq->front->next;
    if (dq->front == NULL) {
        dq->rear = NULL;
    } else {
        dq->front->prev = NULL;
    }
    
    free(temp);
    dq->count--;
    return true;
}

/**
 * Elimină element de la rear
 */
bool deque_pop_rear(Deque *dq, int *result) {
    if (dq->rear == NULL) return false;
    
    DNode *temp = dq->rear;
    *result = temp->data;
    
    dq->rear = dq->rear->prev;
    if (dq->rear == NULL) {
        dq->front = NULL;
    } else {
        dq->rear->next = NULL;
    }
    
    free(temp);
    dq->count--;
    return true;
}

/**
 * Afișează deque-ul
 */
void deque_print(const Deque *dq) {
    printf("Deque [size=%d]: ", dq->count);
    if (dq->front == NULL) {
        printf("(goală)\n");
        return;
    }
    
    printf("front ↔ ");
    DNode *current = dq->front;
    while (current != NULL) {
        printf("[%d]", current->data);
        if (current->next != NULL) printf(" ↔ ");
        current = current->next;
    }
    printf(" ↔ rear\n");
}

/**
 * Demonstrează deque
 */
void demo_deque(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║      " COLOR_CYAN "PART 3: DOUBLE-ENDED QUEUE (DEQUE)" COLOR_RESET "                         ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n\n");
    
    Deque dq;
    deque_init(&dq);
    
    printf(COLOR_GREEN "✓ Deque inițializat\n" COLOR_RESET);
    
    printf("\n" COLOR_YELLOW "→ push_rear: 10, 20, 30\n" COLOR_RESET);
    deque_push_rear(&dq, 10);
    deque_push_rear(&dq, 20);
    deque_push_rear(&dq, 30);
    deque_print(&dq);
    
    printf("\n" COLOR_YELLOW "→ push_front: 5, 1\n" COLOR_RESET);
    deque_push_front(&dq, 5);
    deque_push_front(&dq, 1);
    deque_print(&dq);
    
    int result;
    printf("\n" COLOR_YELLOW "→ pop_front():\n" COLOR_RESET);
    deque_pop_front(&dq, &result);
    printf("  Eliminat de la front: %d\n", result);
    deque_print(&dq);
    
    printf("\n" COLOR_YELLOW "→ pop_rear():\n" COLOR_RESET);
    deque_pop_rear(&dq, &result);
    printf("  Eliminat de la rear: %d\n", result);
    deque_print(&dq);
    
    deque_destroy(&dq);
    printf("\n" COLOR_GREEN "✓ Memoria eliberată!\n" COLOR_RESET);
}

/* =============================================================================
 * PART 4: PRIORITY QUEUE (ARRAY OF QUEUES)
 * =============================================================================
 */

/**
 * Nivele de prioritate
 */
typedef enum {
    PRIORITY_CRITICAL = 0,  /* Cea mai mare prioritate */
    PRIORITY_HIGH = 1,
    PRIORITY_NORMAL = 2,
    PRIORITY_LOW = 3        /* Cea mai mică prioritate */
} PriorityLevel;

/**
 * Structura pentru priority queue
 */
typedef struct {
    CircularQueue levels[NUM_PRIORITIES];
} PriorityQueue;

/**
 * Numele priorităților pentru afișare
 */
const char *priority_names[] = {"CRITICAL", "HIGH", "NORMAL", "LOW"};

/**
 * Inițializează priority queue
 */
void pqueue_init(PriorityQueue *pq) {
    for (int i = 0; i < NUM_PRIORITIES; i++) {
        cqueue_init(&pq->levels[i]);
    }
}

/**
 * Adaugă element cu prioritate
 */
bool pqueue_enqueue(PriorityQueue *pq, int item, PriorityLevel priority) {
    if (priority < 0 || priority >= NUM_PRIORITIES) {
        priority = PRIORITY_NORMAL;
    }
    return cqueue_enqueue(&pq->levels[priority], item);
}

/**
 * Extrage elementul cu cea mai mare prioritate
 */
bool pqueue_dequeue(PriorityQueue *pq, int *result) {
    /* Căutăm de la prioritatea cea mai mare */
    for (int i = 0; i < NUM_PRIORITIES; i++) {
        if (!cqueue_is_empty(&pq->levels[i])) {
            return cqueue_dequeue(&pq->levels[i], result);
        }
    }
    return false;  /* Toate queue-urile goale */
}

/**
 * Verifică dacă priority queue este goală
 */
bool pqueue_is_empty(const PriorityQueue *pq) {
    for (int i = 0; i < NUM_PRIORITIES; i++) {
        if (!cqueue_is_empty(&pq->levels[i])) {
            return false;
        }
    }
    return true;
}

/**
 * Afișează priority queue
 */
void pqueue_print(const PriorityQueue *pq) {
    printf("PriorityQueue:\n");
    for (int i = 0; i < NUM_PRIORITIES; i++) {
        printf("  [%s] ", priority_names[i]);
        cqueue_print(&pq->levels[i]);
    }
}

/**
 * Demonstrează priority queue
 */
void demo_priority_queue(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║      " COLOR_CYAN "PART 4: PRIORITY QUEUE" COLOR_RESET "                                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n\n");
    
    PriorityQueue pq;
    pqueue_init(&pq);
    
    printf(COLOR_GREEN "✓ Priority Queue inițializată\n\n" COLOR_RESET);
    
    /* Adăugăm task-uri cu diferite priorități */
    printf(COLOR_YELLOW "→ Adăugăm task-uri:\n" COLOR_RESET);
    printf("  Task 100 (NORMAL)\n");
    pqueue_enqueue(&pq, 100, PRIORITY_NORMAL);
    printf("  Task 200 (LOW)\n");
    pqueue_enqueue(&pq, 200, PRIORITY_LOW);
    printf("  Task 300 (CRITICAL)\n");
    pqueue_enqueue(&pq, 300, PRIORITY_CRITICAL);
    printf("  Task 400 (HIGH)\n");
    pqueue_enqueue(&pq, 400, PRIORITY_HIGH);
    printf("  Task 500 (NORMAL)\n");
    pqueue_enqueue(&pq, 500, PRIORITY_NORMAL);
    printf("  Task 600 (CRITICAL)\n");
    pqueue_enqueue(&pq, 600, PRIORITY_CRITICAL);
    
    printf("\n");
    pqueue_print(&pq);
    
    /* Procesăm în ordinea priorității */
    printf("\n" COLOR_YELLOW "→ Procesăm task-uri (în ordinea priorității):\n" COLOR_RESET);
    int task;
    int order = 1;
    while (!pqueue_is_empty(&pq)) {
        pqueue_dequeue(&pq, &task);
        printf("  %d. Task %d procesat\n", order++, task);
    }
    
    printf("\n" COLOR_GREEN "✓ Toate task-urile procesate!\n" COLOR_RESET);
}

/* =============================================================================
 * PART 5: BFS (BREADTH-FIRST SEARCH) PE GRAF
 * =============================================================================
 */

/**
 * Structura pentru un graf (reprezentare cu matrice de adiacență)
 */
typedef struct {
    int adj[MAX_NODES][MAX_NODES];  /* Matrice de adiacență */
    int num_nodes;                   /* Număr de noduri */
} Graph;

/**
 * Inițializează un graf
 */
void graph_init(Graph *g, int num_nodes) {
    g->num_nodes = num_nodes;
    for (int i = 0; i < num_nodes; i++) {
        for (int j = 0; j < num_nodes; j++) {
            g->adj[i][j] = 0;
        }
    }
}

/**
 * Adaugă o muchie (neorientată)
 */
void graph_add_edge(Graph *g, int u, int v) {
    if (u >= 0 && u < g->num_nodes && v >= 0 && v < g->num_nodes) {
        g->adj[u][v] = 1;
        g->adj[v][u] = 1;  /* Graf neorientat */
    }
}

/**
 * BFS - parcurgere în lățime
 */
void graph_bfs(const Graph *g, int start) {
    if (start < 0 || start >= g->num_nodes) {
        printf(COLOR_RED "Nod invalid!\n" COLOR_RESET);
        return;
    }
    
    bool visited[MAX_NODES] = {false};
    CircularQueue q;
    cqueue_init(&q);
    
    /* Pornim de la nodul de start */
    cqueue_enqueue(&q, start);
    visited[start] = true;
    
    printf("BFS traversal de la nodul %d: ", start);
    
    while (!cqueue_is_empty(&q)) {
        int current;
        cqueue_dequeue(&q, &current);
        printf("%d ", current);
        
        /* Adăugăm vecinii nevizitați în queue */
        for (int i = 0; i < g->num_nodes; i++) {
            if (g->adj[current][i] && !visited[i]) {
                visited[i] = true;
                cqueue_enqueue(&q, i);
            }
        }
    }
    printf("\n");
}

/**
 * Afișează graful
 */
void graph_print(const Graph *g) {
    printf("Graf (matrice de adiacență):\n");
    printf("    ");
    for (int i = 0; i < g->num_nodes; i++) {
        printf("%d ", i);
    }
    printf("\n");
    printf("   ");
    for (int i = 0; i < g->num_nodes; i++) {
        printf("--");
    }
    printf("\n");
    
    for (int i = 0; i < g->num_nodes; i++) {
        printf("%d | ", i);
        for (int j = 0; j < g->num_nodes; j++) {
            printf("%d ", g->adj[i][j]);
        }
        printf("\n");
    }
}

/**
 * Demonstrează BFS
 */
void demo_bfs(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║      " COLOR_CYAN "PART 5: BFS (BREADTH-FIRST SEARCH)" COLOR_RESET "                         ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n\n");
    
    /*
     * Creăm un graf simplu:
     *
     *        0
     *       /|\
     *      1 2 3
     *     /|   |
     *    4 5   6
     */
    
    Graph g;
    graph_init(&g, 7);
    
    /* Adăugăm muchii */
    graph_add_edge(&g, 0, 1);
    graph_add_edge(&g, 0, 2);
    graph_add_edge(&g, 0, 3);
    graph_add_edge(&g, 1, 4);
    graph_add_edge(&g, 1, 5);
    graph_add_edge(&g, 3, 6);
    
    printf(COLOR_GREEN "✓ Graf creat:\n" COLOR_RESET);
    printf("\n");
    printf("     0\n");
    printf("    /|\\\n");
    printf("   1 2 3\n");
    printf("  /|   |\n");
    printf(" 4 5   6\n\n");
    
    graph_print(&g);
    printf("\n");
    
    printf(COLOR_YELLOW "→ Parcurgeri BFS:\n" COLOR_RESET);
    printf("  ");
    graph_bfs(&g, 0);
    printf("  ");
    graph_bfs(&g, 1);
    printf("  ");
    graph_bfs(&g, 3);
    
    printf("\n" COLOR_GREEN "✓ BFS folosește Queue pentru a vizita nodurile nivel cu nivel!\n" COLOR_RESET);
}

/* =============================================================================
 * PART 6: COMPARAȚIE PERFORMANȚĂ
 * =============================================================================
 */

/**
 * Demonstrează diferențele de performanță
 */
void demo_performance(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║      " COLOR_CYAN "PART 6: COMPARAȚIE CONCEPTUALĂ" COLOR_RESET "                             ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n\n");
    
    printf("┌────────────────────┬─────────────────┬─────────────────┐\n");
    printf("│ " COLOR_CYAN "Criteriu" COLOR_RESET "           │ " COLOR_GREEN "Circular Buffer" COLOR_RESET " │ " COLOR_YELLOW "Linked List" COLOR_RESET "     │\n");
    printf("├────────────────────┼─────────────────┼─────────────────┤\n");
    printf("│ enqueue            │ O(1)            │ O(1)            │\n");
    printf("│ dequeue            │ O(1)            │ O(1)            │\n");
    printf("│ peek               │ O(1)            │ O(1)            │\n");
    printf("│ Memorie/element    │ sizeof(T)       │ sizeof(T)+ptr   │\n");
    printf("│ Capacitate         │ Fixă            │ Dinamică        │\n");
    printf("│ Cache locality     │ Excelentă       │ Slabă           │\n");
    printf("│ Alocare            │ O singură dată  │ Per operație    │\n");
    printf("│ Memory leaks       │ Imposibile      │ Posibile        │\n");
    printf("└────────────────────┴─────────────────┴─────────────────┘\n");
    
    printf("\n" COLOR_GREEN "Recomandări:\n" COLOR_RESET);
    printf("  • " COLOR_CYAN "Circular Buffer" COLOR_RESET ": sisteme embedded, buffere I/O, dimensiune cunoscută\n");
    printf("  • " COLOR_YELLOW "Linked List" COLOR_RESET ": dimensiune variabilă, message queues generale\n");
}

/* =============================================================================
 * MAIN PROGRAM
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║     " COLOR_MAGENTA "SĂPTĂMÂNA 6: COZI (QUEUES) - Exemplu Complet" COLOR_RESET "               ║\n");
    printf("║     Algoritmi și Tehnici de Programare                            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n");
    
    /* Rulăm toate demonstrațiile */
    demo_circular_queue();
    demo_linked_queue();
    demo_deque();
    demo_priority_queue();
    demo_bfs();
    demo_performance();
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║     " COLOR_GREEN "✓ Toate demonstrațiile complete!" COLOR_RESET "                            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}
