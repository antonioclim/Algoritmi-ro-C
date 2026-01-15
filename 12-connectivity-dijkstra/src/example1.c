/**
 * =============================================================================
 * SĂPTĂMÂNA 12: GRAFURI - NOȚIUNI DE BAZĂ
 * Exemplu Complet de Lucru
 * =============================================================================
 *
 * Acest exemplu demonstrează:
 *   1. Reprezentarea grafurilor prin matrice de adiacență
 *   2. Reprezentarea grafurilor prin liste de adiacență
 *   3. Parcurgerea BFS (Breadth-First Search)
 *   4. Parcurgerea DFS (Depth-First Search) - recursiv și iterativ
 *   5. Determinarea componentelor conexe
 *   6. Detectarea ciclurilor în grafuri
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -o example1 example1.c
 * Execuție: ./example1
 *
 * Autor: Curs ATP - ASE București
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* =============================================================================
 * CONSTANTE ȘI MACRO-URI
 * =============================================================================
 */

#define MAX_VERTICES 100
#define INFINITY 999999

/* Culori pentru output în terminal */
#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[0;32m"
#define COLOR_YELLOW  "\033[0;33m"
#define COLOR_CYAN    "\033[0;36m"
#define COLOR_RED     "\033[0;31m"
#define COLOR_MAGENTA "\033[0;35m"

/* =============================================================================
 * PARTEA 1: STRUCTURI DE DATE PENTRU GRAFURI
 * =============================================================================
 */

/**
 * Graf reprezentat prin matrice de adiacență
 * 
 * Complexitate spațială: O(V²)
 * Potrivit pentru: grafuri dense (E ≈ V²)
 */
typedef struct {
    int vertices;                           /* Numărul de noduri */
    int edges;                              /* Numărul de muchii */
    int adj[MAX_VERTICES][MAX_VERTICES];    /* Matricea de adiacență */
    bool directed;                          /* Graf orientat sau neorientat */
} GraphMatrix;

/**
 * Nod pentru lista de adiacență
 */
typedef struct AdjNode {
    int vertex;                 /* Nodul destinație */
    int weight;                 /* Ponderea muchiei (pentru grafuri ponderate) */
    struct AdjNode* next;       /* Pointer la următorul nod din listă */
} AdjNode;

/**
 * Graf reprezentat prin liste de adiacență
 * 
 * Complexitate spațială: O(V + E)
 * Potrivit pentru: grafuri rare (E << V²)
 */
typedef struct {
    int vertices;               /* Numărul de noduri */
    int edges;                  /* Numărul de muchii */
    AdjNode** adjList;          /* Array de pointeri la liste de adiacență */
    bool directed;              /* Graf orientat sau neorientat */
} GraphList;

/**
 * Structură pentru coadă (necesară pentru BFS)
 */
typedef struct {
    int items[MAX_VERTICES];
    int front;
    int rear;
} Queue;

/**
 * Structură pentru stivă (necesară pentru DFS iterativ)
 */
typedef struct {
    int items[MAX_VERTICES];
    int top;
} Stack;

/* =============================================================================
 * PARTEA 2: OPERAȚII PE COADĂ (pentru BFS)
 * =============================================================================
 */

/**
 * Inițializează o coadă goală
 */
void queue_init(Queue* q) {
    q->front = -1;
    q->rear = -1;
}

/**
 * Verifică dacă coada este goală
 */
bool queue_is_empty(Queue* q) {
    return q->front == -1;
}

/**
 * Adaugă un element în coadă (enqueue)
 */
void queue_enqueue(Queue* q, int value) {
    if (q->rear == MAX_VERTICES - 1) {
        printf("Coada este plină!\n");
        return;
    }
    if (q->front == -1) {
        q->front = 0;
    }
    q->rear++;
    q->items[q->rear] = value;
}

/**
 * Extrage un element din coadă (dequeue)
 */
int queue_dequeue(Queue* q) {
    if (queue_is_empty(q)) {
        printf("Coada este goală!\n");
        return -1;
    }
    int item = q->items[q->front];
    q->front++;
    if (q->front > q->rear) {
        q->front = q->rear = -1;
    }
    return item;
}

/* =============================================================================
 * PARTEA 3: OPERAȚII PE STIVĂ (pentru DFS iterativ)
 * =============================================================================
 */

/**
 * Inițializează o stivă goală
 */
void stack_init(Stack* s) {
    s->top = -1;
}

/**
 * Verifică dacă stiva este goală
 */
bool stack_is_empty(Stack* s) {
    return s->top == -1;
}

/**
 * Adaugă un element pe stivă (push)
 */
void stack_push(Stack* s, int value) {
    if (s->top == MAX_VERTICES - 1) {
        printf("Stiva este plină!\n");
        return;
    }
    s->top++;
    s->items[s->top] = value;
}

/**
 * Extrage un element din stivă (pop)
 */
int stack_pop(Stack* s) {
    if (stack_is_empty(s)) {
        printf("Stiva este goală!\n");
        return -1;
    }
    return s->items[s->top--];
}

/* =============================================================================
 * PARTEA 4: OPERAȚII PE GRAF CU MATRICE DE ADIACENȚĂ
 * =============================================================================
 */

/**
 * Inițializează un graf cu matrice de adiacență
 * 
 * @param g       Pointer la structura grafului
 * @param v       Numărul de noduri
 * @param directed true pentru graf orientat, false pentru neorientat
 */
void graph_matrix_init(GraphMatrix* g, int v, bool directed) {
    g->vertices = v;
    g->edges = 0;
    g->directed = directed;
    
    /* Inițializare matrice cu 0 (nicio muchie) */
    for (int i = 0; i < v; i++) {
        for (int j = 0; j < v; j++) {
            g->adj[i][j] = 0;
        }
    }
}

/**
 * Adaugă o muchie în graf (matrice de adiacență)
 * 
 * Complexitate: O(1)
 */
void graph_matrix_add_edge(GraphMatrix* g, int src, int dest) {
    if (src >= g->vertices || dest >= g->vertices || src < 0 || dest < 0) {
        printf(COLOR_RED "Eroare: noduri invalide (%d, %d)\n" COLOR_RESET, src, dest);
        return;
    }
    
    g->adj[src][dest] = 1;
    g->edges++;
    
    /* Pentru graf neorientat, adăugăm și muchia inversă */
    if (!g->directed) {
        g->adj[dest][src] = 1;
    }
}

/**
 * Verifică existența unei muchii
 * 
 * Complexitate: O(1)
 */
bool graph_matrix_has_edge(GraphMatrix* g, int src, int dest) {
    if (src >= g->vertices || dest >= g->vertices) return false;
    return g->adj[src][dest] == 1;
}

/**
 * Afișează matricea de adiacență
 */
void graph_matrix_print(GraphMatrix* g) {
    printf("\nMatrice de adiacență (%d noduri, %d muchii):\n", 
           g->vertices, g->edges);
    printf("    ");
    for (int i = 0; i < g->vertices; i++) {
        printf("%3d ", i);
    }
    printf("\n    ");
    for (int i = 0; i < g->vertices; i++) {
        printf("----");
    }
    printf("\n");
    
    for (int i = 0; i < g->vertices; i++) {
        printf("%2d| ", i);
        for (int j = 0; j < g->vertices; j++) {
            if (g->adj[i][j]) {
                printf(COLOR_GREEN "%3d " COLOR_RESET, g->adj[i][j]);
            } else {
                printf("%3d ", g->adj[i][j]);
            }
        }
        printf("\n");
    }
}

/* =============================================================================
 * PARTEA 5: OPERAȚII PE GRAF CU LISTE DE ADIACENȚĂ
 * =============================================================================
 */

/**
 * Creează un nou nod pentru lista de adiacență
 */
AdjNode* create_adj_node(int vertex, int weight) {
    AdjNode* node = (AdjNode*)malloc(sizeof(AdjNode));
    if (node == NULL) {
        printf(COLOR_RED "Eroare: alocare memorie eșuată!\n" COLOR_RESET);
        exit(EXIT_FAILURE);
    }
    node->vertex = vertex;
    node->weight = weight;
    node->next = NULL;
    return node;
}

/**
 * Inițializează un graf cu liste de adiacență
 */
GraphList* graph_list_create(int v, bool directed) {
    GraphList* g = (GraphList*)malloc(sizeof(GraphList));
    if (g == NULL) {
        printf(COLOR_RED "Eroare: alocare memorie eșuată!\n" COLOR_RESET);
        exit(EXIT_FAILURE);
    }
    
    g->vertices = v;
    g->edges = 0;
    g->directed = directed;
    
    /* Alocăm array-ul de liste */
    g->adjList = (AdjNode**)malloc(v * sizeof(AdjNode*));
    if (g->adjList == NULL) {
        printf(COLOR_RED "Eroare: alocare memorie eșuată!\n" COLOR_RESET);
        free(g);
        exit(EXIT_FAILURE);
    }
    
    /* Inițializăm toate listele ca goale */
    for (int i = 0; i < v; i++) {
        g->adjList[i] = NULL;
    }
    
    return g;
}

/**
 * Adaugă o muchie în graf (liste de adiacență)
 * 
 * Complexitate: O(1) pentru adăugare la început
 */
void graph_list_add_edge(GraphList* g, int src, int dest, int weight) {
    if (src >= g->vertices || dest >= g->vertices || src < 0 || dest < 0) {
        printf(COLOR_RED "Eroare: noduri invalide (%d, %d)\n" COLOR_RESET, src, dest);
        return;
    }
    
    /* Adăugăm dest în lista lui src */
    AdjNode* node = create_adj_node(dest, weight);
    node->next = g->adjList[src];
    g->adjList[src] = node;
    g->edges++;
    
    /* Pentru graf neorientat, adăugăm și muchia inversă */
    if (!g->directed) {
        node = create_adj_node(src, weight);
        node->next = g->adjList[dest];
        g->adjList[dest] = node;
    }
}

/**
 * Afișează listele de adiacență
 */
void graph_list_print(GraphList* g) {
    printf("\nListe de adiacență (%d noduri, %d muchii):\n", 
           g->vertices, g->edges);
    
    for (int i = 0; i < g->vertices; i++) {
        printf(COLOR_CYAN "%d" COLOR_RESET ": ", i);
        AdjNode* current = g->adjList[i];
        while (current != NULL) {
            printf(COLOR_GREEN "→ %d" COLOR_RESET, current->vertex);
            if (current->weight != 1) {
                printf("(%d)", current->weight);
            }
            current = current->next;
            if (current != NULL) printf(" ");
        }
        printf(" → NULL\n");
    }
}

/**
 * Eliberează memoria pentru graf cu liste de adiacență
 */
void graph_list_free(GraphList* g) {
    if (g == NULL) return;
    
    for (int i = 0; i < g->vertices; i++) {
        AdjNode* current = g->adjList[i];
        while (current != NULL) {
            AdjNode* temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(g->adjList);
    free(g);
}

/* =============================================================================
 * PARTEA 6: ALGORITMI DE PARCURGERE - BFS
 * =============================================================================
 */

/**
 * BFS (Breadth-First Search) pentru matrice de adiacență
 * 
 * Parcurge graful nivel cu nivel, pornind de la nodul start.
 * Complexitate: O(V²) pentru matrice de adiacență
 * 
 * @param g      Pointer la graf
 * @param start  Nodul de pornire
 */
void bfs_matrix(GraphMatrix* g, int start) {
    if (start >= g->vertices || start < 0) {
        printf(COLOR_RED "Eroare: nod de start invalid!\n" COLOR_RESET);
        return;
    }
    
    bool visited[MAX_VERTICES] = {false};
    int distance[MAX_VERTICES];
    int parent[MAX_VERTICES];
    
    /* Inițializare distanțe */
    for (int i = 0; i < g->vertices; i++) {
        distance[i] = INFINITY;
        parent[i] = -1;
    }
    
    Queue q;
    queue_init(&q);
    
    /* Marcăm nodul de start ca vizitat */
    visited[start] = true;
    distance[start] = 0;
    queue_enqueue(&q, start);
    
    printf("\n" COLOR_YELLOW "BFS pornind de la nodul %d:\n" COLOR_RESET, start);
    printf("Ordine vizitare: ");
    
    while (!queue_is_empty(&q)) {
        int current = queue_dequeue(&q);
        printf(COLOR_GREEN "%d " COLOR_RESET, current);
        
        /* Verificăm toți vecinii nodului curent */
        for (int i = 0; i < g->vertices; i++) {
            if (g->adj[current][i] && !visited[i]) {
                visited[i] = true;
                distance[i] = distance[current] + 1;
                parent[i] = current;
                queue_enqueue(&q, i);
            }
        }
    }
    
    printf("\n\nDistanțe de la nodul %d:\n", start);
    for (int i = 0; i < g->vertices; i++) {
        if (distance[i] == INFINITY) {
            printf("  Nod %d: ∞ (inaccesibil)\n", i);
        } else {
            printf("  Nod %d: %d\n", i, distance[i]);
        }
    }
}

/**
 * BFS pentru liste de adiacență
 * 
 * Complexitate: O(V + E)
 */
void bfs_list(GraphList* g, int start) {
    if (start >= g->vertices || start < 0) {
        printf(COLOR_RED "Eroare: nod de start invalid!\n" COLOR_RESET);
        return;
    }
    
    bool* visited = (bool*)calloc(g->vertices, sizeof(bool));
    int* distance = (int*)malloc(g->vertices * sizeof(int));
    
    for (int i = 0; i < g->vertices; i++) {
        distance[i] = INFINITY;
    }
    
    Queue q;
    queue_init(&q);
    
    visited[start] = true;
    distance[start] = 0;
    queue_enqueue(&q, start);
    
    printf("\n" COLOR_YELLOW "BFS (liste) pornind de la nodul %d:\n" COLOR_RESET, start);
    printf("Ordine vizitare: ");
    
    while (!queue_is_empty(&q)) {
        int current = queue_dequeue(&q);
        printf(COLOR_GREEN "%d " COLOR_RESET, current);
        
        /* Parcurgem lista de adiacență */
        AdjNode* adj = g->adjList[current];
        while (adj != NULL) {
            if (!visited[adj->vertex]) {
                visited[adj->vertex] = true;
                distance[adj->vertex] = distance[current] + 1;
                queue_enqueue(&q, adj->vertex);
            }
            adj = adj->next;
        }
    }
    
    printf("\n\nDistanțe de la nodul %d:\n", start);
    for (int i = 0; i < g->vertices; i++) {
        if (distance[i] == INFINITY) {
            printf("  Nod %d: ∞ (inaccesibil)\n", i);
        } else {
            printf("  Nod %d: %d\n", i, distance[i]);
        }
    }
    
    free(visited);
    free(distance);
}

/* =============================================================================
 * PARTEA 7: ALGORITMI DE PARCURGERE - DFS
 * =============================================================================
 */

/**
 * DFS recursiv (helper)
 */
void dfs_recursive_helper(GraphMatrix* g, int vertex, bool* visited) {
    visited[vertex] = true;
    printf(COLOR_GREEN "%d " COLOR_RESET, vertex);
    
    /* Vizităm recursiv toți vecinii nevizitați */
    for (int i = 0; i < g->vertices; i++) {
        if (g->adj[vertex][i] && !visited[i]) {
            dfs_recursive_helper(g, i, visited);
        }
    }
}

/**
 * DFS recursiv pentru matrice de adiacență
 * 
 * Complexitate: O(V²)
 */
void dfs_recursive_matrix(GraphMatrix* g, int start) {
    if (start >= g->vertices || start < 0) {
        printf(COLOR_RED "Eroare: nod de start invalid!\n" COLOR_RESET);
        return;
    }
    
    bool visited[MAX_VERTICES] = {false};
    
    printf("\n" COLOR_YELLOW "DFS recursiv pornind de la nodul %d:\n" COLOR_RESET, start);
    printf("Ordine vizitare: ");
    
    dfs_recursive_helper(g, start, visited);
    printf("\n");
}

/**
 * DFS iterativ cu stivă explicită
 * 
 * Demonstrează că recursivitatea poate fi înlocuită cu o stivă.
 * Complexitate: O(V²) pentru matrice
 */
void dfs_iterative_matrix(GraphMatrix* g, int start) {
    if (start >= g->vertices || start < 0) {
        printf(COLOR_RED "Eroare: nod de start invalid!\n" COLOR_RESET);
        return;
    }
    
    bool visited[MAX_VERTICES] = {false};
    Stack s;
    stack_init(&s);
    
    printf("\n" COLOR_YELLOW "DFS iterativ pornind de la nodul %d:\n" COLOR_RESET, start);
    printf("Ordine vizitare: ");
    
    stack_push(&s, start);
    
    while (!stack_is_empty(&s)) {
        int current = stack_pop(&s);
        
        if (!visited[current]) {
            visited[current] = true;
            printf(COLOR_GREEN "%d " COLOR_RESET, current);
            
            /* Adăugăm vecinii pe stivă (în ordine inversă pentru ordine corectă) */
            for (int i = g->vertices - 1; i >= 0; i--) {
                if (g->adj[current][i] && !visited[i]) {
                    stack_push(&s, i);
                }
            }
        }
    }
    printf("\n");
}

/**
 * DFS recursiv pentru liste de adiacență (helper)
 */
void dfs_list_helper(GraphList* g, int vertex, bool* visited) {
    visited[vertex] = true;
    printf(COLOR_GREEN "%d " COLOR_RESET, vertex);
    
    AdjNode* adj = g->adjList[vertex];
    while (adj != NULL) {
        if (!visited[adj->vertex]) {
            dfs_list_helper(g, adj->vertex, visited);
        }
        adj = adj->next;
    }
}

/**
 * DFS pentru liste de adiacență
 * 
 * Complexitate: O(V + E)
 */
void dfs_list(GraphList* g, int start) {
    if (start >= g->vertices || start < 0) {
        printf(COLOR_RED "Eroare: nod de start invalid!\n" COLOR_RESET);
        return;
    }
    
    bool* visited = (bool*)calloc(g->vertices, sizeof(bool));
    
    printf("\n" COLOR_YELLOW "DFS (liste) pornind de la nodul %d:\n" COLOR_RESET, start);
    printf("Ordine vizitare: ");
    
    dfs_list_helper(g, start, visited);
    printf("\n");
    
    free(visited);
}

/* =============================================================================
 * PARTEA 8: COMPONENTE CONEXE
 * =============================================================================
 */

/**
 * Găsește și afișează toate componentele conexe ale grafului
 * 
 * O componentă conexă este un subgraf maximal în care există
 * un drum între oricare două noduri.
 */
void find_connected_components(GraphMatrix* g) {
    bool visited[MAX_VERTICES] = {false};
    int component[MAX_VERTICES];
    int componentCount = 0;
    
    printf("\n" COLOR_YELLOW "Componente conexe:\n" COLOR_RESET);
    
    for (int i = 0; i < g->vertices; i++) {
        if (!visited[i]) {
            componentCount++;
            printf("Componenta %d: ", componentCount);
            
            /* BFS pentru a găsi toate nodurile din componentă */
            Queue q;
            queue_init(&q);
            queue_enqueue(&q, i);
            visited[i] = true;
            component[i] = componentCount;
            
            while (!queue_is_empty(&q)) {
                int current = queue_dequeue(&q);
                printf(COLOR_CYAN "%d " COLOR_RESET, current);
                
                for (int j = 0; j < g->vertices; j++) {
                    if (g->adj[current][j] && !visited[j]) {
                        visited[j] = true;
                        component[j] = componentCount;
                        queue_enqueue(&q, j);
                    }
                }
            }
            printf("\n");
        }
    }
    
    printf("\nTotal componente conexe: " COLOR_GREEN "%d\n" COLOR_RESET, componentCount);
}

/* =============================================================================
 * PARTEA 9: DETECTAREA CICLURILOR
 * =============================================================================
 */

/**
 * Helper pentru detectarea ciclurilor în graf neorientat (DFS)
 */
bool has_cycle_undirected_helper(GraphMatrix* g, int vertex, bool* visited, int parent) {
    visited[vertex] = true;
    
    for (int i = 0; i < g->vertices; i++) {
        if (g->adj[vertex][i]) {
            if (!visited[i]) {
                if (has_cycle_undirected_helper(g, i, visited, vertex)) {
                    return true;
                }
            } else if (i != parent) {
                /* Am găsit o muchie către un nod deja vizitat (nu părintele) */
                return true;
            }
        }
    }
    return false;
}

/**
 * Detectează cicluri în graf neorientat
 * 
 * Un ciclu există dacă în timpul DFS găsim o muchie
 * către un nod deja vizitat (back edge).
 */
bool has_cycle_undirected(GraphMatrix* g) {
    bool visited[MAX_VERTICES] = {false};
    
    /* Verificăm fiecare componentă conexă */
    for (int i = 0; i < g->vertices; i++) {
        if (!visited[i]) {
            if (has_cycle_undirected_helper(g, i, visited, -1)) {
                return true;
            }
        }
    }
    return false;
}

/**
 * Helper pentru detectarea ciclurilor în graf orientat
 * Folosește colorarea nodurilor: ALB=0, GRI=1, NEGRU=2
 */
bool has_cycle_directed_helper(GraphMatrix* g, int vertex, int* color) {
    color[vertex] = 1;  /* Marcăm nodul ca GRI (în curs de procesare) */
    
    for (int i = 0; i < g->vertices; i++) {
        if (g->adj[vertex][i]) {
            if (color[i] == 1) {
                /* Muchie către nod GRI = ciclu! */
                return true;
            }
            if (color[i] == 0 && has_cycle_directed_helper(g, i, color)) {
                return true;
            }
        }
    }
    
    color[vertex] = 2;  /* Marcăm nodul ca NEGRU (complet procesat) */
    return false;
}

/**
 * Detectează cicluri în graf orientat
 */
bool has_cycle_directed(GraphMatrix* g) {
    int color[MAX_VERTICES] = {0};  /* Toate nodurile încep ALB */
    
    for (int i = 0; i < g->vertices; i++) {
        if (color[i] == 0) {
            if (has_cycle_directed_helper(g, i, color)) {
                return true;
            }
        }
    }
    return false;
}

/* =============================================================================
 * PARTEA 10: CITIRE GRAF DIN FIȘIER
 * =============================================================================
 */

/**
 * Citește un graf din fișier
 * 
 * Format fișier:
 *   Prima linie: V E (număr noduri, număr muchii)
 *   Următoarele E linii: src dest (muchie de la src la dest)
 */
GraphList* graph_read_from_file(const char* filename, bool directed) {
    FILE* f = fopen(filename, "r");
    if (f == NULL) {
        printf(COLOR_RED "Eroare: nu pot deschide fișierul %s\n" COLOR_RESET, filename);
        return NULL;
    }
    
    int v, e;
    if (fscanf(f, "%d %d", &v, &e) != 2) {
        printf(COLOR_RED "Eroare: format invalid în fișier\n" COLOR_RESET);
        fclose(f);
        return NULL;
    }
    
    GraphList* g = graph_list_create(v, directed);
    
    for (int i = 0; i < e; i++) {
        int src, dest;
        if (fscanf(f, "%d %d", &src, &dest) != 2) {
            printf(COLOR_RED "Eroare la citirea muchiei %d\n" COLOR_RESET, i + 1);
            break;
        }
        graph_list_add_edge(g, src, dest, 1);
    }
    
    fclose(f);
    printf(COLOR_GREEN "Graf încărcat: %d noduri, %d muchii\n" COLOR_RESET, v, e);
    return g;
}

/* =============================================================================
 * FUNCȚII DEMONSTRATIVE
 * =============================================================================
 */

void demo_matrix_graph(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 1: GRAF CU MATRICE DE ADIACENȚĂ                   ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    /*
     * Creăm următorul graf neorientat:
     * 
     *     0 --- 1 --- 2
     *     |     |
     *     3 --- 4
     *           |
     *           5
     */
    
    GraphMatrix g;
    graph_matrix_init(&g, 6, false);
    
    graph_matrix_add_edge(&g, 0, 1);
    graph_matrix_add_edge(&g, 0, 3);
    graph_matrix_add_edge(&g, 1, 2);
    graph_matrix_add_edge(&g, 1, 4);
    graph_matrix_add_edge(&g, 3, 4);
    graph_matrix_add_edge(&g, 4, 5);
    
    graph_matrix_print(&g);
    
    printf("\nVerificare muchii:\n");
    printf("  Muchie (0,1): %s\n", graph_matrix_has_edge(&g, 0, 1) ? "DA" : "NU");
    printf("  Muchie (0,5): %s\n", graph_matrix_has_edge(&g, 0, 5) ? "DA" : "NU");
    printf("  Muchie (2,5): %s\n", graph_matrix_has_edge(&g, 2, 5) ? "DA" : "NU");
}

void demo_list_graph(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 2: GRAF CU LISTE DE ADIACENȚĂ                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    GraphList* g = graph_list_create(6, false);
    
    graph_list_add_edge(g, 0, 1, 1);
    graph_list_add_edge(g, 0, 3, 1);
    graph_list_add_edge(g, 1, 2, 1);
    graph_list_add_edge(g, 1, 4, 1);
    graph_list_add_edge(g, 3, 4, 1);
    graph_list_add_edge(g, 4, 5, 1);
    
    graph_list_print(g);
    
    graph_list_free(g);
}

void demo_bfs(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 3: PARCURGERE BFS (Breadth-First Search)          ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    printf("\nBFS parcurge graful nivel cu nivel:\n");
    printf("  - Folosește o coadă (FIFO)\n");
    printf("  - Găsește drumul cel mai scurt în grafuri neponderate\n");
    printf("  - Complexitate: O(V + E) pentru liste, O(V²) pentru matrice\n");
    
    GraphMatrix g;
    graph_matrix_init(&g, 6, false);
    graph_matrix_add_edge(&g, 0, 1);
    graph_matrix_add_edge(&g, 0, 3);
    graph_matrix_add_edge(&g, 1, 2);
    graph_matrix_add_edge(&g, 1, 4);
    graph_matrix_add_edge(&g, 3, 4);
    graph_matrix_add_edge(&g, 4, 5);
    
    bfs_matrix(&g, 0);
    
    /* Demonstrație și cu liste */
    GraphList* gl = graph_list_create(6, false);
    graph_list_add_edge(gl, 0, 1, 1);
    graph_list_add_edge(gl, 0, 3, 1);
    graph_list_add_edge(gl, 1, 2, 1);
    graph_list_add_edge(gl, 1, 4, 1);
    graph_list_add_edge(gl, 3, 4, 1);
    graph_list_add_edge(gl, 4, 5, 1);
    
    bfs_list(gl, 0);
    
    graph_list_free(gl);
}

void demo_dfs(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 4: PARCURGERE DFS (Depth-First Search)            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    printf("\nDFS explorează în adâncime înaintea lățimii:\n");
    printf("  - Folosește o stivă (LIFO) sau recursie\n");
    printf("  - Util pentru: detectarea ciclurilor, sortare topologică\n");
    printf("  - Complexitate: O(V + E) pentru liste, O(V²) pentru matrice\n");
    
    GraphMatrix g;
    graph_matrix_init(&g, 6, false);
    graph_matrix_add_edge(&g, 0, 1);
    graph_matrix_add_edge(&g, 0, 3);
    graph_matrix_add_edge(&g, 1, 2);
    graph_matrix_add_edge(&g, 1, 4);
    graph_matrix_add_edge(&g, 3, 4);
    graph_matrix_add_edge(&g, 4, 5);
    
    dfs_recursive_matrix(&g, 0);
    dfs_iterative_matrix(&g, 0);
    
    GraphList* gl = graph_list_create(6, false);
    graph_list_add_edge(gl, 0, 1, 1);
    graph_list_add_edge(gl, 0, 3, 1);
    graph_list_add_edge(gl, 1, 2, 1);
    graph_list_add_edge(gl, 1, 4, 1);
    graph_list_add_edge(gl, 3, 4, 1);
    graph_list_add_edge(gl, 4, 5, 1);
    
    dfs_list(gl, 0);
    
    graph_list_free(gl);
}

void demo_connected_components(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 5: COMPONENTE CONEXE                              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    printf("\nCreăm un graf cu 3 componente conexe:\n");
    printf("  Componenta 1: 0 - 1 - 2\n");
    printf("  Componenta 2: 3 - 4\n");
    printf("  Componenta 3: 5 (izolat)\n");
    
    GraphMatrix g;
    graph_matrix_init(&g, 6, false);
    
    /* Componenta 1 */
    graph_matrix_add_edge(&g, 0, 1);
    graph_matrix_add_edge(&g, 1, 2);
    
    /* Componenta 2 */
    graph_matrix_add_edge(&g, 3, 4);
    
    /* Nodul 5 rămâne izolat */
    
    graph_matrix_print(&g);
    find_connected_components(&g);
}

void demo_cycle_detection(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 6: DETECTAREA CICLURILOR                          ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    /* Graf neorientat FĂRĂ ciclu (arbore) */
    printf("\n1. Graf neorientat fără ciclu (arbore):\n");
    printf("   0 - 1 - 2\n");
    printf("       |\n");
    printf("       3\n");
    
    GraphMatrix g1;
    graph_matrix_init(&g1, 4, false);
    graph_matrix_add_edge(&g1, 0, 1);
    graph_matrix_add_edge(&g1, 1, 2);
    graph_matrix_add_edge(&g1, 1, 3);
    
    printf("   Conține ciclu: %s\n", 
           has_cycle_undirected(&g1) ? COLOR_RED "DA" COLOR_RESET : COLOR_GREEN "NU" COLOR_RESET);
    
    /* Graf neorientat CU ciclu */
    printf("\n2. Graf neorientat cu ciclu:\n");
    printf("   0 - 1\n");
    printf("   |   |\n");
    printf("   3 - 2\n");
    
    GraphMatrix g2;
    graph_matrix_init(&g2, 4, false);
    graph_matrix_add_edge(&g2, 0, 1);
    graph_matrix_add_edge(&g2, 1, 2);
    graph_matrix_add_edge(&g2, 2, 3);
    graph_matrix_add_edge(&g2, 3, 0);
    
    printf("   Conține ciclu: %s\n", 
           has_cycle_undirected(&g2) ? COLOR_RED "DA" COLOR_RESET : COLOR_GREEN "NU" COLOR_RESET);
    
    /* Graf orientat CU ciclu */
    printf("\n3. Graf orientat cu ciclu:\n");
    printf("   0 → 1 → 2\n");
    printf("   ↑       ↓\n");
    printf("   └───────┘\n");
    
    GraphMatrix g3;
    graph_matrix_init(&g3, 3, true);
    graph_matrix_add_edge(&g3, 0, 1);
    graph_matrix_add_edge(&g3, 1, 2);
    graph_matrix_add_edge(&g3, 2, 0);
    
    printf("   Conține ciclu: %s\n", 
           has_cycle_directed(&g3) ? COLOR_RED "DA" COLOR_RESET : COLOR_GREEN "NU" COLOR_RESET);
}

/* =============================================================================
 * FUNCȚIA PRINCIPALĂ
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     SĂPTĂMÂNA 12: GRAFURI - NOȚIUNI DE BAZĂ                   ║\n");
    printf("║     Exemplu Complet - Curs ATP                                ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    demo_matrix_graph();
    demo_list_graph();
    demo_bfs();
    demo_dfs();
    demo_connected_components();
    demo_cycle_detection();
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     SUMAR COMPLEXITĂȚI                                        ║\n");
    printf("╠═══════════════════════════════════════════════════════════════╣\n");
    printf("║  Operație          │ Matrice O(V²) │ Liste O(V+E)            ║\n");
    printf("╠═══════════════════════════════════════════════════════════════╣\n");
    printf("║  Adăugare muchie   │     O(1)      │    O(1)                 ║\n");
    printf("║  Verificare muchie │     O(1)      │    O(degree)            ║\n");
    printf("║  Parcurgere BFS    │     O(V²)     │    O(V+E)               ║\n");
    printf("║  Parcurgere DFS    │     O(V²)     │    O(V+E)               ║\n");
    printf("║  Spațiu            │     O(V²)     │    O(V+E)               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    printf("\n" COLOR_GREEN "✓ Exemplu finalizat cu succes!" COLOR_RESET "\n\n");
    
    return 0;
}

/* =============================================================================
 * NOTE PENTRU STUDENȚI
 * =============================================================================
 *
 * 1. CÂND SĂ FOLOSEȘTI MATRICE DE ADIACENȚĂ:
 *    - Grafuri dense (multe muchii)
 *    - Când ai nevoie de verificare rapidă O(1) a existenței unei muchii
 *    - Algoritmi care accesează frecvent matricea (Floyd-Warshall)
 *
 * 2. CÂND SĂ FOLOSEȘTI LISTE DE ADIACENȚĂ:
 *    - Grafuri rare (puține muchii)
 *    - Când parcurgerile BFS/DFS sunt frecvente
 *    - Când memoria este limitată
 *
 * 3. BFS vs DFS:
 *    - BFS: drumul cel mai scurt, nivel cu nivel, folosește coadă
 *    - DFS: explorare completă, detectare cicluri, folosește stivă/recursie
 *
 * 4. DEPANARE CU GDB:
 *    - gcc -g -o example1 example1.c
 *    - gdb ./example1
 *    - break main
 *    - run
 *    - print g.adj[0][1]
 *
 * 5. VERIFICARE MEMORIE CU VALGRIND:
 *    - valgrind --leak-check=full ./example1
 *
 * =============================================================================
 */
