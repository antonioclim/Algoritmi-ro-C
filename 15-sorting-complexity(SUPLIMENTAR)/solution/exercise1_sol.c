/**
 * =============================================================================
 * SOLUȚIE EXERCIȚIUL 1: Benchmark Comparativ de Algoritmi de Sortare
 * =============================================================================
 *
 * Această soluție demonstrează:
 *   1. Implementarea corectă a Quick Sort cu pivot aleator
 *   2. Implementarea Merge Sort cu buffer auxiliar
 *   3. Implementarea Heap Sort in-place
 *   4. Măsurarea precisă a timpului și numărului de operații
 *   5. Generarea diferitelor tipuri de date de test
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -O2 -o exercise1_sol exercise1_sol.c -lm
 * Execuție:  ./exercise1_sol
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

/* =============================================================================
 * CONSTANTE ȘI VARIABILE GLOBALE
 * =============================================================================
 */

#define DEFAULT_SIZE 10000
#define NUM_DATA_TYPES 4

/* Contoare globale pentru statistici */
static unsigned long g_comparisons = 0;
static unsigned long g_swaps = 0;

/* =============================================================================
 * STRUCTURI
 * =============================================================================
 */

typedef struct {
    double time_ms;
    unsigned long comparisons;
    unsigned long swaps;
    bool is_sorted;
} SortStats;

typedef struct {
    SortStats random_data;
    SortStats sorted_data;
    SortStats reversed_data;
    SortStats nearly_sorted_data;
} BenchmarkResult;

/* =============================================================================
 * FUNCȚII UTILITARE
 * =============================================================================
 */

/**
 * Resetează contoarele globale
 */
void reset_counters(void) {
    g_comparisons = 0;
    g_swaps = 0;
}

/**
 * Interschimbă două elemente și incrementează contorul
 */
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
    g_swaps++;
}

/**
 * Compară două valori și incrementează contorul
 */
int compare(int a, int b) {
    g_comparisons++;
    return a - b;
}

/**
 * Verifică dacă array-ul este sortat
 */
bool is_sorted(const int *arr, size_t n) {
    for (size_t i = 1; i < n; i++) {
        if (arr[i] < arr[i-1]) {
            return false;
        }
    }
    return true;
}

/**
 * Copiază un array
 */
void copy_array(const int *src, int *dst, size_t n) {
    memcpy(dst, src, n * sizeof(int));
}

/**
 * Afișează primele și ultimele elemente ale array-ului
 */
void print_array_summary(const int *arr, size_t n) {
    printf("  [");
    size_t show = (n < 5) ? n : 5;
    for (size_t i = 0; i < show; i++) {
        printf("%d", arr[i]);
        if (i < show - 1) printf(", ");
    }
    if (n > 10) {
        printf(", ..., ");
        for (size_t i = n - 3; i < n; i++) {
            printf("%d", arr[i]);
            if (i < n - 1) printf(", ");
        }
    }
    printf("]\n");
}

/* =============================================================================
 * GENERATOARE DE DATE
 * =============================================================================
 */

/**
 * Generează array cu date aleatorii
 */
void generate_random(int *arr, size_t n) {
    for (size_t i = 0; i < n; i++) {
        arr[i] = rand() % (n * 10);
    }
}

/**
 * Generează array sortat crescător
 */
void generate_sorted(int *arr, size_t n) {
    for (size_t i = 0; i < n; i++) {
        arr[i] = (int)i;
    }
}

/**
 * Generează array sortat descrescător
 */
void generate_reversed(int *arr, size_t n) {
    for (size_t i = 0; i < n; i++) {
        arr[i] = (int)(n - i);
    }
}

/**
 * Generează array aproape sortat (90% în ordine)
 */
void generate_nearly_sorted(int *arr, size_t n) {
    /* Pornește de la array sortat */
    generate_sorted(arr, n);
    
    /* Perturbă 10% din elemente */
    size_t swaps_count = n / 10;
    for (size_t i = 0; i < swaps_count; i++) {
        size_t idx1 = rand() % n;
        size_t idx2 = rand() % n;
        int temp = arr[idx1];
        arr[idx1] = arr[idx2];
        arr[idx2] = temp;
    }
}

/* =============================================================================
 * QUICK SORT CU PIVOT ALEATOR
 * =============================================================================
 */

/**
 * Partiționare Lomuto cu pivot aleator
 */
size_t partition_random(int *arr, size_t low, size_t high) {
    /* Alegem un pivot aleator și îl mutăm la final */
    size_t pivot_idx = low + rand() % (high - low + 1);
    swap(&arr[pivot_idx], &arr[high]);
    
    int pivot = arr[high];
    size_t i = low;
    
    for (size_t j = low; j < high; j++) {
        if (compare(arr[j], pivot) <= 0) {
            swap(&arr[i], &arr[j]);
            i++;
        }
    }
    
    swap(&arr[i], &arr[high]);
    return i;
}

/**
 * Quick Sort recursiv
 */
void quick_sort_recursive(int *arr, size_t low, size_t high) {
    if (low < high) {
        size_t pivot_idx = partition_random(arr, low, high);
        
        if (pivot_idx > 0) {
            quick_sort_recursive(arr, low, pivot_idx - 1);
        }
        quick_sort_recursive(arr, pivot_idx + 1, high);
    }
}

/**
 * Quick Sort - interfață publică
 */
void quick_sort(int *arr, size_t n) {
    if (n > 1) {
        quick_sort_recursive(arr, 0, n - 1);
    }
}

/* =============================================================================
 * MERGE SORT
 * =============================================================================
 */

/**
 * Interclasează două sub-array-uri sortate
 */
void merge(int *arr, int *temp, size_t left, size_t mid, size_t right) {
    size_t i = left;      /* Index pentru jumătatea stângă */
    size_t j = mid + 1;   /* Index pentru jumătatea dreaptă */
    size_t k = left;      /* Index pentru array-ul temporar */
    
    /* Interclasează cele două jumătăți */
    while (i <= mid && j <= right) {
        if (compare(arr[i], arr[j]) <= 0) {
            temp[k++] = arr[i++];
        } else {
            temp[k++] = arr[j++];
        }
    }
    
    /* Copiază elementele rămase din jumătatea stângă */
    while (i <= mid) {
        temp[k++] = arr[i++];
    }
    
    /* Copiază elementele rămase din jumătatea dreaptă */
    while (j <= right) {
        temp[k++] = arr[j++];
    }
    
    /* Copiază înapoi în array-ul original */
    for (i = left; i <= right; i++) {
        arr[i] = temp[i];
    }
}

/**
 * Merge Sort recursiv
 */
void merge_sort_recursive(int *arr, int *temp, size_t left, size_t right) {
    if (left < right) {
        size_t mid = left + (right - left) / 2;
        
        merge_sort_recursive(arr, temp, left, mid);
        merge_sort_recursive(arr, temp, mid + 1, right);
        merge(arr, temp, left, mid, right);
    }
}

/**
 * Merge Sort - interfață publică
 */
void merge_sort(int *arr, size_t n) {
    if (n <= 1) return;
    
    int *temp = (int *)malloc(n * sizeof(int));
    if (temp == NULL) {
        fprintf(stderr, "Eroare: Nu s-a putut aloca memorie pentru buffer!\n");
        return;
    }
    
    merge_sort_recursive(arr, temp, 0, n - 1);
    
    free(temp);
}

/* =============================================================================
 * HEAP SORT
 * =============================================================================
 */

/**
 * Restaurează proprietatea max-heap pentru nodul i
 */
void heapify(int *arr, size_t n, size_t i) {
    size_t largest = i;
    size_t left = 2 * i + 1;
    size_t right = 2 * i + 2;
    
    if (left < n && compare(arr[left], arr[largest]) > 0) {
        largest = left;
    }
    
    if (right < n && compare(arr[right], arr[largest]) > 0) {
        largest = right;
    }
    
    if (largest != i) {
        swap(&arr[i], &arr[largest]);
        heapify(arr, n, largest);
    }
}

/**
 * Heap Sort - interfață publică
 */
void heap_sort(int *arr, size_t n) {
    if (n <= 1) return;
    
    /* Construiește max-heap (bottom-up) */
    for (size_t i = n / 2; i > 0; i--) {
        heapify(arr, n, i - 1);
    }
    
    /* Extrage elementele unul câte unul */
    for (size_t i = n - 1; i > 0; i--) {
        swap(&arr[0], &arr[i]);
        heapify(arr, i, 0);
    }
}

/* =============================================================================
 * BENCHMARK
 * =============================================================================
 */

/**
 * Rulează benchmark pentru un algoritm pe un tip de date
 */
SortStats run_single_benchmark(void (*sort_func)(int*, size_t), 
                                const int *original, 
                                size_t n) {
    SortStats stats = {0};
    
    /* Alocă și copiază array-ul */
    int *arr = (int *)malloc(n * sizeof(int));
    if (arr == NULL) {
        fprintf(stderr, "Eroare: Nu s-a putut aloca memorie!\n");
        return stats;
    }
    copy_array(original, arr, n);
    
    /* Resetează contoarele */
    reset_counters();
    
    /* Măsoară timpul */
    clock_t start = clock();
    sort_func(arr, n);
    clock_t end = clock();
    
    /* Calculează statisticile */
    stats.time_ms = ((double)(end - start) / CLOCKS_PER_SEC) * 1000.0;
    stats.comparisons = g_comparisons;
    stats.swaps = g_swaps;
    stats.is_sorted = is_sorted(arr, n);
    
    free(arr);
    return stats;
}

/**
 * Afișează rezultatele benchmark-ului
 */
void print_benchmark_results(const char *algo_name, const BenchmarkResult *result) {
    printf("\n┌──────────────────────────────────────────────────────────────────┐\n");
    printf("│  %-64s│\n", algo_name);
    printf("├──────────────┬────────────┬──────────────┬──────────────┬────────┤\n");
    printf("│ Tip date     │ Timp (ms)  │ Comparații   │ Swap-uri     │ Valid  │\n");
    printf("├──────────────┼────────────┼──────────────┼──────────────┼────────┤\n");
    
    printf("│ Random       │ %10.2f │ %12lu │ %12lu │   %s   │\n",
           result->random_data.time_ms,
           result->random_data.comparisons,
           result->random_data.swaps,
           result->random_data.is_sorted ? "✓" : "✗");
    
    printf("│ Sorted       │ %10.2f │ %12lu │ %12lu │   %s   │\n",
           result->sorted_data.time_ms,
           result->sorted_data.comparisons,
           result->sorted_data.swaps,
           result->sorted_data.is_sorted ? "✓" : "✗");
    
    printf("│ Reversed     │ %10.2f │ %12lu │ %12lu │   %s   │\n",
           result->reversed_data.time_ms,
           result->reversed_data.comparisons,
           result->reversed_data.swaps,
           result->reversed_data.is_sorted ? "✓" : "✗");
    
    printf("│ Nearly Sort  │ %10.2f │ %12lu │ %12lu │   %s   │\n",
           result->nearly_sorted_data.time_ms,
           result->nearly_sorted_data.comparisons,
           result->nearly_sorted_data.swaps,
           result->nearly_sorted_data.is_sorted ? "✓" : "✗");
    
    printf("└──────────────┴────────────┴──────────────┴──────────────┴────────┘\n");
}

/**
 * Rulează benchmark complet pentru un algoritm
 */
BenchmarkResult run_full_benchmark(void (*sort_func)(int*, size_t),
                                   int *random_data,
                                   int *sorted_data,
                                   int *reversed_data,
                                   int *nearly_sorted_data,
                                   size_t n) {
    BenchmarkResult result;
    
    result.random_data = run_single_benchmark(sort_func, random_data, n);
    result.sorted_data = run_single_benchmark(sort_func, sorted_data, n);
    result.reversed_data = run_single_benchmark(sort_func, reversed_data, n);
    result.nearly_sorted_data = run_single_benchmark(sort_func, nearly_sorted_data, n);
    
    return result;
}

/* =============================================================================
 * MAIN
 * =============================================================================
 */

void print_separator(const char *title) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║  %-75s║\n", title);
    printf("╚═══════════════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

int main(int argc, char *argv[]) {
    /* Parsare argumente */
    size_t n = DEFAULT_SIZE;
    if (argc > 1) {
        n = (size_t)atoi(argv[1]);
        if (n < 10) n = 10;
        if (n > 1000000) n = 1000000;
    }
    
    /* Inițializare generator de numere aleatorii */
    srand((unsigned int)time(NULL));
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║     SĂPTĂMÂNA 15: ALGORITMI DE SORTARE AVANSAȚI                                ║\n");
    printf("║     Exercițiul 1: Benchmark Comparativ - SOLUȚIE                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════════════════╝\n");
    
    printf("\nConfigurație benchmark:\n");
    printf("  - Dimensiune array: %zu elemente\n", n);
    printf("  - Algoritmi testați: Quick Sort, Merge Sort, Heap Sort\n");
    printf("  - Tipuri date: random, sorted, reversed, nearly sorted\n");
    
    /* Alocă array-urile de test */
    int *random_data = (int *)malloc(n * sizeof(int));
    int *sorted_data = (int *)malloc(n * sizeof(int));
    int *reversed_data = (int *)malloc(n * sizeof(int));
    int *nearly_sorted_data = (int *)malloc(n * sizeof(int));
    
    if (!random_data || !sorted_data || !reversed_data || !nearly_sorted_data) {
        fprintf(stderr, "Eroare: Nu s-a putut aloca memorie pentru date!\n");
        return 1;
    }
    
    /* Generează datele de test */
    print_separator("GENERARE DATE DE TEST");
    
    printf("Generare date random...\n");
    generate_random(random_data, n);
    
    printf("Generare date sortate...\n");
    generate_sorted(sorted_data, n);
    
    printf("Generare date inversate...\n");
    generate_reversed(reversed_data, n);
    
    printf("Generare date aproape sortate...\n");
    generate_nearly_sorted(nearly_sorted_data, n);
    
    printf("✓ Date generate cu succes!\n");
    
    /* Rulează benchmark-urile */
    print_separator("BENCHMARK QUICK SORT (Pivot Aleator)");
    BenchmarkResult quick_result = run_full_benchmark(
        quick_sort, random_data, sorted_data, reversed_data, nearly_sorted_data, n);
    print_benchmark_results("QUICK SORT (Randomized Pivot)", &quick_result);
    
    print_separator("BENCHMARK MERGE SORT");
    BenchmarkResult merge_result = run_full_benchmark(
        merge_sort, random_data, sorted_data, reversed_data, nearly_sorted_data, n);
    print_benchmark_results("MERGE SORT (Top-Down)", &merge_result);
    
    print_separator("BENCHMARK HEAP SORT");
    BenchmarkResult heap_result = run_full_benchmark(
        heap_sort, random_data, sorted_data, reversed_data, nearly_sorted_data, n);
    print_benchmark_results("HEAP SORT (In-Place)", &heap_result);
    
    /* Afișează rezumatul comparativ */
    print_separator("REZUMAT COMPARATIV - DATE RANDOM");
    
    printf("┌─────────────────┬────────────┬──────────────┬──────────────┐\n");
    printf("│ Algoritm        │ Timp (ms)  │ Comparații   │ Swap-uri     │\n");
    printf("├─────────────────┼────────────┼──────────────┼──────────────┤\n");
    printf("│ Quick Sort      │ %10.2f │ %12lu │ %12lu │\n",
           quick_result.random_data.time_ms,
           quick_result.random_data.comparisons,
           quick_result.random_data.swaps);
    printf("│ Merge Sort      │ %10.2f │ %12lu │ %12s │\n",
           merge_result.random_data.time_ms,
           merge_result.random_data.comparisons,
           "N/A");
    printf("│ Heap Sort       │ %10.2f │ %12lu │ %12lu │\n",
           heap_result.random_data.time_ms,
           heap_result.random_data.comparisons,
           heap_result.random_data.swaps);
    printf("└─────────────────┴────────────┴──────────────┴──────────────┘\n");
    
    /* Curățare memorie */
    free(random_data);
    free(sorted_data);
    free(reversed_data);
    free(nearly_sorted_data);
    
    print_separator("BENCHMARK COMPLET CU SUCCES!");
    
    return 0;
}
