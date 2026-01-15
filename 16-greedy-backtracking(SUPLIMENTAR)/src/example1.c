/**
 * =============================================================================
 * SĂPTĂMÂNA 16: PARADIGME ALGORITMICE - GREEDY ȘI BACKTRACKING
 * Exemplu Complet și Demonstrativ
 * =============================================================================
 *
 * Acest exemplu demonstrează:
 *   1. Paradigma GREEDY cu algoritmi clasici
 *      - Activity Selection Problem
 *      - Fractional Knapsack Problem
 *      - Huffman Coding (simplificat)
 *
 *   2. Paradigma BACKTRACKING
 *      - N-Queens Problem
 *      - Sudoku Solver
 *      - Generare Combinatorică (Permutări, Combinări)
 *      - Graph Coloring
 *
 *   3. Comparație și analiză între paradigme
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -g -o example1 example1.c -lm
 * Execuție:  ./example1
 * Valgrind:  valgrind --leak-check=full ./example1
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include <limits.h>

/* =============================================================================
 * PARTEA 1: CONSTANTE ȘI STRUCTURI AUXILIARE
 * =============================================================================
 */

#define MAX_ACTIVITIES 100
#define MAX_ITEMS 100
#define MAX_N 20
#define SUDOKU_SIZE 9
#define MAX_VERTICES 20
#define MAX_COLORS 10

/**
 * Structură pentru activități (Activity Selection)
 */
typedef struct {
    int id;
    char name[32];
    int start;
    int finish;
} Activity;

/**
 * Structură pentru obiecte (Knapsack)
 */
typedef struct {
    int id;
    char name[32];
    double weight;
    double value;
    double ratio;
    double fraction;  /* Fracțiunea luată (0.0 - 1.0) */
} KnapsackItem;

/**
 * Structură pentru nod Huffman
 */
typedef struct HuffmanNode {
    char character;
    int frequency;
    struct HuffmanNode *left;
    struct HuffmanNode *right;
} HuffmanNode;

/**
 * Statistici pentru backtracking
 */
typedef struct {
    int nodes_visited;
    int solutions_found;
    int backtracks;
    double time_elapsed;
} BacktrackStats;

/* =============================================================================
 * PARTEA 2: FUNCȚII UTILITARE
 * =============================================================================
 */

/**
 * Afișează separatorul vizual
 */
void print_separator(const char *title) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════════════╗\n");
    printf("║  %-73s║\n", title);
    printf("╚═══════════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

/**
 * Afișează un subseparator
 */
void print_subseparator(const char *title) {
    printf("\n┌───────────────────────────────────────────────────────────────────────────┐\n");
    printf("│  %-73s│\n", title);
    printf("└───────────────────────────────────────────────────────────────────────────┘\n\n");
}

/**
 * Schimbă două valori întregi
 */
void swap_int(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * Returnează maximul dintre două valori
 */
int max_int(int a, int b) {
    return (a > b) ? a : b;
}

/**
 * Returnează minimul dintre două valori
 */
int min_int(int a, int b) {
    return (a < b) ? a : b;
}

/* =============================================================================
 * PARTEA 3: ACTIVITY SELECTION PROBLEM (GREEDY)
 * =============================================================================
 */

/**
 * Funcție de comparare pentru sortare activități după finish time
 */
int compare_activities_by_finish(const void *a, const void *b) {
    return ((Activity*)a)->finish - ((Activity*)b)->finish;
}

/**
 * Selectează activitățile compatibile folosind algoritmul Greedy.
 *
 * @param activities Array de activități
 * @param n Numărul de activități
 * @param selected Array pentru ID-urile selectate (alocat de apelant)
 * @return Numărul de activități selectate
 *
 * Complexitate: O(n log n) pentru sortare + O(n) pentru selecție
 */
int activity_selection(Activity *activities, int n, int *selected) {
    /* Sortare după timpul de finish */
    qsort(activities, n, sizeof(Activity), compare_activities_by_finish);
    
    int count = 0;
    int last_finish = 0;
    
    printf("  Activități sortate după timp finish:\n");
    printf("  %-4s %-15s %-8s %-8s\n", "ID", "Nume", "Start", "Finish");
    printf("  ────────────────────────────────────────\n");
    
    for (int i = 0; i < n; i++) {
        printf("  %-4d %-15s %-8d %-8d", 
               activities[i].id, activities[i].name,
               activities[i].start, activities[i].finish);
        
        /* Alegerea greedy: prima activitate compatibilă */
        if (activities[i].start >= last_finish) {
            selected[count++] = i;
            last_finish = activities[i].finish;
            printf(" ← SELECTAT\n");
        } else {
            printf("\n");
        }
    }
    
    return count;
}

/**
 * Demonstrație Activity Selection
 */
void demo_activity_selection(void) {
    print_subseparator("ACTIVITY SELECTION PROBLEM");
    
    Activity activities[] = {
        {1, "Curs ATP", 8, 10},
        {2, "Lab BD", 9, 11},
        {3, "Seminar Stat", 10, 12},
        {4, "Prânz", 12, 13},
        {5, "Sport", 11, 14},
        {6, "Proiect", 13, 16},
        {7, "Curs OOP", 14, 16},
        {8, "Studiu", 16, 18},
        {9, "Întâlnire", 15, 17}
    };
    int n = sizeof(activities) / sizeof(activities[0]);
    
    printf("  Problema: Selectați numărul maxim de activități compatibile\n");
    printf("  (activitățile nu se pot suprapune în timp)\n\n");
    
    int selected[MAX_ACTIVITIES];
    int count = activity_selection(activities, n, selected);
    
    printf("\n  REZULTAT GREEDY:\n");
    printf("  Număr activități selectate: %d din %d\n\n", count, n);
    
    printf("  Timeline vizual:\n");
    printf("  Ora:   8   9  10  11  12  13  14  15  16  17  18\n");
    printf("        |---|---|---|---|---|---|---|---|---|---|\n");
    
    for (int i = 0; i < count; i++) {
        int idx = selected[i];
        printf("        ");
        for (int t = 8; t < 18; t++) {
            if (t >= activities[idx].start && t < activities[idx].finish) {
                printf("████");
            } else {
                printf("    ");
            }
        }
        printf(" %s\n", activities[idx].name);
    }
    
    printf("\n  Complexitate: O(n log n)\n");
    printf("  Garantează soluția OPTIMĂ (demonstrat prin exchange argument)\n");
}

/* =============================================================================
 * PARTEA 4: FRACTIONAL KNAPSACK PROBLEM (GREEDY)
 * =============================================================================
 */

/**
 * Funcție de comparare pentru sortare obiecte după ratio value/weight (descrescător)
 */
int compare_items_by_ratio(const void *a, const void *b) {
    double diff = ((KnapsackItem*)b)->ratio - ((KnapsackItem*)a)->ratio;
    return (diff > 0) - (diff < 0);
}

/**
 * Rezolvă Fractional Knapsack folosind Greedy.
 *
 * @param items Array de obiecte
 * @param n Numărul de obiecte
 * @param capacity Capacitatea rucsacului
 * @return Valoarea maximă obținută
 *
 * Complexitate: O(n log n)
 */
double fractional_knapsack(KnapsackItem *items, int n, double capacity) {
    /* Calculează ratios */
    for (int i = 0; i < n; i++) {
        items[i].ratio = items[i].value / items[i].weight;
        items[i].fraction = 0.0;
    }
    
    /* Sortează descrescător după ratio */
    qsort(items, n, sizeof(KnapsackItem), compare_items_by_ratio);
    
    printf("  Obiecte sortate după value/weight ratio:\n");
    printf("  %-4s %-12s %-10s %-10s %-12s\n", "ID", "Nume", "Greutate", "Valoare", "Ratio");
    printf("  ─────────────────────────────────────────────────────\n");
    
    for (int i = 0; i < n; i++) {
        printf("  %-4d %-12s %-10.1f %-10.1f %-12.2f\n",
               items[i].id, items[i].name, items[i].weight, 
               items[i].value, items[i].ratio);
    }
    
    double total_value = 0.0;
    double remaining = capacity;
    
    printf("\n  Procesul de selecție Greedy:\n");
    printf("  Capacitate inițială: %.1f kg\n\n", capacity);
    
    for (int i = 0; i < n && remaining > 0; i++) {
        if (items[i].weight <= remaining) {
            /* Luăm obiectul complet */
            items[i].fraction = 1.0;
            total_value += items[i].value;
            remaining -= items[i].weight;
            printf("  + Luăm %s complet (%.1f kg): +%.1f lei (rămas: %.1f kg)\n",
                   items[i].name, items[i].weight, items[i].value, remaining);
        } else {
            /* Luăm o fracțiune */
            items[i].fraction = remaining / items[i].weight;
            double partial_value = items[i].value * items[i].fraction;
            total_value += partial_value;
            printf("  + Luăm %.0f%% din %s (%.1f kg): +%.1f lei\n",
                   items[i].fraction * 100, items[i].name, remaining, partial_value);
            remaining = 0;
        }
    }
    
    return total_value;
}

/**
 * Demonstrație Fractional Knapsack
 */
void demo_fractional_knapsack(void) {
    print_subseparator("FRACTIONAL KNAPSACK PROBLEM");
    
    KnapsackItem items[] = {
        {1, "Laptop", 3.0, 2000.0, 0, 0},
        {2, "Telefon", 0.3, 800.0, 0, 0},
        {3, "Tableta", 0.5, 500.0, 0, 0},
        {4, "Cărți", 5.0, 300.0, 0, 0},
        {5, "Haine", 4.0, 200.0, 0, 0},
        {6, "Aur", 2.0, 5000.0, 0, 0},
        {7, "Argint", 3.0, 1500.0, 0, 0}
    };
    int n = sizeof(items) / sizeof(items[0]);
    double capacity = 10.0;
    
    printf("  Problema: Umpleți rucsacul de %.1f kg pentru valoare maximă\n", capacity);
    printf("  (puteți lua fracțiuni din obiecte)\n\n");
    
    double max_value = fractional_knapsack(items, n, capacity);
    
    printf("\n  REZULTAT:\n");
    printf("  Valoare totală maximă: %.1f lei\n", max_value);
    
    printf("\n  Conținutul rucsacului:\n");
    for (int i = 0; i < n; i++) {
        if (items[i].fraction > 0) {
            printf("  • %s: %.0f%% (%.1f kg, %.1f lei)\n",
                   items[i].name, items[i].fraction * 100,
                   items[i].weight * items[i].fraction,
                   items[i].value * items[i].fraction);
        }
    }
    
    printf("\n  Notă: Pentru 0/1 Knapsack (fără fracțiuni), Greedy NU garantează optim!\n");
    printf("        Se folosește Programare Dinamică.\n");
}

/* =============================================================================
 * PARTEA 5: N-QUEENS PROBLEM (BACKTRACKING)
 * =============================================================================
 */

static int queens_solutions_count = 0;
static bool queens_print_all = false;

/**
 * Afișează tabla de șah cu reginele plasate
 */
void print_queens_board(int col[], int n) {
    printf("  Soluția %d:\n", queens_solutions_count);
    printf("    ");
    for (int c = 0; c < n; c++) printf(" %d ", c);
    printf("\n");
    printf("   ┌");
    for (int c = 0; c < n; c++) printf("───");
    printf("┐\n");
    
    for (int r = 0; r < n; r++) {
        printf(" %d │", r);
        for (int c = 0; c < n; c++) {
            if (col[r] == c) {
                printf(" Q ");
            } else {
                printf(" · ");
            }
        }
        printf("│\n");
    }
    
    printf("   └");
    for (int c = 0; c < n; c++) printf("───");
    printf("┘\n\n");
}

/**
 * Verifică dacă regina de pe rândul 'row' este sigură.
 */
bool is_safe_queen(int col[], int row) {
    for (int i = 0; i < row; i++) {
        /* Aceeași coloană */
        if (col[i] == col[row]) return false;
        
        /* Diagonale */
        if (abs(row - i) == abs(col[row] - col[i])) return false;
    }
    return true;
}

/**
 * Rezolvă N-Queens folosind backtracking
 */
void solve_n_queens(int col[], int row, int n, BacktrackStats *stats) {
    stats->nodes_visited++;
    
    if (row == n) {
        queens_solutions_count++;
        stats->solutions_found++;
        if (queens_print_all || queens_solutions_count <= 2) {
            print_queens_board(col, n);
        }
        return;
    }
    
    for (int c = 0; c < n; c++) {
        col[row] = c;
        
        if (is_safe_queen(col, row)) {
            solve_n_queens(col, row + 1, n, stats);
        } else {
            stats->backtracks++;
        }
    }
}

/**
 * Demonstrație N-Queens
 */
void demo_n_queens(void) {
    print_subseparator("N-QUEENS PROBLEM (BACKTRACKING)");
    
    printf("  Problema: Plasați N regine pe tabla NxN fără să se atace\n");
    printf("  (nici pe linie, coloană sau diagonală)\n\n");
    
    int n = 8;
    int col[MAX_N];
    BacktrackStats stats = {0, 0, 0, 0};
    
    queens_solutions_count = 0;
    queens_print_all = false;
    
    clock_t start = clock();
    solve_n_queens(col, 0, n, &stats);
    clock_t end = clock();
    stats.time_elapsed = (double)(end - start) / CLOCKS_PER_SEC;
    
    printf("  ... (afișate primele 2 soluții din %d)\n\n", stats.solutions_found);
    
    printf("  STATISTICI pentru N=%d:\n", n);
    printf("  ├─ Soluții găsite:    %d\n", stats.solutions_found);
    printf("  ├─ Noduri vizitate:   %d\n", stats.nodes_visited);
    printf("  ├─ Backtrack-uri:     %d\n", stats.backtracks);
    printf("  └─ Timp execuție:     %.4f secunde\n", stats.time_elapsed);
    
    printf("\n  Număr soluții pentru diferite N:\n");
    printf("  ┌─────┬───────────┬─────────────────────┐\n");
    printf("  │  N  │  Soluții  │ Soluții unice       │\n");
    printf("  ├─────┼───────────┼─────────────────────┤\n");
    printf("  │  4  │     2     │ 1 (fără simetrie)   │\n");
    printf("  │  5  │    10     │ 2                   │\n");
    printf("  │  6  │     4     │ 1                   │\n");
    printf("  │  7  │    40     │ 6                   │\n");
    printf("  │  8  │    92     │ 12                  │\n");
    printf("  └─────┴───────────┴─────────────────────┘\n");
}

/* =============================================================================
 * PARTEA 6: SUDOKU SOLVER (BACKTRACKING)
 * =============================================================================
 */

static int sudoku_nodes = 0;
static int sudoku_backtracks = 0;

/**
 * Afișează grid-ul Sudoku formatat
 */
void print_sudoku(int grid[SUDOKU_SIZE][SUDOKU_SIZE]) {
    printf("    ┌───────┬───────┬───────┐\n");
    for (int r = 0; r < SUDOKU_SIZE; r++) {
        printf("    │");
        for (int c = 0; c < SUDOKU_SIZE; c++) {
            if (grid[r][c] == 0) {
                printf(" · ");
            } else {
                printf(" %d ", grid[r][c]);
            }
            if (c % 3 == 2) printf("│");
        }
        printf("\n");
        if (r % 3 == 2 && r < 8) {
            printf("    ├───────┼───────┼───────┤\n");
        }
    }
    printf("    └───────┴───────┴───────┘\n");
}

/**
 * Verifică dacă cifra num poate fi plasată în celula (row, col)
 */
bool is_valid_sudoku(int grid[SUDOKU_SIZE][SUDOKU_SIZE], int row, int col, int num) {
    /* Verifică rândul */
    for (int c = 0; c < SUDOKU_SIZE; c++) {
        if (grid[row][c] == num) return false;
    }
    
    /* Verifică coloana */
    for (int r = 0; r < SUDOKU_SIZE; r++) {
        if (grid[r][col] == num) return false;
    }
    
    /* Verifică subgrid-ul 3×3 */
    int box_row = (row / 3) * 3;
    int box_col = (col / 3) * 3;
    for (int r = box_row; r < box_row + 3; r++) {
        for (int c = box_col; c < box_col + 3; c++) {
            if (grid[r][c] == num) return false;
        }
    }
    
    return true;
}

/**
 * Găsește prima celulă goală
 */
bool find_empty_cell(int grid[SUDOKU_SIZE][SUDOKU_SIZE], int *row, int *col) {
    for (*row = 0; *row < SUDOKU_SIZE; (*row)++) {
        for (*col = 0; *col < SUDOKU_SIZE; (*col)++) {
            if (grid[*row][*col] == 0) return true;
        }
    }
    return false;
}

/**
 * Rezolvă Sudoku folosind backtracking
 */
bool solve_sudoku(int grid[SUDOKU_SIZE][SUDOKU_SIZE]) {
    int row, col;
    sudoku_nodes++;
    
    if (!find_empty_cell(grid, &row, &col)) {
        return true;  /* Soluție completă */
    }
    
    for (int num = 1; num <= 9; num++) {
        if (is_valid_sudoku(grid, row, col, num)) {
            grid[row][col] = num;
            
            if (solve_sudoku(grid)) {
                return true;
            }
            
            grid[row][col] = 0;  /* BACKTRACK */
            sudoku_backtracks++;
        }
    }
    
    return false;
}

/**
 * Demonstrație Sudoku Solver
 */
void demo_sudoku_solver(void) {
    print_subseparator("SUDOKU SOLVER (BACKTRACKING)");
    
    printf("  Problema: Completați grid-ul 9×9 cu cifrele 1-9\n");
    printf("  (fără repetiție pe rând, coloană sau subgrid 3×3)\n\n");
    
    int grid[SUDOKU_SIZE][SUDOKU_SIZE] = {
        {5, 3, 0, 0, 7, 0, 0, 0, 0},
        {6, 0, 0, 1, 9, 5, 0, 0, 0},
        {0, 9, 8, 0, 0, 0, 0, 6, 0},
        {8, 0, 0, 0, 6, 0, 0, 0, 3},
        {4, 0, 0, 8, 0, 3, 0, 0, 1},
        {7, 0, 0, 0, 2, 0, 0, 0, 6},
        {0, 6, 0, 0, 0, 0, 2, 8, 0},
        {0, 0, 0, 4, 1, 9, 0, 0, 5},
        {0, 0, 0, 0, 8, 0, 0, 7, 9}
    };
    
    printf("  Puzzle inițial:\n");
    print_sudoku(grid);
    
    sudoku_nodes = 0;
    sudoku_backtracks = 0;
    
    clock_t start = clock();
    bool solved = solve_sudoku(grid);
    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
    
    if (solved) {
        printf("\n  Soluție găsită:\n");
        print_sudoku(grid);
        
        printf("\n  STATISTICI:\n");
        printf("  ├─ Noduri vizitate:  %d\n", sudoku_nodes);
        printf("  ├─ Backtrack-uri:    %d\n", sudoku_backtracks);
        printf("  └─ Timp execuție:    %.4f secunde\n", elapsed);
    } else {
        printf("\n  ✗ Nu s-a găsit soluție!\n");
    }
}

/* =============================================================================
 * PARTEA 7: GENERARE COMBINATORICĂ (BACKTRACKING)
 * =============================================================================
 */

static int perm_count = 0;
static int comb_count = 0;

/**
 * Afișează un array
 */
void print_array_combo(int arr[], int n, const char *prefix) {
    printf("  %s [", prefix);
    for (int i = 0; i < n; i++) {
        printf("%d", arr[i]);
        if (i < n - 1) printf(", ");
    }
    printf("]\n");
}

/**
 * Generează toate permutările
 */
void generate_permutations(int arr[], int start, int n, int max_print) {
    if (start == n - 1) {
        perm_count++;
        if (perm_count <= max_print) {
            char prefix[16];
            sprintf(prefix, "P%d:", perm_count);
            print_array_combo(arr, n, prefix);
        }
        return;
    }
    
    for (int i = start; i < n; i++) {
        swap_int(&arr[start], &arr[i]);
        generate_permutations(arr, start + 1, n, max_print);
        swap_int(&arr[start], &arr[i]);  /* Backtrack */
    }
}

/**
 * Generează toate combinările de k elemente din n
 */
void generate_combinations(int arr[], int n, int k, int start, 
                           int combo[], int combo_idx, int max_print) {
    if (combo_idx == k) {
        comb_count++;
        if (comb_count <= max_print) {
            char prefix[16];
            sprintf(prefix, "C%d:", comb_count);
            print_array_combo(combo, k, prefix);
        }
        return;
    }
    
    for (int i = start; i < n; i++) {
        combo[combo_idx] = arr[i];
        generate_combinations(arr, n, k, i + 1, combo, combo_idx + 1, max_print);
    }
}

/**
 * Generează toate submulțimile
 */
void generate_subsets(int arr[], int n, int subset[], int idx, int *count, int max_print) {
    if (*count <= max_print) {
        printf("  S%d: {", *count);
        for (int i = 0; i < idx; i++) {
            printf("%d", subset[i]);
            if (i < idx - 1) printf(", ");
        }
        printf("}\n");
    }
    (*count)++;
    
    for (int i = (idx == 0) ? 0 : (subset[idx-1] - arr[0] + 1); i < n; i++) {
        subset[idx] = arr[i];
        generate_subsets(arr, n, subset, idx + 1, count, max_print);
    }
}

/**
 * Demonstrație generare combinatorică
 */
void demo_combinatorics(void) {
    print_subseparator("GENERARE COMBINATORICĂ (BACKTRACKING)");
    
    int arr[] = {1, 2, 3, 4};
    int n = 4;
    int combo[10];
    
    printf("  Mulțime de bază: {1, 2, 3, 4}\n\n");
    
    /* Permutări */
    printf("  PERMUTĂRI P(4) = 4! = 24:\n");
    perm_count = 0;
    generate_permutations(arr, 0, n, 6);
    printf("  ... (afișate primele 6 din %d)\n\n", 24);
    
    /* Reset array pentru că permutările îl modifică */
    arr[0] = 1; arr[1] = 2; arr[2] = 3; arr[3] = 4;
    
    /* Combinări */
    printf("  COMBINĂRI C(4,2) = 6:\n");
    comb_count = 0;
    generate_combinations(arr, n, 2, 0, combo, 0, 10);
    printf("\n");
    
    /* Submulțimi */
    printf("  SUBMULȚIMI 2^4 = 16:\n");
    int subset[10];
    int sub_count = 0;
    /* Afișăm doar primele 8 */
    printf("  S0: {}\n");
    sub_count = 1;
    for (int i = 0; i < n && sub_count <= 8; i++) {
        subset[0] = arr[i];
        printf("  S%d: {%d}\n", sub_count++, arr[i]);
    }
    printf("  ... (total 16 submulțimi)\n");
    
    printf("\n  Complexități:\n");
    printf("  ├─ Permutări P(n):     O(n!)\n");
    printf("  ├─ Combinări C(n,k):   O(C(n,k))\n");
    printf("  └─ Submulțimi:         O(2^n)\n");
}

/* =============================================================================
 * PARTEA 8: GRAPH COLORING (BACKTRACKING)
 * =============================================================================
 */

/**
 * Verifică dacă nodul v poate primi culoarea c
 */
bool can_color(int adj[MAX_VERTICES][MAX_VERTICES], int colors[], int v, int c, int n) {
    for (int i = 0; i < n; i++) {
        if (adj[v][i] && colors[i] == c) return false;
    }
    return true;
}

/**
 * Colorează graful folosind backtracking
 */
bool graph_coloring(int adj[MAX_VERTICES][MAX_VERTICES], int n, int num_colors,
                    int colors[], int v) {
    if (v == n) return true;
    
    for (int c = 1; c <= num_colors; c++) {
        if (can_color(adj, colors, v, c, n)) {
            colors[v] = c;
            if (graph_coloring(adj, n, num_colors, colors, v + 1)) {
                return true;
            }
            colors[v] = 0;
        }
    }
    return false;
}

/**
 * Demonstrație Graph Coloring
 */
void demo_graph_coloring(void) {
    print_subseparator("GRAPH COLORING (BACKTRACKING)");
    
    printf("  Problema: Colorați nodurile grafului cu minim k culori\n");
    printf("  astfel încât noduri adiacente să aibă culori diferite\n\n");
    
    /* Graf simplu cu 5 noduri */
    int n = 5;
    int adj[MAX_VERTICES][MAX_VERTICES] = {
        {0, 1, 1, 0, 0},
        {1, 0, 1, 1, 0},
        {1, 1, 0, 1, 1},
        {0, 1, 1, 0, 1},
        {0, 0, 1, 1, 0}
    };
    
    printf("  Graful (matrice de adiacență):\n");
    printf("       0  1  2  3  4\n");
    printf("    ┌────────────────\n");
    for (int i = 0; i < n; i++) {
        printf("  %d │ ", i);
        for (int j = 0; j < n; j++) {
            printf("%d  ", adj[i][j]);
        }
        printf("\n");
    }
    
    printf("\n  Reprezentare vizuală:\n");
    printf("         0 ─── 1\n");
    printf("         │ ╲ ╱ │\n");
    printf("         │  2  │\n");
    printf("         │ ╱ ╲ │\n");
    printf("         4 ─── 3\n\n");
    
    int colors[MAX_VERTICES] = {0};
    char *color_names[] = {"", "🔴 Roșu", "🔵 Albastru", "🟢 Verde", "🟡 Galben"};
    
    /* Încercăm cu 3 culori */
    if (graph_coloring(adj, n, 3, colors, 0)) {
        printf("  Soluție cu 3 culori:\n");
        for (int i = 0; i < n; i++) {
            printf("    Nod %d: %s\n", i, color_names[colors[i]]);
        }
    } else {
        printf("  Nu se poate colora cu 3 culori!\n");
    }
    
    printf("\n  Aplicații industriale:\n");
    printf("  • Alocare registre în compilatoare\n");
    printf("  • Planificare examene (ore diferite pt. materii comune)\n");
    printf("  • Alocare frecvențe radio\n");
}

/* =============================================================================
 * PARTEA 9: COMPARAȚIE PARADIGME
 * =============================================================================
 */

void demo_comparison(void) {
    print_subseparator("COMPARAȚIE: GREEDY vs. BACKTRACKING vs. DP");
    
    printf("  ┌───────────────┬────────────────────┬──────────────────┬───────────────────┐\n");
    printf("  │   Criteriu    │      GREEDY        │   BACKTRACKING   │ PROGRAMARE DIN.   │\n");
    printf("  ├───────────────┼────────────────────┼──────────────────┼───────────────────┤\n");
    printf("  │ Strategie     │ Alegere locală     │ Explorare        │ Subprobleme       │\n");
    printf("  │               │ optimă             │ exhaustivă       │ suprapuse         │\n");
    printf("  ├───────────────┼────────────────────┼──────────────────┼───────────────────┤\n");
    printf("  │ Garantează    │ NU (doar dacă      │ DA (dacă există) │ DA                │\n");
    printf("  │ optim global? │ prop. greedy)      │                  │                   │\n");
    printf("  ├───────────────┼────────────────────┼──────────────────┼───────────────────┤\n");
    printf("  │ Complexitate  │ O(n log n) tipic   │ O(k^n) worst     │ O(n×W) tipic      │\n");
    printf("  ├───────────────┼────────────────────┼──────────────────┼───────────────────┤\n");
    printf("  │ Spațiu        │ O(1) - O(n)        │ O(n) recursie    │ O(n) - O(n²)      │\n");
    printf("  ├───────────────┼────────────────────┼──────────────────┼───────────────────┤\n");
    printf("  │ Când folosești│ Substructură opt.  │ Toate soluțiile  │ Subprobleme       │\n");
    printf("  │               │ + prop. greedy     │ / Constrângeri   │ se repetă         │\n");
    printf("  ├───────────────┼────────────────────┼──────────────────┼───────────────────┤\n");
    printf("  │ Exemple       │ Activity Select.   │ N-Queens         │ 0/1 Knapsack      │\n");
    printf("  │               │ Fract. Knapsack    │ Sudoku           │ LCS, Edit Dist.   │\n");
    printf("  │               │ Huffman, Dijkstra  │ Graph Coloring   │ Floyd-Warshall    │\n");
    printf("  └───────────────┴────────────────────┴──────────────────┴───────────────────┘\n");
    
    printf("\n  Ghid de alegere:\n\n");
    printf("  ┌─────────────────────────────────────────────────────────────────────────┐\n");
    printf("  │  1. Problema are PROPRIETATEA ALEGERII GREEDY?                          │\n");
    printf("  │     └─ DA → Încearcă GREEDY (verifică corectitudinea!)                  │\n");
    printf("  │     └─ NU → Continuă la pasul 2                                         │\n");
    printf("  ├─────────────────────────────────────────────────────────────────────────┤\n");
    printf("  │  2. Subproblemele SE SUPRAPUN?                                          │\n");
    printf("  │     └─ DA → Folosește PROGRAMARE DINAMICĂ                               │\n");
    printf("  │     └─ NU → Continuă la pasul 3                                         │\n");
    printf("  ├─────────────────────────────────────────────────────────────────────────┤\n");
    printf("  │  3. Trebuie să găsești TOATE SOLUȚIILE sau ai CONSTRÂNGERI?             │\n");
    printf("  │     └─ DA → Folosește BACKTRACKING                                      │\n");
    printf("  │     └─ NU → Reanalyzează problema                                       │\n");
    printf("  └─────────────────────────────────────────────────────────────────────────┘\n");
}

/* =============================================================================
 * PARTEA 10: MAIN ȘI DEMONSTRAȚIE
 * =============================================================================
 */

void show_usage(const char *prog) {
    printf("Utilizare: %s [opțiuni]\n\n", prog);
    printf("Opțiuni:\n");
    printf("  (fără)           - Rulează toate demonstrațiile\n");
    printf("  --greedy         - Doar demonstrații Greedy\n");
    printf("  --backtracking   - Doar demonstrații Backtracking\n");
    printf("  --nqueens N      - Rezolvă N-Queens pentru N dat\n");
    printf("  --sudoku FILE    - Rezolvă Sudoku din fișier\n");
    printf("  --help           - Afișează acest ajutor\n");
}

int main(int argc, char *argv[]) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║     SĂPTĂMÂNA 16: PARADIGME ALGORITMICE - GREEDY ȘI BACKTRACKING              ║\n");
    printf("║     Exemplu Demonstrativ Complet                                              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    bool run_all = true;
    bool run_greedy = false;
    bool run_backtracking = false;
    
    /* Parsare argumente */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--greedy") == 0) {
            run_all = false;
            run_greedy = true;
        } else if (strcmp(argv[i], "--backtracking") == 0) {
            run_all = false;
            run_backtracking = true;
        } else if (strcmp(argv[i], "--help") == 0) {
            show_usage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "--nqueens") == 0 && i + 1 < argc) {
            int n = atoi(argv[++i]);
            if (n >= 4 && n <= MAX_N) {
                printf("Rezolvare N-Queens pentru N=%d\n\n", n);
                int col[MAX_N];
                BacktrackStats stats = {0, 0, 0, 0};
                queens_solutions_count = 0;
                queens_print_all = true;
                solve_n_queens(col, 0, n, &stats);
                printf("\nTotal soluții: %d\n", stats.solutions_found);
            } else {
                printf("N trebuie să fie între 4 și %d\n", MAX_N);
            }
            return 0;
        }
    }
    
    /* ============== PARTEA GREEDY ============== */
    if (run_all || run_greedy) {
        print_separator("PARTEA I: PARADIGMA GREEDY");
        
        printf("  Greedy = \"Fă alegerea LOCAL OPTIMĂ la fiecare pas\"\n");
        printf("  \n");
        printf("  Avantaje:\n");
        printf("  • Simplu de implementat\n");
        printf("  • Eficient (de obicei O(n log n))\n");
        printf("  \n");
        printf("  Dezavantaje:\n");
        printf("  • NU garantează mereu optim global\n");
        printf("  • Trebuie demonstrată corectitudinea\n");
        
        demo_activity_selection();
        demo_fractional_knapsack();
    }
    
    /* ============== PARTEA BACKTRACKING ============== */
    if (run_all || run_backtracking) {
        print_separator("PARTEA II: PARADIGMA BACKTRACKING");
        
        printf("  Backtracking = \"Explorează sistematic TOATE posibilitățile,\n");
        printf("                  abandonând ramurile nepromițătoare\"\n");
        printf("  \n");
        printf("  Avantaje:\n");
        printf("  • Găsește TOATE soluțiile\n");
        printf("  • Garantează optim (dacă există)\n");
        printf("  \n");
        printf("  Dezavantaje:\n");
        printf("  • Complexitate exponențială în worst case\n");
        printf("  • Necesită funcții bune de pruning\n");
        
        demo_n_queens();
        demo_sudoku_solver();
        demo_combinatorics();
        demo_graph_coloring();
    }
    
    /* ============== COMPARAȚIE ============== */
    if (run_all) {
        print_separator("PARTEA III: COMPARAȚIE ȘI GHID DE ALEGERE");
        demo_comparison();
    }
    
    print_separator("TOATE DEMONSTRAȚIILE COMPLETATE CU SUCCES!");
    
    printf("  Următorii pași:\n");
    printf("  1. Implementați exercițiile din src/exercise1.c și src/exercise2.c\n");
    printf("  2. Rezolvați temele din teme/homework-requirements.md\n");
    printf("  3. Încercați provocările din teme/homework-extended.md\n");
    printf("\n");
    
    return 0;
}
