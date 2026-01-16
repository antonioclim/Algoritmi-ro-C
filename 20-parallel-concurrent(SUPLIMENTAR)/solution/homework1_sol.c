/**
 * =============================================================================
 * SOLUȚIE TEMA 1: Thread Pool cu Futures
 * =============================================================================
 *
 * Implementare completă a unui Thread Pool generic cu suport pentru Futures.
 * Permite submiterea de task-uri și așteptarea rezultatelor în mod asincron.
 *
 * Caracteristici:
 *   - Pool configurabil de worker threads
 *   - Coadă thread-safe pentru task-uri
 *   - Future handles pentru obținerea rezultatelor
 *   - Suport pentru anularea task-urilor pending
 *   - Shutdown grațios
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -o homework1_sol homework1_sol.c -lpthread
 *
 * =============================================================================
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

/* =============================================================================
 * CONSTANTE
 * =============================================================================
 */

#define DEFAULT_POOL_SIZE   4
#define DEFAULT_QUEUE_SIZE  64
#define TASK_TIMEOUT_MS     5000

/* =============================================================================
 * STĂRI FUTURE
 * =============================================================================
 */

typedef enum {
    FUTURE_PENDING,     /* Task în coadă, neexecutat */
    FUTURE_RUNNING,     /* Task în execuție */
    FUTURE_COMPLETED,   /* Task completat cu succes */
    FUTURE_CANCELLED,   /* Task anulat */
    FUTURE_ERROR        /* Task eșuat */
} FutureState;

/* =============================================================================
 * STRUCTURI DE DATE
 * =============================================================================
 */

/**
 * Task - unitate de lucru submitabilă la pool
 */
typedef struct {
    void* (*function)(void*);   /* Funcția de executat */
    void *arg;                  /* Argumentul funcției */
    struct Future *future;      /* Future asociat */
} Task;

/**
 * Future - handle pentru rezultatul unui task
 */
typedef struct Future {
    FutureState state;          /* Starea curentă */
    void *result;               /* Rezultatul (dacă completed) */
    char error_msg[256];        /* Mesaj eroare (dacă error) */
    
    pthread_mutex_t mutex;
    pthread_cond_t completed;   /* Semnalare când task-ul e gata */
    
    atomic_int ref_count;       /* Reference counting pentru cleanup */
} Future;

/**
 * Coadă circulară thread-safe pentru task-uri
 */
typedef struct {
    Task *tasks;
    size_t capacity;
    size_t count;
    size_t head;
    size_t tail;
    
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} TaskQueue;

/**
 * Thread Pool principal
 */
typedef struct {
    pthread_t *workers;         /* Array de worker threads */
    size_t num_workers;
    
    TaskQueue *queue;           /* Coada de task-uri */
    
    volatile bool shutdown;     /* Flag pentru oprire */
    volatile bool immediate;    /* Oprire imediată (fără flush) */
    
    /* Statistici */
    atomic_int tasks_submitted;
    atomic_int tasks_completed;
    atomic_int tasks_cancelled;
} ThreadPool;

/* =============================================================================
 * FUNCȚII HELPER
 * =============================================================================
 */

double get_time_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1000000.0;
}

/* =============================================================================
 * IMPLEMENTARE FUTURE
 * =============================================================================
 */

/**
 * Creează un Future nou
 */
Future* future_create(void) {
    Future *f = malloc(sizeof(Future));
    if (!f) return NULL;
    
    f->state = FUTURE_PENDING;
    f->result = NULL;
    f->error_msg[0] = '\0';
    atomic_init(&f->ref_count, 1);
    
    pthread_mutex_init(&f->mutex, NULL);
    pthread_cond_init(&f->completed, NULL);
    
    return f;
}

/**
 * Incrementează reference count
 */
void future_ref(Future *f) {
    atomic_fetch_add(&f->ref_count, 1);
}

/**
 * Decrementează reference count și eliberează dacă 0
 */
void future_unref(Future *f) {
    if (atomic_fetch_sub(&f->ref_count, 1) == 1) {
        pthread_mutex_destroy(&f->mutex);
        pthread_cond_destroy(&f->completed);
        free(f);
    }
}

/**
 * Așteaptă completarea unui Future și returnează rezultatul
 * Blochează până când task-ul e gata sau e anulat
 *
 * @param f Future de așteptat
 * @return Rezultatul task-ului sau NULL dacă anulat/eroare
 */
void* future_get(Future *f) {
    pthread_mutex_lock(&f->mutex);
    
    while (f->state == FUTURE_PENDING || f->state == FUTURE_RUNNING) {
        pthread_cond_wait(&f->completed, &f->mutex);
    }
    
    void *result = f->result;
    FutureState state = f->state;
    
    pthread_mutex_unlock(&f->mutex);
    
    if (state == FUTURE_COMPLETED) {
        return result;
    } else if (state == FUTURE_ERROR) {
        fprintf(stderr, "Task error: %s\n", f->error_msg);
        return NULL;
    } else {
        return NULL;  /* CANCELLED */
    }
}

/**
 * Așteaptă cu timeout
 *
 * @param f Future de așteptat
 * @param timeout_ms Timeout în milisecunde
 * @return Rezultatul sau NULL dacă timeout
 */
void* future_get_timeout(Future *f, int timeout_ms) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += timeout_ms / 1000;
    ts.tv_nsec += (timeout_ms % 1000) * 1000000;
    if (ts.tv_nsec >= 1000000000) {
        ts.tv_sec++;
        ts.tv_nsec -= 1000000000;
    }
    
    pthread_mutex_lock(&f->mutex);
    
    while (f->state == FUTURE_PENDING || f->state == FUTURE_RUNNING) {
        int rc = pthread_cond_timedwait(&f->completed, &f->mutex, &ts);
        if (rc == ETIMEDOUT) {
            pthread_mutex_unlock(&f->mutex);
            return NULL;  /* Timeout */
        }
    }
    
    void *result = f->result;
    pthread_mutex_unlock(&f->mutex);
    
    return result;
}

/**
 * Verifică dacă Future e completat (non-blocking)
 */
bool future_is_done(Future *f) {
    pthread_mutex_lock(&f->mutex);
    bool done = (f->state != FUTURE_PENDING && f->state != FUTURE_RUNNING);
    pthread_mutex_unlock(&f->mutex);
    return done;
}

/**
 * Marchează Future ca completat cu rezultat
 */
void future_complete(Future *f, void *result) {
    pthread_mutex_lock(&f->mutex);
    f->state = FUTURE_COMPLETED;
    f->result = result;
    pthread_cond_broadcast(&f->completed);
    pthread_mutex_unlock(&f->mutex);
}

/**
 * Marchează Future ca anulat
 */
bool future_cancel(Future *f) {
    pthread_mutex_lock(&f->mutex);
    
    if (f->state == FUTURE_PENDING) {
        f->state = FUTURE_CANCELLED;
        pthread_cond_broadcast(&f->completed);
        pthread_mutex_unlock(&f->mutex);
        return true;
    }
    
    pthread_mutex_unlock(&f->mutex);
    return false;  /* Deja în execuție sau completat */
}

/* =============================================================================
 * IMPLEMENTARE TASK QUEUE
 * =============================================================================
 */

TaskQueue* queue_create(size_t capacity) {
    TaskQueue *q = malloc(sizeof(TaskQueue));
    if (!q) return NULL;
    
    q->tasks = malloc(capacity * sizeof(Task));
    if (!q->tasks) {
        free(q);
        return NULL;
    }
    
    q->capacity = capacity;
    q->count = 0;
    q->head = 0;
    q->tail = 0;
    
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->not_empty, NULL);
    pthread_cond_init(&q->not_full, NULL);
    
    return q;
}

void queue_destroy(TaskQueue *q) {
    if (!q) return;
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->not_empty);
    pthread_cond_destroy(&q->not_full);
    free(q->tasks);
    free(q);
}

bool queue_push(TaskQueue *q, Task *task, bool block) {
    pthread_mutex_lock(&q->mutex);
    
    while (q->count == q->capacity) {
        if (!block) {
            pthread_mutex_unlock(&q->mutex);
            return false;
        }
        pthread_cond_wait(&q->not_full, &q->mutex);
    }
    
    q->tasks[q->tail] = *task;
    q->tail = (q->tail + 1) % q->capacity;
    q->count++;
    
    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->mutex);
    
    return true;
}

bool queue_pop(TaskQueue *q, Task *task, bool block) {
    pthread_mutex_lock(&q->mutex);
    
    while (q->count == 0) {
        if (!block) {
            pthread_mutex_unlock(&q->mutex);
            return false;
        }
        pthread_cond_wait(&q->not_empty, &q->mutex);
    }
    
    *task = q->tasks[q->head];
    q->head = (q->head + 1) % q->capacity;
    q->count--;
    
    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->mutex);
    
    return true;
}

void queue_signal_all(TaskQueue *q) {
    pthread_mutex_lock(&q->mutex);
    pthread_cond_broadcast(&q->not_empty);
    pthread_mutex_unlock(&q->mutex);
}

/* =============================================================================
 * IMPLEMENTARE THREAD POOL
 * =============================================================================
 */

/**
 * Funcția executată de worker threads
 */
void* worker_function(void *arg) {
    ThreadPool *pool = (ThreadPool*)arg;
    
    while (true) {
        Task task;
        
        pthread_mutex_lock(&pool->queue->mutex);
        
        /* Așteaptă task sau shutdown */
        while (pool->queue->count == 0 && !pool->shutdown) {
            pthread_cond_wait(&pool->queue->not_empty, &pool->queue->mutex);
        }
        
        /* Verifică shutdown */
        if (pool->shutdown && (pool->immediate || pool->queue->count == 0)) {
            pthread_mutex_unlock(&pool->queue->mutex);
            break;
        }
        
        /* Extrage task */
        if (pool->queue->count > 0) {
            task = pool->queue->tasks[pool->queue->head];
            pool->queue->head = (pool->queue->head + 1) % pool->queue->capacity;
            pool->queue->count--;
            pthread_cond_signal(&pool->queue->not_full);
        }
        
        pthread_mutex_unlock(&pool->queue->mutex);
        
        /* Execută task-ul */
        if (task.function) {
            /* Marchează ca running */
            if (task.future) {
                pthread_mutex_lock(&task.future->mutex);
                if (task.future->state == FUTURE_PENDING) {
                    task.future->state = FUTURE_RUNNING;
                }
                pthread_mutex_unlock(&task.future->mutex);
            }
            
            /* Verifică dacă a fost anulat */
            if (task.future && task.future->state == FUTURE_CANCELLED) {
                atomic_fetch_add(&pool->tasks_cancelled, 1);
                future_unref(task.future);
                continue;
            }
            
            /* Execută funcția */
            void *result = task.function(task.arg);
            
            /* Completează future */
            if (task.future) {
                future_complete(task.future, result);
                future_unref(task.future);
            }
            
            atomic_fetch_add(&pool->tasks_completed, 1);
        }
    }
    
    return NULL;
}

/**
 * Creează un Thread Pool nou
 */
ThreadPool* threadpool_create(size_t num_workers, size_t queue_size) {
    ThreadPool *pool = malloc(sizeof(ThreadPool));
    if (!pool) return NULL;
    
    pool->num_workers = num_workers;
    pool->shutdown = false;
    pool->immediate = false;
    
    atomic_init(&pool->tasks_submitted, 0);
    atomic_init(&pool->tasks_completed, 0);
    atomic_init(&pool->tasks_cancelled, 0);
    
    pool->queue = queue_create(queue_size);
    if (!pool->queue) {
        free(pool);
        return NULL;
    }
    
    pool->workers = malloc(num_workers * sizeof(pthread_t));
    if (!pool->workers) {
        queue_destroy(pool->queue);
        free(pool);
        return NULL;
    }
    
    /* Pornește worker threads */
    for (size_t i = 0; i < num_workers; i++) {
        if (pthread_create(&pool->workers[i], NULL, worker_function, pool) != 0) {
            /* Cleanup on error */
            pool->shutdown = true;
            pool->immediate = true;
            queue_signal_all(pool->queue);
            for (size_t j = 0; j < i; j++) {
                pthread_join(pool->workers[j], NULL);
            }
            free(pool->workers);
            queue_destroy(pool->queue);
            free(pool);
            return NULL;
        }
    }
    
    return pool;
}

/**
 * Submite un task la pool și returnează Future handle
 */
Future* threadpool_submit(ThreadPool *pool, void* (*function)(void*), void *arg) {
    if (pool->shutdown) return NULL;
    
    Future *f = future_create();
    if (!f) return NULL;
    
    future_ref(f);  /* Reference pentru pool */
    
    Task task = {
        .function = function,
        .arg = arg,
        .future = f
    };
    
    if (!queue_push(pool->queue, &task, true)) {
        future_unref(f);
        future_unref(f);
        return NULL;
    }
    
    atomic_fetch_add(&pool->tasks_submitted, 1);
    
    return f;
}

/**
 * Oprește pool-ul
 *
 * @param pool Thread pool de oprit
 * @param immediate True pentru oprire imediată, false pentru flush queue
 */
void threadpool_shutdown(ThreadPool *pool, bool immediate) {
    pool->immediate = immediate;
    pool->shutdown = true;
    
    queue_signal_all(pool->queue);
    
    for (size_t i = 0; i < pool->num_workers; i++) {
        pthread_join(pool->workers[i], NULL);
    }
    
    /* Cleanup task-uri rămase dacă immediate */
    if (immediate) {
        Task task;
        while (queue_pop(pool->queue, &task, false)) {
            if (task.future) {
                future_cancel(task.future);
                future_unref(task.future);
            }
        }
    }
    
    free(pool->workers);
    queue_destroy(pool->queue);
    free(pool);
}

/* =============================================================================
 * DEMONSTRAȚIE ȘI TESTE
 * =============================================================================
 */

/* Task de exemplu: calculează factorial */
void* compute_factorial(void *arg) {
    int n = *(int*)arg;
    long long *result = malloc(sizeof(long long));
    *result = 1;
    for (int i = 2; i <= n; i++) {
        *result *= i;
    }
    usleep(100000);  /* Simulează procesare */
    return result;
}

/* Task de exemplu: calculează Fibonacci */
void* compute_fibonacci(void *arg) {
    int n = *(int*)arg;
    long long *result = malloc(sizeof(long long));
    
    if (n <= 1) {
        *result = n;
    } else {
        long long a = 0, b = 1;
        for (int i = 2; i <= n; i++) {
            long long temp = a + b;
            a = b;
            b = temp;
        }
        *result = b;
    }
    usleep(50000);
    return result;
}

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║     SOLUȚIE TEMA 1: Thread Pool cu Futures                                    ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    /* Creează pool */
    ThreadPool *pool = threadpool_create(DEFAULT_POOL_SIZE, DEFAULT_QUEUE_SIZE);
    if (!pool) {
        fprintf(stderr, "Eroare la crearea pool-ului\n");
        return 1;
    }
    
    printf("Thread pool creat cu %d workers\n\n", DEFAULT_POOL_SIZE);
    
    /* Submite task-uri */
    int nums[] = {5, 10, 12, 15, 20, 8, 6, 25, 30, 35};
    int n_tasks = sizeof(nums) / sizeof(nums[0]);
    Future **futures = malloc(n_tasks * sizeof(Future*));
    
    double start = get_time_ms();
    
    printf("Submitere task-uri...\n");
    for (int i = 0; i < n_tasks; i++) {
        futures[i] = threadpool_submit(pool, compute_factorial, &nums[i]);
        printf("  Task %d: factorial(%d) submis\n", i, nums[i]);
    }
    
    /* Colectează rezultate */
    printf("\nAșteptare rezultate...\n");
    for (int i = 0; i < n_tasks; i++) {
        long long *result = (long long*)future_get(futures[i]);
        if (result) {
            printf("  factorial(%d) = %lld\n", nums[i], *result);
            free(result);
        }
        future_unref(futures[i]);
    }
    
    double elapsed = get_time_ms() - start;
    
    printf("\n");
    printf("┌─────────────────────────────────────────────────────────────────┐\n");
    printf("│ STATISTICI                                                      │\n");
    printf("├─────────────────────────────────────────────────────────────────┤\n");
    printf("│ Task-uri submise:     %10d                               │\n", 
           atomic_load(&pool->tasks_submitted));
    printf("│ Task-uri completate:  %10d                               │\n",
           atomic_load(&pool->tasks_completed));
    printf("│ Task-uri anulate:     %10d                               │\n",
           atomic_load(&pool->tasks_cancelled));
    printf("│ Timp total:           %10.2f ms                           │\n", elapsed);
    printf("└─────────────────────────────────────────────────────────────────┘\n");
    
    free(futures);
    
    /* Shutdown */
    printf("\nShutdown pool...\n");
    threadpool_shutdown(pool, false);
    
    printf("\n✓ Demonstrație completată!\n\n");
    
    return 0;
}
