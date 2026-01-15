/**
 * =============================================================================
 * SOLUȚIE TEMĂ 1: Sistem de Navigație GPS
 * =============================================================================
 * 
 * ATENȚIE: Acest fișier este DOAR pentru instructori!
 * Nu distribuiți studenților înainte de termenul limită.
 * 
 * Această soluție demonstrează:
 *   1. Citirea hărții orașului din fișier
 *   2. Reprezentarea cu liste de adiacență (cu nume străzi)
 *   3. Algoritmul Dijkstra pentru găsirea drumului minim
 *   4. Reconstrucția și afișarea drumului cu nume de străzi
 *   5. Calculul rutelor alternative (excludere muchie)
 * 
 * Compilare: gcc -Wall -Wextra -std=c11 -o hw1_sol homework1_sol.c
 * Utilizare: ./hw1_sol < bucuresti.txt
 * 
 * =============================================================================
 */

#define _POSIX_C_SOURCE 200809L  /* Pentru strdup */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>

/* =============================================================================
 * CONSTANTE
 * =============================================================================
 */

#define INF INT_MAX
#define MAX_STREET_NAME 64

#define GREEN   "\033[0;32m"
#define CYAN    "\033[0;36m"
#define YELLOW  "\033[0;33m"
#define RED     "\033[0;31m"
#define NC      "\033[0m"

/* =============================================================================
 * STRUCTURI DE DATE
 * =============================================================================
 */

typedef struct Edge {
    int dest;
    int weight;
    char street_name[MAX_STREET_NAME];
    struct Edge* next;
} Edge;

typedef struct {
    int V;
    Edge** adj;
} CityMap;

typedef struct {
    int vertex;
    int distance;
} HeapNode;

typedef struct {
    HeapNode* nodes;
    int* pos;
    int size;
    int capacity;
} MinHeap;

/* =============================================================================
 * FUNCȚII HEAP
 * =============================================================================
 */

static void swap_nodes(MinHeap* h, int i, int j) {
    h->pos[h->nodes[i].vertex] = j;
    h->pos[h->nodes[j].vertex] = i;
    HeapNode temp = h->nodes[i];
    h->nodes[i] = h->nodes[j];
    h->nodes[j] = temp;
}

MinHeap* heap_create(int capacity) {
    MinHeap* h = malloc(sizeof(MinHeap));
    h->capacity = capacity;
    h->size = 0;
    h->nodes = malloc(capacity * sizeof(HeapNode));
    h->pos = malloc(capacity * sizeof(int));
    for (int i = 0; i < capacity; i++) h->pos[i] = -1;
    return h;
}

void heap_destroy(MinHeap* h) {
    free(h->nodes);
    free(h->pos);
    free(h);
}

static void heapify_up(MinHeap* h, int idx) {
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        if (h->nodes[idx].distance < h->nodes[parent].distance) {
            swap_nodes(h, idx, parent);
            idx = parent;
        } else break;
    }
}

static void heapify_down(MinHeap* h, int idx) {
    while (true) {
        int smallest = idx;
        int left = 2 * idx + 1;
        int right = 2 * idx + 2;
        if (left < h->size && h->nodes[left].distance < h->nodes[smallest].distance)
            smallest = left;
        if (right < h->size && h->nodes[right].distance < h->nodes[smallest].distance)
            smallest = right;
        if (smallest != idx) {
            swap_nodes(h, idx, smallest);
            idx = smallest;
        } else break;
    }
}

void heap_insert(MinHeap* h, int v, int d) {
    int idx = h->size++;
    h->nodes[idx] = (HeapNode){v, d};
    h->pos[v] = idx;
    heapify_up(h, idx);
}

HeapNode heap_extract_min(MinHeap* h) {
    HeapNode min = h->nodes[0];
    h->pos[min.vertex] = -1;
    h->size--;
    if (h->size > 0) {
        h->nodes[0] = h->nodes[h->size];
        h->pos[h->nodes[0].vertex] = 0;
        heapify_down(h, 0);
    }
    return min;
}

void heap_decrease_key(MinHeap* h, int v, int d) {
    int idx = h->pos[v];
    if (idx == -1) {
        heap_insert(h, v, d);
        return;
    }
    if (d < h->nodes[idx].distance) {
        h->nodes[idx].distance = d;
        heapify_up(h, idx);
    }
}

/* =============================================================================
 * FUNCȚII HARTĂ
 * =============================================================================
 */

CityMap* map_create(int V) {
    CityMap* m = malloc(sizeof(CityMap));
    m->V = V;
    m->adj = calloc(V, sizeof(Edge*));
    return m;
}

void map_add_street(CityMap* m, int u, int v, int weight, const char* name) {
    /* Adăugăm în ambele direcții (stradă bidirecțională) */
    Edge* e1 = malloc(sizeof(Edge));
    e1->dest = v;
    e1->weight = weight;
    strncpy(e1->street_name, name, MAX_STREET_NAME - 1);
    e1->street_name[MAX_STREET_NAME - 1] = '\0';
    e1->next = m->adj[u];
    m->adj[u] = e1;
    
    Edge* e2 = malloc(sizeof(Edge));
    e2->dest = u;
    e2->weight = weight;
    strncpy(e2->street_name, name, MAX_STREET_NAME - 1);
    e2->street_name[MAX_STREET_NAME - 1] = '\0';
    e2->next = m->adj[v];
    m->adj[v] = e2;
}

void map_destroy(CityMap* m) {
    for (int i = 0; i < m->V; i++) {
        Edge* e = m->adj[i];
        while (e) {
            Edge* next = e->next;
            free(e);
            e = next;
        }
    }
    free(m->adj);
    free(m);
}

/* Găsește numele străzii între două noduri */
const char* find_street_name(CityMap* m, int u, int v) {
    for (Edge* e = m->adj[u]; e; e = e->next) {
        if (e->dest == v) return e->street_name;
    }
    return "???";
}

/* =============================================================================
 * DIJKSTRA CU EXCLUDERE OPȚIONALĂ
 * =============================================================================
 */

typedef struct {
    int* dist;
    int* prev;
    char** prev_street;  /* Numele străzii folosite pentru a ajunge */
} DijkstraResult;

DijkstraResult* dijkstra_result_create(int V) {
    DijkstraResult* r = malloc(sizeof(DijkstraResult));
    r->dist = malloc(V * sizeof(int));
    r->prev = malloc(V * sizeof(int));
    r->prev_street = malloc(V * sizeof(char*));
    
    for (int i = 0; i < V; i++) {
        r->dist[i] = INF;
        r->prev[i] = -1;
        r->prev_street[i] = NULL;
    }
    return r;
}

void dijkstra_result_destroy(DijkstraResult* r, int V) {
    for (int i = 0; i < V; i++) {
        free(r->prev_street[i]);
    }
    free(r->prev_street);
    free(r->dist);
    free(r->prev);
    free(r);
}

/**
 * Dijkstra cu posibilitatea de a exclude o muchie
 * exclude_u, exclude_v: muchia de exclus (-1 pentru a nu exclude nimic)
 */
DijkstraResult* dijkstra_with_exclusion(CityMap* m, int src, 
                                         int exclude_u, int exclude_v) {
    DijkstraResult* r = dijkstra_result_create(m->V);
    r->dist[src] = 0;
    
    MinHeap* h = heap_create(m->V);
    heap_insert(h, src, 0);
    
    while (h->size > 0) {
        HeapNode min = heap_extract_min(h);
        int u = min.vertex;
        
        if (min.distance > r->dist[u]) continue;
        
        for (Edge* e = m->adj[u]; e; e = e->next) {
            int v = e->dest;
            
            /* Verificăm dacă trebuie să excludem această muchie */
            if ((u == exclude_u && v == exclude_v) ||
                (u == exclude_v && v == exclude_u)) {
                continue;
            }
            
            if (r->dist[u] != INF && r->dist[u] + e->weight < r->dist[v]) {
                r->dist[v] = r->dist[u] + e->weight;
                r->prev[v] = u;
                
                /* Salvăm numele străzii */
                free(r->prev_street[v]);
                r->prev_street[v] = strdup(e->street_name);
                
                heap_decrease_key(h, v, r->dist[v]);
            }
        }
    }
    
    heap_destroy(h);
    return r;
}

/* =============================================================================
 * FUNCȚII DE AFIȘARE
 * =============================================================================
 */

void print_path_with_streets(DijkstraResult* r, int src, int dest) {
    if (dest == src) {
        printf("%d", src);
        return;
    }
    
    if (r->prev[dest] == -1) {
        printf("(nu exista drum)");
        return;
    }
    
    print_path_with_streets(r, src, r->prev[dest]);
    printf(" %s(%s)%s -> %d", YELLOW, r->prev_street[dest], NC, dest);
}

/**
 * Găsește muchia critică din drumul optim (pentru rută alternativă)
 */
void find_critical_edge(DijkstraResult* r, int dest, int* crit_u, int* crit_v, 
                        char* crit_street) {
    /* Prima muchie din drum (cea care pleacă din src) */
    /* Sau putem alege altă euristică - aici alegem prima */
    
    /* Reconstruim drumul și găsim prima muchie */
    int path[1000];
    int len = 0;
    
    int v = dest;
    while (v != -1) {
        path[len++] = v;
        v = r->prev[v];
    }
    
    /* Inversăm */
    for (int i = 0; i < len / 2; i++) {
        int t = path[i];
        path[i] = path[len - 1 - i];
        path[len - 1 - i] = t;
    }
    
    /* Alegem muchia din mijloc pentru diversitate */
    if (len >= 2) {
        int mid = len / 2;
        *crit_u = path[mid - 1];
        *crit_v = path[mid];
        if (r->prev_street[path[mid]]) {
            strcpy(crit_street, r->prev_street[path[mid]]);
        } else {
            strcpy(crit_street, "necunoscut");
        }
    } else {
        *crit_u = -1;
        *crit_v = -1;
        strcpy(crit_street, "N/A");
    }
}

void process_query(CityMap* m, int src, int dest, int query_num) {
    printf("\n%s=== Interogare %d: %d -> %d ===%s\n", CYAN, query_num, src, dest, NC);
    
    /* Drumul optim */
    DijkstraResult* r = dijkstra_with_exclusion(m, src, -1, -1);
    
    if (r->dist[dest] == INF) {
        printf("Nu exista drum intre %d si %d\n", src, dest);
        dijkstra_result_destroy(r, m->V);
        return;
    }
    
    printf("Distanta minima: %s%d metri%s\n", GREEN, r->dist[dest], NC);
    printf("Drum: ");
    print_path_with_streets(r, src, dest);
    printf("\n");
    
    /* Rută alternativă */
    int crit_u, crit_v;
    char crit_street[MAX_STREET_NAME];
    find_critical_edge(r, dest, &crit_u, &crit_v, crit_street);
    
    if (crit_u != -1) {
        DijkstraResult* alt = dijkstra_with_exclusion(m, src, crit_u, crit_v);
        
        printf("Ruta alternativa (fara %s%s%s): ", YELLOW, crit_street, NC);
        if (alt->dist[dest] == INF) {
            printf("%snu exista%s\n", RED, NC);
        } else {
            printf("%s%d metri%s\n", GREEN, alt->dist[dest], NC);
            printf("Drum: ");
            print_path_with_streets(alt, src, dest);
            printf("\n");
        }
        
        dijkstra_result_destroy(alt, m->V);
    }
    
    dijkstra_result_destroy(r, m->V);
}

/* =============================================================================
 * PROGRAMUL PRINCIPAL
 * =============================================================================
 */

int main(void) {
    int N, M;
    
    /* Citim numărul de intersecții și străzi */
    if (scanf("%d %d", &N, &M) != 2) {
        fprintf(stderr, "Eroare la citirea N și M!\n");
        return 1;
    }
    
    CityMap* city = map_create(N);
    
    /* Citim străzile */
    for (int i = 0; i < M; i++) {
        int u, v, w;
        char street[MAX_STREET_NAME];
        
        if (scanf("%d %d %d %63s", &u, &v, &w, street) != 4) {
            fprintf(stderr, "Eroare la citirea străzii %d!\n", i + 1);
            map_destroy(city);
            return 1;
        }
        
        map_add_street(city, u, v, w, street);
    }
    
    /* Citim interogările */
    int Q;
    if (scanf("%d", &Q) != 1) {
        fprintf(stderr, "Eroare la citirea numărului de interogări!\n");
        map_destroy(city);
        return 1;
    }
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║           SISTEM DE NAVIGAȚIE GPS - București                 ║\n");
    printf("║        Intersecții: %-4d    Străzi: %-4d                      ║\n", N, M);
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    for (int i = 0; i < Q; i++) {
        int src, dest;
        if (scanf("%d %d", &src, &dest) != 2) {
            fprintf(stderr, "Eroare la citirea interogării %d!\n", i + 1);
            break;
        }
        
        process_query(city, src, dest, i + 1);
    }
    
    map_destroy(city);
    return 0;
}

/* =============================================================================
 * EXEMPLU DE FIȘIER DE TEST
 * =============================================================================
 * 
 * bucuresti.txt:
 * 6 8
 * 0 1 500 Calea_Victoriei
 * 0 2 800 Bd_Magheru
 * 1 2 300 Str_Academiei
 * 1 3 600 Bd_Carol
 * 2 4 400 Bd_Bratianu
 * 3 4 200 Str_Lipscani
 * 3 5 500 Bd_Unirii
 * 4 5 300 Splaiul_Independentei
 * 3
 * 0 5
 * 0 4
 * 2 3
 * 
 * =============================================================================
 */
