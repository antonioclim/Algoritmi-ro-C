#define _POSIX_C_SOURCE 200809L
/**
 * =============================================================================
 * SOLUȚIE TEMA 2: CACHE LRU (LEAST RECENTLY USED)
 * =============================================================================
 *
 * DOAR PENTRU INSTRUCTOR - NU DISTRIBUI STUDENȚILOR
 *
 * Implementare: Hash Table + Doubly Linked List
 * - Hash table pentru acces O(1)
 * - Doubly linked list pentru ordinea de utilizare
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -g -o homework2_sol homework2_sol.c
 * Utilizare: ./homework2_sol
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_KEY_LENGTH    64
#define MAX_VALUE_LENGTH  256
#define HASH_TABLE_SIZE   101

/* =============================================================================
 * STRUCTURI DE DATE
 * =============================================================================
 */

/**
 * Nod în doubly linked list + hash table entry
 */
typedef struct CacheNode {
    char *key;
    char *value;
    struct CacheNode *prev;      /* Pentru doubly linked list */
    struct CacheNode *next;      /* Pentru doubly linked list */
    struct CacheNode *hash_next; /* Pentru hash table chaining */
} CacheNode;

/**
 * LRU Cache
 */
typedef struct {
    CacheNode **hash_table;  /* Hash table pentru acces O(1) */
    size_t hash_size;        /* Dimensiunea hash table */
    
    CacheNode *head;         /* Cel mai recent folosit (MRU) */
    CacheNode *tail;         /* Cel mai puțin recent folosit (LRU) */
    
    size_t capacity;         /* Capacitatea maximă */
    size_t count;            /* Numărul curent de elemente */
    
    /* Statistici */
    size_t hits;
    size_t misses;
} LRUCache;

/* =============================================================================
 * FUNCȚII HASH
 * =============================================================================
 */

unsigned long hash_djb2(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

/* =============================================================================
 * FUNCȚII DOUBLY LINKED LIST
 * =============================================================================
 */

/**
 * Elimină un nod din lista (nu eliberează memoria)
 */
void list_remove(LRUCache *cache, CacheNode *node) {
    if (node->prev) {
        node->prev->next = node->next;
    } else {
        cache->head = node->next;
    }
    
    if (node->next) {
        node->next->prev = node->prev;
    } else {
        cache->tail = node->prev;
    }
    
    node->prev = NULL;
    node->next = NULL;
}

/**
 * Adaugă un nod la începutul listei (cel mai recent)
 */
void list_push_front(LRUCache *cache, CacheNode *node) {
    node->prev = NULL;
    node->next = cache->head;
    
    if (cache->head) {
        cache->head->prev = node;
    }
    cache->head = node;
    
    if (cache->tail == NULL) {
        cache->tail = node;
    }
}

/**
 * Mută un nod la începutul listei (marchează ca recent folosit)
 */
void list_move_to_front(LRUCache *cache, CacheNode *node) {
    if (node == cache->head) {
        return;  /* Deja la început */
    }
    
    list_remove(cache, node);
    list_push_front(cache, node);
}

/* =============================================================================
 * FUNCȚII HASH TABLE
 * =============================================================================
 */

/**
 * Caută un nod în hash table
 */
CacheNode* hash_find(LRUCache *cache, const char *key) {
    size_t index = hash_djb2(key) % cache->hash_size;
    
    CacheNode *node = cache->hash_table[index];
    while (node) {
        if (strcmp(node->key, key) == 0) {
            return node;
        }
        node = node->hash_next;
    }
    
    return NULL;
}

/**
 * Inserează un nod în hash table
 */
void hash_insert(LRUCache *cache, CacheNode *node) {
    size_t index = hash_djb2(node->key) % cache->hash_size;
    
    node->hash_next = cache->hash_table[index];
    cache->hash_table[index] = node;
}

/**
 * Elimină un nod din hash table (nu eliberează memoria)
 */
void hash_remove(LRUCache *cache, CacheNode *node) {
    size_t index = hash_djb2(node->key) % cache->hash_size;
    
    CacheNode *curr = cache->hash_table[index];
    CacheNode *prev = NULL;
    
    while (curr) {
        if (curr == node) {
            if (prev) {
                prev->hash_next = curr->hash_next;
            } else {
                cache->hash_table[index] = curr->hash_next;
            }
            node->hash_next = NULL;
            return;
        }
        prev = curr;
        curr = curr->hash_next;
    }
}

/* =============================================================================
 * FUNCȚII LRU CACHE
 * =============================================================================
 */

/**
 * Creează un LRU Cache cu capacitatea specificată
 */
LRUCache* lru_create(size_t capacity) {
    if (capacity == 0) return NULL;
    
    LRUCache *cache = malloc(sizeof(LRUCache));
    if (!cache) return NULL;
    
    cache->hash_table = calloc(HASH_TABLE_SIZE, sizeof(CacheNode*));
    if (!cache->hash_table) {
        free(cache);
        return NULL;
    }
    
    cache->hash_size = HASH_TABLE_SIZE;
    cache->head = NULL;
    cache->tail = NULL;
    cache->capacity = capacity;
    cache->count = 0;
    cache->hits = 0;
    cache->misses = 0;
    
    return cache;
}

/**
 * Eliberează un nod
 */
void node_free(CacheNode *node) {
    if (node) {
        free(node->key);
        free(node->value);
        free(node);
    }
}

/**
 * Elimină cel mai vechi element (LRU eviction)
 * Returnează cheia eliminată (trebuie eliberată de apelant)
 */
char* lru_evict(LRUCache *cache) {
    if (!cache || !cache->tail) return NULL;
    
    CacheNode *lru_node = cache->tail;
    char *evicted_key = strdup(lru_node->key);
    
    /* Elimină din listă și hash table */
    list_remove(cache, lru_node);
    hash_remove(cache, lru_node);
    
    node_free(lru_node);
    cache->count--;
    
    return evicted_key;
}

/**
 * GET - Returnează valoarea și marchează ca recent folosit
 * Returnează NULL dacă nu există (cache miss)
 */
const char* lru_get(LRUCache *cache, const char *key) {
    if (!cache || !key) return NULL;
    
    CacheNode *node = hash_find(cache, key);
    
    if (node) {
        /* Cache hit */
        cache->hits++;
        list_move_to_front(cache, node);
        return node->value;
    }
    
    /* Cache miss */
    cache->misses++;
    return NULL;
}

/**
 * PUT - Inserează/actualizează și marchează ca recent folosit
 * Returnează cheia eliminată dacă cache-ul era plin, NULL altfel
 * (Cheia returnată trebuie eliberată de apelant)
 */
char* lru_put(LRUCache *cache, const char *key, const char *value) {
    if (!cache || !key) return NULL;
    
    char *evicted = NULL;
    
    /* Verifică dacă cheia există deja */
    CacheNode *node = hash_find(cache, key);
    
    if (node) {
        /* Actualizează valoarea existentă */
        free(node->value);
        node->value = strdup(value);
        list_move_to_front(cache, node);
        return NULL;
    }
    
    /* Cheie nouă - verifică capacitatea */
    if (cache->count >= cache->capacity) {
        evicted = lru_evict(cache);
    }
    
    /* Creează nod nou */
    node = malloc(sizeof(CacheNode));
    if (!node) {
        free(evicted);
        return NULL;
    }
    
    node->key = strdup(key);
    node->value = strdup(value);
    node->prev = NULL;
    node->next = NULL;
    node->hash_next = NULL;
    
    /* Inserează în hash table și listă */
    hash_insert(cache, node);
    list_push_front(cache, node);
    cache->count++;
    
    return evicted;
}

/**
 * Verifică dacă o cheie există în cache
 */
bool lru_contains(LRUCache *cache, const char *key) {
    return hash_find(cache, key) != NULL;
}

/**
 * Eliberează memoria cache-ului
 */
void lru_destroy(LRUCache *cache) {
    if (!cache) return;
    
    CacheNode *node = cache->head;
    while (node) {
        CacheNode *next = node->next;
        node_free(node);
        node = next;
    }
    
    free(cache->hash_table);
    free(cache);
}

/* =============================================================================
 * FUNCȚII DE AFIȘARE
 * =============================================================================
 */

/**
 * Afișează conținutul cache-ului (MRU -> LRU)
 */
void lru_print(LRUCache *cache) {
    if (!cache) {
        printf("Cache: NULL\n");
        return;
    }
    
    printf("Cache (%zu/%zu): ", cache->count, cache->capacity);
    
    if (cache->head == NULL) {
        printf("[gol]");
    } else {
        CacheNode *node = cache->head;
        while (node) {
            printf("[%s:%s]", node->key, node->value);
            if (node->next) printf(" -> ");
            node = node->next;
        }
    }
    printf("\n");
}

/**
 * Afișează statisticile cache-ului
 */
void lru_print_stats(LRUCache *cache) {
    if (!cache) {
        printf("[STATS] Cache NULL\n");
        return;
    }
    
    size_t total = cache->hits + cache->misses;
    double hit_rate = total > 0 ? (100.0 * cache->hits / total) : 0.0;
    
    printf("[STATS] Hits: %zu, Misses: %zu, Hit Rate: %.2f%%\n",
           cache->hits, cache->misses, hit_rate);
}

/* =============================================================================
 * FUNCȚIA PRINCIPALĂ
 * =============================================================================
 */

int main(void) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     TEMA 2: CACHE LRU (LEAST RECENTLY USED)                   ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    size_t capacity;
    printf("Capacitate cache: ");
    if (scanf("%zu", &capacity) != 1 || capacity == 0) {
        capacity = 3;
        printf("(folosind capacitate implicită: %zu)\n", capacity);
    }
    
    LRUCache *cache = lru_create(capacity);
    if (!cache) {
        fprintf(stderr, "Eroare: Nu s-a putut crea cache-ul\n");
        return 1;
    }
    
    printf("\nComenzi disponibile:\n");
    printf("  PUT <key> <value> - inserează/actualizează\n");
    printf("  GET <key>         - obține valoarea\n");
    printf("  CONTAINS <key>    - verifică existența\n");
    printf("  PRINT             - afișează conținutul\n");
    printf("  STATS             - afișează statisticile\n");
    printf("  QUIT              - termină programul\n\n");
    
    char command[16];
    char key[MAX_KEY_LENGTH];
    char value[MAX_VALUE_LENGTH];
    
    while (scanf("%15s", command) == 1) {
        if (strcmp(command, "PUT") == 0) {
            if (scanf("%63s %255s", key, value) == 2) {
                char *evicted = lru_put(cache, key, value);
                if (evicted) {
                    printf("[PUT] %s: %s (Evicted: %s)\n", key, value, evicted);
                    free(evicted);
                } else {
                    printf("[PUT] %s: %s\n", key, value);
                }
                lru_print(cache);
            }
        }
        else if (strcmp(command, "GET") == 0) {
            if (scanf("%63s", key) == 1) {
                const char *result = lru_get(cache, key);
                if (result) {
                    printf("[GET] Hit! %s: %s\n", key, result);
                } else {
                    printf("[GET] Miss! %s: NOT FOUND\n", key);
                }
                lru_print(cache);
            }
        }
        else if (strcmp(command, "CONTAINS") == 0) {
            if (scanf("%63s", key) == 1) {
                bool exists = lru_contains(cache, key);
                printf("[CONTAINS] %s: %s\n", key, exists ? "YES" : "NO");
            }
        }
        else if (strcmp(command, "PRINT") == 0) {
            lru_print(cache);
        }
        else if (strcmp(command, "STATS") == 0) {
            lru_print_stats(cache);
        }
        else if (strcmp(command, "QUIT") == 0) {
            break;
        }
        else {
            printf("[ERROR] Comandă necunoscută: %s\n", command);
        }
    }
    
    printf("\n");
    lru_print_stats(cache);
    
    lru_destroy(cache);
    printf("\n[INFO] Memorie eliberată. Program terminat.\n");
    
    return 0;
}

/* =============================================================================
 * EXEMPLU DE UTILIZARE (pentru testare rapidă)
 * =============================================================================
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -o homework2_sol homework2_sol.c
 *
 * $ echo "3
 * PUT A 1
 * PUT B 2
 * PUT C 3
 * GET A
 * PUT D 4
 * GET B
 * STATS
 * QUIT" | ./homework2_sol
 *
 * Output așteptat:
 * Capacitate cache: 3
 * [PUT] A: 1
 * Cache (1/3): [A:1]
 * [PUT] B: 2
 * Cache (2/3): [B:2] -> [A:1]
 * [PUT] C: 3
 * Cache (3/3): [C:3] -> [B:2] -> [A:1]
 * [GET] Hit! A: 1
 * Cache (3/3): [A:1] -> [C:3] -> [B:2]
 * [PUT] D: 4 (Evicted: B)
 * Cache (3/3): [D:4] -> [A:1] -> [C:3]
 * [GET] Miss! B: NOT FOUND
 * [STATS] Hits: 1, Misses: 1, Hit Rate: 50.00%
 *
 * =============================================================================
 */
