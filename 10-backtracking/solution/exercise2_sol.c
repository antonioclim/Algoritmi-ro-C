/**
 * =============================================================================
 * SOLUȚIE EXERCIȚIUL 2: Algoritmul Heap Sort
 * =============================================================================
 *
 * Implementare completă a algoritmului Heap Sort cu toate funcțiile necesare.
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -o exercise2_sol exercise2_sol.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

/* =============================================================================
 * CONSTANTE ȘI MACRO-URI
 * =============================================================================
 */

#define MAX_SIZE 10000

/* SOLUȚIE TODO 1: Macro-uri pentru navigare */
#define PARENT(i)      (((i) - 1) / 2)
#define LEFT_CHILD(i)  ((2 * (i)) + 1)
#define RIGHT_CHILD(i) ((2 * (i)) + 2)

/* =============================================================================
 * FUNCȚII AUXILIARE
 * =============================================================================
 */

static void swap(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void print_array(const char *label, int arr[], int n) {
    printf("%s: [", label);
    for (int i = 0; i < n; i++) {
        printf("%d", arr[i]);
        if (i < n - 1) printf(", ");
    }
    printf("]\n");
}

bool is_sorted(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        if (arr[i] > arr[i + 1]) return false;
    }
    return true;
}

bool is_max_heap(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        int left = LEFT_CHILD(i);
        int right = RIGHT_CHILD(i);
        if (left < n && arr[left] > arr[i]) return false;
        if (right < n && arr[right] > arr[i]) return false;
    }
    return true;
}

void copy_array(int dest[], int src[], int n) {
    memcpy(dest, src, n * sizeof(int));
}

int compare_ints(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

/* =============================================================================
 * SOLUȚII FUNCȚII PRINCIPALE
 * =============================================================================
 */

/* SOLUȚIE TODO 2: Max-Heapify (sift-down) - versiune recursivă */
void max_heapify(int arr[], int n, int i) {
    int largest = i;
    int left = LEFT_CHILD(i);
    int right = RIGHT_CHILD(i);

    /* Compară cu copilul stâng */
    if (left < n && arr[left] > arr[largest]) {
        largest = left;
    }

    /* Compară cu copilul drept */
    if (right < n && arr[right] > arr[largest]) {
        largest = right;
    }

    /* Dacă cel mai mare nu e rădăcina, swap și recursie */
    if (largest != i) {
        swap(&arr[i], &arr[largest]);
        max_heapify(arr, n, largest);
    }
}

/* SOLUȚIE TODO 3: Heapify iterativ */
void max_heapify_iterative(int arr[], int n, int i) {
    while (true) {
        int largest = i;
        int left = LEFT_CHILD(i);
        int right = RIGHT_CHILD(i);

        if (left < n && arr[left] > arr[largest]) {
            largest = left;
        }

        if (right < n && arr[right] > arr[largest]) {
            largest = right;
        }

        if (largest == i) {
            break;  /* Proprietatea heap este satisfăcută */
        }

        swap(&arr[i], &arr[largest]);
        i = largest;  /* Continuă cu subarborele afectat */
    }
}

/* SOLUȚIE TODO 4: Build-Heap (Floyd's algorithm) */
void build_max_heap(int arr[], int n) {
    /* Primul index non-frunză */
    int start_idx = (n / 2) - 1;

    /* Heapify de jos în sus */
    for (int i = start_idx; i >= 0; i--) {
        max_heapify(arr, n, i);
    }
}

/* SOLUȚIE TODO 5: Heap Sort */
void heap_sort(int arr[], int n) {
    /* Pas 1: Construiește max-heap - O(n) */
    build_max_heap(arr, n);

    /* Pas 2: Extrage elemente unul câte unul - O(n log n) */
    for (int i = n - 1; i > 0; i--) {
        /* Mută maximul (rădăcina) la sfârșit */
        swap(&arr[0], &arr[i]);

        /* Heapify pe heap-ul redus */
        max_heapify(arr, i, 0);
    }
}

/* SOLUȚIE TODO 6: Min-heapify pentru sortare descrescătoare */
void min_heapify(int arr[], int n, int i) {
    int smallest = i;
    int left = LEFT_CHILD(i);
    int right = RIGHT_CHILD(i);

    if (left < n && arr[left] < arr[smallest]) {
        smallest = left;
    }

    if (right < n && arr[right] < arr[smallest]) {
        smallest = right;
    }

    if (smallest != i) {
        swap(&arr[i], &arr[smallest]);
        min_heapify(arr, n, smallest);
    }
}

void heap_sort_descending(int arr[], int n) {
    /* Build min-heap */
    for (int i = n / 2 - 1; i >= 0; i--) {
        min_heapify(arr, n, i);
    }

    /* Extract în ordine descrescătoare */
    for (int i = n - 1; i > 0; i--) {
        swap(&arr[0], &arr[i]);
        min_heapify(arr, i, 0);
    }
}

/* =============================================================================
 * FUNCȚII DE TESTARE
 * =============================================================================
 */

void test_heap_sort(int arr[], int n, const char *test_name) {
    printf("\n--- %s ---\n", test_name);
    
    if (n <= 20) {
        print_array("Input", arr, n);
    } else {
        printf("Input: [%d elemente]\n", n);
    }
    
    int *arr_qsort = malloc(n * sizeof(int));
    copy_array(arr_qsort, arr, n);
    
    clock_t start = clock();
    heap_sort(arr, n);
    clock_t end = clock();
    double heap_time = (double)(end - start) / CLOCKS_PER_SEC * 1000;
    
    start = clock();
    qsort(arr_qsort, n, sizeof(int), compare_ints);
    end = clock();
    double qsort_time = (double)(end - start) / CLOCKS_PER_SEC * 1000;
    
    if (n <= 20) {
        print_array("Output heap_sort", arr, n);
        print_array("Output qsort", arr_qsort, n);
    }
    
    bool sorted = is_sorted(arr, n);
    bool matches = (memcmp(arr, arr_qsort, n * sizeof(int)) == 0);
    
    printf("Verificări:\n");
    printf("  - Este sortat: %s\n", sorted ? "✓ DA" : "✗ NU");
    printf("  - Rezultat identic cu qsort: %s\n", matches ? "✓ DA" : "✗ NU");
    printf("Performanță:\n");
    printf("  - heap_sort: %.3f ms\n", heap_time);
    printf("  - qsort:     %.3f ms\n", qsort_time);
    
    free(arr_qsort);
}

void test_build_heap(int arr[], int n) {
    printf("\n--- Test Build Heap ---\n");
    print_array("Înainte", arr, n);
    
    build_max_heap(arr, n);
    
    print_array("După build_max_heap", arr, n);
    printf("Este max-heap valid: %s\n", is_max_heap(arr, n) ? "✓ DA" : "✗ NU");
}

/* =============================================================================
 * PROGRAMUL PRINCIPAL
 * =============================================================================
 */

int main(void) {
    printf("=== EXERCIȚIUL 2: Algoritmul Heap Sort ===\n");
    
    printf("\n>>> Citire date de la stdin <<<\n");
    printf("Introduceți numărul de elemente: ");
    
    int n;
    if (scanf("%d", &n) != 1 || n <= 0 || n > MAX_SIZE) {
        fprintf(stderr, "Eroare: Număr invalid (1-%d)\n", MAX_SIZE);
        return EXIT_FAILURE;
    }
    
    int *arr = malloc(n * sizeof(int));
    if (!arr) {
        fprintf(stderr, "Eroare: Alocare memorie eșuată!\n");
        return EXIT_FAILURE;
    }
    
    printf("Introduceți %d elemente:\n", n);
    for (int i = 0; i < n; i++) {
        if (scanf("%d", &arr[i]) != 1) {
            fprintf(stderr, "Eroare la citire!\n");
            free(arr);
            return EXIT_FAILURE;
        }
    }
    
    int *arr_copy = malloc(n * sizeof(int));
    copy_array(arr_copy, arr, n);
    
    test_build_heap(arr_copy, n);
    free(arr_copy);
    
    test_heap_sort(arr, n, "Test Heap Sort (date utilizator)");
    free(arr);
    
    printf("\n>>> Teste Cazuri Speciale <<<\n");
    
    int single[] = {42};
    test_heap_sort(single, 1, "Array cu 1 element");
    
    int sorted[] = {1, 2, 3, 4, 5, 6, 7, 8};
    test_heap_sort(sorted, 8, "Array deja sortat");
    
    int reverse[] = {8, 7, 6, 5, 4, 3, 2, 1};
    test_heap_sort(reverse, 8, "Array sortat invers");
    
    int duplicates[] = {5, 2, 5, 8, 2, 5, 1, 8};
    test_heap_sort(duplicates, 8, "Array cu duplicate");
    
    int all_same[] = {7, 7, 7, 7, 7, 7, 7};
    test_heap_sort(all_same, 7, "Toate elementele egale");
    
    printf("\n>>> Test Performanță <<<\n");
    int large_n = 5000;
    int *large = malloc(large_n * sizeof(int));
    srand(42);
    for (int i = 0; i < large_n; i++) {
        large[i] = rand() % 100000;
    }
    test_heap_sort(large, large_n, "Array aleator 5000 elemente");
    free(large);
    
    printf("\n=== Toate testele finalizate! ===\n");
    
    return EXIT_SUCCESS;
}
