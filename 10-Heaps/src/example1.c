/**
 * =============================================================================
 * SĂPTĂMÂNA 10: HEAP-URI ȘI COZI CU PRIORITĂȚI
 * Exemplu Complet Funcțional
 * =============================================================================
 *
 * Acest exemplu demonstrează:
 *   1. Reprezentarea heap-ului în array și navigarea
 *   2. Operația heapify (sift-down) pentru max-heap și min-heap
 *   3. Construcția heap-ului în O(n) (algoritmul Floyd)
 *   4. Priority Queue cu toate operațiile
 *   5. Algoritmul Heap Sort
 *   6. Aplicație practică: Task Scheduler
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -o example1 example1.c
 * Utilizare: ./example1
 *
 * Autor: Curs ATP - ASE București
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>

/* =============================================================================
 * CONFIGURAȚIE ȘI MACRO-URI
 * =============================================================================
 */

#define INITIAL_CAPACITY 16
#define GROWTH_FACTOR 2

/* Macro-uri pentru navigarea în heap (indexare de la 0) */
#define PARENT(i)      (((i) - 1) / 2)
#define LEFT_CHILD(i)  ((2 * (i)) + 1)
#define RIGHT_CHILD(i) ((2 * (i)) + 2)

/* Macro pentru verificare dacă un nod are copii */
#define HAS_LEFT(i, n)  (LEFT_CHILD(i) < (n))
#define HAS_RIGHT(i, n) (RIGHT_CHILD(i) < (n))

/* Macro pentru swap */
#define SWAP(a, b) do { \
    int _tmp = (a); \
    (a) = (b); \
    (b) = _tmp; \
} while(0)

/* =============================================================================
 * PARTEA 1: FUNCȚII UTILITARE
 * =============================================================================
 */

/**
 * Afișează un array în format vizual
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
 * Afișează heap-ul ca arbore (reprezentare textuală)
 */
void print_heap_tree(int arr[], int n) {
    if (n == 0) {
        printf("  (heap gol)\n");
        return;
    }
    
    printf("\n  Reprezentare arbore (nivel cu nivel):\n");
    int level = 0;
    int level_size = 1;
    int printed = 0;
    
    while (printed < n) {
        printf("  Nivel %d: ", level);
        for (int i = 0; i < level_size && printed < n; i++) {
            printf("%d ", arr[printed]);
            printed++;
        }
        printf("\n");
        level++;
        level_size *= 2;
    }
}

/**
 * Verifică dacă un array este max-heap valid
 */
bool is_max_heap(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        if (HAS_LEFT(i, n) && arr[LEFT_CHILD(i)] > arr[i])
            return false;
        if (HAS_RIGHT(i, n) && arr[RIGHT_CHILD(i)] > arr[i])
            return false;
    }
    return true;
}

/* =============================================================================
 * PARTEA 2: OPERAȚII FUNDAMENTALE PE HEAP
 * =============================================================================
 */

void demo_part1_navigation(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 1: Navigare în Heap                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    int heap[] = {90, 85, 80, 70, 60, 50, 30};
    int n = 7;
    
    print_array("Heap", heap, n);
    print_heap_tree(heap, n);
    
    printf("\n  Demonstrație navigare:\n");
    for (int i = 0; i < n; i++) {
        printf("  Index %d (valoare %d):\n", i, heap[i]);
        if (i > 0) {
            printf("    - Părinte: index %d (valoare %d)\n", 
                   PARENT(i), heap[PARENT(i)]);
        } else {
            printf("    - Părinte: (este rădăcina)\n");
        }
        if (HAS_LEFT(i, n)) {
            printf("    - Copil stâng: index %d (valoare %d)\n", 
                   LEFT_CHILD(i), heap[LEFT_CHILD(i)]);
        } else {
            printf("    - Copil stâng: (nu există)\n");
        }
        if (HAS_RIGHT(i, n)) {
            printf("    - Copil drept: index %d (valoare %d)\n", 
                   RIGHT_CHILD(i), heap[RIGHT_CHILD(i)]);
        } else {
            printf("    - Copil drept: (nu există)\n");
        }
        printf("\n");
    }
}

/* =============================================================================
 * PARTEA 3: HEAPIFY (SIFT-DOWN)
 * =============================================================================
 */

/**
 * Max-Heapify: Restaurează proprietatea max-heap pentru subarbore
 * 
 * @param arr   Array-ul heap
 * @param n     Dimensiunea heap-ului
 * @param i     Indexul rădăcinii subarborelui
 */
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

    /* Dacă cel mai mare nu este rădăcina, swap și recursie */
    if (largest != i) {
        SWAP(arr[i], arr[largest]);
        max_heapify(arr, n, largest);
    }
}

/**
 * Min-Heapify: Pentru min-heap (bonus demonstrativ)
 */
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
        SWAP(arr[i], arr[smallest]);
        min_heapify(arr, n, smallest);
    }
}

void demo_part2_heapify(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 2: Operația Heapify                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    /* Array cu încălcare la rădăcină */
    int arr[] = {20, 90, 80, 70, 60, 50, 30};
    int n = 7;
    
    printf("  Situație: Rădăcina (20) încalcă proprietatea max-heap\n\n");
    print_array("  Înainte", arr, n);
    print_heap_tree(arr, n);
    printf("  Este max-heap valid? %s\n\n", is_max_heap(arr, n) ? "DA" : "NU");
    
    printf("  Aplicăm max_heapify(arr, %d, 0)...\n\n", n);
    max_heapify(arr, n, 0);
    
    print_array("  După", arr, n);
    print_heap_tree(arr, n);
    printf("  Este max-heap valid? %s\n", is_max_heap(arr, n) ? "DA" : "NU");
}

/* =============================================================================
 * PARTEA 4: BUILD-HEAP (ALGORITMUL FLOYD)
 * =============================================================================
 */

/**
 * Construiește max-heap din array arbitrar în O(n)
 */
void build_max_heap(int arr[], int n) {
    /* Primul index non-frunză */
    int start_idx = (n / 2) - 1;

    /* Heapify de jos în sus */
    for (int i = start_idx; i >= 0; i--) {
        max_heapify(arr, n, i);
    }
}

void demo_part3_build_heap(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 3: Construcția Heap-ului (Build-Heap)             ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    int arr[] = {4, 10, 3, 5, 1, 8, 7, 2, 9, 6};
    int n = 10;
    
    printf("  Transformăm un array arbitrar în max-heap:\n\n");
    print_array("  Array inițial", arr, n);
    printf("  Este max-heap? %s\n\n", is_max_heap(arr, n) ? "DA" : "NU");
    
    printf("  Primul nod non-frunză: index %d (valoare %d)\n", 
           (n/2)-1, arr[(n/2)-1]);
    printf("  Vom aplica heapify de la index %d până la 0...\n\n", (n/2)-1);
    
    build_max_heap(arr, n);
    
    print_array("  După build_max_heap", arr, n);
    print_heap_tree(arr, n);
    printf("  Este max-heap? %s\n", is_max_heap(arr, n) ? "DA" : "NU");
    printf("\n  Complexitate: O(n) - nu O(n log n)!\n");
}

/* =============================================================================
 * PARTEA 5: PRIORITY QUEUE
 * =============================================================================
 */

typedef struct {
    int *data;
    int size;
    int capacity;
} PriorityQueue;

/**
 * Creează o priority queue nouă
 */
PriorityQueue* pq_create(int initial_capacity) {
    PriorityQueue *pq = malloc(sizeof(PriorityQueue));
    if (!pq) return NULL;
    
    pq->data = malloc(initial_capacity * sizeof(int));
    if (!pq->data) {
        free(pq);
        return NULL;
    }
    
    pq->size = 0;
    pq->capacity = initial_capacity;
    return pq;
}

/**
 * Eliberează memoria
 */
void pq_destroy(PriorityQueue *pq) {
    if (pq) {
        free(pq->data);
        free(pq);
    }
}

/**
 * Verifică dacă e goală
 */
bool pq_is_empty(PriorityQueue *pq) {
    return pq->size == 0;
}

/**
 * Returnează maximul fără a-l scoate
 */
int pq_peek(PriorityQueue *pq) {
    if (pq_is_empty(pq)) {
        fprintf(stderr, "Eroare: Priority queue goală!\n");
        exit(EXIT_FAILURE);
    }
    return pq->data[0];
}

/**
 * Redimensionează array-ul intern
 */
static void pq_resize(PriorityQueue *pq, int new_capacity) {
    int *new_data = realloc(pq->data, new_capacity * sizeof(int));
    if (new_data) {
        pq->data = new_data;
        pq->capacity = new_capacity;
    }
}

/**
 * Inserare cu sift-up
 */
void pq_insert(PriorityQueue *pq, int key) {
    /* Verifică și redimensionează dacă e nevoie */
    if (pq->size >= pq->capacity) {
        pq_resize(pq, pq->capacity * GROWTH_FACTOR);
    }

    /* Adaugă la sfârșitul heap-ului */
    int i = pq->size;
    pq->data[i] = key;
    pq->size++;

    /* Sift-up: urcă până la poziția corectă */
    while (i > 0 && pq->data[PARENT(i)] < pq->data[i]) {
        SWAP(pq->data[i], pq->data[PARENT(i)]);
        i = PARENT(i);
    }
}

/**
 * Extrage maximul cu heapify
 */
int pq_extract_max(PriorityQueue *pq) {
    if (pq_is_empty(pq)) {
        fprintf(stderr, "Eroare: Priority queue goală!\n");
        exit(EXIT_FAILURE);
    }

    int max = pq->data[0];

    /* Mută ultimul element la rădăcină */
    pq->data[0] = pq->data[pq->size - 1];
    pq->size--;

    /* Restaurează proprietatea heap */
    if (pq->size > 0) {
        max_heapify(pq->data, pq->size, 0);
    }

    return max;
}

void demo_part4_priority_queue(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 4: Priority Queue                                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    PriorityQueue *pq = pq_create(INITIAL_CAPACITY);
    
    printf("  Inserăm elemente: 30, 10, 50, 20, 40\n\n");
    
    int values[] = {30, 10, 50, 20, 40};
    int n = 5;
    
    for (int i = 0; i < n; i++) {
        printf("  pq_insert(%d)...\n", values[i]);
        pq_insert(pq, values[i]);
        print_array("    Heap", pq->data, pq->size);
    }
    
    printf("\n  Maximum curent (peek): %d\n\n", pq_peek(pq));
    
    printf("  Extragem toate elementele în ordine descrescătoare:\n");
    while (!pq_is_empty(pq)) {
        int max = pq_extract_max(pq);
        printf("    Extras: %d\n", max);
    }
    
    pq_destroy(pq);
    printf("\n  Memorie eliberată corect.\n");
}

/* =============================================================================
 * PARTEA 5: HEAP SORT
 * =============================================================================
 */

/**
 * Heap Sort - sortare in-place cu complexitate O(n log n)
 */
void heap_sort(int arr[], int n) {
    /* Pas 1: Construiește max-heap */
    build_max_heap(arr, n);

    /* Pas 2: Extrage elemente unul câte unul */
    for (int i = n - 1; i > 0; i--) {
        /* Mută maximul curent la sfârșit */
        SWAP(arr[0], arr[i]);

        /* Heapify pe heap-ul redus */
        max_heapify(arr, i, 0);
    }
}

void demo_part5_heap_sort(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 5: Heap Sort                                      ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    int arr[] = {64, 34, 25, 12, 22, 11, 90, 5};
    int n = 8;
    
    print_array("  Array inițial", arr, n);
    
    printf("\n  Aplicăm heap_sort...\n\n");
    
    printf("  Pas 1: Build max-heap\n");
    build_max_heap(arr, n);
    print_array("    După build_heap", arr, n);
    
    printf("\n  Pas 2: Extracții repetate\n");
    for (int i = n - 1; i > 0; i--) {
        SWAP(arr[0], arr[i]);
        max_heapify(arr, i, 0);
        printf("    Iterația %d: ", n - i);
        print_array("", arr, n);
    }
    
    printf("\n");
    print_array("  Array sortat", arr, n);
    printf("\n  Complexitate: O(n log n), Spațiu: O(1) - in-place!\n");
}

/* =============================================================================
 * PARTEA 6: APLICAȚIE - TASK SCHEDULER
 * =============================================================================
 */

typedef struct {
    char name[50];
    int priority;
} Task;

typedef struct {
    Task *tasks;
    int size;
    int capacity;
} TaskScheduler;

TaskScheduler* scheduler_create(int capacity) {
    TaskScheduler *ts = malloc(sizeof(TaskScheduler));
    ts->tasks = malloc(capacity * sizeof(Task));
    ts->size = 0;
    ts->capacity = capacity;
    return ts;
}

void scheduler_destroy(TaskScheduler *ts) {
    free(ts->tasks);
    free(ts);
}

void task_heapify(TaskScheduler *ts, int n, int i) {
    int largest = i;
    int left = LEFT_CHILD(i);
    int right = RIGHT_CHILD(i);

    if (left < n && ts->tasks[left].priority > ts->tasks[largest].priority)
        largest = left;
    if (right < n && ts->tasks[right].priority > ts->tasks[largest].priority)
        largest = right;

    if (largest != i) {
        Task tmp = ts->tasks[i];
        ts->tasks[i] = ts->tasks[largest];
        ts->tasks[largest] = tmp;
        task_heapify(ts, n, largest);
    }
}

void scheduler_add(TaskScheduler *ts, const char *name, int priority) {
    if (ts->size >= ts->capacity) return;
    
    int i = ts->size;
    strcpy(ts->tasks[i].name, name);
    ts->tasks[i].priority = priority;
    ts->size++;

    while (i > 0 && ts->tasks[PARENT(i)].priority < ts->tasks[i].priority) {
        Task tmp = ts->tasks[i];
        ts->tasks[i] = ts->tasks[PARENT(i)];
        ts->tasks[PARENT(i)] = tmp;
        i = PARENT(i);
    }
}

Task scheduler_get_next(TaskScheduler *ts) {
    Task highest = ts->tasks[0];
    ts->tasks[0] = ts->tasks[ts->size - 1];
    ts->size--;
    if (ts->size > 0) {
        task_heapify(ts, ts->size, 0);
    }
    return highest;
}

void demo_part6_application(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 6: Aplicație - Task Scheduler                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    TaskScheduler *ts = scheduler_create(10);
    
    printf("  Adăugăm task-uri cu priorități diferite:\n\n");
    
    scheduler_add(ts, "Verifică email", 2);
    printf("    + 'Verifică email' (prioritate: 2)\n");
    
    scheduler_add(ts, "Repară bug critic", 10);
    printf("    + 'Repară bug critic' (prioritate: 10)\n");
    
    scheduler_add(ts, "Întâlnire echipă", 5);
    printf("    + 'Întâlnire echipă' (prioritate: 5)\n");
    
    scheduler_add(ts, "Documentație", 3);
    printf("    + 'Documentație' (prioritate: 3)\n");
    
    scheduler_add(ts, "Deploy producție", 9);
    printf("    + 'Deploy producție' (prioritate: 9)\n");
    
    printf("\n  Procesăm task-urile în ordinea priorității:\n\n");
    
    int order = 1;
    while (ts->size > 0) {
        Task t = scheduler_get_next(ts);
        printf("    %d. [Prioritate %2d] %s\n", order++, t.priority, t.name);
    }
    
    scheduler_destroy(ts);
    printf("\n  Toate task-urile procesate. Memorie eliberată.\n");
}

/* =============================================================================
 * PROGRAMUL PRINCIPAL
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     SĂPTĂMÂNA 10: HEAP-URI ȘI COZI CU PRIORITĂȚI              ║\n");
    printf("║                    Exemplu Complet                            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");

    demo_part1_navigation();
    demo_part2_heapify();
    demo_part3_build_heap();
    demo_part4_priority_queue();
    demo_part5_heap_sort();
    demo_part6_application();

    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    Demonstrație Completă!                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");

    return 0;
}

/* =============================================================================
 * SFÂRȘITUL FIȘIERULUI
 * =============================================================================
 */
