/**
 * =============================================================================
 * SĂPTĂMÂNA 13: ALGORITMI PE GRAFURI - DRUMURI MINIME
 * Exemplu Complet de Lucru
 * =============================================================================
 *
 * Acest exemplu demonstrează:
 *   1. Reprezentarea grafurilor ponderate (liste de adiacență)
 *   2. Implementarea completă a algoritmului Dijkstra
 *   3. Implementarea unui Min-Heap (Priority Queue)
 *   4. Implementarea completă a algoritmului Bellman-Ford
 *   5. Detectarea ciclurilor negative
 *   6. Reconstrucția și afișarea drumurilor minime
 *   7. Comparația performanței între cei doi algoritmi
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -o example1 example1.c -lm
 * Utilizare: ./example1
 *
 * =============================================================================
 */

#define _POSIX_C_SOURCE 200809L  /* Pentru strdup */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include <time.h>

/* =============================================================================
 * CONSTANTE ȘI DEFINIȚII
 * =============================================================================
 */

#define MAX_VERTICES 1000
#define INF INT_MAX

/* Coduri de culoare ANSI pentru output frumos */
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[0;31m"
#define COLOR_GREEN   "\033[0;32m"
#define COLOR_YELLOW  "\033[0;33m"
#define COLOR_BLUE    "\033[0;34m"
#define COLOR_MAGENTA "\033[0;35m"
#define COLOR_CYAN    "\033[0;36m"
#define COLOR_BOLD    "\033[1m"

/* =============================================================================
 * STRUCTURI DE DATE
 * =============================================================================
 */

/**
 * Structură pentru o muchie în lista de adiacență
 */
typedef struct Edge {
    int dest;           /* Nodul destinație */
    int weight;         /* Ponderea muchiei */
    struct Edge* next;  /* Pointer la următoarea muchie */
} Edge;

/**
 * Structură pentru graf reprezentat prin liste de adiacență
 */
typedef struct {
    int num_vertices;       /* Numărul de noduri */
    int num_edges;          /* Numărul de muchii */
    Edge** adj_list;        /* Array de liste de adiacență */
    char** vertex_names;    /* Nume opționale pentru noduri */
} Graph;

/**
 * Structură pentru o muchie simplă (folosită în Bellman-Ford)
 */
typedef struct {
    int src;
    int dest;
    int weight;
} SimpleEdge;

/**
 * Structură pentru lista de muchii (reprezentare alternativă)
 */
typedef struct {
    int num_vertices;
    int num_edges;
    SimpleEdge* edges;
} EdgeListGraph;

/**
 * Structură pentru un element din Min-Heap
 */
typedef struct {
    int vertex;
    int distance;
} HeapNode;

/**
 * Structură pentru Min-Heap (Priority Queue)
 */
typedef struct {
    HeapNode* nodes;    /* Array de noduri heap */
    int* positions;     /* positions[v] = poziția nodului v în heap */
    int size;           /* Dimensiunea curentă */
    int capacity;       /* Capacitatea maximă */
} MinHeap;

/* =============================================================================
 * PART 1: OPERAȚII PE GRAF
 * =============================================================================
 */

/**
 * Creează un graf nou cu numărul specificat de noduri
 */
Graph* graph_create(int num_vertices) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    if (!graph) {
        fprintf(stderr, "Eroare: Nu s-a putut aloca memorie pentru graf\n");
        exit(EXIT_FAILURE);
    }
    
    graph->num_vertices = num_vertices;
    graph->num_edges = 0;
    
    /* Alocăm array-ul de liste de adiacență */
    graph->adj_list = (Edge**)calloc(num_vertices, sizeof(Edge*));
    if (!graph->adj_list) {
        fprintf(stderr, "Eroare: Nu s-a putut aloca memorie pentru liste\n");
        free(graph);
        exit(EXIT_FAILURE);
    }
    
    /* Alocăm array-ul de nume (opțional) */
    graph->vertex_names = (char**)calloc(num_vertices, sizeof(char*));
    
    return graph;
}

/**
 * Adaugă o muchie direcționată în graf
 */
void graph_add_edge(Graph* graph, int src, int dest, int weight) {
    if (src < 0 || src >= graph->num_vertices ||
        dest < 0 || dest >= graph->num_vertices) {
        fprintf(stderr, "Eroare: Indici de noduri invalizi (%d, %d)\n", src, dest);
        return;
    }
    
    /* Creăm noua muchie */
    Edge* new_edge = (Edge*)malloc(sizeof(Edge));
    if (!new_edge) {
        fprintf(stderr, "Eroare: Nu s-a putut aloca memorie pentru muchie\n");
        return;
    }
    
    new_edge->dest = dest;
    new_edge->weight = weight;
    new_edge->next = graph->adj_list[src];
    graph->adj_list[src] = new_edge;
    
    graph->num_edges++;
}

/**
 * Adaugă o muchie bidirecțională (nedirecționată) în graf
 */
void graph_add_undirected_edge(Graph* graph, int src, int dest, int weight) {
    graph_add_edge(graph, src, dest, weight);
    graph_add_edge(graph, dest, src, weight);
}

/**
 * Setează numele unui nod
 */
void graph_set_vertex_name(Graph* graph, int vertex, const char* name) {
    if (vertex >= 0 && vertex < graph->num_vertices) {
        if (graph->vertex_names[vertex]) {
            free(graph->vertex_names[vertex]);
        }
        graph->vertex_names[vertex] = strdup(name);
    }
}

/**
 * Obține numele unui nod (sau indexul ca string)
 */
const char* graph_get_vertex_name(Graph* graph, int vertex) {
    static char buffer[16];
    if (vertex >= 0 && vertex < graph->num_vertices && 
        graph->vertex_names[vertex]) {
        return graph->vertex_names[vertex];
    }
    snprintf(buffer, sizeof(buffer), "%d", vertex);
    return buffer;
}

/**
 * Afișează graful
 */
void graph_print(Graph* graph) {
    printf("\n%s┌─────────────────────────────────────────────────────┐%s\n", 
           COLOR_CYAN, COLOR_RESET);
    printf("%s│            STRUCTURA GRAFULUI                       │%s\n", 
           COLOR_CYAN, COLOR_RESET);
    printf("%s└─────────────────────────────────────────────────────┘%s\n", 
           COLOR_CYAN, COLOR_RESET);
    
    printf("  Noduri: %d, Muchii: %d\n\n", 
           graph->num_vertices, graph->num_edges);
    
    for (int i = 0; i < graph->num_vertices; i++) {
        printf("  %s[%s]%s → ", COLOR_GREEN, 
               graph_get_vertex_name(graph, i), COLOR_RESET);
        
        Edge* edge = graph->adj_list[i];
        while (edge) {
            printf("%s(%d)%s ", 
                   graph_get_vertex_name(graph, edge->dest),
                   edge->weight,
                   edge->next ? " → " : "");
            edge = edge->next;
        }
        printf("\n");
    }
}

/**
 * Eliberează memoria folosită de graf
 */
void graph_free(Graph* graph) {
    if (!graph) return;
    
    for (int i = 0; i < graph->num_vertices; i++) {
        Edge* edge = graph->adj_list[i];
        while (edge) {
            Edge* temp = edge;
            edge = edge->next;
            free(temp);
        }
        if (graph->vertex_names[i]) {
            free(graph->vertex_names[i]);
        }
    }
    
    free(graph->adj_list);
    free(graph->vertex_names);
    free(graph);
}

/* =============================================================================
 * PART 2: MIN-HEAP (PRIORITY QUEUE)
 * =============================================================================
 */

/**
 * Creează un min-heap nou
 */
MinHeap* heap_create(int capacity) {
    MinHeap* heap = (MinHeap*)malloc(sizeof(MinHeap));
    if (!heap) return NULL;
    
    heap->nodes = (HeapNode*)malloc(capacity * sizeof(HeapNode));
    heap->positions = (int*)malloc(capacity * sizeof(int));
    heap->size = 0;
    heap->capacity = capacity;
    
    /* Inițializăm pozițiile ca invalide */
    for (int i = 0; i < capacity; i++) {
        heap->positions[i] = -1;
    }
    
    return heap;
}

/**
 * Interschimbă două noduri în heap
 */
void heap_swap(MinHeap* heap, int i, int j) {
    /* Actualizăm pozițiile */
    heap->positions[heap->nodes[i].vertex] = j;
    heap->positions[heap->nodes[j].vertex] = i;
    
    /* Interschimbăm nodurile */
    HeapNode temp = heap->nodes[i];
    heap->nodes[i] = heap->nodes[j];
    heap->nodes[j] = temp;
}

/**
 * Menține proprietatea de min-heap (heapify down)
 */
void heap_heapify_down(MinHeap* heap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;
    
    if (left < heap->size && 
        heap->nodes[left].distance < heap->nodes[smallest].distance) {
        smallest = left;
    }
    
    if (right < heap->size && 
        heap->nodes[right].distance < heap->nodes[smallest].distance) {
        smallest = right;
    }
    
    if (smallest != idx) {
        heap_swap(heap, idx, smallest);
        heap_heapify_down(heap, smallest);
    }
}

/**
 * Menține proprietatea de min-heap (heapify up)
 */
void heap_heapify_up(MinHeap* heap, int idx) {
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        if (heap->nodes[idx].distance < heap->nodes[parent].distance) {
            heap_swap(heap, idx, parent);
            idx = parent;
        } else {
            break;
        }
    }
}

/**
 * Inserează un nod în heap
 */
void heap_insert(MinHeap* heap, int vertex, int distance) {
    if (heap->size >= heap->capacity) {
        fprintf(stderr, "Eroare: Heap-ul este plin\n");
        return;
    }
    
    int idx = heap->size++;
    heap->nodes[idx].vertex = vertex;
    heap->nodes[idx].distance = distance;
    heap->positions[vertex] = idx;
    
    heap_heapify_up(heap, idx);
}

/**
 * Verifică dacă heap-ul este gol
 */
bool heap_is_empty(MinHeap* heap) {
    return heap->size == 0;
}

/**
 * Extrage nodul cu distanța minimă
 */
HeapNode heap_extract_min(MinHeap* heap) {
    if (heap_is_empty(heap)) {
        HeapNode empty = {-1, INF};
        return empty;
    }
    
    HeapNode min_node = heap->nodes[0];
    heap->positions[min_node.vertex] = -1;
    
    /* Mutăm ultimul element în rădăcină */
    heap->nodes[0] = heap->nodes[--heap->size];
    if (heap->size > 0) {
        heap->positions[heap->nodes[0].vertex] = 0;
        heap_heapify_down(heap, 0);
    }
    
    return min_node;
}

/**
 * Scade distanța unui nod (decrease-key)
 */
void heap_decrease_key(MinHeap* heap, int vertex, int new_distance) {
    int idx = heap->positions[vertex];
    if (idx == -1 || new_distance >= heap->nodes[idx].distance) {
        return;
    }
    
    heap->nodes[idx].distance = new_distance;
    heap_heapify_up(heap, idx);
}

/**
 * Verifică dacă un nod este în heap
 */
bool heap_contains(MinHeap* heap, int vertex) {
    return heap->positions[vertex] != -1;
}

/**
 * Eliberează memoria heap-ului
 */
void heap_free(MinHeap* heap) {
    if (!heap) return;
    free(heap->nodes);
    free(heap->positions);
    free(heap);
}

/* =============================================================================
 * PART 3: ALGORITMUL DIJKSTRA
 * =============================================================================
 */

/**
 * Rezultatul algoritmului Dijkstra
 */
typedef struct {
    int* distances;     /* Distanțele minime de la sursă */
    int* predecessors;  /* Predecesorii pentru reconstrucție drum */
    int source;         /* Nodul sursă */
    int num_vertices;   /* Numărul de noduri */
    bool success;       /* Dacă algoritmul a rulat cu succes */
} DijkstraResult;

/**
 * Implementarea algoritmului Dijkstra cu Min-Heap
 * 
 * Complexitate: O((V + E) log V) cu binary heap
 * 
 * @param graph Graful de intrare (trebuie să aibă muchii non-negative)
 * @param source Nodul sursă
 * @return Structură cu rezultatele (distanțe și predecesori)
 */
DijkstraResult* dijkstra(Graph* graph, int source) {
    int V = graph->num_vertices;
    
    /* Alocăm rezultatul */
    DijkstraResult* result = (DijkstraResult*)malloc(sizeof(DijkstraResult));
    result->distances = (int*)malloc(V * sizeof(int));
    result->predecessors = (int*)malloc(V * sizeof(int));
    result->source = source;
    result->num_vertices = V;
    result->success = true;
    
    /* Inițializăm distanțele și predecesorii */
    for (int i = 0; i < V; i++) {
        result->distances[i] = INF;
        result->predecessors[i] = -1;
    }
    result->distances[source] = 0;
    
    /* Creăm min-heap și inserăm toate nodurile */
    MinHeap* heap = heap_create(V);
    for (int i = 0; i < V; i++) {
        heap_insert(heap, i, result->distances[i]);
    }
    
    /* Bucla principală Dijkstra */
    while (!heap_is_empty(heap)) {
        /* Extragem nodul cu distanța minimă */
        HeapNode min_node = heap_extract_min(heap);
        int u = min_node.vertex;
        
        /* Dacă distanța este infinit, nodurile rămase sunt inaccesibile */
        if (min_node.distance == INF) {
            break;
        }
        
        /* Relaxăm toate muchiile adiacente */
        Edge* edge = graph->adj_list[u];
        while (edge) {
            int v = edge->dest;
            int weight = edge->weight;
            
            /* Verificăm pentru ponderi negative (nu sunt permise) */
            if (weight < 0) {
                fprintf(stderr, "%sAvertisment: Dijkstra nu suportă muchii negative!%s\n",
                        COLOR_RED, COLOR_RESET);
                result->success = false;
            }
            
            /* Relaxare */
            if (result->distances[u] != INF &&
                result->distances[u] + weight < result->distances[v]) {
                result->distances[v] = result->distances[u] + weight;
                result->predecessors[v] = u;
                
                /* Actualizăm distanța în heap */
                if (heap_contains(heap, v)) {
                    heap_decrease_key(heap, v, result->distances[v]);
                }
            }
            
            edge = edge->next;
        }
    }
    
    heap_free(heap);
    return result;
}

/**
 * Afișează drumul de la sursă la destinație
 */
void print_path(DijkstraResult* result, Graph* graph, int dest) {
    if (result->distances[dest] == INF) {
        printf("  Nu există drum până la %s\n", 
               graph_get_vertex_name(graph, dest));
        return;
    }
    
    /* Reconstruim drumul folosind predecesorii */
    int path[MAX_VERTICES];
    int path_length = 0;
    int current = dest;
    
    while (current != -1) {
        path[path_length++] = current;
        current = result->predecessors[current];
    }
    
    /* Afișăm drumul în ordine */
    printf("  Drum: ");
    for (int i = path_length - 1; i >= 0; i--) {
        printf("%s%s%s", COLOR_GREEN, 
               graph_get_vertex_name(graph, path[i]), COLOR_RESET);
        if (i > 0) printf(" → ");
    }
    printf("\n");
    printf("  Distanță totală: %s%d%s\n", 
           COLOR_YELLOW, result->distances[dest], COLOR_RESET);
}

/**
 * Afișează toate rezultatele Dijkstra
 */
void dijkstra_print_results(DijkstraResult* result, Graph* graph) {
    printf("\n%s┌─────────────────────────────────────────────────────┐%s\n", 
           COLOR_GREEN, COLOR_RESET);
    printf("%s│         REZULTATE DIJKSTRA                          │%s\n", 
           COLOR_GREEN, COLOR_RESET);
    printf("%s└─────────────────────────────────────────────────────┘%s\n", 
           COLOR_GREEN, COLOR_RESET);
    
    printf("\n  Sursă: %s%s%s\n\n", COLOR_CYAN, 
           graph_get_vertex_name(graph, result->source), COLOR_RESET);
    
    printf("  %-10s %-10s %-10s\n", "Nod", "Distanță", "Predecesor");
    printf("  %-10s %-10s %-10s\n", "───", "────────", "──────────");
    
    for (int i = 0; i < result->num_vertices; i++) {
        printf("  %-10s ", graph_get_vertex_name(graph, i));
        
        if (result->distances[i] == INF) {
            printf("%-10s ", "∞");
        } else {
            printf("%-10d ", result->distances[i]);
        }
        
        if (result->predecessors[i] == -1) {
            printf("%-10s", i == result->source ? "SURSĂ" : "—");
        } else {
            printf("%-10s", graph_get_vertex_name(graph, result->predecessors[i]));
        }
        printf("\n");
    }
}

/**
 * Eliberează memoria rezultatului Dijkstra
 */
void dijkstra_free(DijkstraResult* result) {
    if (!result) return;
    free(result->distances);
    free(result->predecessors);
    free(result);
}

/* =============================================================================
 * PART 4: ALGORITMUL BELLMAN-FORD
 * =============================================================================
 */

/**
 * Rezultatul algoritmului Bellman-Ford
 */
typedef struct {
    int* distances;         /* Distanțele minime de la sursă */
    int* predecessors;      /* Predecesorii pentru reconstrucție drum */
    int source;             /* Nodul sursă */
    int num_vertices;       /* Numărul de noduri */
    bool has_negative_cycle;/* Dacă există ciclu negativ */
    int* affected_by_cycle; /* Nodurile afectate de ciclul negativ */
} BellmanFordResult;

/**
 * Implementarea algoritmului Bellman-Ford
 * 
 * Complexitate: O(V × E)
 * 
 * Avantaje față de Dijkstra:
 *   - Suportă muchii cu ponderi negative
 *   - Detectează cicluri negative
 * 
 * @param graph Graful de intrare
 * @param source Nodul sursă
 * @return Structură cu rezultatele
 */
BellmanFordResult* bellman_ford(Graph* graph, int source) {
    int V = graph->num_vertices;
    
    /* Alocăm rezultatul */
    BellmanFordResult* result = (BellmanFordResult*)malloc(sizeof(BellmanFordResult));
    result->distances = (int*)malloc(V * sizeof(int));
    result->predecessors = (int*)malloc(V * sizeof(int));
    result->affected_by_cycle = (int*)calloc(V, sizeof(int));
    result->source = source;
    result->num_vertices = V;
    result->has_negative_cycle = false;
    
    /* Inițializăm distanțele */
    for (int i = 0; i < V; i++) {
        result->distances[i] = INF;
        result->predecessors[i] = -1;
    }
    result->distances[source] = 0;
    
    /* Faza 1: Relaxare (V-1 iterații) */
    printf("\n  %sFaza 1: Relaxare (V-1 = %d iterații)%s\n", 
           COLOR_YELLOW, V-1, COLOR_RESET);
    
    for (int i = 0; i < V - 1; i++) {
        bool any_update = false;
        
        /* Parcurgem toate muchiile */
        for (int u = 0; u < V; u++) {
            if (result->distances[u] == INF) continue;
            
            Edge* edge = graph->adj_list[u];
            while (edge) {
                int v = edge->dest;
                int weight = edge->weight;
                
                /* Relaxare */
                if (result->distances[u] + weight < result->distances[v]) {
                    result->distances[v] = result->distances[u] + weight;
                    result->predecessors[v] = u;
                    any_update = true;
                }
                
                edge = edge->next;
            }
        }
        
        /* Optimizare: dacă nu s-a făcut nicio actualizare, putem opri */
        if (!any_update) {
            printf("    Convergență timpurie la iterația %d\n", i + 1);
            break;
        }
    }
    
    /* Faza 2: Detectare ciclu negativ */
    printf("  %sFaza 2: Detectare cicluri negative%s\n", COLOR_YELLOW, COLOR_RESET);
    
    for (int u = 0; u < V; u++) {
        if (result->distances[u] == INF) continue;
        
        Edge* edge = graph->adj_list[u];
        while (edge) {
            int v = edge->dest;
            int weight = edge->weight;
            
            /* Dacă se mai poate relaxa, există ciclu negativ */
            if (result->distances[u] + weight < result->distances[v]) {
                result->has_negative_cycle = true;
                result->affected_by_cycle[v] = 1;
                
                /* Marcăm toate nodurile accesibile din v ca afectate */
                /* (simplificat - într-o implementare completă am face BFS) */
            }
            
            edge = edge->next;
        }
    }
    
    if (result->has_negative_cycle) {
        printf("    %s⚠ DETECTAT CICLU NEGATIV!%s\n", COLOR_RED, COLOR_RESET);
    } else {
        printf("    %s✓ Nu există cicluri negative%s\n", COLOR_GREEN, COLOR_RESET);
    }
    
    return result;
}

/**
 * Afișează rezultatele Bellman-Ford
 */
void bellman_ford_print_results(BellmanFordResult* result, Graph* graph) {
    printf("\n%s┌─────────────────────────────────────────────────────┐%s\n", 
           COLOR_MAGENTA, COLOR_RESET);
    printf("%s│         REZULTATE BELLMAN-FORD                      │%s\n", 
           COLOR_MAGENTA, COLOR_RESET);
    printf("%s└─────────────────────────────────────────────────────┘%s\n", 
           COLOR_MAGENTA, COLOR_RESET);
    
    printf("\n  Sursă: %s%s%s\n", COLOR_CYAN, 
           graph_get_vertex_name(graph, result->source), COLOR_RESET);
    
    if (result->has_negative_cycle) {
        printf("  %s⚠ ATENȚIE: Graful conține cicluri negative!%s\n", 
               COLOR_RED, COLOR_RESET);
        printf("  Distanțele pentru unele noduri pot fi -∞\n\n");
    }
    printf("\n");
    
    printf("  %-10s %-10s %-10s %-10s\n", "Nod", "Distanță", "Predecesor", "Ciclu?");
    printf("  %-10s %-10s %-10s %-10s\n", "───", "────────", "──────────", "──────");
    
    for (int i = 0; i < result->num_vertices; i++) {
        printf("  %-10s ", graph_get_vertex_name(graph, i));
        
        if (result->affected_by_cycle[i]) {
            printf("%-10s ", "-∞");
        } else if (result->distances[i] == INF) {
            printf("%-10s ", "∞");
        } else {
            printf("%-10d ", result->distances[i]);
        }
        
        if (result->predecessors[i] == -1) {
            printf("%-10s", i == result->source ? "SURSĂ" : "—");
        } else {
            printf("%-10s", graph_get_vertex_name(graph, result->predecessors[i]));
        }
        
        printf(" %s\n", result->affected_by_cycle[i] ? "DA" : "");
    }
}

/**
 * Eliberează memoria rezultatului Bellman-Ford
 */
void bellman_ford_free(BellmanFordResult* result) {
    if (!result) return;
    free(result->distances);
    free(result->predecessors);
    free(result->affected_by_cycle);
    free(result);
}

/* =============================================================================
 * PART 5: FUNCȚII DEMONSTRATIVE
 * =============================================================================
 */

/**
 * Demonstrație 1: Dijkstra pe un graf simplu
 */
void demo_dijkstra_simple(void) {
    printf("\n");
    printf("%s╔═══════════════════════════════════════════════════════════════╗%s\n", 
           COLOR_BOLD, COLOR_RESET);
    printf("%s║      DEMONSTRAȚIE 1: DIJKSTRA - GRAF SIMPLU                   ║%s\n", 
           COLOR_BOLD, COLOR_RESET);
    printf("%s╚═══════════════════════════════════════════════════════════════╝%s\n", 
           COLOR_BOLD, COLOR_RESET);
    
    /*
     * Graful demonstrativ:
     * 
     *        (1)
     *       / | \
     *     4   1   2
     *    /    |    \
     *  (0)───(2)───(3)
     *     3      5
     *        \   /
     *         (4)
     *           6
     */
    
    Graph* graph = graph_create(5);
    
    /* Setăm nume pentru noduri */
    graph_set_vertex_name(graph, 0, "A");
    graph_set_vertex_name(graph, 1, "B");
    graph_set_vertex_name(graph, 2, "C");
    graph_set_vertex_name(graph, 3, "D");
    graph_set_vertex_name(graph, 4, "E");
    
    /* Adăugăm muchiile (direcționate) */
    graph_add_edge(graph, 0, 1, 4);   /* A → B: 4 */
    graph_add_edge(graph, 0, 2, 3);   /* A → C: 3 */
    graph_add_edge(graph, 1, 2, 1);   /* B → C: 1 */
    graph_add_edge(graph, 1, 3, 2);   /* B → D: 2 */
    graph_add_edge(graph, 2, 3, 5);   /* C → D: 5 */
    graph_add_edge(graph, 2, 4, 6);   /* C → E: 6 */
    graph_add_edge(graph, 3, 4, 1);   /* D → E: 1 */
    
    graph_print(graph);
    
    /* Rulăm Dijkstra de la nodul A (0) */
    printf("\n  Rulăm Dijkstra de la nodul %sA%s...\n", COLOR_CYAN, COLOR_RESET);
    DijkstraResult* result = dijkstra(graph, 0);
    
    dijkstra_print_results(result, graph);
    
    /* Afișăm drumurile către fiecare nod */
    printf("\n  %sDrumuri minime:%s\n", COLOR_CYAN, COLOR_RESET);
    for (int i = 0; i < graph->num_vertices; i++) {
        if (i != result->source) {
            print_path(result, graph, i);
        }
    }
    
    dijkstra_free(result);
    graph_free(graph);
}

/**
 * Demonstrație 2: Bellman-Ford cu muchii negative
 */
void demo_bellman_ford_negative(void) {
    printf("\n");
    printf("%s╔═══════════════════════════════════════════════════════════════╗%s\n", 
           COLOR_BOLD, COLOR_RESET);
    printf("%s║      DEMONSTRAȚIE 2: BELLMAN-FORD - MUCHII NEGATIVE           ║%s\n", 
           COLOR_BOLD, COLOR_RESET);
    printf("%s╚═══════════════════════════════════════════════════════════════╝%s\n", 
           COLOR_BOLD, COLOR_RESET);
    
    /*
     * Graful cu muchie negativă:
     * 
     *   (0)──4──>(1)
     *    │       │
     *    2      -3   (muchie negativă!)
     *    │       │
     *    v       v
     *   (2)──1──>(3)
     */
    
    Graph* graph = graph_create(4);
    
    graph_set_vertex_name(graph, 0, "S");
    graph_set_vertex_name(graph, 1, "A");
    graph_set_vertex_name(graph, 2, "B");
    graph_set_vertex_name(graph, 3, "T");
    
    graph_add_edge(graph, 0, 1, 4);   /* S → A: 4 */
    graph_add_edge(graph, 0, 2, 2);   /* S → B: 2 */
    graph_add_edge(graph, 1, 3, -3);  /* A → T: -3 (negativă!) */
    graph_add_edge(graph, 2, 3, 1);   /* B → T: 1 */
    
    graph_print(graph);
    
    printf("\n  %s⚠ Notă: Muchia A → T are pondere negativă (-3)%s\n", 
           COLOR_YELLOW, COLOR_RESET);
    printf("  Dijkstra ar da rezultate incorecte!\n");
    
    /* Rulăm Bellman-Ford */
    printf("\n  Rulăm Bellman-Ford de la nodul %sS%s...\n", COLOR_CYAN, COLOR_RESET);
    BellmanFordResult* result = bellman_ford(graph, 0);
    
    bellman_ford_print_results(result, graph);
    
    printf("\n  %sObservație:%s Drumul S → A → T are costul 4 + (-3) = %s1%s\n",
           COLOR_CYAN, COLOR_RESET, COLOR_GREEN, COLOR_RESET);
    printf("  Mai scurt decât drumul direct S → B → T care are costul 2 + 1 = 3\n");
    
    bellman_ford_free(result);
    graph_free(graph);
}

/**
 * Demonstrație 3: Detectare ciclu negativ
 */
void demo_negative_cycle(void) {
    printf("\n");
    printf("%s╔═══════════════════════════════════════════════════════════════╗%s\n", 
           COLOR_BOLD, COLOR_RESET);
    printf("%s║      DEMONSTRAȚIE 3: DETECTARE CICLU NEGATIV                  ║%s\n", 
           COLOR_BOLD, COLOR_RESET);
    printf("%s╚═══════════════════════════════════════════════════════════════╝%s\n", 
           COLOR_BOLD, COLOR_RESET);
    
    /*
     * Graful cu ciclu negativ:
     * 
     *   (0)──2──>(1)
     *            / \
     *          -1   3
     *          /     \
     *        (3)<─-5─(2)
     *         
     *    Ciclul: 1 → 2 → 3 → 1 are suma: 3 + (-5) + (-1) = -3 < 0
     */
    
    Graph* graph = graph_create(4);
    
    graph_set_vertex_name(graph, 0, "S");
    graph_set_vertex_name(graph, 1, "X");
    graph_set_vertex_name(graph, 2, "Y");
    graph_set_vertex_name(graph, 3, "Z");
    
    graph_add_edge(graph, 0, 1, 2);   /* S → X: 2 */
    graph_add_edge(graph, 1, 2, 3);   /* X → Y: 3 */
    graph_add_edge(graph, 2, 3, -5);  /* Y → Z: -5 */
    graph_add_edge(graph, 3, 1, -1);  /* Z → X: -1 (completează ciclul!) */
    
    graph_print(graph);
    
    printf("\n  %s⚠ CICLUL NEGATIV: X → Y → Z → X%s\n", COLOR_RED, COLOR_RESET);
    printf("  Suma ponderilor: 3 + (-5) + (-1) = %s-3%s < 0\n", 
           COLOR_RED, COLOR_RESET);
    printf("\n  Acest ciclu poate fi parcurs infinit pentru a reduce distanța!\n");
    
    /* Rulăm Bellman-Ford */
    printf("\n  Rulăm Bellman-Ford de la nodul %sS%s...\n", COLOR_CYAN, COLOR_RESET);
    BellmanFordResult* result = bellman_ford(graph, 0);
    
    bellman_ford_print_results(result, graph);
    
    if (result->has_negative_cycle) {
        printf("\n  %sConsecinșă:%s Distanțele către nodurile din ciclu sunt -∞\n",
               COLOR_YELLOW, COLOR_RESET);
        printf("  (pot fi reduse indefinit prin parcurgerea ciclului)\n");
    }
    
    bellman_ford_free(result);
    graph_free(graph);
}

/**
 * Demonstrație 4: Comparație Dijkstra vs Bellman-Ford
 */
void demo_comparison(void) {
    printf("\n");
    printf("%s╔═══════════════════════════════════════════════════════════════╗%s\n", 
           COLOR_BOLD, COLOR_RESET);
    printf("%s║      DEMONSTRAȚIE 4: COMPARAȚIE DIJKSTRA vs BELLMAN-FORD      ║%s\n", 
           COLOR_BOLD, COLOR_RESET);
    printf("%s╚═══════════════════════════════════════════════════════════════╝%s\n", 
           COLOR_BOLD, COLOR_RESET);
    
    /* Creăm un graf mai mare pentru comparație */
    int V = 6;
    Graph* graph = graph_create(V);
    
    graph_set_vertex_name(graph, 0, "Start");
    graph_set_vertex_name(graph, 1, "A");
    graph_set_vertex_name(graph, 2, "B");
    graph_set_vertex_name(graph, 3, "C");
    graph_set_vertex_name(graph, 4, "D");
    graph_set_vertex_name(graph, 5, "End");
    
    /* Muchii (toate non-negative pentru a putea compara) */
    graph_add_edge(graph, 0, 1, 5);
    graph_add_edge(graph, 0, 2, 3);
    graph_add_edge(graph, 1, 2, 2);
    graph_add_edge(graph, 1, 3, 6);
    graph_add_edge(graph, 2, 3, 4);
    graph_add_edge(graph, 2, 4, 2);
    graph_add_edge(graph, 3, 4, 1);
    graph_add_edge(graph, 3, 5, 2);
    graph_add_edge(graph, 4, 5, 3);
    
    graph_print(graph);
    
    /* Măsurăm timpul pentru Dijkstra */
    printf("\n  %s── Rulare DIJKSTRA ──%s\n", COLOR_GREEN, COLOR_RESET);
    clock_t start_d = clock();
    DijkstraResult* dijkstra_result = dijkstra(graph, 0);
    clock_t end_d = clock();
    double time_d = ((double)(end_d - start_d)) / CLOCKS_PER_SEC * 1000;
    
    dijkstra_print_results(dijkstra_result, graph);
    printf("\n  Timp execuție: %.4f ms\n", time_d);
    
    /* Măsurăm timpul pentru Bellman-Ford */
    printf("\n  %s── Rulare BELLMAN-FORD ──%s\n", COLOR_MAGENTA, COLOR_RESET);
    clock_t start_b = clock();
    BellmanFordResult* bf_result = bellman_ford(graph, 0);
    clock_t end_b = clock();
    double time_b = ((double)(end_b - start_b)) / CLOCKS_PER_SEC * 1000;
    
    bellman_ford_print_results(bf_result, graph);
    printf("\n  Timp execuție: %.4f ms\n", time_b);
    
    /* Comparație */
    printf("\n%s┌─────────────────────────────────────────────────────┐%s\n", 
           COLOR_CYAN, COLOR_RESET);
    printf("%s│               TABEL COMPARATIV                      │%s\n", 
           COLOR_CYAN, COLOR_RESET);
    printf("%s└─────────────────────────────────────────────────────┘%s\n", 
           COLOR_CYAN, COLOR_RESET);
    
    printf("\n  %-20s %-15s %-15s\n", "Criteriu", "Dijkstra", "Bellman-Ford");
    printf("  %-20s %-15s %-15s\n", "────────", "────────", "────────────");
    printf("  %-20s O((V+E)logV)    O(V×E)\n", "Complexitate");
    printf("  %-20s Nu              Da\n", "Muchii negative");
    printf("  %-20s Nu              Da\n", "Detectare cicluri");
    printf("  %-20s %.4f ms       %.4f ms\n", "Timp măsurat", time_d, time_b);
    
    dijkstra_free(dijkstra_result);
    bellman_ford_free(bf_result);
    graph_free(graph);
}

/**
 * Demonstrație 5: Aplicație practică - Rețea de routere
 */
void demo_routing_network(void) {
    printf("\n");
    printf("%s╔═══════════════════════════════════════════════════════════════╗%s\n", 
           COLOR_BOLD, COLOR_RESET);
    printf("%s║      DEMONSTRAȚIE 5: APLICAȚIE - REȚEA DE ROUTERE             ║%s\n", 
           COLOR_BOLD, COLOR_RESET);
    printf("%s╚═══════════════════════════════════════════════════════════════╝%s\n", 
           COLOR_BOLD, COLOR_RESET);
    
    printf("\n  Simulăm o rețea de routere (similar cu protocolul OSPF)\n");
    printf("  Ponderile reprezintă latența în milisecunde\n");
    
    Graph* network = graph_create(7);
    
    graph_set_vertex_name(network, 0, "R1");
    graph_set_vertex_name(network, 1, "R2");
    graph_set_vertex_name(network, 2, "R3");
    graph_set_vertex_name(network, 3, "R4");
    graph_set_vertex_name(network, 4, "R5");
    graph_set_vertex_name(network, 5, "R6");
    graph_set_vertex_name(network, 6, "R7");
    
    /* Legături bidirecționale cu latențe */
    graph_add_undirected_edge(network, 0, 1, 10);  /* R1-R2: 10ms */
    graph_add_undirected_edge(network, 0, 2, 15);  /* R1-R3: 15ms */
    graph_add_undirected_edge(network, 1, 3, 12);  /* R2-R4: 12ms */
    graph_add_undirected_edge(network, 1, 4, 20);  /* R2-R5: 20ms */
    graph_add_undirected_edge(network, 2, 4, 10);  /* R3-R5: 10ms */
    graph_add_undirected_edge(network, 3, 5, 8);   /* R4-R6: 8ms */
    graph_add_undirected_edge(network, 4, 5, 15);  /* R5-R6: 15ms */
    graph_add_undirected_edge(network, 4, 6, 25);  /* R5-R7: 25ms */
    graph_add_undirected_edge(network, 5, 6, 5);   /* R6-R7: 5ms */
    
    graph_print(network);
    
    /* Calculăm tabelul de rutare pentru R1 */
    printf("\n  Calculăm tabelul de rutare pentru routerul %sR1%s...\n", 
           COLOR_CYAN, COLOR_RESET);
    
    DijkstraResult* routing = dijkstra(network, 0);
    
    printf("\n%s┌───────────────────────────────────────────────────────────────┐%s\n", 
           COLOR_YELLOW, COLOR_RESET);
    printf("%s│                  TABEL DE RUTARE - R1                         │%s\n", 
           COLOR_YELLOW, COLOR_RESET);
    printf("%s├───────────┬─────────────┬────────────────────────────────────┤%s\n", 
           COLOR_YELLOW, COLOR_RESET);
    printf("%s│ Destinație│  Latență    │  Ruta                              │%s\n", 
           COLOR_YELLOW, COLOR_RESET);
    printf("%s├───────────┼─────────────┼────────────────────────────────────┤%s\n", 
           COLOR_YELLOW, COLOR_RESET);
    
    for (int dest = 1; dest < network->num_vertices; dest++) {
        printf("%s│%s %-10s%s│%s", COLOR_YELLOW, COLOR_RESET,
               graph_get_vertex_name(network, dest), COLOR_YELLOW, COLOR_RESET);
        
        if (routing->distances[dest] == INF) {
            printf("  Inaccesibil │  —");
        } else {
            printf("  %3d ms      %s│%s  ", routing->distances[dest],
                   COLOR_YELLOW, COLOR_RESET);
            
            /* Reconstruim și afișăm ruta */
            int path[MAX_VERTICES];
            int path_len = 0;
            int curr = dest;
            while (curr != -1) {
                path[path_len++] = curr;
                curr = routing->predecessors[curr];
            }
            
            for (int i = path_len - 1; i >= 0; i--) {
                printf("%s", graph_get_vertex_name(network, path[i]));
                if (i > 0) printf("→");
            }
        }
        printf("\n");
    }
    printf("%s└───────────┴─────────────┴────────────────────────────────────┘%s\n", 
           COLOR_YELLOW, COLOR_RESET);
    
    dijkstra_free(routing);
    graph_free(network);
}

/* =============================================================================
 * PART 6: FUNCȚIA MAIN
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("%s╔═══════════════════════════════════════════════════════════════╗%s\n", 
           COLOR_BOLD COLOR_BLUE, COLOR_RESET);
    printf("%s║                                                               ║%s\n", 
           COLOR_BOLD COLOR_BLUE, COLOR_RESET);
    printf("%s║     SĂPTĂMÂNA 13: ALGORITMI PE GRAFURI - DRUMURI MINIME       ║%s\n", 
           COLOR_BOLD COLOR_BLUE, COLOR_RESET);
    printf("%s║                                                               ║%s\n", 
           COLOR_BOLD COLOR_BLUE, COLOR_RESET);
    printf("%s║              Dijkstra & Bellman-Ford                          ║%s\n", 
           COLOR_BOLD COLOR_BLUE, COLOR_RESET);
    printf("%s║                                                               ║%s\n", 
           COLOR_BOLD COLOR_BLUE, COLOR_RESET);
    printf("%s╚═══════════════════════════════════════════════════════════════╝%s\n", 
           COLOR_BOLD COLOR_BLUE, COLOR_RESET);
    
    /* Rulăm toate demonstrațiile */
    demo_dijkstra_simple();
    demo_bellman_ford_negative();
    demo_negative_cycle();
    demo_comparison();
    demo_routing_network();
    
    printf("\n");
    printf("%s╔═══════════════════════════════════════════════════════════════╗%s\n", 
           COLOR_GREEN, COLOR_RESET);
    printf("%s║     ✓ TOATE DEMONSTRAȚIILE AU FOST EXECUTATE CU SUCCES!       ║%s\n", 
           COLOR_GREEN, COLOR_RESET);
    printf("%s╚═══════════════════════════════════════════════════════════════╝%s\n", 
           COLOR_GREEN, COLOR_RESET);
    printf("\n");
    
    return 0;
}

/* =============================================================================
 * SFÂRȚIT EXEMPLU
 * =============================================================================
 */
