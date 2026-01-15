/**
 * =============================================================================
 * SOLUȚIE EXERCIȚIU 1: Algoritmul Dijkstra
 * =============================================================================
 * 
 * ATENȚIE: Acest fișier este DOAR pentru instructori!
 * Nu distribuiți studenților înainte de termenul limită.
 * 
 * Această soluție demonstrează:
 *   1. Implementarea completă a MinHeap cu decrease-key
 *   2. Algoritmul Dijkstra cu heap optimization
 *   3. Reconstrucția drumurilor
 *   4. Gestiunea corectă a memoriei
 * 
 * Compilare: gcc -Wall -Wextra -std=c11 -o ex1_sol exercise1_sol.c
 * Utilizare: ./ex1_sol < ../tests/test1_input.txt
 * 
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>

/* =============================================================================
 * CONSTANTE ȘI CULORI ANSI
 * =============================================================================
 */

#define INF INT_MAX
#define GREEN   "\033[0;32m"
#define CYAN    "\033[0;36m"
#define YELLOW  "\033[0;33m"
#define RED     "\033[0;31m"
#define NC      "\033[0m"

/* =============================================================================
 * STRUCTURI DE DATE
 * =============================================================================
 */

/**
 * TODO 1 (REZOLVAT): Structura HeapNode
 * 
 * Stochează un nod din heap cu vertex-ul și distanța asociată.
 */
typedef struct {
    int vertex;     /* Indicele nodului în graf */
    int distance;   /* Distanța curentă de la sursă */
} HeapNode;

/**
 * TODO 2 (REZOLVAT): Structura MinHeap
 * 
 * Heap binar minim cu suport pentru decrease-key.
 * Array-ul pos[] mapează vertex -> poziție în heap pentru O(1) lookup.
 */
typedef struct {
    HeapNode* nodes;    /* Array-ul de noduri heap */
    int* pos;           /* pos[v] = poziția vertex-ului v în heap (-1 dacă nu e) */
    int size;           /* Numărul curent de elemente */
    int capacity;       /* Capacitatea maximă */
} MinHeap;

/**
 * Structură pentru graf (listă de adiacență)
 */
typedef struct Edge {
    int dest;
    int weight;
    struct Edge* next;
} Edge;

typedef struct {
    int V;
    Edge** adj;
} Graph;

/* =============================================================================
 * FUNCȚII HEAP
 * =============================================================================
 */

/**
 * Funcție auxiliară: swap între două noduri heap
 */
static void swap_nodes(MinHeap* h, int i, int j) {
    /* Actualizăm pozițiile în array-ul pos */
    h->pos[h->nodes[i].vertex] = j;
    h->pos[h->nodes[j].vertex] = i;
    
    /* Swap nodurile */
    HeapNode temp = h->nodes[i];
    h->nodes[i] = h->nodes[j];
    h->nodes[j] = temp;
}

/**
 * TODO 3 (REZOLVAT): Crearea heap-ului
 * 
 * Alocă memorie pentru un heap cu capacitate dată.
 * Inițializează toate pozițiile cu -1 (niciun vertex în heap).
 */
MinHeap* heap_create(int capacity) {
    MinHeap* h = (MinHeap*)malloc(sizeof(MinHeap));
    if (!h) return NULL;
    
    h->capacity = capacity;
    h->size = 0;
    
    h->nodes = (HeapNode*)malloc(capacity * sizeof(HeapNode));
    h->pos = (int*)malloc(capacity * sizeof(int));
    
    if (!h->nodes || !h->pos) {
        free(h->nodes);
        free(h->pos);
        free(h);
        return NULL;
    }
    
    /* Inițializăm toate pozițiile cu -1 */
    for (int i = 0; i < capacity; i++) {
        h->pos[i] = -1;
    }
    
    return h;
}

/**
 * TODO 4 (REZOLVAT): Heapify-up (percolate up)
 * 
 * Restaurează proprietatea de heap după inserare.
 * Compară cu părintele și urcă dacă e necesar.
 */
static void heapify_up(MinHeap* h, int idx) {
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        
        /* Dacă nodul curent e mai mic decât părintele, facem swap */
        if (h->nodes[idx].distance < h->nodes[parent].distance) {
            swap_nodes(h, idx, parent);
            idx = parent;
        } else {
            break;
        }
    }
}

/**
 * TODO 5 (REZOLVAT): Heapify-down (percolate down)
 * 
 * Restaurează proprietatea de heap după extragere.
 * Compară cu copiii și coboară spre cel mai mic.
 */
static void heapify_down(MinHeap* h, int idx) {
    while (true) {
        int smallest = idx;
        int left = 2 * idx + 1;
        int right = 2 * idx + 2;
        
        /* Verificăm copilul stâng */
        if (left < h->size && 
            h->nodes[left].distance < h->nodes[smallest].distance) {
            smallest = left;
        }
        
        /* Verificăm copilul drept */
        if (right < h->size && 
            h->nodes[right].distance < h->nodes[smallest].distance) {
            smallest = right;
        }
        
        /* Dacă cel mai mic nu e nodul curent, facem swap și continuăm */
        if (smallest != idx) {
            swap_nodes(h, idx, smallest);
            idx = smallest;
        } else {
            break;
        }
    }
}

/**
 * TODO 6 (REZOLVAT): Inserare în heap
 * 
 * Adaugă un nou nod cu vertex și distanță.
 * Folosește heapify_up pentru a menține proprietatea.
 */
void heap_insert(MinHeap* h, int vertex, int distance) {
    if (h->size >= h->capacity) {
        fprintf(stderr, "Heap overflow!\n");
        return;
    }
    
    /* Adăugăm la sfârșitul heap-ului */
    int idx = h->size;
    h->size++;
    
    h->nodes[idx].vertex = vertex;
    h->nodes[idx].distance = distance;
    h->pos[vertex] = idx;
    
    /* Restaurăm proprietatea de heap */
    heapify_up(h, idx);
}

/**
 * TODO 7 (REZOLVAT): Extragerea minimului
 * 
 * Scoate și returnează nodul cu distanța minimă.
 * Folosește heapify_down pentru a menține proprietatea.
 */
HeapNode heap_extract_min(MinHeap* h) {
    HeapNode min_node = {-1, INF};
    
    if (h->size <= 0) {
        return min_node;
    }
    
    /* Salvăm rădăcina (minimul) */
    min_node = h->nodes[0];
    h->pos[min_node.vertex] = -1;  /* Nu mai e în heap */
    
    /* Mutăm ultimul element în rădăcină */
    h->size--;
    if (h->size > 0) {
        h->nodes[0] = h->nodes[h->size];
        h->pos[h->nodes[0].vertex] = 0;
        
        /* Restaurăm proprietatea de heap */
        heapify_down(h, 0);
    }
    
    return min_node;
}

/**
 * TODO 8 (REZOLVAT): Decrease-key
 * 
 * Scade distanța unui vertex și restaurează heap-ul.
 * Dacă vertex-ul nu e în heap, îl inserăm.
 */
void heap_decrease_key(MinHeap* h, int vertex, int new_distance) {
    int idx = h->pos[vertex];
    
    if (idx == -1) {
        /* Vertex-ul nu e în heap, îl inserăm */
        heap_insert(h, vertex, new_distance);
        return;
    }
    
    /* Verificăm dacă noua distanță e mai mică */
    if (new_distance >= h->nodes[idx].distance) {
        return;  /* Nu facem nimic dacă nu e îmbunătățire */
    }
    
    /* Actualizăm distanța */
    h->nodes[idx].distance = new_distance;
    
    /* Urcăm în heap (distanța a scăzut) */
    heapify_up(h, idx);
}

/**
 * Verifică dacă heap-ul e gol
 */
bool heap_is_empty(MinHeap* h) {
    return h->size == 0;
}

/**
 * Eliberează memoria heap-ului
 */
void heap_destroy(MinHeap* h) {
    if (h) {
        free(h->nodes);
        free(h->pos);
        free(h);
    }
}

/* =============================================================================
 * FUNCȚII GRAF
 * =============================================================================
 */

/**
 * Creează un graf cu V noduri
 */
Graph* graph_create(int V) {
    Graph* g = (Graph*)malloc(sizeof(Graph));
    if (!g) return NULL;
    
    g->V = V;
    g->adj = (Edge**)calloc(V, sizeof(Edge*));
    
    if (!g->adj) {
        free(g);
        return NULL;
    }
    
    return g;
}

/**
 * Adaugă o muchie direcționată
 */
void graph_add_edge(Graph* g, int src, int dest, int weight) {
    Edge* e = (Edge*)malloc(sizeof(Edge));
    if (!e) return;
    
    e->dest = dest;
    e->weight = weight;
    e->next = g->adj[src];
    g->adj[src] = e;
}

/**
 * Eliberează memoria grafului
 */
void graph_destroy(Graph* g) {
    if (!g) return;
    
    for (int i = 0; i < g->V; i++) {
        Edge* e = g->adj[i];
        while (e) {
            Edge* next = e->next;
            free(e);
            e = next;
        }
    }
    
    free(g->adj);
    free(g);
}

/* =============================================================================
 * ALGORITMUL DIJKSTRA
 * =============================================================================
 */

/**
 * TODO 9 (REZOLVAT): Implementarea algoritmului Dijkstra
 * 
 * Găsește drumurile minime de la sursă la toate celelalte noduri.
 * Folosește MinHeap pentru complexitate O((V+E) log V).
 */
void dijkstra(Graph* g, int src, int* dist, int* prev) {
    int V = g->V;
    
    /* Pasul 1: Inițializare */
    for (int i = 0; i < V; i++) {
        dist[i] = INF;
        prev[i] = -1;
    }
    dist[src] = 0;
    
    /* Pasul 2: Creăm heap-ul și inserăm sursa */
    MinHeap* h = heap_create(V);
    heap_insert(h, src, 0);
    
    /* Pasul 3: Procesăm nodurile */
    while (!heap_is_empty(h)) {
        /* Extragem nodul cu distanța minimă */
        HeapNode min_node = heap_extract_min(h);
        int u = min_node.vertex;
        
        /* Dacă distanța extrasă e mai mare decât cea cunoscută, sărim */
        if (min_node.distance > dist[u]) {
            continue;
        }
        
        /* Relaxăm toate muchiile adiacente */
        for (Edge* e = g->adj[u]; e != NULL; e = e->next) {
            int v = e->dest;
            int weight = e->weight;
            
            /* Verificăm dacă am găsit un drum mai scurt */
            if (dist[u] != INF && dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                prev[v] = u;
                
                /* Actualizăm heap-ul */
                heap_decrease_key(h, v, dist[v]);
            }
        }
    }
    
    /* Cleanup */
    heap_destroy(h);
}

/* =============================================================================
 * FUNCȚII DE AFIȘARE
 * =============================================================================
 */

/**
 * TODO 10 (REZOLVAT): Afișarea drumului reconstruit
 */
void print_path(int* prev, int dest, int src) {
    if (dest == src) {
        printf("%d", src);
        return;
    }
    
    if (prev[dest] == -1) {
        printf("(inaccesibil)");
        return;
    }
    
    print_path(prev, prev[dest], src);
    printf(" -> %d", dest);
}

/**
 * Afișează rezultatele Dijkstra
 */
void print_results(int V, int src, int* dist, int* prev) {
    printf("\n%s=== Rezultate Dijkstra ===%s\n", CYAN, NC);
    printf("Nod sursa: %d\n\n", src);
    
    printf("%sDistante minime:%s\n", YELLOW, NC);
    for (int i = 0; i < V; i++) {
        printf("  Nod %d: distanta = ", i);
        if (dist[i] == INF) {
            printf("%sINF%s (inaccesibil)\n", RED, NC);
        } else {
            printf("%s%d%s\n", GREEN, dist[i], NC);
        }
    }
    
    printf("\n%sDrumuri:%s\n", YELLOW, NC);
    for (int i = 0; i < V; i++) {
        printf("  %d -> %d: ", src, i);
        if (dist[i] == INF) {
            printf("%s(nu exista drum)%s\n", RED, NC);
        } else {
            print_path(prev, i, src);
            printf(" %s(cost: %d)%s\n", GREEN, dist[i], NC);
        }
    }
}

/* =============================================================================
 * PROGRAMUL PRINCIPAL
 * =============================================================================
 */

int main(void) {
    int N, M, S;
    
    /* Citim numărul de noduri, muchii și sursa */
    if (scanf("%d %d %d", &N, &M, &S) != 3) {
        fprintf(stderr, "Eroare la citirea parametrilor!\n");
        return 1;
    }
    
    /* Validare */
    if (N <= 0 || M < 0 || S < 0 || S >= N) {
        fprintf(stderr, "Parametri invalizi: N=%d, M=%d, S=%d\n", N, M, S);
        return 1;
    }
    
    /* Creăm graful */
    Graph* g = graph_create(N);
    if (!g) {
        fprintf(stderr, "Eroare la alocarea memoriei pentru graf!\n");
        return 1;
    }
    
    /* Citim muchiile */
    for (int i = 0; i < M; i++) {
        int u, v, w;
        if (scanf("%d %d %d", &u, &v, &w) != 3) {
            fprintf(stderr, "Eroare la citirea muchiei %d!\n", i + 1);
            graph_destroy(g);
            return 1;
        }
        
        if (u < 0 || u >= N || v < 0 || v >= N) {
            fprintf(stderr, "Muchie invalida: %d -> %d\n", u, v);
            graph_destroy(g);
            return 1;
        }
        
        graph_add_edge(g, u, v, w);
    }
    
    /* Alocăm array-urile pentru rezultate */
    int* dist = (int*)malloc(N * sizeof(int));
    int* prev = (int*)malloc(N * sizeof(int));
    
    if (!dist || !prev) {
        fprintf(stderr, "Eroare la alocarea memoriei pentru rezultate!\n");
        free(dist);
        free(prev);
        graph_destroy(g);
        return 1;
    }
    
    /* Rulăm Dijkstra */
    dijkstra(g, S, dist, prev);
    
    /* Afișăm rezultatele */
    print_results(N, S, dist, prev);
    
    /* Cleanup */
    free(dist);
    free(prev);
    graph_destroy(g);
    
    return 0;
}

/* =============================================================================
 * NOTE PENTRU INSTRUCTORI
 * =============================================================================
 * 
 * Puncte cheie de verificat la evaluare:
 * 
 * 1. Heap corect implementat:
 *    - heapify_up și heapify_down funcționează corect
 *    - decrease_key actualizează poziția în heap
 *    - array-ul pos[] e sincronizat cu heap-ul
 * 
 * 2. Dijkstra corect:
 *    - Inițializare dist[src] = 0, restul INF
 *    - Relaxare corectă: dist[u] + w < dist[v]
 *    - prev[] actualizat pentru reconstrucție
 * 
 * 3. Gestiune memorie:
 *    - Nicio scurgere (verificați cu Valgrind)
 *    - Toate alocările verificate
 * 
 * 4. Cazuri limită tratate:
 *    - Graf deconectat (noduri cu dist = INF)
 *    - Graf cu un singur nod
 *    - Muchii cu pondere 0
 * 
 * =============================================================================
 */
