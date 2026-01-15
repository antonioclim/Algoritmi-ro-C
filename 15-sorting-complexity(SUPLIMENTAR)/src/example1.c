/**
 * =============================================================================
 * SĂPTĂMÂNA 15: ALGORITMI DE SORTARE AVANSAȚI ȘI ANALIZA COMPLEXITĂȚII
 * Exemplu Complet și Demonstrativ
 * =============================================================================
 *
 * Acest exemplu demonstrează:
 *   1. Quick Sort (clasic, randomizat, median-of-three)
 *   2. Merge Sort (top-down recursiv)
 *   3. Heap Sort (in-place cu build-heap)
 *   4. Counting Sort (pentru întregi în interval limitat)
 *   5. Radix Sort LSD (sortare cifră cu cifră)
 *   6. Introsort (algoritm hibrid adaptiv)
 *   7. Analiză empirică pe diferite distribuții de date
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -g -O2 -o example1 example1.c -lm
 * Execuție:  ./example1
 * Valgrind:  valgrind --leak-check=full ./example1 --quick
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
 * PARTEA 1: CONSTANTE ȘI STRUCTURI AUXILIARE
 * =============================================================================
 */

#define MAX_SIZE 100000
#define SMALL_THRESHOLD 16    /* Pentru Insertion Sort în algoritmi hibrizi */

/* Statistici pentru analiză */
typedef struct {
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
 * Afișează un array (primele max_display elemente)
 */
void print_array(int arr[], int n, int max_display) {
    printf("[");
    int limit = (n < max_display) ? n : max_display;
    for (int i = 0; i < limit; i++) {
        printf("%d", arr[i]);
        if (i < limit - 1) printf(", ");
    }
    if (n > max_display) {
        printf(", ... (%d elemente în total)", n);
    }
    printf("]\n");
}

/**
 * Verifică dacă array-ul este sortat crescător
 */
bool is_sorted(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        if (arr[i] > arr[i + 1]) return false;
    }
    return true;
}

/**
 * Copiază un array
 */
void copy_array(int dest[], int src[], int n) {
    memcpy(dest, src, n * sizeof(int));
}

/**
 * Generează array aleator
 */
void generate_random(int arr[], int n, int max_val) {
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % max_val;
    }
}

/**
 * Generează array sortat crescător
 */
void generate_sorted(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        arr[i] = i;
    }
}

/**
 * Generează array sortat descrescător
 */
void generate_reversed(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        arr[i] = n - i;
    }
}

/**
 * Generează array aproape sortat (5% perturbat)
 */
void generate_nearly_sorted(int arr[], int n) {
    generate_sorted(arr, n);
    int swaps_count = n / 20;  /* 5% perturbări */
    for (int i = 0; i < swaps_count; i++) {
        int idx1 = rand() % n;
        int idx2 = rand() % n;
        swap(&arr[idx1], &arr[idx2]);
    }
}

/**
 * Afișează separatorul vizual
 */
void print_separator(const char *title) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║  %-61s║\n", title);
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

/**
 * Afișează subsecțiune
 */
void print_subsection(const char *title) {
    printf("\n┌─── %s ───┐\n\n", title);
}

/* =============================================================================
 * PARTEA 3: INSERTION SORT (pentru algoritmi hibrizi)
 * =============================================================================
 */

/**
 * Insertion Sort - eficient pentru array-uri mici
 * 
 * Complexitate: O(n²), dar foarte rapid pentru n < 16
 */
void insertion_sort(int arr[], int low, int high) {
    for (int i = low + 1; i <= high; i++) {
        int key = arr[i];
        int j = i - 1;
        
        while (j >= low && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

/* =============================================================================
 * PARTEA 4: QUICK SORT - MULTIPLE VARIANTE
 * =============================================================================
 */

/**
 * Partiționare Lomuto (pivot = ultimul element)
 * 
 * @return Poziția finală a pivotului
 */
int partition_lomuto(int arr[], int low, int high) {
    int pivot = arr[high];
    int i = low - 1;
    
    for (int j = low; j < high; j++) {
        if (arr[j] <= pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return i + 1;
}

/**
 * Quick Sort clasic (pivot = ultimul element)
 * 
 * Worst case: O(n²) pe array sortat
 */
void quick_sort_classic(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition_lomuto(arr, low, high);
        quick_sort_classic(arr, low, pi - 1);
        quick_sort_classic(arr, pi + 1, high);
    }
}

/**
 * Partiționare cu pivot aleator
 */
int partition_random(int arr[], int low, int high) {
    int pivot_idx = low + rand() % (high - low + 1);
    swap(&arr[pivot_idx], &arr[high]);
    return partition_lomuto(arr, low, high);
}

/**
 * Quick Sort randomizat
 * 
 * Expected time: O(n log n) cu probabilitate mare
 */
void quick_sort_random(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition_random(arr, low, high);
        quick_sort_random(arr, low, pi - 1);
        quick_sort_random(arr, pi + 1, high);
    }
}

/**
 * Calculează mediana din trei valori
 */
int median_of_three(int arr[], int low, int mid, int high) {
    if (arr[low] > arr[mid]) {
        if (arr[mid] > arr[high]) return mid;
        else if (arr[low] > arr[high]) return high;
        else return low;
    } else {
        if (arr[low] > arr[high]) return low;
        else if (arr[mid] > arr[high]) return high;
        else return mid;
    }
}

/**
 * Partiționare median-of-three
 */
int partition_median_of_three(int arr[], int low, int high) {
    int mid = low + (high - low) / 2;
    int pivot_idx = median_of_three(arr, low, mid, high);
    swap(&arr[pivot_idx], &arr[high]);
    return partition_lomuto(arr, low, high);
}

/**
 * Quick Sort median-of-three
 * 
 * Evită worst case pe date sortate
 */
void quick_sort_median(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition_median_of_three(arr, low, high);
        quick_sort_median(arr, low, pi - 1);
        quick_sort_median(arr, pi + 1, high);
    }
}

/* Wrapper pentru interfață uniformă */
void quick_sort(int arr[], int n) {
    quick_sort_random(arr, 0, n - 1);
}

/* =============================================================================
 * PARTEA 5: MERGE SORT
 * =============================================================================
 */

/**
 * Funcția de merge - interclasează două subliste sortate
 * 
 * @param arr Array-ul principal
 * @param left Indexul de start al primei subliste
 * @param mid Indexul de mijloc
 * @param right Indexul de final al celei de-a doua subliste
 */
void merge(int arr[], int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    
    /* Alocăm array-uri temporare */
    int *L = (int*)malloc(n1 * sizeof(int));
    int *R = (int*)malloc(n2 * sizeof(int));
    
    if (L == NULL || R == NULL) {
        fprintf(stderr, "Eroare: Nu s-a putut aloca memorie pentru merge!\n");
        free(L);
        free(R);
        return;
    }
    
    /* Copiem datele în array-urile temporare */
    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];
    
    /* Interclasăm cele două array-uri înapoi în arr[] */
    int i = 0, j = 0, k = left;
    
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
    
    /* Copiem elementele rămase din L[] */
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }
    
    /* Copiem elementele rămase din R[] */
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
    
    free(L);
    free(R);
}

/**
 * Merge Sort recursiv (top-down)
 * 
 * Complexitate: O(n log n) garantat
 * Spațiu auxiliar: O(n)
 * Stabil: DA
 */
void merge_sort_recursive(int arr[], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        
        merge_sort_recursive(arr, left, mid);
        merge_sort_recursive(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

/* Wrapper pentru interfață uniformă */
void merge_sort(int arr[], int n) {
    merge_sort_recursive(arr, 0, n - 1);
}

/* =============================================================================
 * PARTEA 6: HEAP SORT
 * =============================================================================
 */

/**
 * Heapify (sift-down) - menține proprietatea max-heap
 * 
 * @param arr Array-ul
 * @param n Dimensiunea heap-ului
 * @param i Indexul nodului de reparat
 */
void heapify(int arr[], int n, int i) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    
    if (left < n && arr[left] > arr[largest])
        largest = left;
    
    if (right < n && arr[right] > arr[largest])
        largest = right;
    
    if (largest != i) {
        swap(&arr[i], &arr[largest]);
        heapify(arr, n, largest);
    }
}

/**
 * Heap Sort
 * 
 * Complexitate: O(n log n) garantat
 * Spațiu auxiliar: O(1) - in-place
 * Stabil: NU
 */
void heap_sort(int arr[], int n) {
    /* Faza 1: Build max-heap în O(n) */
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapify(arr, n, i);
    }
    
    /* Faza 2: Extract-max repetat */
    for (int i = n - 1; i > 0; i--) {
        swap(&arr[0], &arr[i]);  /* Mutăm maximul la final */
        heapify(arr, i, 0);       /* Reparam heap-ul redus */
    }
}

/* =============================================================================
 * PARTEA 7: COUNTING SORT
 * =============================================================================
 */

/**
 * Counting Sort
 * 
 * Complexitate: O(n + k) unde k este intervalul valorilor
 * Spațiu auxiliar: O(n + k)
 * Stabil: DA
 * 
 * @param arr Array-ul de sortat
 * @param n Dimensiunea array-ului
 * @param max_val Valoarea maximă (exclusive)
 */
void counting_sort(int arr[], int n, int max_val) {
    int *count = (int*)calloc(max_val, sizeof(int));
    int *output = (int*)malloc(n * sizeof(int));
    
    if (count == NULL || output == NULL) {
        fprintf(stderr, "Eroare: Nu s-a putut aloca memorie pentru Counting Sort!\n");
        free(count);
        free(output);
        return;
    }
    
    /* Numărăm aparițiile */
    for (int i = 0; i < n; i++) {
        count[arr[i]]++;
    }
    
    /* Calculăm prefix sums */
    for (int i = 1; i < max_val; i++) {
        count[i] += count[i - 1];
    }
    
    /* Construim output-ul (de la dreapta pentru stabilitate) */
    for (int i = n - 1; i >= 0; i--) {
        output[count[arr[i]] - 1] = arr[i];
        count[arr[i]]--;
    }
    
    /* Copiem rezultatul înapoi */
    memcpy(arr, output, n * sizeof(int));
    
    free(count);
    free(output);
}

/* =============================================================================
 * PARTEA 8: RADIX SORT LSD
 * =============================================================================
 */

/**
 * Găsește valoarea maximă din array
 */
int get_max(int arr[], int n) {
    int max = arr[0];
    for (int i = 1; i < n; i++) {
        if (arr[i] > max) max = arr[i];
    }
    return max;
}

/**
 * Counting Sort pe o cifră specifică (pentru Radix Sort)
 * 
 * @param exp 1, 10, 100, ... - poziția cifrei
 */
void counting_sort_digit(int arr[], int n, int exp) {
    int *output = (int*)malloc(n * sizeof(int));
    int count[10] = {0};
    
    if (output == NULL) {
        fprintf(stderr, "Eroare: Nu s-a putut aloca memorie!\n");
        return;
    }
    
    /* Numărăm aparițiile pentru cifra curentă */
    for (int i = 0; i < n; i++) {
        int digit = (arr[i] / exp) % 10;
        count[digit]++;
    }
    
    /* Prefix sums */
    for (int i = 1; i < 10; i++) {
        count[i] += count[i - 1];
    }
    
    /* Construim output-ul (stabil - de la dreapta) */
    for (int i = n - 1; i >= 0; i--) {
        int digit = (arr[i] / exp) % 10;
        output[count[digit] - 1] = arr[i];
        count[digit]--;
    }
    
    memcpy(arr, output, n * sizeof(int));
    free(output);
}

/**
 * Radix Sort LSD (Least Significant Digit)
 * 
 * Complexitate: O(d * (n + k)) unde d = număr de cifre, k = 10
 * Spațiu auxiliar: O(n + k)
 * Stabil: DA
 */
void radix_sort(int arr[], int n) {
    int max = get_max(arr, n);
    
    /* Procesăm fiecare cifră, de la unități la cele mai semnificative */
    for (int exp = 1; max / exp > 0; exp *= 10) {
        counting_sort_digit(arr, n, exp);
    }
}

/* =============================================================================
 * PARTEA 9: INTROSORT (ALGORITM HIBRID)
 * =============================================================================
 */

/**
 * Calculează log₂(n) * 2 pentru limita de adâncime
 */
int depth_limit(int n) {
    return (int)(2 * log2(n));
}

/**
 * Introsort recursiv
 * 
 * Strategie:
 * - Dacă partition-ul este mic (<16), folosește Insertion Sort
 * - Dacă adâncimea depășește 2*log(n), trece la Heap Sort
 * - Altfel, continuă cu Quick Sort
 */
void introsort_recursive(int arr[], int low, int high, int depth) {
    int size = high - low + 1;
    
    if (size <= SMALL_THRESHOLD) {
        insertion_sort(arr, low, high);
        return;
    }
    
    if (depth == 0) {
        /* Heap Sort pentru a garanta O(n log n) */
        /* Facem heap sort pe subarray-ul [low, high] */
        int *sub = arr + low;
        heap_sort(sub, size);
        return;
    }
    
    /* Quick Sort cu median-of-three */
    int pi = partition_median_of_three(arr, low, high);
    introsort_recursive(arr, low, pi - 1, depth - 1);
    introsort_recursive(arr, pi + 1, high, depth - 1);
}

/**
 * Introsort - Introspective Sort
 * 
 * Combină avantajele:
 * - Quick Sort: rapid în medie
 * - Heap Sort: O(n log n) garantat
 * - Insertion Sort: eficient pentru n mic
 * 
 * Complexitate: O(n log n) garantat
 * Utilizat în: std::sort() din C++, biblioteca standard GNU
 */
void introsort(int arr[], int n) {
    int depth = depth_limit(n);
    introsort_recursive(arr, 0, n - 1, depth);
}

/* =============================================================================
 * PARTEA 10: BENCHMARKING
 * =============================================================================
 */

typedef void (*SortFunction)(int[], int);

typedef struct {
    const char *name;
    SortFunction sort;
} SortAlgorithm;

/**
 * Măsoară timpul de execuție al unui algoritm
 */
double benchmark_sort(SortFunction sort, int arr[], int n) {
    clock_t start = clock();
    sort(arr, n);
    clock_t end = clock();
    
    return ((double)(end - start)) / CLOCKS_PER_SEC * 1000.0;
}

/**
 * Wrapper pentru Counting Sort (necesită max_val)
 */
static int g_max_val = 100000;

void counting_sort_wrapper(int arr[], int n) {
    counting_sort(arr, n, g_max_val);
}

/**
 * Rulează benchmark pe toate algoritmii și toate tipurile de date
 */
void run_full_benchmark(int n) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                    BENCHMARK COMPLET - %6d ELEMENTE                         ║\n", n);
    printf("╚═══════════════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    SortAlgorithm algorithms[] = {
        {"Quick Sort (random pivot)", quick_sort},
        {"Merge Sort", merge_sort},
        {"Heap Sort", heap_sort},
        {"Introsort", introsort},
        {"Radix Sort", radix_sort},
        {"Counting Sort", counting_sort_wrapper}
    };
    int num_algs = sizeof(algorithms) / sizeof(algorithms[0]);
    
    const char *data_types[] = {"Random", "Sorted", "Reversed", "Nearly Sorted"};
    void (*generators[])(int[], int) = {
        NULL,  /* Random - handled specially */
        generate_sorted,
        generate_reversed,
        generate_nearly_sorted
    };
    int num_types = 4;
    
    int *original = (int*)malloc(n * sizeof(int));
    int *test = (int*)malloc(n * sizeof(int));
    
    if (original == NULL || test == NULL) {
        fprintf(stderr, "Eroare: Nu s-a putut aloca memorie pentru benchmark!\n");
        free(original);
        free(test);
        return;
    }
    
    /* Header tabel */
    printf("┌──────────────────────────┬─────────────┬─────────────┬─────────────┬─────────────┐\n");
    printf("│ Algoritm                 │   Random    │   Sorted    │  Reversed   │ Nearly Sort │\n");
    printf("├──────────────────────────┼─────────────┼─────────────┼─────────────┼─────────────┤\n");
    
    for (int a = 0; a < num_algs; a++) {
        printf("│ %-24s │", algorithms[a].name);
        
        for (int t = 0; t < num_types; t++) {
            /* Generăm datele */
            if (t == 0) {
                generate_random(original, n, g_max_val);
            } else {
                generators[t](original, n);
            }
            
            /* Copiem pentru test */
            copy_array(test, original, n);
            
            /* Măsurăm timpul */
            double time_ms = benchmark_sort(algorithms[a].sort, test, n);
            
            /* Verificăm corectitudinea */
            if (!is_sorted(test, n)) {
                printf(" %9s   │", "EROARE!");
            } else {
                printf(" %8.2f ms │", time_ms);
            }
        }
        printf("\n");
    }
    
    printf("└──────────────────────────┴─────────────┴─────────────┴─────────────┴─────────────┘\n");
    
    free(original);
    free(test);
}

/* =============================================================================
 * PARTEA 11: DEMONSTRAȚII VIZUALE
 * =============================================================================
 */

/**
 * Demonstrație Quick Sort pas cu pas
 */
void demo_quick_sort_visual(void) {
    print_subsection("QUICK SORT - DEMONSTRAȚIE PAS CU PAS");
    
    int arr[] = {8, 3, 7, 1, 5, 9, 2, 6, 4};
    int n = sizeof(arr) / sizeof(arr[0]);
    
    printf("Array inițial:     ");
    print_array(arr, n, n);
    
    printf("\n=== Partiționare cu pivot = ultimul element ===\n\n");
    
    /* Demonstrăm manual prima partiționare */
    int pivot = arr[n-1];
    printf("Pivot ales: %d (ultimul element)\n\n", pivot);
    
    printf("Proces de partiționare:\n");
    printf("  i = -1 (index pentru elemente <= pivot)\n\n");
    
    int i = -1;
    for (int j = 0; j < n - 1; j++) {
        printf("  j=%d: arr[%d]=%d %s %d? ", j, j, arr[j], arr[j] <= pivot ? "<=" : ">", pivot);
        if (arr[j] <= pivot) {
            i++;
            if (i != j) {
                printf("DA → i=%d, swap(arr[%d]=%d, arr[%d]=%d)\n", i, i, arr[i], j, arr[j]);
                swap(&arr[i], &arr[j]);
                printf("       Array: ");
                print_array(arr, n, n);
            } else {
                printf("DA → i=%d (fără swap, i==j)\n", i);
            }
        } else {
            printf("NU\n");
        }
    }
    
    /* Punem pivotul la locul lui */
    swap(&arr[i + 1], &arr[n - 1]);
    printf("\nFinal: swap(arr[%d], arr[%d]) - punem pivotul la poziția %d\n", i+1, n-1, i+1);
    printf("Array după partiționare: ");
    print_array(arr, n, n);
    
    printf("\nPartiții rezultate:\n");
    printf("  [");
    for (int k = 0; k <= i; k++) printf("%d%s", arr[k], k < i ? ", " : "");
    printf("] < %d < [", arr[i+1]);
    for (int k = i+2; k < n; k++) printf("%d%s", arr[k], k < n-1 ? ", " : "");
    printf("]\n");
}

/**
 * Demonstrație Merge Sort pas cu pas
 */
void demo_merge_sort_visual(void) {
    print_subsection("MERGE SORT - DEMONSTRAȚIE VIZUALĂ");
    
    printf("Exemplu pentru array [38, 27, 43, 3, 9, 82, 10]:\n\n");
    
    printf("FAZA DE DIVIDE:\n");
    printf("                   [38, 27, 43, 3, 9, 82, 10]\n");
    printf("                              │\n");
    printf("                   ┌──────────┴──────────┐\n");
    printf("                   │                     │\n");
    printf("              [38, 27, 43, 3]      [9, 82, 10]\n");
    printf("                   │                     │\n");
    printf("              ┌────┴────┐          ┌─────┴─────┐\n");
    printf("              │         │          │           │\n");
    printf("          [38, 27]  [43, 3]    [9, 82]       [10]\n");
    printf("              │         │          │           │\n");
    printf("           ┌──┴──┐   ┌──┴──┐    ┌──┴──┐        │\n");
    printf("          [38]  [27][43]  [3]  [9]  [82]      [10]\n");
    printf("\n");
    printf("FAZA DE MERGE:\n");
    printf("          [38]  [27][43]  [3]  [9]  [82]      [10]\n");
    printf("           └──┬──┘   └──┬──┘    └──┬──┘        │\n");
    printf("          [27, 38]  [3, 43]    [9, 82]       [10]\n");
    printf("              └────┬────┘          └─────┬─────┘\n");
    printf("           [3, 27, 38, 43]        [9, 10, 82]\n");
    printf("                   └──────────┬──────────┘\n");
    printf("              [3, 9, 10, 27, 38, 43, 82] ✓\n");
}

/**
 * Demonstrație Heap Sort pas cu pas
 */
void demo_heap_sort_visual(void) {
    print_subsection("HEAP SORT - DEMONSTRAȚIE VIZUALĂ");
    
    printf("FAZA 1: BUILD-MAX-HEAP\n");
    printf("══════════════════════\n\n");
    printf("Array inițial: [4, 10, 3, 5, 1]\n\n");
    printf("Ca arbore binar:\n");
    printf("         4\n");
    printf("        / \\\n");
    printf("      10   3\n");
    printf("      / \\\n");
    printf("     5   1\n\n");
    printf("După heapify (de la ultimul nod non-frunză la rădăcină):\n");
    printf("         10            (swap 4↔10)\n");
    printf("        /  \\\n");
    printf("       5    3         (swap 10↔5 după ce 10 a coborât)\n");
    printf("      / \\\n");
    printf("     4   1\n\n");
    
    printf("FAZA 2: EXTRACT-MAX REPETAT\n");
    printf("═══════════════════════════\n\n");
    printf("Pas 1: swap(10, 1) → [1, 5, 3, 4 | 10] → heapify → [5, 4, 3, 1 | 10]\n");
    printf("Pas 2: swap(5, 1)  → [1, 4, 3 | 5, 10]  → heapify → [4, 1, 3 | 5, 10]\n");
    printf("Pas 3: swap(4, 3)  → [3, 1 | 4, 5, 10]  → heapify → [3, 1 | 4, 5, 10]\n");
    printf("Pas 4: swap(3, 1)  → [1 | 3, 4, 5, 10]  → DONE\n\n");
    printf("Rezultat sortat: [1, 3, 4, 5, 10] ✓\n");
}

/**
 * Demonstrație Counting Sort
 */
void demo_counting_sort_visual(void) {
    print_subsection("COUNTING SORT - DEMONSTRAȚIE");
    
    int arr[] = {4, 2, 2, 8, 3, 3, 1};
    int n = sizeof(arr) / sizeof(arr[0]);
    int k = 9;  /* Valori în [0, 8] */
    
    printf("Input: ");
    print_array(arr, n, n);
    printf("Interval valori: [0, %d]\n\n", k-1);
    
    printf("Pas 1: NUMĂRARE\n");
    printf("───────────────\n");
    int count[10] = {0};
    for (int i = 0; i < n; i++) {
        count[arr[i]]++;
    }
    printf("count[0..8] = [");
    for (int i = 0; i < k; i++) {
        printf("%d%s", count[i], i < k-1 ? ", " : "");
    }
    printf("]\n");
    printf("              ");
    for (int i = 0; i < k; i++) {
        printf(" %d ", i);
    }
    printf(" (indexuri)\n\n");
    
    printf("Pas 2: PREFIX SUMS (pentru poziționare)\n");
    printf("───────────────────────────────────────\n");
    for (int i = 1; i < k; i++) {
        count[i] += count[i - 1];
    }
    printf("count[0..8] = [");
    for (int i = 0; i < k; i++) {
        printf("%d%s", count[i], i < k-1 ? ", " : "");
    }
    printf("]\n\n");
    
    printf("Pas 3: PLASARE ÎN OUTPUT\n");
    printf("────────────────────────\n");
    printf("(parcurgem de la dreapta la stânga pentru stabilitate)\n\n");
    
    int output[7];
    /* Reset count pentru demonstrație */
    int count2[] = {0, 1, 3, 5, 6, 6, 6, 6, 7};
    for (int i = n - 1; i >= 0; i--) {
        printf("  arr[%d]=%d → output[count[%d]-1] = output[%d] = %d\n", 
               i, arr[i], arr[i], count2[arr[i]]-1, arr[i]);
        output[count2[arr[i]] - 1] = arr[i];
        count2[arr[i]]--;
    }
    
    printf("\nRezultat: ");
    counting_sort(arr, n, 10);
    print_array(arr, n, n);
}

/**
 * Demonstrație Radix Sort
 */
void demo_radix_sort_visual(void) {
    print_subsection("RADIX SORT LSD - DEMONSTRAȚIE");
    
    printf("Input: [170, 45, 75, 90, 802, 24, 2, 66]\n\n");
    
    printf("Sortare după UNITĂȚI (cifra 1):\n");
    printf("  17[0], 9[0], 80[2], [2], 2[4], 4[5], 7[5], 6[6]\n");
    printf("  → [170, 90, 802, 2, 24, 45, 75, 66]\n\n");
    
    printf("Sortare după ZECI (cifra 2):\n");
    printf("  8[0]2, [0]2, [2]4, [4]5, [6]6, 1[7]0, [7]5, [9]0\n");
    printf("  → [802, 2, 24, 45, 66, 170, 75, 90]\n\n");
    
    printf("Sortare după SUTE (cifra 3):\n");
    printf("  [0]02, [0]24, [0]45, [0]66, [0]75, [0]90, [1]70, [8]02\n");
    printf("  → [2, 24, 45, 66, 75, 90, 170, 802] ✓\n\n");
    
    /* Verificare cu implementarea reală */
    int arr[] = {170, 45, 75, 90, 802, 24, 2, 66};
    radix_sort(arr, 8);
    printf("Verificare implementare: ");
    print_array(arr, 8, 8);
}

/* =============================================================================
 * PARTEA 12: MAIN ȘI MENIU
 * =============================================================================
 */

void print_menu(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                         MENIU                                 ║\n");
    printf("╠═══════════════════════════════════════════════════════════════╣\n");
    printf("║  1. Demo Quick Sort (pas cu pas)                              ║\n");
    printf("║  2. Demo Merge Sort (vizual)                                  ║\n");
    printf("║  3. Demo Heap Sort (vizual)                                   ║\n");
    printf("║  4. Demo Counting Sort                                        ║\n");
    printf("║  5. Demo Radix Sort                                           ║\n");
    printf("║  6. Benchmark pe 1000 elemente                                ║\n");
    printf("║  7. Benchmark pe 10000 elemente                               ║\n");
    printf("║  8. Benchmark pe 50000 elemente                               ║\n");
    printf("║  9. Toate demonstrațiile                                      ║\n");
    printf("║  0. Ieșire                                                    ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\nAlegeți opțiunea: ");
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║     SĂPTĂMÂNA 15: ALGORITMI DE SORTARE AVANSAȚI ȘI ANALIZA COMPLEXITĂȚII      ║\n");
    printf("║     Exemplu Demonstrativ                                                       ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    /* Verificăm argumente pentru mod neinteractiv */
    if (argc > 1) {
        if (strcmp(argv[1], "--benchmark") == 0) {
            run_full_benchmark(10000);
            run_full_benchmark(50000);
            return 0;
        } else if (strcmp(argv[1], "--quick") == 0 || strcmp(argv[1], "--quick-test") == 0) {
            /* Test rapid pentru validare */
            int test[] = {5, 2, 8, 1, 9, 3, 7, 4, 6};
            quick_sort(test, 9);
            if (is_sorted(test, 9)) {
                printf("Quick test PASSED\n");
                return 0;
            } else {
                printf("Quick test FAILED\n");
                return 1;
            }
        }
    }
    
    /* Mod interactiv */
    int choice;
    do {
        print_menu();
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            choice = -1;
        }
        
        switch (choice) {
            case 1:
                demo_quick_sort_visual();
                break;
            case 2:
                demo_merge_sort_visual();
                break;
            case 3:
                demo_heap_sort_visual();
                break;
            case 4:
                demo_counting_sort_visual();
                break;
            case 5:
                demo_radix_sort_visual();
                break;
            case 6:
                run_full_benchmark(1000);
                break;
            case 7:
                run_full_benchmark(10000);
                break;
            case 8:
                run_full_benchmark(50000);
                break;
            case 9:
                demo_quick_sort_visual();
                demo_merge_sort_visual();
                demo_heap_sort_visual();
                demo_counting_sort_visual();
                demo_radix_sort_visual();
                run_full_benchmark(10000);
                break;
            case 0:
                printf("\nLa revedere!\n");
                break;
            default:
                printf("\nOpțiune invalidă!\n");
        }
    } while (choice != 0);
    
    print_separator("TOATE DEMONSTRAȚIILE COMPLETATE CU SUCCES!");
    
    return 0;
}
