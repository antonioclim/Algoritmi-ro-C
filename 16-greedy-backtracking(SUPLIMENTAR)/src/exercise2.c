/**
 * =============================================================================
 * EXERCIȚIUL 2: SUDOKU SOLVER (BACKTRACKING)
 * =============================================================================
 *
 * OBIECTIV:
 *   Implementați un solver de Sudoku complet folosind tehnica Backtracking.
 *   Programul trebuie să citească un puzzle din fișier, să îl rezolve și
 *   să afișeze statistici despre procesul de rezolvare.
 *
 * CERINȚE:
 *   1. Citiți puzzle-ul din fișier (0 = celulă goală)
 *   2. Implementați verificarea constrângerilor Sudoku
 *   3. Implementați algoritmul de backtracking
 *   4. Afișați soluția cu format frumos
 *   5. Colectați statistici: noduri vizitate, backtrack-uri, timp
 *   6. BONUS: Mod verbose pentru afișare pas-cu-pas
 *
 * EXEMPLU INPUT (data/sudoku_easy.txt):
 *   5 3 0 0 7 0 0 0 0
 *   6 0 0 1 9 5 0 0 0
 *   0 9 8 0 0 0 0 6 0
 *   8 0 0 0 6 0 0 0 3
 *   4 0 0 8 0 3 0 0 1
 *   7 0 0 0 2 0 0 0 6
 *   0 6 0 0 0 0 2 8 0
 *   0 0 0 4 1 9 0 0 5
 *   0 0 0 0 8 0 0 7 9
 *
 * EXPECTED OUTPUT:
 *   Soluție găsită!
 *   ┌───────┬───────┬───────┐
 *   │ 5 3 4 │ 6 7 8 │ 9 1 2 │
 *   │ 6 7 2 │ 1 9 5 │ 3 4 8 │
 *   │ 1 9 8 │ 3 4 2 │ 5 6 7 │
 *   ... etc
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -o exercise2 exercise2.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

/* =============================================================================
 * CONSTANTE ȘI DEFINIREA TIPURILOR
 * =============================================================================
 */

#define SIZE 9          /* Dimensiunea grid-ului Sudoku */
#define SUBGRID 3       /* Dimensiunea subgrid-ului (3x3) */
#define EMPTY 0         /* Valoarea pentru celulă goală */

/**
 * Structură pentru statisticile backtracking-ului
 */
typedef struct {
    int nodes_visited;      /* Noduri din arborele de căutare vizitate */
    int backtracks;         /* Număr de întoarceri (backtrack) */
    int valid_placements;   /* Plasări valide încercate */
    double time_seconds;    /* Timp de execuție în secunde */
} SolverStats;

/**
 * Structură pentru puzzle-ul Sudoku
 */
typedef struct {
    int grid[SIZE][SIZE];       /* Grid-ul curent */
    int original[SIZE][SIZE];   /* Grid-ul original (pentru afișare) */
    int empty_cells;            /* Număr de celule goale inițial */
} SudokuPuzzle;

/* Variabilă globală pentru mod verbose */
static bool verbose_mode = false;

/* =============================================================================
 * FUNCȚII DE CITIRE ȘI AFIȘARE
 * =============================================================================
 */

/**
 * Citește un puzzle Sudoku din fișier.
 *
 * @param filename Numele fișierului
 * @param puzzle Pointer la structura puzzle
 * @return true dacă citirea a reușit, false altfel
 *
 * Format: 9 rânduri cu câte 9 numere separate prin spații
 *         0 = celulă goală
 */
bool read_puzzle_from_file(const char *filename, SudokuPuzzle *puzzle) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Eroare: Nu s-a putut deschide fișierul %s\n", filename);
        return false;
    }
    
    puzzle->empty_cells = 0;
    
    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            if (fscanf(file, "%d", &puzzle->grid[r][c]) != 1) {
                fprintf(stderr, "Eroare: Format invalid în fișier (rând %d, col %d)\n", r + 1, c + 1);
                fclose(file);
                return false;
            }
            
            puzzle->original[r][c] = puzzle->grid[r][c];
            
            if (puzzle->grid[r][c] == EMPTY) {
                puzzle->empty_cells++;
            }
        }
    }
    
    fclose(file);
    return true;
}

/**
 * Citește un puzzle Sudoku de la stdin.
 */
bool read_puzzle_from_stdin(SudokuPuzzle *puzzle) {
    puzzle->empty_cells = 0;
    
    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            if (scanf("%d", &puzzle->grid[r][c]) != 1) {
                return false;
            }
            
            puzzle->original[r][c] = puzzle->grid[r][c];
            
            if (puzzle->grid[r][c] == EMPTY) {
                puzzle->empty_cells++;
            }
        }
    }
    
    return true;
}

/**
 * Afișează grid-ul Sudoku cu borduri frumoase.
 *
 * @param puzzle Pointer la puzzle
 * @param highlight_solution Dacă true, evidențiază cifrele adăugate
 */
void print_sudoku_grid(const SudokuPuzzle *puzzle, bool highlight_solution) {
    printf("    ┌───────┬───────┬───────┐\n");
    
    for (int r = 0; r < SIZE; r++) {
        printf("    │");
        
        for (int c = 0; c < SIZE; c++) {
            int val = puzzle->grid[r][c];
            
            if (val == EMPTY) {
                printf(" · ");
            } else if (highlight_solution && puzzle->original[r][c] == EMPTY) {
                /* Cifră adăugată de solver - evidențiere */
                printf(" \033[1;32m%d\033[0m ", val);  /* Verde bold */
            } else {
                printf(" %d ", val);
            }
            
            if ((c + 1) % SUBGRID == 0) {
                printf("│");
            }
        }
        printf("\n");
        
        if ((r + 1) % SUBGRID == 0 && r < SIZE - 1) {
            printf("    ├───────┼───────┼───────┤\n");
        }
    }
    
    printf("    └───────┴───────┴───────┘\n");
}

/**
 * Afișează grid-ul simplu (pentru testare automată)
 */
void print_sudoku_simple(const SudokuPuzzle *puzzle) {
    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            printf("%d", puzzle->grid[r][c]);
            if (c < SIZE - 1) printf(" ");
        }
        printf("\n");
    }
}

/* =============================================================================
 * FUNCȚII DE VALIDARE
 * =============================================================================
 */

/**
 * TODO 1: Verifică dacă cifra 'num' poate fi plasată în rândul 'row'
 *
 * @param puzzle Pointer la puzzle
 * @param row Indexul rândului (0-8)
 * @param num Cifra de verificat (1-9)
 * @return true dacă cifra NU apare în rând, false altfel
 *
 * Hint: Parcurgeți toate coloanele și verificați dacă num apare deja
 */
bool is_valid_in_row(const SudokuPuzzle *puzzle, int row, int num) {
    /* YOUR CODE HERE */
    for (int c = 0; c < SIZE; c++) {
        if (puzzle->grid[row][c] == num) {
            return false;
        }
    }
    return true;
}

/**
 * TODO 2: Verifică dacă cifra 'num' poate fi plasată în coloana 'col'
 *
 * @param puzzle Pointer la puzzle
 * @param col Indexul coloanei (0-8)
 * @param num Cifra de verificat (1-9)
 * @return true dacă cifra NU apare în coloană, false altfel
 */
bool is_valid_in_col(const SudokuPuzzle *puzzle, int col, int num) {
    /* YOUR CODE HERE */
    for (int r = 0; r < SIZE; r++) {
        if (puzzle->grid[r][col] == num) {
            return false;
        }
    }
    return true;
}

/**
 * TODO 3: Verifică dacă cifra 'num' poate fi plasată în subgrid-ul 3x3
 *
 * @param puzzle Pointer la puzzle
 * @param row Indexul rândului celulei
 * @param col Indexul coloanei celulei
 * @param num Cifra de verificat (1-9)
 * @return true dacă cifra NU apare în subgrid, false altfel
 *
 * Hint: Calculați colțul din stânga-sus al subgrid-ului:
 *       box_row = (row / 3) * 3
 *       box_col = (col / 3) * 3
 *       Apoi parcurgeți cele 9 celule ale subgrid-ului.
 */
bool is_valid_in_box(const SudokuPuzzle *puzzle, int row, int col, int num) {
    /* YOUR CODE HERE */
    
    /* Calculăm colțul stânga-sus al subgrid-ului 3x3 */
    int box_row = (row / SUBGRID) * SUBGRID;
    int box_col = (col / SUBGRID) * SUBGRID;
    
    /* Verificăm toate cele 9 celule din subgrid */
    for (int r = box_row; r < box_row + SUBGRID; r++) {
        for (int c = box_col; c < box_col + SUBGRID; c++) {
            if (puzzle->grid[r][c] == num) {
                return false;
            }
        }
    }
    
    return true;
}

/**
 * TODO 4: Verifică dacă cifra 'num' poate fi plasată în celula (row, col)
 *
 * @param puzzle Pointer la puzzle
 * @param row Indexul rândului
 * @param col Indexul coloanei
 * @param num Cifra de verificat (1-9)
 * @return true dacă plasarea este validă, false altfel
 *
 * Trebuie să verifice TOATE cele trei constrângeri:
 *   1. Cifra nu apare în rând
 *   2. Cifra nu apare în coloană
 *   3. Cifra nu apare în subgrid-ul 3x3
 */
bool is_valid_placement(const SudokuPuzzle *puzzle, int row, int col, int num) {
    /* YOUR CODE HERE */
    return is_valid_in_row(puzzle, row, num) &&
           is_valid_in_col(puzzle, col, num) &&
           is_valid_in_box(puzzle, row, col, num);
}

/* =============================================================================
 * FUNCȚII AUXILIARE PENTRU BACKTRACKING
 * =============================================================================
 */

/**
 * TODO 5: Găsește prima celulă goală din grid
 *
 * @param puzzle Pointer la puzzle
 * @param row Pointer pentru a returna indexul rândului
 * @param col Pointer pentru a returna indexul coloanei
 * @return true dacă există celulă goală, false dacă grid-ul e complet
 *
 * Hint: Parcurgeți grid-ul de la stânga la dreapta, de sus în jos
 *       Când găsiți o celulă cu valoarea EMPTY (0), setați *row și *col
 */
bool find_empty_cell(const SudokuPuzzle *puzzle, int *row, int *col) {
    /* YOUR CODE HERE */
    for (*row = 0; *row < SIZE; (*row)++) {
        for (*col = 0; *col < SIZE; (*col)++) {
            if (puzzle->grid[*row][*col] == EMPTY) {
                return true;
            }
        }
    }
    return false;
}

/**
 * BONUS: Găsește celula goală cu cele mai puține opțiuni valide
 * (Most Constrained Variable - MCV heuristic)
 *
 * Aceasta este o optimizare care poate accelera semnificativ backtracking-ul.
 */
bool find_most_constrained_cell(const SudokuPuzzle *puzzle, int *best_row, int *best_col) {
    int min_options = SIZE + 1;
    *best_row = -1;
    *best_col = -1;
    
    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            if (puzzle->grid[r][c] == EMPTY) {
                /* Numărăm opțiunile valide pentru această celulă */
                int options = 0;
                for (int num = 1; num <= SIZE; num++) {
                    if (is_valid_placement(puzzle, r, c, num)) {
                        options++;
                    }
                }
                
                if (options < min_options) {
                    min_options = options;
                    *best_row = r;
                    *best_col = c;
                }
                
                /* Dacă găsim celulă cu 0 opțiuni, știm că trebuie backtrack */
                if (options == 0) {
                    return true;
                }
            }
        }
    }
    
    return (*best_row != -1);
}

/* =============================================================================
 * ALGORITM BACKTRACKING - SUDOKU SOLVER
 * =============================================================================
 */

/**
 * TODO 6: Implementați algoritmul de backtracking pentru rezolvarea Sudoku
 *
 * @param puzzle Pointer la puzzle (va fi modificat in-place)
 * @param stats Pointer la statistici (va fi actualizat)
 * @return true dacă s-a găsit o soluție, false altfel
 *
 * Algoritmul (recursiv):
 *   1. Găsește prima celulă goală
 *      - Dacă nu există, puzzle-ul e rezolvat -> return true
 *   2. Pentru fiecare cifră de la 1 la 9:
 *      a. Verifică dacă cifra poate fi plasată (is_valid_placement)
 *      b. Dacă DA:
 *         - Plasează cifra în grid
 *         - Apelează recursiv solve_sudoku()
 *         - Dacă recursivitatea returnează true -> return true
 *         - Altfel, BACKTRACK: șterge cifra (pune EMPTY)
 *   3. Dacă nicio cifră nu a funcționat -> return false
 *
 * Complexitate: O(9^(n²)) în worst case, unde n=9
 *               În practică, mult mai rapid datorită pruning-ului
 */
bool solve_sudoku(SudokuPuzzle *puzzle, SolverStats *stats) {
    /* Incrementăm contorul de noduri vizitate */
    stats->nodes_visited++;
    
    /* YOUR CODE HERE */
    
    /* Pas 1: Găsim prima celulă goală */
    int row, col;
    if (!find_empty_cell(puzzle, &row, &col)) {
        /* Nu mai sunt celule goale -> soluție găsită! */
        return true;
    }
    
    /* Pas 2: Încercăm fiecare cifră de la 1 la 9 */
    for (int num = 1; num <= SIZE; num++) {
        /* Verificăm dacă plasarea e validă */
        if (is_valid_placement(puzzle, row, col, num)) {
            stats->valid_placements++;
            
            /* Plasăm cifra */
            puzzle->grid[row][col] = num;
            
            /* Afișare verbose (opțional) */
            if (verbose_mode) {
                printf("  [%d,%d] = %d\n", row, col, num);
            }
            
            /* Apelăm recursiv */
            if (solve_sudoku(puzzle, stats)) {
                return true;  /* Soluție găsită! */
            }
            
            /* BACKTRACK: ștergem cifra și încercăm următoarea */
            puzzle->grid[row][col] = EMPTY;
            stats->backtracks++;
            
            if (verbose_mode) {
                printf("  [%d,%d] = X (backtrack)\n", row, col);
            }
        }
    }
    
    /* Nicio cifră nu a funcționat -> backtrack la nivelul anterior */
    return false;
}

/* =============================================================================
 * BONUS: SOLVER OPTIMIZAT
 * =============================================================================
 */

/**
 * Solver optimizat folosind MCV heuristic
 */
bool solve_sudoku_optimized(SudokuPuzzle *puzzle, SolverStats *stats) {
    stats->nodes_visited++;
    
    int row, col;
    if (!find_most_constrained_cell(puzzle, &row, &col)) {
        return true;  /* Soluție completă */
    }
    
    for (int num = 1; num <= SIZE; num++) {
        if (is_valid_placement(puzzle, row, col, num)) {
            stats->valid_placements++;
            puzzle->grid[row][col] = num;
            
            if (solve_sudoku_optimized(puzzle, stats)) {
                return true;
            }
            
            puzzle->grid[row][col] = EMPTY;
            stats->backtracks++;
        }
    }
    
    return false;
}

/* =============================================================================
 * FUNCȚII DE AFIȘARE STATISTICI
 * =============================================================================
 */

/**
 * Afișează statisticile rezolvării
 */
void print_stats(const SolverStats *stats, int empty_cells) {
    printf("\n  ╔════════════════════════════════════════════════════════╗\n");
    printf("  ║  STATISTICI BACKTRACKING                               ║\n");
    printf("  ╠════════════════════════════════════════════════════════╣\n");
    printf("  ║  Celule goale inițial:    %-6d                       ║\n", empty_cells);
    printf("  ║  Noduri vizitate:         %-6d                       ║\n", stats->nodes_visited);
    printf("  ║  Plasări valide încercate:%-6d                       ║\n", stats->valid_placements);
    printf("  ║  Backtrack-uri efectuate: %-6d                       ║\n", stats->backtracks);
    printf("  ║  Timp de execuție:        %-6.4f secunde             ║\n", stats->time_seconds);
    printf("  ╚════════════════════════════════════════════════════════╝\n");
}

/* =============================================================================
 * MAIN
 * =============================================================================
 */

void print_usage(const char *prog) {
    printf("Utilizare: %s [opțiuni] [fișier]\n\n", prog);
    printf("Opțiuni:\n");
    printf("  -v, --verbose    Afișare pas cu pas a procesului\n");
    printf("  -o, --optimized  Folosește solver-ul optimizat (MCV)\n");
    printf("  -h, --help       Afișează acest ajutor\n\n");
    printf("Dacă nu se specifică fișier, se citește de la stdin.\n");
}

int main(int argc, char *argv[]) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║  EXERCIȚIUL 2: SUDOKU SOLVER (BACKTRACKING)                   ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    bool use_optimized = false;
    const char *filename = NULL;
    
    /* Parsare argumente */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            verbose_mode = true;
        } else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--optimized") == 0) {
            use_optimized = true;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (argv[i][0] != '-') {
            filename = argv[i];
        }
    }
    
    /* Citire puzzle */
    SudokuPuzzle puzzle;
    bool success;
    
    if (filename != NULL) {
        printf("  Citire puzzle din: %s\n\n", filename);
        success = read_puzzle_from_file(filename, &puzzle);
    } else {
        printf("  Citire puzzle de la stdin...\n\n");
        success = read_puzzle_from_stdin(&puzzle);
    }
    
    if (!success) {
        fprintf(stderr, "Eroare la citirea puzzle-ului!\n");
        return 1;
    }
    
    /* Afișare puzzle inițial */
    printf("  Puzzle inițial (%d celule goale):\n\n", puzzle.empty_cells);
    print_sudoku_grid(&puzzle, false);
    
    /* Inițializare statistici */
    SolverStats stats = {0, 0, 0, 0.0};
    
    /* Rezolvare */
    printf("\n  Rezolvare în curs");
    if (use_optimized) printf(" (mod optimizat)");
    printf("...\n");
    
    if (verbose_mode) {
        printf("\n  [rând,col] = cifră\n");
        printf("  ─────────────────────\n");
    }
    
    clock_t start = clock();
    bool solved;
    
    if (use_optimized) {
        solved = solve_sudoku_optimized(&puzzle, &stats);
    } else {
        solved = solve_sudoku(&puzzle, &stats);
    }
    
    clock_t end = clock();
    stats.time_seconds = (double)(end - start) / CLOCKS_PER_SEC;
    
    /* Afișare rezultat */
    printf("\n");
    if (solved) {
        printf("  ✓ SOLUȚIE GĂSITĂ!\n\n");
        print_sudoku_grid(&puzzle, true);
    } else {
        printf("  ✗ Nu s-a găsit soluție!\n");
        printf("    (Verificați dacă puzzle-ul este valid)\n");
    }
    
    /* Afișare statistici */
    print_stats(&stats, puzzle.empty_cells);
    
    /* Output simplu pentru testare automată */
    if (solved) {
        printf("\n  Output pentru verificare automată:\n");
        print_sudoku_simple(&puzzle);
    }
    
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════════\n");
    printf("  Exercițiu completat! Verificați corectitudinea implementării.\n");
    printf("═══════════════════════════════════════════════════════════════════\n");
    printf("\n");
    
    return solved ? 0 : 1;
}
