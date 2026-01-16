/**
 * =============================================================================
 * SĂPTĂMÂNA 20: Programare Paralelă și Concurentă
 * TEMA 2 - SOLUȚIE: Map-Reduce Word Count în C
 * =============================================================================
 *
 * Implementare completă a paradigmei Map-Reduce pentru numărarea cuvintelor
 * în fișiere text, cu faze Map și Reduce paralele.
 *
 * Arhitectură:
 *   1. Faza MAP: Fiecare mapper procesează un fișier, extrage cuvinte,
 *      le partiționează după hash în bucket-uri pentru reduceri
 *   2. Faza SHUFFLE: Implicitly în bucket-urile partiționate
 *   3. Faza REDUCE: Fiecare reducer agregă cuvintele din bucket-ul său
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -O2 -pthread -o mapreduce homework2_sol.c
 * Execuție:  ./mapreduce file1.txt file2.txt file3.txt
 *
 * Autor: Material educațional ATP - CSIE ASE București
 * Data: Ianuarie 2026
 * =============================================================================
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <pthread.h>
#include <time.h>
#include <stdatomic.h>

/* =============================================================================
 * CONFIGURAȚIE
 * ============================================================================= */

#define MAX_WORD_LENGTH     64
#define HASH_TABLE_SIZE     10007   /* Număr prim pentru hash */
#define NUM_REDUCERS        4       /* Număr de bucket-uri/reduceri */
#define MAX_FILES           100
#define INITIAL_BUCKET_SIZE 1024

/* =============================================================================
 * STRUCTURI DE DATE
 * ============================================================================= */

/**
 * Pereche cheie-valoare pentru Map-Reduce
 */
typedef struct {
    char word[MAX_WORD_LENGTH];
    int count;
} KeyValue;

/**
 * Nod în lista înlănțuită pentru hash table
 */
typedef struct HashNode {
    char word[MAX_WORD_LENGTH];
    int count;
    struct HashNode *next;
} HashNode;

/**
 * Hash table pentru agregare
 */
typedef struct {
    HashNode **buckets;
    size_t size;
    size_t num_entries;
    pthread_mutex_t lock;
} HashTable;

/**
 * Bucket intermediar pentru shuffle (un bucket per reducer)
 */
typedef struct {
    KeyValue *entries;
    size_t capacity;
    size_t count;
    pthread_mutex_t lock;
} IntermediateBucket;

/**
 * Argumentele pentru un thread mapper
 */
typedef struct {
    int mapper_id;
    const char *filename;
    IntermediateBucket *buckets;  /* Array de NUM_REDUCERS bucket-uri */
    int num_reducers;
    
    /* Statistici */
    size_t words_processed;
    double time_spent;
} MapperArgs;

/**
 * Argumentele pentru un thread reducer
 */
typedef struct {
    int reducer_id;
    IntermediateBucket *input_bucket;
    HashTable *output_table;
    
    /* Statistici */
    size_t words_aggregated;
    double time_spent;
} ReducerArgs;

/**
 * Rezultatul final Map-Reduce
 */
typedef struct {
    KeyValue *results;
    size_t count;
    size_t capacity;
} MapReduceResult;

/**
 * Context global Map-Reduce
 */
typedef struct {
    /* Input */
    const char **input_files;
    int num_files;
    
    /* Intermediate */
    IntermediateBucket *intermediate_buckets;
    int num_reducers;
    
    /* Output */
    HashTable **reducer_outputs;
    MapReduceResult final_result;
    
    /* Sincronizare */
    pthread_barrier_t map_barrier;
    pthread_barrier_t reduce_barrier;
    
    /* Statistici */
    atomic_size_t total_words_mapped;
    atomic_size_t total_words_reduced;
    double map_time;
    double reduce_time;
} MapReduceContext;

/* =============================================================================
 * FUNCȚII UTILITARE
 * ============================================================================= */

/**
 * Obține timpul curent în secunde (cu precizie ridicată)
 */
static double get_time(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

/**
 * Funcție hash pentru stringuri (djb2)
 */
static unsigned long hash_string(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

/**
 * Determină în ce bucket merge un cuvânt (pentru partițonare)
 */
static int get_partition(const char *word, int num_partitions) {
    return (int)(hash_string(word) % (unsigned long)num_partitions);
}

/**
 * Normalizează un cuvânt: lowercase, elimină punctuație
 */
static void normalize_word(char *word) {
    char *src = word;
    char *dst = word;
    
    while (*src) {
        if (isalpha((unsigned char)*src)) {
            *dst++ = tolower((unsigned char)*src);
        }
        src++;
    }
    *dst = '\0';
}

/* =============================================================================
 * HASH TABLE PENTRU AGREGARE
 * ============================================================================= */

/**
 * Creează un hash table nou
 */
static HashTable* hashtable_create(size_t size) {
    HashTable *ht = malloc(sizeof(HashTable));
    if (!ht) return NULL;
    
    ht->buckets = calloc(size, sizeof(HashNode*));
    if (!ht->buckets) {
        free(ht);
        return NULL;
    }
    
    ht->size = size;
    ht->num_entries = 0;
    pthread_mutex_init(&ht->lock, NULL);
    
    return ht;
}

/**
 * Inserează sau incrementează un cuvânt în hash table
 */
static void hashtable_insert(HashTable *ht, const char *word, int count) {
    unsigned long h = hash_string(word) % ht->size;
    
    pthread_mutex_lock(&ht->lock);
    
    /* Caută dacă cuvântul există deja */
    HashNode *node = ht->buckets[h];
    while (node) {
        if (strcmp(node->word, word) == 0) {
            node->count += count;
            pthread_mutex_unlock(&ht->lock);
            return;
        }
        node = node->next;
    }
    
    /* Cuvânt nou - adaugă la începutul listei */
    node = malloc(sizeof(HashNode));
    if (node) {
        strncpy(node->word, word, MAX_WORD_LENGTH - 1);
        node->word[MAX_WORD_LENGTH - 1] = '\0';
        node->count = count;
        node->next = ht->buckets[h];
        ht->buckets[h] = node;
        ht->num_entries++;
    }
    
    pthread_mutex_unlock(&ht->lock);
}

/**
 * Eliberează memoria pentru hash table
 */
static void hashtable_free(HashTable *ht) {
    if (!ht) return;
    
    for (size_t i = 0; i < ht->size; i++) {
        HashNode *node = ht->buckets[i];
        while (node) {
            HashNode *next = node->next;
            free(node);
            node = next;
        }
    }
    
    pthread_mutex_destroy(&ht->lock);
    free(ht->buckets);
    free(ht);
}

/**
 * Iterează prin toate intrările din hash table
 */
typedef void (*HashTableIterFunc)(const char *word, int count, void *user_data);

static void hashtable_iterate(HashTable *ht, HashTableIterFunc func, void *user_data) {
    for (size_t i = 0; i < ht->size; i++) {
        HashNode *node = ht->buckets[i];
        while (node) {
            func(node->word, node->count, user_data);
            node = node->next;
        }
    }
}

/* =============================================================================
 * INTERMEDIATE BUCKETS
 * ============================================================================= */

/**
 * Inițializează un bucket intermediar
 */
static void bucket_init(IntermediateBucket *bucket) {
    bucket->entries = malloc(INITIAL_BUCKET_SIZE * sizeof(KeyValue));
    bucket->capacity = INITIAL_BUCKET_SIZE;
    bucket->count = 0;
    pthread_mutex_init(&bucket->lock, NULL);
}

/**
 * Adaugă un cuvânt în bucket (thread-safe)
 */
static void bucket_add(IntermediateBucket *bucket, const char *word) {
    pthread_mutex_lock(&bucket->lock);
    
    /* Redimensionare dacă e necesar */
    if (bucket->count >= bucket->capacity) {
        bucket->capacity *= 2;
        bucket->entries = realloc(bucket->entries, 
                                   bucket->capacity * sizeof(KeyValue));
    }
    
    /* Adaugă entry (count=1 pentru fiecare apariție) */
    strncpy(bucket->entries[bucket->count].word, word, MAX_WORD_LENGTH - 1);
    bucket->entries[bucket->count].word[MAX_WORD_LENGTH - 1] = '\0';
    bucket->entries[bucket->count].count = 1;
    bucket->count++;
    
    pthread_mutex_unlock(&bucket->lock);
}

/**
 * Eliberează memoria pentru bucket
 */
static void bucket_free(IntermediateBucket *bucket) {
    pthread_mutex_destroy(&bucket->lock);
    free(bucket->entries);
}

/* =============================================================================
 * FAZA MAP
 * ============================================================================= */

/**
 * Funcția executată de fiecare mapper thread
 *
 * Citește un fișier, extrage cuvintele și le distribuie în bucket-uri
 * în funcție de hash-ul cuvântului (pentru shuffle implicit).
 */
static void* mapper_thread(void *arg) {
    MapperArgs *args = (MapperArgs*)arg;
    double start_time = get_time();
    
    printf("[Mapper %d] Start procesare: %s\n", args->mapper_id, args->filename);
    
    FILE *file = fopen(args->filename, "r");
    if (!file) {
        fprintf(stderr, "[Mapper %d] EROARE: Nu pot deschide %s\n", 
                args->mapper_id, args->filename);
        return NULL;
    }
    
    char word[MAX_WORD_LENGTH];
    size_t words_count = 0;
    
    /* Citește cuvânt cu cuvânt */
    while (fscanf(file, "%63s", word) == 1) {
        /* Normalizează cuvântul */
        normalize_word(word);
        
        /* Ignoră cuvintele goale sau prea scurte */
        if (strlen(word) < 2) continue;
        
        /* Determină partitionarea (în ce bucket merge) */
        int partition = get_partition(word, args->num_reducers);
        
        /* Emit (word, 1) în bucket-ul corespunzător */
        bucket_add(&args->buckets[partition], word);
        
        words_count++;
    }
    
    fclose(file);
    
    args->words_processed = words_count;
    args->time_spent = get_time() - start_time;
    
    printf("[Mapper %d] Finalizat: %zu cuvinte în %.3f sec\n",
           args->mapper_id, words_count, args->time_spent);
    
    return NULL;
}

/* =============================================================================
 * FAZA REDUCE
 * ============================================================================= */

/**
 * Funcția executată de fiecare reducer thread
 *
 * Procesează toate perechile (word, 1) din bucket-ul său și agregă
 * numărătorile pentru fiecare cuvânt unic.
 */
static void* reducer_thread(void *arg) {
    ReducerArgs *args = (ReducerArgs*)arg;
    double start_time = get_time();
    
    printf("[Reducer %d] Start agregare: %zu entries\n", 
           args->reducer_id, args->input_bucket->count);
    
    size_t words_count = 0;
    
    /* Procesează toate intrările din bucket */
    for (size_t i = 0; i < args->input_bucket->count; i++) {
        KeyValue *kv = &args->input_bucket->entries[i];
        
        /* Agregă în hash table-ul de output */
        hashtable_insert(args->output_table, kv->word, kv->count);
        words_count++;
    }
    
    args->words_aggregated = words_count;
    args->time_spent = get_time() - start_time;
    
    printf("[Reducer %d] Finalizat: %zu → %zu cuvinte unice în %.3f sec\n",
           args->reducer_id, words_count, args->output_table->num_entries,
           args->time_spent);
    
    return NULL;
}

/* =============================================================================
 * COLECTARE REZULTATE
 * ============================================================================= */

/**
 * Callback pentru colectarea rezultatelor din hash table
 */
typedef struct {
    MapReduceResult *result;
} CollectContext;

static void collect_callback(const char *word, int count, void *user_data) {
    CollectContext *ctx = (CollectContext*)user_data;
    MapReduceResult *result = ctx->result;
    
    /* Redimensionare dacă e necesar */
    if (result->count >= result->capacity) {
        result->capacity *= 2;
        result->results = realloc(result->results, 
                                   result->capacity * sizeof(KeyValue));
    }
    
    strncpy(result->results[result->count].word, word, MAX_WORD_LENGTH - 1);
    result->results[result->count].word[MAX_WORD_LENGTH - 1] = '\0';
    result->results[result->count].count = count;
    result->count++;
}

/**
 * Comparator pentru sortarea rezultatelor după count (descrescător)
 */
static int compare_by_count_desc(const void *a, const void *b) {
    const KeyValue *ka = (const KeyValue*)a;
    const KeyValue *kb = (const KeyValue*)b;
    return kb->count - ka->count;
}

/**
 * Comparator pentru sortarea rezultatelor alfabetic
 */
static int compare_by_word(const void *a, const void *b) {
    const KeyValue *ka = (const KeyValue*)a;
    const KeyValue *kb = (const KeyValue*)b;
    return strcmp(ka->word, kb->word);
}

/* =============================================================================
 * ORCHESTRARE MAP-REDUCE
 * ============================================================================= */

/**
 * Execută job-ul Map-Reduce complet
 */
static MapReduceResult mapreduce_execute(const char **files, int num_files, int num_reducers) {
    MapReduceResult result = {0};
    double total_start = get_time();
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║           MAP-REDUCE WORD COUNT - START                       ║\n");
    printf("╠═══════════════════════════════════════════════════════════════╣\n");
    printf("║  Fișiere input:  %-4d                                         ║\n", num_files);
    printf("║  Mappere:        %-4d (unul per fișier)                       ║\n", num_files);
    printf("║  Reducere:       %-4d                                         ║\n", num_reducers);
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    /* =================================
     * INIȚIALIZARE
     * ================================= */
    
    /* Bucket-uri intermediare (unul per reducer) */
    IntermediateBucket *buckets = malloc(num_reducers * sizeof(IntermediateBucket));
    for (int i = 0; i < num_reducers; i++) {
        bucket_init(&buckets[i]);
    }
    
    /* Hash tables pentru output reducere */
    HashTable **reducer_outputs = malloc(num_reducers * sizeof(HashTable*));
    for (int i = 0; i < num_reducers; i++) {
        reducer_outputs[i] = hashtable_create(HASH_TABLE_SIZE);
    }
    
    /* =================================
     * FAZA MAP
     * ================================= */
    
    printf("═══ FAZA MAP ═══════════════════════════════════════════════════\n\n");
    double map_start = get_time();
    
    pthread_t *mapper_threads = malloc(num_files * sizeof(pthread_t));
    MapperArgs *mapper_args = malloc(num_files * sizeof(MapperArgs));
    
    for (int i = 0; i < num_files; i++) {
        mapper_args[i].mapper_id = i;
        mapper_args[i].filename = files[i];
        mapper_args[i].buckets = buckets;
        mapper_args[i].num_reducers = num_reducers;
        mapper_args[i].words_processed = 0;
        
        pthread_create(&mapper_threads[i], NULL, mapper_thread, &mapper_args[i]);
    }
    
    /* Așteaptă toți mapperii să termine */
    size_t total_mapped = 0;
    for (int i = 0; i < num_files; i++) {
        pthread_join(mapper_threads[i], NULL);
        total_mapped += mapper_args[i].words_processed;
    }
    
    double map_time = get_time() - map_start;
    printf("\n[MAP] Total: %zu cuvinte procesate în %.3f sec\n\n", total_mapped, map_time);
    
    /* =================================
     * FAZA SHUFFLE (implicită în bucket-uri)
     * ================================= */
    
    printf("═══ FAZA SHUFFLE ════════════════════════════════════════════════\n\n");
    printf("[SHUFFLE] Distribuție în bucket-uri:\n");
    for (int i = 0; i < num_reducers; i++) {
        printf("  Bucket %d: %zu entries\n", i, buckets[i].count);
    }
    printf("\n");
    
    /* =================================
     * FAZA REDUCE
     * ================================= */
    
    printf("═══ FAZA REDUCE ═════════════════════════════════════════════════\n\n");
    double reduce_start = get_time();
    
    pthread_t *reducer_threads = malloc(num_reducers * sizeof(pthread_t));
    ReducerArgs *reducer_args = malloc(num_reducers * sizeof(ReducerArgs));
    
    for (int i = 0; i < num_reducers; i++) {
        reducer_args[i].reducer_id = i;
        reducer_args[i].input_bucket = &buckets[i];
        reducer_args[i].output_table = reducer_outputs[i];
        reducer_args[i].words_aggregated = 0;
        
        pthread_create(&reducer_threads[i], NULL, reducer_thread, &reducer_args[i]);
    }
    
    /* Așteaptă toți reducerii să termine */
    size_t total_reduced = 0;
    for (int i = 0; i < num_reducers; i++) {
        pthread_join(reducer_threads[i], NULL);
        total_reduced += reducer_args[i].words_aggregated;
    }
    
    double reduce_time = get_time() - reduce_start;
    printf("\n[REDUCE] Total: %zu cuvinte agregate în %.3f sec\n\n", total_reduced, reduce_time);
    
    /* =================================
     * COLECTARE REZULTATE
     * ================================= */
    
    printf("═══ COLECTARE REZULTATE ═════════════════════════════════════════\n\n");
    
    /* Inițializare rezultat */
    result.capacity = INITIAL_BUCKET_SIZE;
    result.results = malloc(result.capacity * sizeof(KeyValue));
    result.count = 0;
    
    CollectContext ctx = { .result = &result };
    
    /* Colectează din toate hash tables-urile reducerilor */
    for (int i = 0; i < num_reducers; i++) {
        hashtable_iterate(reducer_outputs[i], collect_callback, &ctx);
    }
    
    /* Sortează după count (descrescător) */
    qsort(result.results, result.count, sizeof(KeyValue), compare_by_count_desc);
    
    double total_time = get_time() - total_start;
    
    printf("[REZULTAT] %zu cuvinte unice găsite\n", result.count);
    printf("[TIMP] Total: %.3f sec (Map: %.3f, Reduce: %.3f)\n\n", 
           total_time, map_time, reduce_time);
    
    /* =================================
     * CLEANUP
     * ================================= */
    
    for (int i = 0; i < num_reducers; i++) {
        bucket_free(&buckets[i]);
        hashtable_free(reducer_outputs[i]);
    }
    
    free(buckets);
    free(reducer_outputs);
    free(mapper_threads);
    free(mapper_args);
    free(reducer_threads);
    free(reducer_args);
    
    return result;
}

/**
 * Eliberează memoria pentru rezultatul Map-Reduce
 */
static void mapreduce_result_free(MapReduceResult *result) {
    free(result->results);
    result->results = NULL;
    result->count = 0;
    result->capacity = 0;
}

/* =============================================================================
 * AFIȘARE REZULTATE
 * ============================================================================= */

/**
 * Afișează top N cuvinte după frecvență
 */
static void print_top_words(MapReduceResult *result, int top_n) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║           TOP %d CUVINTE DUPĂ FRECVENȚĂ                       ║\n", top_n);
    printf("╠═══════════════════════════════════════════════════════════════╣\n");
    printf("║  Rank  │  Count   │  Word                                    ║\n");
    printf("╠════════╪══════════╪══════════════════════════════════════════╣\n");
    
    int count = (int)result->count < top_n ? (int)result->count : top_n;
    for (int i = 0; i < count; i++) {
        printf("║  %4d  │  %6d  │  %-40s ║\n",
               i + 1, result->results[i].count, result->results[i].word);
    }
    
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
}

/**
 * Salvează rezultatele într-un fișier
 */
static void save_results(MapReduceResult *result, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "EROARE: Nu pot crea fișierul de output %s\n", filename);
        return;
    }
    
    fprintf(file, "word,count\n");
    for (size_t i = 0; i < result->count; i++) {
        fprintf(file, "%s,%d\n", result->results[i].word, result->results[i].count);
    }
    
    fclose(file);
    printf("✓ Rezultate salvate în: %s\n", filename);
}

/* =============================================================================
 * MAIN
 * ============================================================================= */

static void print_usage(const char *program) {
    printf("\n");
    printf("Utilizare: %s [opțiuni] <file1> <file2> ... <fileN>\n", program);
    printf("\n");
    printf("Opțiuni:\n");
    printf("  -r <num>     Numărul de reduceri (default: 4)\n");
    printf("  -t <num>     Afișează top N cuvinte (default: 20)\n");
    printf("  -o <file>    Salvează rezultatele în fișier CSV\n");
    printf("  -h           Afișează acest ajutor\n");
    printf("\n");
    printf("Exemplu:\n");
    printf("  %s -r 8 -t 50 -o results.csv doc1.txt doc2.txt doc3.txt\n", program);
    printf("\n");
}

int main(int argc, char *argv[]) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║     SĂPTĂMÂNA 20: Programare Paralelă și Concurentă                            ║\n");
    printf("║     TEMA 2 - Map-Reduce Word Count                                             ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════════════════╝\n");
    
    /* Parametri default */
    int num_reducers = NUM_REDUCERS;
    int top_n = 20;
    const char *output_file = NULL;
    
    /* Parsare argumente */
    const char *files[MAX_FILES];
    int num_files = 0;
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "-r") == 0 && i + 1 < argc) {
            num_reducers = atoi(argv[++i]);
            if (num_reducers < 1) num_reducers = 1;
            if (num_reducers > 32) num_reducers = 32;
        } else if (strcmp(argv[i], "-t") == 0 && i + 1 < argc) {
            top_n = atoi(argv[++i]);
            if (top_n < 1) top_n = 1;
        } else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            output_file = argv[++i];
        } else if (argv[i][0] != '-') {
            if (num_files < MAX_FILES) {
                files[num_files++] = argv[i];
            }
        }
    }
    
    /* Verificare input */
    if (num_files == 0) {
        printf("\n⚠️  Nu s-au specificat fișiere de procesat.\n");
        printf("   Generez fișiere de test...\n\n");
        
        /* Generare fișiere de test */
        const char *test_files[] = {"test_file1.txt", "test_file2.txt"};
        
        FILE *f1 = fopen(test_files[0], "w");
        fprintf(f1, "the quick brown fox jumps over the lazy dog\n");
        fprintf(f1, "the fox was quick and the dog was lazy\n");
        fprintf(f1, "hello world hello universe\n");
        fclose(f1);
        
        FILE *f2 = fopen(test_files[1], "w");
        fprintf(f2, "parallel programming is fun and challenging\n");
        fprintf(f2, "threads and processes work in parallel\n");
        fprintf(f2, "the world of parallel computing\n");
        fclose(f2);
        
        files[0] = test_files[0];
        files[1] = test_files[1];
        num_files = 2;
    }
    
    /* Execută Map-Reduce */
    MapReduceResult result = mapreduce_execute(files, num_files, num_reducers);
    
    /* Afișează rezultate */
    print_top_words(&result, top_n);
    
    /* Salvează rezultate dacă s-a cerut */
    if (output_file) {
        save_results(&result, output_file);
    }
    
    /* Cleanup */
    mapreduce_result_free(&result);
    
    printf("\n✅ Map-Reduce completat cu succes!\n\n");
    
    return 0;
}
