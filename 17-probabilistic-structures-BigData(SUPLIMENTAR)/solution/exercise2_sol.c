/**
 * =============================================================================
 * SOLUȚIE EXERCIȚIUL 2: Analizator de Frecvențe în Stream (Heavy Hitters)
 * =============================================================================
 *
 * Această soluție demonstrează implementarea unui sistem de identificare
 * "heavy hitters" (elementele cele mai frecvente) într-un stream de date
 * folosind Count-Min Sketch.
 *
 * CONCEPTE CHEIE IMPLEMENTATE:
 *   - Count-Min Sketch cu parametri ε (eroare) și δ (probabilitate)
 *   - Double hashing pentru familia de funcții hash
 *   - Identificarea Top-K heavy hitters
 *   - Validare prin comparație cu numărare exactă
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -O2 -o exercise2_sol exercise2_sol.c -lm
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
#include <time.h>

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
 * FUNCȚII HASH
 * =============================================================================
 */

/**
 * FNV-1a Hash - funcție hash non-criptografică
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

/**
 * DJB2 Hash
 */
uint64_t hash_djb2(const void *data, size_t len) {
    const uint8_t *bytes = (const uint8_t *)data;
    uint64_t hash = 5381;
    
    for (size_t i = 0; i < len; i++) {
        hash = ((hash << 5) + hash) + bytes[i];
    }
    
    return hash;
}

/**
 * Familia de hash-uri folosind tehnica double hashing
 * h_i(x) = h1(x) + i * h2(x)
 */
uint64_t hash_family(const void *data, size_t len, uint32_t i) {
    uint64_t h1 = hash_fnv1a(data, len);
    uint64_t h2 = hash_djb2(data, len);
    return h1 + i * h2;
}

/* =============================================================================
 * STRUCTURA COUNT-MIN SKETCH - SOLUȚIE COMPLETĂ
 * =============================================================================
 */

typedef struct {
    int64_t **counters;        /* Matrice depth × width */
    size_t depth;              /* d = numărul de rânduri */
    size_t width;              /* w = numărul de coloane */
    double epsilon;            /* ε = eroarea relativă maximă */
    double delta;              /* δ = probabilitatea de depășire eroare */
    uint64_t total_count;      /* N = totalul elementelor procesate */
} CountMinSketch;

/**
 * SOLUȚIE TODO 2: Crearea Count-Min Sketch
 *
 * Parametrii sunt derivați din garanțiile teoretice:
 *
 * Garanție: Cu probabilitate 1-δ, eroarea este ≤ ε*N
 *   unde N = numărul total de elemente în stream
 *
 * Formule de dimensionare:
 *   width (w) = ⌈e/ε⌉ ≈ 2.718/ε
 *     - Determină precizia estimării
 *     - Mai mare w → mai mică eroarea
 *
 *   depth (d) = ⌈ln(1/δ)⌉
 *     - Determină probabilitatea de succes
 *     - Mai mare d → mai sigură garanția
 *
 * Exemplu pentru ε=0.01, δ=0.01:
 *   w = ⌈2.718/0.01⌉ = 272
 *   d = ⌈ln(100)⌉ = ⌈4.605⌉ = 5
 *   Memorie = 272 × 5 × 8 bytes = 10.9 KB
 */
CountMinSketch *cms_create(double epsilon, double delta) {
    CountMinSketch *cms = (CountMinSketch *)malloc(sizeof(CountMinSketch));
    if (!cms) return NULL;
    
    /* Calculul dimensiunilor conform formulelor teoretice
     * e = baza logaritmului natural ≈ 2.718281828
     */
    cms->width = (size_t)ceil(2.718281828 / epsilon);
    cms->depth = (size_t)ceil(log(1.0 / delta));
    
    /* Asigurăm minimum rezonabil */
    if (cms->width < 100) cms->width = 100;
    if (cms->depth < 3) cms->depth = 3;
    
    /* Salvăm parametrii */
    cms->epsilon = epsilon;
    cms->delta = delta;
    cms->total_count = 0;
    
    /* Alocarea matricei de contoare
     * Folosim alocarea pe rânduri pentru flexibilitate
     */
    cms->counters = (int64_t **)malloc(cms->depth * sizeof(int64_t *));
    if (!cms->counters) {
        free(cms);
        return NULL;
    }
    
    /* Alocarea fiecărui rând și inițializare cu 0 */
    for (size_t i = 0; i < cms->depth; i++) {
        cms->counters[i] = (int64_t *)calloc(cms->width, sizeof(int64_t));
        if (!cms->counters[i]) {
            /* Cleanup în caz de eroare */
            for (size_t j = 0; j < i; j++) {
                free(cms->counters[j]);
            }
            free(cms->counters);
            free(cms);
            return NULL;
        }
    }
    
    return cms;
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
 * SOLUȚIE TODO 3: Adăugarea unui element în Count-Min Sketch
 *
 * Algoritmul:
 *   Pentru fiecare rând i (0..d-1):
 *     1. Calculăm h_i(element) folosind double hashing
 *     2. Indexul în rândul i: j = h_i mod w
 *     3. Incrementăm counters[i][j] cu count
 *
 * Complexitate: O(d) operații hash și incrementări
 *
 * Proprietatea cheie: Fiecare rând folosește o funcție hash independentă,
 * astfel coliziunile într-un rând nu afectează alte rânduri.
 */
void cms_add(CountMinSketch *cms, const void *data, size_t len, int64_t count) {
    /* Parcurgem fiecare rând al matricei */
    for (size_t i = 0; i < cms->depth; i++) {
        /* Calculăm hash-ul folosind funcția i din familie */
        uint64_t hash = hash_family(data, len, (uint32_t)i);
        
        /* Calculăm indexul în rândul curent */
        size_t index = hash % cms->width;
        
        /* Incrementăm contorul corespunzător */
        cms->counters[i][index] += count;
    }
    
    /* Actualizăm totalul elementelor procesate */
    cms->total_count += count;
}

/**
 * SOLUȚIE TODO 4: Estimarea frecvenței unui element
 *
 * Algoritmul:
 *   1. Pentru fiecare rând, găsim contorul corespunzător
 *   2. Returnăm MINIMUL dintre toate aceste contoare
 *
 * Motivația pentru minimum:
 *   - Fiecare contor poate avea coliziuni (supraestimează)
 *   - CMS NICIODATĂ nu subescitează (numai adaugă, nu scade)
 *   - Minimul minimizează efectul coliziunilor
 *
 * Garanție teoretică:
 *   count(x) ≤ estimate(x) ≤ count(x) + ε*N cu probabilitate 1-δ
 *
 * Complexitate: O(d) operații hash și comparații
 */
int64_t cms_estimate(const CountMinSketch *cms, const void *data, size_t len) {
    int64_t min_count = INT64_MAX;
    
    /* Parcurgem fiecare rând și găsim minimul */
    for (size_t i = 0; i < cms->depth; i++) {
        /* Calculăm hash-ul cu funcția i */
        uint64_t hash = hash_family(data, len, (uint32_t)i);
        
        /* Calculăm indexul în rândul curent */
        size_t index = hash % cms->width;
        
        /* Actualizăm minimul */
        if (cms->counters[i][index] < min_count) {
            min_count = cms->counters[i][index];
        }
    }
    
    return min_count;
}

/**
 * Utilitar: Raportare statistici CMS
 */
void cms_print_stats(const CountMinSketch *cms) {
    printf("\n  Count-Min Sketch Statistics:\n");
    printf("    Width (w):         %zu\n", cms->width);
    printf("    Depth (d):         %zu\n", cms->depth);
    printf("    Epsilon (ε):       %.4f\n", cms->epsilon);
    printf("    Delta (δ):         %.4f\n", cms->delta);
    printf("    Total count (N):   %lu\n", cms->total_count);
    printf("    Memory:            %.2f KB\n", 
           (cms->depth * cms->width * sizeof(int64_t)) / 1024.0);
    printf("    Max error bound:   ≤ %.0f (ε×N)\n", cms->epsilon * cms->total_count);
}

/* =============================================================================
 * STRUCTURĂ PENTRU NUMĂRARE EXACTĂ (PENTRU VALIDARE)
 * =============================================================================
 */

typedef struct ExactNode {
    char *word;
    int64_t count;
    struct ExactNode *next;
} ExactNode;

typedef struct {
    ExactNode **buckets;
    size_t size;
    size_t num_elements;
} ExactCounter;

/**
 * Creare counter exact (hash table cu chaining)
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
 * Eliberare counter exact
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
 * Adăugare în counter exact
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
 * Obține count exact
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

/**
 * Comparator pentru sortarea heavy hitters
 */
int compare_heavy_hitters(const void *a, const void *b) {
    const HeavyHitter *ha = (const HeavyHitter *)a;
    const HeavyHitter *hb = (const HeavyHitter *)b;
    
    /* Sortare descrescătoare după estimare */
    if (hb->estimated_count > ha->estimated_count) return 1;
    if (hb->estimated_count < ha->estimated_count) return -1;
    return 0;
}

/**
 * Normalizare cuvânt: lowercase și eliminare caractere non-alfanumerice
 */
void normalize_word(char *word) {
    char *src = word;
    char *dst = word;
    
    while (*src) {
        if (isalnum((unsigned char)*src)) {
            *dst++ = tolower((unsigned char)*src);
        }
        src++;
    }
    *dst = '\0';
}

/* =============================================================================
 * FUNCȚIA PRINCIPALĂ DE PROCESARE
 * =============================================================================
 */

/**
 * Procesare stream de cuvinte și identificare heavy hitters
 *
 * Algoritmul în două faze:
 *
 * FAZA 1 (Single pass): Procesare stream
 *   - Citim fiecare cuvânt
 *   - Adăugăm în CMS pentru estimare
 *   - Adăugăm în hash table exact pentru validare
 *   - Menținem un set de candidați pentru top-k
 *
 * FAZA 2: Identificare Top-K
 *   - Parcurgem toți candidații
 *   - Estimăm frecvența fiecăruia cu CMS
 *   - Sortăm și returnăm top-k
 */
int process_stream(const char *filename, double epsilon, double delta,
                   TopKResult *result, uint64_t *total_words) {
    /* Deschidere fișier */
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Eroare: Nu pot deschide fișierul %s\n", filename);
        return -1;
    }
    
    /* Creare structuri de date */
    CountMinSketch *cms = cms_create(epsilon, delta);
    ExactCounter *exact = exact_create(EXACT_HASH_SIZE);
    
    if (!cms || !exact) {
        fprintf(stderr, "Eroare: Nu pot aloca structurile de date\n");
        if (cms) cms_free(cms);
        if (exact) exact_free(exact);
        fclose(fp);
        return -1;
    }
    
    /* Procesare stream cuvânt cu cuvânt */
    char word[MAX_WORD_LENGTH];
    *total_words = 0;
    
    while (fscanf(fp, "%255s", word) == 1) {
        /* Normalizare: lowercase și caractere alfanumerice */
        normalize_word(word);
        
        /* Ignorăm cuvinte goale sau prea scurte */
        size_t len = strlen(word);
        if (len < 2) continue;
        
        (*total_words)++;
        
        /* Adăugăm în ambele structuri */
        cms_add(cms, word, len, 1);
        exact_add(exact, word);
    }
    
    fclose(fp);
    
    /* Afișare statistici CMS */
    cms_print_stats(cms);
    
    /* Identificare Top-K heavy hitters
     * 
     * Parcurgem toate elementele din hash table-ul exact
     * și selectăm pe cele cu frecvența estimată cea mai mare
     */
    HeavyHitter *candidates = (HeavyHitter *)malloc(
        exact->num_elements * sizeof(HeavyHitter));
    size_t num_candidates = 0;
    
    for (size_t i = 0; i < exact->size; i++) {
        ExactNode *node = exact->buckets[i];
        while (node) {
            candidates[num_candidates].estimated_count = 
                cms_estimate(cms, node->word, strlen(node->word));
            candidates[num_candidates].exact_count = node->count;
            strncpy(candidates[num_candidates].word, node->word, MAX_WORD_LENGTH - 1);
            candidates[num_candidates].word[MAX_WORD_LENGTH - 1] = '\0';
            
            /* Calculăm eroarea relativă */
            if (node->count > 0) {
                double error = (double)(candidates[num_candidates].estimated_count - 
                                       node->count);
                candidates[num_candidates].error_percent = 
                    (error / (double)node->count) * 100.0;
            } else {
                candidates[num_candidates].error_percent = 0.0;
            }
            
            num_candidates++;
            node = node->next;
        }
    }
    
    /* Sortare descrescătoare după frecvența estimată */
    qsort(candidates, num_candidates, sizeof(HeavyHitter), compare_heavy_hitters);
    
    /* Copiere top-k în rezultat */
    result->count = (num_candidates < TOP_K) ? num_candidates : TOP_K;
    for (int i = 0; i < result->count; i++) {
        result->items[i] = candidates[i];
    }
    
    /* Eliberare memorie */
    free(candidates);
    cms_free(cms);
    exact_free(exact);
    
    return 0;
}

/* =============================================================================
 * FUNCȚII DE AFIȘARE
 * =============================================================================
 */

void print_results(const TopKResult *result, uint64_t total_words) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════════╗\n");
    printf("║                    Heavy Hitters Analysis Report                      ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    printf("  Total words processed: %lu\n", total_words);
    printf("\n");
    printf("  ┌──────┬──────────────────┬─────────────┬─────────────┬───────────┐\n");
    printf("  │ Rank │ Word             │ Estimated   │ Exact       │ Error %%   │\n");
    printf("  ├──────┼──────────────────┼─────────────┼─────────────┼───────────┤\n");
    
    for (int i = 0; i < result->count; i++) {
        printf("  │ %4d │ %-16s │ %11ld │ %11ld │ %8.2f%% │\n",
               i + 1,
               result->items[i].word,
               result->items[i].estimated_count,
               result->items[i].exact_count,
               result->items[i].error_percent);
    }
    
    printf("  └──────┴──────────────────┴─────────────┴─────────────┴───────────┘\n");
    printf("\n");
    
    /* Calculare eroare medie */
    double total_error = 0.0;
    for (int i = 0; i < result->count; i++) {
        total_error += result->items[i].error_percent;
    }
    double avg_error = result->count > 0 ? total_error / result->count : 0.0;
    
    printf("  Average error for Top-%d:  %.2f%%\n", result->count, avg_error);
    printf("\n");
}

/* =============================================================================
 * DEMONSTRARE PROPRIETĂȚI COUNT-MIN SKETCH
 * =============================================================================
 */

void demonstrate_cms_properties(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════════╗\n");
    printf("║          Demonstrare Proprietăți Count-Min Sketch                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    CountMinSketch *cms = cms_create(0.01, 0.01);
    
    /* Adăugăm elemente cu frecvențe cunoscute */
    const char *items[] = {"alpha", "beta", "gamma", "delta", "epsilon"};
    int counts[] = {1000, 500, 250, 100, 50};
    int n_items = 5;
    
    printf("  Adăugăm elemente cu frecvențe cunoscute:\n\n");
    
    for (int i = 0; i < n_items; i++) {
        for (int j = 0; j < counts[i]; j++) {
            cms_add(cms, items[i], strlen(items[i]), 1);
        }
        printf("    %s: count exact = %d\n", items[i], counts[i]);
    }
    
    printf("\n  Estimări Count-Min Sketch:\n\n");
    
    for (int i = 0; i < n_items; i++) {
        int64_t estimate = cms_estimate(cms, items[i], strlen(items[i]));
        double error = ((double)(estimate - counts[i]) / counts[i]) * 100.0;
        
        printf("    %s: estimat = %ld, exact = %d, eroare = %.2f%%\n",
               items[i], estimate, counts[i], error);
    }
    
    printf("\n  Element inexistent:\n");
    const char *missing = "inexistent";
    int64_t false_positive = cms_estimate(cms, missing, strlen(missing));
    printf("    %s: estimat = %ld (false positive din coliziuni)\n",
           missing, false_positive);
    
    printf("\n  Proprietăți demonstrate:\n");
    printf("    ✓ Niciodată nu subescitează (estimat ≥ exact)\n");
    printf("    ✓ Eroarea este mărginită de ε×N = %.0f\n", cms->epsilon * cms->total_count);
    printf("    ✓ Complexitate: O(d) per operație, memorie O(w×d)\n");
    
    cms_free(cms);
    printf("\n");
}

/* =============================================================================
 * MAIN
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    printf("\n=== SOLUȚIE EXERCIȚIUL 2: Heavy Hitters cu Count-Min Sketch ===\n");
    
    /* Demonstrare proprietăți teoretice */
    demonstrate_cms_properties();
    
    /* Verificare argumente */
    const char *filename;
    if (argc < 2) {
        printf("Utilizare: %s <fisier_cuvinte> [epsilon] [delta]\n", argv[0]);
        printf("Folosim fișierul implicit pentru demonstrație...\n");
        filename = "../data/words_stream.txt";
    } else {
        filename = argv[1];
    }
    
    double epsilon = DEFAULT_EPSILON;
    double delta = DEFAULT_DELTA;
    
    if (argc >= 3) epsilon = atof(argv[2]);
    if (argc >= 4) delta = atof(argv[3]);
    
    printf("\nParametri:\n");
    printf("  Fișier:           %s\n", filename);
    printf("  Epsilon (ε):      %.4f (eroare relativă max)\n", epsilon);
    printf("  Delta (δ):        %.4f (probabilitate eroare)\n", delta);
    
    /* Procesare */
    TopKResult result = {0};
    uint64_t total_words = 0;
    clock_t start = clock();
    
    if (process_stream(filename, epsilon, delta, &result, &total_words) != 0) {
        fprintf(stderr, "Eroare la procesarea stream-ului!\n");
        return 1;
    }
    
    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
    
    /* Afișare rezultate */
    print_results(&result, total_words);
    
    printf("  Processing time:  %.3f seconds\n", elapsed);
    printf("  Throughput:       %.0f words/sec\n\n", total_words / elapsed);
    
    return 0;
}
