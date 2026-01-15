/**
 * =============================================================================
 * EXERCIȚIUL 2: PARCURGERE DFS ȘI DETECTAREA CICLURILOR
 * =============================================================================
 *
 * OBIECTIV:
 *   Implementați parcurgerea DFS (Depth-First Search) și folosiți-o pentru
 *   a detecta cicluri în grafuri orientate și neorientate.
 *
 * CERINȚE:
 *   1. Implementați funcțiile pentru stivă (stack)
 *   2. Implementați DFS recursiv
 *   3. Implementați DFS iterativ cu stivă explicită
 *   4. Implementați detectarea ciclurilor în grafuri neorientate
 *   5. Implementați detectarea ciclurilor în grafuri orientate
 *
 * EXEMPLU INPUT:
 *   Graf orientat cu 4 noduri și arcele: 0→1, 1→2, 2→3, 3→1
 *
 * OUTPUT AȘTEPTAT:
 *   DFS: 0, 1, 2, 3
 *   Graf conține ciclu: DA (1 → 2 → 3 → 1)
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -o exercise2 exercise2.c
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

/* Culori pentru noduri în algoritmul de detectare cicluri (graf orientat) */
#define WHITE 0  /* Nod nevizitat */
#define GRAY  1  /* Nod în curs de procesare (pe stiva de recursie) */
#define BLACK 2  /* Nod complet procesat */

/* =============================================================================
 * STRUCTURI DE DATE
 * =============================================================================
 */

/**
 * Structură pentru stivă (necesară pentru DFS iterativ)
 */
typedef struct {
    int items[MAX_VERTICES];
    int top;
} Stack;

/**
 * Structură pentru graf cu matrice de adiacență
 */
typedef struct {
    int vertices;
    bool directed;
    int adj[MAX_VERTICES][MAX_VERTICES];
} Graph;

/* =============================================================================
 * TODO 1: IMPLEMENTAREA STIVEI
 * =============================================================================
 *
 * Stiva funcționează pe principiul LIFO (Last In, First Out).
 * Este fundamentul pentru DFS iterativ.
 */

/**
 * Inițializează stiva (setează top la -1)
 */
void stack_init(Stack* s) {
    /* TODO 1a: Inițializați top */
    /* YOUR CODE HERE */
    
}

/**
 * Verifică dacă stiva este goală
 * 
 * @return true dacă stiva este goală, false altfel
 */
bool stack_is_empty(Stack* s) {
    /* TODO 1b: Returnați true dacă stiva e goală */
    /* YOUR CODE HERE */
    
    return true; /* Replace this */
}

/**
 * Verifică dacă stiva este plină
 */
bool stack_is_full(Stack* s) {
    return s->top == MAX_VERTICES - 1;
}

/**
 * Adaugă un element pe stivă (push)
 * 
 * @param s     Pointer la stivă
 * @param value Valoarea de adăugat
 *
 * Pași:
 *   1. Verificați dacă stiva nu e plină
 *   2. Incrementați top
 *   3. Adăugați valoarea la items[top]
 */
void stack_push(Stack* s, int value) {
    /* TODO 1c: Implementați operația push */
    /* YOUR CODE HERE */
    
}

/**
 * Extrage și returnează elementul din vârful stivei (pop)
 * 
 * @param s Pointer la stivă
 * @return  Elementul extras sau -1 dacă stiva e goală
 */
int stack_pop(Stack* s) {
    /* TODO 1d: Implementați operația pop */
    /* YOUR CODE HERE */
    
    return -1; /* Replace this */
}

/**
 * Returnează elementul din vârf fără a-l extrage (peek)
 */
int stack_peek(Stack* s) {
    if (stack_is_empty(s)) return -1;
    return s->items[s->top];
}

/* =============================================================================
 * TODO 2: OPERAȚII PE GRAF
 * =============================================================================
 */

/**
 * Inițializează un graf
 * 
 * @param g        Pointer la graf
 * @param n        Numărul de noduri
 * @param directed true pentru graf orientat, false pentru neorientat
 */
void graph_init(Graph* g, int n, bool directed) {
    /* TODO 2: Inițializați graful */
    /* - Setați vertices = n
     * - Setați directed
     * - Inițializați matricea cu 0
     */
    /* YOUR CODE HERE */
    
}

/**
 * Adaugă o muchie/arc în graf
 */
void graph_add_edge(Graph* g, int u, int v) {
    if (u >= 0 && u < g->vertices && v >= 0 && v < g->vertices) {
        g->adj[u][v] = 1;
        if (!g->directed) {
            g->adj[v][u] = 1;
        }
    }
}

/**
 * Afișează graful
 */
void graph_print(Graph* g) {
    printf("Graf %s cu %d noduri:\n", 
           g->directed ? "orientat" : "neorientat", g->vertices);
    printf("    ");
    for (int i = 0; i < g->vertices; i++) printf("%3d ", i);
    printf("\n    ");
    for (int i = 0; i < g->vertices; i++) printf("----");
    printf("\n");
    
    for (int i = 0; i < g->vertices; i++) {
        printf("%2d| ", i);
        for (int j = 0; j < g->vertices; j++) {
            printf("%3d ", g->adj[i][j]);
        }
        printf("\n");
    }
}

/* =============================================================================
 * TODO 3: DFS RECURSIV
 * =============================================================================
 *
 * DFS explorează în adâncime înainte de a se întoarce.
 * Recursivitatea folosește implicit stiva de apeluri a sistemului.
 */

/**
 * Funcție helper pentru DFS recursiv
 * 
 * @param g       Pointer la graf
 * @param vertex  Nodul curent
 * @param visited Array de noduri vizitate
 *
 * Algoritm:
 *   1. Marcați vertex ca vizitat
 *   2. Afișați/procesați vertex
 *   3. Pentru fiecare vecin nevizitat, apelați recursiv dfs_helper
 */
void dfs_recursive_helper(Graph* g, int vertex, bool* visited) {
    /* TODO 3a: Marcați nodul ca vizitat și afișați-l */
    /* YOUR CODE HERE */
    
    
    /* TODO 3b: Vizitați recursiv toți vecinii nevizitați */
    /* Hint: Parcurgeți linia vertex din matricea de adiacență
     * și pentru fiecare vecin i cu adj[vertex][i] == 1 și visited[i] == false,
     * apelați recursiv dfs_recursive_helper
     */
    /* YOUR CODE HERE */
    
}

/**
 * DFS recursiv pornind de la un nod
 * 
 * @param g     Pointer la graf
 * @param start Nodul de pornire
 */
void dfs_recursive(Graph* g, int start) {
    printf("DFS Recursiv de la nodul %d: ", start);
    
    /* TODO 3c: Creați și inițializați array-ul visited */
    bool visited[MAX_VERTICES];
    /* YOUR CODE HERE */
    
    
    /* Apelăm funcția helper */
    dfs_recursive_helper(g, start, visited);
    printf("\n");
}

/* =============================================================================
 * TODO 4: DFS ITERATIV
 * =============================================================================
 *
 * Versiunea iterativă folosește o stivă explicită în loc de recursie.
 * Aceasta demonstrează echivalența dintre recursie și stivă.
 */

/**
 * DFS iterativ cu stivă explicită
 * 
 * @param g     Pointer la graf
 * @param start Nodul de pornire
 *
 * Algoritm:
 *   1. Creați o stivă și adăugați nodul de start
 *   2. Cât timp stiva nu e goală:
 *      a. Extrageți un nod din stivă
 *      b. Dacă nu e vizitat:
 *         - Marcați-l ca vizitat și procesați-l
 *         - Adăugați toți vecinii nevizitați pe stivă
 */
void dfs_iterative(Graph* g, int start) {
    printf("DFS Iterativ de la nodul %d: ", start);
    
    /* TODO 4a: Creați și inițializați array-ul visited și stiva */
    bool visited[MAX_VERTICES];
    Stack s;
    /* YOUR CODE HERE */
    
    
    /* TODO 4b: Adăugați nodul de start pe stivă */
    /* YOUR CODE HERE */
    
    
    /* TODO 4c: Implementați bucla principală DFS */
    /*
     * while (!stack_is_empty(&s)) {
     *     - Extrageți un nod din stivă
     *     - Dacă nu e vizitat:
     *         - Marcați-l ca vizitat și afișați-l
     *         - Pentru fiecare vecin nevizitat (de la vertices-1 la 0 pentru ordine corectă):
     *             - Adăugați-l pe stivă
     * }
     */
    /* YOUR CODE HERE */
    
    
    printf("\n");
}

/* =============================================================================
 * TODO 5: DETECTAREA CICLURILOR ÎN GRAF NEORIENTAT
 * =============================================================================
 *
 * Un ciclu există într-un graf neorientat dacă în timpul DFS găsim
 * o muchie către un nod deja vizitat (care nu este părintele curent).
 */

/**
 * Helper pentru detectarea ciclurilor în graf neorientat
 * 
 * @param g       Pointer la graf
 * @param vertex  Nodul curent
 * @param visited Array de noduri vizitate
 * @param parent  Nodul părinte (de unde am venit)
 * @return        true dacă s-a găsit un ciclu
 *
 * Algoritm:
 *   1. Marcați vertex ca vizitat
 *   2. Pentru fiecare vecin al lui vertex:
 *      a. Dacă vecinul nu e vizitat, apelați recursiv
 *      b. Dacă vecinul E vizitat și NU e părintele → CICLU!
 */
bool has_cycle_undirected_helper(Graph* g, int vertex, bool* visited, int parent) {
    /* TODO 5a: Marcați nodul ca vizitat */
    /* YOUR CODE HERE */
    
    
    /* TODO 5b: Verificați toți vecinii */
    for (int i = 0; i < g->vertices; i++) {
        if (g->adj[vertex][i]) {
            /* TODO 5c: Dacă vecinul nu e vizitat, apelați recursiv */
            /* Dacă apelul recursiv returnează true, returnați true */
            /* YOUR CODE HERE */
            
            
            /* TODO 5d: Dacă vecinul E vizitat și NU e părintele → ciclu */
            /* YOUR CODE HERE */
            
        }
    }
    
    return false;
}

/**
 * Verifică dacă graful neorientat conține cicluri
 */
bool has_cycle_undirected(Graph* g) {
    bool visited[MAX_VERTICES];
    for (int i = 0; i < g->vertices; i++) {
        visited[i] = false;
    }
    
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

/* =============================================================================
 * TODO 6: DETECTAREA CICLURILOR ÎN GRAF ORIENTAT
 * =============================================================================
 *
 * Pentru grafuri orientate, folosim colorarea nodurilor:
 * - WHITE (0): nod nevizitat
 * - GRAY (1): nod în curs de procesare (pe stiva de recursie)
 * - BLACK (2): nod complet procesat
 *
 * Un ciclu există dacă găsim o muchie către un nod GRAY (back edge).
 */

/**
 * Helper pentru detectarea ciclurilor în graf orientat
 * 
 * @param g      Pointer la graf
 * @param vertex Nodul curent
 * @param color  Array de culori pentru noduri
 * @return       true dacă s-a găsit un ciclu
 */
bool has_cycle_directed_helper(Graph* g, int vertex, int* color) {
    /* TODO 6a: Marcați nodul ca GRAY (în procesare) */
    /* YOUR CODE HERE */
    
    
    /* TODO 6b: Verificați toți succesorii */
    for (int i = 0; i < g->vertices; i++) {
        if (g->adj[vertex][i]) {
            /* TODO 6c: Dacă succesorul e GRAY → ciclu! */
            /* YOUR CODE HERE */
            
            
            /* TODO 6d: Dacă succesorul e WHITE, apelați recursiv */
            /* YOUR CODE HERE */
            
        }
    }
    
    /* TODO 6e: Marcați nodul ca BLACK (complet procesat) */
    /* YOUR CODE HERE */
    
    
    return false;
}

/**
 * Verifică dacă graful orientat conține cicluri
 */
bool has_cycle_directed(Graph* g) {
    int color[MAX_VERTICES];
    for (int i = 0; i < g->vertices; i++) {
        color[i] = WHITE;
    }
    
    for (int i = 0; i < g->vertices; i++) {
        if (color[i] == WHITE) {
            if (has_cycle_directed_helper(g, i, color)) {
                return true;
            }
        }
    }
    
    return false;
}

/* =============================================================================
 * TODO 7: FUNCȚIE WRAPPER PENTRU DETECTARE CICLURI
 * =============================================================================
 */

/**
 * Verifică dacă graful conține cicluri (orientat sau neorientat)
 */
bool has_cycle(Graph* g) {
    /* TODO 7: Apelați funcția corectă în funcție de tipul grafului */
    /* YOUR CODE HERE */
    
    return false; /* Replace this */
}

/* =============================================================================
 * FUNCȚIA PRINCIPALĂ
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     EXERCIȚIUL 2: DFS și Detectarea Ciclurilor                ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Test 1: Graf neorientat FĂRĂ ciclu (arbore) */
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("TEST 1: Graf neorientat fără ciclu (arbore)\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("    0 - 1 - 2\n");
    printf("        |\n");
    printf("        3\n\n");
    
    Graph g1;
    graph_init(&g1, 4, false);
    graph_add_edge(&g1, 0, 1);
    graph_add_edge(&g1, 1, 2);
    graph_add_edge(&g1, 1, 3);
    
    graph_print(&g1);
    printf("\n");
    
    dfs_recursive(&g1, 0);
    dfs_iterative(&g1, 0);
    
    printf("\nConține ciclu: %s\n\n", 
           has_cycle(&g1) ? "DA ❌" : "NU ✓");
    
    /* Test 2: Graf neorientat CU ciclu */
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("TEST 2: Graf neorientat cu ciclu\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("    0 - 1\n");
    printf("    |   |\n");
    printf("    3 - 2\n\n");
    
    Graph g2;
    graph_init(&g2, 4, false);
    graph_add_edge(&g2, 0, 1);
    graph_add_edge(&g2, 1, 2);
    graph_add_edge(&g2, 2, 3);
    graph_add_edge(&g2, 3, 0);
    
    graph_print(&g2);
    printf("\n");
    
    dfs_recursive(&g2, 0);
    dfs_iterative(&g2, 0);
    
    printf("\nConține ciclu: %s\n\n", 
           has_cycle(&g2) ? "DA ❌" : "NU ✓");
    
    /* Test 3: Graf orientat CU ciclu */
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("TEST 3: Graf orientat cu ciclu\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("    0 → 1 → 2\n");
    printf("        ↑   ↓\n");
    printf("        └── 3\n\n");
    
    Graph g3;
    graph_init(&g3, 4, true);
    graph_add_edge(&g3, 0, 1);
    graph_add_edge(&g3, 1, 2);
    graph_add_edge(&g3, 2, 3);
    graph_add_edge(&g3, 3, 1);
    
    graph_print(&g3);
    printf("\n");
    
    dfs_recursive(&g3, 0);
    dfs_iterative(&g3, 0);
    
    printf("\nConține ciclu: %s\n\n", 
           has_cycle(&g3) ? "DA ❌" : "NU ✓");
    
    /* Test 4: Graf orientat FĂRĂ ciclu (DAG) */
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("TEST 4: Graf orientat fără ciclu (DAG)\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("    0 → 1 → 3\n");
    printf("    ↓   ↓\n");
    printf("    2 → 4\n\n");
    
    Graph g4;
    graph_init(&g4, 5, true);
    graph_add_edge(&g4, 0, 1);
    graph_add_edge(&g4, 0, 2);
    graph_add_edge(&g4, 1, 3);
    graph_add_edge(&g4, 1, 4);
    graph_add_edge(&g4, 2, 4);
    
    graph_print(&g4);
    printf("\n");
    
    dfs_recursive(&g4, 0);
    dfs_iterative(&g4, 0);
    
    printf("\nConține ciclu: %s\n\n", 
           has_cycle(&g4) ? "DA ❌" : "NU ✓");
    
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("REZULTATE AȘTEPTATE:\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("  Test 1: NU conține ciclu\n");
    printf("  Test 2: DA conține ciclu\n");
    printf("  Test 3: DA conține ciclu\n");
    printf("  Test 4: NU conține ciclu\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    return 0;
}

/* =============================================================================
 * BONUS CHALLENGES (Opțional)
 * =============================================================================
 *
 * 1. Modificați funcția de detectare a ciclurilor pentru a afișa nodurile
 *    care formează ciclul găsit.
 *
 * 2. Implementați sortarea topologică folosind DFS (pentru DAG-uri).
 *
 * 3. Implementați o funcție care numără câte cicluri distincte există
 *    în graf.
 *
 * 4. Implementați clasificarea muchiilor în: tree edges, back edges,
 *    forward edges, și cross edges.
 *
 * =============================================================================
 */
