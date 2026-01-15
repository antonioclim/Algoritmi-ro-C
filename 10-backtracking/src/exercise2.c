/**
 * =============================================================================
 * EXERCIȚIUL 2: Algoritmul Heap Sort
 * =============================================================================
 *
 * OBIECTIV:
 *   Implementați algoritmul de sortare Heap Sort care sortează un array
 *   în ordine crescătoare folosind un max-heap.
 *
 * CERINȚE:
 *   1. Implementați funcția heapify (sift-down)
 *   2. Implementați funcția build_heap pentru construcție în O(n)
 *   3. Implementați algoritmul heap_sort complet
 *   4. Comparați performanța cu qsort() din biblioteca standard
 *   5. Tratați cazurile speciale (array gol, un element, duplicate)
 *
 * EXEMPLU INPUT:
 *   Array: [64, 34, 25, 12, 22, 11, 90]
 *
 * OUTPUT AȘTEPTAT:
 *   Array sortat: [11, 12, 22, 25, 34, 64, 90]
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -o exercise2 exercise2.c
 * TESTARE: ./exercise2 < ../tests/test2_input.txt
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

/**
 * TODO 1: Completați macro-urile pentru navigarea în heap
 *
 * Indexare de la 0:
 * - parent(i) = (i-1)/2
 * - left_child(i) = 2*i + 1  
 * - right_child(i) = 2*i + 2
 */
#define PARENT(i)      /* YOUR CODE HERE */
#define LEFT_CHILD(i)  /* YOUR CODE HERE */
#define RIGHT_CHILD(i) /* YOUR CODE HERE */

/* =============================================================================
 * FUNCȚII AUXILIARE (FURNIZATE)
 * =============================================================================
 */

/**
 * Swap două valori - furnizată
 */
static void swap(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

/**
 * Afișează array - furnizată
 */
void print_array(const char *label, int arr[], int n) {
    printf("%s: [", label);
    for (int i = 0; i < n; i++) {
        printf("%d", arr[i]);
        if (i < n - 1) printf(", ");
    }
    printf("]\n");
}

/**
 * Verifică dacă array-ul este sortat crescător - furnizată
 */
bool is_sorted(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        if (arr[i] > arr[i + 1]) return false;
    }
    return true;
}

/**
 * Verifică dacă array-ul este max-heap valid - furnizată
 */
bool is_max_heap(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        int left = LEFT_CHILD(i);
        int right = RIGHT_CHILD(i);
        if (left < n && arr[left] > arr[i]) return false;
        if (right < n && arr[right] > arr[i]) return false;
    }
    return true;
}

/**
 * Copiază array - furnizată
 */
void copy_array(int dest[], int src[], int n) {
    memcpy(dest, src, n * sizeof(int));
}

/**
 * Comparator pentru qsort - furnizată
 */
int compare_ints(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

/* =============================================================================
 * FUNCȚII DE IMPLEMENTAT
 * =============================================================================
 */

/**
 * TODO 2: Implementați operația max_heapify (sift-down)
 *
 * @param arr - Array-ul de sortat
 * @param n   - Dimensiunea heap-ului (nu a array-ului complet!)
 * @param i   - Indexul nodului de la care începem
 *
 * Algoritm:
 *   1. Setați largest = i
 *   2. Calculați left = 2*i + 1, right = 2*i + 2
 *   3. Dacă left < n și arr[left] > arr[largest], largest = left
 *   4. Dacă right < n și arr[right] > arr[largest], largest = right
 *   5. Dacă largest != i:
 *      - swap(arr[i], arr[largest])
 *      - Apel recursiv max_heapify(arr, n, largest)
 *
 * IMPORTANT: n reprezintă dimensiunea heap-ului curent, nu a array-ului!
 *            În timpul sortării, n se micșorează treptat.
 */
void max_heapify(int arr[], int n, int i) {
    /* YOUR CODE HERE */
    
}

/**
 * TODO 3: Implementați varianta iterativă a heapify (BONUS)
 *
 * @param arr - Array-ul
 * @param n   - Dimensiunea heap-ului
 * @param i   - Indexul de start
 *
 * Hint: Înlocuiți recursivitatea cu o buclă while
 */
void max_heapify_iterative(int arr[], int n, int i) {
    /* YOUR CODE HERE (BONUS) */
    
}

/**
 * TODO 4: Implementați construcția heap-ului
 *
 * @param arr - Array-ul de transformat în heap
 * @param n   - Dimensiunea array-ului
 *
 * Algoritm (Floyd - bottom-up):
 *   1. Calculați indexul primului nod non-frunză: start = n/2 - 1
 *   2. Pentru i de la start până la 0 (inclusiv), descrescător:
 *      - Apelați max_heapify(arr, n, i)
 *
 * Complexitate: O(n) - NU O(n log n)!
 *
 * De ce funcționează: Frunzele (de la n/2 la n-1) sunt deja heap-uri
 * triviale. Construim heap-ul de jos în sus.
 */
void build_max_heap(int arr[], int n) {
    /* YOUR CODE HERE */
    
}

/**
 * TODO 5: Implementați algoritmul Heap Sort
 *
 * @param arr - Array-ul de sortat (se modifică in-place)
 * @param n   - Dimensiunea array-ului
 *
 * Algoritm:
 *   1. Construiți max-heap cu build_max_heap(arr, n)
 *   2. Pentru i de la n-1 până la 1, descrescător:
 *      a) swap(arr[0], arr[i]) - mută maximul la sfârșit
 *      b) max_heapify(arr, i, 0) - restaurează heap pe primele i elemente
 *
 * Observații:
 *   - După pasul 1, arr[0] conține maximul
 *   - La fiecare iterație, heap-ul se micșorează cu 1
 *   - Elementele de la index i până la n-1 sunt sortate
 */
void heap_sort(int arr[], int n) {
    /* YOUR CODE HERE */
    
}

/**
 * TODO 6: Implementați heap sort descrescător folosind min-heap
 *
 * @param arr - Array-ul de sortat
 * @param n   - Dimensiunea
 *
 * Hint: Modificați comparațiile din heapify pentru a crea un min-heap
 */
void heap_sort_descending(int arr[], int n) {
    /* YOUR CODE HERE (BONUS) */
    
}

/* =============================================================================
 * FUNCȚII DE TESTARE
 * =============================================================================
 */

/**
 * Testează heap sort pe un array dat
 */
void test_heap_sort(int arr[], int n, const char *test_name) {
    printf("\n--- %s ---\n", test_name);
    
    if (n <= 20) {
        print_array("Input", arr, n);
    } else {
        printf("Input: [%d elemente]\n", n);
    }
    
    /* Copie pentru qsort (comparație) */
    int *arr_qsort = malloc(n * sizeof(int));
    copy_array(arr_qsort, arr, n);
    
    /* Măsurare timp heap_sort */
    clock_t start = clock();
    heap_sort(arr, n);
    clock_t end = clock();
    double heap_time = (double)(end - start) / CLOCKS_PER_SEC * 1000;
    
    /* Măsurare timp qsort */
    start = clock();
    qsort(arr_qsort, n, sizeof(int), compare_ints);
    end = clock();
    double qsort_time = (double)(end - start) / CLOCKS_PER_SEC * 1000;
    
    if (n <= 20) {
        print_array("Output heap_sort", arr, n);
        print_array("Output qsort", arr_qsort, n);
    }
    
    /* Verificări */
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

/**
 * Testează construcția heap-ului
 */
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
    
    /* Test 1: Citire de la stdin */
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
    
    /* Copie pentru test build_heap */
    int *arr_copy = malloc(n * sizeof(int));
    copy_array(arr_copy, arr, n);
    
    /* Test build_heap */
    test_build_heap(arr_copy, n);
    free(arr_copy);
    
    /* Test heap_sort */
    test_heap_sort(arr, n, "Test Heap Sort (date utilizator)");
    free(arr);
    
    /* Test 2: Cazuri speciale */
    printf("\n>>> Teste Cazuri Speciale <<<\n");
    
    /* Array cu un singur element */
    int single[] = {42};
    test_heap_sort(single, 1, "Array cu 1 element");
    
    /* Array deja sortat */
    int sorted[] = {1, 2, 3, 4, 5, 6, 7, 8};
    test_heap_sort(sorted, 8, "Array deja sortat");
    
    /* Array sortat invers */
    int reverse[] = {8, 7, 6, 5, 4, 3, 2, 1};
    test_heap_sort(reverse, 8, "Array sortat invers");
    
    /* Array cu duplicate */
    int duplicates[] = {5, 2, 5, 8, 2, 5, 1, 8};
    test_heap_sort(duplicates, 8, "Array cu duplicate");
    
    /* Array cu toate elementele egale */
    int all_same[] = {7, 7, 7, 7, 7, 7, 7};
    test_heap_sort(all_same, 7, "Toate elementele egale");
    
    /* Test 3: Performanță pe array mare */
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

/* =============================================================================
 * PROVOCĂRI BONUS (OPȚIONAL)
 * =============================================================================
 *
 * 1. Implementați heap_sort_iterative fără recursivitate în heapify
 *
 * 2. Implementați d-ary heap sort (heap cu d copii în loc de 2)
 *
 * 3. Analizați când heap sort este mai rapid decât quicksort
 *    (hint: date aproape sortate sau cu multe duplicate)
 *
 * 4. Implementați partial_sort folosind heap (primele k elemente sortate)
 *
 * 5. Demonstrați instabilitatea heap sort cu un contraexemplu
 *
 * =============================================================================
 */
