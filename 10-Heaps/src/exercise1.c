/**
 * =============================================================================
 * EXERCIȚIUL 1: Implementare Priority Queue cu Max-Heap
 * =============================================================================
 *
 * OBIECTIV:
 *   Implementați o coadă cu priorități (priority queue) bazată pe max-heap.
 *   Structura trebuie să suporte operațiile standard: inserare, extragere
 *   a elementului maxim, vizualizare și verificare dacă este goală.
 *
 * CERINȚE:
 *   1. Completați structura PriorityQueue
 *   2. Implementați funcțiile de creare și distrugere
 *   3. Implementați heapify (sift-down)
 *   4. Implementați inserarea cu sift-up
 *   5. Implementați extragerea maximului
 *   6. Implementați redimensionarea dinamică
 *   7. Gestionați corect memoria (fără memory leaks)
 *
 * EXEMPLU INPUT:
 *   Inserare: 30, 10, 50, 20, 40
 *
 * OUTPUT AȘTEPTAT:
 *   Maximum (peek): 50
 *   Extragere în ordine: 50, 40, 30, 20, 10
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -o exercise1 exercise1.c
 * TESTARE: ./exercise1 < ../tests/test1_input.txt
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

/**
 * TODO 1: Completați macro-urile pentru navigarea în heap
 *
 * Pentru un heap reprezentat în array cu indexare de la 0:
 * - Părintele nodului i este la indexul (i-1)/2
 * - Copilul stâng al nodului i este la indexul 2*i+1
 * - Copilul drept al nodului i este la indexul 2*i+2
 *
 * Hint: Folosiți paranteze pentru a evita probleme cu precedența operatorilor
 */
#define PARENT(i)      /* YOUR CODE HERE */
#define LEFT_CHILD(i)  /* YOUR CODE HERE */
#define RIGHT_CHILD(i) /* YOUR CODE HERE */

/* =============================================================================
 * DEFINIȚIA STRUCTURII
 * =============================================================================
 */

/**
 * TODO 2: Completați definiția structurii PriorityQueue
 *
 * Structura trebuie să conțină:
 * - Un pointer către array-ul de date (int*)
 * - Dimensiunea curentă (câte elemente sunt în heap)
 * - Capacitatea (câte elemente pot fi stocate înainte de redimensionare)
 */
typedef struct {
    /* YOUR CODE HERE */
    
} PriorityQueue;

/* =============================================================================
 * FUNCȚII AUXILIARE (FURNIZATE)
 * =============================================================================
 */

/**
 * Funcție auxiliară pentru swap - furnizată
 */
static void swap(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

/**
 * Afișează conținutul heap-ului - furnizată
 */
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

/**
 * TODO 3: Implementați funcția de creare a priority queue
 *
 * @param initial_capacity - Capacitatea inițială (dacă e 0, folosiți INITIAL_CAPACITY)
 * @return Pointer către noua priority queue, sau NULL în caz de eroare
 *
 * Pași:
 *   1. Alocați memorie pentru structura PriorityQueue
 *   2. Verificați dacă alocarea a reușit
 *   3. Dacă initial_capacity e 0, folosiți INITIAL_CAPACITY
 *   4. Alocați memorie pentru array-ul de date
 *   5. Verificați dacă alocarea a reușit (eliberați structura dacă nu)
 *   6. Inițializați size = 0 și capacity = capacitatea aleasă
 *   7. Returnați pointer-ul
 */
PriorityQueue* pq_create(int initial_capacity) {
    /* YOUR CODE HERE */
    
    return NULL;  /* Înlocuiți cu implementarea corectă */
}

/**
 * TODO 4: Implementați funcția de distrugere/eliberare a memoriei
 *
 * @param pq - Pointer către priority queue
 *
 * Pași:
 *   1. Verificați dacă pq este NULL
 *   2. Eliberați array-ul de date
 *   3. Eliberați structura
 *
 * Hint: Ordinea eliberării este importantă!
 */
void pq_destroy(PriorityQueue *pq) {
    /* YOUR CODE HERE */
    
}

/* =============================================================================
 * FUNCȚII DE INTEROGARE
 * =============================================================================
 */

/**
 * TODO 5: Implementați funcția care verifică dacă heap-ul este gol
 *
 * @param pq - Pointer către priority queue
 * @return true dacă heap-ul este gol, false altfel
 */
bool pq_is_empty(PriorityQueue *pq) {
    /* YOUR CODE HERE */
    
    return true;  /* Înlocuiți cu implementarea corectă */
}

/**
 * TODO 6: Implementați funcția care returnează maximul fără a-l extrage
 *
 * @param pq - Pointer către priority queue
 * @return Valoarea maximă din heap
 *
 * Notă: Dacă heap-ul este gol, afișați eroare și returnați -1
 *       (într-o implementare reală am folosi coduri de eroare sau excepții)
 */
int pq_peek(PriorityQueue *pq) {
    /* YOUR CODE HERE */
    
    return -1;  /* Înlocuiți cu implementarea corectă */
}

/**
 * Returnează dimensiunea curentă - furnizată
 */
int pq_size(PriorityQueue *pq) {
    return (pq != NULL) ? pq->size : 0;
}

/* =============================================================================
 * FUNCȚII HEAP FUNDAMENTALE
 * =============================================================================
 */

/**
 * TODO 7: Implementați operația heapify (sift-down) pentru max-heap
 *
 * @param pq - Pointer către priority queue
 * @param i  - Indexul nodului de la care începem heapify
 *
 * Algoritm:
 *   1. Presupunem că cel mai mare element este la indexul i (largest = i)
 *   2. Calculăm indicii copiilor stâng și drept
 *   3. Dacă copilul stâng există (< size) și e mai mare ca arr[largest],
 *      actualizăm largest = left
 *   4. Dacă copilul drept există și e mai mare ca arr[largest],
 *      actualizăm largest = right
 *   5. Dacă largest != i:
 *      - Facem swap între arr[i] și arr[largest]
 *      - Apelăm recursiv heapify pe largest
 *
 * Hint: Folosiți macro-urile LEFT_CHILD și RIGHT_CHILD
 */
void pq_heapify(PriorityQueue *pq, int i) {
    if (pq == NULL || pq->size == 0) return;
    
    /* YOUR CODE HERE */
    
}

/**
 * TODO 8: Implementați redimensionarea array-ului când capacitatea e depășită
 *
 * @param pq - Pointer către priority queue
 *
 * Pași:
 *   1. Calculați noua capacitate (capacity * GROWTH_FACTOR)
 *   2. Folosiți realloc pentru a redimensiona array-ul
 *   3. Verificați dacă realloc a reușit
 *   4. Actualizați pointer-ul data și capacitatea
 *
 * Hint: Salvați rezultatul realloc într-un pointer temporar pentru
 *       a nu pierde datele originale în caz de eroare
 */
static void pq_resize(PriorityQueue *pq) {
    /* YOUR CODE HERE */
    
}

/**
 * TODO 9: Implementați inserarea unui element în priority queue
 *
 * @param pq  - Pointer către priority queue
 * @param key - Valoarea de inserat
 *
 * Algoritm (sift-up):
 *   1. Verificați dacă pq este NULL
 *   2. Dacă size >= capacity, apelați pq_resize
 *   3. Adăugați elementul la sfârșitul array-ului (index = size)
 *   4. Incrementați size
 *   5. Sift-up: Cât timp nodul nu e rădăcină (i > 0) și
 *      părintele e mai mic decât nodul curent:
 *      - Faceți swap între nod și părinte
 *      - Mutați i la indexul părintelui
 */
void pq_insert(PriorityQueue *pq, int key) {
    if (pq == NULL) return;
    
    /* YOUR CODE HERE */
    
}

/**
 * TODO 10: Implementați extragerea elementului maxim
 *
 * @param pq - Pointer către priority queue
 * @return Valoarea maximă extrasă, sau -1 dacă heap-ul e gol
 *
 * Algoritm:
 *   1. Verificați dacă pq este NULL sau gol
 *   2. Salvați valoarea de la rădăcină (index 0)
 *   3. Mutați ultimul element la rădăcină
 *   4. Decrementați size
 *   5. Dacă heap-ul nu e gol, apelați heapify de la rădăcină
 *   6. Returnați valoarea salvată
 */
int pq_extract_max(PriorityQueue *pq) {
    /* YOUR CODE HERE */
    
    return -1;  /* Înlocuiți cu implementarea corectă */
}

/* =============================================================================
 * PROGRAMUL PRINCIPAL - TESTARE
 * =============================================================================
 */

int main(void) {
    printf("=== EXERCIȚIUL 1: Priority Queue cu Max-Heap ===\n\n");
    
    /* Creează priority queue */
    PriorityQueue *pq = pq_create(INITIAL_CAPACITY);
    if (pq == NULL) {
        fprintf(stderr, "Eroare: Nu s-a putut crea priority queue!\n");
        return EXIT_FAILURE;
    }
    printf("Priority queue creată cu succes.\n\n");
    
    /* Citire date de la stdin */
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
    
    /* Afișare informații */
    printf("\n--- Stare Curentă ---\n");
    printf("Dimensiune: %d\n", pq_size(pq));
    printf("Este gol? %s\n", pq_is_empty(pq) ? "Da" : "Nu");
    if (!pq_is_empty(pq)) {
        printf("Maximum (peek): %d\n", pq_peek(pq));
    }
    printf("Conținut: ");
    pq_print(pq);
    
    /* Extragere toate elementele */
    printf("\n--- Extragere în Ordine Descrescătoare ---\n");
    printf("Elemente extrase: ");
    while (!pq_is_empty(pq)) {
        int max = pq_extract_max(pq);
        printf("%d ", max);
    }
    printf("\n");
    
    /* Verificare finală */
    printf("\n--- Verificare Finală ---\n");
    printf("Dimensiune după extragere: %d\n", pq_size(pq));
    printf("Este gol? %s\n", pq_is_empty(pq) ? "Da" : "Nu");
    
    /* Eliberare memorie */
    pq_destroy(pq);
    printf("\nMemorie eliberată. Program terminat cu succes!\n");
    
    return EXIT_SUCCESS;
}

/* =============================================================================
 * PROVOCĂRI BONUS (OPȚIONAL)
 * =============================================================================
 *
 * 1. Adăugați funcția pq_increase_key(pq, index, new_key) care mărește
 *    valoarea unui element și restaurează proprietatea heap
 *
 * 2. Adăugați funcția pq_delete(pq, index) care șterge un element arbitrar
 *
 * 3. Transformați implementarea într-un min-heap (inversați comparațiile)
 *
 * 4. Adăugați suport pentru elemente generice folosind void* și comparator
 *
 * =============================================================================
 */
