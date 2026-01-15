/**
 * =============================================================================
 * SOLUȚIE TEMA 1: Sistem de Sortare Multi-Algoritm Adaptiv
 * Săptămâna 15: Algoritmi de Sortare Avansați
 * =============================================================================
 *
 * Acest program implementează un sistem inteligent de sortare care:
 *   1. Analizează caracteristicile datelor de intrare
 *   2. Selectează automat algoritmul optim
 *   3. Compară performanța cu alegerea naivă (Quick Sort)
 *
 * Algoritmi disponibili:
 *   - Insertion Sort (array mic sau aproape sortat)
 *   - Counting Sort (interval mic de valori)
 *   - Quick Sort cu median-of-three (cazul general)
 *   - Merge Sort (fallback stabil)
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -O2 -o homework1 homework1_sol.c
 * Execuție:  ./homework1 test.txt
 *            ./homework1 --test
 *            ./homework1 --generate 10000
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <limits.h>

/* =============================================================================
 * PARTEA 1: CONSTANTE ȘI TIPURI DE DATE
 * =============================================================================
 */

#define INSERTION_THRESHOLD 20      /* Folosim Insertion Sort sub acest prag */
#define SORTED_RATIO_THRESHOLD 0.8  /* Considerat "aproape sortat" */
#define RANGE_MULTIPLIER 2          /* Counting Sort dacă range < n * factor */

/**
 * Enum pentru algoritmii disponibili
 */
typedef enum {
    ALG_INSERTION_SORT,
    ALG_COUNTING_SORT,
    ALG_QUICK_SORT,
    ALG_MERGE_SORT
} SortAlgorithm;

/**
 * Structură pentru caracteristicile datelor
 */
typedef struct {
    int n;                  /* Dimensiune array */
    int min_val;            /* Valoare minimă */
    int max_val;            /* Valoare maximă */
    double sorted_ratio;    /* Proporția elementelor în ordine */
    double duplicate_ratio; /* Proporția valorilor duplicate */
    bool mostly_sorted;     /* Array aproape sortat? */
    bool small_range;       /* Interval mic de valori? */
} DataCharacteristics;

/**
 * Structură pentru statistici de sortare
 */
typedef struct {
    SortAlgorithm algorithm_used;
    long long comparisons;
    long long swaps;
    double time_ms;
} SortStats;

/* =============================================================================
 * PARTEA 2: FUNCȚII UTILITARE
 * =============================================================================
 */

/**
 * Interschimbă două elemente
 */
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * Returnează numele algoritmului ca string
 */
const char* algorithm_name(SortAlgorithm alg) {
    switch (alg) {
        case ALG_INSERTION_SORT: return "Insertion Sort";
        case ALG_COUNTING_SORT:  return "Counting Sort";
        case ALG_QUICK_SORT:     return "Quick Sort (median-of-three)";
        case ALG_MERGE_SORT:     return "Merge Sort";
        default:                 return "Unknown";
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
        if (arr[i] < arr[i-1]) return false;
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
 * PARTEA 3: ANALIZĂ CARACTERISTICI DATE
 * =============================================================================
 */

/**
 * Analizează caracteristicile unui array
 *
 * @param arr Array-ul de analizat
 * @param n Dimensiunea array-ului
 * @return Structura cu caracteristicile detectate
 *
 * Complexitate: O(n)
 */
DataCharacteristics analyze_data(const int arr[], int n) {
    DataCharacteristics dc = {0};
    dc.n = n;
    
    if (n == 0) return dc;
    
    /* Găsim min și max */
    dc.min_val = arr[0];
    dc.max_val = arr[0];
    
    int sorted_count = 0;  /* Câte perechi consecutive sunt în ordine */
    
    for (int i = 0; i < n; i++) {
        if (arr[i] < dc.min_val) dc.min_val = arr[i];
        if (arr[i] > dc.max_val) dc.max_val = arr[i];
        
        if (i > 0 && arr[i] >= arr[i-1]) {
            sorted_count++;
        }
    }
    
    /* Calculăm sorted_ratio */
    dc.sorted_ratio = (n > 1) ? (double)sorted_count / (n - 1) : 1.0;
    dc.mostly_sorted = (dc.sorted_ratio >= SORTED_RATIO_THRESHOLD);
    
    /* Verificăm dacă intervalul e mic */
    int range = dc.max_val - dc.min_val + 1;
    dc.small_range = (range <= n * RANGE_MULTIPLIER);
    
    /* Estimăm duplicate_ratio folosind un sampling pentru array-uri mari */
    if (n <= 10000) {
        /* Pentru array-uri mici, numărăm exact */
        int *count = (int*)calloc(range, sizeof(int));
        if (count != NULL) {
            for (int i = 0; i < n; i++) {
                count[arr[i] - dc.min_val]++;
            }
            
            int unique = 0;
            for (int i = 0; i < range; i++) {
                if (count[i] > 0) unique++;
            }
            
            dc.duplicate_ratio = 1.0 - (double)unique / n;
            free(count);
        }
    } else {
        /* Pentru array-uri mari, estimăm prin sampling */
        dc.duplicate_ratio = (range < n) ? (1.0 - (double)range / n) : 0.0;
    }
    
    return dc;
}

/**
 * Afișează caracteristicile datelor
 */
void print_characteristics(const DataCharacteristics *dc) {
    printf("Analiză date:\n");
    printf("  Dimensiune: %d\n", dc->n);
    printf("  Interval: [%d, %d] (range = %d)\n", 
           dc->min_val, dc->max_val, dc->max_val - dc->min_val + 1);
    printf("  Sorted ratio: %.2f%%\n", dc->sorted_ratio * 100);
    printf("  Duplicate ratio: %.2f%%\n", dc->duplicate_ratio * 100);
    printf("  Aproape sortat: %s\n", dc->mostly_sorted ? "DA" : "NU");
    printf("  Interval mic: %s\n", dc->small_range ? "DA" : "NU");
    printf("\n");
}

/* =============================================================================
 * PARTEA 4: SELECȚIE ALGORITM
 * =============================================================================
 */

/**
 * Selectează algoritmul optim bazat pe caracteristicile datelor
 *
 * @param dc Caracteristicile datelor
 * @return Algoritmul recomandat
 */
SortAlgorithm select_algorithm(const DataCharacteristics *dc) {
    /* Regula 1: Array foarte mic -> Insertion Sort */
    if (dc->n < INSERTION_THRESHOLD) {
        return ALG_INSERTION_SORT;
    }
    
    /* Regula 2: Array aproape sortat -> Insertion Sort (O(n) în acest caz) */
    if (dc->mostly_sorted) {
        return ALG_INSERTION_SORT;
    }
    
    /* Regula 3: Interval mic de valori -> Counting Sort (O(n + k)) */
    if (dc->small_range && dc->min_val >= 0) {
        return ALG_COUNTING_SORT;
    }
    
    /* Regula 4: Cazul general -> Quick Sort cu median-of-three */
    return ALG_QUICK_SORT;
}

/**
 * Explică decizia de selecție
 */
void explain_selection(SortAlgorithm alg, const DataCharacteristics *dc) {
    printf("Algoritm selectat: %s\n", algorithm_name(alg));
    printf("Motiv: ");
    
    switch (alg) {
        case ALG_INSERTION_SORT:
            if (dc->n < INSERTION_THRESHOLD) {
                printf("Array mic (n=%d < %d)\n", dc->n, INSERTION_THRESHOLD);
            } else {
                printf("Array aproape sortat (%.1f%% în ordine)\n", 
                       dc->sorted_ratio * 100);
            }
            break;
            
        case ALG_COUNTING_SORT:
            printf("Interval mic de valori (range=%d, n=%d)\n",
                   dc->max_val - dc->min_val + 1, dc->n);
            break;
            
        case ALG_QUICK_SORT:
            printf("Caz general - performanță O(n log n) medie\n");
            break;
            
        case ALG_MERGE_SORT:
            printf("Fallback stabil\n");
            break;
    }
    printf("\n");
}

/* =============================================================================
 * PARTEA 5: IMPLEMENTĂRI ALGORITMI DE SORTARE
 * =============================================================================
 */

/* --- INSERTION SORT --- */

/**
 * Insertion Sort - optim pentru array-uri mici sau aproape sortate
 *
 * Complexitate: O(n²) worst, O(n) best (array sortat)
 */
void insertion_sort(int arr[], int n, SortStats *stats) {
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;
        
        while (j >= 0 && arr[j] > key) {
            stats->comparisons++;
            arr[j + 1] = arr[j];
            stats->swaps++;
            j--;
        }
        if (j >= 0) stats->comparisons++;
        
        arr[j + 1] = key;
    }
}

/* --- COUNTING SORT --- */

/**
 * Counting Sort - optim pentru interval mic de valori
 *
 * Complexitate: O(n + k) unde k este intervalul de valori
 */
void counting_sort(int arr[], int n, int min_val, int max_val, SortStats *stats) {
    int range = max_val - min_val + 1;
    
    int *count = (int*)calloc(range, sizeof(int));
    int *output = (int*)malloc(n * sizeof(int));
    
    if (count == NULL || output == NULL) {
        fprintf(stderr, "Eroare: Nu s-a putut aloca memorie!\n");
        free(count);
        free(output);
        return;
    }
    
    /* Numărăm aparițiile */
    for (int i = 0; i < n; i++) {
        count[arr[i] - min_val]++;
    }
    
    /* Calculăm pozițiile cumulative */
    for (int i = 1; i < range; i++) {
        count[i] += count[i - 1];
    }
    
    /* Construim output-ul (de la sfârșit pentru stabilitate) */
    for (int i = n - 1; i >= 0; i--) {
        int idx = arr[i] - min_val;
        output[count[idx] - 1] = arr[i];
        count[idx]--;
        stats->swaps++;
    }
    
    /* Copiem înapoi în array-ul original */
    for (int i = 0; i < n; i++) {
        arr[i] = output[i];
    }
    
    stats->comparisons = 0;  /* Counting Sort nu face comparații */
    
    free(count);
    free(output);
}

/* --- QUICK SORT CU MEDIAN-OF-THREE --- */

/**
 * Găsește mediana dintre trei elemente și o pune pe poziția pivot
 */
int median_of_three(int arr[], int low, int high, SortStats *stats) {
    int mid = low + (high - low) / 2;
    
    /* Sortăm low, mid, high */
    if (arr[low] > arr[mid]) {
        swap(&arr[low], &arr[mid]);
        stats->swaps++;
    }
    stats->comparisons++;
    
    if (arr[low] > arr[high]) {
        swap(&arr[low], &arr[high]);
        stats->swaps++;
    }
    stats->comparisons++;
    
    if (arr[mid] > arr[high]) {
        swap(&arr[mid], &arr[high]);
        stats->swaps++;
    }
    stats->comparisons++;
    
    /* Punem mediana pe high-1 pentru partiționare */
    swap(&arr[mid], &arr[high - 1]);
    stats->swaps++;
    
    return arr[high - 1];
}

/**
 * Partiționare Lomuto modificată pentru median-of-three
 */
int partition_mot(int arr[], int low, int high, SortStats *stats) {
    int pivot = median_of_three(arr, low, high, stats);
    
    int i = low;
    int j = high - 1;
    
    /* Partiționare Hoare modificată */
    while (true) {
        while (arr[++i] < pivot) stats->comparisons++;
        stats->comparisons++;
        
        while (arr[--j] > pivot) stats->comparisons++;
        stats->comparisons++;
        
        if (i >= j) break;
        
        swap(&arr[i], &arr[j]);
        stats->swaps++;
    }
    
    /* Punem pivot-ul pe poziția corectă */
    swap(&arr[i], &arr[high - 1]);
    stats->swaps++;
    
    return i;
}

/**
 * Quick Sort recursiv cu median-of-three și cutoff la Insertion Sort
 */
void quick_sort_recursive(int arr[], int low, int high, SortStats *stats) {
    if (high - low < 10) {
        /* Cutoff la Insertion Sort pentru subarray-uri mici */
        for (int i = low + 1; i <= high; i++) {
            int key = arr[i];
            int j = i - 1;
            while (j >= low && arr[j] > key) {
                stats->comparisons++;
                arr[j + 1] = arr[j];
                stats->swaps++;
                j--;
            }
            if (j >= low) stats->comparisons++;
            arr[j + 1] = key;
        }
        return;
    }
    
    int pi = partition_mot(arr, low, high, stats);
    
    quick_sort_recursive(arr, low, pi - 1, stats);
    quick_sort_recursive(arr, pi + 1, high, stats);
}

/**
 * Quick Sort cu median-of-three (wrapper)
 */
void quick_sort_mot(int arr[], int n, SortStats *stats) {
    if (n <= 1) return;
    quick_sort_recursive(arr, 0, n - 1, stats);
}

/* --- MERGE SORT --- */

/**
 * Merge pentru Merge Sort
 */
void merge(int arr[], int left, int mid, int right, SortStats *stats) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    
    int *L = (int*)malloc(n1 * sizeof(int));
    int *R = (int*)malloc(n2 * sizeof(int));
    
    if (L == NULL || R == NULL) {
        free(L);
        free(R);
        return;
    }
    
    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];
    
    int i = 0, j = 0, k = left;
    
    while (i < n1 && j < n2) {
        stats->comparisons++;
        if (L[i] <= R[j]) {
            arr[k++] = L[i++];
        } else {
            arr[k++] = R[j++];
        }
        stats->swaps++;
    }
    
    while (i < n1) {
        arr[k++] = L[i++];
        stats->swaps++;
    }
    
    while (j < n2) {
        arr[k++] = R[j++];
        stats->swaps++;
    }
    
    free(L);
    free(R);
}

/**
 * Merge Sort recursiv
 */
void merge_sort_recursive(int arr[], int left, int right, SortStats *stats) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        
        merge_sort_recursive(arr, left, mid, stats);
        merge_sort_recursive(arr, mid + 1, right, stats);
        merge(arr, left, mid, right, stats);
    }
}

/**
 * Merge Sort (wrapper)
 */
void merge_sort(int arr[], int n, SortStats *stats) {
    if (n <= 1) return;
    merge_sort_recursive(arr, 0, n - 1, stats);
}

/* =============================================================================
 * PARTEA 6: SORTARE ADAPTIVĂ
 * =============================================================================
 */

/**
 * Sortare adaptivă - selectează și execută algoritmul optim
 *
 * @param arr Array-ul de sortat
 * @param n Dimensiunea
 * @param dc Caracteristicile (opțional, NULL pentru auto-detectare)
 * @param stats Statistici de ieșire
 */
void adaptive_sort(int arr[], int n, DataCharacteristics *dc, SortStats *stats) {
    DataCharacteristics local_dc;
    
    /* Analizăm datele dacă nu avem caracteristici */
    if (dc == NULL) {
        local_dc = analyze_data(arr, n);
        dc = &local_dc;
    }
    
    /* Selectăm algoritmul */
    SortAlgorithm alg = select_algorithm(dc);
    stats->algorithm_used = alg;
    stats->comparisons = 0;
    stats->swaps = 0;
    
    /* Executăm sortarea */
    double start = get_time_ms();
    
    switch (alg) {
        case ALG_INSERTION_SORT:
            insertion_sort(arr, n, stats);
            break;
            
        case ALG_COUNTING_SORT:
            counting_sort(arr, n, dc->min_val, dc->max_val, stats);
            break;
            
        case ALG_QUICK_SORT:
            quick_sort_mot(arr, n, stats);
            break;
            
        case ALG_MERGE_SORT:
            merge_sort(arr, n, stats);
            break;
    }
    
    stats->time_ms = get_time_ms() - start;
}

/* =============================================================================
 * PARTEA 7: COMPARAȚIE CU ALEGEREA NAIVĂ
 * =============================================================================
 */

/**
 * Compară sortarea adaptivă cu Quick Sort naiv
 */
void compare_with_naive(const int original[], int n, const DataCharacteristics *dc) {
    int *arr_adaptive = (int*)malloc(n * sizeof(int));
    int *arr_naive = (int*)malloc(n * sizeof(int));
    
    if (arr_adaptive == NULL || arr_naive == NULL) {
        free(arr_adaptive);
        free(arr_naive);
        return;
    }
    
    /* Copiem array-ul original */
    copy_array(arr_adaptive, original, n);
    copy_array(arr_naive, original, n);
    
    SortStats stats_adaptive = {0};
    SortStats stats_naive = {0};
    
    /* Sortare adaptivă */
    DataCharacteristics dc_copy = *dc;
    adaptive_sort(arr_adaptive, n, &dc_copy, &stats_adaptive);
    
    /* Sortare naivă (Quick Sort mereu) */
    double start = get_time_ms();
    quick_sort_mot(arr_naive, n, &stats_naive);
    stats_naive.time_ms = get_time_ms() - start;
    stats_naive.algorithm_used = ALG_QUICK_SORT;
    
    /* Afișăm comparația */
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("                    COMPARAȚIE REZULTATE\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    printf("┌────────────────────┬──────────────────┬──────────────────┐\n");
    printf("│ Metrică            │ Adaptiv          │ Naiv (QuickSort) │\n");
    printf("├────────────────────┼──────────────────┼──────────────────┤\n");
    printf("│ Algoritm           │ %-16s │ %-16s │\n",
           algorithm_name(stats_adaptive.algorithm_used),
           algorithm_name(stats_naive.algorithm_used));
    printf("│ Timp (ms)          │ %14.3f   │ %14.3f   │\n",
           stats_adaptive.time_ms, stats_naive.time_ms);
    printf("│ Comparații         │ %14lld   │ %14lld   │\n",
           stats_adaptive.comparisons, stats_naive.comparisons);
    printf("│ Mutări/Swaps       │ %14lld   │ %14lld   │\n",
           stats_adaptive.swaps, stats_naive.swaps);
    printf("└────────────────────┴──────────────────┴──────────────────┘\n\n");
    
    if (stats_naive.time_ms > 0) {
        double speedup = stats_naive.time_ms / stats_adaptive.time_ms;
        if (speedup >= 1.0) {
            printf("Speedup adaptiv: %.2fx mai rapid\n", speedup);
        } else {
            printf("Speedup adaptiv: %.2fx mai lent (overhead analiză)\n", 1.0/speedup);
        }
    }
    
    /* Verificare corectitudine */
    printf("\nVerificare sortare: ");
    if (is_sorted(arr_adaptive, n) && is_sorted(arr_naive, n)) {
        printf("✓ Ambele rezultate sunt corecte\n");
    } else {
        printf("✗ EROARE - rezultate incorecte!\n");
    }
    
    free(arr_adaptive);
    free(arr_naive);
}

/* =============================================================================
 * PARTEA 8: FUNCȚII I/O
 * =============================================================================
 */

/**
 * Citește un array din fișier
 */
int* read_array_from_file(const char *filename, int *n) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Eroare: Nu s-a putut deschide '%s'\n", filename);
        return NULL;
    }
    
    /* Prima linie poate conține dimensiunea sau nu */
    /* Numărăm mai întâi elementele */
    int capacity = 1000;
    int count = 0;
    int *arr = (int*)malloc(capacity * sizeof(int));
    
    if (arr == NULL) {
        fclose(file);
        return NULL;
    }
    
    int value;
    while (fscanf(file, "%d", &value) == 1) {
        if (count >= capacity) {
            capacity *= 2;
            int *new_arr = (int*)realloc(arr, capacity * sizeof(int));
            if (new_arr == NULL) {
                free(arr);
                fclose(file);
                return NULL;
            }
            arr = new_arr;
        }
        arr[count++] = value;
    }
    
    fclose(file);
    *n = count;
    return arr;
}

/**
 * Generează un fișier de test
 */
void generate_test_file(const char *filename, int n, const char *type) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Eroare: Nu s-a putut crea '%s'\n", filename);
        return;
    }
    
    srand((unsigned int)time(NULL));
    
    for (int i = 0; i < n; i++) {
        int value;
        
        if (strcmp(type, "random") == 0) {
            value = rand() % 1000000;
        } else if (strcmp(type, "sorted") == 0) {
            value = i;
        } else if (strcmp(type, "reversed") == 0) {
            value = n - i;
        } else if (strcmp(type, "nearly_sorted") == 0) {
            value = i;
            if (rand() % 10 == 0) {
                value = rand() % n;  /* 10% dezordine */
            }
        } else if (strcmp(type, "small_range") == 0) {
            value = rand() % 100;  /* Doar valori 0-99 */
        } else {
            value = rand() % 1000000;
        }
        
        fprintf(file, "%d\n", value);
    }
    
    fclose(file);
    printf("Generat '%s' cu %d elemente (tip: %s)\n", filename, n, type);
}

/**
 * Rulează teste automate
 */
void run_tests(void) {
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("                    TESTE AUTOMATE\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    int test_sizes[] = {15, 100, 1000, 10000};
    const char *test_types[] = {"random", "sorted", "reversed", "nearly_sorted", "small_range"};
    int num_sizes = sizeof(test_sizes) / sizeof(test_sizes[0]);
    int num_types = sizeof(test_types) / sizeof(test_types[0]);
    
    for (int s = 0; s < num_sizes; s++) {
        int n = test_sizes[s];
        printf("──────────────────────────────────────────\n");
        printf("Dimensiune: %d\n", n);
        printf("──────────────────────────────────────────\n");
        
        for (int t = 0; t < num_types; t++) {
            /* Generăm date de test */
            int *arr = (int*)malloc(n * sizeof(int));
            if (arr == NULL) continue;
            
            srand(42);  /* Seed fix pentru reproducibilitate */
            
            for (int i = 0; i < n; i++) {
                if (strcmp(test_types[t], "random") == 0) {
                    arr[i] = rand() % 1000000;
                } else if (strcmp(test_types[t], "sorted") == 0) {
                    arr[i] = i;
                } else if (strcmp(test_types[t], "reversed") == 0) {
                    arr[i] = n - i;
                } else if (strcmp(test_types[t], "nearly_sorted") == 0) {
                    arr[i] = i;
                    if (rand() % 10 == 0) arr[i] = rand() % n;
                } else if (strcmp(test_types[t], "small_range") == 0) {
                    arr[i] = rand() % 100;
                }
            }
            
            /* Analizăm și sortăm */
            DataCharacteristics dc = analyze_data(arr, n);
            SortAlgorithm alg = select_algorithm(&dc);
            
            SortStats stats = {0};
            adaptive_sort(arr, n, &dc, &stats);
            
            /* Verificăm */
            bool correct = is_sorted(arr, n);
            
            printf("  %-15s -> %-20s | %.3f ms | %s\n",
                   test_types[t],
                   algorithm_name(alg),
                   stats.time_ms,
                   correct ? "✓" : "✗");
            
            free(arr);
        }
        printf("\n");
    }
    
    printf("Teste completate!\n");
}

/* =============================================================================
 * PARTEA 9: MAIN
 * =============================================================================
 */

void print_usage(const char *prog) {
    printf("Utilizare:\n");
    printf("  %s <fisier.txt>              - Sortează datele din fișier\n", prog);
    printf("  %s --test                    - Rulează teste automate\n", prog);
    printf("  %s --generate <n> [tip]      - Generează fișier de test\n", prog);
    printf("                                (tip: random, sorted, reversed, nearly_sorted, small_range)\n");
}

int main(int argc, char *argv[]) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║     TEMA 1: SISTEM DE SORTARE MULTI-ALGORITM ADAPTIV                          ║\n");
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
    
    if (strcmp(argv[1], "--generate") == 0) {
        if (argc < 3) {
            printf("Eroare: Specificați dimensiunea!\n");
            return 1;
        }
        int n = atoi(argv[2]);
        const char *type = (argc >= 4) ? argv[3] : "random";
        char filename[64];
        sprintf(filename, "test_%d_%s.txt", n, type);
        generate_test_file(filename, n, type);
        return 0;
    }
    
    /* Citim fișierul */
    int n;
    int *arr = read_array_from_file(argv[1], &n);
    
    if (arr == NULL || n == 0) {
        printf("Eroare: Nu s-au putut citi datele din '%s'\n", argv[1]);
        return 1;
    }
    
    printf("Citite %d elemente din '%s'\n\n", n, argv[1]);
    
    /* Analizăm datele */
    DataCharacteristics dc = analyze_data(arr, n);
    print_characteristics(&dc);
    
    /* Selectăm și explicăm algoritmul */
    SortAlgorithm alg = select_algorithm(&dc);
    explain_selection(alg, &dc);
    
    /* Comparăm cu alegerea naivă */
    compare_with_naive(arr, n, &dc);
    
    free(arr);
    
    printf("\n");
    return 0;
}
