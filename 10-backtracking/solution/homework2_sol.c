/**
 * =============================================================================
 * SOLUȚIE TEMA 2: Algoritmul Kth Largest cu Heap
 * =============================================================================
 *
 * Structură de date care menține eficient cele mai mari k elemente
 * dintr-un stream de numere folosind un min-heap de dimensiune k.
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -o homework2_sol homework2_sol.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

/* =============================================================================
 * MACRO-URI PENTRU MIN-HEAP
 * =============================================================================
 */

#define PARENT(i)      (((i) - 1) / 2)
#define LEFT_CHILD(i)  ((2 * (i)) + 1)
#define RIGHT_CHILD(i) ((2 * (i)) + 2)

/* =============================================================================
 * STRUCTURA KTH LARGEST
 * =============================================================================
 */

typedef struct {
    int *heap;    /* Min-heap de dimensiune k */
    int k;        /* Numărul de elemente de păstrat */
    int size;     /* Dimensiunea curentă (≤ k) */
} KthLargest;

/* =============================================================================
 * FUNCȚII MIN-HEAP
 * =============================================================================
 */

/**
 * Swap două valori
 */
static void swap(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

/**
 * Min-heapify (sift-down)
 */
static void min_heapify(int arr[], int n, int i) {
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

/**
 * Sift-up pentru min-heap
 */
static void min_sift_up(int arr[], int i) {
    while (i > 0 && arr[PARENT(i)] > arr[i]) {
        swap(&arr[i], &arr[PARENT(i)]);
        i = PARENT(i);
    }
}

/**
 * Inserare în min-heap
 */
static void min_heap_insert(int arr[], int *size, int val) {
    arr[*size] = val;
    (*size)++;
    min_sift_up(arr, *size - 1);
}

/**
 * Înlocuiește rădăcina și restaurează heap
 */
static void min_heap_replace_root(int arr[], int size, int val) {
    arr[0] = val;
    min_heapify(arr, size, 0);
}

/* =============================================================================
 * FUNCȚII KTH LARGEST
 * =============================================================================
 */

/**
 * Creează structura KthLargest și o inițializează cu un array
 *
 * @param k    - Păstrăm cele mai mari k elemente
 * @param nums - Array inițial de numere
 * @param n    - Dimensiunea array-ului
 * @return     - Pointer către structura creată
 */
KthLargest* kth_create(int k, int *nums, int n) {
    KthLargest *kl = malloc(sizeof(KthLargest));
    if (!kl) return NULL;
    
    kl->heap = malloc(k * sizeof(int));
    if (!kl->heap) {
        free(kl);
        return NULL;
    }
    
    kl->k = k;
    kl->size = 0;
    
    /* Inserăm elementele inițiale */
    for (int i = 0; i < n; i++) {
        if (kl->size < k) {
            /* Heap-ul nu e plin, inserăm direct */
            min_heap_insert(kl->heap, &kl->size, nums[i]);
        } else if (nums[i] > kl->heap[0]) {
            /* nums[i] e mai mare decât minimul, înlocuim */
            min_heap_replace_root(kl->heap, kl->size, nums[i]);
        }
        /* Altfel, nums[i] nu intră în top k */
    }
    
    return kl;
}

/**
 * Eliberează memoria
 */
void kth_destroy(KthLargest *kl) {
    if (kl) {
        free(kl->heap);
        free(kl);
    }
}

/**
 * Adaugă un element și returnează al k-lea cel mai mare
 *
 * @param kl  - Pointer către structura KthLargest
 * @param val - Valoarea de adăugat
 * @return    - Al k-lea cel mai mare element
 */
int kth_add(KthLargest *kl, int val) {
    if (!kl) return INT_MIN;
    
    if (kl->size < kl->k) {
        /* Heap-ul nu e plin, inserăm */
        min_heap_insert(kl->heap, &kl->size, val);
    } else if (val > kl->heap[0]) {
        /* val e mai mare decât al k-lea, înlocuim minimul */
        min_heap_replace_root(kl->heap, kl->size, val);
    }
    /* Altfel, val nu afectează top k */
    
    /* Al k-lea cel mai mare este minimul din heap (rădăcina) */
    return kl->heap[0];
}

/**
 * Returnează al k-lea cel mai mare fără a modifica structura
 */
int kth_get_kth_largest(KthLargest *kl) {
    if (!kl || kl->size == 0) {
        fprintf(stderr, "Eroare: Structura este goală!\n");
        return INT_MIN;
    }
    
    if (kl->size < kl->k) {
        fprintf(stderr, "Atenție: Nu avem încă k elemente!\n");
    }
    
    return kl->heap[0];
}

/**
 * Afișează conținutul heap-ului
 */
void kth_print(KthLargest *kl) {
    if (!kl || kl->size == 0) {
        printf("[heap gol]\n");
        return;
    }
    
    printf("Min-heap (k=%d, size=%d): [", kl->k, kl->size);
    for (int i = 0; i < kl->size; i++) {
        printf("%d", kl->heap[i]);
        if (i < kl->size - 1) printf(", ");
    }
    printf("]\n");
}

/* =============================================================================
 * FUNCȚIE DE VERIFICARE (SORTARE NAIVĂ)
 * =============================================================================
 */

int compare_desc(const void *a, const void *b) {
    return (*(int*)b - *(int*)a);
}

int naive_kth_largest(int *nums, int n, int k) {
    if (n < k) return INT_MIN;
    
    int *sorted = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) sorted[i] = nums[i];
    
    qsort(sorted, n, sizeof(int), compare_desc);
    
    int result = sorted[k - 1];
    free(sorted);
    return result;
}

/* =============================================================================
 * PROGRAMUL PRINCIPAL
 * =============================================================================
 */

int main(void) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║        TEMA 2: Algoritmul Kth Largest cu Heap                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Test 1: Exemplu din cerințe */
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("TEST 1: Exemplu din cerințe\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    int k = 3;
    int initial[] = {4, 5, 8, 2};
    int n = 4;
    
    printf("k = %d\n", k);
    printf("Array inițial: [4, 5, 8, 2]\n\n");
    
    KthLargest *kl = kth_create(k, initial, n);
    if (!kl) {
        fprintf(stderr, "Eroare la creare!\n");
        return EXIT_FAILURE;
    }
    
    printf("După inițializare: ");
    kth_print(kl);
    printf("Al %d-lea cel mai mare: %d\n\n", k, kth_get_kth_largest(kl));
    
    int adds[] = {3, 5, 10, 9, 4};
    int num_adds = 5;
    
    for (int i = 0; i < num_adds; i++) {
        int result = kth_add(kl, adds[i]);
        printf("Adaug %2d: al %d-lea = %d | ", adds[i], k, result);
        kth_print(kl);
    }
    
    kth_destroy(kl);
    
    /* Test 2: Verificare cu sortare naivă */
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("TEST 2: Verificare corectitudine\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    int test_data[] = {1, 9, 2, 8, 3, 7, 4, 6, 5, 10};
    int test_n = 10;
    int test_k = 4;
    
    printf("Date test: [1, 9, 2, 8, 3, 7, 4, 6, 5, 10]\n");
    printf("k = %d\n\n", test_k);
    
    /* Metoda heap */
    KthLargest *kl2 = kth_create(test_k, test_data, test_n);
    int heap_result = kth_get_kth_largest(kl2);
    kth_destroy(kl2);
    
    /* Metoda naivă */
    int naive_result = naive_kth_largest(test_data, test_n, test_k);
    
    printf("Rezultat heap:  %d\n", heap_result);
    printf("Rezultat naiv:  %d\n", naive_result);
    printf("Verificare:     %s\n", 
           (heap_result == naive_result) ? "✓ CORECT" : "✗ INCORECT");
    
    /* Test 3: Stream dinamic */
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("TEST 3: Stream dinamic de numere\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    printf("Citire interactivă. Introduceți k: ");
    int user_k;
    if (scanf("%d", &user_k) != 1 || user_k <= 0) {
        printf("Valoare invalidă, folosim k=3\n");
        user_k = 3;
    }
    
    printf("Introduceți numărul de elemente inițiale: ");
    int user_n;
    if (scanf("%d", &user_n) != 1 || user_n < 0) {
        printf("Valoare invalidă, folosim 0\n");
        user_n = 0;
    }
    
    int *user_initial = NULL;
    if (user_n > 0) {
        user_initial = malloc(user_n * sizeof(int));
        printf("Introduceți %d elemente: ", user_n);
        for (int i = 0; i < user_n; i++) {
            if (scanf("%d", &user_initial[i]) != 1) {
                user_initial[i] = 0;
            }
        }
    }
    
    KthLargest *kl3 = kth_create(user_k, user_initial, user_n);
    free(user_initial);
    
    if (!kl3) {
        fprintf(stderr, "Eroare la creare!\n");
        return EXIT_FAILURE;
    }
    
    printf("\nStructură inițializată. ");
    kth_print(kl3);
    
    printf("\nIntroduceți numărul de adăugări: ");
    int num_ops;
    if (scanf("%d", &num_ops) != 1) num_ops = 0;
    
    printf("Introduceți %d numere:\n", num_ops);
    for (int i = 0; i < num_ops; i++) {
        int val;
        if (scanf("%d", &val) != 1) break;
        int result = kth_add(kl3, val);
        printf("  add(%d) = %d | ", val, result);
        kth_print(kl3);
    }
    
    kth_destroy(kl3);
    
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("Toate testele finalizate!\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    
    return EXIT_SUCCESS;
}
