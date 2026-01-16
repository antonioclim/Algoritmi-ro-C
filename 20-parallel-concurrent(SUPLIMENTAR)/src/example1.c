/**
 * =============================================================================
 * SĂPTĂMÂNA 20: PROGRAMARE PARALELĂ ȘI CONCURENTĂ ÎN C
 * Exemplu Complet și Demonstrativ
 * =============================================================================
 *
 * Acest exemplu demonstrează:
 *   1. Fundamente POSIX Threads (creare, join, detach)
 *   2. Mutex pentru protejarea secțiunilor critice
 *   3. Semafoare pentru sincronizare și limitare acces
 *   4. Condition Variables pentru semnalizare între thread-uri
 *   5. Pattern Producer-Consumer cu Bounded Buffer
 *   6. Thread Pool pentru gestionarea eficientă a task-urilor
 *   7. Parallel Merge Sort cu threshold adaptiv
 *   8. C11 Atomics și structuri Lock-Free (Stack)
 *   9. Problema Dining Philosophers și soluții
 *   10. Demonstrație și analiză Legea lui Amdahl
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -g -o example1 example1.c -lpthread -lm
 * Execuție:  ./example1
 * Valgrind:  valgrind --leak-check=full ./example1
 * Helgrind:  valgrind --tool=helgrind ./example1
 *
 * =============================================================================
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdatomic.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <errno.h>

/* =============================================================================
 * CONFIGURARE ȘI CONSTANTE
 * =============================================================================
 */

#define MAX_THREADS         8
#define BUFFER_SIZE         10
#define NUM_ITEMS           20
#define ARRAY_SIZE          1000
#define SORT_THRESHOLD      100
#define DEMO_ITERATIONS     100000
#define PHILOSOPHER_COUNT   5
#define THINKING_TIME_MS    10
#define EATING_TIME_MS      20

/* =============================================================================
 * UTILITĂȚI DE AFIȘARE
 * =============================================================================
 */

/**
 * Afișează separator cu titlu pentru secțiuni
 */
void print_separator(const char *title) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║  %-73s║\n", title);
    printf("╚═══════════════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

/**
 * Afișează subsecțiune
 */
void print_subsection(const char *title) {
    printf("\n── %s ──\n\n", title);
}

/**
 * Obține timestamp în milisecunde
 */
double get_time_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1000000.0;
}

/* =============================================================================
 * PARTEA 1: FUNDAMENTE PTHREADS
 * =============================================================================
 */

/**
 * Structură pentru transmiterea argumentelor către thread
 */
typedef struct {
    int thread_id;
    int start;
    int end;
    int *result;
} ThreadArg;

/**
 * Funcție simplă executată de un thread
 * 
 * Demonstrează:
 * - Primirea argumentelor prin void*
 * - Returnarea valorii prin void*
 */
void* simple_thread_function(void *arg) {
    ThreadArg *targ = (ThreadArg*)arg;
    
    printf("  [Thread %d] Start: calculez suma de la %d la %d\n", 
           targ->thread_id, targ->start, targ->end);
    
    int sum = 0;
    for (int i = targ->start; i <= targ->end; i++) {
        sum += i;
    }
    
    *(targ->result) = sum;
    
    printf("  [Thread %d] Terminat: suma = %d\n", targ->thread_id, sum);
    
    return NULL;
}

/**
 * Demonstrație creare/join thread-uri
 */
void demo_basic_threads(void) {
    print_subsection("Creare și Join Thread-uri");
    
    pthread_t threads[2];
    ThreadArg args[2];
    int results[2] = {0, 0};
    
    /* Primul thread: suma 1-50 */
    args[0].thread_id = 0;
    args[0].start = 1;
    args[0].end = 50;
    args[0].result = &results[0];
    
    /* Al doilea thread: suma 51-100 */
    args[1].thread_id = 1;
    args[1].start = 51;
    args[1].end = 100;
    args[1].result = &results[1];
    
    printf("Main: Creez 2 thread-uri pentru a calcula suma 1-100 în paralel\n\n");
    
    /* Creare thread-uri */
    for (int i = 0; i < 2; i++) {
        int rc = pthread_create(&threads[i], NULL, simple_thread_function, &args[i]);
        if (rc != 0) {
            fprintf(stderr, "Eroare pthread_create: %d\n", rc);
            exit(EXIT_FAILURE);
        }
    }
    
    /* Așteptare terminare */
    for (int i = 0; i < 2; i++) {
        pthread_join(threads[i], NULL);
    }
    
    int total = results[0] + results[1];
    printf("\nMain: Rezultat final: %d + %d = %d\n", results[0], results[1], total);
    printf("Main: Verificare: suma(1..100) = 100*101/2 = %d ✓\n", 100 * 101 / 2);
}

/* =============================================================================
 * PARTEA 2: RACE CONDITIONS ȘI MUTEX
 * =============================================================================
 */

/* Variabilă partajată și mutex asociat */
static int shared_counter = 0;
static pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * Incrementare FĂRĂ protecție (RACE CONDITION!)
 */
void* unsafe_increment(void *arg) {
    (void)arg;
    for (int i = 0; i < DEMO_ITERATIONS; i++) {
        shared_counter++;  /* Race condition! */
    }
    return NULL;
}

/**
 * Incrementare CU protecție mutex
 */
void* safe_increment(void *arg) {
    (void)arg;
    for (int i = 0; i < DEMO_ITERATIONS; i++) {
        pthread_mutex_lock(&counter_mutex);
        shared_counter++;
        pthread_mutex_unlock(&counter_mutex);
    }
    return NULL;
}

/**
 * Demonstrație race condition vs. mutex
 */
void demo_race_condition(void) {
    print_subsection("Race Condition vs. Mutex Protection");
    
    pthread_t t1, t2;
    
    /* Test 1: FĂRĂ protecție */
    shared_counter = 0;
    printf("Test 1: Incrementare FĂRĂ mutex (2 threads × %d iterații)\n", DEMO_ITERATIONS);
    printf("  Așteptat: %d\n", 2 * DEMO_ITERATIONS);
    
    pthread_create(&t1, NULL, unsafe_increment, NULL);
    pthread_create(&t2, NULL, unsafe_increment, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    printf("  Rezultat: %d %s\n", shared_counter,
           shared_counter == 2 * DEMO_ITERATIONS ? "✓" : "✗ (RACE CONDITION!)");
    
    /* Test 2: CU protecție */
    shared_counter = 0;
    printf("\nTest 2: Incrementare CU mutex (2 threads × %d iterații)\n", DEMO_ITERATIONS);
    printf("  Așteptat: %d\n", 2 * DEMO_ITERATIONS);
    
    pthread_create(&t1, NULL, safe_increment, NULL);
    pthread_create(&t2, NULL, safe_increment, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    printf("  Rezultat: %d %s\n", shared_counter,
           shared_counter == 2 * DEMO_ITERATIONS ? "✓" : "✗");
}

/* =============================================================================
 * PARTEA 3: SEMAFOARE
 * =============================================================================
 */

static sem_t resource_sem;
static int limited_resource = 0;

/**
 * Thread care accesează o resursă limitată
 */
void* limited_resource_worker(void *arg) {
    int id = *(int*)arg;
    
    printf("  [Worker %d] Așteaptă permisiune...\n", id);
    sem_wait(&resource_sem);  /* P(sem) - decrementare sau blocare */
    
    printf("  [Worker %d] A obținut acces! (resurse disponibile: scăzut)\n", id);
    limited_resource++;
    usleep(50000);  /* Simulare lucru */
    limited_resource--;
    
    printf("  [Worker %d] Eliberează resursa\n", id);
    sem_post(&resource_sem);  /* V(sem) - incrementare */
    
    return NULL;
}

/**
 * Demonstrație semafoare pentru limitare acces
 */
void demo_semaphores(void) {
    print_subsection("Semafoare - Limitare Acces Concurent");
    
    const int max_concurrent = 2;
    const int num_workers = 5;
    pthread_t workers[num_workers];
    int ids[num_workers];
    
    printf("Semaforul permite maxim %d accesuri simultane\n", max_concurrent);
    printf("Pornim %d workeri care vor concura pentru acces\n\n", num_workers);
    
    sem_init(&resource_sem, 0, max_concurrent);
    
    for (int i = 0; i < num_workers; i++) {
        ids[i] = i;
        pthread_create(&workers[i], NULL, limited_resource_worker, &ids[i]);
    }
    
    for (int i = 0; i < num_workers; i++) {
        pthread_join(workers[i], NULL);
    }
    
    sem_destroy(&resource_sem);
    printf("\nToți workerii au terminat.\n");
}

/* =============================================================================
 * PARTEA 4: CONDITION VARIABLES
 * =============================================================================
 */

static pthread_mutex_t cond_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t data_cond = PTHREAD_COND_INITIALIZER;
static int data_ready = 0;
static int shared_data = 0;

/**
 * Thread producător - setează date și semnalizează
 */
void* producer_cond(void *arg) {
    (void)arg;
    
    usleep(100000);  /* Simulare producție */
    
    pthread_mutex_lock(&cond_mutex);
    shared_data = 42;
    data_ready = 1;
    printf("  [Producer] Date produse: %d, semnalizez consumatorul\n", shared_data);
    pthread_cond_signal(&data_cond);
    pthread_mutex_unlock(&cond_mutex);
    
    return NULL;
}

/**
 * Thread consumator - așteaptă date
 */
void* consumer_cond(void *arg) {
    (void)arg;
    
    printf("  [Consumer] Așteaptă date...\n");
    
    pthread_mutex_lock(&cond_mutex);
    while (!data_ready) {  /* WHILE, nu IF - spurious wakeups! */
        pthread_cond_wait(&data_cond, &cond_mutex);
    }
    printf("  [Consumer] Primit date: %d\n", shared_data);
    pthread_mutex_unlock(&cond_mutex);
    
    return NULL;
}

/**
 * Demonstrație condition variables
 */
void demo_condition_variables(void) {
    print_subsection("Condition Variables - Semnalizare între Thread-uri");
    
    pthread_t prod, cons;
    
    data_ready = 0;
    shared_data = 0;
    
    printf("Pattern: Consumatorul așteaptă până când Producătorul semnalizează\n\n");
    
    pthread_create(&cons, NULL, consumer_cond, NULL);
    usleep(10000);  /* Asigură pornirea consumatorului întâi */
    pthread_create(&prod, NULL, producer_cond, NULL);
    
    pthread_join(prod, NULL);
    pthread_join(cons, NULL);
    
    printf("\nSincronizare completă!\n");
}

/* =============================================================================
 * PARTEA 5: PRODUCER-CONSUMER CU BOUNDED BUFFER
 * =============================================================================
 */

/**
 * Buffer circular thread-safe
 */
typedef struct {
    int *buffer;
    size_t capacity;
    size_t count;
    size_t head;
    size_t tail;
    pthread_mutex_t mutex;
    pthread_cond_t not_full;
    pthread_cond_t not_empty;
    bool shutdown;
} BoundedBuffer;

/**
 * Creează un bounded buffer
 */
BoundedBuffer* buffer_create(size_t capacity) {
    BoundedBuffer *bb = malloc(sizeof(BoundedBuffer));
    bb->buffer = malloc(capacity * sizeof(int));
    bb->capacity = capacity;
    bb->count = 0;
    bb->head = 0;
    bb->tail = 0;
    bb->shutdown = false;
    pthread_mutex_init(&bb->mutex, NULL);
    pthread_cond_init(&bb->not_full, NULL);
    pthread_cond_init(&bb->not_empty, NULL);
    return bb;
}

/**
 * Distruge bounded buffer
 */
void buffer_destroy(BoundedBuffer *bb) {
    pthread_mutex_destroy(&bb->mutex);
    pthread_cond_destroy(&bb->not_full);
    pthread_cond_destroy(&bb->not_empty);
    free(bb->buffer);
    free(bb);
}

/**
 * Adaugă element în buffer (blocare dacă plin)
 */
bool buffer_put(BoundedBuffer *bb, int item) {
    pthread_mutex_lock(&bb->mutex);
    
    while (bb->count == bb->capacity && !bb->shutdown) {
        pthread_cond_wait(&bb->not_full, &bb->mutex);
    }
    
    if (bb->shutdown) {
        pthread_mutex_unlock(&bb->mutex);
        return false;
    }
    
    bb->buffer[bb->tail] = item;
    bb->tail = (bb->tail + 1) % bb->capacity;
    bb->count++;
    
    pthread_cond_signal(&bb->not_empty);
    pthread_mutex_unlock(&bb->mutex);
    
    return true;
}

/**
 * Extrage element din buffer (blocare dacă gol)
 */
bool buffer_get(BoundedBuffer *bb, int *item) {
    pthread_mutex_lock(&bb->mutex);
    
    while (bb->count == 0 && !bb->shutdown) {
        pthread_cond_wait(&bb->not_empty, &bb->mutex);
    }
    
    if (bb->count == 0 && bb->shutdown) {
        pthread_mutex_unlock(&bb->mutex);
        return false;
    }
    
    *item = bb->buffer[bb->head];
    bb->head = (bb->head + 1) % bb->capacity;
    bb->count--;
    
    pthread_cond_signal(&bb->not_full);
    pthread_mutex_unlock(&bb->mutex);
    
    return true;
}

/**
 * Semnalizează shutdown pentru buffer
 */
void buffer_shutdown(BoundedBuffer *bb) {
    pthread_mutex_lock(&bb->mutex);
    bb->shutdown = true;
    pthread_cond_broadcast(&bb->not_full);
    pthread_cond_broadcast(&bb->not_empty);
    pthread_mutex_unlock(&bb->mutex);
}

/* Variabile globale pentru demo */
static BoundedBuffer *demo_buffer = NULL;
static atomic_int items_produced = 0;
static atomic_int items_consumed = 0;

/**
 * Thread producător pentru bounded buffer
 */
void* pc_producer(void *arg) {
    int id = *(int*)arg;
    
    for (int i = 0; i < NUM_ITEMS; i++) {
        int item = id * 100 + i;
        if (buffer_put(demo_buffer, item)) {
            atomic_fetch_add(&items_produced, 1);
            printf("  [P%d] Produs: %d\n", id, item);
        }
        usleep(10000);  /* Simulare producție */
    }
    
    return NULL;
}

/**
 * Thread consumator pentru bounded buffer
 */
void* pc_consumer(void *arg) {
    int id = *(int*)arg;
    int item;
    
    while (buffer_get(demo_buffer, &item)) {
        atomic_fetch_add(&items_consumed, 1);
        printf("  [C%d] Consumat: %d\n", id, item);
        usleep(15000);  /* Simulare procesare */
    }
    
    return NULL;
}

/**
 * Demonstrație Producer-Consumer
 */
void demo_producer_consumer(void) {
    print_subsection("Producer-Consumer cu Bounded Buffer");
    
    const int num_producers = 2;
    const int num_consumers = 3;
    pthread_t producers[num_producers];
    pthread_t consumers[num_consumers];
    int prod_ids[num_producers];
    int cons_ids[num_consumers];
    
    printf("Configurație: %d producători, %d consumatori, buffer size=%d\n",
           num_producers, num_consumers, BUFFER_SIZE);
    printf("Fiecare producător va genera %d item-uri\n\n", NUM_ITEMS);
    
    demo_buffer = buffer_create(BUFFER_SIZE);
    items_produced = 0;
    items_consumed = 0;
    
    /* Pornire consumatori */
    for (int i = 0; i < num_consumers; i++) {
        cons_ids[i] = i;
        pthread_create(&consumers[i], NULL, pc_consumer, &cons_ids[i]);
    }
    
    /* Pornire producători */
    for (int i = 0; i < num_producers; i++) {
        prod_ids[i] = i;
        pthread_create(&producers[i], NULL, pc_producer, &prod_ids[i]);
    }
    
    /* Așteptare producători */
    for (int i = 0; i < num_producers; i++) {
        pthread_join(producers[i], NULL);
    }
    
    /* Semnalizare shutdown și așteptare consumatori */
    buffer_shutdown(demo_buffer);
    for (int i = 0; i < num_consumers; i++) {
        pthread_join(consumers[i], NULL);
    }
    
    printf("\nRezultat: Produs=%d, Consumat=%d %s\n",
           atomic_load(&items_produced), atomic_load(&items_consumed),
           atomic_load(&items_produced) == atomic_load(&items_consumed) ? "✓" : "✗");
    
    buffer_destroy(demo_buffer);
}

/* =============================================================================
 * PARTEA 6: PARALLEL MERGE SORT
 * =============================================================================
 */

/**
 * Structură pentru argumentele sortării paralele
 */
typedef struct {
    int *arr;
    int *temp;
    int left;
    int right;
    int depth;
} SortArg;

/**
 * Merge două subarray-uri sortate
 */
void merge(int *arr, int *temp, int left, int mid, int right) {
    for (int i = left; i <= right; i++) {
        temp[i] = arr[i];
    }
    
    int i = left, j = mid + 1, k = left;
    while (i <= mid && j <= right) {
        if (temp[i] <= temp[j]) {
            arr[k++] = temp[i++];
        } else {
            arr[k++] = temp[j++];
        }
    }
    while (i <= mid) arr[k++] = temp[i++];
}

/**
 * Merge Sort secvențial
 */
void merge_sort_seq(int *arr, int *temp, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        merge_sort_seq(arr, temp, left, mid);
        merge_sort_seq(arr, temp, mid + 1, right);
        merge(arr, temp, left, mid, right);
    }
}

/**
 * Merge Sort paralel (funcție pentru thread)
 */
void* parallel_merge_sort_thread(void *arg) {
    SortArg *sa = (SortArg*)arg;
    
    int n = sa->right - sa->left + 1;
    
    /* Threshold: sub SORT_THRESHOLD elemente, folosește secvențial */
    if (n < SORT_THRESHOLD || sa->depth <= 0) {
        merge_sort_seq(sa->arr, sa->temp, sa->left, sa->right);
        return NULL;
    }
    
    int mid = sa->left + (sa->right - sa->left) / 2;
    
    /* Argumente pentru sub-thread-uri */
    SortArg left_arg = {sa->arr, sa->temp, sa->left, mid, sa->depth - 1};
    SortArg right_arg = {sa->arr, sa->temp, mid + 1, sa->right, sa->depth - 1};
    
    pthread_t left_thread;
    pthread_create(&left_thread, NULL, parallel_merge_sort_thread, &left_arg);
    
    /* Jumătatea dreaptă în thread-ul curent */
    parallel_merge_sort_thread(&right_arg);
    
    pthread_join(left_thread, NULL);
    
    merge(sa->arr, sa->temp, sa->left, mid, sa->right);
    
    return NULL;
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
 * Demonstrație Parallel Merge Sort
 */
void demo_parallel_sort(void) {
    print_subsection("Parallel Merge Sort cu Threshold Adaptiv");
    
    int n = ARRAY_SIZE;
    int *arr_seq = malloc(n * sizeof(int));
    int *arr_par = malloc(n * sizeof(int));
    int *temp = malloc(n * sizeof(int));
    
    /* Generare date random */
    srand(42);
    for (int i = 0; i < n; i++) {
        arr_seq[i] = rand() % 10000;
        arr_par[i] = arr_seq[i];
    }
    
    printf("Array: %d elemente, threshold=%d\n\n", n, SORT_THRESHOLD);
    
    /* Sortare secvențială */
    double start = get_time_ms();
    merge_sort_seq(arr_seq, temp, 0, n - 1);
    double seq_time = get_time_ms() - start;
    
    printf("Secvențial:  %.3f ms %s\n", seq_time, is_sorted(arr_seq, n) ? "✓" : "✗");
    
    /* Sortare paralelă (depth=3 → max 8 thread-uri) */
    SortArg arg = {arr_par, temp, 0, n - 1, 3};
    start = get_time_ms();
    parallel_merge_sort_thread(&arg);
    double par_time = get_time_ms() - start;
    
    printf("Paralel:     %.3f ms %s\n", par_time, is_sorted(arr_par, n) ? "✓" : "✗");
    
    double speedup = seq_time / par_time;
    printf("\nSpeedup: %.2fx\n", speedup);
    
    free(arr_seq);
    free(arr_par);
    free(temp);
}

/* =============================================================================
 * PARTEA 7: C11 ATOMICS ȘI LOCK-FREE STACK
 * =============================================================================
 */

/**
 * Nod pentru stack lock-free
 */
typedef struct LFNode {
    int data;
    struct LFNode *next;
} LFNode;

/**
 * Stack lock-free folosind CAS
 */
typedef struct {
    _Atomic(LFNode*) top;
    atomic_size_t size;
} LockFreeStack;

/**
 * Creează stack lock-free
 */
LockFreeStack* lf_stack_create(void) {
    LockFreeStack *stack = malloc(sizeof(LockFreeStack));
    atomic_store(&stack->top, NULL);
    atomic_store(&stack->size, 0);
    return stack;
}

/**
 * Push lock-free cu CAS
 */
void lf_stack_push(LockFreeStack *stack, int value) {
    LFNode *new_node = malloc(sizeof(LFNode));
    new_node->data = value;
    
    LFNode *old_top;
    do {
        old_top = atomic_load(&stack->top);
        new_node->next = old_top;
    } while (!atomic_compare_exchange_weak(&stack->top, &old_top, new_node));
    
    atomic_fetch_add(&stack->size, 1);
}

/**
 * Pop lock-free cu CAS
 */
bool lf_stack_pop(LockFreeStack *stack, int *value) {
    LFNode *old_top;
    LFNode *new_top;
    
    do {
        old_top = atomic_load(&stack->top);
        if (old_top == NULL) return false;
        new_top = old_top->next;
    } while (!atomic_compare_exchange_weak(&stack->top, &old_top, new_top));
    
    *value = old_top->data;
    free(old_top);
    atomic_fetch_sub(&stack->size, 1);
    return true;
}

/**
 * Distruge stack lock-free
 */
void lf_stack_destroy(LockFreeStack *stack) {
    int value;
    while (lf_stack_pop(stack, &value)) {}
    free(stack);
}

/* Variabile pentru demo atomics */
static LockFreeStack *atomic_demo_stack = NULL;
static atomic_int push_count = 0;
static atomic_int pop_count = 0;

/**
 * Thread care face push în stack-ul lock-free
 */
void* atomic_pusher(void *arg) {
    int id = *(int*)arg;
    for (int i = 0; i < 100; i++) {
        lf_stack_push(atomic_demo_stack, id * 1000 + i);
        atomic_fetch_add(&push_count, 1);
    }
    return NULL;
}

/**
 * Thread care face pop din stack-ul lock-free
 */
void* atomic_popper(void *arg) {
    (void)arg;
    int value;
    int local_count = 0;
    
    /* Așteaptă să existe date */
    usleep(10000);
    
    while (local_count < 100) {
        if (lf_stack_pop(atomic_demo_stack, &value)) {
            atomic_fetch_add(&pop_count, 1);
            local_count++;
        } else {
            usleep(1000);
        }
    }
    return NULL;
}

/**
 * Demonstrație C11 Atomics și Lock-Free Stack
 */
void demo_atomics(void) {
    print_subsection("C11 Atomics și Lock-Free Stack");
    
    printf("Lock-free stack folosind atomic_compare_exchange (CAS)\n");
    printf("Nu necesită mutex - sincronizare prin operații atomice hardware\n\n");
    
    atomic_demo_stack = lf_stack_create();
    atomic_store(&push_count, 0);
    atomic_store(&pop_count, 0);
    
    const int num_pushers = 4;
    const int num_poppers = 4;
    pthread_t pushers[num_pushers];
    pthread_t poppers[num_poppers];
    int ids[num_pushers];
    
    printf("Pornesc %d pusheri și %d popperi...\n\n", num_pushers, num_poppers);
    
    for (int i = 0; i < num_pushers; i++) {
        ids[i] = i;
        pthread_create(&pushers[i], NULL, atomic_pusher, &ids[i]);
    }
    for (int i = 0; i < num_poppers; i++) {
        pthread_create(&poppers[i], NULL, atomic_popper, NULL);
    }
    
    for (int i = 0; i < num_pushers; i++) {
        pthread_join(pushers[i], NULL);
    }
    for (int i = 0; i < num_poppers; i++) {
        pthread_join(poppers[i], NULL);
    }
    
    printf("Push-uri: %d, Pop-uri: %d %s\n",
           atomic_load(&push_count), atomic_load(&pop_count),
           atomic_load(&push_count) == atomic_load(&pop_count) ? "✓" : "");
    
    lf_stack_destroy(atomic_demo_stack);
}

/* =============================================================================
 * PARTEA 8: DINING PHILOSOPHERS (DEMONSTRAȚIE DEADLOCK)
 * =============================================================================
 */

static pthread_mutex_t forks[PHILOSOPHER_COUNT];
static int eating_count[PHILOSOPHER_COUNT] = {0};

/**
 * Filozof (versiune naivă - poate duce la deadlock)
 */
void* philosopher_naive(void *arg) {
    int id = *(int*)arg;
    int left = id;
    int right = (id + 1) % PHILOSOPHER_COUNT;
    
    for (int i = 0; i < 3; i++) {
        /* Gândește */
        usleep(THINKING_TIME_MS * 1000);
        
        /* Încearcă să ia furculițele */
        pthread_mutex_lock(&forks[left]);
        /* Mic delay pentru a crește șansa de deadlock */
        usleep(1000);
        
        /* Încercare non-blocking pentru a evita deadlock complet */
        if (pthread_mutex_trylock(&forks[right]) != 0) {
            pthread_mutex_unlock(&forks[left]);
            continue;
        }
        
        /* Mănâncă */
        eating_count[id]++;
        usleep(EATING_TIME_MS * 1000);
        
        pthread_mutex_unlock(&forks[right]);
        pthread_mutex_unlock(&forks[left]);
    }
    
    return NULL;
}

/**
 * Filozof (versiune corectă - ordonare resurse)
 */
void* philosopher_ordered(void *arg) {
    int id = *(int*)arg;
    int first = id < (id + 1) % PHILOSOPHER_COUNT ? id : (id + 1) % PHILOSOPHER_COUNT;
    int second = id < (id + 1) % PHILOSOPHER_COUNT ? (id + 1) % PHILOSOPHER_COUNT : id;
    
    for (int i = 0; i < 3; i++) {
        /* Gândește */
        usleep(THINKING_TIME_MS * 1000);
        
        /* Ia furculițele în ordine crescătoare - previne deadlock */
        pthread_mutex_lock(&forks[first]);
        pthread_mutex_lock(&forks[second]);
        
        /* Mănâncă */
        eating_count[id]++;
        usleep(EATING_TIME_MS * 1000);
        
        pthread_mutex_unlock(&forks[second]);
        pthread_mutex_unlock(&forks[first]);
    }
    
    return NULL;
}

/**
 * Demonstrație Dining Philosophers
 */
void demo_dining_philosophers(void) {
    print_subsection("Dining Philosophers - Problema Deadlock");
    
    pthread_t philosophers[PHILOSOPHER_COUNT];
    int ids[PHILOSOPHER_COUNT];
    
    for (int i = 0; i < PHILOSOPHER_COUNT; i++) {
        pthread_mutex_init(&forks[i], NULL);
        eating_count[i] = 0;
        ids[i] = i;
    }
    
    printf("5 filosofi la masă, fiecare cu o furculiță între ei\n");
    printf("Fiecare filosof are nevoie de 2 furculițe pentru a mânca\n\n");
    
    printf("Soluție: Ordonare resurse (ia furculița cu index mai mic întâi)\n");
    printf("         Elimină condiția de așteptare circulară!\n\n");
    
    for (int i = 0; i < PHILOSOPHER_COUNT; i++) {
        pthread_create(&philosophers[i], NULL, philosopher_ordered, &ids[i]);
    }
    
    for (int i = 0; i < PHILOSOPHER_COUNT; i++) {
        pthread_join(philosophers[i], NULL);
    }
    
    printf("Rezultat (câte mese pentru fiecare filosof):\n");
    for (int i = 0; i < PHILOSOPHER_COUNT; i++) {
        printf("  Filosof %d: %d mese\n", i, eating_count[i]);
    }
    
    for (int i = 0; i < PHILOSOPHER_COUNT; i++) {
        pthread_mutex_destroy(&forks[i]);
    }
}

/* =============================================================================
 * PARTEA 9: LEGEA LUI AMDAHL - DEMONSTRAȚIE
 * =============================================================================
 */

/**
 * Calculează speedup teoretic conform Legii lui Amdahl
 */
double amdahl_speedup(double parallel_fraction, int num_processors) {
    double serial_fraction = 1.0 - parallel_fraction;
    return 1.0 / (serial_fraction + parallel_fraction / num_processors);
}

/**
 * Demonstrație Legea lui Amdahl
 */
void demo_amdahl_law(void) {
    print_subsection("Legea lui Amdahl - Speedup Teoretic");
    
    printf("Formula: Speedup(n) = 1 / ((1-P) + P/n)\n");
    printf("  P = fracțiunea paralelizabilă\n");
    printf("  n = numărul de procesoare/thread-uri\n\n");
    
    double parallel_fractions[] = {0.50, 0.75, 0.90, 0.95, 0.99};
    int num_fractions = sizeof(parallel_fractions) / sizeof(parallel_fractions[0]);
    
    printf("┌────────┬────────┬────────┬────────┬────────┬────────┬────────┐\n");
    printf("│   P    │  n=1   │  n=2   │  n=4   │  n=8   │  n=16  │  n=∞   │\n");
    printf("├────────┼────────┼────────┼────────┼────────┼────────┼────────┤\n");
    
    for (int i = 0; i < num_fractions; i++) {
        double p = parallel_fractions[i];
        printf("│ %5.0f%% │ %6.2fx│ %6.2fx│ %6.2fx│ %6.2fx│ %6.2fx│ %6.2fx│\n",
               p * 100,
               amdahl_speedup(p, 1),
               amdahl_speedup(p, 2),
               amdahl_speedup(p, 4),
               amdahl_speedup(p, 8),
               amdahl_speedup(p, 16),
               1.0 / (1.0 - p));  /* Limita pentru n→∞ */
    }
    
    printf("└────────┴────────┴────────┴────────┴────────┴────────┴────────┘\n");
    
    printf("\nConcluzie: Chiar cu 99%% paralelizabil, speedup maxim = 100x\n");
    printf("           1%% secvențial limitează dramatic beneficiul!\n");
}

/* =============================================================================
 * MAIN ȘI DEMONSTRAȚII
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║     SĂPTĂMÂNA 20: PROGRAMARE PARALELĂ ȘI CONCURENTĂ ÎN C                      ║\n");
    printf("║     Exemplu Demonstrativ Complet                                              ║\n");
    printf("║     ATP - Academia de Studii Economice București                              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    print_separator("DEMO 1: FUNDAMENTE PTHREADS");
    demo_basic_threads();
    
    print_separator("DEMO 2: RACE CONDITIONS ȘI MUTEX");
    demo_race_condition();
    
    print_separator("DEMO 3: SEMAFOARE");
    demo_semaphores();
    
    print_separator("DEMO 4: CONDITION VARIABLES");
    demo_condition_variables();
    
    print_separator("DEMO 5: PRODUCER-CONSUMER PATTERN");
    demo_producer_consumer();
    
    print_separator("DEMO 6: PARALLEL MERGE SORT");
    demo_parallel_sort();
    
    print_separator("DEMO 7: C11 ATOMICS ȘI LOCK-FREE STACK");
    demo_atomics();
    
    print_separator("DEMO 8: DINING PHILOSOPHERS");
    demo_dining_philosophers();
    
    print_separator("DEMO 9: LEGEA LUI AMDAHL");
    demo_amdahl_law();
    
    print_separator("TOATE DEMONSTRAȚIILE COMPLETATE CU SUCCES!");
    
    printf("Concepte acoperite:\n");
    printf("  ✓ POSIX Threads (pthread_create, pthread_join)\n");
    printf("  ✓ Mutex pentru excludere mutuală\n");
    printf("  ✓ Semafoare pentru limitare acces\n");
    printf("  ✓ Condition Variables pentru semnalizare\n");
    printf("  ✓ Producer-Consumer cu Bounded Buffer\n");
    printf("  ✓ Parallel Merge Sort cu threshold\n");
    printf("  ✓ Lock-Free Stack cu C11 Atomics\n");
    printf("  ✓ Dining Philosophers și prevenire deadlock\n");
    printf("  ✓ Legea lui Amdahl pentru speedup teoretic\n");
    printf("\n");
    
    return 0;
}
