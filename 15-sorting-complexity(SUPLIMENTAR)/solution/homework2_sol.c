/**
 * =============================================================================
 * SOLUȚIE TEMA 2: Shell Sort cu Analiza Secvențelor de Gap
 * Săptămâna 15: Algoritmi de Sortare Avansați
 * =============================================================================
 *
 * Acest program implementează Shell Sort cu 4 secvențe de gap diferite
 * și analizează empiric performanța fiecăreia pe diferite tipuri de date.
 *
 * Secvențe implementate:
 *   1. Shell original (1959): N/2, N/4, ..., 1
 *   2. Hibbard (1963): 2^k - 1
 *   3. Sedgewick (1986): 4^k + 3*2^(k-1) + 1
 *   4. Ciura (2001): 1, 4, 10, 23, 57, 132, 301, 701, ...
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -O2 -o homework2 homework2_sol.c -lm
 * Execuție:  ./homework2 10000
 *            ./homework2 --test
 *            ./homework2 --visual
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

/* =============================================================================
 * PARTEA 1: CONSTANTE ȘI TIPURI DE DATE
 * =============================================================================
 */

#define MAX_GAPS 50         /* Numărul maxim de gap-uri într-o secvență */
#define NUM_TRIALS 3        /* Numărul de rulări pentru medie */

/**
 * Enum pentru tipurile de secvențe de gap
 */
typedef enum {
    GAP_SHELL_ORIGINAL,
    GAP_HIBBARD,
    GAP_SEDGEWICK,
    GAP_CIURA
} GapSequenceType;

/**
 * Structură pentru o secvență de gap-uri
 */
typedef struct {
    int gaps[MAX_GAPS];     /* Array-ul de gap-uri */
    int count;              /* Numărul de gap-uri */
    const char *name;       /* Numele secvenței */
} GapSequence;

/**
 * Structură pentru statistici de sortare
 */
typedef struct {
    long long comparisons;
    long long moves;
    double time_ms;
} SortStats;

/**
 * Structură pentru rezultatele benchmark-ului
 */
typedef struct {
    GapSequenceType type;
    SortStats random;
    SortStats sorted;
    SortStats reversed;
    SortStats nearly_sorted;
} BenchmarkResult;

/* Secvența Ciura empirică (optim dovedită experimental) */
static const int CIURA_GAPS[] = {1, 4, 10, 23, 57, 132, 301, 701, 1750, 3937, 
                                  8858, 19930, 44842, 100894, 227011, 510774,
                                  1149241, 2585793, 5818032};
static const int CIURA_COUNT = sizeof(CIURA_GAPS) / sizeof(CIURA_GAPS[0]);

/* =============================================================================
 * PARTEA 2: GENERARE SECVENȚE DE GAP
 * =============================================================================
 */

/**
 * Generează secvența originală Shell: n/2, n/4, ..., 1
 * Complexitate: O(n²) worst case
 */
void generate_shell_original(GapSequence *seq, int n) {
    seq->name = "Shell Original";
    seq->count = 0;
    
    for (int gap = n / 2; gap >= 1; gap /= 2) {
        seq->gaps[seq->count++] = gap;
        if (seq->count >= MAX_GAPS) break;
    }
}

/**
 * Generează secvența Hibbard: 2^k - 1 = 1, 3, 7, 15, 31, ...
 * Complexitate: O(n^(3/2)) worst case
 */
void generate_hibbard(GapSequence *seq, int n) {
    seq->name = "Hibbard";
    seq->count = 0;
    
    /* Găsim toate valorile 2^k - 1 < n */
    int temp_gaps[MAX_GAPS];
    int temp_count = 0;
    
    for (int k = 1; ; k++) {
        int gap = (1 << k) - 1;  /* 2^k - 1 */
        if (gap >= n) break;
        temp_gaps[temp_count++] = gap;
        if (temp_count >= MAX_GAPS) break;
    }
    
    /* Inversăm pentru ordine descrescătoare */
    for (int i = temp_count - 1; i >= 0; i--) {
        seq->gaps[seq->count++] = temp_gaps[i];
    }
}

/**
 * Generează secvența Sedgewick: 4^k + 3*2^(k-1) + 1 sau 9*4^(k-1) - 9*2^(k-1) + 1
 * Complexitate: O(n^(4/3)) worst case
 */
void generate_sedgewick(GapSequence *seq, int n) {
    seq->name = "Sedgewick";
    seq->count = 0;
    
    int temp_gaps[MAX_GAPS];
    int temp_count = 0;
    
    /* Formula Sedgewick combinată */
    for (int k = 0; ; k++) {
        int gap;
        if (k == 0) {
            gap = 1;
        } else if (k % 2 == 1) {
            /* 8*2^k - 6*2^((k+1)/2) + 1 */
            int pow2k = 1 << k;
            int pow2half = 1 << ((k + 1) / 2);
            gap = 8 * pow2k - 6 * pow2half + 1;
        } else {
            /* 9*(2^k - 2^(k/2)) + 1 */
            int pow2k = 1 << k;
            int pow2half = 1 << (k / 2);
            gap = 9 * (pow2k - pow2half) + 1;
        }
        
        if (gap >= n) break;
        temp_gaps[temp_count++] = gap;
        if (temp_count >= MAX_GAPS) break;
    }
    
    /* Inversăm pentru ordine descrescătoare */
    for (int i = temp_count - 1; i >= 0; i--) {
        seq->gaps[seq->count++] = temp_gaps[i];
    }
}

/**
 * Generează secvența Ciura (empirică)
 * Complexitate: aproximativ O(n^1.3)
 */
void generate_ciura(GapSequence *seq, int n) {
    seq->name = "Ciura";
    seq->count = 0;
    
    /* Găsim cel mai mare gap din secvența Ciura care e < n */
    int start_idx = 0;
    for (int i = CIURA_COUNT - 1; i >= 0; i--) {
        if (CIURA_GAPS[i] < n) {
            start_idx = i;
            break;
        }
    }
    
    /* Copiem gap-urile în ordine descrescătoare */
    for (int i = start_idx; i >= 0; i--) {
        seq->gaps[seq->count++] = CIURA_GAPS[i];
    }
}

/**
 * Generează o secvență de gap în funcție de tip
 */
void generate_gap_sequence(GapSequence *seq, GapSequenceType type, int n) {
    switch (type) {
        case GAP_SHELL_ORIGINAL:
            generate_shell_original(seq, n);
            break;
        case GAP_HIBBARD:
            generate_hibbard(seq, n);
            break;
        case GAP_SEDGEWICK:
            generate_sedgewick(seq, n);
            break;
        case GAP_CIURA:
            generate_ciura(seq, n);
            break;
    }
}

/**
 * Afișează o secvență de gap
 */
void print_gap_sequence(const GapSequence *seq) {
    printf("%s (%d gaps): ", seq->name, seq->count);
    for (int i = 0; i < seq->count && i < 10; i++) {
        printf("%d", seq->gaps[i]);
        if (i < seq->count - 1 && i < 9) printf(", ");
    }
    if (seq->count > 10) printf(", ...");
    printf("\n");
}

/* =============================================================================
 * PARTEA 3: SHELL SORT GENERIC
 * =============================================================================
 */

/**
 * Shell Sort parametrizat cu secvență de gap
 *
 * @param arr Array-ul de sortat
 * @param n Dimensiunea
 * @param seq Secvența de gap-uri
 * @param stats Statistici de ieșire
 */
void shell_sort(int arr[], int n, const GapSequence *seq, SortStats *stats) {
    stats->comparisons = 0;
    stats->moves = 0;
    
    /* Parcurgem fiecare gap din secvență */
    for (int g = 0; g < seq->count; g++) {
        int gap = seq->gaps[g];
        
        /* Insertion Sort cu pas 'gap' */
        for (int i = gap; i < n; i++) {
            int temp = arr[i];
            stats->moves++;
            
            int j = i;
            while (j >= gap && arr[j - gap] > temp) {
                stats->comparisons++;
                arr[j] = arr[j - gap];
                stats->moves++;
                j -= gap;
            }
            
            if (j >= gap) stats->comparisons++;  /* Ultima comparație */
            
            arr[j] = temp;
            stats->moves++;
        }
    }
}

/* =============================================================================
 * PARTEA 4: GENERARE DATE DE TEST
 * =============================================================================
 */

/**
 * Generează un array cu valori aleatorii
 */
void generate_random(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % 1000000;
    }
}

/**
 * Generează un array sortat
 */
void generate_sorted(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        arr[i] = i;
    }
}

/**
 * Generează un array inversat
 */
void generate_reversed(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        arr[i] = n - i;
    }
}

/**
 * Generează un array aproape sortat (10% dezordine)
 */
void generate_nearly_sorted(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        arr[i] = i;
    }
    
    /* Permutăm 10% din elemente */
    int swaps = n / 10;
    for (int i = 0; i < swaps; i++) {
        int a = rand() % n;
        int b = rand() % n;
        int temp = arr[a];
        arr[a] = arr[b];
        arr[b] = temp;
    }
}

/**
 * Copiază un array
 */
void copy_array(int dest[], const int src[], int n) {
    for (int i = 0; i < n; i++) {
        dest[i] = src[i];
    }
}

/**
 * Verifică dacă array-ul este sortat
 */
bool is_sorted(const int arr[], int n) {
    for (int i = 1; i < n; i++) {
        if (arr[i] < arr[i - 1]) return false;
    }
    return true;
}

/**
 * Returnează timpul curent în milisecunde
 */
double get_time_ms(void) {
    return (double)clock() * 1000.0 / CLOCKS_PER_SEC;
}

/* =============================================================================
 * PARTEA 5: BENCHMARK
 * =============================================================================
 */

/**
 * Rulează un benchmark pentru o secvență pe un tip de date
 */
SortStats benchmark_single(const int original[], int n, const GapSequence *seq) {
    int *arr = (int*)malloc(n * sizeof(int));
    SortStats total = {0, 0, 0.0};
    
    if (arr == NULL) return total;
    
    for (int trial = 0; trial < NUM_TRIALS; trial++) {
        copy_array(arr, original, n);
        
        SortStats stats = {0, 0, 0.0};
        double start = get_time_ms();
        
        shell_sort(arr, n, seq, &stats);
        
        stats.time_ms = get_time_ms() - start;
        
        total.comparisons += stats.comparisons;
        total.moves += stats.moves;
        total.time_ms += stats.time_ms;
    }
    
    /* Calculăm media */
    total.comparisons /= NUM_TRIALS;
    total.moves /= NUM_TRIALS;
    total.time_ms /= NUM_TRIALS;
    
    free(arr);
    return total;
}

/**
 * Rulează benchmark complet pentru o secvență
 */
BenchmarkResult run_benchmark(GapSequenceType type, int n) {
    BenchmarkResult result;
    result.type = type;
    
    /* Generăm secvența de gap */
    GapSequence seq;
    generate_gap_sequence(&seq, type, n);
    
    /* Alocăm array-uri pentru fiecare tip de date */
    int *random_arr = (int*)malloc(n * sizeof(int));
    int *sorted_arr = (int*)malloc(n * sizeof(int));
    int *reversed_arr = (int*)malloc(n * sizeof(int));
    int *nearly_sorted_arr = (int*)malloc(n * sizeof(int));
    
    if (!random_arr || !sorted_arr || !reversed_arr || !nearly_sorted_arr) {
        free(random_arr);
        free(sorted_arr);
        free(reversed_arr);
        free(nearly_sorted_arr);
        return result;
    }
    
    /* Generăm datele */
    srand(42);  /* Seed fix pentru reproducibilitate */
    generate_random(random_arr, n);
    generate_sorted(sorted_arr, n);
    generate_reversed(reversed_arr, n);
    generate_nearly_sorted(nearly_sorted_arr, n);
    
    /* Rulăm benchmark-urile */
    result.random = benchmark_single(random_arr, n, &seq);
    result.sorted = benchmark_single(sorted_arr, n, &seq);
    result.reversed = benchmark_single(reversed_arr, n, &seq);
    result.nearly_sorted = benchmark_single(nearly_sorted_arr, n, &seq);
    
    free(random_arr);
    free(sorted_arr);
    free(reversed_arr);
    free(nearly_sorted_arr);
    
    return result;
}

/**
 * Afișează rezultatele benchmark-ului într-un tabel
 */
void print_benchmark_results(BenchmarkResult results[], int count, int n) {
    const char *names[] = {"Shell Orig.", "Hibbard", "Sedgewick", "Ciura"};
    
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════════════════════\n");
    printf("                    SHELL SORT - ANALIZA SECVENȚELOR DE GAP\n");
    printf("═══════════════════════════════════════════════════════════════════════════════\n");
    printf("\n");
    printf("Dimensiune: %d elemente | Rulări mediate: %d\n\n", n, NUM_TRIALS);
    
    /* Tabel cu timpii */
    printf("┌─────────────┬──────────────┬──────────────┬──────────────┬──────────────┐\n");
    printf("│  Secvență   │    Random    │    Sorted    │   Reversed   │ Nearly Sort. │\n");
    printf("├─────────────┼──────────────┼──────────────┼──────────────┼──────────────┤\n");
    
    for (int i = 0; i < count; i++) {
        printf("│ %-11s │ %9.3f ms │ %9.3f ms │ %9.3f ms │ %9.3f ms │\n",
               names[results[i].type],
               results[i].random.time_ms,
               results[i].sorted.time_ms,
               results[i].reversed.time_ms,
               results[i].nearly_sorted.time_ms);
    }
    
    printf("└─────────────┴──────────────┴──────────────┴──────────────┴──────────────┘\n");
    
    /* Tabel cu comparații */
    printf("\nComparații (mii):\n");
    printf("┌─────────────┬──────────────┬──────────────┬──────────────┬──────────────┐\n");
    printf("│  Secvență   │    Random    │    Sorted    │   Reversed   │ Nearly Sort. │\n");
    printf("├─────────────┼──────────────┼──────────────┼──────────────┼──────────────┤\n");
    
    for (int i = 0; i < count; i++) {
        printf("│ %-11s │ %10.1f K │ %10.1f K │ %10.1f K │ %10.1f K │\n",
               names[results[i].type],
               results[i].random.comparisons / 1000.0,
               results[i].sorted.comparisons / 1000.0,
               results[i].reversed.comparisons / 1000.0,
               results[i].nearly_sorted.comparisons / 1000.0);
    }
    
    printf("└─────────────┴──────────────┴──────────────┴──────────────┴──────────────┘\n");
    
    /* Găsim cel mai rapid pentru fiecare tip de date */
    printf("\n🏆 Cea mai rapidă secvență pentru fiecare scenariu:\n");
    printf("───────────────────────────────────────────────────\n");
    
    const char *scenarios[] = {"Random", "Sorted", "Reversed", "Nearly Sorted"};
    
    for (int s = 0; s < 4; s++) {
        double best_time = 1e9;
        int best_idx = 0;
        
        for (int i = 0; i < count; i++) {
            double time;
            switch (s) {
                case 0: time = results[i].random.time_ms; break;
                case 1: time = results[i].sorted.time_ms; break;
                case 2: time = results[i].reversed.time_ms; break;
                case 3: time = results[i].nearly_sorted.time_ms; break;
                default: time = 1e9;
            }
            
            if (time < best_time) {
                best_time = time;
                best_idx = i;
            }
        }
        
        printf("  %-14s → %s (%.3f ms)\n", scenarios[s], names[results[best_idx].type], best_time);
    }
    
    /* Recomandare generală */
    double avg_times[4];
    for (int i = 0; i < count; i++) {
        avg_times[i] = (results[i].random.time_ms + 
                        results[i].sorted.time_ms + 
                        results[i].reversed.time_ms + 
                        results[i].nearly_sorted.time_ms) / 4.0;
    }
    
    int best_overall = 0;
    for (int i = 1; i < count; i++) {
        if (avg_times[i] < avg_times[best_overall]) {
            best_overall = i;
        }
    }
    
    printf("\n📊 Recomandare pentru uz general: %s\n", names[results[best_overall].type]);
    printf("   (Timp mediu: %.3f ms)\n", avg_times[best_overall]);
}

/* =============================================================================
 * PARTEA 6: VIZUALIZARE PROCES
 * =============================================================================
 */

/**
 * Afișează un array
 */
void print_array(const int arr[], int n, const char *label) {
    printf("%s: [", label);
    for (int i = 0; i < n; i++) {
        printf("%d", arr[i]);
        if (i < n - 1) printf(", ");
    }
    printf("]\n");
}

/**
 * Shell Sort cu vizualizare pas cu pas
 */
void shell_sort_visual(int arr[], int n, const GapSequence *seq) {
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("          SHELL SORT - VIZUALIZARE PAS CU PAS\n");
    printf("          Secvență: %s\n", seq->name);
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    print_array(arr, n, "Initial");
    printf("\nSecvența de gap-uri: ");
    for (int g = 0; g < seq->count; g++) {
        printf("%d", seq->gaps[g]);
        if (g < seq->count - 1) printf(" → ");
    }
    printf("\n\n");
    
    for (int g = 0; g < seq->count; g++) {
        int gap = seq->gaps[g];
        printf("────────────────────────────────────────\n");
        printf("Gap = %d\n", gap);
        printf("────────────────────────────────────────\n");
        
        for (int i = gap; i < n; i++) {
            int temp = arr[i];
            int j = i;
            
            /* Afișăm starea curentă */
            printf("  i=%d: Comparăm arr[%d]=%d cu elemente la distanță %d\n", 
                   i, i, arr[i], gap);
            
            bool moved = false;
            while (j >= gap && arr[j - gap] > temp) {
                arr[j] = arr[j - gap];
                j -= gap;
                moved = true;
            }
            arr[j] = temp;
            
            if (moved) {
                printf("       Mutat %d la poziția %d\n", temp, j);
                print_array(arr, n, "       Array");
            }
        }
        
        printf("\n  După gap=%d: ", gap);
        print_array(arr, n, "");
        printf("\n");
    }
    
    printf("════════════════════════════════════════════════════════════\n");
    print_array(arr, n, "FINAL");
    printf("Array sortat: %s\n", is_sorted(arr, n) ? "✓ DA" : "✗ NU");
    printf("════════════════════════════════════════════════════════════\n");
}

/**
 * Demonstrație vizuală cu array mic
 */
void demo_visual(void) {
    int arr[] = {64, 34, 25, 12, 22, 11, 90, 5};
    int n = sizeof(arr) / sizeof(arr[0]);
    
    GapSequence seq;
    generate_ciura(&seq, n);
    
    shell_sort_visual(arr, n, &seq);
}

/* =============================================================================
 * PARTEA 7: AFIȘARE GRAFIC ASCII
 * =============================================================================
 */

/**
 * Afișează un grafic ASCII simplu al timpilor
 */
void print_ascii_graph(BenchmarkResult results[], int count) {
    const char *names[] = {"Shell", "Hibb.", "Sedgw.", "Ciura"};
    const char *bars = "████████████████████████████████████████████████████████████";
    
    printf("\n📈 Grafic Comparativ (timp pe date Random):\n");
    printf("─────────────────────────────────────────────────────\n");
    
    /* Găsim maximul pentru scalare */
    double max_time = 0;
    for (int i = 0; i < count; i++) {
        if (results[i].random.time_ms > max_time) {
            max_time = results[i].random.time_ms;
        }
    }
    
    /* Afișăm barele */
    for (int i = 0; i < count; i++) {
        int bar_len = (int)(results[i].random.time_ms / max_time * 40);
        if (bar_len < 1) bar_len = 1;
        
        printf("%-6s │%.*s %.2f ms\n", 
               names[i], 
               bar_len, bars, 
               results[i].random.time_ms);
    }
    
    printf("─────────────────────────────────────────────────────\n");
}

/* =============================================================================
 * PARTEA 8: TESTE AUTOMATE
 * =============================================================================
 */

/**
 * Rulează teste de corectitudine
 */
void run_tests(void) {
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("                    TESTE DE CORECTITUDINE\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    int test_sizes[] = {10, 100, 1000, 5000};
    GapSequenceType types[] = {GAP_SHELL_ORIGINAL, GAP_HIBBARD, GAP_SEDGEWICK, GAP_CIURA};
    const char *type_names[] = {"Shell", "Hibbard", "Sedgewick", "Ciura"};
    
    int total_tests = 0;
    int passed_tests = 0;
    
    for (int s = 0; s < 4; s++) {
        int n = test_sizes[s];
        printf("Dimensiune: %d\n", n);
        
        for (int t = 0; t < 4; t++) {
            int *arr = (int*)malloc(n * sizeof(int));
            if (arr == NULL) continue;
            
            srand(42 + s * 100 + t);
            generate_random(arr, n);
            
            GapSequence seq;
            generate_gap_sequence(&seq, types[t], n);
            
            SortStats stats = {0, 0, 0};
            shell_sort(arr, n, &seq, &stats);
            
            bool correct = is_sorted(arr, n);
            total_tests++;
            if (correct) passed_tests++;
            
            printf("  %-10s: %s (%.2f ms, %lld comp)\n", 
                   type_names[t],
                   correct ? "✓ PASS" : "✗ FAIL",
                   stats.time_ms,
                   stats.comparisons);
            
            free(arr);
        }
        printf("\n");
    }
    
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("Rezultat: %d/%d teste trecute\n", passed_tests, total_tests);
    printf("═══════════════════════════════════════════════════════════════\n");
}

/* =============================================================================
 * PARTEA 9: MAIN
 * =============================================================================
 */

void print_usage(const char *prog) {
    printf("Utilizare:\n");
    printf("  %s <n>          - Rulează benchmark pentru n elemente\n", prog);
    printf("  %s --test       - Rulează teste de corectitudine\n", prog);
    printf("  %s --visual     - Demonstrație vizuală pas cu pas\n", prog);
    printf("  %s --sequences  - Afișează secvențele de gap\n", prog);
}

int main(int argc, char *argv[]) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║     TEMA 2: SHELL SORT CU ANALIZA SECVENȚELOR DE GAP                          ║\n");
    printf("║     Săptămâna 15: Algoritmi de Sortare Avansați                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }
    
    if (strcmp(argv[1], "--test") == 0) {
        run_tests();
        return 0;
    }
    
    if (strcmp(argv[1], "--visual") == 0) {
        demo_visual();
        return 0;
    }
    
    if (strcmp(argv[1], "--sequences") == 0) {
        int n = 10000;
        printf("Secvențe de gap pentru n = %d:\n\n", n);
        
        GapSequence seq;
        
        generate_shell_original(&seq, n);
        print_gap_sequence(&seq);
        
        generate_hibbard(&seq, n);
        print_gap_sequence(&seq);
        
        generate_sedgewick(&seq, n);
        print_gap_sequence(&seq);
        
        generate_ciura(&seq, n);
        print_gap_sequence(&seq);
        
        return 0;
    }
    
    /* Benchmark cu dimensiunea specificată */
    int n = atoi(argv[1]);
    if (n <= 0) {
        printf("Eroare: Dimensiunea trebuie să fie pozitivă!\n");
        return 1;
    }
    
    printf("Rulează benchmark pentru n = %d...\n", n);
    
    /* Afișăm secvențele folosite */
    printf("\nSecvențele de gap utilizate:\n");
    printf("────────────────────────────\n");
    
    GapSequence seq;
    GapSequenceType types[] = {GAP_SHELL_ORIGINAL, GAP_HIBBARD, GAP_SEDGEWICK, GAP_CIURA};
    
    for (int i = 0; i < 4; i++) {
        generate_gap_sequence(&seq, types[i], n);
        print_gap_sequence(&seq);
    }
    
    /* Rulăm benchmark-urile */
    BenchmarkResult results[4];
    
    for (int i = 0; i < 4; i++) {
        printf("\nBenchmark %d/4...", i + 1);
        fflush(stdout);
        results[i] = run_benchmark(types[i], n);
        printf(" done\n");
    }
    
    /* Afișăm rezultatele */
    print_benchmark_results(results, 4, n);
    
    /* Afișăm graficul ASCII */
    print_ascii_graph(results, 4);
    
    /* Analiza și concluzii */
    printf("\n📝 Analiză:\n");
    printf("───────────\n");
    printf("• Secvența Shell originală are complexitate O(n²) în worst-case\n");
    printf("• Secvența Hibbard îmbunătățește la O(n^1.5)\n");
    printf("• Secvența Sedgewick oferă O(n^4/3) în worst-case\n");
    printf("• Secvența Ciura este optimă empiric pentru cazul general\n");
    printf("\n");
    printf("Pentru array-uri aproape sortate, toate secvențele au performanțe\n");
    printf("similare datorită naturii adaptive a Shell Sort.\n");
    printf("\n");
    
    return 0;
}
