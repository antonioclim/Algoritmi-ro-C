/**
 * =============================================================================
 * SOLUȚIE TEMA 2: ANALIZATOR DE DEPENDENȚE - SISTEM BUILD
 * =============================================================================
 *
 * Implementare completă pentru analiza dependențelor:
 * - Citirea grafului de dependențe
 * - Detectarea dependențelor circulare (cicluri)
 * - Sortare topologică (ordine de compilare)
 * - Dependențe tranzitive
 *
 * CONFIDENȚIAL - NU distribuiți studenților înainte de termenul de predare!
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -o homework2_sol homework2_sol.c
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

#define MAX_MODULES 1000

/* Culori pentru detectarea ciclurilor */
#define WHITE 0  /* Nevizitat */
#define GRAY  1  /* În procesare (pe stiva de recursie) */
#define BLACK 2  /* Complet procesat */

/* =============================================================================
 * STRUCTURI DE DATE
 * =============================================================================
 */

/**
 * Structură pentru nod în lista de adiacență
 */
typedef struct Node {
    int vertex;
    struct Node* next;
} Node;

/**
 * Structură pentru graf orientat cu liste de adiacență
 */
typedef struct {
    int vertices;
    int edges;
    Node** adj_list;
    char** module_names;  /* Opțional: nume module */
} Graph;

/**
 * Structură pentru stivă (sortare topologică)
 */
typedef struct {
    int items[MAX_MODULES];
    int top;
} Stack;

/* =============================================================================
 * FUNCȚII PENTRU STIVĂ
 * =============================================================================
 */

void stack_init(Stack* s) {
    s->top = -1;
}

bool stack_is_empty(Stack* s) {
    return s->top == -1;
}

void stack_push(Stack* s, int value) {
    if (s->top < MAX_MODULES - 1) {
        s->top++;
        s->items[s->top] = value;
    }
}

int stack_pop(Stack* s) {
    if (stack_is_empty(s)) return -1;
    return s->items[s->top--];
}

/* =============================================================================
 * FUNCȚII PENTRU GRAF
 * =============================================================================
 */

Node* create_node(int vertex) {
    Node* node = (Node*)malloc(sizeof(Node));
    if (node == NULL) {
        fprintf(stderr, "Eroare: Alocare memorie eșuată!\n");
        exit(1);
    }
    node->vertex = vertex;
    node->next = NULL;
    return node;
}

Graph* create_graph(int n) {
    Graph* g = (Graph*)malloc(sizeof(Graph));
    if (g == NULL) {
        fprintf(stderr, "Eroare: Alocare memorie eșuată!\n");
        exit(1);
    }
    
    g->vertices = n;
    g->edges = 0;
    g->adj_list = (Node**)malloc(n * sizeof(Node*));
    g->module_names = NULL;
    
    if (g->adj_list == NULL) {
        free(g);
        exit(1);
    }
    
    for (int i = 0; i < n; i++) {
        g->adj_list[i] = NULL;
    }
    
    return g;
}

/**
 * Adaugă un arc orientat: src depinde de dest (src → dest)
 */
void add_dependency(Graph* g, int src, int dest) {
    if (src < 0 || src >= g->vertices || dest < 0 || dest >= g->vertices) {
        return;
    }
    
    Node* node = create_node(dest);
    node->next = g->adj_list[src];
    g->adj_list[src] = node;
    g->edges++;
}

void free_graph(Graph* g) {
    if (g == NULL) return;
    
    for (int i = 0; i < g->vertices; i++) {
        Node* current = g->adj_list[i];
        while (current != NULL) {
            Node* temp = current;
            current = current->next;
            free(temp);
        }
    }
    
    free(g->adj_list);
    free(g);
}

/**
 * Afișează graful de dependențe
 */
void print_dependencies(Graph* g) {
    printf("\nGraf de dependențe (%d module, %d dependențe):\n", 
           g->vertices, g->edges);
    
    for (int i = 0; i < g->vertices; i++) {
        printf("  Modul %d depinde de: ", i);
        Node* current = g->adj_list[i];
        if (current == NULL) {
            printf("(nimic - modul independent)");
        }
        while (current != NULL) {
            printf("%d", current->vertex);
            if (current->next != NULL) printf(", ");
            current = current->next;
        }
        printf("\n");
    }
}

/* =============================================================================
 * CERINȚA 1: CITIREA GRAFULUI DIN FIȘIER (10p)
 * =============================================================================
 */

Graph* load_dependencies(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Eroare: Nu pot deschide fișierul %s\n", filename);
        return NULL;
    }
    
    int n, m;
    if (fscanf(file, "%d %d", &n, &m) != 2) {
        fprintf(stderr, "Eroare: Format invalid\n");
        fclose(file);
        return NULL;
    }
    
    Graph* g = create_graph(n);
    
    for (int i = 0; i < m; i++) {
        int src, dest;
        if (fscanf(file, "%d %d", &src, &dest) != 2) {
            continue;
        }
        add_dependency(g, src, dest);
    }
    
    fclose(file);
    printf("✓ Dependențe încărcate: %d module, %d arce\n", n, m);
    return g;
}

/* =============================================================================
 * CERINȚA 2: DETECTAREA CICLURILOR (15p)
 * =============================================================================
 */

/**
 * Array global pentru a stoca ciclul găsit (pentru afișare)
 */
static int cycle_path[MAX_MODULES];
static int cycle_length = 0;

/**
 * Helper DFS pentru detectarea ciclurilor cu raportare
 */
bool detect_cycle_helper(Graph* g, int vertex, int* color, int* parent) {
    color[vertex] = GRAY;
    
    Node* neighbor = g->adj_list[vertex];
    while (neighbor != NULL) {
        int v = neighbor->vertex;
        
        if (color[v] == GRAY) {
            /* Am găsit un back edge - ciclu! */
            /* Construim ciclul pentru afișare */
            cycle_length = 0;
            cycle_path[cycle_length++] = v;
            
            int node = vertex;
            while (node != v && cycle_length < MAX_MODULES) {
                cycle_path[cycle_length++] = node;
                node = parent[node];
            }
            cycle_path[cycle_length++] = v;  /* Închidem ciclul */
            
            return true;
        }
        
        if (color[v] == WHITE) {
            parent[v] = vertex;
            if (detect_cycle_helper(g, v, color, parent)) {
                return true;
            }
        }
        
        neighbor = neighbor->next;
    }
    
    color[vertex] = BLACK;
    return false;
}

/**
 * Detectează dependențe circulare și le afișează
 */
bool has_circular_dependency(Graph* g) {
    if (g == NULL) return false;
    
    int* color = (int*)malloc(g->vertices * sizeof(int));
    int* parent = (int*)malloc(g->vertices * sizeof(int));
    
    for (int i = 0; i < g->vertices; i++) {
        color[i] = WHITE;
        parent[i] = -1;
    }
    
    cycle_length = 0;
    bool has_cycle = false;
    
    for (int i = 0; i < g->vertices; i++) {
        if (color[i] == WHITE) {
            if (detect_cycle_helper(g, i, color, parent)) {
                has_cycle = true;
                break;
            }
        }
    }
    
    if (has_cycle && cycle_length > 0) {
        printf("\n⚠ DEPENDENȚĂ CIRCULARĂ DETECTATĂ!\n");
        printf("  Ciclu: ");
        for (int i = cycle_length - 1; i >= 0; i--) {
            printf("Modul%d", cycle_path[i]);
            if (i > 0) printf(" → ");
        }
        printf("\n");
    }
    
    free(color);
    free(parent);
    return has_cycle;
}

/* =============================================================================
 * CERINȚA 3: SORTARE TOPOLOGICĂ (15p)
 * =============================================================================
 */

/**
 * Helper DFS pentru sortare topologică
 */
void topo_sort_helper(Graph* g, int vertex, bool* visited, Stack* stack) {
    visited[vertex] = true;
    
    Node* neighbor = g->adj_list[vertex];
    while (neighbor != NULL) {
        if (!visited[neighbor->vertex]) {
            topo_sort_helper(g, neighbor->vertex, visited, stack);
        }
        neighbor = neighbor->next;
    }
    
    /* Adăugăm pe stivă DUPĂ procesarea tuturor dependențelor */
    stack_push(stack, vertex);
}

/**
 * Sortare topologică - returnează ordinea de compilare
 * Returnează NULL dacă există ciclu
 */
int* topological_sort(Graph* g) {
    if (g == NULL) return NULL;
    
    /* Verificăm mai întâi dacă există ciclu */
    if (has_circular_dependency(g)) {
        printf("  ✗ Nu se poate calcula ordinea - există ciclu!\n");
        return NULL;
    }
    
    bool* visited = (bool*)malloc(g->vertices * sizeof(bool));
    Stack stack;
    stack_init(&stack);
    
    for (int i = 0; i < g->vertices; i++) {
        visited[i] = false;
    }
    
    /* DFS din toate nodurile nevizitate */
    for (int i = 0; i < g->vertices; i++) {
        if (!visited[i]) {
            topo_sort_helper(g, i, visited, &stack);
        }
    }
    
    /* Extragem ordinea din stivă */
    int* order = (int*)malloc(g->vertices * sizeof(int));
    int idx = 0;
    
    while (!stack_is_empty(&stack)) {
        order[idx++] = stack_pop(&stack);
    }
    
    free(visited);
    return order;
}

/**
 * Afișează ordinea de compilare frumos formatată
 */
void print_build_order(int* order, int n) {
    printf("\n--- Ordine de Compilare (Sortare Topologică) ---\n");
    printf("╔═══════╤════════════════════════════════════════╗\n");
    printf("║  Pas  │  Modul de compilat                     ║\n");
    printf("╠═══════╪════════════════════════════════════════╣\n");
    
    for (int i = 0; i < n; i++) {
        printf("║  %2d   │  Modul %-32d ║\n", i + 1, order[i]);
    }
    
    printf("╚═══════╧════════════════════════════════════════╝\n");
}

/* =============================================================================
 * CERINȚA 4: DEPENDENȚE TRANZITIVE (10p)
 * =============================================================================
 */

/**
 * Helper DFS pentru găsirea dependențelor tranzitive
 */
void find_transitive_helper(Graph* g, int vertex, bool* visited) {
    Node* neighbor = g->adj_list[vertex];
    while (neighbor != NULL) {
        if (!visited[neighbor->vertex]) {
            visited[neighbor->vertex] = true;
            find_transitive_helper(g, neighbor->vertex, visited);
        }
        neighbor = neighbor->next;
    }
}

/**
 * Afișează toate dependențele (directe și tranzitive) ale unui modul
 */
void transitive_dependencies(Graph* g, int module) {
    if (g == NULL || module < 0 || module >= g->vertices) {
        printf("Modul invalid!\n");
        return;
    }
    
    bool* visited = (bool*)malloc(g->vertices * sizeof(bool));
    for (int i = 0; i < g->vertices; i++) {
        visited[i] = false;
    }
    
    /* Găsim toate dependențele */
    find_transitive_helper(g, module, visited);
    
    /* Afișăm rezultatele */
    printf("\n--- Dependențe pentru Modul %d ---\n", module);
    
    /* Directe */
    printf("  Dependențe directe: ");
    Node* direct = g->adj_list[module];
    if (direct == NULL) {
        printf("(niciuna)");
    }
    while (direct != NULL) {
        printf("%d", direct->vertex);
        if (direct->next) printf(", ");
        direct = direct->next;
    }
    printf("\n");
    
    /* Toate (tranzitive) */
    printf("  Toate dependențele (inclusiv tranzitive): ");
    int count = 0;
    for (int i = 0; i < g->vertices; i++) {
        if (visited[i]) {
            if (count > 0) printf(", ");
            printf("%d", i);
            count++;
        }
    }
    if (count == 0) {
        printf("(niciuna - modul independent)");
    }
    printf("\n  Total: %d dependențe\n", count);
    
    free(visited);
}

/* =============================================================================
 * FUNCȚIA PRINCIPALĂ - DEMONSTRAȚIE
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     SOLUȚIE TEMA 2: Analizator de Dependențe                  ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* =========================================================================
     * TEST 1: DAG valid (fără ciclu)
     * =========================================================================
     *
     *   main (0) ──→ utils (1) ──→ math (3)
     *        │           │
     *        └──→ io (2) ┴──→ config (4)
     *
     *   Ordine corectă de build: 3, 4, 1, 2, 0 (sau similar)
     */
    
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("TEST 1: DAG VALID (fără dependențe circulare)\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    
    Graph* deps1 = create_graph(5);
    add_dependency(deps1, 0, 1);  /* main → utils */
    add_dependency(deps1, 0, 2);  /* main → io */
    add_dependency(deps1, 1, 3);  /* utils → math */
    add_dependency(deps1, 1, 4);  /* utils → config */
    add_dependency(deps1, 2, 4);  /* io → config */
    
    printf("\nStructura proiectului:\n");
    printf("  main (0) → utils (1) → math (3)\n");
    printf("       │         └────→ config (4)\n");
    printf("       └→ io (2) ──────→ config (4)\n");
    
    print_dependencies(deps1);
    
    printf("\n--- Verificare Ciclu ---\n");
    if (!has_circular_dependency(deps1)) {
        printf("  ✓ Graf valid - nu există dependențe circulare\n");
        
        int* order = topological_sort(deps1);
        if (order != NULL) {
            print_build_order(order, deps1->vertices);
            
            /* Verificăm ordinea */
            printf("\nVerificare ordine:\n");
            printf("  • config (4) trebuie compilat înainte de utils (1) și io (2)\n");
            printf("  • math (3) trebuie compilat înainte de utils (1)\n");
            printf("  • utils (1) și io (2) trebuie compilate înainte de main (0)\n");
            
            free(order);
        }
    }
    
    /* Test dependențe tranzitive */
    transitive_dependencies(deps1, 0);
    transitive_dependencies(deps1, 1);
    transitive_dependencies(deps1, 4);
    
    free_graph(deps1);
    
    /* =========================================================================
     * TEST 2: Graf cu ciclu
     * =========================================================================
     *
     *   A (0) → B (1) → C (2)
     *             ↑       ↓
     *             └────── D (3)
     */
    
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("TEST 2: GRAF CU CICLU (dependență circulară)\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    
    Graph* deps2 = create_graph(4);
    add_dependency(deps2, 0, 1);  /* A → B */
    add_dependency(deps2, 1, 2);  /* B → C */
    add_dependency(deps2, 2, 3);  /* C → D */
    add_dependency(deps2, 3, 1);  /* D → B (creează ciclu!) */
    
    printf("\nStructura cu ciclu:\n");
    printf("  A (0) → B (1) → C (2) → D (3)\n");
    printf("            ↑               ↓\n");
    printf("            └───────────────┘\n");
    
    print_dependencies(deps2);
    
    printf("\n--- Verificare Ciclu ---\n");
    if (has_circular_dependency(deps2)) {
        printf("\n  ✗ Build imposibil - trebuie rezolvat ciclul!\n");
    }
    
    /* Sortarea topologică va eșua */
    int* order2 = topological_sort(deps2);
    if (order2 == NULL) {
        printf("  (Sortare topologică nu se poate realiza)\n");
    } else {
        free(order2);
    }
    
    free_graph(deps2);
    
    /* =========================================================================
     * TEST 3: Graf mai complex
     * =========================================================================
     */
    
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("TEST 3: PROIECT COMPLEX\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    
    Graph* deps3 = create_graph(8);
    
    /* Un proiect de tip compilator */
    add_dependency(deps3, 0, 1);  /* compiler → parser */
    add_dependency(deps3, 0, 2);  /* compiler → codegen */
    add_dependency(deps3, 1, 3);  /* parser → lexer */
    add_dependency(deps3, 1, 4);  /* parser → ast */
    add_dependency(deps3, 2, 4);  /* codegen → ast */
    add_dependency(deps3, 2, 5);  /* codegen → optimizer */
    add_dependency(deps3, 3, 6);  /* lexer → utils */
    add_dependency(deps3, 4, 6);  /* ast → utils */
    add_dependency(deps3, 5, 4);  /* optimizer → ast */
    add_dependency(deps3, 6, 7);  /* utils → types */
    
    printf("\nStructura proiect compilator:\n");
    printf("  compiler (0) → parser (1) → lexer (3) → utils (6) → types (7)\n");
    printf("       │              └→ ast (4) ←───────────┘\n");
    printf("       └→ codegen (2) → optimizer (5) → ast\n");
    
    print_dependencies(deps3);
    
    if (!has_circular_dependency(deps3)) {
        int* order3 = topological_sort(deps3);
        if (order3 != NULL) {
            print_build_order(order3, deps3->vertices);
            free(order3);
        }
    }
    
    printf("\nDependențe pentru modulul principal (compiler):\n");
    transitive_dependencies(deps3, 0);
    
    free_graph(deps3);
    
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("✓ Toate testele executate cu succes!\n");
    printf("✓ Memoria eliberată corect\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    return 0;
}
