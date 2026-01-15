/**
 * =============================================================================
 * SOLUȚIE EXERCIȚIUL 2: PARCURGERE DFS ȘI DETECTAREA CICLURILOR
 * =============================================================================
 *
 * Aceasta este soluția completă pentru Exercițiul 2.
 * NU distribuiți studenților înainte de termenul de predare!
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -o exercise2_sol exercise2_sol.c
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
 * SOLUȚIE TODO 1: IMPLEMENTAREA STIVEI
 * =============================================================================
 */

/**
 * Inițializează stiva (setează top la -1)
 */
void stack_init(Stack* s) {
    /* SOLUȚIE TODO 1a */
    s->top = -1;
}

/**
 * Verifică dacă stiva este goală
 */
bool stack_is_empty(Stack* s) {
    /* SOLUȚIE TODO 1b */
    return s->top == -1;
}

/**
 * Verifică dacă stiva este plină
 */
bool stack_is_full(Stack* s) {
    return s->top == MAX_VERTICES - 1;
}

/**
 * Adaugă un element pe stivă (push)
 */
void stack_push(Stack* s, int value) {
    /* SOLUȚIE TODO 1c */
    if (!stack_is_full(s)) {
        s->top++;
        s->items[s->top] = value;
    }
}

/**
 * Extrage și returnează elementul din vârful stivei (pop)
 */
int stack_pop(Stack* s) {
    /* SOLUȚIE TODO 1d */
    if (stack_is_empty(s)) {
        return -1;
    }
    int item = s->items[s->top];
    s->top--;
    return item;
}

/**
 * Returnează elementul din vârf fără a-l extrage (peek)
 */
int stack_peek(Stack* s) {
    if (stack_is_empty(s)) return -1;
    return s->items[s->top];
}

/* =============================================================================
 * SOLUȚIE TODO 2: OPERAȚII PE GRAF
 * =============================================================================
 */

/**
 * Inițializează un graf
 */
void graph_init(Graph* g, int n, bool directed) {
    /* SOLUȚIE TODO 2 */
    g->vertices = n;
    g->directed = directed;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            g->adj[i][j] = 0;
        }
    }
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
 * SOLUȚIE TODO 3: DFS RECURSIV
 * =============================================================================
 */

/**
 * Funcție helper pentru DFS recursiv
 */
void dfs_recursive_helper(Graph* g, int vertex, bool* visited) {
    /* SOLUȚIE TODO 3a: Marcăm nodul și afișăm */
    visited[vertex] = true;
    printf("%d ", vertex);
    
    /* SOLUȚIE TODO 3b: Vizităm recursiv vecinii */
    for (int i = 0; i < g->vertices; i++) {
        if (g->adj[vertex][i] && !visited[i]) {
            dfs_recursive_helper(g, i, visited);
        }
    }
}

/**
 * DFS recursiv pornind de la un nod
 */
void dfs_recursive(Graph* g, int start) {
    printf("DFS Recursiv de la nodul %d: ", start);
    
    /* SOLUȚIE TODO 3c: Inițializare visited */
    bool visited[MAX_VERTICES];
    for (int i = 0; i < g->vertices; i++) {
        visited[i] = false;
    }
    
    dfs_recursive_helper(g, start, visited);
    printf("\n");
}

/* =============================================================================
 * SOLUȚIE TODO 4: DFS ITERATIV
 * =============================================================================
 */

/**
 * DFS iterativ cu stivă explicită
 */
void dfs_iterative(Graph* g, int start) {
    printf("DFS Iterativ de la nodul %d: ", start);
    
    /* SOLUȚIE TODO 4a: Inițializare */
    bool visited[MAX_VERTICES];
    Stack s;
    
    for (int i = 0; i < g->vertices; i++) {
        visited[i] = false;
    }
    stack_init(&s);
    
    /* SOLUȚIE TODO 4b: Adăugare nod start */
    stack_push(&s, start);
    
    /* SOLUȚIE TODO 4c: Bucla principală */
    while (!stack_is_empty(&s)) {
        int current = stack_pop(&s);
        
        if (!visited[current]) {
            visited[current] = true;
            printf("%d ", current);
            
            /* Adăugăm vecinii în ordine inversă pentru a păstra ordinea */
            for (int i = g->vertices - 1; i >= 0; i--) {
                if (g->adj[current][i] && !visited[i]) {
                    stack_push(&s, i);
                }
            }
        }
    }
    printf("\n");
}

/* =============================================================================
 * SOLUȚIE TODO 5: DETECTAREA CICLURILOR ÎN GRAF NEORIENTAT
 * =============================================================================
 */

/**
 * Helper pentru detectarea ciclurilor în graf neorientat
 */
bool has_cycle_undirected_helper(Graph* g, int vertex, bool* visited, int parent) {
    /* SOLUȚIE TODO 5a: Marcăm nodul */
    visited[vertex] = true;
    
    /* SOLUȚIE TODO 5b,c,d: Verificăm toți vecinii */
    for (int i = 0; i < g->vertices; i++) {
        if (g->adj[vertex][i]) {
            /* Dacă vecinul nu e vizitat, continuăm recursiv */
            if (!visited[i]) {
                if (has_cycle_undirected_helper(g, i, visited, vertex)) {
                    return true;
                }
            }
            /* Dacă vecinul E vizitat și NU e părintele → ciclu */
            else if (i != parent) {
                return true;
            }
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
 * SOLUȚIE TODO 6: DETECTAREA CICLURILOR ÎN GRAF ORIENTAT
 * =============================================================================
 */

/**
 * Helper pentru detectarea ciclurilor în graf orientat
 */
bool has_cycle_directed_helper(Graph* g, int vertex, int* color) {
    /* SOLUȚIE TODO 6a: Marcăm nodul ca GRAY */
    color[vertex] = GRAY;
    
    /* SOLUȚIE TODO 6b,c,d: Verificăm toți succesorii */
    for (int i = 0; i < g->vertices; i++) {
        if (g->adj[vertex][i]) {
            /* Dacă succesorul e GRAY → back edge → ciclu! */
            if (color[i] == GRAY) {
                return true;
            }
            /* Dacă succesorul e WHITE, apelăm recursiv */
            if (color[i] == WHITE) {
                if (has_cycle_directed_helper(g, i, color)) {
                    return true;
                }
            }
        }
    }
    
    /* SOLUȚIE TODO 6e: Marcăm nodul ca BLACK */
    color[vertex] = BLACK;
    
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
 * SOLUȚIE TODO 7: FUNCȚIE WRAPPER PENTRU DETECTARE CICLURI
 * =============================================================================
 */

/**
 * Verifică dacă graful conține cicluri (orientat sau neorientat)
 */
bool has_cycle(Graph* g) {
    /* SOLUȚIE TODO 7 */
    if (g->directed) {
        return has_cycle_directed(g);
    } else {
        return has_cycle_undirected(g);
    }
}

/* =============================================================================
 * FUNCȚIA PRINCIPALĂ
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     SOLUȚIE EXERCIȚIUL 2: DFS și Detectarea Ciclurilor        ║\n");
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
    
    bool cycle1 = has_cycle(&g1);
    printf("\nConține ciclu: %s %s\n\n", 
           cycle1 ? "DA" : "NU", 
           cycle1 ? "❌" : "✓");
    
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
    
    bool cycle2 = has_cycle(&g2);
    printf("\nConține ciclu: %s %s\n\n", 
           cycle2 ? "DA" : "NU",
           cycle2 ? "✓ (corect!)" : "❌");
    
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
    
    bool cycle3 = has_cycle(&g3);
    printf("\nConține ciclu: %s %s\n\n", 
           cycle3 ? "DA" : "NU",
           cycle3 ? "✓ (corect!)" : "❌");
    
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
    
    bool cycle4 = has_cycle(&g4);
    printf("\nConține ciclu: %s %s\n\n", 
           cycle4 ? "DA" : "NU",
           cycle4 ? "❌" : "✓ (corect!)");
    
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("VERIFICARE REZULTATE:\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("  Test 1: %s (așteptat: NU)\n", !cycle1 ? "✓ CORECT" : "✗ GREȘIT");
    printf("  Test 2: %s (așteptat: DA)\n",  cycle2 ? "✓ CORECT" : "✗ GREȘIT");
    printf("  Test 3: %s (așteptat: DA)\n",  cycle3 ? "✓ CORECT" : "✗ GREȘIT");
    printf("  Test 4: %s (așteptat: NU)\n", !cycle4 ? "✓ CORECT" : "✗ GREȘIT");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    int correct = !cycle1 + cycle2 + cycle3 + !cycle4;
    printf("Scor: %d/4 teste corecte\n\n", correct);
    
    return 0;
}
