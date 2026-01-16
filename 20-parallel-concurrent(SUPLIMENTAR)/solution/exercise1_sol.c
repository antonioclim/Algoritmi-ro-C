/**
 * =============================================================================
 * SOLUȚIE EXERCIȚIUL 1: Log Processor cu Producer-Consumer
 * =============================================================================
 *
 * Acest fișier conține soluția completă pentru exercițiul 1.
 * Demonstrează implementarea corectă a pattern-ului producer-consumer
 * cu bounded buffer thread-safe.
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -o exercise1_sol exercise1_sol.c -lpthread
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

/* =============================================================================
 * CONSTANTE ȘI CONFIGURARE
 * =============================================================================
 */

#define MAX_LINE_LENGTH     256
#define DEFAULT_BUFFER_SIZE 20
#define DEFAULT_PRODUCERS   2
#define DEFAULT_CONSUMERS   4
#define SIMULATED_LINES     50

/* =============================================================================
 * STRUCTURI DE DATE
 * =============================================================================
 */

/**
 * Structură pentru o linie de log
 */
typedef struct {
    char content[MAX_LINE_LENGTH];
    int line_number;
    int producer_id;
} LogLine;

/**
 * SOLUȚIE TODO 1: Bounded Buffer complet
 *
 * Structură thread-safe pentru comunicația dintre producători și consumatori.
 * Folosește buffer circular cu mutex și condition variables pentru sincronizare.
 */
typedef struct {
    LogLine *buffer;            /* Array circular de linii */
    size_t capacity;            /* Capacitatea maximă */
    size_t count;               /* Numărul curent de elemente */
    size_t head;                /* Index pentru extragere (consumator) */
    size_t tail;                /* Index pentru inserare (producător) */
    
    pthread_mutex_t mutex;      /* Mutex pentru acces exclusiv */
    pthread_cond_t not_full;    /* Condiție: buffer nu mai e plin */
    pthread_cond_t not_empty;   /* Condiție: buffer nu mai e gol */
    
    bool shutdown;              /* Flag pentru oprire grațioasă */
    atomic_int done_producers;  /* Câți producători au terminat */
    int total_producers;        /* Total producători așteptați */
} BoundedBuffer;

/**
 * Structură pentru argumentele producătorilor
 */
typedef struct {
    int id;
    BoundedBuffer *buffer;
    char *filename;
    int start_line;
    int end_line;
    /* Statistici */
    int lines_produced;
    double time_spent_ms;
} ProducerArg;

/**
 * Structură pentru argumentele consumatorilor
 */
typedef struct {
    int id;
    BoundedBuffer *buffer;
    /* Statistici */
    int lines_consumed;
    int error_lines;
    int warning_lines;
    int info_lines;
    double time_spent_ms;
} ConsumerArg;

/* =============================================================================
 * VARIABILE GLOBALE PENTRU STATISTICI
 * =============================================================================
 */

static atomic_int total_produced = 0;
static atomic_int total_consumed = 0;
static pthread_mutex_t stats_mutex = PTHREAD_MUTEX_INITIALIZER;

/* =============================================================================
 * FUNCȚII HELPER
 * =============================================================================
 */

/**
 * Obține timestamp în milisecunde pentru măsurători de performanță
 */
double get_time_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1000000.0;
}

/**
 * Determină tipul liniei de log bazat pe conținut
 * Caută keyword-uri specifice în text
 *
 * @param line Linia de analizat
 * @return 'E' pentru ERROR, 'W' pentru WARNING, 'I' pentru INFO
 */
char get_log_level(const char *line) {
    if (strstr(line, "ERROR") != NULL || strstr(line, "CRITICAL") != NULL) {
        return 'E';
    }
    if (strstr(line, "WARNING") != NULL || strstr(line, "WARN") != NULL) {
        return 'W';
    }
    return 'I';  /* Default: INFO */
}

/**
 * Generează o linie de log simulată cu timestamp și nivel aleator
 */
void generate_simulated_line(char *dest, int line_num) {
    const char *levels[] = {"INFO", "WARNING", "ERROR"};
    const char *messages[] = {
        "Application started successfully",
        "Processing request from user",
        "Database query executed",
        "Cache miss for key",
        "Connection timeout detected",
        "Memory usage threshold exceeded",
        "Failed to parse configuration",
        "Authentication attempt failed",
        "Service health check passed",
        "Background job completed"
    };
    
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);
    
    int level_idx = rand() % 3;
    int msg_idx = rand() % 10;
    
    /* Biasează către INFO (70%), WARNING (20%), ERROR (10%) */
    int r = rand() % 100;
    if (r < 70) level_idx = 0;
    else if (r < 90) level_idx = 1;
    else level_idx = 2;
    
    snprintf(dest, MAX_LINE_LENGTH, "[%s] [%s] Line %d: %s",
             timestamp, levels[level_idx], line_num, messages[msg_idx]);
}

/* =============================================================================
 * SOLUȚIE TODO 2: IMPLEMENTAREA FUNCȚIILOR BUFFER
 * =============================================================================
 */

/**
 * Creează un bounded buffer nou cu capacitatea specificată
 *
 * @param capacity Numărul maxim de elemente în buffer
 * @param total_producers Numărul de producători (pentru terminare grațioasă)
 * @return Pointer la buffer-ul creat sau NULL la eroare
 *
 * Implementare:
 *   1. Alocă structura BoundedBuffer
 *   2. Alocă array-ul intern pentru LogLine
 *   3. Inițializează primitive de sincronizare (mutex, condvars)
 *   4. Setează valorile inițiale pentru contoare
 */
BoundedBuffer* buffer_create(size_t capacity, int total_producers) {
    BoundedBuffer *bb = malloc(sizeof(BoundedBuffer));
    if (!bb) {
        perror("malloc BoundedBuffer");
        return NULL;
    }
    
    bb->buffer = malloc(capacity * sizeof(LogLine));
    if (!bb->buffer) {
        perror("malloc buffer array");
        free(bb);
        return NULL;
    }
    
    bb->capacity = capacity;
    bb->count = 0;
    bb->head = 0;
    bb->tail = 0;
    bb->shutdown = false;
    atomic_init(&bb->done_producers, 0);
    bb->total_producers = total_producers;
    
    /* Inițializare primitive POSIX */
    if (pthread_mutex_init(&bb->mutex, NULL) != 0) {
        perror("pthread_mutex_init");
        free(bb->buffer);
        free(bb);
        return NULL;
    }
    
    if (pthread_cond_init(&bb->not_full, NULL) != 0) {
        perror("pthread_cond_init not_full");
        pthread_mutex_destroy(&bb->mutex);
        free(bb->buffer);
        free(bb);
        return NULL;
    }
    
    if (pthread_cond_init(&bb->not_empty, NULL) != 0) {
        perror("pthread_cond_init not_empty");
        pthread_cond_destroy(&bb->not_full);
        pthread_mutex_destroy(&bb->mutex);
        free(bb->buffer);
        free(bb);
        return NULL;
    }
    
    return bb;
}

/**
 * Distruge bounded buffer și eliberează toate resursele
 *
 * @param bb Pointer la buffer-ul de distrus
 */
void buffer_destroy(BoundedBuffer *bb) {
    if (!bb) return;
    
    pthread_cond_destroy(&bb->not_empty);
    pthread_cond_destroy(&bb->not_full);
    pthread_mutex_destroy(&bb->mutex);
    free(bb->buffer);
    free(bb);
}

/**
 * Adaugă o linie în buffer (operație blocantă dacă buffer-ul e plin)
 *
 * @param bb Pointer la buffer
 * @param line Pointer la linia de adăugat
 * @return true dacă s-a adăugat cu succes, false dacă s-a cerut shutdown
 *
 * Implementare (corectă pentru spurious wakeups):
 *   - Folosește WHILE (nu IF) pentru verificarea condiției
 *   - Verifică shutdown după fiecare trezire
 *   - Semnalizează consumatorii după inserare
 */
bool buffer_put(BoundedBuffer *bb, LogLine *line) {
    pthread_mutex_lock(&bb->mutex);
    
    /* IMPORTANT: while loop pentru spurious wakeups! */
    while (bb->count == bb->capacity && !bb->shutdown) {
        pthread_cond_wait(&bb->not_full, &bb->mutex);
    }
    
    /* Verifică dacă s-a cerut oprire */
    if (bb->shutdown) {
        pthread_mutex_unlock(&bb->mutex);
        return false;
    }
    
    /* Inserează în buffer circular la poziția tail */
    bb->buffer[bb->tail] = *line;
    bb->tail = (bb->tail + 1) % bb->capacity;
    bb->count++;
    
    /* Semnalizează un consumator care așteaptă */
    pthread_cond_signal(&bb->not_empty);
    pthread_mutex_unlock(&bb->mutex);
    
    return true;
}

/**
 * Extrage o linie din buffer (operație blocantă dacă buffer-ul e gol)
 *
 * @param bb Pointer la buffer
 * @param line Pointer unde se copiază linia extrasă
 * @return true dacă s-a extras cu succes, false dacă nu mai sunt date
 *
 * Implementare:
 *   - Așteaptă cât timp buffer-ul e gol
 *   - Verifică dacă toți producătorii au terminat
 *   - Returnează false dacă gol și toți producătorii au terminat
 */
bool buffer_get(BoundedBuffer *bb, LogLine *line) {
    pthread_mutex_lock(&bb->mutex);
    
    /* Așteaptă cât timp buffer-ul e gol */
    while (bb->count == 0) {
        /* Verifică dacă toți producătorii au terminat */
        if (atomic_load(&bb->done_producers) >= bb->total_producers) {
            pthread_mutex_unlock(&bb->mutex);
            return false;  /* Nu mai vin date */
        }
        pthread_cond_wait(&bb->not_empty, &bb->mutex);
    }
    
    /* Extrage din buffer circular de la poziția head */
    *line = bb->buffer[bb->head];
    bb->head = (bb->head + 1) % bb->capacity;
    bb->count--;
    
    /* Semnalizează un producător care așteaptă */
    pthread_cond_signal(&bb->not_full);
    pthread_mutex_unlock(&bb->mutex);
    
    return true;
}

/**
 * Marchează un producător ca terminat
 * Când toți producătorii au terminat, semnalizează toți consumatorii
 *
 * @param bb Pointer la buffer
 */
void buffer_producer_done(BoundedBuffer *bb) {
    pthread_mutex_lock(&bb->mutex);
    
    int done = atomic_fetch_add(&bb->done_producers, 1) + 1;
    
    /* Dacă toți producătorii au terminat, trezește toți consumatorii */
    if (done >= bb->total_producers) {
        pthread_cond_broadcast(&bb->not_empty);
    }
    
    pthread_mutex_unlock(&bb->mutex);
}

/* =============================================================================
 * SOLUȚIE TODO 3: IMPLEMENTAREA THREAD-URILOR PRODUCER ȘI CONSUMER
 * =============================================================================
 */

/**
 * Funcția executată de thread-urile producător
 *
 * Comportament:
 *   - Generează/citește linii de log
 *   - Le introduce în buffer
 *   - Colectează statistici
 *   - Semnalizează când a terminat
 */
void* producer_thread(void *arg) {
    ProducerArg *parg = (ProducerArg*)arg;
    double start_time = get_time_ms();
    
    printf("  [Producer %d] Started (lines %d-%d)\n", 
           parg->id, parg->start_line, parg->end_line);
    
    for (int i = parg->start_line; i <= parg->end_line; i++) {
        LogLine line;
        line.line_number = i;
        line.producer_id = parg->id;
        
        /* Generează linie simulată (sau citește din fișier) */
        generate_simulated_line(line.content, i);
        
        /* Încearcă să adauge în buffer */
        if (!buffer_put(parg->buffer, &line)) {
            printf("  [Producer %d] Shutdown during put\n", parg->id);
            break;
        }
        
        parg->lines_produced++;
        atomic_fetch_add(&total_produced, 1);
        
        /* Simulează timp de procesare variabil */
        usleep((rand() % 100) * 10);  /* 0-1ms */
    }
    
    /* Marchează producătorul ca terminat */
    buffer_producer_done(parg->buffer);
    
    parg->time_spent_ms = get_time_ms() - start_time;
    
    printf("  [Producer %d] Finished: %d lines in %.2f ms\n",
           parg->id, parg->lines_produced, parg->time_spent_ms);
    
    return NULL;
}

/**
 * Funcția executată de thread-urile consumator
 *
 * Comportament:
 *   - Extrage linii din buffer
 *   - Clasifică după nivel (ERROR/WARNING/INFO)
 *   - Colectează statistici
 *   - Se oprește când nu mai sunt date
 */
void* consumer_thread(void *arg) {
    ConsumerArg *carg = (ConsumerArg*)arg;
    double start_time = get_time_ms();
    LogLine line;
    
    printf("  [Consumer %d] Started\n", carg->id);
    
    while (buffer_get(carg->buffer, &line)) {
        /* Clasifică linia după nivel */
        char level = get_log_level(line.content);
        
        switch (level) {
            case 'E': carg->error_lines++; break;
            case 'W': carg->warning_lines++; break;
            case 'I': carg->info_lines++; break;
        }
        
        carg->lines_consumed++;
        atomic_fetch_add(&total_consumed, 1);
        
        /* Debug: afișează liniile de eroare */
        if (level == 'E') {
            printf("  [Consumer %d] ERROR detected: %s\n", 
                   carg->id, line.content);
        }
        
        /* Simulează timp de procesare */
        usleep((rand() % 200) * 10);  /* 0-2ms */
    }
    
    carg->time_spent_ms = get_time_ms() - start_time;
    
    printf("  [Consumer %d] Finished: %d lines (E:%d W:%d I:%d) in %.2f ms\n",
           carg->id, carg->lines_consumed,
           carg->error_lines, carg->warning_lines, carg->info_lines,
           carg->time_spent_ms);
    
    return NULL;
}

/* =============================================================================
 * FUNCȚII DE RAPORTARE
 * =============================================================================
 */

/**
 * Afișează statistici finale
 */
void print_statistics(ProducerArg *producers, int num_producers,
                      ConsumerArg *consumers, int num_consumers,
                      double total_time_ms) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                           STATISTICI FINALE                                    ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    /* Statistici producători */
    printf("┌─────────────────────────────────────────────────────────────────┐\n");
    printf("│ PRODUCĂTORI                                                     │\n");
    printf("├──────────┬──────────────┬──────────────────────────────────────┤\n");
    printf("│ Thread   │ Linii        │ Timp (ms)                            │\n");
    printf("├──────────┼──────────────┼──────────────────────────────────────┤\n");
    
    int total_prod_lines = 0;
    for (int i = 0; i < num_producers; i++) {
        printf("│ P%-7d │ %12d │ %38.2f │\n",
               producers[i].id, producers[i].lines_produced, 
               producers[i].time_spent_ms);
        total_prod_lines += producers[i].lines_produced;
    }
    printf("├──────────┼──────────────┼──────────────────────────────────────┤\n");
    printf("│ TOTAL    │ %12d │                                      │\n", total_prod_lines);
    printf("└──────────┴──────────────┴──────────────────────────────────────┘\n");
    
    /* Statistici consumatori */
    printf("\n┌─────────────────────────────────────────────────────────────────┐\n");
    printf("│ CONSUMATORI                                                     │\n");
    printf("├──────────┬──────────────┬───────┬────────┬──────┬──────────────┤\n");
    printf("│ Thread   │ Linii        │ ERROR │ WARN   │ INFO │ Timp (ms)    │\n");
    printf("├──────────┼──────────────┼───────┼────────┼──────┼──────────────┤\n");
    
    int total_cons_lines = 0;
    int total_errors = 0, total_warnings = 0, total_info = 0;
    for (int i = 0; i < num_consumers; i++) {
        printf("│ C%-7d │ %12d │ %5d │ %6d │ %4d │ %12.2f │\n",
               consumers[i].id, consumers[i].lines_consumed,
               consumers[i].error_lines, consumers[i].warning_lines,
               consumers[i].info_lines, consumers[i].time_spent_ms);
        total_cons_lines += consumers[i].lines_consumed;
        total_errors += consumers[i].error_lines;
        total_warnings += consumers[i].warning_lines;
        total_info += consumers[i].info_lines;
    }
    printf("├──────────┼──────────────┼───────┼────────┼──────┼──────────────┤\n");
    printf("│ TOTAL    │ %12d │ %5d │ %6d │ %4d │              │\n",
           total_cons_lines, total_errors, total_warnings, total_info);
    printf("└──────────┴──────────────┴───────┴────────┴──────┴──────────────┘\n");
    
    /* Summar general */
    printf("\n┌─────────────────────────────────────────────────────────────────┐\n");
    printf("│ SUMAR GENERAL                                                   │\n");
    printf("├─────────────────────────────────────────────────────────────────┤\n");
    printf("│ Timp total:          %10.2f ms                              │\n", total_time_ms);
    printf("│ Throughput producere: %9.2f linii/sec                       │\n", 
           total_prod_lines / (total_time_ms / 1000.0));
    printf("│ Throughput consum:    %9.2f linii/sec                       │\n",
           total_cons_lines / (total_time_ms / 1000.0));
    printf("│ Verificare integritate: %s                                 │\n",
           total_prod_lines == total_cons_lines ? "✓ OK" : "✗ EROARE");
    printf("└─────────────────────────────────────────────────────────────────┘\n");
}

/* =============================================================================
 * MAIN
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║     SOLUȚIE EXERCIȚIUL 1: Log Processor cu Producer-Consumer                  ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    /* Parsare argumente */
    int num_producers = DEFAULT_PRODUCERS;
    int num_consumers = DEFAULT_CONSUMERS;
    int total_lines = SIMULATED_LINES;
    
    if (argc >= 3) {
        num_producers = atoi(argv[2]);
        if (num_producers < 1 || num_producers > 10) {
            fprintf(stderr, "Număr producători invalid (1-10)\n");
            return 1;
        }
    }
    if (argc >= 4) {
        num_consumers = atoi(argv[3]);
        if (num_consumers < 1 || num_consumers > 10) {
            fprintf(stderr, "Număr consumatori invalid (1-10)\n");
            return 1;
        }
    }
    
    printf("Configurare:\n");
    printf("  Producători: %d\n", num_producers);
    printf("  Consumatori: %d\n", num_consumers);
    printf("  Linii totale: %d\n", total_lines);
    printf("  Buffer size: %d\n\n", DEFAULT_BUFFER_SIZE);
    
    srand(time(NULL));
    
    /* Creează buffer-ul partajat */
    BoundedBuffer *buffer = buffer_create(DEFAULT_BUFFER_SIZE, num_producers);
    if (!buffer) {
        fprintf(stderr, "Eroare la crearea buffer-ului\n");
        return 1;
    }
    
    /* Alocă și inițializează argumentele thread-urilor */
    ProducerArg *prod_args = calloc(num_producers, sizeof(ProducerArg));
    ConsumerArg *cons_args = calloc(num_consumers, sizeof(ConsumerArg));
    pthread_t *prod_threads = malloc(num_producers * sizeof(pthread_t));
    pthread_t *cons_threads = malloc(num_consumers * sizeof(pthread_t));
    
    /* Împarte liniile între producători */
    int lines_per_producer = total_lines / num_producers;
    int remaining_lines = total_lines % num_producers;
    int current_line = 1;
    
    for (int i = 0; i < num_producers; i++) {
        prod_args[i].id = i;
        prod_args[i].buffer = buffer;
        prod_args[i].filename = (argc >= 2) ? argv[1] : NULL;
        prod_args[i].start_line = current_line;
        prod_args[i].end_line = current_line + lines_per_producer - 1;
        if (i < remaining_lines) {
            prod_args[i].end_line++;
        }
        current_line = prod_args[i].end_line + 1;
        prod_args[i].lines_produced = 0;
        prod_args[i].time_spent_ms = 0;
    }
    
    for (int i = 0; i < num_consumers; i++) {
        cons_args[i].id = i;
        cons_args[i].buffer = buffer;
        cons_args[i].lines_consumed = 0;
        cons_args[i].error_lines = 0;
        cons_args[i].warning_lines = 0;
        cons_args[i].info_lines = 0;
        cons_args[i].time_spent_ms = 0;
    }
    
    double start_time = get_time_ms();
    
    printf("Pornire thread-uri...\n\n");
    
    /* Pornește consumatorii întâi (evită race condition la startup) */
    for (int i = 0; i < num_consumers; i++) {
        pthread_create(&cons_threads[i], NULL, consumer_thread, &cons_args[i]);
    }
    
    /* Pornește producătorii */
    for (int i = 0; i < num_producers; i++) {
        pthread_create(&prod_threads[i], NULL, producer_thread, &prod_args[i]);
    }
    
    /* Așteaptă terminarea producătorilor */
    for (int i = 0; i < num_producers; i++) {
        pthread_join(prod_threads[i], NULL);
    }
    
    /* Așteaptă terminarea consumatorilor */
    for (int i = 0; i < num_consumers; i++) {
        pthread_join(cons_threads[i], NULL);
    }
    
    double total_time = get_time_ms() - start_time;
    
    /* Afișează statistici */
    print_statistics(prod_args, num_producers, cons_args, num_consumers, total_time);
    
    /* Curățare */
    buffer_destroy(buffer);
    free(prod_args);
    free(cons_args);
    free(prod_threads);
    free(cons_threads);
    
    printf("\n✓ Program terminat cu succes!\n\n");
    
    return 0;
}
