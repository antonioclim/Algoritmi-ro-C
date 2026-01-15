/**
 * =============================================================================
 * SOLUȚIE EXERCIȚIUL 1: PARCURGERE BFS ȘI GĂSIREA DRUMULUI CEL MAI SCURT
 * =============================================================================
 *
 * Aceasta este soluția completă pentru Exercițiul 1.
 * NU distribuiți studenților înainte de termenul de predare!
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -o exercise1_sol exercise1_sol.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* =============================================================================
 * CONSTANTE
 * =============================================================================
 */

#define MAX_VERTICES 100
#define INFINITY 999999

/* =============================================================================
 * STRUCTURI DE DATE
 * =============================================================================
 */

/**
 * Structură pentru coadă (necesară pentru BFS)
 */
typedef struct {
    int items[MAX_VERTICES];
    int front;
    int rear;
} Queue;

/**
 * Structură pentru graf cu matrice de adiacență
 */
typedef struct {
    int vertices;
    int adj[MAX_VERTICES][MAX_VERTICES];
} Graph;

/* =============================================================================
 * SOLUȚIE TODO 1: IMPLEMENTAREA COZII
 * =============================================================================
 */

/**
 * Inițializează coada (setează front și rear la -1)
 */
void queue_init(Queue* q) {
    /* SOLUȚIE TODO 1a */
    q->front = -1;
    q->rear = -1;
}

/**
 * Verifică dacă coada este goală
 */
bool queue_is_empty(Queue* q) {
    /* SOLUȚIE TODO 1b */
    return q->front == -1;
}

/**
 * Adaugă un element în coadă (enqueue)
 */
void queue_enqueue(Queue* q, int value) {
    /* SOLUȚIE TODO 1c */
    if (q->front == -1) {
        q->front = 0;
    }
    q->rear++;
    q->items[q->rear] = value;
}

/**
 * Extrage și returnează primul element din coadă (dequeue)
 */
int queue_dequeue(Queue* q) {
    /* SOLUȚIE TODO 1d */
    if (queue_is_empty(q)) {
        return -1;
    }
    
    int item = q->items[q->front];
    q->front++;
    
    if (q->front > q->rear) {
        /* Coada a devenit goală, resetăm */
        q->front = -1;
        q->rear = -1;
    }
    
    return item;
}

/* =============================================================================
 * SOLUȚIE TODO 2: INIȚIALIZAREA GRAFULUI
 * =============================================================================
 */

/**
 * Inițializează un graf cu n noduri
 */
void graph_init(Graph* g, int n) {
    /* SOLUȚIE TODO 2 */
    g->vertices = n;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            g->adj[i][j] = 0;
        }
    }
}

/**
 * Adaugă o muchie neorientată între nodurile u și v
 */
void graph_add_edge(Graph* g, int u, int v) {
    if (u >= 0 && u < g->vertices && v >= 0 && v < g->vertices) {
        g->adj[u][v] = 1;
        g->adj[v][u] = 1;
    }
}

/* =============================================================================
 * SOLUȚIE TODO 3: IMPLEMENTAREA BFS
 * =============================================================================
 */

/**
 * Execută BFS și calculează distanțele de la nodul sursă
 */
void bfs(Graph* g, int source, int* distance, int* parent) {
    /* SOLUȚIE TODO 3a: Array visited */
    bool visited[MAX_VERTICES];
    for (int i = 0; i < g->vertices; i++) {
        visited[i] = false;
        distance[i] = INFINITY;
        parent[i] = -1;
    }
    
    /* SOLUȚIE TODO 3b: Coadă */
    Queue q;
    queue_init(&q);
    
    /* SOLUȚIE TODO 3c: Nodul sursă */
    visited[source] = true;
    distance[source] = 0;
    
    /* SOLUȚIE TODO 3d: Adăugare în coadă */
    queue_enqueue(&q, source);
    
    /* SOLUȚIE TODO 3e: Bucla principală BFS */
    while (!queue_is_empty(&q)) {
        int current = queue_dequeue(&q);
        
        for (int i = 0; i < g->vertices; i++) {
            if (g->adj[current][i] && !visited[i]) {
                visited[i] = true;
                distance[i] = distance[current] + 1;
                parent[i] = current;
                queue_enqueue(&q, i);
            }
        }
    }
}

/* =============================================================================
 * SOLUȚIE TODO 4: RECONSTRUIREA DRUMULUI
 * =============================================================================
 */

/**
 * Reconstruiește drumul de la sursă la destinație
 */
int reconstruct_path(int* parent, int source, int dest, int* path) {
    /* SOLUȚIE TODO 4a: Verificare existență drum */
    if (dest != source && parent[dest] == -1) {
        return -1;  /* Nu există drum */
    }
    
    /* SOLUȚIE TODO 4b: Construire drum invers */
    int length = 0;
    int current = dest;
    
    while (current != -1) {
        path[length] = current;
        length++;
        if (current == source) break;
        current = parent[current];
    }
    
    /* SOLUȚIE TODO 4c: Inversare drum */
    for (int i = 0; i < length / 2; i++) {
        int temp = path[i];
        path[i] = path[length - 1 - i];
        path[length - 1 - i] = temp;
    }
    
    return length;
}

/* =============================================================================
 * SOLUȚIE TODO 5: AFIȘAREA DRUMULUI
 * =============================================================================
 */

/**
 * Afișează drumul frumos formatat
 */
void print_path(int* path, int length) {
    /* SOLUȚIE TODO 5 */
    if (length <= 0) {
        printf("Drum vid\n");
        return;
    }
    
    printf("Drumul: ");
    for (int i = 0; i < length; i++) {
        printf("%d", path[i]);
        if (i < length - 1) {
            printf(" -> ");
        }
    }
    printf("\n");
}

/* =============================================================================
 * FUNCȚIA PRINCIPALĂ
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     SOLUȚIE EXERCIȚIUL 1: BFS și Drumul Cel Mai Scurt         ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Creăm graful din exemplu:
     *
     *     0 --- 1
     *     |     |
     *     2 --- 3 --- 5
     *     |           |
     *     4 ----------+
     */
    
    Graph g;
    graph_init(&g, 6);
    
    graph_add_edge(&g, 0, 1);
    graph_add_edge(&g, 0, 2);
    graph_add_edge(&g, 1, 3);
    graph_add_edge(&g, 2, 3);
    graph_add_edge(&g, 2, 4);
    graph_add_edge(&g, 3, 5);
    graph_add_edge(&g, 4, 5);
    
    printf("Graf creat cu 6 noduri și 7 muchii\n\n");
    
    /* Afișăm matricea de adiacență */
    printf("Matricea de adiacență:\n    ");
    for (int i = 0; i < g.vertices; i++) printf("%3d ", i);
    printf("\n    ");
    for (int i = 0; i < g.vertices; i++) printf("----");
    printf("\n");
    
    for (int i = 0; i < g.vertices; i++) {
        printf("%2d| ", i);
        for (int j = 0; j < g.vertices; j++) {
            printf("%3d ", g.adj[i][j]);
        }
        printf("\n");
    }
    
    /* Executăm BFS */
    int distance[MAX_VERTICES];
    int parent[MAX_VERTICES];
    int source = 0;
    int dest = 5;
    
    printf("\n--- Executare BFS de la nodul %d ---\n", source);
    bfs(&g, source, distance, parent);
    
    /* Afișăm distanțele */
    printf("\nDistanțe de la nodul %d:\n", source);
    for (int i = 0; i < g.vertices; i++) {
        if (distance[i] == INFINITY) {
            printf("  Nod %d: INFINIT (inaccesibil)\n", i);
        } else {
            printf("  Nod %d: %d\n", i, distance[i]);
        }
    }
    
    /* Afișăm array-ul de părinți */
    printf("\nPărinți (pentru reconstrucția drumului):\n");
    for (int i = 0; i < g.vertices; i++) {
        printf("  parent[%d] = %d\n", i, parent[i]);
    }
    
    /* Reconstruim și afișăm drumul */
    int path[MAX_VERTICES];
    int path_length = reconstruct_path(parent, source, dest, path);
    
    printf("\n--- Drumul cel mai scurt de la %d la %d ---\n", source, dest);
    if (path_length <= 0 && source != dest) {
        printf("Nu există drum între %d și %d\n", source, dest);
    } else {
        print_path(path, path_length);
        printf("Lungime drum: %d muchii\n", path_length > 0 ? path_length - 1 : 0);
    }
    
    /* Test suplimentar: drum între 1 și 4 */
    printf("\n--- Test suplimentar: drum de la 1 la 4 ---\n");
    bfs(&g, 1, distance, parent);
    path_length = reconstruct_path(parent, 1, 4, path);
    print_path(path, path_length);
    printf("Lungime drum: %d muchii\n", path_length > 0 ? path_length - 1 : 0);
    
    printf("\n✓ Toate testele executate cu succes!\n\n");
    
    return 0;
}
