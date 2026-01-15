/**
 * =============================================================================
 * EXERCIȚIUL 1: Benchmark Comparativ de Algoritmi de Sortare
 * =============================================================================
 *
 * OBIECTIV:
 *   Implementați și comparați empiric performanța algoritmilor Quick Sort,
 *   Merge Sort și Heap Sort pe diferite tipuri de date de intrare.
 *
 * CERINȚE:
 *   1. Implementați Quick Sort cu pivot aleator
 *   2. Implementați Merge Sort (top-down recursiv)
 *   3. Implementați Heap Sort (in-place)
 *   4. Măsurați timpul de execuție și numărul de comparații
 *   5. Generați array-uri de test: random, sortat, inversat, aproape sortat
 *   6. Afișați un raport tabelar cu rezultatele
 *
 * EXEMPLU INPUT:
 *   Dimensiune array: 10000
 *
 * EXPECTED OUTPUT:
 *   Tabel cu timpii de execuție pentru fiecare algoritm și tip de date
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -O2 -o exercise1 exercise1.c -lm
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define DEFAULT_SIZE 10000

/* =============================================================================
 * DEFINIREA TIPURILOR ȘI VARIABILE GLOBALE
 * =============================================================================
 */

/* Counter global pentru numărul de comparații */
static long long g_comparisons = 0;
static long long g_swaps = 0;

typedef struct {
    long long comparisons;
    long long swaps;
    double time_ms;
    bool correct;
} BenchmarkResult;

/* =============================================================================
 * FUNCȚII UTILITARE (FURNIZATE)
 * =============================================================================
 */

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
 * Resetează contorii
 */
void reset_counters(void) {
    g_comparisons = 0;
    g_swaps = 0;
}

/**
 * Verifică dacă array-ul este sortat
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
void generate_random(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % 100000;
    }
}

/**
 * Generează array sortat
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
 * Generează array aproape sortat
 */
void generate_nearly_sorted(int arr[], int n) {
    generate_sorted(arr, n);
    /* Perturbăm 5% din elemente */
    int perturbations = n / 20;
    for (int i = 0; i < perturbations; i++) {
        int idx1 = rand() % n;
        int idx2 = rand() % n;
        int temp = arr[idx1];
        arr[idx1] = arr[idx2];
        arr[idx2] = temp;
    }
}

/* =============================================================================
 * FUNCȚII DE IMPLEMENTAT - QUICK SORT
 * =============================================================================
 */

/**
 * TODO 1: Implementați partiționarea cu pivot aleator
 *
 * @param arr Array-ul de partiționat
 * @param low Index de start
 * @param high Index de final
 * @return Poziția finală a pivotului
 *
 * Pași:
 *   1. Alegeți un pivot aleator din intervalul [low, high]
 *   2. Interschimbați pivotul cu ultimul element
 *   3. Partiționați folosind schema Lomuto:
 *      - i = low - 1 (index pentru elemente <= pivot)
 *      - Pentru j de la low la high-1:
 *        - Dacă arr[j] <= pivot, incrementați i și swap(arr[i], arr[j])
 *   4. Puneți pivotul la poziția corectă: swap(arr[i+1], arr[high])
 *   5. Returnați i+1
 *
 * IMPORTANT: Folosiți funcțiile swap() și compare() pentru statistici!
 */
int partition_random(int arr[], int low, int high) {
    /* YOUR CODE HERE */
    
    /* Alegem pivot aleator și îl mutăm la final */
    int pivot_idx = low + rand() % (high - low + 1);
    swap(&arr[pivot_idx], &arr[high]);
    
    int pivot = arr[high];
    int i = low - 1;
    
    /* Partiționare Lomuto */
    for (int j = low; j < high; j++) {
        if (compare(arr[j], pivot) <= 0) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    
    /* Punem pivotul la locul potrivit */
    swap(&arr[i + 1], &arr[high]);
    
    return i + 1;
}

/**
 * TODO 2: Implementați Quick Sort recursiv
 *
 * @param arr Array-ul de sortat
 * @param low Index de start
 * @param high Index de final
 *
 * Pași:
 *   1. Verificați condiția de terminare: low < high
 *   2. Apelați partition_random pentru a obține poziția pivotului
 *   3. Apelați recursiv quick_sort pe cele două subarray-uri
 */
void quick_sort_recursive(int arr[], int low, int high) {
    /* YOUR CODE HERE */
    
    if (low < high) {
        int pi = partition_random(arr, low, high);
        quick_sort_recursive(arr, low, pi - 1);
        quick_sort_recursive(arr, pi + 1, high);
    }
}

/* Wrapper pentru interfață uniformă */
void quick_sort(int arr[], int n) {
    quick_sort_recursive(arr, 0, n - 1);
}

/* =============================================================================
 * FUNCȚII DE IMPLEMENTAT - MERGE SORT
 * =============================================================================
 */

/**
 * TODO 3: Implementați funcția merge
 *
 * @param arr Array-ul principal
 * @param left Index de start al primei subliste
 * @param mid Index de mijloc
 * @param right Index de final al celei de-a doua subliste
 *
 * Pași:
 *   1. Calculați dimensiunile celor două subliste
 *   2. Alocați array-uri temporare L[] și R[]
 *   3. Copiați datele în array-urile temporare
 *   4. Interclasați cele două array-uri înapoi în arr[]
 *   5. Copiați elementele rămase (dacă există)
 *   6. Eliberați memoria alocată
 *
 * IMPORTANT: Folosiți compare() pentru fiecare comparație!
 */
void merge(int arr[], int left, int mid, int right) {
    /* YOUR CODE HERE */
    
    int n1 = mid - left + 1;
    int n2 = right - mid;
    
    int *L = (int*)malloc(n1 * sizeof(int));
    int *R = (int*)malloc(n2 * sizeof(int));
    
    if (L == NULL || R == NULL) {
        fprintf(stderr, "Eroare: Nu s-a putut aloca memorie!\n");
        free(L);
        free(R);
        return;
    }
    
    /* Copiem datele */
    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];
    
    /* Interclasăm */
    int i = 0, j = 0, k = left;
    
    while (i < n1 && j < n2) {
        if (compare(L[i], R[j]) <= 0) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
    
    /* Copiem restul din L[] */
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }
    
    /* Copiem restul din R[] */
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
    
    free(L);
    free(R);
}

/**
 * TODO 4: Implementați Merge Sort recursiv
 *
 * @param arr Array-ul de sortat
 * @param left Index de start
 * @param right Index de final
 */
void merge_sort_recursive(int arr[], int left, int right) {
    /* YOUR CODE HERE */
    
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
 * FUNCȚII DE IMPLEMENTAT - HEAP SORT
 * =============================================================================
 */

/**
 * TODO 5: Implementați funcția heapify (sift-down)
 *
 * @param arr Array-ul heap
 * @param n Dimensiunea heap-ului
 * @param i Indexul nodului de "reparat"
 *
 * Pași:
 *   1. Calculați indicii copiilor: left = 2*i + 1, right = 2*i + 2
 *   2. Găsiți cel mai mare dintre nod și copii
 *   3. Dacă cel mai mare nu este nodul curent, swap și recursie
 *
 * IMPORTANT: Folosiți compare() pentru comparații și swap() pentru interschimbări!
 */
void heapify(int arr[], int n, int i) {
    /* YOUR CODE HERE */
    
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    
    if (left < n && compare(arr[left], arr[largest]) > 0)
        largest = left;
    
    if (right < n && compare(arr[right], arr[largest]) > 0)
        largest = right;
    
    if (largest != i) {
        swap(&arr[i], &arr[largest]);
        heapify(arr, n, largest);
    }
}

/**
 * TODO 6: Implementați Heap Sort
 *
 * @param arr Array-ul de sortat
 * @param n Dimensiunea array-ului
 *
 * Pași:
 *   1. Build max-heap: heapify de la n/2-1 până la 0
 *   2. Extract-max repetat:
 *      - Swap root cu ultimul element
 *      - Reduce heap size
 *      - Heapify root
 */
void heap_sort(int arr[], int n) {
    /* YOUR CODE HERE */
    
    /* Build max-heap */
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapify(arr, n, i);
    }
    
    /* Extract-max repetat */
    for (int i = n - 1; i > 0; i--) {
        swap(&arr[0], &arr[i]);
        heapify(arr, i, 0);
    }
}

/* =============================================================================
 * FUNCȚII DE BENCHMARK (FURNIZATE)
 * =============================================================================
 */

typedef void (*SortFunction)(int[], int);

/**
 * Execută un benchmark pentru un algoritm și tip de date
 */
BenchmarkResult run_benchmark(SortFunction sort, int original[], int n) {
    BenchmarkResult result;
    
    /* Alocăm și copiem array-ul de test */
    int *test = (int*)malloc(n * sizeof(int));
    if (test == NULL) {
        result.correct = false;
        return result;
    }
    copy_array(test, original, n);
    
    /* Resetăm contorii */
    reset_counters();
    
    /* Măsurăm timpul */
    clock_t start = clock();
    sort(test, n);
    clock_t end = clock();
    
    /* Colectăm rezultatele */
    result.time_ms = ((double)(end - start)) / CLOCKS_PER_SEC * 1000.0;
    result.comparisons = g_comparisons;
    result.swaps = g_swaps;
    result.correct = is_sorted(test, n);
    
    free(test);
    return result;
}

/**
 * Afișează header-ul tabelului
 */
void print_header(void) {
    printf("\n");
    printf("┌────────────────────┬──────────────────────────────────────────────────────────────────┐\n");
    printf("│                    │                      Timp (ms) / Comparații / Swaps             │\n");
    printf("│ Algoritm           ├────────────────┬────────────────┬────────────────┬────────────────┤\n");
    printf("│                    │     Random     │     Sorted     │    Reversed    │ Nearly Sorted  │\n");
    printf("├────────────────────┼────────────────┼────────────────┼────────────────┼────────────────┤\n");
}

/**
 * Afișează o linie din tabel
 */
void print_result_row(const char *name, BenchmarkResult results[4]) {
    printf("│ %-18s │", name);
    for (int i = 0; i < 4; i++) {
        if (results[i].correct) {
            printf(" %7.2f ms    │", results[i].time_ms);
        } else {
            printf("    EROARE     │");
        }
    }
    printf("\n");
    
    /* Linie cu comparații */
    printf("│                    │");
    for (int i = 0; i < 4; i++) {
        if (results[i].correct) {
            printf(" %10lld cmp│", results[i].comparisons);
        } else {
            printf("               │");
        }
    }
    printf("\n");
    
    /* Linie cu swap-uri */
    printf("│                    │");
    for (int i = 0; i < 4; i++) {
        if (results[i].correct) {
            printf(" %10lld swp│", results[i].swaps);
        } else {
            printf("               │");
        }
    }
    printf("\n");
    printf("├────────────────────┼────────────────┼────────────────┼────────────────┼────────────────┤\n");
}

/**
 * Afișează footer-ul tabelului
 */
void print_footer(void) {
    printf("└────────────────────┴────────────────┴────────────────┴────────────────┴────────────────┘\n");
}

/* =============================================================================
 * MAIN
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    srand(time(NULL));
    
    printf("═══════════════════════════════════════════════════════════════════════════════\n");
    printf("           EXERCIȚIUL 1: BENCHMARK COMPARATIV DE SORTARE\n");
    printf("═══════════════════════════════════════════════════════════════════════════════\n");
    
    /* Verificăm argumente pentru test rapid */
    if (argc > 1 && strcmp(argv[1], "--quick-test") == 0) {
        int test[] = {5, 2, 8, 1, 9};
        quick_sort(test, 5);
        printf("Quick test: %s\n", is_sorted(test, 5) ? "PASSED" : "FAILED");
        return is_sorted(test, 5) ? 0 : 1;
    }
    
    int n = DEFAULT_SIZE;
    
    printf("\nDimensiune array: %d elemente\n", n);
    printf("Generare date de test...\n");
    
    /* Alocăm array-urile */
    int *random_arr = (int*)malloc(n * sizeof(int));
    int *sorted_arr = (int*)malloc(n * sizeof(int));
    int *reversed_arr = (int*)malloc(n * sizeof(int));
    int *nearly_arr = (int*)malloc(n * sizeof(int));
    
    if (!random_arr || !sorted_arr || !reversed_arr || !nearly_arr) {
        fprintf(stderr, "Eroare: Nu s-a putut aloca memorie!\n");
        return 1;
    }
    
    /* Generăm datele */
    generate_random(random_arr, n);
    generate_sorted(sorted_arr, n);
    generate_reversed(reversed_arr, n);
    generate_nearly_sorted(nearly_arr, n);
    
    printf("Date generate. Rulare benchmark...\n");
    
    /* Definim algoritmii */
    typedef struct {
        const char *name;
        SortFunction sort;
    } Algorithm;
    
    Algorithm algorithms[] = {
        {"Quick Sort", quick_sort},
        {"Merge Sort", merge_sort},
        {"Heap Sort", heap_sort}
    };
    int num_algorithms = sizeof(algorithms) / sizeof(algorithms[0]);
    
    /* Array-urile de test */
    int *test_arrays[] = {random_arr, sorted_arr, reversed_arr, nearly_arr};
    
    /* Rulăm benchmark */
    print_header();
    
    for (int a = 0; a < num_algorithms; a++) {
        BenchmarkResult results[4];
        
        for (int t = 0; t < 4; t++) {
            results[t] = run_benchmark(algorithms[a].sort, test_arrays[t], n);
        }
        
        print_result_row(algorithms[a].name, results);
    }
    
    /* Înlocuim footer-ul */
    printf("└────────────────────┴────────────────┴────────────────┴────────────────┴────────────────┘\n");
    
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════════════════════\n");
    printf("                          ANALIZĂ REZULTATE\n");
    printf("═══════════════════════════════════════════════════════════════════════════════\n");
    printf("\n");
    printf("OBSERVAȚII:\n");
    printf("  • Quick Sort: Cel mai rapid în medie, dar poate avea probleme pe date sortate\n");
    printf("  • Merge Sort: Performanță consistentă O(n log n) pe toate tipurile de date\n");
    printf("  • Heap Sort:  Garantat O(n log n), dar constanta mai mare în practică\n");
    printf("\n");
    printf("RECOMANDĂRI:\n");
    printf("  • Date aleatore: Quick Sort (cel mai rapid)\n");
    printf("  • Date parțial sortate: Merge Sort sau Insertion Sort hibrid\n");
    printf("  • Memorie limitată: Heap Sort (in-place)\n");
    printf("  • Nevoie de stabilitate: Merge Sort\n");
    printf("\n");
    
    /* Eliberăm memoria */
    free(random_arr);
    free(sorted_arr);
    free(reversed_arr);
    free(nearly_arr);
    
    printf("═══════════════════════════════════════════════════════════════════════════════\n");
    printf("                    BENCHMARK COMPLETAT CU SUCCES!\n");
    printf("═══════════════════════════════════════════════════════════════════════════════\n");
    
    return 0;
}
