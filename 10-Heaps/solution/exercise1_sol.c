/**
 * =============================================================================
 * SOLUȚIE EXERCIȚIUL 1: Priority Queue cu Max-Heap
 * =============================================================================
 *
 * Această soluție demonstrează implementarea completă a unei priority queue
 * folosind un max-heap reprezentat în array.
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -o exercise1_sol exercise1_sol.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* =============================================================================
 * CONSTANTE ȘI MACRO-URI
 * =============================================================================
 */

#define INITIAL_CAPACITY 8
#define GROWTH_FACTOR 2

/* SOLUȚIE TODO 1: Macro-uri pentru navigarea în heap */
#define PARENT(i)      (((i) - 1) / 2)
#define LEFT_CHILD(i)  ((2 * (i)) + 1)
#define RIGHT_CHILD(i) ((2 * (i)) + 2)

/* =============================================================================
 * DEFINIȚIA STRUCTURII
 * =============================================================================
 */

/* SOLUȚIE TODO 2: Structura PriorityQueue */
typedef struct {
    int *data;      /* Pointer către array-ul de date */
    int size;       /* Numărul curent de elemente */
    int capacity;   /* Capacitatea alocată */
} PriorityQueue;

/* =============================================================================
 * FUNCȚII AUXILIARE
 * =============================================================================
 */

static void swap(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void pq_print(PriorityQueue *pq) {
    if (pq == NULL || pq->size == 0) {
        printf("[heap gol]\n");
        return;
    }
    printf("[");
    for (int i = 0; i < pq->size; i++) {
        printf("%d", pq->data[i]);
        if (i < pq->size - 1) printf(", ");
    }
    printf("]\n");
}

/* =============================================================================
 * FUNCȚII DE GESTIONARE A MEMORIEI
 * =============================================================================
 */

/* SOLUȚIE TODO 3: Funcția de creare */
PriorityQueue* pq_create(int initial_capacity) {
    /* Pas 1: Alocă structura */
    PriorityQueue *pq = malloc(sizeof(PriorityQueue));
    if (pq == NULL) {
        return NULL;
    }
    
    /* Pas 2: Folosește capacitatea implicită dacă e 0 */
    if (initial_capacity <= 0) {
        initial_capacity = INITIAL_CAPACITY;
    }
    
    /* Pas 3: Alocă array-ul de date */
    pq->data = malloc(initial_capacity * sizeof(int));
    if (pq->data == NULL) {
        free(pq);
        return NULL;
    }
    
    /* Pas 4: Inițializează câmpurile */
    pq->size = 0;
    pq->capacity = initial_capacity;
    
    return pq;
}

/* SOLUȚIE TODO 4: Funcția de distrugere */
void pq_destroy(PriorityQueue *pq) {
    if (pq != NULL) {
        /* Eliberează array-ul mai întâi */
        free(pq->data);
        /* Apoi eliberează structura */
        free(pq);
    }
}

/* =============================================================================
 * FUNCȚII DE INTEROGARE
 * =============================================================================
 */

/* SOLUȚIE TODO 5: Verificare dacă e gol */
bool pq_is_empty(PriorityQueue *pq) {
    return (pq == NULL || pq->size == 0);
}

/* SOLUȚIE TODO 6: Peek - returnează maximul fără a-l extrage */
int pq_peek(PriorityQueue *pq) {
    if (pq_is_empty(pq)) {
        fprintf(stderr, "Eroare: Priority queue goală!\n");
        return -1;
    }
    return pq->data[0];
}

int pq_size(PriorityQueue *pq) {
    return (pq != NULL) ? pq->size : 0;
}

/* =============================================================================
 * FUNCȚII HEAP FUNDAMENTALE
 * =============================================================================
 */

/* SOLUȚIE TODO 7: Heapify (sift-down) */
void pq_heapify(PriorityQueue *pq, int i) {
    if (pq == NULL || pq->size == 0) return;
    
    int largest = i;
    int left = LEFT_CHILD(i);
    int right = RIGHT_CHILD(i);
    
    /* Compară cu copilul stâng */
    if (left < pq->size && pq->data[left] > pq->data[largest]) {
        largest = left;
    }
    
    /* Compară cu copilul drept */
    if (right < pq->size && pq->data[right] > pq->data[largest]) {
        largest = right;
    }
    
    /* Dacă cel mai mare nu este rădăcina */
    if (largest != i) {
        swap(&pq->data[i], &pq->data[largest]);
        pq_heapify(pq, largest);  /* Recursie */
    }
}

/* SOLUȚIE TODO 8: Redimensionare */
static void pq_resize(PriorityQueue *pq) {
    int new_capacity = pq->capacity * GROWTH_FACTOR;
    
    /* Încearcă redimensionarea */
    int *new_data = realloc(pq->data, new_capacity * sizeof(int));
    
    if (new_data != NULL) {
        pq->data = new_data;
        pq->capacity = new_capacity;
    } else {
        fprintf(stderr, "Atenție: Redimensionare eșuată!\n");
    }
}

/* SOLUȚIE TODO 9: Inserare cu sift-up */
void pq_insert(PriorityQueue *pq, int key) {
    if (pq == NULL) return;
    
    /* Verifică și redimensionează dacă e nevoie */
    if (pq->size >= pq->capacity) {
        pq_resize(pq);
    }
    
    /* Adaugă la sfârșit */
    int i = pq->size;
    pq->data[i] = key;
    pq->size++;
    
    /* Sift-up: urcă până la poziția corectă */
    while (i > 0 && pq->data[PARENT(i)] < pq->data[i]) {
        swap(&pq->data[i], &pq->data[PARENT(i)]);
        i = PARENT(i);
    }
}

/* SOLUȚIE TODO 10: Extragere maximum */
int pq_extract_max(PriorityQueue *pq) {
    if (pq_is_empty(pq)) {
        fprintf(stderr, "Eroare: Priority queue goală!\n");
        return -1;
    }
    
    /* Salvează maximul */
    int max = pq->data[0];
    
    /* Mută ultimul element la rădăcină */
    pq->data[0] = pq->data[pq->size - 1];
    pq->size--;
    
    /* Restaurează proprietatea heap */
    if (pq->size > 0) {
        pq_heapify(pq, 0);
    }
    
    return max;
}

/* =============================================================================
 * PROGRAMUL PRINCIPAL
 * =============================================================================
 */

int main(void) {
    printf("=== EXERCIȚIUL 1: Priority Queue cu Max-Heap ===\n\n");
    
    PriorityQueue *pq = pq_create(INITIAL_CAPACITY);
    if (pq == NULL) {
        fprintf(stderr, "Eroare: Nu s-a putut crea priority queue!\n");
        return EXIT_FAILURE;
    }
    printf("Priority queue creată cu succes.\n\n");
    
    printf("Introduceți numărul de elemente: ");
    int n;
    if (scanf("%d", &n) != 1 || n < 0) {
        fprintf(stderr, "Eroare: Număr invalid de elemente!\n");
        pq_destroy(pq);
        return EXIT_FAILURE;
    }
    
    printf("Introduceți %d elemente:\n", n);
    for (int i = 0; i < n; i++) {
        int val;
        if (scanf("%d", &val) != 1) {
            fprintf(stderr, "Eroare la citirea elementului %d!\n", i + 1);
            pq_destroy(pq);
            return EXIT_FAILURE;
        }
        printf("  Inserez %d... ", val);
        pq_insert(pq, val);
        printf("Heap: ");
        pq_print(pq);
    }
    
    printf("\n--- Stare Curentă ---\n");
    printf("Dimensiune: %d\n", pq_size(pq));
    printf("Este gol? %s\n", pq_is_empty(pq) ? "Da" : "Nu");
    if (!pq_is_empty(pq)) {
        printf("Maximum (peek): %d\n", pq_peek(pq));
    }
    printf("Conținut: ");
    pq_print(pq);
    
    printf("\n--- Extragere în Ordine Descrescătoare ---\n");
    printf("Elemente extrase: ");
    while (!pq_is_empty(pq)) {
        int max = pq_extract_max(pq);
        printf("%d ", max);
    }
    printf("\n");
    
    printf("\n--- Verificare Finală ---\n");
    printf("Dimensiune după extragere: %d\n", pq_size(pq));
    printf("Este gol? %s\n", pq_is_empty(pq) ? "Da" : "Nu");
    
    pq_destroy(pq);
    printf("\nMemorie eliberată. Program terminat cu succes!\n");
    
    return EXIT_SUCCESS;
}
