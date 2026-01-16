/**
 * =============================================================================
 * EXERCIȚIUL 1: Log Processor cu Producer-Consumer
 * =============================================================================
 *
 * OBIECTIV:
 *   Implementarea unui sistem de procesare log-uri folosind pattern-ul 
 *   producer-consumer cu bounded buffer. Producătorii citesc linii dintr-un
 *   fișier log, iar consumatorii procesează și agregă statistici.
 *
 * CERINȚE:
 *   1. Definirea structurii BoundedBuffer cu mutex și condition variables
 *   2. N thread-uri producător care citesc din fișierul de log
 *   3. M thread-uri consumator care procesează liniile
 *   4. Terminare grațioasă când tot fișierul a fost procesat
 *   5. Afișare statistici per thread și total
 *
 * EXEMPLU UTILIZARE:
 *   ./exercise1                     # Folosește date simulate
 *   ./exercise1 log.txt 2 4         # 2 producători, 4 consumatori
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -o exercise1 exercise1.c -lpthread
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
 * TODO 1: Completați structura BoundedBuffer
 *
 * Bounded Buffer thread-safe pentru schimbul de date între
 * producători și consumatori.
 *
 * Hint: Aveți nevoie de:
 *   - Array de LogLine pentru stocare
 *   - Indecși pentru head și tail (buffer circular)
 *   - Contoare pentru elements count și capacity
 *   - Mutex pentru protejarea accesului
 *   - Condition variables pentru not_full și not_empty
 *   - Flag pentru shutdown
 */
typedef struct {
    /* YOUR CODE HERE */
    LogLine *buffer;
    size_t capacity;
    size_t count;
    size_t head;
    size_t tail;
    
    pthread_mutex_t mutex;
    pthread_cond_t not_full;
    pthread_cond_t not_empty;
    
    bool shutdown;
    atomic_int done_producers;  /* Câți producători au terminat */
    int total_producers;
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
 * Obține timestamp în milisecunde
 */
double get_time_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1000000.0;
}

/**
 * Determină tipul liniei de log
 * Returnează: 'E' pentru ERROR, 'W' pentru WARNING, 'I' pentru INFO
 */
char get_log_level(const char *line) {
    if (strstr(line, "ERROR") != NULL) return 'E';
    if (strstr(line, "WARNING") != NULL || strstr(line, "WARN") != NULL) return 'W';
    return 'I';
}

/* =============================================================================
 * TODO 2: IMPLEMENTAȚI FUNCȚIILE BUFFER
 * =============================================================================
 */

/**
 * Creează un bounded buffer nou
 *
 * @param capacity Capacitatea buffer-ului
 * @param total_producers Numărul total de producători
 * @return Pointer la buffer-ul creat
 *
 * Pași:
 *   1. Alocă memorie pentru structura BoundedBuffer
 *   2. Alocă array-ul de LogLine cu capacitatea dată
 *   3. Inițializează toate câmpurile (count=0, head=0, tail=0, etc.)
 *   4. Inițializează mutex-ul și condition variables
 *   5. Setează shutdown=false și done_producers=0
 */
BoundedBuffer* buffer_create(size_t capacity, int total_producers) {
    /* YOUR CODE HERE */
    BoundedBuffer *bb = malloc(sizeof(BoundedBuffer));
    if (!bb) return NULL;
    
    bb->buffer = malloc(capacity * sizeof(LogLine));
    if (!bb->buffer) {
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
    
    pthread_mutex_init(&bb->mutex, NULL);
    pthread_cond_init(&bb->not_full, NULL);
    pthread_cond_init(&bb->not_empty, NULL);
    
    return bb;
}

/**
 * Distruge bounded buffer și eliberează resursele
 *
 * @param bb Pointer la buffer
 */
void buffer_destroy(BoundedBuffer *bb) {
    /* YOUR CODE HERE */
    if (!bb) return;
    
    pthread_mutex_destroy(&bb->mutex);
    pthread_cond_destroy(&bb->not_full);
    pthread_cond_destroy(&bb->not_empty);
    free(bb->buffer);
    free(bb);
}

/**
 * Adaugă o linie în buffer (blocare dacă plin)
 *
 * @param bb Pointer la buffer
 * @param line Linia de adăugat
 * @return true dacă a reușit, false dacă shutdown
 *
 * Pași:
 *   1. Blochează mutex-ul
 *   2. Așteaptă (while loop!) cât timp buffer-ul este plin și nu e shutdown
 *   3. Verifică dacă e shutdown - dacă da, deblochează și returnează false
 *   4. Copiază linia în buffer[tail]
 *   5. Actualizează tail = (tail + 1) % capacity
 *   6. Incrementează count
 *   7. Semnalizează not_empty
 *   8. Deblochează mutex și returnează true
 */
bool buffer_put(BoundedBuffer *bb, LogLine *line) {
    /* YOUR CODE HERE */
    pthread_mutex_lock(&bb->mutex);
    
    while (bb->count == bb->capacity && !bb->shutdown) {
        pthread_cond_wait(&bb->not_full, &bb->mutex);
    }
    
    if (bb->shutdown) {
        pthread_mutex_unlock(&bb->mutex);
        return false;
    }
    
    bb->buffer[bb->tail] = *line;
    bb->tail = (bb->tail + 1) % bb->capacity;
    bb->count++;
    
    pthread_cond_signal(&bb->not_empty);
    pthread_mutex_unlock(&bb->mutex);
    
    return true;
}

/**
 * Extrage o linie din buffer (blocare dacă gol)
 *
 * @param bb Pointer la buffer
 * @param line Pointer unde se copiază linia extrasă
 * @return true dacă a extras, false dacă nu mai sunt date
 *
 * Pași:
 *   1. Blochează mutex-ul
 *   2. Așteaptă cât timp buffer-ul este gol și nu toți producătorii au terminat
 *   3. Verifică dacă gol și toți au terminat - returnează false
 *   4. Copiază buffer[head] în line
 *   5. Actualizează head = (head + 1) % capacity
 *   6. Decrementează count
 *   7. Semnalizează not_full
 *   8. Deblochează mutex și returnează true
 */
bool buffer_get(BoundedBuffer *bb, LogLine *line) {
    /* YOUR CODE HERE */
    pthread_mutex_lock(&bb->mutex);
    
    while (bb->count == 0) {
        /* Verifică dacă toți producătorii au terminat */
        if (atomic_load(&bb->done_producers) >= bb->total_producers) {
            pthread_mutex_unlock(&bb->mutex);
            return false;
        }
        pthread_cond_wait(&bb->not_empty, &bb->mutex);
    }
    
    *line = bb->buffer[bb->head];
    bb->head = (bb->head + 1) % bb->capacity;
    bb->count--;
    
    pthread_cond_signal(&bb->not_full);
    pthread_mutex_unlock(&bb->mutex);
    
    return true;
}

/**
 * Marchează un producător ca terminat
 * Semnalizează consumatorii dacă toți producătorii au terminat
 */
void buffer_producer_done(BoundedBuffer *bb) {
    pthread_mutex_lock(&bb->mutex);
    atomic_fetch_add(&bb->done_producers, 1);
    /* Trezește toți consumatorii pentru a verifica condiția de terminare */
    pthread_cond_broadcast(&bb->not_empty);
    pthread_mutex_unlock(&bb->mutex);
}

/* =============================================================================
 * TODO 3: IMPLEMENTAȚI THREAD-URILE PRODUCĂTOR ȘI CONSUMATOR
 * =============================================================================
 */

/**
 * Generează linii de log simulate pentru testare
 */
void generate_simulated_line(char *buffer, int line_num) {
    const char *levels[] = {"INFO", "WARNING", "ERROR"};
    const char *messages[] = {
        "User login successful",
        "Database connection established",
        "Cache miss for key: user_123",
        "Failed to connect to external API",
        "Memory usage threshold exceeded",
        "Configuration reloaded",
        "Request timeout after 30s",
        "Invalid input format detected"
    };
    
    int level_idx = (line_num % 10 == 0) ? 2 : (line_num % 5 == 0) ? 1 : 0;
    int msg_idx = line_num % 8;
    
    snprintf(buffer, MAX_LINE_LENGTH, "[2026-01-16 10:%02d:%02d] %s: %s",
             line_num / 60, line_num % 60, levels[level_idx], messages[msg_idx]);
}

/**
 * Thread producător
 *
 * Citește linii (din fișier sau simulate) și le adaugă în buffer
 */
void* producer_thread(void *arg) {
    ProducerArg *parg = (ProducerArg*)arg;
    double start_time = get_time_ms();
    parg->lines_produced = 0;
    
    printf("  [Producer %d] Start (linii %d-%d)\n", 
           parg->id, parg->start_line, parg->end_line);
    
    for (int i = parg->start_line; i < parg->end_line; i++) {
        LogLine line;
        line.line_number = i;
        line.producer_id = parg->id;
        
        /* Generează sau citește linie */
        generate_simulated_line(line.content, i);
        
        if (!buffer_put(parg->buffer, &line)) {
            break;  /* Shutdown */
        }
        
        parg->lines_produced++;
        atomic_fetch_add(&total_produced, 1);
        
        /* Simulare timp de citire */
        usleep(1000 + (rand() % 2000));
    }
    
    /* Marchează producătorul ca terminat */
    buffer_producer_done(parg->buffer);
    
    parg->time_spent_ms = get_time_ms() - start_time;
    printf("  [Producer %d] Terminat: %d linii în %.1f ms\n",
           parg->id, parg->lines_produced, parg->time_spent_ms);
    
    return NULL;
}

/**
 * Thread consumator
 *
 * Extrage linii din buffer și le procesează (agregă statistici)
 */
void* consumer_thread(void *arg) {
    ConsumerArg *carg = (ConsumerArg*)arg;
    double start_time = get_time_ms();
    carg->lines_consumed = 0;
    carg->error_lines = 0;
    carg->warning_lines = 0;
    carg->info_lines = 0;
    
    printf("  [Consumer %d] Start\n", carg->id);
    
    LogLine line;
    while (buffer_get(carg->buffer, &line)) {
        /* Procesare: clasifică linia */
        char level = get_log_level(line.content);
        
        switch (level) {
            case 'E': carg->error_lines++; break;
            case 'W': carg->warning_lines++; break;
            default: carg->info_lines++; break;
        }
        
        carg->lines_consumed++;
        atomic_fetch_add(&total_consumed, 1);
        
        /* Simulare timp de procesare */
        usleep(2000 + (rand() % 3000));
    }
    
    carg->time_spent_ms = get_time_ms() - start_time;
    printf("  [Consumer %d] Terminat: %d linii în %.1f ms (E:%d W:%d I:%d)\n",
           carg->id, carg->lines_consumed, carg->time_spent_ms,
           carg->error_lines, carg->warning_lines, carg->info_lines);
    
    return NULL;
}

/* =============================================================================
 * MAIN
 * =============================================================================
 */

void print_usage(const char *prog_name) {
    printf("Utilizare: %s [fisier_log] [num_producatori] [num_consumatori]\n", prog_name);
    printf("  Implicit: date simulate, %d producători, %d consumatori\n",
           DEFAULT_PRODUCERS, DEFAULT_CONSUMERS);
}

int main(int argc, char *argv[]) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║     EXERCIȚIUL 1: Log Processor cu Producer-Consumer                          ║\n");
    printf("║     Săptămâna 20 - Programare Paralelă                                        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    /* Parsare argumente */
    char *filename = NULL;
    int num_producers = DEFAULT_PRODUCERS;
    int num_consumers = DEFAULT_CONSUMERS;
    int total_lines = SIMULATED_LINES;
    
    if (argc >= 2) filename = argv[1];
    if (argc >= 3) num_producers = atoi(argv[2]);
    if (argc >= 4) num_consumers = atoi(argv[3]);
    
    if (num_producers < 1) num_producers = 1;
    if (num_consumers < 1) num_consumers = 1;
    
    printf("Configurație:\n");
    printf("  Fișier: %s\n", filename ? filename : "(simulat)");
    printf("  Producători: %d\n", num_producers);
    printf("  Consumatori: %d\n", num_consumers);
    printf("  Linii totale: %d\n", total_lines);
    printf("  Buffer size: %d\n", DEFAULT_BUFFER_SIZE);
    printf("\n");
    
    srand(time(NULL));
    double start_time = get_time_ms();
    
    /* Creează buffer */
    BoundedBuffer *buffer = buffer_create(DEFAULT_BUFFER_SIZE, num_producers);
    if (!buffer) {
        fprintf(stderr, "Eroare la crearea buffer-ului!\n");
        return EXIT_FAILURE;
    }
    
    /* Alocă argumente pentru thread-uri */
    pthread_t *producers = malloc(num_producers * sizeof(pthread_t));
    pthread_t *consumers = malloc(num_consumers * sizeof(pthread_t));
    ProducerArg *prod_args = malloc(num_producers * sizeof(ProducerArg));
    ConsumerArg *cons_args = malloc(num_consumers * sizeof(ConsumerArg));
    
    /* Pornește consumatorii */
    printf("Pornesc consumatorii...\n");
    for (int i = 0; i < num_consumers; i++) {
        cons_args[i].id = i;
        cons_args[i].buffer = buffer;
        pthread_create(&consumers[i], NULL, consumer_thread, &cons_args[i]);
    }
    
    /* Pornește producătorii */
    printf("Pornesc producătorii...\n\n");
    int lines_per_producer = total_lines / num_producers;
    for (int i = 0; i < num_producers; i++) {
        prod_args[i].id = i;
        prod_args[i].buffer = buffer;
        prod_args[i].filename = filename;
        prod_args[i].start_line = i * lines_per_producer;
        prod_args[i].end_line = (i == num_producers - 1) ? 
                                 total_lines : (i + 1) * lines_per_producer;
        pthread_create(&producers[i], NULL, producer_thread, &prod_args[i]);
    }
    
    /* Așteaptă producătorii */
    for (int i = 0; i < num_producers; i++) {
        pthread_join(producers[i], NULL);
    }
    
    /* Așteaptă consumatorii */
    for (int i = 0; i < num_consumers; i++) {
        pthread_join(consumers[i], NULL);
    }
    
    double total_time = get_time_ms() - start_time;
    
    /* Afișare statistici finale */
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════════════════════\n");
    printf("                              STATISTICI FINALE                                 \n");
    printf("═══════════════════════════════════════════════════════════════════════════════\n");
    
    printf("\nProducători:\n");
    int total_prod = 0;
    for (int i = 0; i < num_producers; i++) {
        printf("  P%d: %d linii, %.1f ms\n", i, prod_args[i].lines_produced, 
               prod_args[i].time_spent_ms);
        total_prod += prod_args[i].lines_produced;
    }
    
    printf("\nConsumatori:\n");
    int total_cons = 0, total_errors = 0, total_warnings = 0, total_info = 0;
    for (int i = 0; i < num_consumers; i++) {
        printf("  C%d: %d linii, %.1f ms (E:%d W:%d I:%d)\n", 
               i, cons_args[i].lines_consumed, cons_args[i].time_spent_ms,
               cons_args[i].error_lines, cons_args[i].warning_lines, 
               cons_args[i].info_lines);
        total_cons += cons_args[i].lines_consumed;
        total_errors += cons_args[i].error_lines;
        total_warnings += cons_args[i].warning_lines;
        total_info += cons_args[i].info_lines;
    }
    
    printf("\nSumar:\n");
    printf("  Total produs:   %d linii\n", total_prod);
    printf("  Total consumat: %d linii\n", total_cons);
    printf("  Verificare:     %s\n", total_prod == total_cons ? "✓ CORECT" : "✗ EROARE");
    printf("\n");
    printf("  Erori:          %d (%.1f%%)\n", total_errors, 100.0 * total_errors / total_cons);
    printf("  Avertismente:   %d (%.1f%%)\n", total_warnings, 100.0 * total_warnings / total_cons);
    printf("  Info:           %d (%.1f%%)\n", total_info, 100.0 * total_info / total_cons);
    printf("\n");
    printf("  Timp total:     %.1f ms\n", total_time);
    printf("  Throughput:     %.1f linii/s\n", 1000.0 * total_cons / total_time);
    printf("\n");
    
    /* Curățare */
    buffer_destroy(buffer);
    free(producers);
    free(consumers);
    free(prod_args);
    free(cons_args);
    
    return EXIT_SUCCESS;
}
