#define _POSIX_C_SOURCE 200809L
/**
 * =============================================================================
 * SOLUȚIE EXERCIȚIUL 1: HASH TABLE CU SEPARATE CHAINING
 * =============================================================================
 *
 * DOAR PENTRU INSTRUCTOR - NU DISTRIBUI STUDENȚILOR
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -g -o exercise1_sol exercise1_sol.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define INITIAL_SIZE      17
#define MAX_LOAD_FACTOR   2.0
#define MAX_KEY_LENGTH    64
#define MAX_VALUE_LENGTH  256

/* =============================================================================
 * SOLUȚIE TODO 1: Structura HashNode
 * =============================================================================
 */
typedef struct HashNode {
    char *key;
    char *value;
    struct HashNode *next;
} HashNode;

/* =============================================================================
 * SOLUȚIE TODO 2: Structura HashTable
 * =============================================================================
 */
typedef struct {
    HashNode **buckets;
    size_t size;
    size_t count;
} HashTable;

/* =============================================================================
 * SOLUȚIE TODO 3: Funcția hash djb2
 * =============================================================================
 */
unsigned long hash_djb2(const char *str) {
    unsigned long hash = 5381;
    int c;
    
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;  /* hash * 33 + c */
    }
    
    return hash;
}

/* =============================================================================
 * SOLUȚIE TODO 4: Funcția de creare
 * =============================================================================
 */
HashTable* ht_create(size_t size) {
    HashTable *ht = malloc(sizeof(HashTable));
    if (ht == NULL) {
        return NULL;
    }
    
    ht->buckets = calloc(size, sizeof(HashNode*));
    if (ht->buckets == NULL) {
        free(ht);
        return NULL;
    }
    
    ht->size = size;
    ht->count = 0;
    return ht;
}

double ht_load_factor(HashTable *ht) {
    if (ht == NULL || ht->size == 0) return 0.0;
    return (double)ht->count / ht->size;
}

/* Forward declaration */
void ht_insert(HashTable *ht, const char *key, const char *value);

/* =============================================================================
 * SOLUȚIE TODO 5: Redimensionare
 * =============================================================================
 */
void ht_resize(HashTable *ht, size_t new_size) {
    if (ht == NULL) return;
    
    HashNode **old_buckets = ht->buckets;
    size_t old_size = ht->size;
    
    ht->buckets = calloc(new_size, sizeof(HashNode*));
    if (ht->buckets == NULL) {
        ht->buckets = old_buckets;
        return;
    }
    
    ht->size = new_size;
    ht->count = 0;
    
    for (size_t i = 0; i < old_size; i++) {
        HashNode *node = old_buckets[i];
        while (node != NULL) {
            HashNode *next = node->next;
            
            /* Recalculează indexul pentru noua dimensiune */
            size_t new_index = hash_djb2(node->key) % new_size;
            
            /* Inserează în noua poziție */
            node->next = ht->buckets[new_index];
            ht->buckets[new_index] = node;
            ht->count++;
            
            node = next;
        }
    }
    
    free(old_buckets);
}

/* =============================================================================
 * SOLUȚIE TODO 6: Inserare
 * =============================================================================
 */
void ht_insert(HashTable *ht, const char *key, const char *value) {
    if (ht == NULL || key == NULL) return;
    
    size_t index = hash_djb2(key) % ht->size;
    
    /* Caută cheia existentă */
    HashNode *current = ht->buckets[index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            /* Actualizează valoarea */
            free(current->value);
            current->value = strdup(value);
            return;
        }
        current = current->next;
    }
    
    /* Cheie nouă - creează nod */
    HashNode *new_node = malloc(sizeof(HashNode));
    if (new_node == NULL) return;
    
    new_node->key = strdup(key);
    new_node->value = strdup(value);
    new_node->next = ht->buckets[index];
    ht->buckets[index] = new_node;
    ht->count++;
    
    /* Verifică pentru redimensionare */
    if (ht_load_factor(ht) > MAX_LOAD_FACTOR) {
        ht_resize(ht, ht->size * 2 + 1);
    }
}

/* =============================================================================
 * SOLUȚIE TODO 7: Căutare
 * =============================================================================
 */
const char* ht_search(HashTable *ht, const char *key) {
    if (ht == NULL || key == NULL) return NULL;
    
    size_t index = hash_djb2(key) % ht->size;
    
    HashNode *current = ht->buckets[index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
        current = current->next;
    }
    
    return NULL;
}

/* =============================================================================
 * SOLUȚIE TODO 8: Ștergere
 * =============================================================================
 */
bool ht_delete(HashTable *ht, const char *key) {
    if (ht == NULL || key == NULL) return false;
    
    size_t index = hash_djb2(key) % ht->size;
    
    HashNode *current = ht->buckets[index];
    HashNode *prev = NULL;
    
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            if (prev != NULL) {
                prev->next = current->next;
            } else {
                ht->buckets[index] = current->next;
            }
            
            free(current->key);
            free(current->value);
            free(current);
            ht->count--;
            return true;
        }
        prev = current;
        current = current->next;
    }
    
    return false;
}

/* =============================================================================
 * SOLUȚIE TODO 9: Eliberare memorie
 * =============================================================================
 */
void ht_destroy(HashTable *ht) {
    if (ht == NULL) return;
    
    for (size_t i = 0; i < ht->size; i++) {
        HashNode *current = ht->buckets[i];
        while (current != NULL) {
            HashNode *next = current->next;
            free(current->key);
            free(current->value);
            free(current);
            current = next;
        }
    }
    
    free(ht->buckets);
    free(ht);
}

/* =============================================================================
 * FUNCȚII AUXILIARE
 * =============================================================================
 */
void ht_print_stats(HashTable *ht) {
    if (ht == NULL) {
        printf("[STATS] Hash table este NULL\n");
        return;
    }
    
    size_t empty_buckets = 0;
    size_t max_chain = 0;
    
    for (size_t i = 0; i < ht->size; i++) {
        if (ht->buckets[i] == NULL) {
            empty_buckets++;
        } else {
            size_t chain_len = 0;
            HashNode *node = ht->buckets[i];
            while (node) {
                chain_len++;
                node = node->next;
            }
            if (chain_len > max_chain) {
                max_chain = chain_len;
            }
        }
    }
    
    printf("[STATS] Size: %zu, Count: %zu, Load: %.2f, Empty: %zu, MaxChain: %zu\n",
           ht->size, ht->count, ht_load_factor(ht), empty_buckets, max_chain);
}

void ht_print(HashTable *ht) {
    if (ht == NULL) {
        printf("[PRINT] Hash table este NULL\n");
        return;
    }
    
    printf("[PRINT] Conținut hash table (%zu elemente):\n", ht->count);
    for (size_t i = 0; i < ht->size; i++) {
        if (ht->buckets[i] != NULL) {
            printf("  Bucket[%zu]: ", i);
            HashNode *node = ht->buckets[i];
            while (node) {
                printf("(%s: %s)", node->key, node->value);
                if (node->next) printf(" -> ");
                node = node->next;
            }
            printf("\n");
        }
    }
}

/* =============================================================================
 * FUNCȚIA PRINCIPALĂ
 * =============================================================================
 */
int main(void) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     EXERCIȚIUL 1: HASH TABLE CU SEPARATE CHAINING             ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    HashTable *ht = ht_create(INITIAL_SIZE);
    if (ht == NULL) {
        fprintf(stderr, "Eroare: Nu s-a putut crea hash table\n");
        return 1;
    }
    
    printf("Comenzi disponibile:\n");
    printf("  INSERT <key> <value> - inserează o pereche\n");
    printf("  SEARCH <key>         - caută o cheie\n");
    printf("  DELETE <key>         - șterge o cheie\n");
    printf("  STATS                - afișează statisticile\n");
    printf("  PRINT                - afișează conținutul\n");
    printf("  QUIT                 - termină programul\n\n");
    
    char command[16];
    char key[MAX_KEY_LENGTH];
    char value[MAX_VALUE_LENGTH];
    
    while (scanf("%15s", command) == 1) {
        if (strcmp(command, "INSERT") == 0) {
            if (scanf("%63s %255s", key, value) == 2) {
                size_t index = hash_djb2(key) % ht->size;
                ht_insert(ht, key, value);
                printf("[INSERT] %s: %s -> bucket[%zu]\n", key, value, index);
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
                    printf("[DELETE] %s: OK\n", key);
                } else {
                    printf("[DELETE] %s: NOT FOUND\n", key);
                }
            }
        }
        else if (strcmp(command, "STATS") == 0) {
            ht_print_stats(ht);
        }
        else if (strcmp(command, "PRINT") == 0) {
            ht_print(ht);
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
