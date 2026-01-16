/**
 * =============================================================================
 * SOLUȚIE TEMA 1: Sistem de Caching Inteligent cu Bloom Filter
 * =============================================================================
 *
 * Această soluție demonstrează un sistem de caching cu două niveluri care
 * utilizează Bloom Filter pentru a evita accesul inutil la baza de date.
 *
 * ARHITECTURA:
 *   Request → [Bloom Filter] → NOT FOUND → Skip DB lookup (economie!)
 *                ↓ MAYBE
 *            [L1 Cache] → HIT → Return cached value
 *                ↓ MISS
 *            [Database] → Found? → Add to L1 & BF
 *
 * IMPLEMENTĂRI:
 *   - Bloom Filter standard pentru membership testing
 *   - Counting Bloom Filter pentru suport delete
 *   - LRU Cache ca L1 cache
 *   - Simulare pattern Zipf
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -O2 -o homework1_sol homework1_sol.c -lm
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
 * CONSTANTE ȘI CONFIGURARE
 * =============================================================================
 */

#define MAX_KEY_LENGTH 256
#define MAX_VALUE_LENGTH 1024
#define DEFAULT_CACHE_CAPACITY 1000
#define DEFAULT_EXPECTED_ITEMS 100000
#define DEFAULT_FPR 0.01

/* =============================================================================
 * FUNCȚII HASH
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
 * BLOOM FILTER STANDARD
 * =============================================================================
 */

typedef struct {
    uint8_t *bitmap;
    size_t size_bits;
    size_t size_bytes;
    size_t num_hashes;
    size_t elements_added;
} BloomFilter;

void bloom_calculate_params(size_t n, double fpr, size_t *out_m, size_t *out_k) {
    const double ln2_sq = 0.4804530139182014;
    double m = -((double)n * log(fpr)) / ln2_sq;
    double k = (m / (double)n) * 0.693147180559945;
    *out_m = (size_t)ceil(m);
    *out_k = (size_t)ceil(k);
    if (*out_k < 1) *out_k = 1;
}

BloomFilter *bloom_create(size_t expected_n, double target_fpr) {
    BloomFilter *bf = (BloomFilter *)malloc(sizeof(BloomFilter));
    if (!bf) return NULL;
    
    bloom_calculate_params(expected_n, target_fpr, &bf->size_bits, &bf->num_hashes);
    bf->size_bytes = (bf->size_bits + 7) / 8;
    bf->bitmap = (uint8_t *)calloc(bf->size_bytes, 1);
    if (!bf->bitmap) {
        free(bf);
        return NULL;
    }
    bf->elements_added = 0;
    return bf;
}

void bloom_add(BloomFilter *bf, const void *data, size_t len) {
    for (size_t i = 0; i < bf->num_hashes; i++) {
        uint64_t hash = hash_family(data, len, (uint32_t)i);
        size_t bit_index = hash % bf->size_bits;
        bf->bitmap[bit_index / 8] |= (1 << (bit_index % 8));
    }
    bf->elements_added++;
}

bool bloom_check(const BloomFilter *bf, const void *data, size_t len) {
    for (size_t i = 0; i < bf->num_hashes; i++) {
        uint64_t hash = hash_family(data, len, (uint32_t)i);
        size_t bit_index = hash % bf->size_bits;
        if (!(bf->bitmap[bit_index / 8] & (1 << (bit_index % 8)))) {
            return false;
        }
    }
    return true;
}

void bloom_free(BloomFilter *bf) {
    if (bf) {
        free(bf->bitmap);
        free(bf);
    }
}

/* =============================================================================
 * COUNTING BLOOM FILTER (Partea C)
 * 
 * Un Counting Bloom Filter extinde conceptul de Bloom Filter pentru a 
 * suporta operația de ștergere. În loc de biți, folosim contoare de 4 biți.
 *
 * Avantaje:
 *   - Suportă ștergere (bloom standard nu poate)
 *   - Păstrează proprietatea de "no false negatives"
 *
 * Dezavantaje:
 *   - 4× mai multă memorie decât Bloom standard
 *   - Risc de overflow la contoare (counter saturation)
 *
 * Implementare: Două contoare de 4 biți per byte (packed nibbles)
 * =============================================================================
 */

typedef struct {
    uint8_t *counters;      /* Contoare de 4 biți (2 per byte) */
    size_t size;            /* Numărul de contoare */
    size_t size_bytes;      /* Dimensiune în bytes */
    size_t num_hashes;      /* k funcții hash */
    size_t elements_added;  /* Contor elemente */
} CountingBloomFilter;

/**
 * Creează un Counting Bloom Filter
 * 
 * Folosim aceeași dimensionare ca Bloom standard, dar cu contoare în loc de biți.
 */
CountingBloomFilter *cbf_create(size_t expected_n, double target_fpr) {
    CountingBloomFilter *cbf = (CountingBloomFilter *)malloc(sizeof(CountingBloomFilter));
    if (!cbf) return NULL;
    
    size_t m, k;
    bloom_calculate_params(expected_n, target_fpr, &m, &k);
    
    cbf->size = m;
    cbf->size_bytes = (m + 1) / 2;  /* 2 contoare de 4 biți per byte */
    cbf->num_hashes = k;
    cbf->elements_added = 0;
    
    cbf->counters = (uint8_t *)calloc(cbf->size_bytes, 1);
    if (!cbf->counters) {
        free(cbf);
        return NULL;
    }
    
    return cbf;
}

/**
 * Obține valoarea contorului la poziția index
 * 
 * Layout: byte = [counter_odd (4 biți high) | counter_even (4 biți low)]
 */
static uint8_t cbf_get_counter(const CountingBloomFilter *cbf, size_t index) {
    size_t byte_idx = index / 2;
    if (index % 2 == 0) {
        /* Nibble low (biții 0-3) */
        return cbf->counters[byte_idx] & 0x0F;
    } else {
        /* Nibble high (biții 4-7) */
        return (cbf->counters[byte_idx] >> 4) & 0x0F;
    }
}

/**
 * Setează valoarea contorului la poziția index
 */
static void cbf_set_counter(CountingBloomFilter *cbf, size_t index, uint8_t value) {
    /* Saturare la 15 pentru a evita overflow */
    if (value > 15) value = 15;
    
    size_t byte_idx = index / 2;
    if (index % 2 == 0) {
        /* Nibble low */
        cbf->counters[byte_idx] = (cbf->counters[byte_idx] & 0xF0) | value;
    } else {
        /* Nibble high */
        cbf->counters[byte_idx] = (cbf->counters[byte_idx] & 0x0F) | (value << 4);
    }
}

/**
 * Adaugă un element în CBF
 * 
 * Pentru fiecare poziție determinată de funcțiile hash, incrementăm contorul.
 */
void cbf_add(CountingBloomFilter *cbf, const void *data, size_t len) {
    for (size_t i = 0; i < cbf->num_hashes; i++) {
        uint64_t hash = hash_family(data, len, (uint32_t)i);
        size_t index = hash % cbf->size;
        
        uint8_t current = cbf_get_counter(cbf, index);
        if (current < 15) {  /* Evităm overflow */
            cbf_set_counter(cbf, index, current + 1);
        }
    }
    cbf->elements_added++;
}

/**
 * Șterge un element din CBF
 * 
 * Returnează true dacă ștergerea a reușit (elementul era probabil prezent),
 * false dacă un contor ar fi devenit negativ (element absent).
 *
 * ATENȚIE: Ștergerea unui element ne-adăugat corupe structura!
 */
bool cbf_remove(CountingBloomFilter *cbf, const void *data, size_t len) {
    /* Prima dată verificăm dacă elementul pare prezent */
    for (size_t i = 0; i < cbf->num_hashes; i++) {
        uint64_t hash = hash_family(data, len, (uint32_t)i);
        size_t index = hash % cbf->size;
        
        if (cbf_get_counter(cbf, index) == 0) {
            /* Un contor este 0 - elementul sigur nu există */
            return false;
        }
    }
    
    /* Decrementăm toate contoarele */
    for (size_t i = 0; i < cbf->num_hashes; i++) {
        uint64_t hash = hash_family(data, len, (uint32_t)i);
        size_t index = hash % cbf->size;
        
        uint8_t current = cbf_get_counter(cbf, index);
        if (current > 0) {
            cbf_set_counter(cbf, index, current - 1);
        }
    }
    
    if (cbf->elements_added > 0) {
        cbf->elements_added--;
    }
    return true;
}

/**
 * Verifică apartenența în CBF
 */
bool cbf_check(const CountingBloomFilter *cbf, const void *data, size_t len) {
    for (size_t i = 0; i < cbf->num_hashes; i++) {
        uint64_t hash = hash_family(data, len, (uint32_t)i);
        size_t index = hash % cbf->size;
        
        if (cbf_get_counter(cbf, index) == 0) {
            return false;
        }
    }
    return true;
}

void cbf_free(CountingBloomFilter *cbf) {
    if (cbf) {
        free(cbf->counters);
        free(cbf);
    }
}

/* =============================================================================
 * LRU CACHE (L1 Cache)
 * 
 * Implementare simplă folosind doubly linked list + hash table.
 * - Inserare: O(1)
 * - Lookup: O(1) amortizat
 * - Evicție: O(1) - scoatem din coada listei
 * =============================================================================
 */

typedef struct LRUNode {
    char *key;
    char *value;
    struct LRUNode *prev;
    struct LRUNode *next;
    struct LRUNode *hash_next;  /* Pentru hash chain */
} LRUNode;

typedef struct {
    LRUNode *head;          /* Most recently used */
    LRUNode *tail;          /* Least recently used */
    LRUNode **hash_table;   /* Hash table pentru lookup O(1) */
    size_t hash_size;       /* Dimensiunea hash table */
    size_t capacity;        /* Capacitatea maximă */
    size_t size;            /* Dimensiunea curentă */
} LRUCache;

LRUCache *lru_create(size_t capacity) {
    LRUCache *cache = (LRUCache *)malloc(sizeof(LRUCache));
    if (!cache) return NULL;
    
    cache->capacity = capacity;
    cache->size = 0;
    cache->head = NULL;
    cache->tail = NULL;
    cache->hash_size = capacity * 2 + 1;  /* Load factor ~0.5 */
    
    cache->hash_table = (LRUNode **)calloc(cache->hash_size, sizeof(LRUNode *));
    if (!cache->hash_table) {
        free(cache);
        return NULL;
    }
    
    return cache;
}

/**
 * Mută un nod în capul listei (most recently used)
 */
static void lru_move_to_head(LRUCache *cache, LRUNode *node) {
    if (node == cache->head) return;  /* Deja în cap */
    
    /* Scoatem nodul din poziția curentă */
    if (node->prev) node->prev->next = node->next;
    if (node->next) node->next->prev = node->prev;
    if (node == cache->tail) cache->tail = node->prev;
    
    /* Inserăm în cap */
    node->prev = NULL;
    node->next = cache->head;
    if (cache->head) cache->head->prev = node;
    cache->head = node;
    if (!cache->tail) cache->tail = node;
}

/**
 * Găsește în hash table
 */
static LRUNode *lru_hash_find(LRUCache *cache, const char *key) {
    size_t hash = hash_fnv1a(key, strlen(key)) % cache->hash_size;
    
    LRUNode *node = cache->hash_table[hash];
    while (node) {
        if (strcmp(node->key, key) == 0) {
            return node;
        }
        node = node->hash_next;
    }
    return NULL;
}

/**
 * Adaugă în hash table
 */
static void lru_hash_insert(LRUCache *cache, LRUNode *node) {
    size_t hash = hash_fnv1a(node->key, strlen(node->key)) % cache->hash_size;
    node->hash_next = cache->hash_table[hash];
    cache->hash_table[hash] = node;
}

/**
 * Șterge din hash table
 */
static void lru_hash_remove(LRUCache *cache, LRUNode *node) {
    size_t hash = hash_fnv1a(node->key, strlen(node->key)) % cache->hash_size;
    
    LRUNode **ptr = &cache->hash_table[hash];
    while (*ptr) {
        if (*ptr == node) {
            *ptr = node->hash_next;
            return;
        }
        ptr = &(*ptr)->hash_next;
    }
}

/**
 * Obține valoarea pentru o cheie (returnează NULL dacă nu există)
 */
char *lru_get(LRUCache *cache, const char *key) {
    LRUNode *node = lru_hash_find(cache, key);
    if (!node) return NULL;
    
    /* Mutăm în cap (recently used) */
    lru_move_to_head(cache, node);
    return node->value;
}

/**
 * Inserează sau actualizează o pereche cheie-valoare
 */
void lru_put(LRUCache *cache, const char *key, const char *value) {
    LRUNode *node = lru_hash_find(cache, key);
    
    if (node) {
        /* Actualizăm valoarea existentă */
        free(node->value);
        node->value = strdup(value);
        lru_move_to_head(cache, node);
        return;
    }
    
    /* Creem nod nou */
    node = (LRUNode *)malloc(sizeof(LRUNode));
    node->key = strdup(key);
    node->value = strdup(value);
    node->prev = NULL;
    node->next = cache->head;
    node->hash_next = NULL;
    
    if (cache->head) cache->head->prev = node;
    cache->head = node;
    if (!cache->tail) cache->tail = node;
    
    lru_hash_insert(cache, node);
    cache->size++;
    
    /* Evicție dacă depășim capacitatea */
    if (cache->size > cache->capacity) {
        LRUNode *evict = cache->tail;
        if (evict) {
            /* Scoatem din lista */
            if (evict->prev) evict->prev->next = NULL;
            cache->tail = evict->prev;
            if (cache->head == evict) cache->head = NULL;
            
            /* Scoatem din hash */
            lru_hash_remove(cache, evict);
            
            /* Eliberăm memorie */
            free(evict->key);
            free(evict->value);
            free(evict);
            cache->size--;
        }
    }
}

void lru_free(LRUCache *cache) {
    if (cache) {
        LRUNode *node = cache->head;
        while (node) {
            LRUNode *next = node->next;
            free(node->key);
            free(node->value);
            free(node);
            node = next;
        }
        free(cache->hash_table);
        free(cache);
    }
}

/* =============================================================================
 * CACHE SYSTEM (Integrare completă)
 * =============================================================================
 */

typedef struct {
    BloomFilter *filter;            /* Bloom Filter pentru negative lookups */
    CountingBloomFilter *cbf;       /* Counting BF pentru suport delete */
    LRUCache *l1_cache;             /* L1 cache */
    size_t cache_hits;              /* Hit-uri în L1 */
    size_t cache_misses;            /* Miss-uri în L1 */
    size_t bloom_negatives;         /* Lookup-uri DB evitate datorită BF */
    size_t false_positives;         /* False positive-uri detectate */
    size_t total_requests;          /* Total cereri procesate */
    size_t db_lookups;              /* Lookup-uri în DB efectuate */
} CacheSystem;

/**
 * Crează sistemul de caching
 */
CacheSystem *cache_create(size_t expected_items, double fpr, size_t cache_capacity) {
    CacheSystem *cs = (CacheSystem *)malloc(sizeof(CacheSystem));
    if (!cs) return NULL;
    
    cs->filter = bloom_create(expected_items, fpr);
    cs->cbf = cbf_create(expected_items, fpr);
    cs->l1_cache = lru_create(cache_capacity);
    
    if (!cs->filter || !cs->cbf || !cs->l1_cache) {
        if (cs->filter) bloom_free(cs->filter);
        if (cs->cbf) cbf_free(cs->cbf);
        if (cs->l1_cache) lru_free(cs->l1_cache);
        free(cs);
        return NULL;
    }
    
    cs->cache_hits = 0;
    cs->cache_misses = 0;
    cs->bloom_negatives = 0;
    cs->false_positives = 0;
    cs->total_requests = 0;
    cs->db_lookups = 0;
    
    return cs;
}

/**
 * Simulare lookup în baza de date
 * În realitate, aceasta ar fi o funcție externă
 */
typedef const char *(*DBLookupFunc)(const char *key);

/* Database simplu pentru simulare */
typedef struct DBEntry {
    char *key;
    char *value;
    struct DBEntry *next;
} DBEntry;

static DBEntry *simulated_db = NULL;

void db_put(const char *key, const char *value) {
    DBEntry *entry = (DBEntry *)malloc(sizeof(DBEntry));
    entry->key = strdup(key);
    entry->value = strdup(value);
    entry->next = simulated_db;
    simulated_db = entry;
}

const char *db_get(const char *key) {
    DBEntry *entry = simulated_db;
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }
    return NULL;
}

void db_free(void) {
    DBEntry *entry = simulated_db;
    while (entry) {
        DBEntry *next = entry->next;
        free(entry->key);
        free(entry->value);
        free(entry);
        entry = next;
    }
    simulated_db = NULL;
}

/**
 * GET operation - Logica principală de caching
 *
 * Flow:
 * 1. Verifică Bloom Filter
 *    - Dacă BF zice "nu există" → return NULL (economie lookup DB)
 * 2. Verifică L1 Cache
 *    - Dacă găsit → return din cache (hit)
 * 3. Lookup în DB
 *    - Dacă găsit → adaugă în cache și BF, return valoare
 *    - Dacă nu → BF a dat false positive
 */
char *cache_get(CacheSystem *cs, const char *key, DBLookupFunc db_lookup) {
    cs->total_requests++;
    size_t key_len = strlen(key);
    
    /* PASUL 1: Verifică Bloom Filter */
    if (!bloom_check(cs->filter, key, key_len)) {
        /* BF zice SIGUR că nu există - evităm lookup-ul în DB */
        cs->bloom_negatives++;
        return NULL;
    }
    
    /* PASUL 2: BF zice "poate există" - verifică L1 Cache */
    char *cached = lru_get(cs->l1_cache, key);
    if (cached) {
        cs->cache_hits++;
        return cached;
    }
    
    /* PASUL 3: Cache miss - trebuie să accesăm DB */
    cs->cache_misses++;
    cs->db_lookups++;
    
    const char *db_value = db_lookup(key);
    if (db_value) {
        /* Găsit în DB - adăugăm în cache */
        lru_put(cs->l1_cache, key, db_value);
        return lru_get(cs->l1_cache, key);
    } else {
        /* NU există în DB - BF a dat false positive */
        cs->false_positives++;
        return NULL;
    }
}

/**
 * PUT operation - Adaugă în sistem
 */
void cache_put(CacheSystem *cs, const char *key, const char *value) {
    size_t key_len = strlen(key);
    
    /* Adăugăm în toate structurile */
    bloom_add(cs->filter, key, key_len);
    cbf_add(cs->cbf, key, key_len);
    lru_put(cs->l1_cache, key, value);
    
    /* Simulăm adăugarea în DB */
    db_put(key, value);
}

/**
 * DELETE operation (folosind Counting Bloom Filter)
 */
bool cache_delete(CacheSystem *cs, const char *key) {
    size_t key_len = strlen(key);
    
    /* Ștergem din CBF (BF standard nu suportă delete) */
    return cbf_remove(cs->cbf, key, key_len);
}

/**
 * Afișare statistici
 */
void cache_print_stats(const CacheSystem *cs) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                    Cache System Report                         ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    printf("  Performance Metrics:\n");
    printf("    Total requests:          %zu\n", cs->total_requests);
    printf("    Cache hits (L1):         %zu (%.2f%%)\n", 
           cs->cache_hits, 
           cs->total_requests > 0 ? 100.0 * cs->cache_hits / cs->total_requests : 0);
    printf("    Cache misses:            %zu (%.2f%%)\n",
           cs->cache_misses,
           cs->total_requests > 0 ? 100.0 * cs->cache_misses / cs->total_requests : 0);
    printf("\n");
    
    printf("  Bloom Filter Savings:\n");
    printf("    DB lookups avoided:      %zu (guaranteed non-existent)\n", cs->bloom_negatives);
    printf("    Actual DB lookups:       %zu\n", cs->db_lookups);
    printf("    False positives:         %zu (%.2f%%)\n",
           cs->false_positives,
           cs->db_lookups > 0 ? 100.0 * cs->false_positives / cs->db_lookups : 0);
    printf("    Theoretical FPR:         1.00%%\n");
    printf("\n");
    
    size_t bf_memory = cs->filter->size_bytes;
    size_t cbf_memory = cs->cbf->size_bytes;
    size_t cache_memory = cs->l1_cache->capacity * (MAX_KEY_LENGTH + MAX_VALUE_LENGTH);
    
    printf("  Memory Usage:\n");
    printf("    Bloom Filter:            %.2f KB\n", bf_memory / 1024.0);
    printf("    Counting BF:             %.2f KB\n", cbf_memory / 1024.0);
    printf("    L1 Cache (estimated):    %.2f KB\n", cache_memory / 1024.0);
    printf("    Total:                   %.2f KB\n", 
           (bf_memory + cbf_memory + cache_memory) / 1024.0);
    printf("\n");
    
    /* Estimare economii */
    size_t saved_lookups = cs->bloom_negatives;
    printf("  Efficiency Analysis:\n");
    printf("    DB lookups saved:        %zu\n", saved_lookups);
    printf("    Estimated time saved:    %.2f ms (assuming 1ms/lookup)\n", 
           saved_lookups * 1.0);
    printf("\n");
}

void cache_free(CacheSystem *cs) {
    if (cs) {
        bloom_free(cs->filter);
        cbf_free(cs->cbf);
        lru_free(cs->l1_cache);
        free(cs);
    }
}

/* =============================================================================
 * SIMULARE DISTRIBUȚIE ZIPF (Partea B)
 *
 * Distribuția Zipf modelează "long tail" din accesul la date reale:
 *   - 20% din chei primesc 80% din cereri
 *   - Câteva elemente sunt foarte populare
 *   - Majoritatea sunt accesate rar
 *
 * Formula: P(rank k) ∝ 1/k^α, unde α ≈ 1
 * =============================================================================
 */

/**
 * Generator de numere cu distribuție Zipf
 */
typedef struct {
    double *cdf;        /* Cumulative Distribution Function */
    size_t n;           /* Numărul de elemente */
    double alpha;       /* Parametrul Zipf (tipic 1.0) */
} ZipfGenerator;

ZipfGenerator *zipf_create(size_t n, double alpha) {
    ZipfGenerator *zg = (ZipfGenerator *)malloc(sizeof(ZipfGenerator));
    if (!zg) return NULL;
    
    zg->n = n;
    zg->alpha = alpha;
    zg->cdf = (double *)malloc(n * sizeof(double));
    if (!zg->cdf) {
        free(zg);
        return NULL;
    }
    
    /* Calculăm constanta de normalizare */
    double sum = 0.0;
    for (size_t i = 1; i <= n; i++) {
        sum += 1.0 / pow((double)i, alpha);
    }
    
    /* Construim CDF */
    double cumulative = 0.0;
    for (size_t i = 0; i < n; i++) {
        cumulative += (1.0 / pow((double)(i + 1), alpha)) / sum;
        zg->cdf[i] = cumulative;
    }
    
    return zg;
}

/**
 * Generează un index conform distribuției Zipf
 */
size_t zipf_sample(ZipfGenerator *zg) {
    double r = (double)rand() / RAND_MAX;
    
    /* Căutare binară în CDF */
    size_t low = 0, high = zg->n;
    while (low < high) {
        size_t mid = (low + high) / 2;
        if (zg->cdf[mid] < r) {
            low = mid + 1;
        } else {
            high = mid;
        }
    }
    
    return low;
}

void zipf_free(ZipfGenerator *zg) {
    if (zg) {
        free(zg->cdf);
        free(zg);
    }
}

/* =============================================================================
 * SIMULARE COMPLETĂ
 * =============================================================================
 */

void run_simulation(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║          SIMULARE: Cache System cu Distribuție Zipf           ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    srand((unsigned int)time(NULL));
    
    /* Configurare */
    const size_t NUM_KEYS = 10000;       /* Numărul total de chei în "univers" */
    const size_t NUM_DB_KEYS = 5000;     /* Chei care există efectiv în DB */
    const size_t NUM_REQUESTS = 100000;  /* Numărul de cereri de simulat */
    const size_t CACHE_CAPACITY = 1000;  /* Capacitatea L1 cache */
    const double FPR = 0.01;             /* Target false positive rate */
    
    printf("  Configurare:\n");
    printf("    Total keys universe:     %zu\n", NUM_KEYS);
    printf("    Keys in database:        %zu\n", NUM_DB_KEYS);
    printf("    Simulation requests:     %zu\n", NUM_REQUESTS);
    printf("    L1 cache capacity:       %zu\n", CACHE_CAPACITY);
    printf("    Target FPR:              %.2f%%\n", FPR * 100);
    printf("\n");
    
    /* Creare sistem */
    CacheSystem *cs = cache_create(NUM_DB_KEYS, FPR, CACHE_CAPACITY);
    if (!cs) {
        fprintf(stderr, "Eroare la crearea sistemului de cache!\n");
        return;
    }
    
    /* Populare DB cu primele NUM_DB_KEYS chei */
    printf("  Populare database cu %zu chei...\n", NUM_DB_KEYS);
    for (size_t i = 0; i < NUM_DB_KEYS; i++) {
        char key[64], value[128];
        snprintf(key, sizeof(key), "user:%zu", i);
        snprintf(value, sizeof(value), "{\"id\":%zu,\"data\":\"sample_%zu\"}", i, i);
        cache_put(cs, key, value);
    }
    
    /* Creare generator Zipf */
    ZipfGenerator *zipf = zipf_create(NUM_KEYS, 1.0);
    if (!zipf) {
        fprintf(stderr, "Eroare la crearea generatorului Zipf!\n");
        cache_free(cs);
        return;
    }
    
    /* Reset statistici după populare */
    cs->cache_hits = 0;
    cs->cache_misses = 0;
    cs->bloom_negatives = 0;
    cs->false_positives = 0;
    cs->total_requests = 0;
    cs->db_lookups = 0;
    
    /* Simulare cereri */
    printf("  Simulare %zu cereri cu distribuție Zipf...\n\n", NUM_REQUESTS);
    
    clock_t start = clock();
    
    for (size_t i = 0; i < NUM_REQUESTS; i++) {
        size_t key_idx = zipf_sample(zipf);
        char key[64];
        snprintf(key, sizeof(key), "user:%zu", key_idx);
        
        cache_get(cs, key, db_get);
    }
    
    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
    
    /* Afișare rezultate */
    cache_print_stats(cs);
    
    printf("  Simulation Performance:\n");
    printf("    Time elapsed:            %.3f seconds\n", elapsed);
    printf("    Throughput:              %.0f requests/sec\n", NUM_REQUESTS / elapsed);
    printf("\n");
    
    /* Cleanup */
    zipf_free(zipf);
    cache_free(cs);
    db_free();
}

/* =============================================================================
 * DEMONSTRARE COUNTING BLOOM FILTER
 * =============================================================================
 */

void demonstrate_counting_bf(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║          DEMONSTRARE: Counting Bloom Filter (Delete)          ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    CountingBloomFilter *cbf = cbf_create(1000, 0.01);
    
    /* Adăugăm elemente */
    const char *items[] = {"apple", "banana", "cherry", "date", "elderberry"};
    int n = 5;
    
    printf("  Adăugăm elemente:\n");
    for (int i = 0; i < n; i++) {
        cbf_add(cbf, items[i], strlen(items[i]));
        printf("    + %s\n", items[i]);
    }
    
    printf("\n  Verificăm apartenența:\n");
    for (int i = 0; i < n; i++) {
        bool present = cbf_check(cbf, items[i], strlen(items[i]));
        printf("    %s: %s\n", items[i], present ? "✓ DA" : "✗ NU");
    }
    
    /* Ștergem câteva elemente */
    printf("\n  Ștergem 'banana' și 'date':\n");
    cbf_remove(cbf, "banana", strlen("banana"));
    cbf_remove(cbf, "date", strlen("date"));
    
    printf("\n  Verificăm din nou:\n");
    for (int i = 0; i < n; i++) {
        bool present = cbf_check(cbf, items[i], strlen(items[i]));
        printf("    %s: %s\n", items[i], present ? "✓ DA" : "✗ NU");
    }
    
    /* Verificare element inexistent */
    printf("\n  Element inexistent ('mango'): %s\n",
           cbf_check(cbf, "mango", strlen("mango")) ? "✓ DA" : "✗ NU");
    
    printf("\n  ✓ Counting BF suportă delete, spre deosebire de BF standard!\n");
    printf("  ✓ Trade-off: 4× mai multă memorie (contoare vs biți)\n\n");
    
    cbf_free(cbf);
}

/* =============================================================================
 * MAIN
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║   SOLUȚIE TEMA 1: Sistem de Caching Inteligent cu Bloom Filter ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    /* Demonstrare Counting Bloom Filter */
    demonstrate_counting_bf();
    
    /* Simulare completă */
    run_simulation();
    
    (void)argc;
    (void)argv;
    
    return 0;
}
