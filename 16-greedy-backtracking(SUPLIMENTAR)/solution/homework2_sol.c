/**
 * =============================================================================
 * SOLUȚIE TEMA 2: Rezolvator de Labirint cu Backtracking
 * =============================================================================
 *
 * DESCRIERE:
 *   Implementare completă a unui rezolvator de labirint folosind tehnica
 *   backtracking. Programul găsește drumul de la punctul de start la
 *   punctul de destinație, evitând pereții.
 *
 * REPREZENTARE LABIRINT:
 *   - '#' sau '1' = perete (nu se poate trece)
 *   - '.' sau '0' = culoar (se poate trece)
 *   - 'S' = start
 *   - 'E' = end (destinație)
 *   - '*' = drumul găsit (în soluție)
 *
 * ALGORITM BACKTRACKING:
 *   1. Pornește de la poziția curentă (inițial S)
 *   2. Marchează poziția ca vizitată
 *   3. Dacă am ajuns la E, am găsit soluția
 *   4. Încearcă fiecare direcție (sus, dreapta, jos, stânga)
 *   5. Dacă direcția e validă și nevizitată, recursează
 *   6. Dacă recursiv găsim E, returnăm success
 *   7. Altfel, backtrack (demarcăm poziția)
 *
 * COMPLEXITATE:
 *   - Worst case: O(4^(n*m)) - toate celulele libere
 *   - Practic: mult mai bine cu pruning (visited check)
 *
 * VARIANTE IMPLEMENTATE:
 *   - DFS (Depth-First Search) - backtracking clasic
 *   - BFS (Breadth-First Search) - drum cel mai scurt
 *   - Toate drumurile posibile
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -o homework2_sol homework2_sol.c
 * RULARE:    ./homework2_sol [fisier_labirint.txt]
 *            ./homework2_sol -a   # toate drumurile
 *            ./homework2_sol -b   # BFS (cel mai scurt drum)
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

#define MAX_SIZE 100
#define MAX_PATHS 1000

/* Direcții de mișcare: sus, dreapta, jos, stânga */
const int DR[] = {-1, 0, 1, 0};
const int DC[] = {0, 1, 0, -1};
const char *DIR_NAMES[] = {"SUS", "DREAPTA", "JOS", "STÂNGA"};

/* Culori ANSI */
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
 * Structura pentru o poziție în labirint
 */
typedef struct {
    int row;
    int col;
} Position;

/**
 * Structura pentru un drum (secvență de poziții)
 */
typedef struct {
    Position steps[MAX_SIZE * MAX_SIZE];
    int length;
} Path;

/**
 * Structura pentru labirint
 */
typedef struct {
    char grid[MAX_SIZE][MAX_SIZE];       /* Grid-ul original */
    char solution[MAX_SIZE][MAX_SIZE];   /* Grid cu soluția */
    bool visited[MAX_SIZE][MAX_SIZE];    /* Celule vizitate */
    int rows;                            /* Număr de rânduri */
    int cols;                            /* Număr de coloane */
    Position start;                      /* Poziția de start */
    Position end;                        /* Poziția de destinație */
    bool has_start;                      /* Dacă s-a găsit start */
    bool has_end;                        /* Dacă s-a găsit end */
} Maze;

/**
 * Statistici pentru rezolvare
 */
typedef struct {
    unsigned long long nodes_visited;    /* Celule vizitate */
    unsigned long long backtracks;       /* Număr de backtrack-uri */
    int paths_found;                     /* Drumuri găsite */
    int shortest_path;                   /* Lungimea celui mai scurt drum */
    double time_seconds;                 /* Timp de execuție */
} SolverStats;

/**
 * Coadă pentru BFS
 */
typedef struct {
    Position items[MAX_SIZE * MAX_SIZE];
    int parent_row[MAX_SIZE * MAX_SIZE]; /* Pentru reconstrucție drum */
    int parent_col[MAX_SIZE * MAX_SIZE];
    int front;
    int rear;
} Queue;

/* =============================================================================
 * FUNCȚII PENTRU COADĂ (BFS)
 * =============================================================================
 */

void queue_init(Queue *q) {
    q->front = 0;
    q->rear = 0;
}

bool queue_is_empty(Queue *q) {
    return q->front == q->rear;
}

void queue_enqueue(Queue *q, Position pos, int parent_r, int parent_c) {
    q->items[q->rear] = pos;
    q->parent_row[q->rear] = parent_r;
    q->parent_col[q->rear] = parent_c;
    q->rear++;
}

Position queue_dequeue(Queue *q) {
    return q->items[q->front++];
}

/* =============================================================================
 * FUNCȚII I/O
 * =============================================================================
 */

/**
 * Citește un labirint dintr-un fișier
 * 
 * @param filename Numele fișierului
 * @param maze Pointer la structura Maze
 * @return true dacă citirea a reușit
 */
bool read_maze_from_file(const char *filename, Maze *maze) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Eroare: Nu s-a putut deschide '%s'\n", filename);
        return false;
    }
    
    maze->rows = 0;
    maze->cols = 0;
    maze->has_start = false;
    maze->has_end = false;
    
    char line[MAX_SIZE + 2];
    
    while (fgets(line, sizeof(line), file) && maze->rows < MAX_SIZE) {
        /* Ignorăm comentarii */
        if (line[0] == '#' && maze->rows == 0) continue;
        
        int len = strlen(line);
        while (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r')) {
            line[--len] = '\0';
        }
        
        if (len == 0) continue;
        
        /* Copiază linia și procesează caracterele */
        for (int c = 0; c < len && c < MAX_SIZE; c++) {
            char ch = line[c];
            
            /* Normalizăm caracterele */
            if (ch == '1' || ch == '#') {
                maze->grid[maze->rows][c] = '#';
            } else if (ch == '0' || ch == '.' || ch == ' ') {
                maze->grid[maze->rows][c] = '.';
            } else if (ch == 'S' || ch == 's') {
                maze->grid[maze->rows][c] = 'S';
                maze->start.row = maze->rows;
                maze->start.col = c;
                maze->has_start = true;
            } else if (ch == 'E' || ch == 'e' || ch == 'F' || ch == 'f') {
                maze->grid[maze->rows][c] = 'E';
                maze->end.row = maze->rows;
                maze->end.col = c;
                maze->has_end = true;
            } else {
                maze->grid[maze->rows][c] = ch;
            }
            
            maze->solution[maze->rows][c] = maze->grid[maze->rows][c];
            maze->visited[maze->rows][c] = false;
        }
        
        if (len > maze->cols) maze->cols = len;
        maze->rows++;
    }
    
    fclose(file);
    
    /* Verificări */
    if (!maze->has_start) {
        /* Implicit: colțul stânga-sus liber */
        maze->start.row = 0;
        maze->start.col = 0;
        maze->has_start = true;
    }
    
    if (!maze->has_end) {
        /* Implicit: colțul dreapta-jos liber */
        maze->end.row = maze->rows - 1;
        maze->end.col = maze->cols - 1;
        maze->has_end = true;
    }
    
    return maze->rows > 0;
}

/**
 * Generează un labirint de test
 */
void generate_test_maze(Maze *maze) {
    const char *test_maze[] = {
        "S.#....#..",
        ".###.#.#.#",
        "...#.#....",
        "##.#.####.",
        "...#......",
        ".#####.##.",
        ".......#..",
        "####.###.#",
        "........#.",
        "#.######.E"
    };
    
    maze->rows = 10;
    maze->cols = 10;
    maze->has_start = false;
    maze->has_end = false;
    
    for (int i = 0; i < maze->rows; i++) {
        for (int j = 0; j < maze->cols; j++) {
            maze->grid[i][j] = test_maze[i][j];
            maze->solution[i][j] = test_maze[i][j];
            maze->visited[i][j] = false;
            
            if (test_maze[i][j] == 'S') {
                maze->start.row = i;
                maze->start.col = j;
                maze->has_start = true;
            } else if (test_maze[i][j] == 'E') {
                maze->end.row = i;
                maze->end.col = j;
                maze->has_end = true;
            }
        }
    }
}

/* =============================================================================
 * FUNCȚII DE VALIDARE
 * =============================================================================
 */

/**
 * Verifică dacă o poziție este validă (în bounds și nu e perete)
 */
bool is_valid_position(Maze *maze, int row, int col) {
    if (row < 0 || row >= maze->rows) return false;
    if (col < 0 || col >= maze->cols) return false;
    if (maze->grid[row][col] == '#') return false;
    return true;
}

/**
 * Verifică dacă o poziție poate fi vizitată
 */
bool can_visit(Maze *maze, int row, int col) {
    return is_valid_position(maze, row, col) && !maze->visited[row][col];
}

/* =============================================================================
 * ALGORITM DFS (BACKTRACKING)
 * =============================================================================
 */

/**
 * Rezolvă labirintul folosind DFS (backtracking)
 * 
 * @param maze Pointer la labirint
 * @param row Rândul curent
 * @param col Coloana curentă
 * @param path Pointer la drumul curent
 * @param stats Pointer la statistici
 * @return true dacă s-a găsit drumul la E
 */
bool solve_maze_dfs(Maze *maze, int row, int col, Path *path, SolverStats *stats) {
    /* Verificăm dacă suntem în bounds și nu e perete */
    if (!can_visit(maze, row, col)) {
        return false;
    }
    
    if (stats) stats->nodes_visited++;
    
    /* Marchează ca vizitat */
    maze->visited[row][col] = true;
    
    /* Adaugă la drum */
    path->steps[path->length].row = row;
    path->steps[path->length].col = col;
    path->length++;
    
    /* Am ajuns la destinație? */
    if (row == maze->end.row && col == maze->end.col) {
        return true;
    }
    
    /* Încearcă fiecare direcție */
    for (int d = 0; d < 4; d++) {
        int new_row = row + DR[d];
        int new_col = col + DC[d];
        
        if (solve_maze_dfs(maze, new_row, new_col, path, stats)) {
            return true;
        }
    }
    
    /* Backtrack: nu am găsit drum */
    path->length--;
    maze->visited[row][col] = false;
    if (stats) stats->backtracks++;
    
    return false;
}

/* =============================================================================
 * ALGORITM BFS (CEL MAI SCURT DRUM)
 * =============================================================================
 */

/**
 * Găsește cel mai scurt drum folosind BFS
 * 
 * @param maze Pointer la labirint
 * @param path Pointer la drumul rezultat
 * @param stats Pointer la statistici
 * @return true dacă s-a găsit drum
 */
bool solve_maze_bfs(Maze *maze, Path *path, SolverStats *stats) {
    Queue queue;
    queue_init(&queue);
    
    /* Resetăm visited */
    for (int i = 0; i < maze->rows; i++) {
        for (int j = 0; j < maze->cols; j++) {
            maze->visited[i][j] = false;
        }
    }
    
    /* Matricea pentru reconstrucția drumului */
    int parent_row[MAX_SIZE][MAX_SIZE];
    int parent_col[MAX_SIZE][MAX_SIZE];
    
    for (int i = 0; i < maze->rows; i++) {
        for (int j = 0; j < maze->cols; j++) {
            parent_row[i][j] = -1;
            parent_col[i][j] = -1;
        }
    }
    
    /* Adăugăm start în coadă */
    queue_enqueue(&queue, maze->start, -1, -1);
    maze->visited[maze->start.row][maze->start.col] = true;
    
    bool found = false;
    
    while (!queue_is_empty(&queue)) {
        Position curr = queue_dequeue(&queue);
        
        if (stats) stats->nodes_visited++;
        
        /* Am ajuns la destinație? */
        if (curr.row == maze->end.row && curr.col == maze->end.col) {
            found = true;
            break;
        }
        
        /* Explorăm vecinii */
        for (int d = 0; d < 4; d++) {
            int new_row = curr.row + DR[d];
            int new_col = curr.col + DC[d];
            
            if (can_visit(maze, new_row, new_col)) {
                maze->visited[new_row][new_col] = true;
                parent_row[new_row][new_col] = curr.row;
                parent_col[new_row][new_col] = curr.col;
                
                Position next = {new_row, new_col};
                queue_enqueue(&queue, next, curr.row, curr.col);
            }
        }
    }
    
    if (!found) {
        return false;
    }
    
    /* Reconstruim drumul (de la end la start) */
    path->length = 0;
    int r = maze->end.row;
    int c = maze->end.col;
    
    while (r != -1 && c != -1) {
        path->steps[path->length].row = r;
        path->steps[path->length].col = c;
        path->length++;
        
        int pr = parent_row[r][c];
        int pc = parent_col[r][c];
        r = pr;
        c = pc;
    }
    
    /* Inversăm drumul (să fie de la start la end) */
    for (int i = 0; i < path->length / 2; i++) {
        Position temp = path->steps[i];
        path->steps[i] = path->steps[path->length - 1 - i];
        path->steps[path->length - 1 - i] = temp;
    }
    
    return true;
}

/* =============================================================================
 * GĂSEȘTE TOATE DRUMURILE
 * =============================================================================
 */

static Path all_paths[MAX_PATHS];
static int all_paths_count = 0;

/**
 * Găsește toate drumurile de la start la end
 */
void find_all_paths_helper(Maze *maze, int row, int col, Path *current, SolverStats *stats) {
    if (!can_visit(maze, row, col)) return;
    if (all_paths_count >= MAX_PATHS) return;
    
    if (stats) stats->nodes_visited++;
    
    maze->visited[row][col] = true;
    current->steps[current->length].row = row;
    current->steps[current->length].col = col;
    current->length++;
    
    if (row == maze->end.row && col == maze->end.col) {
        /* Am găsit un drum - salvăm o copie */
        memcpy(&all_paths[all_paths_count], current, sizeof(Path));
        all_paths_count++;
        if (stats) stats->paths_found++;
    } else {
        /* Continuăm căutarea */
        for (int d = 0; d < 4; d++) {
            find_all_paths_helper(maze, row + DR[d], col + DC[d], current, stats);
        }
    }
    
    /* Backtrack */
    current->length--;
    maze->visited[row][col] = false;
    if (stats) stats->backtracks++;
}

void find_all_paths(Maze *maze, SolverStats *stats) {
    all_paths_count = 0;
    
    /* Resetăm visited */
    for (int i = 0; i < maze->rows; i++) {
        for (int j = 0; j < maze->cols; j++) {
            maze->visited[i][j] = false;
        }
    }
    
    Path current;
    current.length = 0;
    
    find_all_paths_helper(maze, maze->start.row, maze->start.col, &current, stats);
}

/* =============================================================================
 * FUNCȚII DE AFIȘARE
 * =============================================================================
 */

/**
 * Afișează labirintul cu culori
 */
void print_maze(Maze *maze, bool use_colors) {
    printf("\n");
    
    /* Bordura de sus */
    printf("    ┌");
    for (int c = 0; c < maze->cols; c++) printf("──");
    printf("─┐\n");
    
    for (int r = 0; r < maze->rows; r++) {
        printf("    │ ");
        for (int c = 0; c < maze->cols; c++) {
            char ch = maze->solution[r][c];
            
            if (use_colors) {
                if (ch == '#') {
                    printf(ANSI_BLUE "██" ANSI_RESET);
                } else if (ch == '*') {
                    printf(ANSI_GREEN ANSI_BOLD "**" ANSI_RESET);
                } else if (ch == 'S') {
                    printf(ANSI_CYAN ANSI_BOLD "S " ANSI_RESET);
                } else if (ch == 'E') {
                    printf(ANSI_RED ANSI_BOLD "E " ANSI_RESET);
                } else {
                    printf(". ");
                }
            } else {
                printf("%c ", ch);
            }
        }
        printf("│\n");
    }
    
    /* Bordura de jos */
    printf("    └");
    for (int c = 0; c < maze->cols; c++) printf("──");
    printf("─┘\n");
    printf("\n");
}

/**
 * Aplică drumul pe soluție
 */
void apply_path_to_solution(Maze *maze, Path *path) {
    /* Resetăm soluția */
    for (int r = 0; r < maze->rows; r++) {
        for (int c = 0; c < maze->cols; c++) {
            maze->solution[r][c] = maze->grid[r][c];
        }
    }
    
    /* Marcăm drumul */
    for (int i = 0; i < path->length; i++) {
        int r = path->steps[i].row;
        int c = path->steps[i].col;
        
        if (maze->grid[r][c] != 'S' && maze->grid[r][c] != 'E') {
            maze->solution[r][c] = '*';
        }
    }
}

/**
 * Afișează drumul ca secvență de pași
 */
void print_path_steps(Path *path) {
    printf("Drum (%d pași): ", path->length);
    
    for (int i = 0; i < path->length; i++) {
        printf("(%d,%d)", path->steps[i].row, path->steps[i].col);
        if (i < path->length - 1) printf(" → ");
    }
    printf("\n");
}

/**
 * Afișează statisticile
 */
void print_stats_maze(SolverStats *stats, const char *algorithm) {
    printf("\n┌─────────────────────────────────────────┐\n");
    printf("│       STATISTICI - %-20s │\n", algorithm);
    printf("├─────────────────────────────────────────┤\n");
    printf("│ Celule vizitate:    %18llu │\n", stats->nodes_visited);
    printf("│ Backtrack-uri:      %18llu │\n", stats->backtracks);
    printf("│ Drumuri găsite:     %18d │\n", stats->paths_found);
    if (stats->shortest_path > 0) {
        printf("│ Cel mai scurt drum: %18d │\n", stats->shortest_path);
    }
    printf("│ Timp execuție:      %15.6f s │\n", stats->time_seconds);
    printf("└─────────────────────────────────────────┘\n");
}

/**
 * Afișează legendă
 */
void print_legend(void) {
    printf("Legendă:\n");
    printf("  " ANSI_CYAN ANSI_BOLD "S" ANSI_RESET "  = Start\n");
    printf("  " ANSI_RED ANSI_BOLD "E" ANSI_RESET "  = End (destinație)\n");
    printf("  " ANSI_BLUE "██" ANSI_RESET " = Perete\n");
    printf("  " ANSI_GREEN ANSI_BOLD "**" ANSI_RESET " = Drum găsit\n");
    printf("  .  = Culoar liber\n");
}

/**
 * Afișează help
 */
void print_help(const char *program_name) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║           REZOLVATOR DE LABIRINT - BACKTRACKING               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    printf("Utilizare: %s [opțiuni] [fișier]\n", program_name);
    printf("\n");
    printf("Opțiuni:\n");
    printf("  -b, --bfs        Folosește BFS (cel mai scurt drum)\n");
    printf("  -a, --all        Găsește toate drumurile posibile\n");
    printf("  -s, --simple     Output simplu (fără culori)\n");
    printf("  -v, --verbose    Afișează statistici detaliate\n");
    printf("  -h, --help       Afișează acest ajutor\n");
    printf("\n");
    printf("Format fișier:\n");
    printf("  # sau 1 = perete\n");
    printf("  . sau 0 = culoar\n");
    printf("  S = start\n");
    printf("  E = end\n");
    printf("\n");
    printf("Exemplu:\n");
    printf("  S.#...\n");
    printf("  .###..\n");
    printf("  .....E\n");
    printf("\n");
}

/* =============================================================================
 * FUNCȚIA MAIN
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    Maze maze;
    Path path;
    SolverStats stats = {0, 0, 0, 0, 0.0};
    
    bool use_bfs = false;
    bool find_all = false;
    bool use_colors = true;
    bool verbose = false;
    char *filename = NULL;
    
    /* Parsare argumente */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "--bfs") == 0) {
            use_bfs = true;
        } else if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--all") == 0) {
            find_all = true;
        } else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--simple") == 0) {
            use_colors = false;
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            verbose = true;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_help(argv[0]);
            return 0;
        } else if (argv[i][0] != '-') {
            filename = argv[i];
        }
    }
    
    /* Citire labirint */
    if (filename) {
        if (!read_maze_from_file(filename, &maze)) {
            fprintf(stderr, "Eroare la citirea labirintului!\n");
            return 1;
        }
    } else {
        printf("Folosim labirint de test...\n");
        generate_test_maze(&maze);
    }
    
    /* Header */
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║              REZOLVATOR DE LABIRINT - BACKTRACKING                            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════════════════╝\n");
    
    /* Afișare labirint original */
    printf("\n── Labirint Original (%d x %d) ──\n", maze.rows, maze.cols);
    print_maze(&maze, use_colors);
    print_legend();
    
    printf("\nStart: (%d, %d)\n", maze.start.row, maze.start.col);
    printf("End:   (%d, %d)\n", maze.end.row, maze.end.col);
    
    /* Rezolvare */
    clock_t start_time = clock();
    bool found = false;
    
    if (find_all) {
        /* Găsește toate drumurile */
        printf("\n── Căutare Toate Drumurile (DFS Backtracking) ──\n");
        
        find_all_paths(&maze, verbose ? &stats : NULL);
        
        clock_t end_time = clock();
        stats.time_seconds = (double)(end_time - start_time) / CLOCKS_PER_SEC;
        
        if (all_paths_count > 0) {
            found = true;
            printf("\n✓ S-au găsit %d drumuri!\n", all_paths_count);
            
            /* Găsim cel mai scurt */
            int shortest_idx = 0;
            for (int i = 1; i < all_paths_count; i++) {
                if (all_paths[i].length < all_paths[shortest_idx].length) {
                    shortest_idx = i;
                }
            }
            stats.shortest_path = all_paths[shortest_idx].length;
            
            /* Afișăm primele 5 drumuri */
            int show_count = (all_paths_count < 5) ? all_paths_count : 5;
            printf("\nPrimele %d drumuri:\n", show_count);
            
            for (int i = 0; i < show_count; i++) {
                printf("\nDrumul %d (lungime %d):\n", i + 1, all_paths[i].length);
                apply_path_to_solution(&maze, &all_paths[i]);
                print_maze(&maze, use_colors);
            }
            
            printf("\nCel mai scurt drum (lungime %d):\n", all_paths[shortest_idx].length);
            apply_path_to_solution(&maze, &all_paths[shortest_idx]);
            print_maze(&maze, use_colors);
            print_path_steps(&all_paths[shortest_idx]);
        }
        
    } else if (use_bfs) {
        /* BFS - cel mai scurt drum */
        printf("\n── Căutare cu BFS (Cel Mai Scurt Drum) ──\n");
        
        path.length = 0;
        found = solve_maze_bfs(&maze, &path, verbose ? &stats : NULL);
        
        clock_t end_time = clock();
        stats.time_seconds = (double)(end_time - start_time) / CLOCKS_PER_SEC;
        
        if (found) {
            stats.paths_found = 1;
            stats.shortest_path = path.length;
            
            printf("\n✓ Drum găsit! Lungime: %d\n", path.length);
            apply_path_to_solution(&maze, &path);
            print_maze(&maze, use_colors);
            print_path_steps(&path);
        }
        
    } else {
        /* DFS - backtracking clasic */
        printf("\n── Căutare cu DFS (Backtracking) ──\n");
        
        /* Resetăm visited */
        for (int i = 0; i < maze.rows; i++) {
            for (int j = 0; j < maze.cols; j++) {
                maze.visited[i][j] = false;
            }
        }
        
        path.length = 0;
        found = solve_maze_dfs(&maze, maze.start.row, maze.start.col, &path,
                               verbose ? &stats : NULL);
        
        clock_t end_time = clock();
        stats.time_seconds = (double)(end_time - start_time) / CLOCKS_PER_SEC;
        
        if (found) {
            stats.paths_found = 1;
            stats.shortest_path = path.length;
            
            printf("\n✓ Drum găsit! Lungime: %d\n", path.length);
            apply_path_to_solution(&maze, &path);
            print_maze(&maze, use_colors);
            print_path_steps(&path);
        }
    }
    
    /* Rezultat */
    if (!found) {
        printf("\n✗ Nu există drum de la Start la End!\n");
    }
    
    /* Statistici */
    if (verbose) {
        const char *alg_name = find_all ? "ALL PATHS" : (use_bfs ? "BFS" : "DFS");
        print_stats_maze(&stats, alg_name);
    }
    
    /* Rezumat */
    printf("\n");
    printf("┌─────────────────────────────────────────────────────────────────┐\n");
    printf("│                        REZUMAT FINAL                            │\n");
    printf("├─────────────────────────────────────────────────────────────────┤\n");
    printf("│ Labirint:     %d x %d                                           \n", maze.rows, maze.cols);
    printf("│ Algoritm:     %-48s │\n", find_all ? "DFS (Toate Drumurile)" : (use_bfs ? "BFS (Cel Mai Scurt)" : "DFS (Backtracking)"));
    printf("│ Status:       %-48s │\n", found ? "REZOLVAT" : "FĂRĂ SOLUȚIE");
    if (found) {
        printf("│ Drumuri:      %-48d │\n", stats.paths_found);
        if (stats.shortest_path > 0) {
            printf("│ Lungime min:  %-48d │\n", stats.shortest_path);
        }
    }
    printf("└─────────────────────────────────────────────────────────────────┘\n");
    printf("\n");
    
    return found ? 0 : 1;
}
