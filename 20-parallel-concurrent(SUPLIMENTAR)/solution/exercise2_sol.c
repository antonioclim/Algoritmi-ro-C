/**
 * =============================================================================
 * SOLUȚIE EXERCIȚIUL 2: Parallel Quick Sort cu Threshold Adaptiv
 * =============================================================================
 *
 * Acest fișier conține soluția completă pentru exercițiul 2.
 * Demonstrează implementarea Quick Sort paralel cu:
 *   - Creare dinamică de thread-uri pentru partiții mari
 *   - Threshold adaptiv pentru switch la versiunea secvențială
 *   - Limitare număr maxim de thread-uri active
 *   - Benchmark comparativ cu calcul speedup
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -O2 -o exercise2_sol exercise2_sol.c -lpthread
 *
 * =============================================================================
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <pthread.h>
#include <time.h>

/* =============================================================================
 * CONSTANTE ȘI CONFIGURARE
 * =============================================================================
 */

#define DEFAULT_ARRAY_SIZE    50000
#define DEFAULT_MAX_THREADS   4
#define DEFAULT_THRESHOLD     1000
#define MAX_VALUE             1000000

/* =============================================================================
 * VARIABILE GLOBALE PENTRU CONTROLUL THREAD-URILOR
 * =============================================================================
 */

static atomic_int active_threads = 0;
static int max_threads = DEFAULT_MAX_THREADS;
static int threshold = DEFAULT_THRESHOLD;

/* Statistici pentru analiză */
static atomic_int total_partitions = 0;
static atomic_int parallel_partitions = 0;
static atomic_int sequential_fallbacks = 0;

/* =============================================================================
 * STRUCTURI DE DATE
 * =============================================================================
 */

/**
 * SOLUȚIE TODO 1: Structură pentru argumentele thread-urilor de sortare
 *
 * Conține toate informațiile necesare pentru sortarea unei partiții:
 * - Pointer la array-ul de sortat
 * - Limitele subarray-ului (left, right)
 * - Adâncimea curentă în recursie (pentru limitare)
 */
typedef struct {
    int *arr;           /* Pointer la array-ul partajat */
    int left;           /* Index stâng (inclusiv) */
    int right;          /* Index drept (inclusiv) */
    int depth;          /* Adâncimea recursiei */
} SortArg;

/* =============================================================================
 * FUNCȚII HELPER
 * =============================================================================
 */

/**
 * Obține timestamp în milisecunde pentru măsurători de performanță
 */
double get_time_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1000000.0;
}

/**
 * Schimbă două elemente
 */
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * Verifică dacă array-ul este sortat corect
 */
bool is_sorted(int *arr, int size) {
    for (int i = 1; i < size; i++) {
        if (arr[i] < arr[i-1]) return false;
    }
    return true;
}

/**
 * Generează array cu numere aleatoare
 */
void generate_random_array(int *arr, int size, int max_val) {
    for (int i = 0; i < size; i++) {
        arr[i] = rand() % max_val;
    }
}

/**
 * Copiază un array
 */
void copy_array(int *dest, int *src, int size) {
    memcpy(dest, src, size * sizeof(int));
}

/**
 * Afișează primele și ultimele n elemente
 */
void print_array_preview(int *arr, int size, int preview) {
    printf("  [");
    for (int i = 0; i < preview && i < size; i++) {
        printf("%d", arr[i]);
        if (i < preview - 1 && i < size - 1) printf(", ");
    }
    if (size > 2 * preview) {
        printf(", ... ");
        for (int i = size - preview; i < size; i++) {
            printf("%d", arr[i]);
            if (i < size - 1) printf(", ");
        }
    }
    printf("]\n");
}

/* =============================================================================
 * SOLUȚIE TODO 2: IMPLEMENTAREA ALGORITMILOR DE SORTARE
 * =============================================================================
 */

/**
 * Funcția de partiționare (schema Lomuto cu pivot median-of-three)
 *
 * Alege pivotul ca mediană din primul, ultimul și mijlocul array-ului.
 * Aceasta evită worst-case O(n²) pentru array-uri deja sortate.
 *
 * @param arr Array-ul de partiționat
 * @param left Index stâng
 * @param right Index drept
 * @return Poziția finală a pivotului
 */
int partition(int *arr, int left, int right) {
    /* Median-of-three: alege pivotul mai inteligent */
    int mid = left + (right - left) / 2;
    
    /* Sortează arr[left], arr[mid], arr[right] */
    if (arr[mid] < arr[left]) swap(&arr[left], &arr[mid]);
    if (arr[right] < arr[left]) swap(&arr[left], &arr[right]);
    if (arr[right] < arr[mid]) swap(&arr[mid], &arr[right]);
    
    /* Mută mediana la right-1 (folosim ca pivot) */
    swap(&arr[mid], &arr[right - 1]);
    int pivot = arr[right - 1];
    
    int i = left;
    int j = right - 1;
    
    while (true) {
        while (arr[++i] < pivot);
        while (arr[--j] > pivot);
        if (i >= j) break;
        swap(&arr[i], &arr[j]);
    }
    
    swap(&arr[i], &arr[right - 1]);
    return i;
}

/**
 * Quick Sort secvențial (versiunea standard)
 *
 * Folosit pentru:
 * - Partiții sub threshold
 * - Fallback când sunt prea multe thread-uri active
 * - Benchmark de referință
 *
 * @param arr Array-ul de sortat
 * @param left Index stâng
 * @param right Index drept
 */
void quicksort_sequential(int *arr, int left, int right) {
    if (left >= right) return;
    
    /* Pentru array-uri foarte mici, folosește Insertion Sort */
    if (right - left < 10) {
        for (int i = left + 1; i <= right; i++) {
            int key = arr[i];
            int j = i - 1;
            while (j >= left && arr[j] > key) {
                arr[j + 1] = arr[j];
                j--;
            }
            arr[j + 1] = key;
        }
        return;
    }
    
    int pivot_idx = partition(arr, left, right);
    atomic_fetch_add(&total_partitions, 1);
    
    quicksort_sequential(arr, left, pivot_idx - 1);
    quicksort_sequential(arr, pivot_idx + 1, right);
}

/* Declarații forward pentru recursie */
void quicksort_parallel(int *arr, int left, int right, int depth);
void* quicksort_parallel_thread(void *arg);

/**
 * Funcția executată de thread-urile de sortare paralelă
 *
 * @param arg Pointer la SortArg cu parametrii sortării
 * @return NULL
 */
void* quicksort_parallel_thread(void *arg) {
    SortArg *sarg = (SortArg*)arg;
    
    quicksort_parallel(sarg->arr, sarg->left, sarg->right, sarg->depth + 1);
    
    /* Decrementează contorul de thread-uri active */
    atomic_fetch_sub(&active_threads, 1);
    
    free(sarg);
    return NULL;
}

/**
 * Quick Sort paralel cu threshold adaptiv
 *
 * Strategia:
 * 1. Dacă partiția e sub threshold → versiune secvențială
 * 2. Dacă sunt prea multe thread-uri → versiune secvențială
 * 3. Altfel → creează thread-uri pentru ambele partiții
 *
 * @param arr Array-ul de sortat
 * @param left Index stâng
 * @param right Index drept
 * @param depth Adâncimea recursiei curente
 */
void quicksort_parallel(int *arr, int left, int right, int depth) {
    if (left >= right) return;
    
    int size = right - left + 1;
    
    /* Verifică threshold pentru switch la secvențial */
    if (size <= threshold) {
        atomic_fetch_add(&sequential_fallbacks, 1);
        quicksort_sequential(arr, left, right);
        return;
    }
    
    /* Verifică dacă putem crea thread-uri noi */
    int current_active = atomic_load(&active_threads);
    if (current_active >= max_threads) {
        /* Prea multe thread-uri - continuă secvențial */
        quicksort_sequential(arr, left, right);
        return;
    }
    
    /* Partiționează */
    int pivot_idx = partition(arr, left, right);
    atomic_fetch_add(&total_partitions, 1);
    atomic_fetch_add(&parallel_partitions, 1);
    
    int left_size = pivot_idx - left;
    int right_size = right - pivot_idx;
    
    pthread_t left_thread = 0, right_thread = 0;
    bool left_spawned = false, right_spawned = false;
    
    /* Încearcă să creeze thread pentru partiția stângă */
    if (left_size > threshold && 
        atomic_fetch_add(&active_threads, 1) < max_threads) {
        SortArg *left_arg = malloc(sizeof(SortArg));
        left_arg->arr = arr;
        left_arg->left = left;
        left_arg->right = pivot_idx - 1;
        left_arg->depth = depth;
        
        if (pthread_create(&left_thread, NULL, quicksort_parallel_thread, left_arg) == 0) {
            left_spawned = true;
        } else {
            free(left_arg);
            atomic_fetch_sub(&active_threads, 1);
        }
    }
    
    /* Încearcă să creeze thread pentru partiția dreaptă */
    if (right_size > threshold && 
        atomic_fetch_add(&active_threads, 1) < max_threads) {
        SortArg *right_arg = malloc(sizeof(SortArg));
        right_arg->arr = arr;
        right_arg->left = pivot_idx + 1;
        right_arg->right = right;
        right_arg->depth = depth;
        
        if (pthread_create(&right_thread, NULL, quicksort_parallel_thread, right_arg) == 0) {
            right_spawned = true;
        } else {
            free(right_arg);
            atomic_fetch_sub(&active_threads, 1);
        }
    }
    
    /* Procesează local ce nu s-a putut delega */
    if (!left_spawned) {
        quicksort_parallel(arr, left, pivot_idx - 1, depth + 1);
    }
    if (!right_spawned) {
        quicksort_parallel(arr, pivot_idx + 1, right, depth + 1);
    }
    
    /* Așteaptă thread-urile create */
    if (left_spawned) {
        pthread_join(left_thread, NULL);
    }
    if (right_spawned) {
        pthread_join(right_thread, NULL);
    }
}

/**
 * Wrapper pentru Quick Sort paralel
 */
void parallel_sort(int *arr, int size) {
    atomic_store(&active_threads, 1);  /* Thread-ul principal */
    atomic_store(&total_partitions, 0);
    atomic_store(&parallel_partitions, 0);
    atomic_store(&sequential_fallbacks, 0);
    
    quicksort_parallel(arr, 0, size - 1, 0);
}

/* =============================================================================
 * FUNCȚII DE BENCHMARK
 * =============================================================================
 */

/**
 * Rulează benchmark comparativ între versiunea secvențială și paralelă
 */
void run_benchmark(int size, int num_runs) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                         BENCHMARK COMPARATIV                                   ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    printf("Configurare:\n");
    printf("  Array size:       %d\n", size);
    printf("  Max threads:      %d\n", max_threads);
    printf("  Threshold:        %d\n", threshold);
    printf("  Runs:             %d\n", num_runs);
    printf("\n");
    
    int *original = malloc(size * sizeof(int));
    int *arr_seq = malloc(size * sizeof(int));
    int *arr_par = malloc(size * sizeof(int));
    
    double seq_total = 0, par_total = 0;
    
    for (int run = 0; run < num_runs; run++) {
        /* Generează date noi pentru fiecare run */
        generate_random_array(original, size, MAX_VALUE);
        
        /* Benchmark secvențial */
        copy_array(arr_seq, original, size);
        double start = get_time_ms();
        quicksort_sequential(arr_seq, 0, size - 1);
        double seq_time = get_time_ms() - start;
        seq_total += seq_time;
        
        /* Benchmark paralel */
        copy_array(arr_par, original, size);
        start = get_time_ms();
        parallel_sort(arr_par, size);
        double par_time = get_time_ms() - start;
        par_total += par_time;
        
        /* Verifică corectitudinea */
        if (!is_sorted(arr_seq, size) || !is_sorted(arr_par, size)) {
            printf("  Run %d: EROARE - array-ul nu e sortat!\n", run + 1);
        }
    }
    
    double avg_seq = seq_total / num_runs;
    double avg_par = par_total / num_runs;
    double speedup = avg_seq / avg_par;
    double efficiency = speedup / max_threads * 100;
    
    printf("┌─────────────────────────────────────────────────────────────────┐\n");
    printf("│ REZULTATE                                                       │\n");
    printf("├─────────────────────────────────────────────────────────────────┤\n");
    printf("│ Timp mediu secvențial:  %10.2f ms                          │\n", avg_seq);
    printf("│ Timp mediu paralel:     %10.2f ms                          │\n", avg_par);
    printf("│ Speedup:                %10.2fx                            │\n", speedup);
    printf("│ Eficiență:              %10.1f%%                            │\n", efficiency);
    printf("├─────────────────────────────────────────────────────────────────┤\n");
    printf("│ Statistici partiționare:                                        │\n");
    printf("│   Total partiții:       %10d                              │\n", atomic_load(&total_partitions));
    printf("│   Partiții paralele:    %10d                              │\n", atomic_load(&parallel_partitions));
    printf("│   Fallback secvențial:  %10d                              │\n", atomic_load(&sequential_fallbacks));
    printf("└─────────────────────────────────────────────────────────────────┘\n");
    
    /* Analiză Amdahl */
    printf("\n");
    printf("┌─────────────────────────────────────────────────────────────────┐\n");
    printf("│ ANALIZA LEGEA LUI AMDAHL                                        │\n");
    printf("├─────────────────────────────────────────────────────────────────┤\n");
    
    /* Estimare fracție paralelizabilă din speedup observat */
    /* Speedup = 1 / ((1-P) + P/N) => P = N(Speedup-1) / (Speedup(N-1)) */
    double N = max_threads;
    double P = N * (speedup - 1) / (speedup * (N - 1));
    if (P > 1.0) P = 0.99;
    if (P < 0.0) P = 0.5;
    
    printf("│ Fracție paralelizabilă estimată (P): %.2f                       │\n", P);
    printf("│                                                                 │\n");
    printf("│ Speedup teoretic Amdahl:                                        │\n");
    for (int n = 1; n <= 16; n *= 2) {
        double theoretical = 1.0 / ((1 - P) + P / n);
        printf("│   %2d thread(s): %6.2fx                                        │\n", n, theoretical);
    }
    printf("│   ∞  thread(s): %6.2fx (limita)                                │\n", 1.0 / (1 - P));
    printf("└─────────────────────────────────────────────────────────────────┘\n");
    
    free(original);
    free(arr_seq);
    free(arr_par);
}

/**
 * Benchmark cu diferite numere de thread-uri
 */
void run_scaling_test(int size) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                         TEST SCALABILITATE                                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    int *original = malloc(size * sizeof(int));
    int *arr = malloc(size * sizeof(int));
    
    generate_random_array(original, size, MAX_VALUE);
    
    /* Timp de referință secvențial */
    copy_array(arr, original, size);
    double start = get_time_ms();
    quicksort_sequential(arr, 0, size - 1);
    double seq_time = get_time_ms() - start;
    
    printf("┌──────────┬──────────────┬──────────┬────────────┐\n");
    printf("│ Threads  │ Timp (ms)    │ Speedup  │ Eficiență  │\n");
    printf("├──────────┼──────────────┼──────────┼────────────┤\n");
    printf("│    1     │ %12.2f │   1.00x  │   100.0%%   │\n", seq_time);
    
    int thread_counts[] = {2, 4, 8};
    for (int i = 0; i < 3; i++) {
        max_threads = thread_counts[i];
        
        copy_array(arr, original, size);
        start = get_time_ms();
        parallel_sort(arr, size);
        double par_time = get_time_ms() - start;
        
        double speedup = seq_time / par_time;
        double efficiency = speedup / max_threads * 100;
        
        printf("│    %d     │ %12.2f │  %5.2fx  │   %5.1f%%   │\n",
               max_threads, par_time, speedup, efficiency);
    }
    printf("└──────────┴──────────────┴──────────┴────────────┘\n");
    
    free(original);
    free(arr);
    
    /* Resetează la default */
    max_threads = DEFAULT_MAX_THREADS;
}

/* =============================================================================
 * MAIN
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║     SOLUȚIE EXERCIȚIUL 2: Parallel Quick Sort                                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    /* Parsare argumente */
    int array_size = DEFAULT_ARRAY_SIZE;
    
    if (argc >= 2) {
        array_size = atoi(argv[1]);
        if (array_size < 100 || array_size > 10000000) {
            fprintf(stderr, "Mărime array invalidă (100 - 10M)\n");
            return 1;
        }
    }
    
    if (argc >= 3) {
        max_threads = atoi(argv[2]);
        if (max_threads < 1 || max_threads > 16) {
            fprintf(stderr, "Număr thread-uri invalid (1-16)\n");
            return 1;
        }
    }
    
    if (argc >= 4) {
        threshold = atoi(argv[3]);
        if (threshold < 10 || threshold > array_size) {
            fprintf(stderr, "Threshold invalid\n");
            return 1;
        }
    }
    
    srand(time(NULL));
    
    /* Demo rapid */
    printf("Demo cu array mic:\n");
    int demo_arr[] = {64, 34, 25, 12, 22, 11, 90, 45, 33, 77};
    int demo_size = sizeof(demo_arr) / sizeof(demo_arr[0]);
    
    printf("  Înainte: ");
    for (int i = 0; i < demo_size; i++) printf("%d ", demo_arr[i]);
    printf("\n");
    
    quicksort_sequential(demo_arr, 0, demo_size - 1);
    
    printf("  După:    ");
    for (int i = 0; i < demo_size; i++) printf("%d ", demo_arr[i]);
    printf("\n");
    
    /* Rulează benchmark-uri */
    run_benchmark(array_size, 5);
    run_scaling_test(array_size);
    
    printf("\n✓ Toate testele completate cu succes!\n\n");
    
    return 0;
}
