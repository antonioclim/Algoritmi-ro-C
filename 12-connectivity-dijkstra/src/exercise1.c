/**
 * =============================================================================
 * EXERCIȚIUL 1: PARCURGERE BFS ȘI GĂSIREA DRUMULUI CEL MAI SCURT
 * =============================================================================
 *
 * OBIECTIV:
 *   Implementați parcurgerea BFS (Breadth-First Search) pentru a găsi drumul
 *   cel mai scurt între două noduri într-un graf neorientat și neponderat.
 *
 * CERINȚE:
 *   1. Completați implementarea cozii (queue)
 *   2. Implementați funcția BFS care calculează distanțele
 *   3. Implementați funcția care reconstruiește drumul cel mai scurt
 *   4. Implementați funcția care afișează drumul
 *
 * EXEMPLU INPUT:
 *   Graf cu 6 noduri și muchiile:
 *   0-1, 0-2, 1-3, 2-3, 2-4, 3-5, 4-5
 *
 *   Cerere: drumul de la 0 la 5
 *
 * OUTPUT AȘTEPTAT:
 *   Drumul cel mai scurt de la 0 la 5: 0 -> 2 -> 4 -> 5
 *   Lungime: 3 muchii
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -o exercise1 exercise1.c
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
 * TODO 1: IMPLEMENTAREA COZII
 * =============================================================================
 *
 * Completați funcțiile pentru operațiile pe coadă.
 * O coadă (queue) funcționează pe principiul FIFO (First In, First Out).
 *
 * Hint: front și rear sunt -1 când coada este goală
 */

/**
 * Inițializează coada (setează front și rear la -1)
 */
void queue_init(Queue* q) {
    /* TODO 1a: Inițializați front și rear */
    /* YOUR CODE HERE */
    
}

/**
 * Verifică dacă coada este goală
 * 
 * @return true dacă coada este goală, false altfel
 *
 * Hint: Coada este goală când front == -1
 */
bool queue_is_empty(Queue* q) {
    /* TODO 1b: Returnați true dacă coada e goală */
    /* YOUR CODE HERE */
    
    return true; /* Replace this */
}

/**
 * Adaugă un element în coadă (enqueue)
 * 
 * @param q     Pointer la coadă
 * @param value Valoarea de adăugat
 *
 * Pași:
 *   1. Dacă coada e goală (front == -1), setați front = 0
 *   2. Incrementați rear
 *   3. Adăugați valoarea la items[rear]
 */
void queue_enqueue(Queue* q, int value) {
    /* TODO 1c: Implementați operația enqueue */
    /* YOUR CODE HERE */
    
}

/**
 * Extrage și returnează primul element din coadă (dequeue)
 * 
 * @param q Pointer la coadă
 * @return  Elementul extras sau -1 dacă coada e goală
 *
 * Pași:
 *   1. Salvați items[front]
 *   2. Incrementați front
 *   3. Dacă front > rear, resetați coada (front = rear = -1)
 *   4. Returnați elementul salvat
 */
int queue_dequeue(Queue* q) {
    /* TODO 1d: Implementați operația dequeue */
    /* YOUR CODE HERE */
    
    return -1; /* Replace this */
}

/* =============================================================================
 * TODO 2: INIȚIALIZAREA GRAFULUI
 * =============================================================================
 */

/**
 * Inițializează un graf cu n noduri
 * 
 * @param g Pointer la graf
 * @param n Numărul de noduri
 *
 * Pași:
 *   1. Setați g->vertices = n
 *   2. Inițializați matricea de adiacență cu 0
 */
void graph_init(Graph* g, int n) {
    /* TODO 2: Inițializați graful */
    /* YOUR CODE HERE */
    
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
 * TODO 3: IMPLEMENTAREA BFS
 * =============================================================================
 *
 * BFS (Breadth-First Search) parcurge graful nivel cu nivel.
 * Folosește o coadă pentru a vizita nodurile în ordinea corectă.
 */

/**
 * Execută BFS și calculează distanțele de la nodul sursă
 * 
 * @param g        Pointer la graf
 * @param source   Nodul de pornire
 * @param distance Array pentru stocarea distanțelor (output)
 * @param parent   Array pentru stocarea părinților (output)
 *
 * Algoritm:
 *   1. Inițializați toate distanțele cu INFINITY și părinții cu -1
 *   2. Setați distance[source] = 0
 *   3. Adăugați source în coadă
 *   4. Cât timp coada nu e goală:
 *      a. Extrageți un nod current din coadă
 *      b. Pentru fiecare vecin nevizitat al lui current:
 *         - Setați distance[vecin] = distance[current] + 1
 *         - Setați parent[vecin] = current
 *         - Adăugați vecinul în coadă
 */
void bfs(Graph* g, int source, int* distance, int* parent) {
    /* TODO 3a: Creați și inițializați un array visited */
    bool visited[MAX_VERTICES];
    for (int i = 0; i < g->vertices; i++) {
        visited[i] = false;
        distance[i] = INFINITY;
        parent[i] = -1;
    }
    
    /* TODO 3b: Creați o coadă și inițializați-o */
    /* YOUR CODE HERE */
    
    
    /* TODO 3c: Marcați nodul sursă ca vizitat și setați distanța la 0 */
    /* YOUR CODE HERE */
    
    
    /* TODO 3d: Adăugați nodul sursă în coadă */
    /* YOUR CODE HERE */
    
    
    /* TODO 3e: Implementați bucla principală BFS */
    /*
     * while (!queue_is_empty(&q)) {
     *     - Extrageți nodul curent din coadă
     *     - Pentru fiecare nod i de la 0 la vertices-1:
     *         - Dacă există muchie (current, i) și i nu e vizitat:
     *             - Marcați i ca vizitat
     *             - Setați distance[i] = distance[current] + 1
     *             - Setați parent[i] = current
     *             - Adăugați i în coadă
     * }
     */
    /* YOUR CODE HERE */
    
}

/* =============================================================================
 * TODO 4: RECONSTRUIREA DRUMULUI
 * =============================================================================
 */

/**
 * Reconstruiește drumul de la sursă la destinație
 * 
 * @param parent Array-ul de părinți calculat de BFS
 * @param dest   Nodul destinație
 * @param path   Array pentru stocarea drumului (output)
 * @return       Lungimea drumului sau -1 dacă nu există drum
 *
 * Algoritm:
 *   1. Porniți de la dest și mergeți înapoi prin părinți
 *   2. Adăugați fiecare nod în array-ul path
 *   3. Inversați array-ul (deoarece l-am construit invers)
 */
int reconstruct_path(int* parent, int source, int dest, int* path) {
    /* TODO 4a: Verificați dacă există drum (parent[dest] == -1 și dest != source) */
    /* YOUR CODE HERE */
    
    
    /* TODO 4b: Construiți drumul de la dest la source folosind părinții */
    int length = 0;
    /* YOUR CODE HERE */
    
    
    /* TODO 4c: Inversați drumul pentru ordinea corectă */
    /* Hint: Folosiți o variabilă temporară pentru a schimba elementele */
    /* YOUR CODE HERE */
    
    
    return length; /* Replace this if needed */
}

/* =============================================================================
 * TODO 5: AFIȘAREA DRUMULUI
 * =============================================================================
 */

/**
 * Afișează drumul frumos formatat
 * 
 * @param path   Array-ul cu nodurile din drum
 * @param length Lungimea drumului
 */
void print_path(int* path, int length) {
    /* TODO 5: Afișați drumul în formatul: 0 -> 1 -> 2 -> 5 */
    /* YOUR CODE HERE */
    
    printf("(Implementați afișarea drumului)\n");
}

/* =============================================================================
 * FUNCȚIA PRINCIPALĂ
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     EXERCIȚIUL 1: BFS și Drumul Cel Mai Scurt                 ║\n");
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
    
    printf("\n");
    return 0;
}

/* =============================================================================
 * BONUS CHALLENGES (Opțional)
 * =============================================================================
 *
 * 1. Modificați BFS pentru a găsi TOATE drumurile cele mai scurte între două
 *    noduri (nu doar unul).
 *
 * 2. Implementați o funcție care determină dacă graful este bipartit folosind
 *    BFS cu colorare.
 *
 * 3. Adăugați suport pentru citirea grafului din fișier.
 *
 * =============================================================================
 */
