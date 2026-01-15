/**
 * =============================================================================
 * SOLUȚIE EXERCIȚIU 2: Algoritmul Bellman-Ford
 * =============================================================================
 * 
 * ATENȚIE: Acest fișier este DOAR pentru instructori!
 * Nu distribuiți studenților înainte de termenul limită.
 * 
 * Această soluție demonstrează:
 *   1. Reprezentarea grafului cu listă de muchii
 *   2. Algoritmul Bellman-Ford complet
 *   3. Detectarea ciclurilor negative
 *   4. Propagarea efectelor ciclurilor cu BFS
 *   5. Găsirea și afișarea ciclului negativ (bonus)
 * 
 * Compilare: gcc -Wall -Wextra -std=c11 -o ex2_sol exercise2_sol.c
 * Utilizare: ./ex2_sol < ../tests/test2_input.txt
 * 
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>

/* =============================================================================
 * CONSTANTE ȘI CULORI ANSI
 * =============================================================================
 */

#define INF INT_MAX
#define NEG_INF INT_MIN

#define GREEN   "\033[0;32m"
#define CYAN    "\033[0;36m"
#define YELLOW  "\033[0;33m"
#define RED     "\033[0;31m"
#define MAGENTA "\033[0;35m"
#define NC      "\033[0m"

/* =============================================================================
 * STRUCTURI DE DATE
 * =============================================================================
 */

/**
 * TODO 1 (REZOLVAT): Structura Edge pentru o muchie
 */
typedef struct {
    int src;        /* Nodul sursă */
    int dest;       /* Nodul destinație */
    int weight;     /* Ponderea muchiei */
} Edge;

/**
 * TODO 2 (REZOLVAT): Structura Graph cu listă de muchii
 */
typedef struct {
    int V;          /* Numărul de noduri */
    int E;          /* Numărul de muchii */
    Edge* edges;    /* Array-ul de muchii */
} Graph;

/**
 * TODO 3 (REZOLVAT): Structura pentru rezultatele Bellman-Ford
 */
typedef struct {
    int* dist;              /* Distanțele minime */
    int* prev;              /* Predecesorii pentru reconstrucție drum */
    bool* in_neg_cycle;     /* Noduri afectate de cicluri negative */
    bool has_neg_cycle;     /* Flag global pentru ciclu negativ */
} BellmanFordResult;

/* =============================================================================
 * FUNCȚII PENTRU GRAF
 * =============================================================================
 */

/**
 * TODO 4 (REZOLVAT): Crearea grafului
 */
Graph* graph_create(int V, int E) {
    Graph* g = (Graph*)malloc(sizeof(Graph));
    if (!g) return NULL;
    
    g->V = V;
    g->E = E;
    g->edges = (Edge*)malloc(E * sizeof(Edge));
    
    if (!g->edges) {
        free(g);
        return NULL;
    }
    
    return g;
}

/**
 * TODO 5 (REZOLVAT): Adăugarea unei muchii
 */
void graph_add_edge(Graph* g, int idx, int src, int dest, int weight) {
    if (idx >= 0 && idx < g->E) {
        g->edges[idx].src = src;
        g->edges[idx].dest = dest;
        g->edges[idx].weight = weight;
    }
}

/**
 * Eliberează memoria grafului
 */
void graph_destroy(Graph* g) {
    if (g) {
        free(g->edges);
        free(g);
    }
}

/* =============================================================================
 * FUNCȚII PENTRU REZULTATE
 * =============================================================================
 */

/**
 * TODO 6 (REZOLVAT): Crearea și inițializarea structurii de rezultate
 */
BellmanFordResult* result_create(int V, int src) {
    BellmanFordResult* r = (BellmanFordResult*)malloc(sizeof(BellmanFordResult));
    if (!r) return NULL;
    
    r->dist = (int*)malloc(V * sizeof(int));
    r->prev = (int*)malloc(V * sizeof(int));
    r->in_neg_cycle = (bool*)calloc(V, sizeof(bool));
    r->has_neg_cycle = false;
    
    if (!r->dist || !r->prev || !r->in_neg_cycle) {
        free(r->dist);
        free(r->prev);
        free(r->in_neg_cycle);
        free(r);
        return NULL;
    }
    
    /* Inițializăm distanțele */
    for (int i = 0; i < V; i++) {
        r->dist[i] = INF;
        r->prev[i] = -1;
    }
    r->dist[src] = 0;
    
    return r;
}

/**
 * Eliberează memoria rezultatelor
 */
void result_destroy(BellmanFordResult* r) {
    if (r) {
        free(r->dist);
        free(r->prev);
        free(r->in_neg_cycle);
        free(r);
    }
}

/* =============================================================================
 * ALGORITMUL BELLMAN-FORD
 * =============================================================================
 */

/**
 * TODO 7 (REZOLVAT): Faza de relaxare (V-1 iterații)
 * 
 * Relaxăm toate muchiile de V-1 ori.
 * După V-1 iterații, avem distanțele minime (dacă nu există ciclu negativ).
 */
static void bellman_ford_relax(Graph* g, BellmanFordResult* r) {
    int V = g->V;
    int E = g->E;
    
    /* V-1 iterații de relaxare */
    for (int i = 0; i < V - 1; i++) {
        bool any_update = false;
        
        /* Parcurgem toate muchiile */
        for (int j = 0; j < E; j++) {
            int u = g->edges[j].src;
            int v = g->edges[j].dest;
            int w = g->edges[j].weight;
            
            /* Relaxăm muchia dacă găsim drum mai scurt */
            if (r->dist[u] != INF && r->dist[u] + w < r->dist[v]) {
                r->dist[v] = r->dist[u] + w;
                r->prev[v] = u;
                any_update = true;
            }
        }
        
        /* Optimizare: dacă nu s-a făcut nicio actualizare, am terminat */
        if (!any_update) {
            break;
        }
    }
}

/**
 * TODO 8 (REZOLVAT): Detectarea și propagarea ciclurilor negative
 * 
 * După V-1 iterații, orice îmbunătățire indică un ciclu negativ.
 * Folosim BFS pentru a marca toate nodurile afectate.
 */
static void bellman_ford_detect_negative_cycles(Graph* g, BellmanFordResult* r) {
    int V = g->V;
    int E = g->E;
    
    /* Coada pentru BFS */
    int* queue = (int*)malloc(V * sizeof(int));
    bool* in_queue = (bool*)calloc(V, sizeof(bool));
    int front = 0, rear = 0;
    
    /* Pasul 1: Găsim nodurile direct afectate de ciclul negativ */
    for (int j = 0; j < E; j++) {
        int u = g->edges[j].src;
        int v = g->edges[j].dest;
        int w = g->edges[j].weight;
        
        /* Dacă încă putem relaxa, avem ciclu negativ */
        if (r->dist[u] != INF && r->dist[u] != NEG_INF &&
            r->dist[u] + w < r->dist[v]) {
            
            r->has_neg_cycle = true;
            
            if (!r->in_neg_cycle[v] && !in_queue[v]) {
                queue[rear++] = v;
                in_queue[v] = true;
            }
        }
    }
    
    /* Pasul 2: Propagăm cu BFS la toate nodurile accesibile din ciclu */
    while (front < rear) {
        int v = queue[front++];
        
        r->in_neg_cycle[v] = true;
        r->dist[v] = NEG_INF;
        
        /* Găsim toate nodurile accesibile din v */
        for (int j = 0; j < E; j++) {
            if (g->edges[j].src == v) {
                int dest = g->edges[j].dest;
                
                if (!r->in_neg_cycle[dest] && !in_queue[dest]) {
                    queue[rear++] = dest;
                    in_queue[dest] = true;
                }
            }
        }
    }
    
    free(queue);
    free(in_queue);
}

/**
 * TODO 9 (REZOLVAT): Funcția principală Bellman-Ford
 */
BellmanFordResult* bellman_ford(Graph* g, int src) {
    BellmanFordResult* r = result_create(g->V, src);
    if (!r) return NULL;
    
    /* Faza 1: Relaxare */
    bellman_ford_relax(g, r);
    
    /* Faza 2: Detectare cicluri negative */
    bellman_ford_detect_negative_cycles(g, r);
    
    return r;
}

/* =============================================================================
 * FUNCȚII DE AFIȘARE
 * =============================================================================
 */

/**
 * TODO 10 (REZOLVAT): Afișarea drumului
 */
void print_path_recursive(int* prev, int v, int src) {
    if (v == src) {
        printf("%d", src);
        return;
    }
    if (prev[v] == -1) {
        printf("?");
        return;
    }
    print_path_recursive(prev, prev[v], src);
    printf(" -> %d", v);
}

/**
 * TODO 11 (REZOLVAT): Afișarea rezultatelor complete
 */
void print_results(Graph* g, int src, BellmanFordResult* r) {
    printf("\n%s=== Rezultate Bellman-Ford ===%s\n", CYAN, NC);
    printf("Nod sursa: %d\n\n", src);
    
    /* Status ciclu negativ */
    if (r->has_neg_cycle) {
        printf("%s⚠️  S-a detectat ciclu negativ!%s\n\n", RED, NC);
    } else {
        printf("%s✓ Nu s-a detectat ciclu negativ.%s\n\n", GREEN, NC);
    }
    
    /* Distanțe */
    printf("%sDistante minime:%s\n", YELLOW, NC);
    for (int i = 0; i < g->V; i++) {
        printf("  Nod %d: distanta = ", i);
        if (r->in_neg_cycle[i]) {
            printf("%s-∞ (afectat de ciclu negativ)%s\n", RED, NC);
        } else if (r->dist[i] == INF) {
            printf("%sINF (inaccesibil)%s\n", MAGENTA, NC);
        } else {
            printf("%s%d%s\n", GREEN, r->dist[i], NC);
        }
    }
    
    /* Drumuri */
    printf("\n%sDrumuri:%s\n", YELLOW, NC);
    for (int i = 0; i < g->V; i++) {
        printf("  %d -> %d: ", src, i);
        if (r->in_neg_cycle[i]) {
            printf("%s(afectat de ciclu negativ)%s\n", RED, NC);
        } else if (r->dist[i] == INF) {
            printf("%s(nu exista drum)%s\n", MAGENTA, NC);
        } else {
            print_path_recursive(r->prev, i, src);
            printf(" %s(cost: %d)%s\n", GREEN, r->dist[i], NC);
        }
    }
}

/**
 * TODO 12 BONUS (REZOLVAT): Găsirea și afișarea ciclului negativ actual
 */
void find_and_print_negative_cycle(Graph* g, BellmanFordResult* r) {
    if (!r->has_neg_cycle) {
        return;
    }
    
    printf("\n%s=== Ciclu Negativ Detectat ===%s\n", RED, NC);
    
    int V = g->V;
    int E = g->E;
    
    /* Copiem distanțele pentru a reface relaxarea */
    int* dist = (int*)malloc(V * sizeof(int));
    int* prev = (int*)malloc(V * sizeof(int));
    
    for (int i = 0; i < V; i++) {
        dist[i] = INF;
        prev[i] = -1;
    }
    
    /* Găsim un nod de start accesibil */
    int start = -1;
    for (int i = 0; i < V; i++) {
        if (!r->in_neg_cycle[i] && r->dist[i] != INF) {
            /* Acest nod e accesibil dar nu în ciclu - poate fi start */
        }
        if (r->in_neg_cycle[i]) {
            start = i;
            break;
        }
    }
    
    if (start == -1) {
        printf("  Nu s-a putut identifica ciclul exact.\n");
        free(dist);
        free(prev);
        return;
    }
    
    /* Refacem V iterații pentru a găsi un nod din ciclu */
    for (int i = 0; i < V; i++) {
        dist[i] = 0;  /* Pornim de la toate nodurile */
    }
    
    int cycle_node = -1;
    for (int i = 0; i < V; i++) {
        cycle_node = -1;
        for (int j = 0; j < E; j++) {
            int u = g->edges[j].src;
            int v = g->edges[j].dest;
            int w = g->edges[j].weight;
            
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                prev[v] = u;
                
                if (i == V - 1) {
                    cycle_node = v;
                }
            }
        }
    }
    
    if (cycle_node != -1) {
        /* Mergem înapoi V pași pentru a fi sigur că suntem în ciclu */
        for (int i = 0; i < V; i++) {
            cycle_node = prev[cycle_node];
        }
        
        /* Reconstruim ciclul */
        printf("  Ciclu: ");
        int current = cycle_node;
        do {
            printf("%d", current);
            current = prev[current];
            if (current != cycle_node) {
                printf(" -> ");
            }
        } while (current != cycle_node);
        printf(" -> %d\n", cycle_node);
        
        /* Calculăm costul ciclului */
        int cycle_cost = 0;
        current = cycle_node;
        do {
            int next = prev[current];
            /* Găsim muchia next -> current */
            for (int j = 0; j < E; j++) {
                if (g->edges[j].src == next && g->edges[j].dest == current) {
                    cycle_cost += g->edges[j].weight;
                    break;
                }
            }
            current = next;
        } while (current != cycle_node);
        
        printf("  Cost ciclu: %s%d%s (negativ!)\n", RED, cycle_cost, NC);
    }
    
    free(dist);
    free(prev);
}

/* =============================================================================
 * PROGRAMUL PRINCIPAL
 * =============================================================================
 */

int main(void) {
    int N, M, S;
    
    /* Citim parametrii */
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
    Graph* g = graph_create(N, M);
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
        
        graph_add_edge(g, i, u, v, w);
    }
    
    /* Rulăm Bellman-Ford */
    BellmanFordResult* r = bellman_ford(g, S);
    if (!r) {
        fprintf(stderr, "Eroare la rularea Bellman-Ford!\n");
        graph_destroy(g);
        return 1;
    }
    
    /* Afișăm rezultatele */
    print_results(g, S, r);
    
    /* Bonus: afișăm ciclul negativ dacă există */
    find_and_print_negative_cycle(g, r);
    
    /* Cleanup */
    result_destroy(r);
    graph_destroy(g);
    
    return 0;
}

/* =============================================================================
 * NOTE PENTRU INSTRUCTORI
 * =============================================================================
 * 
 * Puncte cheie de verificat la evaluare:
 * 
 * 1. Structuri de date corecte:
 *    - Edge cu src, dest, weight
 *    - Graph cu array de edges (nu liste de adiacență!)
 * 
 * 2. Bellman-Ford corect:
 *    - V-1 iterații de relaxare
 *    - Verificare dist[u] != INF înainte de relaxare
 *    - Detectare ciclu negativ prin iterația V
 * 
 * 3. Propagare BFS:
 *    - Toate nodurile accesibile din ciclu trebuie marcate
 *    - dist[v] = -INF pentru nodurile afectate
 * 
 * 4. Bonus (ciclu negativ):
 *    - Găsirea unui nod din ciclu prin V iterații
 *    - Backtracking pentru reconstrucție
 *    - Calculul costului ciclului
 * 
 * 5. Gestiune memorie:
 *    - Nicio scurgere (Valgrind clean)
 *    - Toate alocările verificate
 * 
 * =============================================================================
 */
