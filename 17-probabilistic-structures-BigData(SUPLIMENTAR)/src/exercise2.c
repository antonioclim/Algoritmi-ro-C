/**
 * =============================================================================
 * EXERCIȚIUL 2: Analizator de Frecvențe în Stream (Heavy Hitters)
 * =============================================================================
 *
 * OBIECTIV:
 *   Implementarea unui sistem care identifică "heavy hitters" (elementele 
 *   cele mai frecvente) într-un stream de date folosind Count-Min Sketch.
 *
 * CERINȚE:
 *   1. Implementați Count-Min Sketch cu parametri ε și δ configurabili
 *   2. Procesați un stream de cuvinte din fișier
 *   3. Identificați Top-K cele mai frecvente cuvinte
 *   4. Comparați cu numărarea exactă (pentru validare pe date mici)
 *   5. Raportați eroarea relativă pentru fiecare heavy hitter
 *
 * EXEMPLU UTILIZARE:
 *   ./exercise2 data/words_stream.txt
 *
 * EXPECTED OUTPUT:
 *   === Heavy Hitters Analysis ===
 *   Stream size:          1,000,000 words
 *   CMS parameters:       ε=0.01, δ=0.01
 *   
 *   Top-10 Heavy Hitters:
 *   Rank  Word          Estimated    Error
 *   ----  ----          ---------    -----
 *     1   the             45,123    0.08%
 *     2   and             32,456    0.17%
 *     ...
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -o exercise2 exercise2.c -lm
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <ctype.h>

/* =============================================================================
 * CONSTANTE ȘI CONFIGURARE
 * =============================================================================
 */

#define MAX_WORD_LENGTH 256
#define DEFAULT_EPSILON 0.01
#define DEFAULT_DELTA 0.01
#define TOP_K 10

/* Dimensiune pentru hash table-ul exact (pentru validare) */
#define EXACT_HASH_SIZE 10007

/* =============================================================================
 * FUNCȚII HASH (FURNIZATE)
 * =============================================================================
 */

uint64_t hash_fnv1a(const void *data, size_t len) {
    const uint8_t *bytes = (const uint8_t *)data;
    uint64_t hash = 14695981039346656037ULL;
    const uint64_t prime = 1099511628211ULL;
    
    for (size_t i = 0; i < len; i++) {
        hash ^= bytes[i];
        hash *= prime;
    }
    
    return hash;
}

uint64_t hash_djb2(const void *data, size_t len) {
    const uint8_t *bytes = (const uint8_t *)data;
    uint64_t hash = 5381;
    
    for (size_t i = 0; i < len; i++) {
        hash = ((hash << 5) + hash) + bytes[i];
    }
    
    return hash;
}

uint64_t hash_family(const void *data, size_t len, uint32_t i) {
    uint64_t h1 = hash_fnv1a(data, len);
    uint64_t h2 = hash_djb2(data, len);
    return h1 + i * h2;
}

/* =============================================================================
 * STRUCTURA COUNT-MIN SKETCH
 * =============================================================================
 */

/**
 * TODO 1: Completați structura CountMinSketch
 *
 * Hint: Aveți nevoie de:
 *   - Matrice de contoare (depth × width)
 *   - Dimensiunile (depth, width)
 *   - Parametrii (epsilon, delta)
 *   - Totalul elementelor procesate
 */
typedef struct {
    /* YOUR CODE HERE */
    int64_t **counters;        /* Matrice depth × width */
    size_t depth;              /* Numărul de rânduri */
    size_t width;              /* Numărul de coloane */
    double epsilon;            /* Eroarea relativă maximă */
    double delta;              /* Probabilitatea de eroare */
    uint64_t total_count;      /* Totalul elementelor */
} CountMinSketch;

/**
 * TODO 2: Implementați funcția de creare Count-Min Sketch
 *
 * Formule pentru dimensiuni:
 *   width = ceil(e / epsilon)     unde e ≈ 2.718
 *   depth = ceil(ln(1 / delta))
 *
 * @param epsilon Eroarea relativă maximă (ex: 0.01)
 * @param delta   Probabilitatea de eroare (ex: 0.01)
 * @return Pointer la CMS sau NULL la eroare
 */
CountMinSketch *cms_create(double epsilon, double delta) {
    /* YOUR CODE HERE */
    
    CountMinSketch *cms = (CountMinSketch *)malloc(sizeof(CountMinSketch));
    if (!cms) return NULL;
    
    /* Calculați dimensiunile
     * Hint:
     * cms->width = (size_t)ceil(2.718281828 / epsilon);
     * cms->depth = (size_t)ceil(log(1.0 / delta));
     */
    
    /* Alocați matricea
     * Hint:
     * cms->counters = (int64_t **)malloc(cms->depth * sizeof(int64_t *));
     * for (size_t i = 0; i < cms->depth; i++) {
     *     cms->counters[i] = (int64_t *)calloc(cms->width, sizeof(int64_t));
     * }
     */
    
    return cms;  /* Înlocuiți cu implementarea completă */
}

/**
 * Eliberare memorie CMS
 */
void cms_free(CountMinSketch *cms) {
    if (cms) {
        if (cms->counters) {
            for (size_t i = 0; i < cms->depth; i++) {
                free(cms->counters[i]);
            }
            free(cms->counters);
        }
        free(cms);
    }
}

/**
 * TODO 3: Implementați funcția de adăugare element
 *
 * Algoritm:
 *   Pentru fiecare rând i (0 la depth-1):
 *     1. Calculați hash_i = hash_family(data, len, i)
 *     2. Calculați index = hash_i % width
 *     3. Incrementați counters[i][index] cu count
 *   Adăugați count la total_count
 *
 * @param cms   Count-Min Sketch
 * @param data  Pointer la date
 * @param len   Lungimea datelor
 * @param count Numărul de apariții
 */
void cms_add(CountMinSketch *cms, const void *data, size_t len, int64_t count) {
    /* YOUR CODE HERE */
    
    /* Hint:
     * for (size_t i = 0; i < cms->depth; i++) {
     *     uint64_t hash = hash_family(data, len, (uint32_t)i);
     *     size_t index = hash % cms->width;
     *     cms->counters[i][index] += count;
     * }
     * cms->total_count += count;
     */
}

/**
 * TODO 4: Implementați funcția de estimare frecvență
 *
 * Algoritm:
 *   min_count = INFINIT
 *   Pentru fiecare rând i:
 *     1. Calculați hash_i și index
 *     2. Dacă counters[i][index] < min_count:
 *        min_count = counters[i][index]
 *   Returnați min_count
 *
 * @param cms  Count-Min Sketch
 * @param data Pointer la date
 * @param len  Lungimea datelor
 * @return Frecvența estimată
 */
int64_t cms_estimate(const CountMinSketch *cms, const void *data, size_t len) {
    /* YOUR CODE HERE */
    
    /* Hint:
     * int64_t min_count = INT64_MAX;
     * for (size_t i = 0; i < cms->depth; i++) {
     *     uint64_t hash = hash_family(data, len, (uint32_t)i);
     *     size_t index = hash % cms->width;
     *     if (cms->counters[i][index] < min_count) {
     *         min_count = cms->counters[i][index];
     *     }
     * }
     * return min_count;
     */
    
    return 0;  /* Placeholder - înlocuiți! */
}

/* =============================================================================
 * STRUCTURĂ PENTRU NUMĂRARE EXACTĂ (PENTRU VALIDARE)
 * =============================================================================
 */

/**
 * Nod pentru hash table cu chaining
 */
typedef struct ExactNode {
    char *word;
    int64_t count;
    struct ExactNode *next;
} ExactNode;

/**
 * Hash table pentru numărare exactă
 */
typedef struct {
    ExactNode **buckets;
    size_t size;
    size_t num_elements;
} ExactCounter;

/**
 * Creare counter exact (furnizată)
 */
ExactCounter *exact_create(size_t size) {
    ExactCounter *ec = (ExactCounter *)malloc(sizeof(ExactCounter));
    if (!ec) return NULL;
    
    ec->buckets = (ExactNode **)calloc(size, sizeof(ExactNode *));
    if (!ec->buckets) {
        free(ec);
        return NULL;
    }
    
    ec->size = size;
    ec->num_elements = 0;
    
    return ec;
}

/**
 * Eliberare counter exact (furnizată)
 */
void exact_free(ExactCounter *ec) {
    if (ec) {
        for (size_t i = 0; i < ec->size; i++) {
            ExactNode *node = ec->buckets[i];
            while (node) {
                ExactNode *next = node->next;
                free(node->word);
                free(node);
                node = next;
            }
        }
        free(ec->buckets);
        free(ec);
    }
}

/**
 * Adăugare în counter exact (furnizată)
 */
void exact_add(ExactCounter *ec, const char *word) {
    size_t hash = hash_fnv1a(word, strlen(word)) % ec->size;
    
    /* Caută dacă există */
    ExactNode *node = ec->buckets[hash];
    while (node) {
        if (strcmp(node->word, word) == 0) {
            node->count++;
            return;
        }
        node = node->next;
    }
    
    /* Creează nod nou */
    ExactNode *new_node = (ExactNode *)malloc(sizeof(ExactNode));
    new_node->word = strdup(word);
    new_node->count = 1;
    new_node->next = ec->buckets[hash];
    ec->buckets[hash] = new_node;
    ec->num_elements++;
}

/**
 * Obține count exact (furnizată)
 */
int64_t exact_get(const ExactCounter *ec, const char *word) {
    size_t hash = hash_fnv1a(word, strlen(word)) % ec->size;
    
    ExactNode *node = ec->buckets[hash];
    while (node) {
        if (strcmp(node->word, word) == 0) {
            return node->count;
        }
        node = node->next;
    }
    
    return 0;
}

/* =============================================================================
 * STRUCTURI PENTRU TOP-K HEAVY HITTERS
 * =============================================================================
 */

typedef struct {
    char word[MAX_WORD_LENGTH];
    int64_t estimated_count;
    int64_t exact_count;
    double error_percent;
} HeavyHitter;

typedef struct {
    HeavyHitter items[TOP_K];
    int count;
} TopKResult;

/* =============================================================================
 * FUNCȚII AUXILIARE
 * =============================================================================
 */

/**
 * Convertește string la lowercase și elimină caractere non-alfanumerice
 */
void normalize_word(char *word) {
    char *src = word;
    char *dst = word;
    
    while (*src) {
        if (isalnum((unsigned char)*src)) {
            *dst = tolower((unsigned char)*src);
            dst++;
        }
        src++;
    }
    *dst = '\0';
}

/**
 * TODO 5: Implementați găsirea Top-K heavy hitters
 *
 * Algoritm:
 *   1. Iterați prin toate cuvintele unice din ExactCounter
 *   2. Pentru fiecare cuvânt:
 *      a. Obțineți frecvența estimată din CMS
 *      b. Obțineți frecvența exactă din ExactCounter
 *      c. Dacă frecvența estimată e mai mare decât minimul din Top-K:
 *         - Adăugați în Top-K (sortare descrescătoare)
 *   3. Calculați eroarea pentru fiecare heavy hitter
 *
 * @param cms    Count-Min Sketch
 * @param ec     ExactCounter pentru validare
 * @param result Output: structura TopKResult
 */
void find_top_k(const CountMinSketch *cms, const ExactCounter *ec, 
                TopKResult *result) {
    /* YOUR CODE HERE */
    
    result->count = 0;
    
    /* Iterăm prin toate bucket-urile */
    for (size_t i = 0; i < ec->size; i++) {
        ExactNode *node = ec->buckets[i];
        while (node) {
            int64_t estimated = cms_estimate(cms, node->word, strlen(node->word));
            int64_t exact = node->count;
            
            /* TODO: Verificați dacă acest cuvânt ar trebui să fie în Top-K */
            /* Hint: Mențineți Top-K sortat descrescător după frecvență estimată */
            
            /* Placeholder - doar primele TOP_K găsite */
            if (result->count < TOP_K) {
                strncpy(result->items[result->count].word, node->word, 
                        MAX_WORD_LENGTH - 1);
                result->items[result->count].estimated_count = estimated;
                result->items[result->count].exact_count = exact;
                
                if (exact > 0) {
                    result->items[result->count].error_percent = 
                        ((double)(estimated - exact) / exact) * 100.0;
                } else {
                    result->items[result->count].error_percent = 0.0;
                }
                
                result->count++;
            }
            
            node = node->next;
        }
    }
    
    /* TODO: Sortați rezultatele descrescător după estimated_count */
    /* Hint: Folosiți bubble sort sau qsort */
}

/* =============================================================================
 * PROCESARE STREAM
 * =============================================================================
 */

typedef struct {
    uint64_t total_words;
    size_t unique_words;
    size_t cms_memory;
    double epsilon;
    double delta;
    TopKResult top_k;
} StreamAnalysisReport;

/**
 * TODO 6: Implementați funcția de procesare stream
 *
 * @param filename Calea către fișier
 * @param epsilon  Eroarea CMS
 * @param delta    Probabilitatea de eroare CMS
 * @param report   Output: raportul de analiză
 * @return 0 la succes, -1 la eroare
 */
int process_stream(const char *filename, double epsilon, double delta,
                   StreamAnalysisReport *report) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Eroare: Nu pot deschide fișierul %s\n", filename);
        return -1;
    }
    
    /* Creează structurile de date */
    CountMinSketch *cms = cms_create(epsilon, delta);
    ExactCounter *ec = exact_create(EXACT_HASH_SIZE);
    
    if (!cms || !ec) {
        fprintf(stderr, "Eroare la alocarea structurilor de date\n");
        if (cms) cms_free(cms);
        if (ec) exact_free(ec);
        fclose(fp);
        return -1;
    }
    
    /* Procesează fișierul */
    char word[MAX_WORD_LENGTH];
    report->total_words = 0;
    
    while (fscanf(fp, "%255s", word) == 1) {
        normalize_word(word);
        
        if (strlen(word) == 0) continue;
        
        /* Adaugă în ambele structuri */
        cms_add(cms, word, strlen(word), 1);
        exact_add(ec, word);
        
        report->total_words++;
    }
    
    /* Completează raportul */
    report->unique_words = ec->num_elements;
    report->cms_memory = cms->depth * cms->width * sizeof(int64_t);
    report->epsilon = epsilon;
    report->delta = delta;
    
    /* Găsește Top-K */
    find_top_k(cms, ec, &report->top_k);
    
    /* Cleanup */
    cms_free(cms);
    exact_free(ec);
    fclose(fp);
    
    return 0;
}

/* =============================================================================
 * AFIȘARE RAPORT
 * =============================================================================
 */

void print_report(const StreamAnalysisReport *report) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                 Heavy Hitters Analysis                        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    printf("  Stream Statistics:\n");
    printf("    Total words:      %lu\n", report->total_words);
    printf("    Unique words:     %zu\n", report->unique_words);
    printf("\n");
    printf("  CMS Parameters:\n");
    printf("    Epsilon (ε):      %.4f\n", report->epsilon);
    printf("    Delta (δ):        %.4f\n", report->delta);
    printf("    Memory used:      %.2f KB\n", report->cms_memory / 1024.0);
    printf("\n");
    printf("  Top-%d Heavy Hitters:\n", TOP_K);
    printf("  %-6s %-20s %12s %12s %10s\n", 
           "Rank", "Word", "Estimated", "Exact", "Error");
    printf("  %-6s %-20s %12s %12s %10s\n", 
           "────", "────────────────────", "─────────", "─────", "─────");
    
    for (int i = 0; i < report->top_k.count; i++) {
        printf("  %-6d %-20s %12ld %12ld %9.2f%%\n",
               i + 1,
               report->top_k.items[i].word,
               report->top_k.items[i].estimated_count,
               report->top_k.items[i].exact_count,
               report->top_k.items[i].error_percent);
    }
    printf("\n");
}

/* =============================================================================
 * MAIN
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    printf("\n=== EXERCIȚIUL 2: Analizator de Frecvențe (Heavy Hitters) ===\n");
    
    /* Parsare argumente */
    const char *filename;
    double epsilon = DEFAULT_EPSILON;
    double delta = DEFAULT_DELTA;
    
    if (argc < 2) {
        printf("Utilizare: %s <fisier_cuvinte> [epsilon] [delta]\n", argv[0]);
        printf("Folosim fișierul implicit pentru demonstrație...\n");
        filename = "data/words_stream.txt";
    } else {
        filename = argv[1];
    }
    
    if (argc >= 3) epsilon = atof(argv[2]);
    if (argc >= 4) delta = atof(argv[3]);
    
    printf("\nParametri:\n");
    printf("  Fișier:    %s\n", filename);
    printf("  Epsilon:   %.4f (eroare maximă %.2f%%)\n", epsilon, epsilon * 100);
    printf("  Delta:     %.4f (încredere %.2f%%)\n", delta, (1.0 - delta) * 100);
    
    /* Procesare */
    StreamAnalysisReport report = {0};
    
    if (process_stream(filename, epsilon, delta, &report) != 0) {
        fprintf(stderr, "Eroare la procesarea stream-ului!\n");
        return 1;
    }
    
    /* Afișare raport */
    print_report(&report);
    
    return 0;
}
