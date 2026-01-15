/**
 * =============================================================================
 * SOLUȚIE TEMA 1: Problema Rucsacului Fracționar - Variante
 * =============================================================================
 *
 * DESCRIERE:
 *   Implementare completă a problemei rucsacului fracționar (Fractional
 *   Knapsack) cu multiple variante și extensii:
 *   - Varianta clasică: maximizare valoare cu greutate maximă W
 *   - Varianta cu obiecte obligatorii: unele obiecte TREBUIE incluse
 *   - Varianta cu categorii: limite pe tipuri de obiecte
 *   - Analiza detaliată a soluției
 *
 * PARADIGMĂ: GREEDY
 *   Proprietatea greedy: alegerea obiectului cu cel mai mare raport
 *   valoare/greutate garantează soluția optimă pentru varianta fracționară.
 *
 * ALGORITM:
 *   1. Sortăm obiectele descrescător după raportul value/weight
 *   2. Parcurgem obiectele în ordine și luăm cât putem
 *   3. Dacă nu încape complet, luăm fracțiune
 *
 * COMPLEXITATE: O(n log n) - dominată de sortare
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -o homework1_sol homework1_sol.c -lm
 * RULARE:    ./homework1_sol [fisier_obiecte.txt]
 *            ./homework1_sol -m [fisier.txt]   # cu obiecte obligatorii
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

/* =============================================================================
 * CONSTANTE
 * =============================================================================
 */

#define MAX_ITEMS 1000
#define MAX_NAME_LEN 64
#define MAX_LINE_LEN 256

/* Culori ANSI */
#define ANSI_GREEN   "\x1b[32m"
#define ANSI_YELLOW  "\x1b[33m"
#define ANSI_CYAN    "\x1b[36m"
#define ANSI_BOLD    "\x1b[1m"
#define ANSI_RESET   "\x1b[0m"

/* =============================================================================
 * STRUCTURI DE DATE
 * =============================================================================
 */

/**
 * Structura pentru un obiect din rucsac
 */
typedef struct {
    int id;                     /* ID unic al obiectului */
    char name[MAX_NAME_LEN];    /* Numele obiectului */
    double weight;              /* Greutatea obiectului */
    double value;               /* Valoarea obiectului */
    double ratio;               /* Raportul value/weight (calculat) */
    bool mandatory;             /* Dacă obiectul este obligatoriu */
    char category[32];          /* Categoria obiectului (opțional) */
} Item;

/**
 * Structura pentru un obiect în soluție (cu fracțiune)
 */
typedef struct {
    int item_id;                /* ID-ul obiectului */
    double fraction;            /* Fracțiunea luată (0.0 - 1.0) */
    double weight_taken;        /* Greutatea efectiv luată */
    double value_gained;        /* Valoarea obținută */
} SelectedItem;

/**
 * Structura pentru soluția completă
 */
typedef struct {
    SelectedItem items[MAX_ITEMS];  /* Obiectele selectate */
    int count;                      /* Număr de obiecte în soluție */
    double total_weight;            /* Greutatea totală */
    double total_value;             /* Valoarea totală */
    double capacity_used;           /* Procentul de capacitate utilizat */
} KnapsackSolution;

/**
 * Statistici pentru rezolvare
 */
typedef struct {
    int items_considered;       /* Obiecte luate în considerare */
    int items_taken_full;       /* Obiecte luate complet */
    int items_taken_partial;    /* Obiecte luate parțial */
    int mandatory_items;        /* Obiecte obligatorii */
    double wasted_capacity;     /* Capacitate neutilizată */
} SolverStats;

/* =============================================================================
 * FUNCȚII DE COMPARARE PENTRU QSORT
 * =============================================================================
 */

/**
 * Compară două obiecte după raportul value/weight (descrescător)
 * Aceasta este cheia algoritmului greedy.
 */
int compare_by_ratio(const void *a, const void *b) {
    const Item *item_a = (const Item *)a;
    const Item *item_b = (const Item *)b;
    
    /* Obiectele obligatorii au prioritate maximă */
    if (item_a->mandatory && !item_b->mandatory) return -1;
    if (!item_a->mandatory && item_b->mandatory) return 1;
    
    /* Sortare descrescătoare după raport */
    if (item_b->ratio > item_a->ratio) return 1;
    if (item_b->ratio < item_a->ratio) return -1;
    return 0;
}

/**
 * Compară după valoare (pentru rapoarte)
 */
int compare_by_value(const void *a, const void *b) {
    const Item *item_a = (const Item *)a;
    const Item *item_b = (const Item *)b;
    
    if (item_b->value > item_a->value) return 1;
    if (item_b->value < item_a->value) return -1;
    return 0;
}

/* =============================================================================
 * FUNCȚII I/O
 * =============================================================================
 */

/**
 * Citește obiectele dintr-un fișier
 * Format: nume greutate valoare [M pentru mandatory] [categoria]
 * 
 * @param filename Numele fișierului
 * @param items Array-ul de obiecte (output)
 * @param count Pointer pentru numărul de obiecte citite
 * @return true dacă citirea a reușit
 */
bool read_items_from_file(const char *filename, Item *items, int *count) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Eroare: Nu s-a putut deschide '%s'\n", filename);
        return false;
    }
    
    char line[MAX_LINE_LEN];
    *count = 0;
    
    while (fgets(line, sizeof(line), file) && *count < MAX_ITEMS) {
        /* Ignorăm comentarii și linii goale */
        if (line[0] == '#' || line[0] == '\n') continue;
        
        Item *item = &items[*count];
        item->id = *count + 1;
        item->mandatory = false;
        strcpy(item->category, "general");
        
        /* Parsăm linia */
        char mandatory_flag = ' ';
        int parsed = sscanf(line, "%63s %lf %lf %c %31s",
                           item->name, &item->weight, &item->value,
                           &mandatory_flag, item->category);
        
        if (parsed >= 3) {
            item->mandatory = (mandatory_flag == 'M' || mandatory_flag == 'm');
            item->ratio = (item->weight > 0) ? item->value / item->weight : 0;
            (*count)++;
        }
    }
    
    fclose(file);
    return *count > 0;
}

/**
 * Generează obiecte de test
 */
void generate_test_items(Item *items, int *count) {
    struct {
        const char *name;
        double weight;
        double value;
        bool mandatory;
    } test_data[] = {
        {"Laptop",      2.5, 2000, false},
        {"Telefon",     0.3,  800, true},   /* Obligatoriu */
        {"Tableta",     0.8,  600, false},
        {"Ceas_Smart",  0.1,  300, false},
        {"Casti",       0.2,  150, false},
        {"PowerBank",   0.4,  100, false},
        {"Incarcator",  0.3,   50, true},   /* Obligatoriu */
        {"Carte",       0.5,   80, false},
        {"Umbrela",     0.6,   40, false},
        {"Lanterna",    0.2,   30, false},
        {"Aparat_Foto", 1.2, 1500, false},
        {"Obiectiv",    0.8,  800, false},
        {"Trepied",     1.5,  200, false}
    };
    
    *count = sizeof(test_data) / sizeof(test_data[0]);
    
    for (int i = 0; i < *count; i++) {
        items[i].id = i + 1;
        strcpy(items[i].name, test_data[i].name);
        items[i].weight = test_data[i].weight;
        items[i].value = test_data[i].value;
        items[i].mandatory = test_data[i].mandatory;
        items[i].ratio = items[i].value / items[i].weight;
        strcpy(items[i].category, "general");
    }
}

/* =============================================================================
 * ALGORITMUL GREEDY - FRACTIONAL KNAPSACK
 * =============================================================================
 */

/**
 * Rezolvă problema rucsacului fracționar folosind algoritmul greedy
 * 
 * DEMONSTRAȚIE CORECTITUDINE:
 *   Fie S* soluția optimă și S soluția greedy. Presupunem S ≠ S*.
 *   Fie i primul obiect unde diferă. În S, am luat obiectul cu cel mai
 *   mare ratio disponibil. Putem înlocui ce a luat S* cu ce a luat S
 *   fără a reduce valoarea totală. Deci S este la fel de bună sau mai bună.
 * 
 * @param items Array-ul de obiecte (va fi sortat!)
 * @param n Numărul de obiecte
 * @param capacity Capacitatea rucsacului
 * @param solution Pointer la structura soluție (output)
 * @param stats Pointer la statistici (poate fi NULL)
 * @return Valoarea totală obținută
 */
double fractional_knapsack(Item *items, int n, double capacity,
                          KnapsackSolution *solution, SolverStats *stats) {
    
    /* Inițializare soluție */
    solution->count = 0;
    solution->total_weight = 0;
    solution->total_value = 0;
    
    /* Inițializare statistici */
    if (stats) {
        stats->items_considered = 0;
        stats->items_taken_full = 0;
        stats->items_taken_partial = 0;
        stats->mandatory_items = 0;
    }
    
    /* Pasul 1: Calculăm rapoartele și sortăm */
    for (int i = 0; i < n; i++) {
        items[i].ratio = items[i].value / items[i].weight;
    }
    qsort(items, n, sizeof(Item), compare_by_ratio);
    
    double remaining = capacity;
    
    /* Pasul 2: Mai întâi adăugăm obiectele obligatorii */
    for (int i = 0; i < n; i++) {
        if (items[i].mandatory) {
            if (items[i].weight <= remaining) {
                /* Luăm obiectul complet */
                SelectedItem *sel = &solution->items[solution->count++];
                sel->item_id = items[i].id;
                sel->fraction = 1.0;
                sel->weight_taken = items[i].weight;
                sel->value_gained = items[i].value;
                
                remaining -= items[i].weight;
                solution->total_weight += items[i].weight;
                solution->total_value += items[i].value;
                
                if (stats) {
                    stats->mandatory_items++;
                    stats->items_taken_full++;
                }
                
                /* Marcăm ca procesat */
                items[i].weight = -1;
            } else {
                /* Nu încape - problemă! */
                fprintf(stderr, "Avertisment: Obiectul obligatoriu '%s' nu încape!\n",
                       items[i].name);
            }
        }
    }
    
    /* Pasul 3: Parcurgem restul obiectelor în ordinea raportului */
    for (int i = 0; i < n; i++) {
        /* Sărim obiectele deja procesate sau cu greutate 0 */
        if (items[i].weight <= 0) continue;
        
        if (stats) stats->items_considered++;
        
        if (remaining <= 0) break;  /* Rucsac plin */
        
        if (items[i].weight <= remaining) {
            /* Obiectul încape complet */
            SelectedItem *sel = &solution->items[solution->count++];
            sel->item_id = items[i].id;
            sel->fraction = 1.0;
            sel->weight_taken = items[i].weight;
            sel->value_gained = items[i].value;
            
            remaining -= items[i].weight;
            solution->total_weight += items[i].weight;
            solution->total_value += items[i].value;
            
            if (stats) stats->items_taken_full++;
        } else {
            /* Luăm doar o fracțiune */
            double fraction = remaining / items[i].weight;
            
            SelectedItem *sel = &solution->items[solution->count++];
            sel->item_id = items[i].id;
            sel->fraction = fraction;
            sel->weight_taken = remaining;
            sel->value_gained = items[i].value * fraction;
            
            solution->total_weight += remaining;
            solution->total_value += items[i].value * fraction;
            remaining = 0;
            
            if (stats) stats->items_taken_partial++;
        }
    }
    
    /* Calculăm procentul de capacitate utilizat */
    solution->capacity_used = (solution->total_weight / capacity) * 100;
    
    if (stats) {
        stats->wasted_capacity = capacity - solution->total_weight;
    }
    
    return solution->total_value;
}

/* =============================================================================
 * VARIANTA: 0/1 KNAPSACK (PENTRU COMPARAȚIE)
 * =============================================================================
 */

/**
 * Rezolvă problema 0/1 Knapsack folosind programare dinamică
 * (Nu este greedy! Inclus pentru comparație.)
 * 
 * @param items Array-ul de obiecte
 * @param n Numărul de obiecte
 * @param capacity Capacitatea (convertită la int)
 * @return Valoarea maximă (fără fracții)
 */
double knapsack_01_dp(Item *items, int n, int capacity) {
    /* Alocăm tabela DP */
    double **dp = (double **)malloc((n + 1) * sizeof(double *));
    for (int i = 0; i <= n; i++) {
        dp[i] = (double *)calloc(capacity + 1, sizeof(double));
    }
    
    /* Completăm tabela */
    for (int i = 1; i <= n; i++) {
        int w = (int)items[i-1].weight;
        double v = items[i-1].value;
        
        for (int j = 0; j <= capacity; j++) {
            /* Nu luăm obiectul i */
            dp[i][j] = dp[i-1][j];
            
            /* Luăm obiectul i (dacă încape) */
            if (w <= j && dp[i-1][j-w] + v > dp[i][j]) {
                dp[i][j] = dp[i-1][j-w] + v;
            }
        }
    }
    
    double result = dp[n][capacity];
    
    /* Eliberăm memoria */
    for (int i = 0; i <= n; i++) {
        free(dp[i]);
    }
    free(dp);
    
    return result;
}

/* =============================================================================
 * FUNCȚII DE AFIȘARE
 * =============================================================================
 */

/**
 * Afișează lista de obiecte disponibile
 */
void print_items(Item *items, int n) {
    printf("\n┌────┬────────────────┬──────────┬──────────┬──────────┬─────────┐\n");
    printf("│ ID │ Nume           │ Greutate │ Valoare  │ Ratio    │ Obligat │\n");
    printf("├────┼────────────────┼──────────┼──────────┼──────────┼─────────┤\n");
    
    for (int i = 0; i < n; i++) {
        printf("│ %2d │ %-14s │ %8.2f │ %8.2f │ %8.2f │   %c     │\n",
               items[i].id, items[i].name, items[i].weight,
               items[i].value, items[i].ratio,
               items[i].mandatory ? 'X' : ' ');
    }
    
    printf("└────┴────────────────┴──────────┴──────────┴──────────┴─────────┘\n");
}

/**
 * Afișează soluția găsită
 */
void print_solution(KnapsackSolution *solution, Item *original_items, int n) {
    printf("\n┌────────────────────────────────────────────────────────────────┐\n");
    printf("│                      SOLUȚIE OPTIMĂ                            │\n");
    printf("├────┬────────────────┬──────────┬──────────┬──────────┬─────────┤\n");
    printf("│ ID │ Obiect         │ Fracție  │ Greutate │ Valoare  │ Status  │\n");
    printf("├────┼────────────────┼──────────┼──────────┼──────────┼─────────┤\n");
    
    for (int i = 0; i < solution->count; i++) {
        SelectedItem *sel = &solution->items[i];
        
        /* Găsim numele obiectului */
        const char *name = "?";
        bool mandatory = false;
        for (int j = 0; j < n; j++) {
            if (original_items[j].id == sel->item_id) {
                name = original_items[j].name;
                mandatory = original_items[j].mandatory;
                break;
            }
        }
        
        const char *status;
        if (mandatory) {
            status = "OBLIG";
        } else if (sel->fraction == 1.0) {
            status = "FULL";
        } else {
            status = "PART";
        }
        
        printf("│ %2d │ %-14s │ %6.1f%% │ %8.2f │ %8.2f │ %-7s │\n",
               sel->item_id, name,
               sel->fraction * 100, sel->weight_taken,
               sel->value_gained, status);
    }
    
    printf("├────┴────────────────┴──────────┼──────────┼──────────┼─────────┤\n");
    printf("│ TOTAL                          │ %8.2f │ %8.2f │ %5.1f%% │\n",
           solution->total_weight, solution->total_value, solution->capacity_used);
    printf("└────────────────────────────────┴──────────┴──────────┴─────────┘\n");
}

/**
 * Afișează statisticile de rezolvare
 */
void print_stats_knapsack(SolverStats *stats) {
    printf("\n┌─────────────────────────────────────────┐\n");
    printf("│           STATISTICI REZOLVARE          │\n");
    printf("├─────────────────────────────────────────┤\n");
    printf("│ Obiecte considerate:    %15d │\n", stats->items_considered);
    printf("│ Obiecte luate complet:  %15d │\n", stats->items_taken_full);
    printf("│ Obiecte luate parțial:  %15d │\n", stats->items_taken_partial);
    printf("│ Obiecte obligatorii:    %15d │\n", stats->mandatory_items);
    printf("│ Capacitate rămasă:      %15.2f │\n", stats->wasted_capacity);
    printf("└─────────────────────────────────────────┘\n");
}

/**
 * Afișează comparație Fractional vs 0/1 Knapsack
 */
void print_comparison(double frac_value, double dp_value, double capacity) {
    double diff = frac_value - dp_value;
    double diff_percent = (diff / frac_value) * 100;
    
    printf("\n┌─────────────────────────────────────────────────────────────────┐\n");
    printf("│            COMPARAȚIE: GREEDY vs PROGRAMARE DINAMICĂ            │\n");
    printf("├─────────────────────────────────────────────────────────────────┤\n");
    printf("│                                                                 │\n");
    printf("│   Capacitate rucsac: %.2f kg                                    \n", capacity);
    printf("│                                                                 │\n");
    printf("│   ┌─────────────────────┬─────────────────┐                     │\n");
    printf("│   │ Algoritm            │ Valoare Max     │                     │\n");
    printf("│   ├─────────────────────┼─────────────────┤                     │\n");
    printf("│   │ Fractional (Greedy) │ %13.2f   │                     │\n", frac_value);
    printf("│   │ 0/1 (DP)            │ %13.2f   │                     │\n", dp_value);
    printf("│   ├─────────────────────┼─────────────────┤                     │\n");
    printf("│   │ Diferență           │ %13.2f   │ (%.1f%%)              \n", diff, diff_percent);
    printf("│   └─────────────────────┴─────────────────┘                     │\n");
    printf("│                                                                 │\n");
    printf("│   Notă: Fractional permite fracționare și dă mereu              │\n");
    printf("│         valoare >= 0/1 Knapsack                                 │\n");
    printf("│                                                                 │\n");
    printf("└─────────────────────────────────────────────────────────────────┘\n");
}

/**
 * Afișează vizualizarea rucsacului
 */
void print_knapsack_visual(KnapsackSolution *solution, double capacity) {
    printf("\n┌─────────────────────────────────────────────────────────────────┐\n");
    printf("│                   VIZUALIZARE RUCSAC                            │\n");
    printf("├─────────────────────────────────────────────────────────────────┤\n");
    printf("│                                                                 │\n");
    printf("│   Capacitate totală: %.2f kg                                    \n", capacity);
    printf("│                                                                 │\n");
    printf("│   [");
    
    /* Afișăm bara de progres */
    int bar_width = 50;
    int filled = (int)(solution->capacity_used * bar_width / 100);
    
    for (int i = 0; i < bar_width; i++) {
        if (i < filled) {
            printf("█");
        } else {
            printf("░");
        }
    }
    
    printf("] %.1f%%\n", solution->capacity_used);
    printf("│                                                                 │\n");
    printf("│   Utilizat: %.2f kg / %.2f kg                                   \n",
           solution->total_weight, capacity);
    printf("│   Valoare:  %.2f lei                                            \n",
           solution->total_value);
    printf("│                                                                 │\n");
    printf("└─────────────────────────────────────────────────────────────────┘\n");
}

/**
 * Afișează help
 */
void print_help(const char *program_name) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║         PROBLEMA RUCSACULUI FRACȚIONAR - GREEDY               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    printf("Utilizare: %s [opțiuni] [fișier]\n", program_name);
    printf("\n");
    printf("Opțiuni:\n");
    printf("  -c <capacitate>  Setează capacitatea rucsacului (implicit: 5.0)\n");
    printf("  -m               Include obiecte obligatorii\n");
    printf("  -d               Compară cu 0/1 Knapsack (DP)\n");
    printf("  -v               Afișare verbosă cu statistici\n");
    printf("  -h               Afișează acest ajutor\n");
    printf("\n");
    printf("Format fișier:\n");
    printf("  # Comentariu\n");
    printf("  NumeObiect Greutate Valoare [M] [Categorie]\n");
    printf("  Laptop 2.5 2000\n");
    printf("  Telefon 0.3 800 M  # M = obligatoriu\n");
    printf("\n");
}

/* =============================================================================
 * FUNCȚIA MAIN
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    Item items[MAX_ITEMS];
    Item items_copy[MAX_ITEMS];  /* Copie pentru comparație */
    int n = 0;
    
    double capacity = 5.0;
    bool verbose = false;
    bool compare_dp = false;
    char *filename = NULL;
    
    /* Parsare argumente */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-c") == 0 && i + 1 < argc) {
            capacity = atof(argv[++i]);
        } else if (strcmp(argv[i], "-v") == 0) {
            verbose = true;
        } else if (strcmp(argv[i], "-d") == 0) {
            compare_dp = true;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_help(argv[0]);
            return 0;
        } else if (argv[i][0] != '-') {
            filename = argv[i];
        }
    }
    
    /* Citire obiecte */
    if (filename) {
        if (!read_items_from_file(filename, items, &n)) {
            fprintf(stderr, "Eroare la citirea fișierului!\n");
            return 1;
        }
    } else {
        printf("Folosim date de test...\n");
        generate_test_items(items, &n);
    }
    
    /* Facem copie pentru DP (sortarea modifică array-ul) */
    memcpy(items_copy, items, n * sizeof(Item));
    
    /* Header */
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║         PROBLEMA RUCSACULUI FRACȚIONAR - ALGORITM GREEDY                      ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════════════════╝\n");
    
    /* Afișare obiecte disponibile */
    printf("\n── Obiecte Disponibile (%d) ──", n);
    print_items(items, n);
    
    /* Rezolvare */
    KnapsackSolution solution;
    SolverStats stats;
    
    printf("\n── Rezolvare (Capacitate: %.2f kg) ──\n", capacity);
    
    double frac_value = fractional_knapsack(items, n, capacity, &solution,
                                            verbose ? &stats : NULL);
    
    /* Afișare soluție */
    print_solution(&solution, items_copy, n);
    
    /* Vizualizare */
    print_knapsack_visual(&solution, capacity);
    
    /* Statistici */
    if (verbose) {
        print_stats_knapsack(&stats);
    }
    
    /* Comparație cu DP */
    if (compare_dp) {
        double dp_value = knapsack_01_dp(items_copy, n, (int)capacity);
        print_comparison(frac_value, dp_value, capacity);
    }
    
    /* Rezumat final */
    printf("\n");
    printf("┌─────────────────────────────────────────────────────────────────┐\n");
    printf("│                        REZUMAT FINAL                            │\n");
    printf("├─────────────────────────────────────────────────────────────────┤\n");
    printf("│ ✓ Valoare maximă obținută: " ANSI_GREEN "%.2f lei" ANSI_RESET "                           \n", solution.total_value);
    printf("│ ✓ Greutate utilizată: %.2f / %.2f kg (%.1f%%)                   \n",
           solution.total_weight, capacity, solution.capacity_used);
    printf("│ ✓ Obiecte în rucsac: %d                                         \n", solution.count);
    printf("└─────────────────────────────────────────────────────────────────┘\n");
    printf("\n");
    
    return 0;
}
