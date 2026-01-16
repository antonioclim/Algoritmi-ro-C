/**
 * =============================================================================
 * BENCHMARK: Sortare Secvențială vs. Paralelă
 * =============================================================================
 *
 * Acest program compară performanța sortării secvențiale vs. paralelă
 * pentru diferite dimensiuni de array și număr de thread-uri.
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -O2 -o benchmark_threads benchmark_threads.c -lpthread -lm
 * Execuție:  ./benchmark_threads [array_size]
 *
 * Academia de Studii Economice București - CSIE
 * Algoritmi și Tehnici de Programare (ATP)
 * =============================================================================
 */

#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>
#include <stdatomic.h>

/* =============================================================================
 * CONFIGURAȚIE
 * =============================================================================
 */

#define DEFAULT_ARRAY_SIZE 1000000
#define THRESHOLD 10000          /* Sub acest prag, sortare secvențială */
#define MAX_THREADS 8
#define NUM_RUNS 5               /* Număr de rulări pentru medie */

/* =============================================================================
 * STRUCTURI
 * =============================================================================
 */

typedef struct {
    int *arr;
    size_t left;
    size_t right;
    int depth;
} SortTask;

typedef struct {
    double sequential_time;
    double parallel_time[MAX_THREADS + 1];
    size_t array_size;
    int num_runs;
} BenchmarkResult;

/* =============================================================================
 * VARIABILE GLOBALE
 * =============================================================================
 */

static atomic_int active_threads = 0;
static int max_threads = 4;

/* =============================================================================
 * UTILITĂȚI TIMP
 * =============================================================================
 */

double get_time_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1000000.0;
}

/* =============================================================================
 * FUNCȚII SORTARE
 * =============================================================================
 */

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * Partiționare Lomuto cu pivot aleator
 */
size_t partition(int *arr, size_t left, size_t right) {
    /* Pivot aleator pentru performanță medie mai bună */
    size_t pivot_idx = left + rand() % (right - left + 1);
    swap(&arr[pivot_idx], &arr[right]);
    
    int pivot = arr[right];
    size_t i = left;
    
    for (size_t j = left; j < right; j++) {
        if (arr[j] <= pivot) {
            swap(&arr[i], &arr[j]);
            i++;
        }
    }
    swap(&arr[i], &arr[right]);
    return i;
}

/**
 * Quick Sort secvențial
 */
void quicksort_sequential(int *arr, size_t left, size_t right) {
    if (left >= right || right == (size_t)-1) return;
    
    size_t pivot_idx = partition(arr, left, right);
    
    if (pivot_idx > 0) {
        quicksort_sequential(arr, left, pivot_idx - 1);
    }
    quicksort_sequential(arr, pivot_idx + 1, right);
}

/**
 * Quick Sort paralel - funcția thread
 */
void* quicksort_parallel_thread(void *arg) {
    SortTask *task = (SortTask*)arg;
    int *arr = task->arr;
    size_t left = task->left;
    size_t right = task->right;
    int depth = task->depth;
    
    free(task);
    
    if (left >= right || right == (size_t)-1) return NULL;
    
    size_t size = right - left + 1;
    
    /* Dacă sub prag sau prea adânc, sortare secvențială */
    if (size < THRESHOLD || depth > 4 || atomic_load(&active_threads) >= max_threads) {
        quicksort_sequential(arr, left, right);
        return NULL;
    }
    
    size_t pivot_idx = partition(arr, left, right);
    
    /* Încearcă să creeze thread pentru partea stângă */
    pthread_t thread;
    bool thread_created = false;
    
    if (pivot_idx > left && atomic_load(&active_threads) < max_threads) {
        SortTask *left_task = malloc(sizeof(SortTask));
        if (left_task) {
            left_task->arr = arr;
            left_task->left = left;
            left_task->right = pivot_idx - 1;
            left_task->depth = depth + 1;
            
            atomic_fetch_add(&active_threads, 1);
            if (pthread_create(&thread, NULL, quicksort_parallel_thread, left_task) == 0) {
                thread_created = true;
            } else {
                atomic_fetch_sub(&active_threads, 1);
                free(left_task);
            }
        }
    }
    
    /* Sortează partea dreaptă în thread-ul curent */
    if (pivot_idx < right) {
        SortTask right_task = {arr, pivot_idx + 1, right, depth + 1};
        quicksort_parallel_thread(&right_task);
    }
    
    /* Așteaptă thread-ul creat */
    if (thread_created) {
        pthread_join(thread, NULL);
        atomic_fetch_sub(&active_threads, 1);
    } else if (pivot_idx > left) {
        /* Nu s-a putut crea thread, sortează secvențial */
        quicksort_sequential(arr, left, pivot_idx - 1);
    }
    
    return NULL;
}

/**
 * Wrapper pentru sortare paralelă
 */
void quicksort_parallel(int *arr, size_t n, int num_threads) {
    if (n <= 1) return;
    
    max_threads = num_threads;
    atomic_store(&active_threads, 1);  /* Thread-ul principal */
    
    SortTask *task = malloc(sizeof(SortTask));
    task->arr = arr;
    task->left = 0;
    task->right = n - 1;
    task->depth = 0;
    
    quicksort_parallel_thread(task);
}

/* =============================================================================
 * FUNCȚII BENCHMARK
 * =============================================================================
 */

/**
 * Verifică dacă array-ul este sortat
 */
bool is_sorted(int *arr, size_t n) {
    for (size_t i = 1; i < n; i++) {
        if (arr[i] < arr[i-1]) return false;
    }
    return true;
}

/**
 * Generează array random
 */
void generate_random_array(int *arr, size_t n) {
    for (size_t i = 0; i < n; i++) {
        arr[i] = rand() % 1000000;
    }
}

/**
 * Copiază array
 */
void copy_array(int *dest, int *src, size_t n) {
    memcpy(dest, src, n * sizeof(int));
}

/**
 * Rulează benchmark-ul
 */
BenchmarkResult run_benchmark(size_t array_size, int num_runs) {
    BenchmarkResult result;
    result.array_size = array_size;
    result.num_runs = num_runs;
    result.sequential_time = 0;
    
    for (int i = 0; i <= MAX_THREADS; i++) {
        result.parallel_time[i] = 0;
    }
    
    /* Alocă array-uri */
    int *original = malloc(array_size * sizeof(int));
    int *working = malloc(array_size * sizeof(int));
    
    if (!original || !working) {
        fprintf(stderr, "Eroare alocare memorie!\n");
        exit(1);
    }
    
    /* Rulări multiple pentru medie */
    for (int run = 0; run < num_runs; run++) {
        printf("  Run %d/%d...\r", run + 1, num_runs);
        fflush(stdout);
        
        /* Generează date noi pentru fiecare rulare */
        srand(42 + run);  /* Seed diferit dar reproductibil */
        generate_random_array(original, array_size);
        
        /* Benchmark secvențial */
        copy_array(working, original, array_size);
        double start = get_time_ms();
        quicksort_sequential(working, 0, array_size - 1);
        result.sequential_time += get_time_ms() - start;
        
        if (!is_sorted(working, array_size)) {
            fprintf(stderr, "EROARE: Sortare secvențială incorectă!\n");
            exit(1);
        }
        
        /* Benchmark paralel pentru diferite numere de thread-uri */
        for (int threads = 1; threads <= MAX_THREADS; threads++) {
            copy_array(working, original, array_size);
            start = get_time_ms();
            quicksort_parallel(working, array_size, threads);
            result.parallel_time[threads] += get_time_ms() - start;
            
            if (!is_sorted(working, array_size)) {
                fprintf(stderr, "EROARE: Sortare paralelă (%d threads) incorectă!\n", threads);
                exit(1);
            }
        }
    }
    
    /* Calculează medii */
    result.sequential_time /= num_runs;
    for (int i = 1; i <= MAX_THREADS; i++) {
        result.parallel_time[i] /= num_runs;
    }
    
    free(original);
    free(working);
    
    return result;
}

/**
 * Afișează rezultatele
 */
void print_results(BenchmarkResult *result) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                        REZULTATE BENCHMARK                                    ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    printf("Configurație:\n");
    printf("  • Dimensiune array: %zu elemente\n", result->array_size);
    printf("  • Număr rulări: %d\n", result->num_runs);
    printf("  • Prag secvențial: %d elemente\n", THRESHOLD);
    printf("\n");
    
    printf("Timp sortare secvențială: %.2f ms\n\n", result->sequential_time);
    
    printf("┌──────────┬────────────────┬──────────────┬──────────────┐\n");
    printf("│ Threads  │  Timp (ms)     │   Speedup    │  Eficiență   │\n");
    printf("├──────────┼────────────────┼──────────────┼──────────────┤\n");
    
    for (int t = 1; t <= MAX_THREADS; t++) {
        double speedup = result->sequential_time / result->parallel_time[t];
        double efficiency = speedup / t * 100;
        
        printf("│    %d     │    %8.2f    │    %5.2fx    │   %5.1f%%    │\n",
               t, result->parallel_time[t], speedup, efficiency);
    }
    
    printf("└──────────┴────────────────┴──────────────┴──────────────┘\n");
    
    /* Calculează și afișează Amdahl's Law */
    printf("\n");
    printf("Analiza Amdahl's Law:\n");
    
    /* Estimare fracție paralelă din speedup-ul cu 2 threads */
    double S2 = result->sequential_time / result->parallel_time[2];
    /* Speedup(2) = 1 / ((1-P) + P/2) => P = (2 - 2/S2) / (2 - 1) */
    double P = 2.0 * (1.0 - 1.0/S2);
    if (P > 1.0) P = 0.95;
    if (P < 0.0) P = 0.5;
    
    printf("  • Fracție paralelizabilă estimată: %.1f%%\n", P * 100);
    printf("  • Speedup maxim teoretic (∞ CPU): %.2fx\n", 1.0 / (1.0 - P));
    printf("\n");
    
    printf("Comparație Teoretic vs. Real:\n");
    printf("┌──────────┬────────────────┬────────────────┐\n");
    printf("│ Threads  │ Speedup Real   │ Amdahl Teoretic│\n");
    printf("├──────────┼────────────────┼────────────────┤\n");
    
    for (int t = 1; t <= MAX_THREADS; t++) {
        double real_speedup = result->sequential_time / result->parallel_time[t];
        double amdahl_speedup = 1.0 / ((1.0 - P) + P / t);
        
        printf("│    %d     │     %5.2fx     │     %5.2fx     │\n",
               t, real_speedup, amdahl_speedup);
    }
    
    printf("└──────────┴────────────────┴────────────────┘\n");
}

/**
 * Exportă rezultate pentru grafice (CSV)
 */
void export_csv(BenchmarkResult *result, const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) {
        fprintf(stderr, "Nu pot deschide %s pentru scriere\n", filename);
        return;
    }
    
    fprintf(f, "threads,time_ms,speedup,efficiency\n");
    fprintf(f, "0,%.2f,1.00,100.0\n", result->sequential_time);  /* 0 = secvențial */
    
    for (int t = 1; t <= MAX_THREADS; t++) {
        double speedup = result->sequential_time / result->parallel_time[t];
        double efficiency = speedup / t * 100;
        fprintf(f, "%d,%.2f,%.2f,%.1f\n", t, result->parallel_time[t], speedup, efficiency);
    }
    
    fclose(f);
    printf("\nRezultate exportate în: %s\n", filename);
}

/* =============================================================================
 * MAIN
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║     BENCHMARK: Sortare Secvențială vs. Paralelă                              ║\n");
    printf("║     Quick Sort cu POSIX Threads                                              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    /* Parsează argumente */
    size_t array_size = DEFAULT_ARRAY_SIZE;
    if (argc > 1) {
        array_size = (size_t)atol(argv[1]);
        if (array_size < 1000) {
            array_size = 1000;
        }
    }
    
    printf("Rulare benchmark cu %zu elemente...\n", array_size);
    printf("\n");
    
    /* Rulează benchmark */
    BenchmarkResult result = run_benchmark(array_size, NUM_RUNS);
    
    /* Afișează rezultate */
    print_results(&result);
    
    /* Exportă CSV */
    export_csv(&result, "benchmark_results.csv");
    
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════════════════════\n");
    printf("Benchmark completat cu succes!\n");
    printf("═══════════════════════════════════════════════════════════════════════════════\n");
    
    return 0;
}
