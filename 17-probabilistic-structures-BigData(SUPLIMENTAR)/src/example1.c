/**
 * =============================================================================
 * SĂPTĂMÂNA 17: Structuri de Date Probabilistice pentru Big Data
 * Exemplu Complet și Demonstrativ
 * =============================================================================
 *
 * Acest exemplu demonstrează implementarea completă a:
 *   1. Funcții Hash (FNV-1a, DJB2, double hashing)
 *   2. Bloom Filter pentru membership testing
 *   3. Count-Min Sketch pentru estimare frecvențe
 *   4. HyperLogLog pentru estimare cardinalitate
 *   5. Skip List pentru căutare/inserare O(log n)
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -g -o example1 example1.c -lm
 * Execuție:  ./example1
 * Valgrind:  valgrind --leak-check=full ./example1
 *
 * Autor: Curs ATP - ASE București CSIE
 * Data:  Ianuarie 2026
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <time.h>

/* =============================================================================
 * PARTEA 1: FUNCȚII HASH
 * =============================================================================
 */

/**
 * FNV-1a Hash - Fowler-Noll-Vo hash function
 * 
 * Proprietăți: Distribuție excelentă, simplu, rapid pentru stringuri
 * Utilizare: Bloom Filter, hash tables
 * 
 * @param data Pointer la date
 * @param len  Lungimea datelor în bytes
 * @return Hash pe 64 de biți
 */
uint64_t hash_fnv1a(const void *data, size_t len) {
    const uint8_t *bytes = (const uint8_t *)data;
    uint64_t hash = 14695981039346656037ULL;  /* FNV offset basis */
    const uint64_t prime = 1099511628211ULL;  /* FNV prime */
    
    for (size_t i = 0; i < len; i++) {
        hash ^= bytes[i];
        hash *= prime;
    }
    
    return hash;
}

/**
 * DJB2 Hash - Daniel J. Bernstein hash function
 * 
 * Proprietăți: Foarte simplu, bun pentru stringuri scurte
 * Formula: hash = hash * 33 + c
 * 
 * @param data Pointer la date
 * @param len  Lungimea datelor în bytes
 * @return Hash pe 64 de biți
 */
uint64_t hash_djb2(const void *data, size_t len) {
    const uint8_t *bytes = (const uint8_t *)data;
    uint64_t hash = 5381;
    
    for (size_t i = 0; i < len; i++) {
        hash = ((hash << 5) + hash) + bytes[i];  /* hash * 33 + c */
    }
    
    return hash;
}

/**
 * Murmur3-like simplified hash
 * 
 * Acceptă un seed pentru a genera funcții hash diferite
 * 
 * @param data Pointer la date
 * @param len  Lungimea datelor
 * @param seed Seed pentru variație
 * @return Hash pe 64 de biți
 */
uint64_t hash_murmur_simple(const void *data, size_t len, uint32_t seed) {
    const uint8_t *bytes = (const uint8_t *)data;
    uint64_t h = seed ^ len;
    
    for (size_t i = 0; i < len; i++) {
        h ^= bytes[i];
        h *= 0x5bd1e9955bd1e995ULL;
        h ^= h >> 47;
    }
    
    h *= 0x5bd1e9955bd1e995ULL;
    h ^= h >> 47;
    
    return h;
}

/**
 * Generare funcție hash i din familia de hash-uri
 * Folosind tehnica Kirsch-Mitzenmacher: hash_i(x) = hash1(x) + i * hash2(x)
 * 
 * Acest lucru permite generarea a k funcții hash din doar 2 hash-uri de bază,
 * reducând calculul necesar pentru Bloom Filter.
 * 
 * @param data Pointer la date
 * @param len  Lungimea datelor
 * @param i    Indexul funcției hash (0, 1, 2, ...)
 * @return Hash pe 64 de biți
 */
uint64_t hash_family(const void *data, size_t len, uint32_t i) {
    uint64_t h1 = hash_fnv1a(data, len);
    uint64_t h2 = hash_djb2(data, len);
    return h1 + i * h2;
}

/* =============================================================================
 * PARTEA 2: BLOOM FILTER
 * =============================================================================
 */

/**
 * Structura Bloom Filter
 * 
 * Un Bloom Filter este o structură de date probabilistică pentru testarea
 * apartenenței. Răspunde la întrebarea: "Am văzut acest element înainte?"
 * 
 * Proprietăți:
 * - False negatives: IMPOSIBILE (dacă spunem NU, e garantat NU)
 * - False positives: Posibile, dar cu probabilitate controlabilă
 * - Ștergere: NU este suportată în varianta standard
 */
typedef struct {
    uint8_t *bitmap;           /* Array de biți */
    size_t size_bits;          /* Dimensiunea în biți (m) */
    size_t size_bytes;         /* Dimensiunea în bytes */
    size_t num_hashes;         /* Numărul de funcții hash (k) */
    size_t elements_added;     /* Contor elemente adăugate */
    double target_fpr;         /* Rata de false positives țintă */
} BloomFilter;

/**
 * Calculează parametrii optimi pentru Bloom Filter
 * 
 * Formule:
 *   m = -n * ln(p) / (ln(2))²
 *   k = (m/n) * ln(2)
 * 
 * @param n           Număr așteptat de elemente
 * @param fpr         Rata de false positives dorită (ex: 0.01 = 1%)
 * @param out_m       Output: numărul de biți
 * @param out_k       Output: numărul de funcții hash
 */
void bloom_calculate_params(size_t n, double fpr, size_t *out_m, size_t *out_k) {
    /* m = -n * ln(p) / (ln(2))² */
    double ln2_sq = 0.4804530139182014;  /* ln(2)² */
    double m = -(double)n * log(fpr) / ln2_sq;
    
    /* k = (m/n) * ln(2) ≈ 0.693 * (m/n) */
    double k = (m / (double)n) * 0.693147180559945;
    
    *out_m = (size_t)ceil(m);
    *out_k = (size_t)ceil(k);
    
    /* Asigură valori minime rezonabile */
    if (*out_k < 1) *out_k = 1;
    if (*out_m < 8) *out_m = 8;
}

/**
 * Creează un nou Bloom Filter
 * 
 * @param expected_n  Numărul așteptat de elemente
 * @param target_fpr  Rata de false positives dorită
 * @return Pointer la noul Bloom Filter sau NULL în caz de eroare
 */
BloomFilter *bloom_create(size_t expected_n, double target_fpr) {
    BloomFilter *bf = (BloomFilter *)malloc(sizeof(BloomFilter));
    if (!bf) return NULL;
    
    /* Calculează parametrii optimi */
    bloom_calculate_params(expected_n, target_fpr, &bf->size_bits, &bf->num_hashes);
    
    /* Alocă bitmap-ul */
    bf->size_bytes = (bf->size_bits + 7) / 8;  /* Rotunjire în sus */
    bf->bitmap = (uint8_t *)calloc(bf->size_bytes, 1);
    if (!bf->bitmap) {
        free(bf);
        return NULL;
    }
    
    bf->elements_added = 0;
    bf->target_fpr = target_fpr;
    
    return bf;
}

/**
 * Eliberează memoria unui Bloom Filter
 */
void bloom_free(BloomFilter *bf) {
    if (bf) {
        free(bf->bitmap);
        free(bf);
    }
}

/**
 * Setează un bit în bitmap
 */
static inline void bloom_set_bit(BloomFilter *bf, size_t index) {
    size_t byte_index = index / 8;
    size_t bit_offset = index % 8;
    bf->bitmap[byte_index] |= (1 << bit_offset);
}

/**
 * Verifică dacă un bit este setat
 */
static inline bool bloom_get_bit(const BloomFilter *bf, size_t index) {
    size_t byte_index = index / 8;
    size_t bit_offset = index % 8;
    return (bf->bitmap[byte_index] & (1 << bit_offset)) != 0;
}

/**
 * Adaugă un element în Bloom Filter
 * 
 * Pentru fiecare din cele k funcții hash, calculează poziția și setează bitul.
 * 
 * @param bf   Bloom Filter
 * @param data Pointer la date
 * @param len  Lungimea datelor
 */
void bloom_add(BloomFilter *bf, const void *data, size_t len) {
    for (size_t i = 0; i < bf->num_hashes; i++) {
        uint64_t hash = hash_family(data, len, (uint32_t)i);
        size_t index = hash % bf->size_bits;
        bloom_set_bit(bf, index);
    }
    bf->elements_added++;
}

/**
 * Verifică dacă un element ar putea fi în Bloom Filter
 * 
 * @param bf   Bloom Filter
 * @param data Pointer la date
 * @param len  Lungimea datelor
 * @return true dacă elementul PROBABIL este în set, false dacă SIGUR NU este
 */
bool bloom_check(const BloomFilter *bf, const void *data, size_t len) {
    for (size_t i = 0; i < bf->num_hashes; i++) {
        uint64_t hash = hash_family(data, len, (uint32_t)i);
        size_t index = hash % bf->size_bits;
        if (!bloom_get_bit(bf, index)) {
            return false;  /* Sigur NU este în set */
        }
    }
    return true;  /* Probabil este în set (poate fi false positive) */
}

/**
 * Estimează rata actuală de false positives
 * 
 * Formula: FPR ≈ (1 - e^(-kn/m))^k
 */
double bloom_estimate_fpr(const BloomFilter *bf) {
    double exponent = -(double)(bf->num_hashes * bf->elements_added) / 
                       (double)bf->size_bits;
    double base = 1.0 - exp(exponent);
    return pow(base, bf->num_hashes);
}

/**
 * Afișează statisticile unui Bloom Filter
 */
void bloom_print_stats(const BloomFilter *bf) {
    printf("Bloom Filter Statistics:\n");
    printf("  Size (bits):       %zu\n", bf->size_bits);
    printf("  Size (bytes):      %zu (%.2f KB)\n", 
           bf->size_bytes, bf->size_bytes / 1024.0);
    printf("  Hash functions:    %zu\n", bf->num_hashes);
    printf("  Elements added:    %zu\n", bf->elements_added);
    printf("  Target FPR:        %.4f%%\n", bf->target_fpr * 100);
    printf("  Estimated FPR:     %.4f%%\n", bloom_estimate_fpr(bf) * 100);
}

/* =============================================================================
 * PARTEA 3: COUNT-MIN SKETCH
 * =============================================================================
 */

/**
 * Structura Count-Min Sketch
 * 
 * Count-Min Sketch estimează frecvența elementelor într-un stream de date.
 * Folosește o matrice de contoare cu depth rânduri și width coloane.
 * 
 * Garanții:
 * - Niciodată subestimează (estimate >= count_real)
 * - Supraestimează cu maxim ε × N cu probabilitate 1 - δ
 */
typedef struct {
    int64_t **counters;        /* Matrice depth × width */
    size_t depth;              /* Numărul de rânduri (d) */
    size_t width;              /* Numărul de coloane (w) */
    double epsilon;            /* Eroarea relativă maximă */
    double delta;              /* Probabilitatea de a depăși eroarea */
    uint64_t total_count;      /* Suma totală a elementelor */
} CountMinSketch;

/**
 * Creează un nou Count-Min Sketch
 * 
 * @param epsilon Eroarea relativă maximă (ex: 0.01 pentru 1%)
 * @param delta   Probabilitatea de eroare (ex: 0.01 pentru 99% încredere)
 * @return Pointer la noul CMS sau NULL în caz de eroare
 */
CountMinSketch *cms_create(double epsilon, double delta) {
    CountMinSketch *cms = (CountMinSketch *)malloc(sizeof(CountMinSketch));
    if (!cms) return NULL;
    
    /* Calculează dimensiunile
     * width = ceil(e / epsilon)
     * depth = ceil(ln(1/delta))
     */
    cms->width = (size_t)ceil(2.718281828 / epsilon);
    cms->depth = (size_t)ceil(log(1.0 / delta));
    cms->epsilon = epsilon;
    cms->delta = delta;
    cms->total_count = 0;
    
    /* Alocă matricea de contoare */
    cms->counters = (int64_t **)malloc(cms->depth * sizeof(int64_t *));
    if (!cms->counters) {
        free(cms);
        return NULL;
    }
    
    for (size_t i = 0; i < cms->depth; i++) {
        cms->counters[i] = (int64_t *)calloc(cms->width, sizeof(int64_t));
        if (!cms->counters[i]) {
            /* Cleanup on error */
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
 * Eliberează memoria unui Count-Min Sketch
 */
void cms_free(CountMinSketch *cms) {
    if (cms) {
        for (size_t i = 0; i < cms->depth; i++) {
            free(cms->counters[i]);
        }
        free(cms->counters);
        free(cms);
    }
}

/**
 * Adaugă count apariții ale unui element
 * 
 * @param cms   Count-Min Sketch
 * @param data  Pointer la date
 * @param len   Lungimea datelor
 * @param count Numărul de apariții de adăugat
 */
void cms_add(CountMinSketch *cms, const void *data, size_t len, int64_t count) {
    for (size_t i = 0; i < cms->depth; i++) {
        uint64_t hash = hash_family(data, len, (uint32_t)i);
        size_t index = hash % cms->width;
        cms->counters[i][index] += count;
    }
    cms->total_count += count;
}

/**
 * Estimează frecvența unui element
 * 
 * Returnează minimul peste toate rândurile pentru a minimiza supraestimarea.
 * 
 * @param cms  Count-Min Sketch
 * @param data Pointer la date
 * @param len  Lungimea datelor
 * @return Frecvența estimată (>= frecvența reală)
 */
int64_t cms_estimate(const CountMinSketch *cms, const void *data, size_t len) {
    int64_t min_count = INT64_MAX;
    
    for (size_t i = 0; i < cms->depth; i++) {
        uint64_t hash = hash_family(data, len, (uint32_t)i);
        size_t index = hash % cms->width;
        if (cms->counters[i][index] < min_count) {
            min_count = cms->counters[i][index];
        }
    }
    
    return min_count;
}

/**
 * Afișează statisticile unui Count-Min Sketch
 */
void cms_print_stats(const CountMinSketch *cms) {
    size_t memory = cms->depth * cms->width * sizeof(int64_t);
    
    printf("Count-Min Sketch Statistics:\n");
    printf("  Dimensions:        %zu × %zu\n", cms->depth, cms->width);
    printf("  Memory used:       %zu bytes (%.2f KB)\n", memory, memory / 1024.0);
    printf("  Epsilon (error):   %.4f\n", cms->epsilon);
    printf("  Delta (confidence): %.4f\n", cms->delta);
    printf("  Total count:       %lu\n", cms->total_count);
}

/* =============================================================================
 * PARTEA 4: HYPERLOGLOG
 * =============================================================================
 */

/**
 * Structura HyperLogLog
 * 
 * HyperLogLog estimează cardinalitatea (numărul de elemente distincte)
 * folosind observația că în hash-uri uniform distribuite, probabilitatea
 * de a vedea k zerouri la început este 1/2^k.
 * 
 * Cu precision p, folosim M = 2^p registre și obținem eroare ~1.04/√M
 */
typedef struct {
    uint8_t *registers;        /* Array de M registre (max 6 biți fiecare) */
    size_t num_registers;      /* M = 2^precision */
    uint8_t precision;         /* p, tipic între 4 și 16 */
    double alpha;              /* Constantă de corecție bias */
} HyperLogLog;

/**
 * Calculează constanta alpha pentru corecție bias
 */
static double hll_alpha(size_t m) {
    if (m == 16) return 0.673;
    if (m == 32) return 0.697;
    if (m == 64) return 0.709;
    return 0.7213 / (1.0 + 1.079 / (double)m);
}

/**
 * Numără biții conducători zero (leading zeros) + 1
 * 
 * @param value Valoarea de analizat
 * @param bits  Numărul de biți de considerat
 * @return Poziția primului bit 1 (1-indexed)
 */
static uint8_t count_leading_zeros_plus_one(uint64_t value, int bits) {
    if (value == 0) return bits + 1;
    
    uint8_t count = 1;
    while ((value & (1ULL << (bits - 1))) == 0 && count <= bits) {
        count++;
        value <<= 1;
    }
    return count;
}

/**
 * Creează un nou HyperLogLog
 * 
 * @param precision Precizia p (4-16, implicit 14)
 * @return Pointer la noul HLL sau NULL în caz de eroare
 */
HyperLogLog *hll_create(uint8_t precision) {
    if (precision < 4 || precision > 18) {
        precision = 14;  /* Valoare implicită */
    }
    
    HyperLogLog *hll = (HyperLogLog *)malloc(sizeof(HyperLogLog));
    if (!hll) return NULL;
    
    hll->precision = precision;
    hll->num_registers = 1ULL << precision;  /* M = 2^p */
    hll->alpha = hll_alpha(hll->num_registers);
    
    hll->registers = (uint8_t *)calloc(hll->num_registers, sizeof(uint8_t));
    if (!hll->registers) {
        free(hll);
        return NULL;
    }
    
    return hll;
}

/**
 * Eliberează memoria unui HyperLogLog
 */
void hll_free(HyperLogLog *hll) {
    if (hll) {
        free(hll->registers);
        free(hll);
    }
}

/**
 * Adaugă un element în HyperLogLog
 * 
 * @param hll  HyperLogLog
 * @param data Pointer la date
 * @param len  Lungimea datelor
 */
void hll_add(HyperLogLog *hll, const void *data, size_t len) {
    uint64_t hash = hash_fnv1a(data, len);
    
    /* Primii p biți determină indexul registrului */
    size_t index = hash >> (64 - hll->precision);
    
    /* Restul biților pentru leading zeros */
    uint64_t remaining = hash << hll->precision;
    int remaining_bits = 64 - hll->precision;
    
    uint8_t rank = count_leading_zeros_plus_one(remaining, remaining_bits);
    
    /* Actualizează registrul dacă noul rank e mai mare */
    if (rank > hll->registers[index]) {
        hll->registers[index] = rank;
    }
}

/**
 * Estimează cardinalitatea (numărul de elemente distincte)
 * 
 * Formula: E = α × M² / Σ(2^(-reg[j]))
 * 
 * Include corecții pentru:
 * - Small range (linear counting)
 * - Large range (hash collision)
 * 
 * @return Cardinalitatea estimată
 */
double hll_count(const HyperLogLog *hll) {
    double sum = 0.0;
    size_t zeros = 0;
    
    for (size_t i = 0; i < hll->num_registers; i++) {
        sum += pow(2.0, -(double)hll->registers[i]);
        if (hll->registers[i] == 0) {
            zeros++;
        }
    }
    
    double m = (double)hll->num_registers;
    double estimate = hll->alpha * m * m / sum;
    
    /* Small range correction (linear counting) */
    if (estimate <= 2.5 * m && zeros > 0) {
        estimate = m * log(m / (double)zeros);
    }
    
    /* Large range correction */
    double two_32 = 4294967296.0;  /* 2^32 */
    if (estimate > two_32 / 30.0) {
        estimate = -two_32 * log(1.0 - estimate / two_32);
    }
    
    return estimate;
}

/**
 * Afișează statisticile unui HyperLogLog
 */
void hll_print_stats(const HyperLogLog *hll) {
    size_t memory = hll->num_registers * sizeof(uint8_t);
    
    printf("HyperLogLog Statistics:\n");
    printf("  Precision (p):     %d\n", hll->precision);
    printf("  Registers (M):     %zu\n", hll->num_registers);
    printf("  Memory used:       %zu bytes (%.2f KB)\n", memory, memory / 1024.0);
    printf("  Expected error:    %.2f%%\n", 104.0 / sqrt((double)hll->num_registers));
    printf("  Estimated count:   %.0f\n", hll_count(hll));
}

/* =============================================================================
 * PARTEA 5: SKIP LIST
 * =============================================================================
 */

#define SKIPLIST_MAX_LEVEL 16
#define SKIPLIST_P 0.5

/**
 * Structura unui nod din Skip List
 */
typedef struct SkipNode {
    int key;
    void *value;
    struct SkipNode **forward;  /* Array de pointeri la următorii la fiecare nivel */
    int level;                  /* Nivelul maxim al acestui nod */
} SkipNode;

/**
 * Structura Skip List
 */
typedef struct {
    SkipNode *header;          /* Nodul sentinel de început */
    int max_level;             /* Nivelul maxim permis */
    int current_level;         /* Nivelul maxim curent în listă */
    size_t size;               /* Numărul de elemente */
} SkipList;

/**
 * Creează un nod nou
 */
static SkipNode *skiplist_create_node(int level, int key, void *value) {
    SkipNode *node = (SkipNode *)malloc(sizeof(SkipNode));
    if (!node) return NULL;
    
    node->forward = (SkipNode **)malloc(sizeof(SkipNode *) * (level + 1));
    if (!node->forward) {
        free(node);
        return NULL;
    }
    
    for (int i = 0; i <= level; i++) {
        node->forward[i] = NULL;
    }
    
    node->key = key;
    node->value = value;
    node->level = level;
    
    return node;
}

/**
 * Generează nivelul aleator pentru un nod nou
 * 
 * Folosește o monedă virtuală: continuă să crească nivelul
 * atâta timp cât "dă ban" (cu probabilitatea p = 0.5)
 */
static int skiplist_random_level(void) {
    int level = 0;
    while ((double)rand() / RAND_MAX < SKIPLIST_P && level < SKIPLIST_MAX_LEVEL) {
        level++;
    }
    return level;
}

/**
 * Creează o nouă Skip List
 */
SkipList *skiplist_create(void) {
    SkipList *list = (SkipList *)malloc(sizeof(SkipList));
    if (!list) return NULL;
    
    list->max_level = SKIPLIST_MAX_LEVEL;
    list->current_level = 0;
    list->size = 0;
    
    /* Creează nodul header cu valoare sentinelă */
    list->header = skiplist_create_node(SKIPLIST_MAX_LEVEL, 0, NULL);
    if (!list->header) {
        free(list);
        return NULL;
    }
    
    return list;
}

/**
 * Eliberează memoria unui nod
 */
static void skiplist_free_node(SkipNode *node) {
    if (node) {
        free(node->forward);
        free(node);
    }
}

/**
 * Eliberează memoria unei Skip List
 */
void skiplist_free(SkipList *list) {
    if (!list) return;
    
    SkipNode *current = list->header->forward[0];
    while (current) {
        SkipNode *next = current->forward[0];
        skiplist_free_node(current);
        current = next;
    }
    
    skiplist_free_node(list->header);
    free(list);
}

/**
 * Caută un element în Skip List
 * 
 * Complexitate: O(log n) în medie
 * 
 * @return Pointer la nod sau NULL dacă nu există
 */
SkipNode *skiplist_search(const SkipList *list, int key) {
    SkipNode *current = list->header;
    
    /* Începe de la nivelul maxim și coboară */
    for (int i = list->current_level; i >= 0; i--) {
        while (current->forward[i] && current->forward[i]->key < key) {
            current = current->forward[i];
        }
    }
    
    /* Verifică dacă am găsit cheia */
    current = current->forward[0];
    if (current && current->key == key) {
        return current;
    }
    
    return NULL;
}

/**
 * Inserează un element în Skip List
 * 
 * Complexitate: O(log n) în medie
 * 
 * @return true dacă inserarea a reușit, false altfel
 */
bool skiplist_insert(SkipList *list, int key, void *value) {
    /* Array pentru a ține nodurile de actualizat */
    SkipNode *update[SKIPLIST_MAX_LEVEL + 1];
    SkipNode *current = list->header;
    
    /* Găsește pozițiile de actualizat la fiecare nivel */
    for (int i = list->current_level; i >= 0; i--) {
        while (current->forward[i] && current->forward[i]->key < key) {
            current = current->forward[i];
        }
        update[i] = current;
    }
    
    current = current->forward[0];
    
    /* Dacă cheia există deja, actualizează valoarea */
    if (current && current->key == key) {
        current->value = value;
        return true;
    }
    
    /* Generează nivel aleator pentru noul nod */
    int new_level = skiplist_random_level();
    
    /* Dacă noul nivel e mai mare, actualizează header-ul */
    if (new_level > list->current_level) {
        for (int i = list->current_level + 1; i <= new_level; i++) {
            update[i] = list->header;
        }
        list->current_level = new_level;
    }
    
    /* Creează noul nod */
    SkipNode *new_node = skiplist_create_node(new_level, key, value);
    if (!new_node) return false;
    
    /* Inserează nodul la toate nivelurile */
    for (int i = 0; i <= new_level; i++) {
        new_node->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = new_node;
    }
    
    list->size++;
    return true;
}

/**
 * Șterge un element din Skip List
 * 
 * @return true dacă elementul a fost șters, false dacă nu exista
 */
bool skiplist_delete(SkipList *list, int key) {
    SkipNode *update[SKIPLIST_MAX_LEVEL + 1];
    SkipNode *current = list->header;
    
    /* Găsește nodurile de actualizat */
    for (int i = list->current_level; i >= 0; i--) {
        while (current->forward[i] && current->forward[i]->key < key) {
            current = current->forward[i];
        }
        update[i] = current;
    }
    
    current = current->forward[0];
    
    /* Dacă cheia nu există */
    if (!current || current->key != key) {
        return false;
    }
    
    /* Actualizează pointerii */
    for (int i = 0; i <= list->current_level; i++) {
        if (update[i]->forward[i] != current) break;
        update[i]->forward[i] = current->forward[i];
    }
    
    skiplist_free_node(current);
    
    /* Actualizează nivelul curent dacă e necesar */
    while (list->current_level > 0 && !list->header->forward[list->current_level]) {
        list->current_level--;
    }
    
    list->size--;
    return true;
}

/**
 * Afișează Skip List-ul în format ASCII art
 */
void skiplist_print(const SkipList *list) {
    printf("\nSkip List Visualization (size=%zu):\n", list->size);
    printf("────────────────────────────────────────\n");
    
    for (int level = list->current_level; level >= 0; level--) {
        printf("Level %2d: HEAD", level);
        
        SkipNode *current = list->header->forward[0];
        SkipNode *level_node = list->header->forward[level];
        
        while (current) {
            if (current == level_node) {
                printf(" ─── %d", current->key);
                if (level_node) level_node = level_node->forward[level];
            } else {
                /* Calculează spațiul necesar */
                int digits = 1;
                int temp = current->key;
                while (temp >= 10) { digits++; temp /= 10; }
                printf(" %*s", 5 + digits, "");
            }
            current = current->forward[0];
        }
        printf(" ─── NIL\n");
    }
    printf("────────────────────────────────────────\n");
}

/**
 * Afișează statisticile unei Skip List
 */
void skiplist_print_stats(const SkipList *list) {
    printf("Skip List Statistics:\n");
    printf("  Elements:          %zu\n", list->size);
    printf("  Current level:     %d\n", list->current_level);
    printf("  Max level:         %d\n", list->max_level);
    printf("  Expected O(log n): %.2f\n", list->size > 0 ? log2((double)list->size) : 0);
}

/* =============================================================================
 * PARTEA 6: UTILITĂȚI DE AFIȘARE
 * =============================================================================
 */

void print_separator(const char *title) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║  %-61s║\n", title);
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

void print_subsection(const char *title) {
    printf("\n── %s ──\n\n", title);
}

/* =============================================================================
 * PARTEA 7: DEMONSTRAȚII
 * =============================================================================
 */

/**
 * Demonstrație Bloom Filter
 */
void demo_bloom_filter(void) {
    print_separator("DEMO 1: Bloom Filter");
    
    printf("Creăm un Bloom Filter pentru 10,000 elemente cu 1%% false positives\n\n");
    
    BloomFilter *bf = bloom_create(10000, 0.01);
    if (!bf) {
        printf("Eroare la crearea Bloom Filter!\n");
        return;
    }
    
    bloom_print_stats(bf);
    
    print_subsection("Adăugare elemente");
    
    /* Adaugă URL-uri */
    const char *urls[] = {
        "https://example.com/page1",
        "https://example.com/page2",
        "https://test.org/index",
        "https://demo.net/home",
        "https://sample.io/api"
    };
    int num_urls = sizeof(urls) / sizeof(urls[0]);
    
    for (int i = 0; i < num_urls; i++) {
        bloom_add(bf, urls[i], strlen(urls[i]));
        printf("  Adăugat: %s\n", urls[i]);
    }
    
    /* Adaugă mai multe elemente pentru statistici */
    char buffer[64];
    for (int i = 0; i < 1000; i++) {
        snprintf(buffer, sizeof(buffer), "https://gen%d.com/page%d", i, i * 7);
        bloom_add(bf, buffer, strlen(buffer));
    }
    printf("  + 1000 URL-uri generate adăugate\n");
    
    print_subsection("Verificare apartenență");
    
    /* Verifică URL-uri existente */
    for (int i = 0; i < num_urls; i++) {
        bool found = bloom_check(bf, urls[i], strlen(urls[i]));
        printf("  %s: %s\n", urls[i], found ? "PROBABIL în set" : "SIGUR NU în set");
    }
    
    /* Verifică URL-uri inexistente */
    const char *test_urls[] = {
        "https://notadded.com/test",
        "https://missing.org/page",
        "https://absent.net/item"
    };
    printf("\n  URL-uri NEADĂUGATE:\n");
    for (int i = 0; i < 3; i++) {
        bool found = bloom_check(bf, test_urls[i], strlen(test_urls[i]));
        printf("  %s: %s %s\n", test_urls[i], 
               found ? "PROBABIL în set" : "SIGUR NU în set",
               found ? "(FALSE POSITIVE!)" : "(corect)");
    }
    
    print_subsection("Statistici finale");
    bloom_print_stats(bf);
    
    bloom_free(bf);
}

/**
 * Demonstrație Count-Min Sketch
 */
void demo_count_min_sketch(void) {
    print_separator("DEMO 2: Count-Min Sketch");
    
    printf("Creăm un CMS cu ε=0.01 și δ=0.01 (99%% încredere, 1%% eroare)\n\n");
    
    CountMinSketch *cms = cms_create(0.01, 0.01);
    if (!cms) {
        printf("Eroare la crearea Count-Min Sketch!\n");
        return;
    }
    
    cms_print_stats(cms);
    
    print_subsection("Simulare stream de cuvinte");
    
    /* Simulează un stream cu frecvențe diferite */
    const char *words[] = {"the", "and", "to", "of", "a", "in", "is", "it"};
    int frequencies[] = {1000, 800, 600, 500, 400, 300, 200, 100};
    int num_words = sizeof(words) / sizeof(words[0]);
    
    printf("  Adăugăm cuvinte cu frecvențe cunoscute:\n");
    for (int i = 0; i < num_words; i++) {
        cms_add(cms, words[i], strlen(words[i]), frequencies[i]);
        printf("    \"%s\": %d apariții\n", words[i], frequencies[i]);
    }
    
    print_subsection("Estimare frecvențe");
    
    printf("  %-10s  %10s  %10s  %10s\n", "Cuvânt", "Estimat", "Real", "Eroare");
    printf("  %-10s  %10s  %10s  %10s\n", "──────", "───────", "────", "──────");
    
    for (int i = 0; i < num_words; i++) {
        int64_t estimated = cms_estimate(cms, words[i], strlen(words[i]));
        double error = ((double)(estimated - frequencies[i]) / frequencies[i]) * 100;
        printf("  %-10s  %10ld  %10d  %9.2f%%\n", 
               words[i], estimated, frequencies[i], error);
    }
    
    /* Verifică un cuvânt inexistent */
    const char *missing = "nonexistent";
    int64_t est_missing = cms_estimate(cms, missing, strlen(missing));
    printf("\n  Cuvânt inexistent \"%s\": estimat = %ld (coliziuni)\n", 
           missing, est_missing);
    
    print_subsection("Statistici finale");
    cms_print_stats(cms);
    
    cms_free(cms);
}

/**
 * Demonstrație HyperLogLog
 */
void demo_hyperloglog(void) {
    print_separator("DEMO 3: HyperLogLog");
    
    printf("Creăm un HyperLogLog cu precizie p=14 (16384 registre)\n\n");
    
    HyperLogLog *hll = hll_create(14);
    if (!hll) {
        printf("Eroare la crearea HyperLogLog!\n");
        return;
    }
    
    hll_print_stats(hll);
    
    print_subsection("Adăugare elemente unice");
    
    /* Adaugă elemente și urmărește evoluția estimării */
    int checkpoints[] = {100, 1000, 5000, 10000, 50000, 100000};
    int num_checkpoints = sizeof(checkpoints) / sizeof(checkpoints[0]);
    int checkpoint_idx = 0;
    
    char buffer[64];
    for (int i = 1; i <= 100000; i++) {
        snprintf(buffer, sizeof(buffer), "user_%d", i);
        hll_add(hll, buffer, strlen(buffer));
        
        if (checkpoint_idx < num_checkpoints && i == checkpoints[checkpoint_idx]) {
            double estimate = hll_count(hll);
            double error = fabs(estimate - i) / i * 100;
            printf("  După %6d elemente: estimat = %8.0f, eroare = %.2f%%\n", 
                   i, estimate, error);
            checkpoint_idx++;
        }
    }
    
    print_subsection("Test duplicate");
    
    /* Adaugă duplicate - nu ar trebui să schimbe estimarea semnificativ */
    double before = hll_count(hll);
    
    for (int i = 1; i <= 10000; i++) {
        snprintf(buffer, sizeof(buffer), "user_%d", i);  /* Duplicate! */
        hll_add(hll, buffer, strlen(buffer));
    }
    
    double after = hll_count(hll);
    printf("  Înainte de duplicate: %.0f\n", before);
    printf("  După 10,000 duplicate: %.0f\n", after);
    printf("  Diferența: %.0f (%.2f%%)\n", fabs(after - before), 
           fabs(after - before) / before * 100);
    
    print_subsection("Statistici finale");
    hll_print_stats(hll);
    
    hll_free(hll);
}

/**
 * Demonstrație Skip List
 */
void demo_skip_list(void) {
    print_separator("DEMO 4: Skip List");
    
    printf("Creăm o Skip List și adăugăm elemente\n\n");
    
    SkipList *list = skiplist_create();
    if (!list) {
        printf("Eroare la crearea Skip List!\n");
        return;
    }
    
    print_subsection("Inserare elemente");
    
    int values[] = {3, 6, 7, 9, 12, 17, 19, 21, 25, 26};
    int num_values = sizeof(values) / sizeof(values[0]);
    
    for (int i = 0; i < num_values; i++) {
        skiplist_insert(list, values[i], NULL);
        printf("  Inserat: %d\n", values[i]);
    }
    
    print_subsection("Vizualizare structură");
    skiplist_print(list);
    
    print_subsection("Căutare elemente");
    
    int search_keys[] = {7, 19, 26, 15, 100};
    for (int i = 0; i < 5; i++) {
        SkipNode *found = skiplist_search(list, search_keys[i]);
        printf("  Căutare %d: %s\n", search_keys[i], 
               found ? "GĂSIT" : "NU EXISTĂ");
    }
    
    print_subsection("Ștergere element");
    
    printf("  Ștergem elementul 12...\n");
    skiplist_delete(list, 12);
    skiplist_print(list);
    
    print_subsection("Statistici finale");
    skiplist_print_stats(list);
    
    skiplist_free(list);
}

/**
 * Comparație structuri probabilistice vs. exacte
 */
void demo_comparison(void) {
    print_separator("DEMO 5: Comparație Memorie");
    
    printf("Comparăm memoria folosită pentru 1,000,000 de elemente:\n\n");
    
    printf("  %-25s  %15s  %15s\n", "Structură", "Memorie", "Tip Eroare");
    printf("  %-25s  %15s  %15s\n", "─────────", "───────", "──────────");
    
    /* HashSet (simulat) - presupunem 50 bytes per element în medie */
    printf("  %-25s  %15s  %15s\n", "HashSet (exact)", "~50 MB", "0%");
    
    /* Bloom Filter */
    BloomFilter *bf = bloom_create(1000000, 0.01);
    printf("  %-25s  %12.2f KB  %15s\n", "Bloom Filter (1% FPR)", 
           bf->size_bytes / 1024.0, "~1% FP");
    bloom_free(bf);
    
    /* HyperLogLog */
    HyperLogLog *hll = hll_create(14);
    printf("  %-25s  %12.2f KB  %15s\n", "HyperLogLog (p=14)", 
           hll->num_registers * sizeof(uint8_t) / 1024.0, "~0.8%");
    hll_free(hll);
    
    /* Count-Min Sketch */
    CountMinSketch *cms = cms_create(0.01, 0.01);
    size_t cms_mem = cms->depth * cms->width * sizeof(int64_t);
    printf("  %-25s  %12.2f KB  %15s\n", "Count-Min Sketch", 
           cms_mem / 1024.0, "~1% over");
    cms_free(cms);
    
    printf("\n  Notă: Structurile probabilistice folosesc ~1000x mai puțină memorie!\n");
}

/* =============================================================================
 * PARTEA 8: MAIN
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    /* Inițializare generator de numere aleatoare */
    srand((unsigned int)time(NULL));
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║     SĂPTĂMÂNA 17: Structuri de Date Probabilistice pentru Big Data            ║\n");
    printf("║     Exemplu Demonstrativ Complet                                              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════════════════╝\n");
    
    /* Verifică argumente pentru benchmark */
    if (argc > 1) {
        if (strcmp(argv[1], "--benchmark-bloom") == 0) {
            /* Benchmark Bloom Filter */
            BloomFilter *bf = bloom_create(1000000, 0.01);
            char buffer[64];
            for (int i = 0; i < 100000; i++) {
                snprintf(buffer, sizeof(buffer), "item_%d", i);
                bloom_add(bf, buffer, strlen(buffer));
            }
            bloom_free(bf);
            return 0;
        }
        /* Alte benchmark-uri similare... */
    }
    
    /* Rulează toate demonstrațiile */
    demo_bloom_filter();
    demo_count_min_sketch();
    demo_hyperloglog();
    demo_skip_list();
    demo_comparison();
    
    print_separator("TOATE DEMONSTRAȚIILE COMPLETATE CU SUCCES!");
    
    printf("Acest exemplu a demonstrat:\n");
    printf("  ✓ Funcții hash pentru structuri probabilistice\n");
    printf("  ✓ Bloom Filter pentru membership testing\n");
    printf("  ✓ Count-Min Sketch pentru estimare frecvențe\n");
    printf("  ✓ HyperLogLog pentru estimare cardinalitate\n");
    printf("  ✓ Skip List pentru căutare/inserare O(log n)\n");
    printf("  ✓ Comparație memorie: probabilistic vs. exact\n");
    printf("\n");
    
    return 0;
}
