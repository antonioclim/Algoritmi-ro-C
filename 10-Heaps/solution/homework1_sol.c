/**
 * =============================================================================
 * SOLUȚIE TEMA 1: Sistem de Gestionare a Proceselor
 * =============================================================================
 *
 * Simulator de planificare a proceselor folosind priority queue cu max-heap.
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -o homework1_sol homework1_sol.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* =============================================================================
 * CONSTANTE ȘI MACRO-URI
 * =============================================================================
 */

#define INITIAL_CAPACITY 16
#define NAME_LENGTH 32

#define PARENT(i)      (((i) - 1) / 2)
#define LEFT_CHILD(i)  ((2 * (i)) + 1)
#define RIGHT_CHILD(i) ((2 * (i)) + 2)

/* =============================================================================
 * STRUCTURI DE DATE
 * =============================================================================
 */

typedef struct {
    int pid;              /* Process ID unic */
    char name[NAME_LENGTH]; /* Numele procesului */
    int priority;         /* Prioritate (0-100) */
    int burst_time;       /* Timp de execuție în ms */
    int arrival_time;     /* Timpul de sosire */
} Process;

typedef struct {
    Process *processes;   /* Array de procese */
    int size;             /* Număr curent de procese */
    int capacity;         /* Capacitate alocată */
    int next_pid;         /* Următorul PID disponibil */
} ProcessQueue;

/* =============================================================================
 * FUNCȚII AUXILIARE
 * =============================================================================
 */

static void swap_processes(Process *a, Process *b) {
    Process tmp = *a;
    *a = *b;
    *b = tmp;
}

/* =============================================================================
 * FUNCȚII PENTRU PROCESS QUEUE
 * =============================================================================
 */

/**
 * Creează o nouă coadă de procese
 */
ProcessQueue* pqueue_create(int capacity) {
    ProcessQueue *pq = malloc(sizeof(ProcessQueue));
    if (!pq) return NULL;
    
    if (capacity <= 0) capacity = INITIAL_CAPACITY;
    
    pq->processes = malloc(capacity * sizeof(Process));
    if (!pq->processes) {
        free(pq);
        return NULL;
    }
    
    pq->size = 0;
    pq->capacity = capacity;
    pq->next_pid = 1;
    
    return pq;
}

/**
 * Eliberează memoria
 */
void pqueue_destroy(ProcessQueue *pq) {
    if (pq) {
        free(pq->processes);
        free(pq);
    }
}

/**
 * Verifică dacă coada este goală
 */
bool pqueue_is_empty(ProcessQueue *pq) {
    return (pq == NULL || pq->size == 0);
}

/**
 * Heapify bazat pe prioritate (max-heap)
 */
static void pqueue_heapify(ProcessQueue *pq, int i) {
    int largest = i;
    int left = LEFT_CHILD(i);
    int right = RIGHT_CHILD(i);

    if (left < pq->size && 
        pq->processes[left].priority > pq->processes[largest].priority) {
        largest = left;
    }

    if (right < pq->size && 
        pq->processes[right].priority > pq->processes[largest].priority) {
        largest = right;
    }

    if (largest != i) {
        swap_processes(&pq->processes[i], &pq->processes[largest]);
        pqueue_heapify(pq, largest);
    }
}

/**
 * Redimensionează array-ul intern
 */
static void pqueue_resize(ProcessQueue *pq) {
    int new_capacity = pq->capacity * 2;
    Process *new_processes = realloc(pq->processes, 
                                     new_capacity * sizeof(Process));
    if (new_processes) {
        pq->processes = new_processes;
        pq->capacity = new_capacity;
    }
}

/**
 * Inserează un proces în coadă
 */
void pqueue_insert(ProcessQueue *pq, Process p) {
    if (!pq) return;
    
    if (pq->size >= pq->capacity) {
        pqueue_resize(pq);
    }
    
    /* Atribuie PID dacă nu e setat */
    if (p.pid == 0) {
        p.pid = pq->next_pid++;
    }
    
    /* Adaugă la sfârșit */
    int i = pq->size;
    pq->processes[i] = p;
    pq->size++;
    
    /* Sift-up */
    while (i > 0 && 
           pq->processes[PARENT(i)].priority < pq->processes[i].priority) {
        swap_processes(&pq->processes[i], &pq->processes[PARENT(i)]);
        i = PARENT(i);
    }
}

/**
 * Extrage procesul cu cea mai mare prioritate
 */
Process pqueue_extract_max(ProcessQueue *pq) {
    Process empty = {0, "", 0, 0, 0};
    
    if (pqueue_is_empty(pq)) {
        fprintf(stderr, "Eroare: Coada de procese este goală!\n");
        return empty;
    }
    
    Process max = pq->processes[0];
    pq->processes[0] = pq->processes[pq->size - 1];
    pq->size--;
    
    if (pq->size > 0) {
        pqueue_heapify(pq, 0);
    }
    
    return max;
}

/**
 * Găsește indexul unui proces după PID
 */
static int find_process_by_pid(ProcessQueue *pq, int pid) {
    for (int i = 0; i < pq->size; i++) {
        if (pq->processes[i].pid == pid) {
            return i;
        }
    }
    return -1;
}

/**
 * Modifică prioritatea unui proces
 */
void pqueue_increase_priority(ProcessQueue *pq, int pid, int new_priority) {
    if (!pq) return;
    
    int i = find_process_by_pid(pq, pid);
    if (i == -1) {
        fprintf(stderr, "Eroare: Procesul cu PID %d nu a fost găsit!\n", pid);
        return;
    }
    
    if (new_priority < pq->processes[i].priority) {
        fprintf(stderr, "Eroare: Noua prioritate trebuie să fie mai mare!\n");
        return;
    }
    
    pq->processes[i].priority = new_priority;
    
    /* Sift-up pentru a restaura proprietatea heap */
    while (i > 0 && 
           pq->processes[PARENT(i)].priority < pq->processes[i].priority) {
        swap_processes(&pq->processes[i], &pq->processes[PARENT(i)]);
        i = PARENT(i);
    }
}

/**
 * Afișează conținutul cozii
 */
void pqueue_print(ProcessQueue *pq) {
    if (pqueue_is_empty(pq)) {
        printf("Coada de procese este goală.\n");
        return;
    }
    
    printf("\n┌──────┬────────────────────┬───────────┬────────────┬──────────┐\n");
    printf("│ PID  │ Nume               │ Prioritate│ Burst Time │ Arrival  │\n");
    printf("├──────┼────────────────────┼───────────┼────────────┼──────────┤\n");
    
    for (int i = 0; i < pq->size; i++) {
        printf("│ %4d │ %-18s │ %9d │ %8d ms│ %6d ms│\n",
               pq->processes[i].pid,
               pq->processes[i].name,
               pq->processes[i].priority,
               pq->processes[i].burst_time,
               pq->processes[i].arrival_time);
    }
    
    printf("└──────┴────────────────────┴───────────┴────────────┴──────────┘\n");
}

/* =============================================================================
 * SIMULARE
 * =============================================================================
 */

void simulate_scheduling(ProcessQueue *pq) {
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("               SIMULARE PLANIFICARE PROCESE                    \n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    int total_time = 0;
    int order = 1;
    
    printf("Ordine de execuție:\n\n");
    
    while (!pqueue_is_empty(pq)) {
        Process p = pqueue_extract_max(pq);
        
        printf("  %d. [PID %d] %-15s (prioritate: %3d, timp: %4d ms)\n",
               order++, p.pid, p.name, p.priority, p.burst_time);
        
        total_time += p.burst_time;
    }
    
    printf("\n───────────────────────────────────────────────────────────────\n");
    printf("Timp total de execuție: %d ms\n", total_time);
    printf("Număr total de procese executate: %d\n", order - 1);
    printf("═══════════════════════════════════════════════════════════════\n");
}

/* =============================================================================
 * PROGRAMUL PRINCIPAL
 * =============================================================================
 */

int main(void) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║        TEMA 1: Sistem de Gestionare a Proceselor              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    ProcessQueue *pq = pqueue_create(INITIAL_CAPACITY);
    if (!pq) {
        fprintf(stderr, "Eroare: Nu s-a putut crea coada de procese!\n");
        return EXIT_FAILURE;
    }
    
    /* Citire număr de procese */
    printf("Introduceți numărul de procese: ");
    int n;
    if (scanf("%d", &n) != 1 || n <= 0) {
        fprintf(stderr, "Eroare: Număr invalid de procese!\n");
        pqueue_destroy(pq);
        return EXIT_FAILURE;
    }
    
    printf("Introduceți procesele (nume prioritate burst_time arrival_time):\n");
    
    /* Citire procese */
    for (int i = 0; i < n; i++) {
        Process p;
        p.pid = 0;  /* Va fi setat automat */
        
        if (scanf("%31s %d %d %d", p.name, &p.priority, 
                  &p.burst_time, &p.arrival_time) != 4) {
            fprintf(stderr, "Eroare la citirea procesului %d!\n", i + 1);
            pqueue_destroy(pq);
            return EXIT_FAILURE;
        }
        
        pqueue_insert(pq, p);
        printf("  Proces adăugat: %s (prioritate: %d)\n", p.name, p.priority);
    }
    
    /* Afișare stare curentă */
    printf("\nStare curentă a cozii de procese:");
    pqueue_print(pq);
    
    /* Simulare */
    simulate_scheduling(pq);
    
    /* Curățare */
    pqueue_destroy(pq);
    printf("\nResurse eliberate. Program terminat cu succes!\n");
    
    return EXIT_SUCCESS;
}
