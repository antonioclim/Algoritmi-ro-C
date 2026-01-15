#define _POSIX_C_SOURCE 200809L
/**
 * =============================================================================
 * EXERCIȚIUL 2: HASH TABLE CU OPEN ADDRESSING
 * =============================================================================
 *
 * OBIECTIV:
 *   Implementarea unei hash table folosind open addressing cu linear probing
 *   și compararea cu double hashing.
 *
 * CERINȚE:
 *   1. Implementarea linear probing cu marcaje DELETED
 *   2. Implementarea double hashing pentru comparație
 *   3. Operații: insert, search, delete
 *   4. Redimensionare când load factor > 0.7
 *   5. Detectarea și raportarea clustering-ului
 *   6. Benchmark cu multe inserări
 *
 * EXEMPLU INPUT:
 *   INSERT alfa 1
 *   INSERT beta 2
 *   SEARCH alfa
 *   DELETE beta
 *   CLUSTER
 *   BENCHMARK 1000
 *
 * EXEMPLU OUTPUT:
 *   [INSERT] alfa: 1 -> slot[3] (direct)
 *   [INSERT] beta: 2 -> slot[5] (probe: 2)
 *   [SEARCH] alfa: 1
 *   [DELETE] beta: OK (marked DELETED)
 *   [CLUSTER] Max: 4, Avg probes: 1.8
 *   [BENCHMARK] 1000 insertions in 2.5ms
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -g -o exercise2 exercise2.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

/* =============================================================================
 * CONSTANTE
 * =============================================================================
 */

#define INITIAL_SIZE      17    /* Dimensiune inițială (număr prim) */
#define MAX_LOAD_FACTOR   0.7   /* Prag pentru redimensionare */
#define MAX_KEY_LENGTH    64
#define MAX_VALUE_LENGTH  256

/* Markeri speciali pentru sloturi */
#define SLOT_EMPTY    ((Entry*)0)
#define SLOT_DELETED  ((Entry*)-1)

/* =============================================================================
 * DEFINIȚII DE TIPURI
 * =============================================================================
 */

/**
 * Entry în hash table (pentru open addressing)
 */
typedef struct {
    char *key;
    char *value;
} Entry;

/**
 * Hash table cu open addressing
 */
typedef struct {
    Entry **slots;          /* Array de pointeri la Entry */
    size_t size;            /* Dimensiunea tabelei */
    size_t count;           /* Numărul de elemente active */
    size_t deleted_count;   /* Numărul de sloturi DELETED */
    size_t total_probes;    /* Total probe-uri pentru statistici */
    size_t total_inserts;   /* Total inserări pentru statistici */
} HashTable;

/* =============================================================================
 * FUNCȚII HASH
 * =============================================================================
 */

/**
 * TODO 1: Implementează funcția hash primară (djb2)
 *
 * Algoritmul djb2:
 *   hash = 5381
 *   pentru fiecare caracter c: hash = hash * 33 + c
 *
 * @param str - șirul de caractere
 * @return - valoarea hash
 */
unsigned long hash_primary(const char *str) {
    /* YOUR CODE HERE */
    unsigned long hash = 5381;
    int c;
    
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    
    return hash;
}

/**
 * TODO 2: Implementează funcția hash secundară (pentru double hashing)
 *
 * Funcția hash secundară trebuie să:
 *   - Returneze o valoare != 0
 *   - Fie relativ primă cu dimensiunea tabelei
 *
 * Formula sugerată: 1 + (hash_sdbm(str) % (table_size - 1))
 *
 * hash_sdbm: hash = hash * 65599 + c
 *            sau: hash = c + (hash << 6) + (hash << 16) - hash
 *
 * @param str - șirul de caractere
 * @param table_size - dimensiunea tabelei
 * @return - pasul pentru probing (>= 1)
 */
unsigned long hash_secondary(const char *str, size_t table_size) {
    /* YOUR CODE HERE */
    
    return 1;  /* Înlocuiește cu formula corectă */
}

/* =============================================================================
 * FUNCȚII DE BAZĂ
 * =============================================================================
 */

/**
 * TODO 3: Implementează funcția de creare
 *
 * Pași:
 *   1. Alocă structura HashTable
 *   2. Alocă array-ul de sloturi cu calloc (inițializat cu NULL)
 *   3. Inițializează size, count=0, deleted_count=0
 *   4. Inițializează total_probes=0, total_inserts=0
 *
 * @param size - dimensiunea inițială
 * @return - pointer la HashTable sau NULL
 */
HashTable* ht_create(size_t size) {
    /* YOUR CODE HERE */
    
    return NULL;  /* Înlocuiește cu pointer-ul corect */
}

/**
 * Calculează load factor-ul efectiv
 * Include și sloturile DELETED deoarece afectează probing-ul
 */
double ht_load_factor(HashTable *ht) {
    if (ht == NULL || ht->size == 0) return 0.0;
    return (double)(ht->count + ht->deleted_count) / ht->size;
}

/**
 * TODO 4: Implementează redimensionarea
 *
 * Pași:
 *   1. Salvează sloturile vechi și dimensiunea veche
 *   2. Alocă noul array de sloturi
 *   3. Actualizează size, count=0, deleted_count=0
 *   4. Reinserează DOAR elementele active (nu DELETED)
 *   5. Eliberează memoria pentru elementele reinserite din array-ul vechi
 *   6. Eliberează array-ul vechi
 *
 * @param ht - pointer la hash table
 * @param new_size - noua dimensiune
 */
void ht_resize(HashTable *ht, size_t new_size) {
    if (ht == NULL) return;
    
    /* YOUR CODE HERE */
}

/**
 * TODO 5: Implementează inserarea cu LINEAR PROBING
 *
 * Algoritmul:
 *   1. Verifică load factor și redimensionează dacă necesar
 *   2. Calculează indexul inițial: hash_primary(key) % size
 *   3. Memorează prima poziție DELETED întâlnită (pentru refolosire)
 *   4. Parcurge folosind linear probing: index = (index + 1) % size
 *      - Dacă EMPTY: inserează la first_deleted sau index
 *      - Dacă DELETED: memorează dacă e primul
 *      - Dacă cheie existentă: actualizează valoarea
 *   5. Incrementează count și actualizează statisticile
 *
 * @param ht - pointer la hash table
 * @param key - cheia
 * @param value - valoarea
 * @return - numărul de probe-uri efectuate
 */
size_t ht_insert_linear(HashTable *ht, const char *key, const char *value) {
    if (ht == NULL || key == NULL) return 0;
    
    /* Verifică load factor */
    if (ht_load_factor(ht) > MAX_LOAD_FACTOR) {
        ht_resize(ht, ht->size * 2 + 1);
    }
    
    /* YOUR CODE HERE */
    
    /* Hint: Structura codului
     *
     * size_t index = hash_primary(key) % ht->size;
     * size_t start = index;
     * size_t first_deleted = ht->size;  // Invalid marker
     * size_t probes = 0;
     *
     * do {
     *     probes++;
     *
     *     if (ht->slots[index] == SLOT_EMPTY) {
     *         // Slot gol - inserează aici sau la first_deleted
     *         size_t insert_idx = (first_deleted < ht->size) ? first_deleted : index;
     *         
     *         Entry *entry = malloc(sizeof(Entry));
     *         entry->key = strdup(key);
     *         entry->value = strdup(value);
     *         
     *         if (ht->slots[insert_idx] == SLOT_DELETED) {
     *             ht->deleted_count--;
     *         }
     *         ht->slots[insert_idx] = entry;
     *         ht->count++;
     *         ht->total_probes += probes;
     *         ht->total_inserts++;
     *         return probes;
     *     }
     *
     *     if (ht->slots[index] == SLOT_DELETED) {
     *         if (first_deleted == ht->size) {
     *             first_deleted = index;
     *         }
     *     } else if (strcmp(ht->slots[index]->key, key) == 0) {
     *         // Cheie existentă - actualizează
     *         free(ht->slots[index]->value);
     *         ht->slots[index]->value = strdup(value);
     *         return probes;
     *     }
     *
     *     index = (index + 1) % ht->size;  // Linear probing
     * } while (index != start);
     */
    
    return 0;
}

/**
 * TODO 6: Implementează inserarea cu DOUBLE HASHING
 *
 * Algoritmul:
 *   Similar cu linear probing, dar pasul este:
 *   index = (hash1(key) + i * hash2(key)) % size
 *
 * @param ht - pointer la hash table
 * @param key - cheia
 * @param value - valoarea
 * @return - numărul de probe-uri efectuate
 */
size_t ht_insert_double(HashTable *ht, const char *key, const char *value) {
    if (ht == NULL || key == NULL) return 0;
    
    if (ht_load_factor(ht) > MAX_LOAD_FACTOR) {
        ht_resize(ht, ht->size * 2 + 1);
    }
    
    /* YOUR CODE HERE */
    
    /* Hint: Diferența față de linear probing
     *
     * size_t h1 = hash_primary(key) % ht->size;
     * size_t h2 = hash_secondary(key, ht->size);
     * size_t index = h1;
     *
     * for (size_t i = 0; i < ht->size; i++) {
     *     probes++;
     *     // ... verificări la fel ...
     *     
     *     index = (h1 + (i + 1) * h2) % ht->size;  // Double hashing
     * }
     */
    
    return 0;
}

/**
 * TODO 7: Implementează căutarea (funcționează pentru ambele metode)
 *
 * @param ht - pointer la hash table
 * @param key - cheia căutată
 * @return - pointer la valoare sau NULL
 */
const char* ht_search(HashTable *ht, const char *key) {
    if (ht == NULL || key == NULL) return NULL;
    
    /* YOUR CODE HERE */
    
    /* Hint:
     * size_t index = hash_primary(key) % ht->size;
     * size_t start = index;
     *
     * do {
     *     if (ht->slots[index] == SLOT_EMPTY) {
     *         return NULL;
     *     }
     *     if (ht->slots[index] != SLOT_DELETED &&
     *         strcmp(ht->slots[index]->key, key) == 0) {
     *         return ht->slots[index]->value;
     *     }
     *     index = (index + 1) % ht->size;
     * } while (index != start);
     */
    
    return NULL;
}

/**
 * TODO 8: Implementează ștergerea
 *
 * IMPORTANT: Nu poți seta slotul la EMPTY deoarece ar întrerupe
 * lanțul de probing. Trebuie să-l marchezi ca DELETED.
 *
 * @param ht - pointer la hash table
 * @param key - cheia de șters
 * @return - true dacă a fost ștearsă, false altfel
 */
bool ht_delete(HashTable *ht, const char *key) {
    if (ht == NULL || key == NULL) return false;
    
    /* YOUR CODE HERE */
    
    /* Hint:
     * Când găsești cheia:
     *   free(ht->slots[index]->key);
     *   free(ht->slots[index]->value);
     *   free(ht->slots[index]);
     *   ht->slots[index] = SLOT_DELETED;  // NU SLOT_EMPTY!
     *   ht->count--;
     *   ht->deleted_count++;
     *   return true;
     */
    
    return false;
}

/**
 * TODO 9: Implementează eliberarea memoriei
 */
void ht_destroy(HashTable *ht) {
    if (ht == NULL) return;
    
    /* YOUR CODE HERE */
}

/* =============================================================================
 * FUNCȚII DE ANALIZĂ (IMPLEMENTATE)
 * =============================================================================
 */

/**
 * Analizează clustering-ul în tabelă
 */
void ht_analyze_clustering(HashTable *ht) {
    if (ht == NULL) {
        printf("[CLUSTER] Hash table este NULL\n");
        return;
    }
    
    size_t max_cluster = 0;
    size_t current_cluster = 0;
    size_t num_clusters = 0;
    size_t total_cluster_size = 0;
    
    for (size_t i = 0; i < ht->size; i++) {
        if (ht->slots[i] != SLOT_EMPTY && ht->slots[i] != SLOT_DELETED) {
            current_cluster++;
            if (current_cluster > max_cluster) {
                max_cluster = current_cluster;
            }
        } else {
            if (current_cluster > 0) {
                num_clusters++;
                total_cluster_size += current_cluster;
            }
            current_cluster = 0;
        }
    }
    
    /* Verifică ultimul cluster */
    if (current_cluster > 0) {
        num_clusters++;
        total_cluster_size += current_cluster;
    }
    
    double avg_cluster = num_clusters > 0 ? 
                         (double)total_cluster_size / num_clusters : 0;
    double avg_probes = ht->total_inserts > 0 ?
                        (double)ht->total_probes / ht->total_inserts : 0;
    
    printf("[CLUSTER] Max: %zu, NumClusters: %zu, AvgCluster: %.2f, AvgProbes: %.2f\n",
           max_cluster, num_clusters, avg_cluster, avg_probes);
}

/**
 * Afișează statistici
 */
void ht_print_stats(HashTable *ht) {
    if (ht == NULL) {
        printf("[STATS] Hash table este NULL\n");
        return;
    }
    
    printf("[STATS] Size: %zu, Count: %zu, Deleted: %zu, Load: %.2f\n",
           ht->size, ht->count, ht->deleted_count, ht_load_factor(ht));
}

/**
 * Vizualizează tabela (pentru debugging)
 */
void ht_visualize(HashTable *ht) {
    if (ht == NULL) {
        printf("[VIS] Hash table este NULL\n");
        return;
    }
    
    printf("[VIS] ");
    for (size_t i = 0; i < ht->size && i < 50; i++) {  /* Max 50 pentru afișare */
        if (ht->slots[i] == SLOT_EMPTY) {
            printf(".");
        } else if (ht->slots[i] == SLOT_DELETED) {
            printf("X");
        } else {
            printf("#");
        }
    }
    if (ht->size > 50) {
        printf("... (+%zu)", ht->size - 50);
    }
    printf("\n");
    printf("[VIS] Legend: . = EMPTY, X = DELETED, # = OCCUPIED\n");
}

/**
 * TODO 10: Implementează benchmark-ul
 *
 * Pași:
 *   1. Creează o tabelă nouă
 *   2. Generează n chei unice și inserează-le
 *   3. Măsoară timpul
 *   4. Afișează rezultatele și statisticile
 *   5. Eliberează memoria
 */
void ht_benchmark(size_t n, bool use_double_hashing) {
    printf("[BENCHMARK] Testing with %zu elements (%s)...\n",
           n, use_double_hashing ? "double hashing" : "linear probing");
    
    HashTable *ht = ht_create(INITIAL_SIZE);
    if (ht == NULL) {
        printf("[BENCHMARK] Eroare la creare\n");
        return;
    }
    
    char key[32], value[32];
    
    clock_t start = clock();
    
    for (size_t i = 0; i < n; i++) {
        snprintf(key, sizeof(key), "key_%zu", i);
        snprintf(value, sizeof(value), "val_%zu", i);
        
        if (use_double_hashing) {
            ht_insert_double(ht, key, value);
        } else {
            ht_insert_linear(ht, key, value);
        }
    }
    
    clock_t end = clock();
    double time_ms = 1000.0 * (end - start) / CLOCKS_PER_SEC;
    
    printf("[BENCHMARK] Time: %.2f ms\n", time_ms);
    ht_print_stats(ht);
    ht_analyze_clustering(ht);
    
    ht_destroy(ht);
}

/* =============================================================================
 * FUNCȚIA PRINCIPALĂ
 * =============================================================================
 */

int main(void) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     EXERCIȚIUL 2: HASH TABLE CU OPEN ADDRESSING               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    HashTable *ht = ht_create(INITIAL_SIZE);
    if (ht == NULL) {
        fprintf(stderr, "Eroare: Nu s-a putut crea hash table\n");
        return 1;
    }
    
    printf("Comenzi disponibile:\n");
    printf("  INSERT <key> <value>  - inserează (linear probing)\n");
    printf("  INSERTD <key> <value> - inserează (double hashing)\n");
    printf("  SEARCH <key>          - caută o cheie\n");
    printf("  DELETE <key>          - șterge o cheie\n");
    printf("  STATS                 - afișează statisticile\n");
    printf("  CLUSTER               - analizează clustering-ul\n");
    printf("  VIS                   - vizualizează tabela\n");
    printf("  BENCHMARK <n>         - test cu n elemente\n");
    printf("  BENCHMARKD <n>        - benchmark double hashing\n");
    printf("  QUIT                  - termină programul\n\n");
    
    char command[16];
    char key[MAX_KEY_LENGTH];
    char value[MAX_VALUE_LENGTH];
    
    while (scanf("%15s", command) == 1) {
        if (strcmp(command, "INSERT") == 0) {
            if (scanf("%63s %255s", key, value) == 2) {
                size_t probes = ht_insert_linear(ht, key, value);
                size_t ideal = hash_primary(key) % ht->size;
                printf("[INSERT] %s: %s (ideal: %zu, probes: %zu)\n", 
                       key, value, ideal, probes);
            }
        }
        else if (strcmp(command, "INSERTD") == 0) {
            if (scanf("%63s %255s", key, value) == 2) {
                size_t probes = ht_insert_double(ht, key, value);
                printf("[INSERTD] %s: %s (probes: %zu)\n", key, value, probes);
            }
        }
        else if (strcmp(command, "SEARCH") == 0) {
            if (scanf("%63s", key) == 1) {
                const char *result = ht_search(ht, key);
                if (result) {
                    printf("[SEARCH] %s: %s\n", key, result);
                } else {
                    printf("[SEARCH] %s: NOT FOUND\n", key);
                }
            }
        }
        else if (strcmp(command, "DELETE") == 0) {
            if (scanf("%63s", key) == 1) {
                if (ht_delete(ht, key)) {
                    printf("[DELETE] %s: OK (marked DELETED)\n", key);
                } else {
                    printf("[DELETE] %s: NOT FOUND\n", key);
                }
            }
        }
        else if (strcmp(command, "STATS") == 0) {
            ht_print_stats(ht);
        }
        else if (strcmp(command, "CLUSTER") == 0) {
            ht_analyze_clustering(ht);
        }
        else if (strcmp(command, "VIS") == 0) {
            ht_visualize(ht);
        }
        else if (strcmp(command, "BENCHMARK") == 0) {
            size_t n;
            if (scanf("%zu", &n) == 1) {
                ht_benchmark(n, false);
            }
        }
        else if (strcmp(command, "BENCHMARKD") == 0) {
            size_t n;
            if (scanf("%zu", &n) == 1) {
                ht_benchmark(n, true);
            }
        }
        else if (strcmp(command, "QUIT") == 0) {
            break;
        }
        else {
            printf("[ERROR] Comandă necunoscută: %s\n", command);
        }
    }
    
    ht_destroy(ht);
    printf("\n[INFO] Memorie eliberată. Program terminat.\n");
    
    return 0;
}

/* =============================================================================
 * PROVOCĂRI BONUS (Opțional)
 * =============================================================================
 *
 * 1. Implementează quadratic probing și compară cu linear/double
 * 2. Adaugă suport pentru Robin Hood hashing
 * 3. Implementează Cuckoo hashing pentru worst-case O(1)
 * 4. Creează grafice de performanță (output CSV pentru plot)
 * 5. Implementează thread-safe hash table cu mutex-uri
 *
 * =============================================================================
 */
