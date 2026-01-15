/**
 * =============================================================================
 * EXERCIȚIU 1: Implementare Dijkstra cu Priority Queue
 * =============================================================================
 *
 * OBIECTIV:
 *   Implementați algoritmul Dijkstra pentru găsirea drumurilor minime
 *   de la un nod sursă la toate celelalte noduri, folosind un min-heap
 *   ca priority queue pentru optimizarea selectării nodului minim.
 *
 * CERINȚE:
 *   1. Completați structura MinHeap și operațiile asociate
 *   2. Implementați algoritmul Dijkstra
 *   3. Implementați reconstrucția drumului minim
 *   4. Tratați cazurile de noduri inaccesibile
 *
 * FORMAT INTRARE:
 *   Prima linie: N M S (noduri, muchii, sursă)
 *   Următoarele M linii: U V W (muchie de la U la V cu ponderea W)
 *
 * EXEMPLU INTRARE:
 *   5 7 0
 *   0 1 4
 *   0 2 3
 *   1 2 1
 *   1 3 2
 *   2 3 5
 *   2 4 6
 *   3 4 1
 *
 * OUTPUT AȘTEPTAT:
 *   Distanța de la 0 la 0: 0
 *   Distanța de la 0 la 1: 4, Drum: 0 -> 1
 *   Distanța de la 0 la 2: 3, Drum: 0 -> 2
 *   Distanța de la 0 la 3: 6, Drum: 0 -> 1 -> 3
 *   Distanța de la 0 la 4: 7, Drum: 0 -> 1 -> 3 -> 4
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -o exercise1 exercise1.c
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
#define INF INT_MAX

/* =============================================================================
 * DEFINIȚII DE TIPURI
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
 * Structură pentru graf (liste de adiacență)
 */
typedef struct {
    int num_vertices;
    int num_edges;
    Edge** adj_list;
} Graph;

/**
 * TODO 1: Definiți structura HeapNode
 *
 * Un nod din heap trebuie să conțină:
 *   - vertex: identificatorul nodului din graf
 *   - distance: distanța curentă de la sursă
 *
 * Hint: Ambele câmpuri sunt de tip int
 */
typedef struct {
    /* CODUL TĂU AICI */
    int vertex;
    int distance;
} HeapNode;

/**
 * TODO 2: Definiți structura MinHeap (Priority Queue)
 *
 * Heap-ul trebuie să conțină:
 *   - nodes: array de HeapNode
 *   - positions: array care mapează vertex -> poziția în heap
 *   - size: numărul curent de elemente
 *   - capacity: capacitatea maximă
 *
 * Hint: positions[v] returnează indexul nodului v în array-ul nodes
 *       sau -1 dacă v nu este în heap
 */
typedef struct {
    /* CODUL TĂU AICI */
    HeapNode* nodes;
    int* positions;
    int size;
    int capacity;
} MinHeap;

/* =============================================================================
 * FUNCȚII PENTRU GRAF
 * =============================================================================
 */

/**
 * Creează un graf cu numărul specificat de noduri
 */
Graph* graph_create(int num_vertices) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->num_vertices = num_vertices;
    graph->num_edges = 0;
    graph->adj_list = (Edge**)calloc(num_vertices, sizeof(Edge*));
    return graph;
}

/**
 * Adaugă o muchie direcționată în graf
 */
void graph_add_edge(Graph* graph, int src, int dest, int weight) {
    Edge* new_edge = (Edge*)malloc(sizeof(Edge));
    new_edge->dest = dest;
    new_edge->weight = weight;
    new_edge->next = graph->adj_list[src];
    graph->adj_list[src] = new_edge;
    graph->num_edges++;
}

/**
 * Eliberează memoria grafului
 */
void graph_free(Graph* graph) {
    for (int i = 0; i < graph->num_vertices; i++) {
        Edge* edge = graph->adj_list[i];
        while (edge) {
            Edge* temp = edge;
            edge = edge->next;
            free(temp);
        }
    }
    free(graph->adj_list);
    free(graph);
}

/* =============================================================================
 * FUNCȚII PENTRU MIN-HEAP (PRIORITY QUEUE)
 * =============================================================================
 */

/**
 * TODO 3: Implementați funcția de creare a heap-ului
 *
 * @param capacity Capacitatea maximă a heap-ului
 * @return Pointer la heap-ul creat
 *
 * Pași:
 *   1. Alocați memoria pentru structura MinHeap
 *   2. Alocați array-ul nodes (capacity elemente)
 *   3. Alocați array-ul positions (capacity elemente)
 *   4. Inițializați size = 0
 *   5. Inițializați toate pozițiile cu -1 (nefolosite)
 */
MinHeap* heap_create(int capacity) {
    /* CODUL TĂU AICI */
    MinHeap* heap = (MinHeap*)malloc(sizeof(MinHeap));
    if (!heap) return NULL;
    
    heap->nodes = (HeapNode*)malloc(capacity * sizeof(HeapNode));
    heap->positions = (int*)malloc(capacity * sizeof(int));
    heap->size = 0;
    heap->capacity = capacity;
    
    for (int i = 0; i < capacity; i++) {
        heap->positions[i] = -1;
    }
    
    return heap;
}

/**
 * Funcție auxiliară: interschimbă două noduri în heap
 */
void heap_swap(MinHeap* heap, int i, int j) {
    /* Actualizăm array-ul positions */
    heap->positions[heap->nodes[i].vertex] = j;
    heap->positions[heap->nodes[j].vertex] = i;
    
    /* Interschimbăm nodurile */
    HeapNode temp = heap->nodes[i];
    heap->nodes[i] = heap->nodes[j];
    heap->nodes[j] = temp;
}

/**
 * TODO 4: Implementați heapify_up (urcarea unui nod)
 *
 * @param heap Pointer la heap
 * @param idx Indexul nodului de urcat
 *
 * Algoritm:
 *   while (nodul nu e rădăcină AND nodul e mai mic decât părintele):
 *       swap(nod, părinte)
 *       idx = poziția_părinte
 *
 * Hint: parent_idx = (idx - 1) / 2
 */
void heap_heapify_up(MinHeap* heap, int idx) {
    /* CODUL TĂU AICI */
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
 * TODO 5: Implementați heapify_down (coborârea unui nod)
 *
 * @param heap Pointer la heap
 * @param idx Indexul nodului de coborât
 *
 * Algoritm:
 *   smallest = idx
 *   if (copil_stâng există AND copil_stâng < smallest):
 *       smallest = copil_stâng
 *   if (copil_drept există AND copil_drept < smallest):
 *       smallest = copil_drept
 *   if (smallest != idx):
 *       swap(idx, smallest)
 *       heapify_down(smallest)  // recursiv sau iterativ
 *
 * Hint: left = 2*idx + 1, right = 2*idx + 2
 */
void heap_heapify_down(MinHeap* heap, int idx) {
    /* CODUL TĂU AICI */
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
 * TODO 6: Implementați inserarea în heap
 *
 * @param heap Pointer la heap
 * @param vertex Nodul de inserat
 * @param distance Distanța asociată
 *
 * Pași:
 *   1. Verificați dacă heap-ul nu e plin
 *   2. Adăugați nodul la sfârșitul array-ului
 *   3. Actualizați positions[vertex]
 *   4. Incrementați size
 *   5. Apelați heapify_up pentru a menține proprietatea de heap
 */
void heap_insert(MinHeap* heap, int vertex, int distance) {
    /* CODUL TĂU AICI */
    if (heap->size >= heap->capacity) {
        return;
    }
    
    int idx = heap->size;
    heap->nodes[idx].vertex = vertex;
    heap->nodes[idx].distance = distance;
    heap->positions[vertex] = idx;
    heap->size++;
    
    heap_heapify_up(heap, idx);
}

/**
 * Verifică dacă heap-ul este gol
 */
bool heap_is_empty(MinHeap* heap) {
    return heap->size == 0;
}

/**
 * Verifică dacă un nod este în heap
 */
bool heap_contains(MinHeap* heap, int vertex) {
    return heap->positions[vertex] != -1;
}

/**
 * TODO 7: Implementați extragerea minimului
 *
 * @param heap Pointer la heap
 * @return HeapNode cu distanța minimă (sau vertex=-1 dacă heap gol)
 *
 * Pași:
 *   1. Verificați dacă heap-ul nu e gol
 *   2. Salvați rădăcina (minimul)
 *   3. Marcați positions[min.vertex] = -1
 *   4. Mutați ultimul element în rădăcină
 *   5. Decrementați size
 *   6. Dacă heap-ul nu e gol, apelați heapify_down(0)
 *   7. Returnați minimul salvat
 */
HeapNode heap_extract_min(MinHeap* heap) {
    /* CODUL TĂU AICI */
    HeapNode empty = {-1, INF};
    if (heap_is_empty(heap)) {
        return empty;
    }
    
    HeapNode min = heap->nodes[0];
    heap->positions[min.vertex] = -1;
    
    heap->size--;
    if (heap->size > 0) {
        heap->nodes[0] = heap->nodes[heap->size];
        heap->positions[heap->nodes[0].vertex] = 0;
        heap_heapify_down(heap, 0);
    }
    
    return min;
}

/**
 * TODO 8: Implementați decrease_key (actualizarea distanței)
 *
 * @param heap Pointer la heap
 * @param vertex Nodul de actualizat
 * @param new_distance Noua distanță (mai mică)
 *
 * Pași:
 *   1. Găsiți poziția nodului: idx = positions[vertex]
 *   2. Verificați dacă nodul e în heap și noua distanță e mai mică
 *   3. Actualizați distanța
 *   4. Apelați heapify_up pentru a restabili proprietatea de heap
 */
void heap_decrease_key(MinHeap* heap, int vertex, int new_distance) {
    /* CODUL TĂU AICI */
    int idx = heap->positions[vertex];
    if (idx == -1 || new_distance >= heap->nodes[idx].distance) {
        return;
    }
    
    heap->nodes[idx].distance = new_distance;
    heap_heapify_up(heap, idx);
}

/**
 * Eliberează memoria heap-ului
 */
void heap_free(MinHeap* heap) {
    free(heap->nodes);
    free(heap->positions);
    free(heap);
}

/* =============================================================================
 * ALGORITMUL DIJKSTRA
 * =============================================================================
 */

/**
 * TODO 9: Implementați algoritmul Dijkstra
 *
 * @param graph Graful de intrare
 * @param source Nodul sursă
 * @param distances Array pentru stocarea distanțelor (alocat de apelant)
 * @param predecessors Array pentru stocarea predecesorilor (alocat de apelant)
 *
 * Algoritm:
 *   1. Inițializați distances[s] = 0, distances[v] = INF pentru v ≠ s
 *   2. Inițializați predecessors[v] = -1 pentru toți v
 *   3. Inserați toate nodurile în heap cu distanțele inițiale
 *   4. Cât timp heap-ul nu e gol:
 *      a. Extrageți nodul u cu distanța minimă
 *      b. Dacă dist[u] == INF, opriți (nodurile rămase sunt inaccesibile)
 *      c. Pentru fiecare vecin v al lui u:
 *         - Dacă dist[u] + weight(u,v) < dist[v]:
 *           - Actualizați dist[v] = dist[u] + weight(u,v)
 *           - Actualizați pred[v] = u
 *           - Apelați decrease_key pentru v
 */
void dijkstra(Graph* graph, int source, int* distances, int* predecessors) {
    int V = graph->num_vertices;
    
    /* TODO 9a: Inițializați distanțele și predecesorii */
    /* CODUL TĂU AICI */
    for (int i = 0; i < V; i++) {
        distances[i] = INF;
        predecessors[i] = -1;
    }
    distances[source] = 0;
    
    /* TODO 9b: Creați heap-ul și inserați toate nodurile */
    /* CODUL TĂU AICI */
    MinHeap* heap = heap_create(V);
    for (int i = 0; i < V; i++) {
        heap_insert(heap, i, distances[i]);
    }
    
    /* TODO 9c: Bucla principală Dijkstra */
    /* CODUL TĂU AICI */
    while (!heap_is_empty(heap)) {
        HeapNode min_node = heap_extract_min(heap);
        int u = min_node.vertex;
        
        if (min_node.distance == INF) {
            break;
        }
        
        Edge* edge = graph->adj_list[u];
        while (edge) {
            int v = edge->dest;
            int w = edge->weight;
            
            if (distances[u] != INF && distances[u] + w < distances[v]) {
                distances[v] = distances[u] + w;
                predecessors[v] = u;
                
                if (heap_contains(heap, v)) {
                    heap_decrease_key(heap, v, distances[v]);
                }
            }
            
            edge = edge->next;
        }
    }
    
    /* TODO 9d: Eliberați heap-ul */
    /* CODUL TĂU AICI */
    heap_free(heap);
}

/* =============================================================================
 * FUNCȚII DE AFIȘARE
 * =============================================================================
 */

/**
 * TODO 10: Implementați afișarea drumului de la sursă la destinație
 *
 * @param source Nodul sursă
 * @param dest Nodul destinație
 * @param predecessors Array-ul de predecesori
 *
 * Algoritm:
 *   1. Construiți drumul parcurgând predecesorii de la dest la source
 *   2. Afișați drumul în ordine inversă (de la source la dest)
 *
 * Hint: Folosiți un array pentru a stoca nodurile drumului
 */
void print_path(int source, int dest, int* predecessors) {
    (void)source;  /* Parametru disponibil pentru extensii viitoare */
    /* CODUL TĂU AICI */
    int path[MAX_VERTICES];
    int path_len = 0;
    int current = dest;
    
    while (current != -1) {
        path[path_len++] = current;
        current = predecessors[current];
    }
    
    printf("Drum: ");
    for (int i = path_len - 1; i >= 0; i--) {
        printf("%d", path[i]);
        if (i > 0) printf(" -> ");
    }
}

/**
 * Afișează toate rezultatele
 */
void print_results(int source, int num_vertices, int* distances, int* predecessors) {
    for (int i = 0; i < num_vertices; i++) {
        printf("Distanța de la %d la %d: ", source, i);
        
        if (distances[i] == INF) {
            printf("INFINIT (inaccesibil)\n");
        } else {
            printf("%d", distances[i]);
            if (i != source) {
                printf(", ");
                print_path(source, i, predecessors);
            }
            printf("\n");
        }
    }
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
    if (N <= 0 || N > MAX_VERTICES || M < 0 || S < 0 || S >= N) {
        fprintf(stderr, "Eroare: Parametri invalizi\n");
        return EXIT_FAILURE;
    }
    
    /* Creăm graful */
    Graph* graph = graph_create(N);
    
    /* Citim muchiile */
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
            fprintf(stderr, "Avertisment: Dijkstra nu suportă muchii negative (%d)\n", w);
        }
        
        graph_add_edge(graph, u, v, w);
    }
    
    /* Alocăm array-urile pentru rezultate */
    int* distances = (int*)malloc(N * sizeof(int));
    int* predecessors = (int*)malloc(N * sizeof(int));
    
    /* Rulăm Dijkstra */
    dijkstra(graph, S, distances, predecessors);
    
    /* Afișăm rezultatele */
    print_results(S, N, distances, predecessors);
    
    /* Eliberăm memoria */
    free(distances);
    free(predecessors);
    graph_free(graph);
    
    return EXIT_SUCCESS;
}

/* =============================================================================
 * PROVOCĂRI BONUS (Opțional)
 * =============================================================================
 *
 * 1. [+5p] Adăugați suport pentru grafuri nedirecționate (muchii bidirecționale)
 *
 * 2. [+5p] Implementați o versiune care citește numele nodurilor (string-uri)
 *    în loc de indici numerici
 *
 * 3. [+10p] Implementați A* (A-star) ca extensie a lui Dijkstra cu euristică
 *    pentru navigație 2D (folosiți distanța Manhattan sau Euclidiană)
 *
 * 4. [+5p] Adăugați măsurarea timpului de execuție și afișați statistici
 *    despre numărul de operații heap
 *
 * =============================================================================
 */
