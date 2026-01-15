#define _POSIX_C_SOURCE 200809L
/**
 * =============================================================================
 * SOLUȚIE EXERCIȚIUL 2: HASH TABLE CU OPEN ADDRESSING
 * =============================================================================
 *
 * DOAR PENTRU INSTRUCTOR - NU DISTRIBUI STUDENȚILOR
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -g -o exercise2_sol exercise2_sol.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define INITIAL_SIZE      17
#define MAX_LOAD_FACTOR   0.7
#define MAX_KEY_LENGTH    64
#define MAX_VALUE_LENGTH  256

#define SLOT_EMPTY    ((Entry*)0)
#define SLOT_DELETED  ((Entry*)-1)

typedef struct {
    char *key;
    char *value;
} Entry;

typedef struct {
    Entry **slots;
    size_t size;
    size_t count;
    size_t deleted_count;
    size_t total_probes;
    size_t total_inserts;
} HashTable;

/* =============================================================================
 * SOLUȚIE TODO 1: Funcția hash primară
 * =============================================================================
 */
unsigned long hash_primary(const char *str) {
    unsigned long hash = 5381;
    int c;
    
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    
    return hash;
}

/* =============================================================================
 * SOLUȚIE TODO 2: Funcția hash secundară
 * =============================================================================
 */
unsigned long hash_secondary(const char *str, size_t table_size) {
    unsigned long hash = 0;
    int c;
    
    while ((c = *str++)) {
        hash = c + (hash << 6) + (hash << 16) - hash;
    }
    
    return 1 + (hash % (table_size - 1));
}

/* =============================================================================
 * SOLUȚIE TODO 3: Funcția de creare
 * =============================================================================
 */
HashTable* ht_create(size_t size) {
    HashTable *ht = malloc(sizeof(HashTable));
    if (ht == NULL) return NULL;
    
    ht->slots = calloc(size, sizeof(Entry*));
    if (ht->slots == NULL) {
        free(ht);
        return NULL;
    }
    
    ht->size = size;
    ht->count = 0;
    ht->deleted_count = 0;
    ht->total_probes = 0;
    ht->total_inserts = 0;
    
    return ht;
}

double ht_load_factor(HashTable *ht) {
    if (ht == NULL || ht->size == 0) return 0.0;
    return (double)(ht->count + ht->deleted_count) / ht->size;
}

/* Forward declarations */
size_t ht_insert_linear(HashTable *ht, const char *key, const char *value);
size_t ht_insert_double(HashTable *ht, const char *key, const char *value);

/* =============================================================================
 * SOLUȚIE TODO 4: Redimensionare
 * =============================================================================
 */
void ht_resize(HashTable *ht, size_t new_size) {
    if (ht == NULL) return;
    
    Entry **old_slots = ht->slots;
    size_t old_size = ht->size;
    
    ht->slots = calloc(new_size, sizeof(Entry*));
    if (ht->slots == NULL) {
        ht->slots = old_slots;
        return;
    }
    
    size_t old_count = ht->count;
    ht->size = new_size;
    ht->count = 0;
    ht->deleted_count = 0;
    
    for (size_t i = 0; i < old_size; i++) {
        if (old_slots[i] != SLOT_EMPTY && old_slots[i] != SLOT_DELETED) {
            ht_insert_linear(ht, old_slots[i]->key, old_slots[i]->value);
            free(old_slots[i]->key);
            free(old_slots[i]->value);
            free(old_slots[i]);
        }
    }
    
    free(old_slots);
    (void)old_count;  /* Suppress unused warning */
}

/* =============================================================================
 * SOLUȚIE TODO 5: Inserare cu Linear Probing
 * =============================================================================
 */
size_t ht_insert_linear(HashTable *ht, const char *key, const char *value) {
    if (ht == NULL || key == NULL) return 0;
    
    if (ht_load_factor(ht) > MAX_LOAD_FACTOR) {
        ht_resize(ht, ht->size * 2 + 1);
    }
    
    size_t index = hash_primary(key) % ht->size;
    size_t start = index;
    size_t first_deleted = ht->size;
    size_t probes = 0;
    
    do {
        probes++;
        
        if (ht->slots[index] == SLOT_EMPTY) {
            size_t insert_idx = (first_deleted < ht->size) ? first_deleted : index;
            
            Entry *entry = malloc(sizeof(Entry));
            if (entry == NULL) return probes;
            
            entry->key = strdup(key);
            entry->value = strdup(value);
            
            if (ht->slots[insert_idx] == SLOT_DELETED) {
                ht->deleted_count--;
            }
            ht->slots[insert_idx] = entry;
            ht->count++;
            ht->total_probes += probes;
            ht->total_inserts++;
            return probes;
        }
        
        if (ht->slots[index] == SLOT_DELETED) {
            if (first_deleted == ht->size) {
                first_deleted = index;
            }
        } else if (strcmp(ht->slots[index]->key, key) == 0) {
            free(ht->slots[index]->value);
            ht->slots[index]->value = strdup(value);
            return probes;
        }
        
        index = (index + 1) % ht->size;
    } while (index != start);
    
    return probes;
}

/* =============================================================================
 * SOLUȚIE TODO 6: Inserare cu Double Hashing
 * =============================================================================
 */
size_t ht_insert_double(HashTable *ht, const char *key, const char *value) {
    if (ht == NULL || key == NULL) return 0;
    
    if (ht_load_factor(ht) > MAX_LOAD_FACTOR) {
        ht_resize(ht, ht->size * 2 + 1);
    }
    
    size_t h1 = hash_primary(key) % ht->size;
    size_t h2 = hash_secondary(key, ht->size);
    size_t index = h1;
    size_t first_deleted = ht->size;
    size_t probes = 0;
    
    for (size_t i = 0; i < ht->size; i++) {
        probes++;
        
        if (ht->slots[index] == SLOT_EMPTY) {
            size_t insert_idx = (first_deleted < ht->size) ? first_deleted : index;
            
            Entry *entry = malloc(sizeof(Entry));
            if (entry == NULL) return probes;
            
            entry->key = strdup(key);
            entry->value = strdup(value);
            
            if (ht->slots[insert_idx] == SLOT_DELETED) {
                ht->deleted_count--;
            }
            ht->slots[insert_idx] = entry;
            ht->count++;
            ht->total_probes += probes;
            ht->total_inserts++;
            return probes;
        }
        
        if (ht->slots[index] == SLOT_DELETED) {
            if (first_deleted == ht->size) {
                first_deleted = index;
            }
        } else if (strcmp(ht->slots[index]->key, key) == 0) {
            free(ht->slots[index]->value);
            ht->slots[index]->value = strdup(value);
            return probes;
        }
        
        index = (h1 + (i + 1) * h2) % ht->size;
    }
    
    return probes;
}

/* =============================================================================
 * SOLUȚIE TODO 7: Căutare
 * =============================================================================
 */
const char* ht_search(HashTable *ht, const char *key) {
    if (ht == NULL || key == NULL) return NULL;
    
    size_t index = hash_primary(key) % ht->size;
    size_t start = index;
    
    do {
        if (ht->slots[index] == SLOT_EMPTY) {
            return NULL;
        }
        
        if (ht->slots[index] != SLOT_DELETED &&
            strcmp(ht->slots[index]->key, key) == 0) {
            return ht->slots[index]->value;
        }
        
        index = (index + 1) % ht->size;
    } while (index != start);
    
    return NULL;
}

/* =============================================================================
 * SOLUȚIE TODO 8: Ștergere
 * =============================================================================
 */
bool ht_delete(HashTable *ht, const char *key) {
    if (ht == NULL || key == NULL) return false;
    
    size_t index = hash_primary(key) % ht->size;
    size_t start = index;
    
    do {
        if (ht->slots[index] == SLOT_EMPTY) {
            return false;
        }
        
        if (ht->slots[index] != SLOT_DELETED &&
            strcmp(ht->slots[index]->key, key) == 0) {
            free(ht->slots[index]->key);
            free(ht->slots[index]->value);
            free(ht->slots[index]);
            ht->slots[index] = SLOT_DELETED;
            ht->count--;
            ht->deleted_count++;
            return true;
        }
        
        index = (index + 1) % ht->size;
    } while (index != start);
    
    return false;
}

/* =============================================================================
 * SOLUȚIE TODO 9: Eliberare memorie
 * =============================================================================
 */
void ht_destroy(HashTable *ht) {
    if (ht == NULL) return;
    
    for (size_t i = 0; i < ht->size; i++) {
        if (ht->slots[i] != SLOT_EMPTY && ht->slots[i] != SLOT_DELETED) {
            free(ht->slots[i]->key);
            free(ht->slots[i]->value);
            free(ht->slots[i]);
        }
    }
    
    free(ht->slots);
    free(ht);
}

/* =============================================================================
 * FUNCȚII AUXILIARE
 * =============================================================================
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

void ht_print_stats(HashTable *ht) {
    if (ht == NULL) {
        printf("[STATS] Hash table este NULL\n");
        return;
    }
    
    printf("[STATS] Size: %zu, Count: %zu, Deleted: %zu, Load: %.2f\n",
           ht->size, ht->count, ht->deleted_count, ht_load_factor(ht));
}

void ht_visualize(HashTable *ht) {
    if (ht == NULL) {
        printf("[VIS] Hash table este NULL\n");
        return;
    }
    
    printf("[VIS] ");
    for (size_t i = 0; i < ht->size && i < 50; i++) {
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
