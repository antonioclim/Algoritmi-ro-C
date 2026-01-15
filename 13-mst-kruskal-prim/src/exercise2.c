/**
 * =============================================================================
 * EXERCIȚIU 2: Implementare Bellman-Ford cu Detectare Cicluri Negative
 * =============================================================================
 *
 * OBIECTIV:
 *   Implementați algoritmul Bellman-Ford pentru găsirea drumurilor minime,
 *   care suportă muchii cu ponderi negative și poate detecta cicluri negative.
 *
 * CERINȚE:
 *   1. Implementați reprezentarea grafului ca listă de muchii
 *   2. Implementați faza de relaxare (V-1 iterații)
 *   3. Implementați detectarea ciclurilor negative
 *   4. Identificați toate nodurile afectate de cicluri negative
 *   5. Afișați rezultatele cu marcaje pentru noduri cu distanță -∞
 *
 * FORMAT INTRARE:
 *   Prima linie: N M S (noduri, muchii, sursă)
 *   Următoarele M linii: U V W (muchie de la U la V cu ponderea W)
 *   Ponderile pot fi negative!
 *
 * EXEMPLU INTRARE (fără ciclu negativ):
 *   4 4 0
 *   0 1 4
 *   0 2 2
 *   1 3 -3
 *   2 3 1
 *
 * OUTPUT AȘTEPTAT:
 *   Nu există ciclu negativ.
 *   Distanța de la 0 la 0: 0
 *   Distanța de la 0 la 1: 4, Drum: 0 -> 1
 *   Distanța de la 0 la 2: 2, Drum: 0 -> 2
 *   Distanța de la 0 la 3: 1, Drum: 0 -> 1 -> 3
 *
 * EXEMPLU INTRARE (cu ciclu negativ):
 *   4 4 0
 *   0 1 2
 *   1 2 3
 *   2 3 -5
 *   3 1 -1
 *
 * OUTPUT AȘTEPTAT:
 *   ATENȚIE: Graful conține ciclu negativ!
 *   Distanța de la 0 la 0: 0
 *   Distanța de la 0 la 1: -INFINIT (afectat de ciclu negativ)
 *   Distanța de la 0 la 2: -INFINIT (afectat de ciclu negativ)
 *   Distanța de la 0 la 3: -INFINIT (afectat de ciclu negativ)
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -o exercise2 exercise2.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>

/* =============================================================================
 * CONSTANTE
 * =============================================================================
 */

#define MAX_VERTICES 1000
#define MAX_EDGES 10000
#define INF INT_MAX
#define NEG_INF INT_MIN

/* =============================================================================
 * DEFINIȚII DE TIPURI
 * =============================================================================
 */

/**
 * TODO 1: Definiți structura pentru o muchie
 *
 * O muchie trebuie să conțină:
 *   - src: nodul sursă
 *   - dest: nodul destinație
 *   - weight: ponderea muchiei (poate fi negativă!)
 */
typedef struct {
    /* CODUL TĂU AICI */
    int src;
    int dest;
    int weight;
} Edge;

/**
 * TODO 2: Definiți structura pentru graf (reprezentat ca listă de muchii)
 *
 * Graful trebuie să conțină:
 *   - num_vertices: numărul de noduri
 *   - num_edges: numărul de muchii
 *   - edges: array de muchii
 */
typedef struct {
    /* CODUL TĂU AICI */
    int num_vertices;
    int num_edges;
    Edge* edges;
} Graph;

/**
 * TODO 3: Definiți structura pentru rezultatul Bellman-Ford
 *
 * Rezultatul trebuie să conțină:
 *   - distances: array cu distanțele minime
 *   - predecessors: array cu predecesorii
 *   - has_negative_cycle: flag boolean
 *   - in_negative_cycle: array boolean - in_negative_cycle[v] = true
 *                        dacă v este afectat de un ciclu negativ
 */
typedef struct {
    /* CODUL TĂU AICI */
    int* distances;
    int* predecessors;
    bool has_negative_cycle;
    bool* in_negative_cycle;
} BellmanFordResult;

/* =============================================================================
 * FUNCȚII PENTRU GRAF
 * =============================================================================
 */

/**
 * TODO 4: Implementați crearea grafului
 *
 * @param num_vertices Numărul de noduri
 * @param max_edges Numărul maxim de muchii
 * @return Pointer la graful creat
 *
 * Pași:
 *   1. Alocați structura Graph
 *   2. Inițializați num_vertices și num_edges = 0
 *   3. Alocați array-ul de muchii
 */
Graph* graph_create(int num_vertices, int max_edges) {
    /* CODUL TĂU AICI */
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    if (!graph) return NULL;
    
    graph->num_vertices = num_vertices;
    graph->num_edges = 0;
    graph->edges = (Edge*)malloc(max_edges * sizeof(Edge));
    
    return graph;
}

/**
 * TODO 5: Implementați adăugarea unei muchii
 *
 * @param graph Pointer la graf
 * @param src Nodul sursă
 * @param dest Nodul destinație
 * @param weight Ponderea (poate fi negativă!)
 *
 * Pași:
 *   1. Setați câmpurile muchiei la poziția num_edges
 *   2. Incrementați num_edges
 */
void graph_add_edge(Graph* graph, int src, int dest, int weight) {
    /* CODUL TĂU AICI */
    graph->edges[graph->num_edges].src = src;
    graph->edges[graph->num_edges].dest = dest;
    graph->edges[graph->num_edges].weight = weight;
    graph->num_edges++;
}

/**
 * Eliberează memoria grafului
 */
void graph_free(Graph* graph) {
    if (graph) {
        free(graph->edges);
        free(graph);
    }
}

/* =============================================================================
 * FUNCȚII AUXILIARE
 * =============================================================================
 */

/**
 * Structură pentru coadă simplă (folosită în BFS)
 */
typedef struct {
    int* data;
    int front;
    int rear;
    int capacity;
} Queue;

/**
 * Creează o coadă
 */
Queue* queue_create(int capacity) {
    Queue* q = (Queue*)malloc(sizeof(Queue));
    q->data = (int*)malloc(capacity * sizeof(int));
    q->front = 0;
    q->rear = 0;
    q->capacity = capacity;
    return q;
}

/**
 * Verifică dacă coada e goală
 */
bool queue_is_empty(Queue* q) {
    return q->front == q->rear;
}

/**
 * Adaugă în coadă
 */
void queue_enqueue(Queue* q, int value) {
    q->data[q->rear++] = value;
}

/**
 * Scoate din coadă
 */
int queue_dequeue(Queue* q) {
    return q->data[q->front++];
}

/**
 * Eliberează coada
 */
void queue_free(Queue* q) {
    free(q->data);
    free(q);
}

/* =============================================================================
 * ALGORITMUL BELLMAN-FORD
 * =============================================================================
 */

/**
 * TODO 6: Implementați inițializarea rezultatului
 *
 * @param num_vertices Numărul de noduri
 * @param source Nodul sursă
 * @return Pointer la rezultatul inițializat
 *
 * Pași:
 *   1. Alocați structura BellmanFordResult
 *   2. Alocați array-urile distances, predecessors, in_negative_cycle
 *   3. Inițializați:
 *      - distances[source] = 0, distances[v] = INF pentru v ≠ source
 *      - predecessors[v] = -1 pentru toți v
 *      - in_negative_cycle[v] = false pentru toți v
 *      - has_negative_cycle = false
 */
BellmanFordResult* result_create(int num_vertices, int source) {
    /* CODUL TĂU AICI */
    BellmanFordResult* result = (BellmanFordResult*)malloc(sizeof(BellmanFordResult));
    if (!result) return NULL;
    
    result->distances = (int*)malloc(num_vertices * sizeof(int));
    result->predecessors = (int*)malloc(num_vertices * sizeof(int));
    result->in_negative_cycle = (bool*)calloc(num_vertices, sizeof(bool));
    result->has_negative_cycle = false;
    
    for (int i = 0; i < num_vertices; i++) {
        result->distances[i] = INF;
        result->predecessors[i] = -1;
    }
    result->distances[source] = 0;
    
    return result;
}

/**
 * Eliberează memoria rezultatului
 */
void result_free(BellmanFordResult* result) {
    if (result) {
        free(result->distances);
        free(result->predecessors);
        free(result->in_negative_cycle);
        free(result);
    }
}

/**
 * TODO 7: Implementați faza de relaxare (V-1 iterații)
 *
 * @param graph Graful
 * @param result Structura rezultat (va fi modificată)
 *
 * Algoritm:
 *   for i = 0 to V-2:
 *       for fiecare muchie (u, v, w):
 *           if dist[u] ≠ INF AND dist[u] + w < dist[v]:
 *               dist[v] = dist[u] + w
 *               pred[v] = u
 *
 * Optimizare opțională: dacă într-o iterație nu se face nicio
 * actualizare, algoritmul poate opri (convergență timpurie)
 */
void bellman_ford_relax(Graph* graph, BellmanFordResult* result) {
    int V = graph->num_vertices;
    int E = graph->num_edges;
    
    /* CODUL TĂU AICI */
    for (int i = 0; i < V - 1; i++) {
        bool any_update = false;
        
        for (int j = 0; j < E; j++) {
            int u = graph->edges[j].src;
            int v = graph->edges[j].dest;
            int w = graph->edges[j].weight;
            
            if (result->distances[u] != INF &&
                result->distances[u] + w < result->distances[v]) {
                result->distances[v] = result->distances[u] + w;
                result->predecessors[v] = u;
                any_update = true;
            }
        }
        
        /* Optimizare: oprire timpurie */
        if (!any_update) {
            break;
        }
    }
}

/**
 * TODO 8: Implementați detectarea ciclurilor negative
 *
 * @param graph Graful
 * @param result Structura rezultat (va fi modificată)
 *
 * Algoritm:
 *   Pentru fiecare muchie (u, v, w):
 *       if dist[u] ≠ INF AND dist[u] + w < dist[v]:
 *           has_negative_cycle = true
 *           // Nodul v este într-un ciclu negativ sau accesibil din unul
 *
 * După detectare, trebuie să marcăm TOATE nodurile afectate de cicluri.
 * Un nod este afectat dacă:
 *   - Face parte direct din ciclul negativ, SAU
 *   - Este accesibil din ciclul negativ
 */
void bellman_ford_detect_negative_cycle(Graph* graph, BellmanFordResult* result) {
    int V = graph->num_vertices;
    int E = graph->num_edges;
    
    /* TODO 8a: Găsiți toate nodurile direct în cicluri negative */
    /* CODUL TĂU AICI */
    bool* directly_affected = (bool*)calloc(V, sizeof(bool));
    
    for (int j = 0; j < E; j++) {
        int u = graph->edges[j].src;
        int v = graph->edges[j].dest;
        int w = graph->edges[j].weight;
        
        if (result->distances[u] != INF &&
            result->distances[u] + w < result->distances[v]) {
            result->has_negative_cycle = true;
            directly_affected[v] = true;
        }
    }
    
    /* TODO 8b: Folosiți BFS pentru a marca toate nodurile accesibile
     * din cele direct afectate
     * 
     * Hint: Construiți un graf auxiliar (sau folosiți lista de muchii)
     *       și faceți BFS din fiecare nod direct afectat
     */
    /* CODUL TĂU AICI */
    if (result->has_negative_cycle) {
        /* Construim liste de adiacență pentru BFS */
        int** adj = (int**)malloc(V * sizeof(int*));
        int* adj_size = (int*)calloc(V, sizeof(int));
        int* adj_cap = (int*)malloc(V * sizeof(int));
        
        for (int i = 0; i < V; i++) {
            adj_cap[i] = 10;
            adj[i] = (int*)malloc(adj_cap[i] * sizeof(int));
        }
        
        /* Populăm listele de adiacență */
        for (int j = 0; j < E; j++) {
            int u = graph->edges[j].src;
            int v = graph->edges[j].dest;
            
            if (adj_size[u] >= adj_cap[u]) {
                adj_cap[u] *= 2;
                adj[u] = (int*)realloc(adj[u], adj_cap[u] * sizeof(int));
            }
            adj[u][adj_size[u]++] = v;
        }
        
        /* BFS din nodurile direct afectate */
        Queue* q = queue_create(V);
        bool* visited = (bool*)calloc(V, sizeof(bool));
        
        for (int i = 0; i < V; i++) {
            if (directly_affected[i]) {
                queue_enqueue(q, i);
                visited[i] = true;
                result->in_negative_cycle[i] = true;
            }
        }
        
        while (!queue_is_empty(q)) {
            int u = queue_dequeue(q);
            
            for (int j = 0; j < adj_size[u]; j++) {
                int v = adj[u][j];
                if (!visited[v]) {
                    visited[v] = true;
                    result->in_negative_cycle[v] = true;
                    queue_enqueue(q, v);
                }
            }
        }
        
        /* Eliberăm memoria auxiliară */
        queue_free(q);
        free(visited);
        for (int i = 0; i < V; i++) {
            free(adj[i]);
        }
        free(adj);
        free(adj_size);
        free(adj_cap);
    }
    
    free(directly_affected);
}

/**
 * TODO 9: Implementați funcția principală Bellman-Ford
 *
 * @param graph Graful de intrare
 * @param source Nodul sursă
 * @return Pointer la structura cu rezultatele
 *
 * Pași:
 *   1. Inițializați rezultatul (result_create)
 *   2. Rulați faza de relaxare (bellman_ford_relax)
 *   3. Detectați ciclurile negative (bellman_ford_detect_negative_cycle)
 *   4. Returnați rezultatul
 */
BellmanFordResult* bellman_ford(Graph* graph, int source) {
    /* CODUL TĂU AICI */
    BellmanFordResult* result = result_create(graph->num_vertices, source);
    
    bellman_ford_relax(graph, result);
    bellman_ford_detect_negative_cycle(graph, result);
    
    return result;
}

/* =============================================================================
 * FUNCȚII DE AFIȘARE
 * =============================================================================
 */

/**
 * TODO 10: Implementați afișarea drumului
 *
 * @param source Nodul sursă
 * @param dest Nodul destinație
 * @param result Rezultatul Bellman-Ford
 *
 * Notă: Verificați dacă destinația este afectată de ciclu negativ!
 *       În acest caz, drumul nu are sens (ar fi infinit de scurt)
 */
void print_path(int source, int dest, BellmanFordResult* result) {
    (void)source;  /* Parametru disponibil pentru extensii viitoare */
    /* CODUL TĂU AICI */
    if (result->in_negative_cycle[dest]) {
        printf("(drum nedefinit - ciclu negativ)");
        return;
    }
    
    int path[MAX_VERTICES];
    int path_len = 0;
    int current = dest;
    
    while (current != -1) {
        path[path_len++] = current;
        current = result->predecessors[current];
    }
    
    printf("Drum: ");
    for (int i = path_len - 1; i >= 0; i--) {
        printf("%d", path[i]);
        if (i > 0) printf(" -> ");
    }
}

/**
 * TODO 11: Implementați afișarea rezultatelor complete
 *
 * @param source Nodul sursă
 * @param num_vertices Numărul de noduri
 * @param result Rezultatul Bellman-Ford
 *
 * Pentru fiecare nod v:
 *   - Dacă in_negative_cycle[v]: afișați "-INFINIT"
 *   - Dacă distances[v] == INF: afișați "INFINIT (inaccesibil)"
 *   - Altfel: afișați distanța și drumul
 */
void print_results(int source, int num_vertices, BellmanFordResult* result) {
    /* Afișăm mesaj despre ciclul negativ */
    if (result->has_negative_cycle) {
        printf("ATENȚIE: Graful conține ciclu negativ!\n\n");
    } else {
        printf("Nu există ciclu negativ.\n\n");
    }
    
    /* CODUL TĂU AICI */
    for (int i = 0; i < num_vertices; i++) {
        printf("Distanța de la %d la %d: ", source, i);
        
        if (result->in_negative_cycle[i]) {
            printf("-INFINIT (afectat de ciclu negativ)\n");
        } else if (result->distances[i] == INF) {
            printf("INFINIT (inaccesibil)\n");
        } else {
            printf("%d", result->distances[i]);
            if (i != source) {
                printf(", ");
                print_path(source, i, result);
            }
            printf("\n");
        }
    }
}

/* =============================================================================
 * FUNCȚII DE ANALIZĂ (BONUS)
 * =============================================================================
 */

/**
 * TODO 12 (BONUS): Implementați găsirea ciclului negativ
 *
 * @param graph Graful
 * @param result Rezultatul Bellman-Ford (cu has_negative_cycle = true)
 *
 * Această funcție ar trebui să găsească și să afișeze un ciclu negativ efectiv.
 *
 * Algoritm:
 *   1. Rulați încă V iterații de relaxare, urmărind actualizările
 *   2. Când găsiți un nod care se actualizează în a V-a iterație,
 *      urmăriți predecesorii pentru a găsi ciclul
 *   3. Afișați ciclul și suma ponderilor
 */
void find_and_print_negative_cycle(Graph* graph, BellmanFordResult* result) {
    if (!result->has_negative_cycle) {
        printf("Nu există ciclu negativ.\n");
        return;
    }
    
    int V = graph->num_vertices;
    int E = graph->num_edges;
    
    /* Copie a distanțelor pentru a detecta ciclul */
    int* temp_dist = (int*)malloc(V * sizeof(int));
    int* temp_pred = (int*)malloc(V * sizeof(int));
    
    memcpy(temp_dist, result->distances, V * sizeof(int));
    memcpy(temp_pred, result->predecessors, V * sizeof(int));
    
    /* Rulăm V iterații suplimentare pentru a găsi un nod în ciclu */
    int cycle_node = -1;
    
    for (int i = 0; i < V && cycle_node == -1; i++) {
        for (int j = 0; j < E; j++) {
            int u = graph->edges[j].src;
            int v = graph->edges[j].dest;
            int w = graph->edges[j].weight;
            
            if (temp_dist[u] != INF && temp_dist[u] + w < temp_dist[v]) {
                temp_dist[v] = temp_dist[u] + w;
                temp_pred[v] = u;
                cycle_node = v;
            }
        }
    }
    
    if (cycle_node != -1) {
        /* Mergem înapoi V pași pentru a fi siguri că suntem în ciclu */
        for (int i = 0; i < V; i++) {
            cycle_node = temp_pred[cycle_node];
        }
        
        /* Găsim și afișăm ciclul */
        printf("\nCiclul negativ găsit: ");
        int start = cycle_node;
        int sum = 0;
        
        printf("%d", cycle_node);
        cycle_node = temp_pred[cycle_node];
        
        while (cycle_node != start) {
            /* Găsim ponderea muchiei */
            for (int j = 0; j < E; j++) {
                if (graph->edges[j].dest == cycle_node) {
                    int prev = temp_pred[cycle_node];
                    if (graph->edges[j].src == prev) {
                        sum += graph->edges[j].weight;
                        break;
                    }
                }
            }
            printf(" <- %d", cycle_node);
            cycle_node = temp_pred[cycle_node];
        }
        printf(" <- %d\n", start);
        
        /* Nota: calculul sumei de mai sus este simplificat */
        printf("(Ciclul poate fi parcurs infinit pentru a reduce distanța)\n");
    }
    
    free(temp_dist);
    free(temp_pred);
}

/* =============================================================================
 * FUNCȚIA MAIN
 * =============================================================================
 */

int main(void) {
    int N, M, S;
    
    /* Citim parametrii grafului */
    if (scanf("%d %d %d", &N, &M, &S) != 3) {
        fprintf(stderr, "Eroare: Format intrare invalid\n");
        fprintf(stderr, "Format așteptat: N M S (noduri muchii sursă)\n");
        return EXIT_FAILURE;
    }
    
    /* Validăm parametrii */
    if (N <= 0 || N > MAX_VERTICES || M < 0 || M > MAX_EDGES || 
        S < 0 || S >= N) {
        fprintf(stderr, "Eroare: Parametri invalizi\n");
        return EXIT_FAILURE;
    }
    
    /* Creăm graful */
    Graph* graph = graph_create(N, M);
    if (!graph) {
        fprintf(stderr, "Eroare: Nu s-a putut aloca memoria\n");
        return EXIT_FAILURE;
    }
    
    /* Citim muchiile */
    bool has_negative = false;
    for (int i = 0; i < M; i++) {
        int u, v, w;
        if (scanf("%d %d %d", &u, &v, &w) != 3) {
            fprintf(stderr, "Eroare: Muchie invalidă la linia %d\n", i + 1);
            graph_free(graph);
            return EXIT_FAILURE;
        }
        
        if (u < 0 || u >= N || v < 0 || v >= N) {
            fprintf(stderr, "Eroare: Indici de noduri invalizi (%d, %d)\n", u, v);
            graph_free(graph);
            return EXIT_FAILURE;
        }
        
        if (w < 0) {
            has_negative = true;
        }
        
        graph_add_edge(graph, u, v, w);
    }
    
    /* Informăm despre muchiile negative */
    if (has_negative) {
        printf("Notă: Graful conține muchii cu ponderi negative.\n\n");
    }
    
    /* Rulăm Bellman-Ford */
    BellmanFordResult* result = bellman_ford(graph, S);
    
    /* Afișăm rezultatele */
    print_results(S, N, result);
    
    /* Bonus: găsim ciclul negativ dacă există */
    if (result->has_negative_cycle) {
        find_and_print_negative_cycle(graph, result);
    }
    
    /* Eliberăm memoria */
    result_free(result);
    graph_free(graph);
    
    return EXIT_SUCCESS;
}

/* =============================================================================
 * PROVOCĂRI BONUS (Opțional)
 * =============================================================================
 *
 * 1. [+5p] Implementați optimizarea SPFA (Shortest Path Faster Algorithm)
 *    care folosește o coadă în loc de iterații fixe
 *
 * 2. [+10p] Implementați detectarea tuturor ciclurilor negative din graf
 *    (nu doar unul)
 *
 * 3. [+5p] Adăugați suport pentru afișarea numărului de iterații necesare
 *    până la convergență
 *
 * 4. [+10p] Implementați algoritmul Johnson pentru All-Pairs Shortest Path
 *    care combină Bellman-Ford cu Dijkstra
 *
 * 5. [+5p] Comparați performanța cu Dijkstra pe grafuri fără muchii negative
 *    și afișați timpii de execuție
 *
 * =============================================================================
 */
