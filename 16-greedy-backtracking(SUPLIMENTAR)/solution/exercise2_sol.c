/**
 * =============================================================================
 * SOLUȚIE EXERCIȚIUL 2: Sudoku Solver cu Backtracking
 * =============================================================================
 *
 * DESCRIERE:
 *   Implementare completă a algoritmului de rezolvare Sudoku folosind
 *   tehnica backtracking. Include optimizări precum MCV (Most Constrained
 *   Variable) și statistici detaliate.
 *
 * ALGORITM:
 *   1. Găsim prima celulă goală (sau cea mai constrânsă)
 *   2. Încercăm fiecare cifră 1-9
 *   3. Verificăm dacă plasarea este validă (rând, coloană, box 3x3)
 *   4. Dacă da, plasăm și recursăm
 *   5. Dacă recursiv găsim soluție, returnăm true
 *   6. Dacă nu, backtrack (resetăm celula) și încercăm următoarea cifră
 *   7. Dacă nicio cifră nu merge, returnăm false
 *
 * COMPLEXITATE:
 *   - Worst case: O(9^81) - toate celulele goale, toate combinațiile
 *   - Practic: mult mai bine datorită pruning-ului (constrângeri)
 *   - Cu MCV: reduce dramatic spațiul de căutare
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -o exercise2_sol exercise2_sol.c
 * RULARE:    ./exercise2_sol [fisier_sudoku.txt]
 *            ./exercise2_sol -v [fisier_sudoku.txt]    # verbose
 *            ./exercise2_sol -o [fisier_sudoku.txt]    # optimizat (MCV)
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

/* =============================================================================
 * CONSTANTE ȘI CONFIGURARE
 * =============================================================================
 */

#define SIZE 9           /* Dimensiunea grid-ului Sudoku */
#define SUBGRID 3        /* Dimensiunea subgrid-ului (box) */
#define EMPTY 0          /* Valoarea pentru celulă goală */

/* Culori ANSI pentru output */
#define ANSI_RED     "\x1b[31m"
#define ANSI_GREEN   "\x1b[32m"
#define ANSI_YELLOW  "\x1b[33m"
#define ANSI_BLUE    "\x1b[34m"
#define ANSI_MAGENTA "\x1b[35m"
#define ANSI_CYAN    "\x1b[36m"
#define ANSI_BOLD    "\x1b[1m"
#define ANSI_RESET   "\x1b[0m"

/* =============================================================================
 * STRUCTURI DE DATE
 * =============================================================================
 */

/**
 * Statistici pentru procesul de rezolvare
 */
typedef struct {
    unsigned long long nodes_visited;     /* Noduri vizitate în arborele de căutare */
    unsigned long long backtracks;        /* Număr de backtrack-uri efectuate */
    unsigned long long valid_placements;  /* Plasări valide încercate */
    unsigned long long invalid_checks;    /* Verificări care au eșuat */
    double time_seconds;                  /* Timp total de execuție */
} SolverStats;

/**
 * Structura pentru puzzle-ul Sudoku
 */
typedef struct {
    int grid[SIZE][SIZE];         /* Grid-ul curent (modificabil) */
    int original[SIZE][SIZE];     /* Grid-ul original (pentru evidențiere) */
    int empty_cells;              /* Număr de celule goale */
    bool solved;                  /* Flag dacă a fost rezolvat */
} SudokuPuzzle;

/* =============================================================================
 * FUNCȚII DE VERIFICARE VALIDITATE
 * =============================================================================
 */

/**
 * Verifică dacă un număr poate fi plasat pe un rând
 * 
 * @param grid Grid-ul Sudoku
 * @param row Rândul de verificat
 * @param num Numărul de verificat
 * @return true dacă num NU apare pe rând, false altfel
 */
bool is_valid_in_row(int grid[SIZE][SIZE], int row, int num) {
    for (int col = 0; col < SIZE; col++) {
        if (grid[row][col] == num) {
            return false;  /* Numărul există deja pe acest rând */
        }
    }
    return true;
}

/**
 * Verifică dacă un număr poate fi plasat pe o coloană
 * 
 * @param grid Grid-ul Sudoku
 * @param col Coloana de verificat
 * @param num Numărul de verificat
 * @return true dacă num NU apare pe coloană, false altfel
 */
bool is_valid_in_col(int grid[SIZE][SIZE], int col, int num) {
    for (int row = 0; row < SIZE; row++) {
        if (grid[row][col] == num) {
            return false;  /* Numărul există deja pe această coloană */
        }
    }
    return true;
}

/**
 * Verifică dacă un număr poate fi plasat într-un box 3x3
 * 
 * @param grid Grid-ul Sudoku
 * @param row Rândul celulei
 * @param col Coloana celulei
 * @param num Numărul de verificat
 * @return true dacă num NU apare în box, false altfel
 */
bool is_valid_in_box(int grid[SIZE][SIZE], int row, int col, int num) {
    /* Calculăm colțul stânga-sus al box-ului 3x3 */
    int box_row = (row / SUBGRID) * SUBGRID;
    int box_col = (col / SUBGRID) * SUBGRID;
    
    for (int i = 0; i < SUBGRID; i++) {
        for (int j = 0; j < SUBGRID; j++) {
            if (grid[box_row + i][box_col + j] == num) {
                return false;  /* Numărul există deja în box */
            }
        }
    }
    return true;
}

/**
 * Verifică dacă o plasare este validă (combină toate cele 3 verificări)
 * 
 * @param grid Grid-ul Sudoku
 * @param row Rândul celulei
 * @param col Coloana celulei
 * @param num Numărul de verificat
 * @return true dacă plasarea este validă, false altfel
 */
bool is_valid_placement(int grid[SIZE][SIZE], int row, int col, int num) {
    return is_valid_in_row(grid, row, num) &&
           is_valid_in_col(grid, col, num) &&
           is_valid_in_box(grid, row, col, num);
}

/* =============================================================================
 * FUNCȚII DE CĂUTARE CELULE
 * =============================================================================
 */

/**
 * Găsește prima celulă goală (parcurgere linie cu linie)
 * 
 * @param grid Grid-ul Sudoku
 * @param row Pointer pentru a returna rândul găsit
 * @param col Pointer pentru a returna coloana găsită
 * @return true dacă s-a găsit o celulă goală, false altfel
 */
bool find_empty_cell(int grid[SIZE][SIZE], int *row, int *col) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (grid[i][j] == EMPTY) {
                *row = i;
                *col = j;
                return true;
            }
        }
    }
    return false;  /* Nicio celulă goală - puzzle rezolvat */
}

/**
 * Numără opțiunile valide pentru o celulă
 * 
 * @param grid Grid-ul Sudoku
 * @param row Rândul celulei
 * @param col Coloana celulei
 * @return Numărul de cifre care pot fi plasate valid
 */
int count_valid_options(int grid[SIZE][SIZE], int row, int col) {
    int count = 0;
    for (int num = 1; num <= 9; num++) {
        if (is_valid_placement(grid, row, col, num)) {
            count++;
        }
    }
    return count;
}

/**
 * Găsește celula cu cele mai puține opțiuni valide (MCV - Most Constrained Variable)
 * Aceasta este o euristică importantă care reduce dramatic spațiul de căutare.
 * 
 * @param grid Grid-ul Sudoku
 * @param row Pointer pentru a returna rândul găsit
 * @param col Pointer pentru a returna coloana găsită
 * @return true dacă s-a găsit o celulă goală, false altfel
 */
bool find_most_constrained_cell(int grid[SIZE][SIZE], int *row, int *col) {
    int min_options = 10;  /* Mai mare decât maximul posibil (9) */
    bool found = false;
    
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (grid[i][j] == EMPTY) {
                int options = count_valid_options(grid, i, j);
                
                /* Dacă găsim o celulă fără opțiuni, puzzle-ul e invalid */
                if (options == 0) {
                    *row = i;
                    *col = j;
                    return true;  /* Returnăm această celulă - backtracking imediat */
                }
                
                if (options < min_options) {
                    min_options = options;
                    *row = i;
                    *col = j;
                    found = true;
                }
            }
        }
    }
    
    return found;
}

/* =============================================================================
 * ALGORITMI DE REZOLVARE
 * =============================================================================
 */

/**
 * Rezolvă puzzle-ul Sudoku folosind backtracking standard
 * 
 * Algoritmul:
 *   1. Găsește prima celulă goală
 *   2. Dacă nu există, puzzle-ul e rezolvat
 *   3. Încearcă fiecare cifră 1-9
 *   4. Dacă plasarea e validă, plasează și recursează
 *   5. Dacă recursiv găsim soluție, returnăm true
 *   6. Altfel, backtrack (resetăm celula)
 *   7. Dacă nicio cifră nu merge, returnăm false
 * 
 * @param grid Grid-ul Sudoku de rezolvat (modificat in-place)
 * @param stats Pointer pentru statistici (poate fi NULL)
 * @return true dacă s-a găsit soluție, false altfel
 */
bool solve_sudoku(int grid[SIZE][SIZE], SolverStats *stats) {
    int row, col;
    
    /* Incrementăm nodurile vizitate */
    if (stats) stats->nodes_visited++;
    
    /* Pasul 1: Găsește o celulă goală */
    if (!find_empty_cell(grid, &row, &col)) {
        return true;  /* Nicio celulă goală - puzzle rezolvat! */
    }
    
    /* Pasul 2: Încearcă fiecare cifră 1-9 */
    for (int num = 1; num <= 9; num++) {
        /* Pasul 3: Verifică dacă plasarea e validă */
        if (is_valid_placement(grid, row, col, num)) {
            if (stats) stats->valid_placements++;
            
            /* Pasul 4: Plasează cifra */
            grid[row][col] = num;
            
            /* Pasul 5: Recursează */
            if (solve_sudoku(grid, stats)) {
                return true;  /* Soluție găsită! */
            }
            
            /* Pasul 6: Backtrack - cifra nu a dus la soluție */
            grid[row][col] = EMPTY;
            if (stats) stats->backtracks++;
        } else {
            if (stats) stats->invalid_checks++;
        }
    }
    
    /* Pasul 7: Nicio cifră nu a funcționat */
    return false;
}

/**
 * Rezolvă puzzle-ul Sudoku folosind backtracking cu MCV
 * Versiune optimizată care selectează celula cu cele mai puține opțiuni.
 * 
 * @param grid Grid-ul Sudoku de rezolvat (modificat in-place)
 * @param stats Pointer pentru statistici (poate fi NULL)
 * @return true dacă s-a găsit soluție, false altfel
 */
bool solve_sudoku_optimized(int grid[SIZE][SIZE], SolverStats *stats) {
    int row, col;
    
    if (stats) stats->nodes_visited++;
    
    /* Găsește celula cu cele mai puține opțiuni valide */
    if (!find_most_constrained_cell(grid, &row, &col)) {
        return true;  /* Nicio celulă goală - puzzle rezolvat! */
    }
    
    /* Încearcă fiecare cifră 1-9 */
    for (int num = 1; num <= 9; num++) {
        if (is_valid_placement(grid, row, col, num)) {
            if (stats) stats->valid_placements++;
            
            grid[row][col] = num;
            
            if (solve_sudoku_optimized(grid, stats)) {
                return true;
            }
            
            grid[row][col] = EMPTY;
            if (stats) stats->backtracks++;
        } else {
            if (stats) stats->invalid_checks++;
        }
    }
    
    return false;
}

/* =============================================================================
 * FUNCȚII I/O
 * =============================================================================
 */

/**
 * Citește un puzzle Sudoku dintr-un fișier
 * Format: 9 linii cu 9 cifre fiecare (0 sau . pentru celule goale)
 * 
 * @param filename Numele fișierului
 * @param puzzle Pointer la structura SudokuPuzzle
 * @return true dacă citirea a reușit, false altfel
 */
bool read_puzzle_from_file(const char *filename, SudokuPuzzle *puzzle) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Eroare: Nu s-a putut deschide fișierul '%s'\n", filename);
        return false;
    }
    
    puzzle->empty_cells = 0;
    puzzle->solved = false;
    
    char line[256];
    int row = 0;
    
    while (fgets(line, sizeof(line), file) && row < SIZE) {
        /* Ignorăm linii goale și comentarii */
        if (line[0] == '\n' || line[0] == '#') continue;
        
        int col = 0;
        for (int i = 0; line[i] != '\0' && col < SIZE; i++) {
            char c = line[i];
            if (c >= '1' && c <= '9') {
                puzzle->grid[row][col] = c - '0';
                puzzle->original[row][col] = c - '0';
                col++;
            } else if (c == '0' || c == '.') {
                puzzle->grid[row][col] = EMPTY;
                puzzle->original[row][col] = EMPTY;
                puzzle->empty_cells++;
                col++;
            }
            /* Ignorăm alte caractere (spații, separatori) */
        }
        
        if (col == SIZE) row++;
    }
    
    fclose(file);
    
    if (row != SIZE) {
        fprintf(stderr, "Eroare: Fișierul nu conține un puzzle valid (găsite %d rânduri)\n", row);
        return false;
    }
    
    return true;
}

/**
 * Citește un puzzle Sudoku de la stdin
 * Format: 9 linii cu 9 cifre fiecare
 * 
 * @param puzzle Pointer la structura SudokuPuzzle
 * @return true dacă citirea a reușit, false altfel
 */
bool read_puzzle_from_stdin(SudokuPuzzle *puzzle) {
    puzzle->empty_cells = 0;
    puzzle->solved = false;
    
    char line[256];
    int row = 0;
    
    printf("Introduceți puzzle-ul Sudoku (9 linii, 0 sau . pentru celule goale):\n");
    
    while (fgets(line, sizeof(line), stdin) && row < SIZE) {
        if (line[0] == '\n' || line[0] == '#') continue;
        
        int col = 0;
        for (int i = 0; line[i] != '\0' && col < SIZE; i++) {
            char c = line[i];
            if (c >= '1' && c <= '9') {
                puzzle->grid[row][col] = c - '0';
                puzzle->original[row][col] = c - '0';
                col++;
            } else if (c == '0' || c == '.') {
                puzzle->grid[row][col] = EMPTY;
                puzzle->original[row][col] = EMPTY;
                puzzle->empty_cells++;
                col++;
            }
        }
        
        if (col == SIZE) row++;
    }
    
    return row == SIZE;
}

/* =============================================================================
 * FUNCȚII DE AFIȘARE
 * =============================================================================
 */

/**
 * Afișează grid-ul Sudoku cu borduri și evidențiere
 * Celulele completate de solver sunt afișate în verde
 * 
 * @param puzzle Pointer la puzzle (pentru comparație original vs soluție)
 * @param show_colors Dacă să folosim culori ANSI
 */
void print_sudoku_grid(SudokuPuzzle *puzzle, bool show_colors) {
    printf("\n");
    printf("    ╔═══════════════════════════╗\n");
    
    for (int i = 0; i < SIZE; i++) {
        if (i > 0 && i % 3 == 0) {
            printf("    ╠═══════════════════════════╣\n");
        }
        
        printf("    ║ ");
        
        for (int j = 0; j < SIZE; j++) {
            if (j > 0 && j % 3 == 0) {
                printf("│ ");
            }
            
            int val = puzzle->grid[i][j];
            bool was_original = puzzle->original[i][j] != EMPTY;
            
            if (val == EMPTY) {
                printf(". ");
            } else if (show_colors && !was_original) {
                /* Cifră adăugată de solver - afișare în verde */
                printf(ANSI_GREEN ANSI_BOLD "%d" ANSI_RESET " ", val);
            } else {
                printf("%d ", val);
            }
        }
        
        printf("║\n");
    }
    
    printf("    ╚═══════════════════════════╝\n");
    printf("\n");
}

/**
 * Afișează grid-ul în format simplu (pentru testare automată)
 * 
 * @param grid Grid-ul Sudoku
 */
void print_grid_simple(int grid[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            printf("%d", grid[i][j]);
            if (j < SIZE - 1) printf(" ");
        }
        printf("\n");
    }
}

/**
 * Afișează statisticile de rezolvare
 * 
 * @param stats Pointer la structura de statistici
 */
void print_stats(SolverStats *stats) {
    printf("┌─────────────────────────────────────────┐\n");
    printf("│          STATISTICI REZOLVARE          │\n");
    printf("├─────────────────────────────────────────┤\n");
    printf("│ Noduri vizitate:    %18llu │\n", stats->nodes_visited);
    printf("│ Plasări valide:     %18llu │\n", stats->valid_placements);
    printf("│ Verificări invalide:%18llu │\n", stats->invalid_checks);
    printf("│ Backtrack-uri:      %18llu │\n", stats->backtracks);
    printf("│ Timp execuție:      %15.6f s │\n", stats->time_seconds);
    printf("└─────────────────────────────────────────┘\n");
}

/**
 * Afișează help-ul pentru utilizare
 */
void print_help(const char *program_name) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║              SUDOKU SOLVER - BACKTRACKING                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    printf("Utilizare: %s [opțiuni] [fișier]\n", program_name);
    printf("\n");
    printf("Opțiuni:\n");
    printf("  -v, --verbose    Afișează statistici detaliate\n");
    printf("  -o, --optimized  Folosește euristică MCV (Most Constrained Variable)\n");
    printf("  -s, --simple     Output simplu (pentru testare automată)\n");
    printf("  -h, --help       Afișează acest ajutor\n");
    printf("\n");
    printf("Exemple:\n");
    printf("  %s sudoku_easy.txt           # Rezolvă puzzle din fișier\n", program_name);
    printf("  %s -v -o sudoku_hard.txt     # Verbose, optimizat\n", program_name);
    printf("  %s                           # Citește de la stdin\n", program_name);
    printf("\n");
}

/* =============================================================================
 * FUNCȚIA MAIN
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    SudokuPuzzle puzzle;
    SolverStats stats = {0, 0, 0, 0, 0.0};
    
    bool verbose = false;
    bool optimized = false;
    bool simple_output = false;
    char *filename = NULL;
    
    /* Parsare argumente */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            verbose = true;
        } else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--optimized") == 0) {
            optimized = true;
        } else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--simple") == 0) {
            simple_output = true;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_help(argv[0]);
            return 0;
        } else if (argv[i][0] != '-') {
            filename = argv[i];
        }
    }
    
    /* Citire puzzle */
    bool read_ok;
    if (filename) {
        read_ok = read_puzzle_from_file(filename, &puzzle);
    } else {
        read_ok = read_puzzle_from_stdin(&puzzle);
    }
    
    if (!read_ok) {
        fprintf(stderr, "Eroare la citirea puzzle-ului!\n");
        return 1;
    }
    
    /* Afișare header (dacă nu e output simplu) */
    if (!simple_output) {
        printf("\n");
        printf("╔═══════════════════════════════════════════════════════════════╗\n");
        printf("║           SUDOKU SOLVER - BACKTRACKING                        ║\n");
        printf("╚═══════════════════════════════════════════════════════════════╝\n");
        
        printf("\n── Puzzle Original (%d celule goale) ──\n", puzzle.empty_cells);
        print_sudoku_grid(&puzzle, false);
    }
    
    /* Rezolvare cu timing */
    clock_t start = clock();
    
    bool solved;
    if (optimized) {
        solved = solve_sudoku_optimized(puzzle.grid, verbose ? &stats : NULL);
    } else {
        solved = solve_sudoku(puzzle.grid, verbose ? &stats : NULL);
    }
    
    clock_t end = clock();
    stats.time_seconds = (double)(end - start) / CLOCKS_PER_SEC;
    
    puzzle.solved = solved;
    
    /* Afișare rezultat */
    if (simple_output) {
        /* Output simplu pentru testare automată */
        if (solved) {
            print_grid_simple(puzzle.grid);
        } else {
            printf("NO_SOLUTION\n");
        }
    } else {
        if (solved) {
            printf("── Soluție Găsită! ──\n");
            printf("(Cifrele verzi au fost adăugate de solver)\n");
            print_sudoku_grid(&puzzle, true);
            
            if (verbose) {
                printf("\n");
                print_stats(&stats);
            }
            
            printf("\n✓ Puzzle rezolvat cu succes!\n");
        } else {
            printf("\n✗ Puzzle-ul nu are soluție!\n");
            
            if (verbose) {
                printf("\n");
                print_stats(&stats);
            }
        }
    }
    
    return solved ? 0 : 1;
}
