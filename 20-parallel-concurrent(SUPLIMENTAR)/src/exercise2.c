/**
 * =============================================================================
 * EXERCIȚIUL 2: Parallel Quick Sort cu Threshold Adaptiv
 * =============================================================================
 *
 * OBIECTIV:
 *   Implementarea unui algoritm Quick Sort paralel care utilizează thread-uri
 *   pentru a sorta simultan partițiile rezultate. Include threshold adaptiv
 *   pentru a evita overhead-ul pe array-uri mici.
 *
 * CERINȚE:
 *   1. Implementare Quick Sort secvențial (pentru comparație și fallback)
 *   2. Implementare Quick Sort paralel cu creare thread-uri pentru partiții
 *   3. Threshold configurabil pentru switch la varianta secvențială
 *   4. Limitare număr maxim de thread-uri active
 *   5. Benchmark comparativ și calcul speedup
 *
 * EXEMPLU UTILIZARE:
 *   ./exercise2                     # Folosește date simulate, 4 threads
 *   ./exercise2 numbers.txt 8       # Citește din fișier, 8 threads
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -O2 -o exercise2 exercise2.c -lpthread
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

/* =============================================================================
 * STRUCTURI DE DATE
 * =============================================================================
 */

/**
 * TODO 1: Definiți structura pentru argumentele thread-urilor de sortare
 *
 * Hint: Aveți nevoie de:
 *   - Pointer la array
 *   - Indicii left și right pentru subarray
 *   - Adâncime curentă (pentru limitare recursie)
 */
typedef struct {
    /* YOUR CODE HERE */
    int *arr;
    int left;
    int right;
    int depth;
} SortArg;

/* =============================================================================
 * FUNCȚII HELPER
 * =============================================================================
 */

/**
 * Obține timestamp în milisecunde
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
 * Verifică dacă array-ul este sortat
 */
bool is_sorted(int *arr, int n) {
    for (int i = 1; i < n; i++) {
        if (arr[i] < arr[i-1]) return false;
    }
    return true;
}

/**
 * Generează array cu numere random
 */
int* generate_random_array(int n) {
    int *arr = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % MAX_VALUE;
    }
    return arr;
}

/**
 * Copiază un array
 */
int* copy_array(int *src, int n) {
    int *dst = malloc(n * sizeof(int));
    memcpy(dst, src, n * sizeof(int));
    return dst;
}

/**
 * Afișează primele și ultimele elemente ale array-ului
 */
void print_array_preview(int *arr, int n, const char *label) {
    printf("%s (n=%d): [", label, n);
    int show = n < 10 ? n : 5;
    for (int i = 0; i < show; i++) {
        printf("%d%s", arr[i], i < show-1 ? ", " : "");
    }
    if (n > 10) {
        printf(", ... , ");
        for (int i = n - 3; i < n; i++) {
            printf("%d%s", arr[i], i < n-1 ? ", " : "");
        }
    }
    printf("]\n");
}

/* =============================================================================
 * TODO 2: IMPLEMENTAȚI ALGORITMII DE SORTARE
 * =============================================================================
 */

/**
 * Funcția de partiționare (Lomuto partition scheme)
 *
 * @param arr Array-ul de partiționat
 * @param left Index stânga
 * @param right Index dreapta
 * @return Index-ul pivotului după partiționare
 *
 * Algoritm:
 *   1. Alege pivotul (ultimul element)
 *   2. Inițializează i = left - 1
 *   3. Pentru j de la left la right-1:
 *      - Dacă arr[j] < pivot: incrementează i și swap(arr[i], arr[j])
 *   4. Swap(arr[i+1], arr[right]) - pune pivotul la locul lui
 *   5. Returnează i + 1
 */
int partition(int *arr, int left, int right) {
    /* YOUR CODE HERE */
    
    /* Alegere pivot random pentru performanță mai bună */
    int pivot_idx = left + rand() % (right - left + 1);
    swap(&arr[pivot_idx], &arr[right]);
    
    int pivot = arr[right];
    int i = left - 1;
    
    for (int j = left; j < right; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    
    swap(&arr[i + 1], &arr[right]);
    return i + 1;
}

/**
 * Quick Sort secvențial (recursiv)
 *
 * @param arr Array-ul de sortat
 * @param left Index stânga
 * @param right Index dreapta
 */
void quicksort_sequential(int *arr, int left, int right) {
    /* YOUR CODE HERE */
    
    if (left < right) {
        int pivot = partition(arr, left, right);
        quicksort_sequential(arr, left, pivot - 1);
        quicksort_sequential(arr, pivot + 1, right);
    }
}

/**
 * Quick Sort paralel - funcție pentru thread
 *
 * Logică:
 *   1. Verifică condiția de bază (left >= right)
 *   2. Calculează dimensiunea subarray-ului
 *   3. Dacă sub threshold SAU prea multe thread-uri active: folosește secvențial
 *   4. Altfel: partiționează și creează thread nou pentru o jumătate
 *   5. Procesează cealaltă jumătate în thread-ul curent
 *   6. Așteaptă thread-ul creat (join)
 */
void* quicksort_parallel_thread(void *arg) {
    SortArg *sa = (SortArg*)arg;
    
    /* YOUR CODE HERE */
    
    /* Condiție de bază */
    if (sa->left >= sa->right) {
        return NULL;
    }
    
    int n = sa->right - sa->left + 1;
    
    /* Verifică dacă trebuie să folosim varianta secvențială */
    if (n < threshold || atomic_load(&active_threads) >= max_threads || sa->depth <= 0) {
        quicksort_sequential(sa->arr, sa->left, sa->right);
        return NULL;
    }
    
    /* Partiționare */
    int pivot = partition(sa->arr, sa->left, sa->right);
    
    /* Creează thread pentru jumătatea stângă */
    SortArg left_arg = {sa->arr, sa->left, pivot - 1, sa->depth - 1};
    pthread_t left_thread;
    
    /* Incrementează contorul de thread-uri active */
    atomic_fetch_add(&active_threads, 1);
    
    int rc = pthread_create(&left_thread, NULL, quicksort_parallel_thread, &left_arg);
    
    if (rc != 0) {
        /* Fallback la secvențial dacă nu putem crea thread */
        atomic_fetch_sub(&active_threads, 1);
        quicksort_sequential(sa->arr, sa->left, pivot - 1);
    }
    
    /* Procesează jumătatea dreaptă în thread-ul curent */
    SortArg right_arg = {sa->arr, pivot + 1, sa->right, sa->depth - 1};
    quicksort_parallel_thread(&right_arg);
    
    /* Așteaptă thread-ul stâng */
    if (rc == 0) {
        pthread_join(left_thread, NULL);
        atomic_fetch_sub(&active_threads, 1);
    }
    
    return NULL;
}

/**
 * Wrapper pentru Quick Sort paralel
 */
void quicksort_parallel(int *arr, int n, int num_threads) {
    max_threads = num_threads;
    atomic_store(&active_threads, 0);
    
    /* Calculează adâncimea maximă bazată pe numărul de thread-uri */
    int max_depth = 0;
    int t = num_threads;
    while (t > 0) {
        max_depth++;
        t /= 2;
    }
    max_depth += 2;  /* Un pic de buffer */
    
    SortArg arg = {arr, 0, n - 1, max_depth};
    quicksort_parallel_thread(&arg);
}

/* =============================================================================
 * FUNCȚII DE BENCHMARK
 * =============================================================================
 */

/**
 * Rulează benchmark și afișează rezultate
 */
void run_benchmark(int n, int num_threads_arr[], int num_configs) {
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════════════════════\n");
    printf("                         BENCHMARK PARALLEL QUICK SORT                          \n");
    printf("═══════════════════════════════════════════════════════════════════════════════\n");
    printf("\n");
    printf("Configurație:\n");
    printf("  Array size:  %d elemente\n", n);
    printf("  Threshold:   %d elemente\n", threshold);
    printf("  Max value:   %d\n", MAX_VALUE);
    printf("\n");
    
    /* Generează array original */
    srand(42);  /* Seed fix pentru reproducibilitate */
    int *original = generate_random_array(n);
    
    /* Benchmark secvențial */
    int *arr_seq = copy_array(original, n);
    
    printf("Rulare secvențială...\n");
    double start = get_time_ms();
    quicksort_sequential(arr_seq, 0, n - 1);
    double seq_time = get_time_ms() - start;
    
    printf("  Timp: %.2f ms\n", seq_time);
    printf("  Verificare: %s\n", is_sorted(arr_seq, n) ? "✓ SORTAT CORECT" : "✗ EROARE");
    printf("\n");
    
    /* Benchmark paralel cu diferite numere de thread-uri */
    printf("┌──────────┬──────────────┬──────────────┬──────────────┐\n");
    printf("│ Threads  │  Timp (ms)   │   Speedup    │   Eficiență  │\n");
    printf("├──────────┼──────────────┼──────────────┼──────────────┤\n");
    
    for (int i = 0; i < num_configs; i++) {
        int num_threads = num_threads_arr[i];
        int *arr_par = copy_array(original, n);
        
        start = get_time_ms();
        quicksort_parallel(arr_par, n, num_threads);
        double par_time = get_time_ms() - start;
        
        double speedup = seq_time / par_time;
        double efficiency = speedup / num_threads * 100.0;
        
        bool correct = is_sorted(arr_par, n);
        
        printf("│    %2d    │   %8.2f   │    %5.2fx    │   %6.1f%%   │%s\n",
               num_threads, par_time, speedup, efficiency,
               correct ? "" : " ✗");
        
        free(arr_par);
    }
    
    printf("└──────────┴──────────────┴──────────────┴──────────────┘\n");
    
    /* Curățare */
    free(original);
    free(arr_seq);
    
    /* Calcul și afișare Legea lui Amdahl */
    printf("\n");
    printf("Analiză Amdahl:\n");
    printf("  Dacă 90%% din cod este paralelizabil:\n");
    printf("  - Speedup teoretic maxim (∞ threads): 10.00x\n");
    printf("  - Speedup teoretic cu 4 threads: 3.08x\n");
    printf("  - Speedup teoretic cu 8 threads: 4.71x\n");
    printf("\n");
}

/* =============================================================================
 * CITIRE DIN FIȘIER
 * =============================================================================
 */

/**
 * Citește array din fișier (un număr per linie)
 */
int* read_array_from_file(const char *filename, int *out_n) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "Nu pot deschide fișierul: %s\n", filename);
        return NULL;
    }
    
    /* Prima trecere: numără liniile */
    int n = 0;
    int temp;
    while (fscanf(f, "%d", &temp) == 1) {
        n++;
    }
    
    if (n == 0) {
        fclose(f);
        fprintf(stderr, "Fișier gol sau invalid!\n");
        return NULL;
    }
    
    /* A doua trecere: citește numerele */
    rewind(f);
    int *arr = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        fscanf(f, "%d", &arr[i]);
    }
    
    fclose(f);
    *out_n = n;
    return arr;
}

/* =============================================================================
 * MAIN
 * =============================================================================
 */

void print_usage(const char *prog_name) {
    printf("Utilizare: %s [fisier] [num_threads] [threshold]\n", prog_name);
    printf("  Implicit: date random (%d elemente), %d threads, threshold=%d\n",
           DEFAULT_ARRAY_SIZE, DEFAULT_MAX_THREADS, DEFAULT_THRESHOLD);
}

int main(int argc, char *argv[]) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║     EXERCIȚIUL 2: Parallel Quick Sort cu Threshold Adaptiv                    ║\n");
    printf("║     Săptămâna 20 - Programare Paralelă                                        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    /* Parsare argumente */
    char *filename = NULL;
    int n = DEFAULT_ARRAY_SIZE;
    int num_threads = DEFAULT_MAX_THREADS;
    int *arr = NULL;
    
    if (argc >= 2 && strcmp(argv[1], "-h") == 0) {
        print_usage(argv[0]);
        return 0;
    }
    
    if (argc >= 2) filename = argv[1];
    if (argc >= 3) num_threads = atoi(argv[2]);
    if (argc >= 4) threshold = atoi(argv[3]);
    
    if (num_threads < 1) num_threads = 1;
    if (threshold < 10) threshold = 10;
    
    /* Citire sau generare date */
    if (filename) {
        arr = read_array_from_file(filename, &n);
        if (!arr) {
            printf("Folosesc date generate...\n");
            filename = NULL;
        }
    }
    
    if (!filename) {
        srand(time(NULL));
        arr = generate_random_array(n);
    }
    
    printf("Date:\n");
    printf("  Sursă: %s\n", filename ? filename : "(generate random)");
    printf("  Elemente: %d\n", n);
    printf("  Max threads: %d\n", num_threads);
    printf("  Threshold: %d\n", threshold);
    printf("\n");
    
    print_array_preview(arr, n, "Array inițial");
    
    /* Rulează benchmark cu diferite configurații */
    int thread_configs[] = {1, 2, 4, 8};
    int num_configs = 4;
    
    /* Ajustează configurațiile în funcție de num_threads solicitat */
    for (int i = 0; i < num_configs; i++) {
        if (thread_configs[i] > num_threads) {
            num_configs = i;
            break;
        }
    }
    if (num_configs == 0) {
        thread_configs[0] = num_threads;
        num_configs = 1;
    }
    
    /* Copiază array-ul original pentru benchmark */
    int *benchmark_arr = copy_array(arr, n);
    
    run_benchmark(n, thread_configs, num_configs);
    
    /* Sortare finală și afișare rezultat */
    printf("\nSortare finală cu %d threads:\n", num_threads);
    
    double start = get_time_ms();
    quicksort_parallel(arr, n, num_threads);
    double elapsed = get_time_ms() - start;
    
    print_array_preview(arr, n, "Array sortat");
    printf("\n");
    printf("Timp: %.2f ms\n", elapsed);
    printf("Verificare: %s\n", is_sorted(arr, n) ? "✓ ARRAY SORTAT CORECT" : "✗ EROARE SORTARE");
    printf("\n");
    
    /* Curățare */
    free(arr);
    free(benchmark_arr);
    
    return 0;
}
