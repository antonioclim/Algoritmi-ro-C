/**
 * =============================================================================
 * SĂPTĂMÂNA 11: TABELE DE DISPERSIE (HASH TABLES)
 * Exemplu Complet Demonstrativ
 * =============================================================================
 */

#define _POSIX_C_SOURCE 200809L  /* Pentru strdup */

/**
 *
 * Acest exemplu demonstrează:
 *   1. Funcții hash pentru șiruri de caractere (djb2, sdbm, FNV-1a)
 *   2. Hash table cu separate chaining
 *   3. Hash table cu open addressing (linear probing)
 *   4. Operații: insert, search, delete
 *   5. Redimensionare automată (rehashing)
 *   6. Statistici și vizualizare
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -g -o example1 example1.c
 * Utilizare: ./example1
 *
 * Curs: Algoritmi și Tehnici de Programare (ATP)
 * Academia de Studii Economice București - CSIE
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

/* =============================================================================
 * CONSTANTE ȘI DEFINIȚII
 * =============================================================================
 */

#define INITIAL_SIZE        17      /* Dimensiune inițială (număr prim) */
#define LOAD_FACTOR_CHAIN   2.0     /* Prag pentru chaining */
#define LOAD_FACTOR_OPEN    0.7     /* Prag pentru open addressing */

/* Markeri pentru open addressing */
#define SLOT_EMPTY    NULL
#define SLOT_DELETED  ((HashNode*)-1)

/* Culori ANSI pentru output */
#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[0;32m"
#define COLOR_YELLOW  "\033[0;33m"
#define COLOR_BLUE    "\033[0;34m"
#define COLOR_RED     "\033[0;31m"
#define COLOR_CYAN    "\033[0;36m"
#define COLOR_MAGENTA "\033[0;35m"
#define COLOR_BOLD    "\033[1m"

/* =============================================================================
 * PARTEA 1: FUNCȚII HASH
 * =============================================================================
 */

/**
 * djb2 - Funcția hash a lui Dan Bernstein
 * Una dintre cele mai populare funcții hash pentru șiruri.
 * Folosește constanta magică 5381 și multiplicator 33.
 */
unsigned long hash_djb2(const char *str) {
    unsigned long hash = 5381;
    int c;
    
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;  /* hash * 33 + c */
    }
    
    return hash;
}

/**
 * sdbm - Funcție hash din Berkeley DB
 * Oferă distribuție excelentă pentru multe tipuri de date.
 */
unsigned long hash_sdbm(const char *str) {
    unsigned long hash = 0;
    int c;
    
    while ((c = *str++)) {
        hash = c + (hash << 6) + (hash << 16) - hash;
    }
    
    return hash;
}

/**
 * FNV-1a (Fowler-Noll-Vo)
 * Foarte populară, distribuție uniformă excelentă.
 */
unsigned long hash_fnv1a(const char *str) {
    unsigned long hash = 2166136261u;  /* FNV offset basis */
    
    while (*str) {
        hash ^= (unsigned char)*str++;
        hash *= 16777619u;  /* FNV prime */
    }
    
    return hash;
}

/**
 * Funcție hash secundară pentru double hashing
 * Trebuie să returneze o valoare != 0
 */
unsigned long hash_secondary(const char *str, size_t table_size) {
    unsigned long hash = hash_sdbm(str);
    /* Asigură că rezultatul e nenul și mai mic decât table_size */
    return 1 + (hash % (table_size - 1));
}

/* =============================================================================
 * PARTEA 2: STRUCTURI DE DATE
 * =============================================================================
 */

/**
 * Nod pentru hash table (folosit în chaining și open addressing)
 */
typedef struct HashNode {
    char *key;              /* Cheia (alocat dinamic) */
    char *value;            /* Valoarea (alocat dinamic) */
    struct HashNode *next;  /* Pointer pentru chaining (NULL în open addr) */
} HashNode;

/**
 * Hash table cu chaining
 */
typedef struct {
    HashNode **buckets;     /* Array de pointeri la liste */
    size_t size;            /* Dimensiunea tabelei */
    size_t count;           /* Numărul de elemente */
} HashTableChain;

/**
 * Hash table cu open addressing
 */
typedef struct {
    HashNode **slots;       /* Array de pointeri la noduri */
    size_t size;            /* Dimensiunea tabelei */
    size_t count;           /* Numărul de elemente */
    size_t deleted_count;   /* Numărul de sloturi DELETED */
} HashTableOpen;

/* =============================================================================
 * PARTEA 3: HASH TABLE CU CHAINING
 * =============================================================================
 */

/**
 * Creează o nouă hash table cu chaining
 */
HashTableChain* htc_create(size_t size) {
    HashTableChain *ht = malloc(sizeof(HashTableChain));
    if (!ht) return NULL;
    
    ht->buckets = calloc(size, sizeof(HashNode*));
    if (!ht->buckets) {
        free(ht);
        return NULL;
    }
    
    ht->size = size;
    ht->count = 0;
    return ht;
}

/**
 * Calculează load factor pentru chaining
 */
double htc_load_factor(HashTableChain *ht) {
    return (double)ht->count / ht->size;
}

/**
 * Inserează o pereche cheie-valoare (chaining)
 */
void htc_insert(HashTableChain *ht, const char *key, const char *value);

/**
 * Redimensionează tabela (chaining)
 */
void htc_resize(HashTableChain *ht, size_t new_size) {
    HashNode **old_buckets = ht->buckets;
    size_t old_size = ht->size;
    
    /* Alocă noua tabelă */
    ht->buckets = calloc(new_size, sizeof(HashNode*));
    if (!ht->buckets) {
        ht->buckets = old_buckets;  /* Revert on failure */
        return;
    }
    
    ht->size = new_size;
    ht->count = 0;
    
    /* Reinserează toate elementele */
    for (size_t i = 0; i < old_size; i++) {
        HashNode *node = old_buckets[i];
        while (node) {
            HashNode *next = node->next;
            
            /* Recalculează indexul */
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

/**
 * Inserează o pereche cheie-valoare (chaining)
 */
void htc_insert(HashTableChain *ht, const char *key, const char *value) {
    size_t index = hash_djb2(key) % ht->size;
    
    /* Verifică dacă cheia există deja */
    HashNode *current = ht->buckets[index];
    while (current) {
        if (strcmp(current->key, key) == 0) {
            /* Actualizează valoarea */
            free(current->value);
            current->value = strdup(value);
            return;
        }
        current = current->next;
    }
    
    /* Cheie nouă - inserare la început */
    HashNode *new_node = malloc(sizeof(HashNode));
    if (!new_node) return;
    
    new_node->key = strdup(key);
    new_node->value = strdup(value);
    new_node->next = ht->buckets[index];
    ht->buckets[index] = new_node;
    ht->count++;
    
    /* Verifică load factor pentru redimensionare */
    if (htc_load_factor(ht) > LOAD_FACTOR_CHAIN) {
        htc_resize(ht, ht->size * 2 + 1);
    }
}

/**
 * Caută o cheie în hash table (chaining)
 * Returnează valoarea sau NULL dacă nu există
 */
const char* htc_search(HashTableChain *ht, const char *key) {
    size_t index = hash_djb2(key) % ht->size;
    
    HashNode *current = ht->buckets[index];
    while (current) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
        current = current->next;
    }
    
    return NULL;
}

/**
 * Șterge o cheie din hash table (chaining)
 * Returnează true dacă a fost găsită și ștearsă
 */
bool htc_delete(HashTableChain *ht, const char *key) {
    size_t index = hash_djb2(key) % ht->size;
    
    HashNode *current = ht->buckets[index];
    HashNode *prev = NULL;
    
    while (current) {
        if (strcmp(current->key, key) == 0) {
            if (prev) {
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

/**
 * Eliberează memoria pentru hash table (chaining)
 */
void htc_destroy(HashTableChain *ht) {
    for (size_t i = 0; i < ht->size; i++) {
        HashNode *current = ht->buckets[i];
        while (current) {
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

/**
 * Afișează statistici pentru hash table (chaining)
 */
void htc_print_stats(HashTableChain *ht) {
    printf("\n%s╔═══════════════════════════════════════════════════════════════╗%s\n", COLOR_CYAN, COLOR_RESET);
    printf("%s║           STATISTICI HASH TABLE (CHAINING)                    ║%s\n", COLOR_CYAN, COLOR_RESET);
    printf("%s╚═══════════════════════════════════════════════════════════════╝%s\n\n", COLOR_CYAN, COLOR_RESET);
    
    printf("  Dimensiune tabelă:  %s%zu%s\n", COLOR_YELLOW, ht->size, COLOR_RESET);
    printf("  Număr elemente:     %s%zu%s\n", COLOR_YELLOW, ht->count, COLOR_RESET);
    printf("  Load factor:        %s%.2f%s\n", COLOR_YELLOW, htc_load_factor(ht), COLOR_RESET);
    
    /* Calculează statistici despre liste */
    size_t empty_buckets = 0;
    size_t max_chain = 0;
    size_t total_chain = 0;
    
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
            total_chain += chain_len;
            if (chain_len > max_chain) {
                max_chain = chain_len;
            }
        }
    }
    
    size_t used_buckets = ht->size - empty_buckets;
    double avg_chain = used_buckets > 0 ? (double)total_chain / used_buckets : 0;
    
    printf("  Bucket-uri goale:   %s%zu%s (%.1f%%)\n", 
           COLOR_YELLOW, empty_buckets, COLOR_RESET, 
           100.0 * empty_buckets / ht->size);
    printf("  Lanț maxim:         %s%zu%s\n", COLOR_YELLOW, max_chain, COLOR_RESET);
    printf("  Lanț mediu:         %s%.2f%s\n", COLOR_YELLOW, avg_chain, COLOR_RESET);
}

/* =============================================================================
 * PARTEA 4: HASH TABLE CU OPEN ADDRESSING
 * =============================================================================
 */

/**
 * Creează o nouă hash table cu open addressing
 */
HashTableOpen* hto_create(size_t size) {
    HashTableOpen *ht = malloc(sizeof(HashTableOpen));
    if (!ht) return NULL;
    
    ht->slots = calloc(size, sizeof(HashNode*));
    if (!ht->slots) {
        free(ht);
        return NULL;
    }
    
    ht->size = size;
    ht->count = 0;
    ht->deleted_count = 0;
    return ht;
}

/**
 * Calculează load factor pentru open addressing
 */
double hto_load_factor(HashTableOpen *ht) {
    return (double)(ht->count + ht->deleted_count) / ht->size;
}

/**
 * Inserează o pereche cheie-valoare (open addressing - linear probing)
 */
void hto_insert(HashTableOpen *ht, const char *key, const char *value);

/**
 * Redimensionează tabela (open addressing)
 */
void hto_resize(HashTableOpen *ht, size_t new_size) {
    HashNode **old_slots = ht->slots;
    size_t old_size = ht->size;
    
    /* Alocă noua tabelă */
    ht->slots = calloc(new_size, sizeof(HashNode*));
    if (!ht->slots) {
        ht->slots = old_slots;
        return;
    }
    
    ht->size = new_size;
    ht->count = 0;
    ht->deleted_count = 0;
    
    /* Reinserează toate elementele (ignoră DELETED) */
    for (size_t i = 0; i < old_size; i++) {
        if (old_slots[i] != SLOT_EMPTY && old_slots[i] != SLOT_DELETED) {
            hto_insert(ht, old_slots[i]->key, old_slots[i]->value);
            free(old_slots[i]->key);
            free(old_slots[i]->value);
            free(old_slots[i]);
        }
    }
    
    free(old_slots);
}

/**
 * Inserează o pereche cheie-valoare (open addressing - linear probing)
 */
void hto_insert(HashTableOpen *ht, const char *key, const char *value) {
    /* Verifică load factor înainte de inserare */
    if (hto_load_factor(ht) > LOAD_FACTOR_OPEN) {
        hto_resize(ht, ht->size * 2 + 1);
    }
    
    size_t index = hash_djb2(key) % ht->size;
    size_t start = index;
    size_t first_deleted = ht->size;  /* Invalid index */
    
    do {
        if (ht->slots[index] == SLOT_EMPTY) {
            /* Slot gol - inserează */
            size_t insert_idx = (first_deleted < ht->size) ? first_deleted : index;
            
            HashNode *node = malloc(sizeof(HashNode));
            node->key = strdup(key);
            node->value = strdup(value);
            node->next = NULL;
            
            if (ht->slots[insert_idx] == SLOT_DELETED) {
                ht->deleted_count--;
            }
            ht->slots[insert_idx] = node;
            ht->count++;
            return;
        }
        
        if (ht->slots[index] == SLOT_DELETED) {
            /* Memorează prima poziție DELETED întâlnită */
            if (first_deleted == ht->size) {
                first_deleted = index;
            }
        } else if (strcmp(ht->slots[index]->key, key) == 0) {
            /* Cheie existentă - actualizează */
            free(ht->slots[index]->value);
            ht->slots[index]->value = strdup(value);
            return;
        }
        
        index = (index + 1) % ht->size;  /* Linear probing */
    } while (index != start);
    
    /* Nu ar trebui să ajungem aici dacă load factor e verificat */
    fprintf(stderr, "Eroare: Tabela plină!\n");
}

/**
 * Caută o cheie în hash table (open addressing)
 */
const char* hto_search(HashTableOpen *ht, const char *key) {
    size_t index = hash_djb2(key) % ht->size;
    size_t start = index;
    
    do {
        if (ht->slots[index] == SLOT_EMPTY) {
            return NULL;  /* Nu există */
        }
        
        if (ht->slots[index] != SLOT_DELETED &&
            strcmp(ht->slots[index]->key, key) == 0) {
            return ht->slots[index]->value;
        }
        
        index = (index + 1) % ht->size;
    } while (index != start);
    
    return NULL;
}

/**
 * Șterge o cheie din hash table (open addressing)
 */
bool hto_delete(HashTableOpen *ht, const char *key) {
    size_t index = hash_djb2(key) % ht->size;
    size_t start = index;
    
    do {
        if (ht->slots[index] == SLOT_EMPTY) {
            return false;  /* Nu există */
        }
        
        if (ht->slots[index] != SLOT_DELETED &&
            strcmp(ht->slots[index]->key, key) == 0) {
            /* Găsit - marchează ca DELETED */
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

/**
 * Eliberează memoria pentru hash table (open addressing)
 */
void hto_destroy(HashTableOpen *ht) {
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

/**
 * Afișează statistici pentru hash table (open addressing)
 */
void hto_print_stats(HashTableOpen *ht) {
    printf("\n%s╔═══════════════════════════════════════════════════════════════╗%s\n", COLOR_MAGENTA, COLOR_RESET);
    printf("%s║         STATISTICI HASH TABLE (OPEN ADDRESSING)               ║%s\n", COLOR_MAGENTA, COLOR_RESET);
    printf("%s╚═══════════════════════════════════════════════════════════════╝%s\n\n", COLOR_MAGENTA, COLOR_RESET);
    
    printf("  Dimensiune tabelă:  %s%zu%s\n", COLOR_YELLOW, ht->size, COLOR_RESET);
    printf("  Număr elemente:     %s%zu%s\n", COLOR_YELLOW, ht->count, COLOR_RESET);
    printf("  Sloturi DELETED:    %s%zu%s\n", COLOR_YELLOW, ht->deleted_count, COLOR_RESET);
    printf("  Load factor:        %s%.2f%s\n", COLOR_YELLOW, hto_load_factor(ht), COLOR_RESET);
    
    /* Calculează clustering */
    size_t max_cluster = 0;
    size_t current_cluster = 0;
    size_t total_probes = 0;
    
    for (size_t i = 0; i < ht->size; i++) {
        if (ht->slots[i] != SLOT_EMPTY && ht->slots[i] != SLOT_DELETED) {
            current_cluster++;
            if (current_cluster > max_cluster) {
                max_cluster = current_cluster;
            }
            
            /* Calculează numărul de probe-uri pentru acest element */
            size_t ideal = hash_djb2(ht->slots[i]->key) % ht->size;
            size_t actual = i;
            size_t probes = (actual >= ideal) ? (actual - ideal + 1) : 
                           (ht->size - ideal + actual + 1);
            total_probes += probes;
        } else {
            current_cluster = 0;
        }
    }
    
    double avg_probes = ht->count > 0 ? (double)total_probes / ht->count : 0;
    
    printf("  Cluster maxim:      %s%zu%s\n", COLOR_YELLOW, max_cluster, COLOR_RESET);
    printf("  Probe-uri medii:    %s%.2f%s\n", COLOR_YELLOW, avg_probes, COLOR_RESET);
}

/* =============================================================================
 * PARTEA 5: DEMONSTRAȚII
 * =============================================================================
 */

void demo_hash_functions(void) {
    printf("\n");
    printf("%s╔═══════════════════════════════════════════════════════════════╗%s\n", COLOR_BLUE, COLOR_RESET);
    printf("%s║     PARTEA 1: DEMONSTRAȚIE FUNCȚII HASH                       ║%s\n", COLOR_BLUE, COLOR_RESET);
    printf("%s╚═══════════════════════════════════════════════════════════════╝%s\n\n", COLOR_BLUE, COLOR_RESET);
    
    const char *test_strings[] = {
        "student", "Student", "STUDENT",
        "ana", "Ana", "ANA",
        "algoritm", "hash", "table",
        "abc", "acb", "bac", "bca", "cab", "cba"  /* Anagrame */
    };
    int num_tests = sizeof(test_strings) / sizeof(test_strings[0]);
    
    printf("  %-15s %15s %15s %15s\n", "Șir", "djb2", "sdbm", "FNV-1a");
    printf("  ─────────────────────────────────────────────────────────────\n");
    
    for (int i = 0; i < num_tests; i++) {
        printf("  %-15s %15lu %15lu %15lu\n",
               test_strings[i],
               hash_djb2(test_strings[i]),
               hash_sdbm(test_strings[i]),
               hash_fnv1a(test_strings[i]));
    }
    
    printf("\n  %sObservație:%s Anagramele (abc, acb, ...) au hash-uri diferite\n", 
           COLOR_GREEN, COLOR_RESET);
    printf("  datorită ordinii caracterelor în formula hash.\n");
}

void demo_chaining(void) {
    printf("\n");
    printf("%s╔═══════════════════════════════════════════════════════════════╗%s\n", COLOR_BLUE, COLOR_RESET);
    printf("%s║     PARTEA 2: HASH TABLE CU SEPARATE CHAINING                 ║%s\n", COLOR_BLUE, COLOR_RESET);
    printf("%s╚═══════════════════════════════════════════════════════════════╝%s\n\n", COLOR_BLUE, COLOR_RESET);
    
    HashTableChain *ht = htc_create(7);  /* Dimensiune mică pentru demonstrație */
    
    /* Date de test */
    const char *keys[] = {"Ion", "Maria", "Andrei", "Elena", "Mihai", 
                          "Ana", "George", "Ioana", "Alexandru", "Cristina"};
    const char *values[] = {"10", "9", "8", "10", "7", 
                            "9", "8", "10", "6", "9"};
    int num_entries = sizeof(keys) / sizeof(keys[0]);
    
    /* Inserare */
    printf("  %s► Inserare studenți:%s\n", COLOR_CYAN, COLOR_RESET);
    for (int i = 0; i < num_entries; i++) {
        size_t index = hash_djb2(keys[i]) % ht->size;
        printf("    %s%-12s%s → nota %s → bucket[%zu]\n", 
               COLOR_YELLOW, keys[i], COLOR_RESET, values[i], index);
        htc_insert(ht, keys[i], values[i]);
    }
    
    /* Afișează statistici */
    htc_print_stats(ht);
    
    /* Căutare */
    printf("\n  %s► Căutare:%s\n", COLOR_CYAN, COLOR_RESET);
    const char *search_keys[] = {"Maria", "Andrei", "Necunoscut"};
    for (int i = 0; i < 3; i++) {
        const char *result = htc_search(ht, search_keys[i]);
        if (result) {
            printf("    %s%-12s%s → nota: %s%s%s\n", 
                   COLOR_YELLOW, search_keys[i], COLOR_RESET,
                   COLOR_GREEN, result, COLOR_RESET);
        } else {
            printf("    %s%-12s%s → %snu există%s\n", 
                   COLOR_YELLOW, search_keys[i], COLOR_RESET,
                   COLOR_RED, COLOR_RESET);
        }
    }
    
    /* Ștergere */
    printf("\n  %s► Ștergere:%s\n", COLOR_CYAN, COLOR_RESET);
    if (htc_delete(ht, "Andrei")) {
        printf("    Andrei a fost șters cu succes\n");
    }
    
    /* Verificare după ștergere */
    const char *result = htc_search(ht, "Andrei");
    printf("    Căutare Andrei: %s\n", 
           result ? result : "(nu există)");
    
    /* Actualizare */
    printf("\n  %s► Actualizare:%s\n", COLOR_CYAN, COLOR_RESET);
    printf("    Ion: nota veche = %s\n", htc_search(ht, "Ion"));
    htc_insert(ht, "Ion", "10");  /* Update */
    printf("    Ion: nota nouă = %s\n", htc_search(ht, "Ion"));
    
    htc_destroy(ht);
    printf("\n  %s✓ Memorie eliberată%s\n", COLOR_GREEN, COLOR_RESET);
}

void demo_open_addressing(void) {
    printf("\n");
    printf("%s╔═══════════════════════════════════════════════════════════════╗%s\n", COLOR_BLUE, COLOR_RESET);
    printf("%s║     PARTEA 3: HASH TABLE CU OPEN ADDRESSING                   ║%s\n", COLOR_BLUE, COLOR_RESET);
    printf("%s╚═══════════════════════════════════════════════════════════════╝%s\n\n", COLOR_BLUE, COLOR_RESET);
    
    HashTableOpen *ht = hto_create(11);  /* Dimensiune mică pentru demonstrație */
    
    /* Date de test */
    const char *keys[] = {"alfa", "beta", "gamma", "delta", "epsilon",
                          "zeta", "eta", "theta"};
    const char *values[] = {"1", "2", "3", "4", "5", "6", "7", "8"};
    int num_entries = sizeof(keys) / sizeof(keys[0]);
    
    /* Inserare cu vizualizare probing */
    printf("  %s► Inserare cu linear probing:%s\n", COLOR_CYAN, COLOR_RESET);
    for (int i = 0; i < num_entries; i++) {
        size_t ideal = hash_djb2(keys[i]) % ht->size;
        hto_insert(ht, keys[i], values[i]);
        
        /* Găsește poziția reală */
        size_t actual = ideal;
        while (ht->slots[actual] == SLOT_EMPTY || 
               ht->slots[actual] == SLOT_DELETED ||
               strcmp(ht->slots[actual]->key, keys[i]) != 0) {
            actual = (actual + 1) % ht->size;
        }
        
        if (actual == ideal) {
            printf("    %s%-10s%s → slot[%zu] %s(direct)%s\n", 
                   COLOR_YELLOW, keys[i], COLOR_RESET, actual,
                   COLOR_GREEN, COLOR_RESET);
        } else {
            printf("    %s%-10s%s → slot[%zu] %s(ideal: %zu, probe: %zu)%s\n", 
                   COLOR_YELLOW, keys[i], COLOR_RESET, actual,
                   COLOR_RED, ideal, (actual - ideal + ht->size) % ht->size,
                   COLOR_RESET);
        }
    }
    
    /* Afișează statistici */
    hto_print_stats(ht);
    
    /* Vizualizare tabelă */
    printf("\n  %s► Vizualizare tabelă:%s\n", COLOR_CYAN, COLOR_RESET);
    printf("    ");
    for (size_t i = 0; i < ht->size; i++) {
        if (ht->slots[i] == SLOT_EMPTY) {
            printf("[%s-%s]", COLOR_BLUE, COLOR_RESET);
        } else if (ht->slots[i] == SLOT_DELETED) {
            printf("[%sX%s]", COLOR_RED, COLOR_RESET);
        } else {
            printf("[%s%c%s]", COLOR_GREEN, ht->slots[i]->key[0], COLOR_RESET);
        }
    }
    printf("\n");
    
    /* Demonstrare DELETED marker */
    printf("\n  %s► Demonstrare DELETED marker:%s\n", COLOR_CYAN, COLOR_RESET);
    printf("    Șterg 'gamma'...\n");
    hto_delete(ht, "gamma");
    
    printf("    Caut 'delta' (era după gamma în probing)...\n");
    const char *result = hto_search(ht, "delta");
    printf("    Rezultat: %s%s%s\n", 
           result ? COLOR_GREEN : COLOR_RED,
           result ? result : "nu există",
           COLOR_RESET);
    
    printf("\n    Tabelă după ștergere: ");
    for (size_t i = 0; i < ht->size; i++) {
        if (ht->slots[i] == SLOT_EMPTY) {
            printf("[%s-%s]", COLOR_BLUE, COLOR_RESET);
        } else if (ht->slots[i] == SLOT_DELETED) {
            printf("[%sX%s]", COLOR_RED, COLOR_RESET);
        } else {
            printf("[%s%c%s]", COLOR_GREEN, ht->slots[i]->key[0], COLOR_RESET);
        }
    }
    printf("\n    %s(X = DELETED, - = EMPTY)%s\n", COLOR_YELLOW, COLOR_RESET);
    
    hto_destroy(ht);
    printf("\n  %s✓ Memorie eliberată%s\n", COLOR_GREEN, COLOR_RESET);
}

void demo_comparison(void) {
    printf("\n");
    printf("%s╔═══════════════════════════════════════════════════════════════╗%s\n", COLOR_BLUE, COLOR_RESET);
    printf("%s║     PARTEA 4: COMPARAȚIE PERFORMANȚĂ                          ║%s\n", COLOR_BLUE, COLOR_RESET);
    printf("%s╚═══════════════════════════════════════════════════════════════╝%s\n\n", COLOR_BLUE, COLOR_RESET);
    
    const int NUM_ELEMENTS = 10000;
    char key[32], value[32];
    
    /* Test chaining */
    HashTableChain *htc = htc_create(INITIAL_SIZE);
    clock_t start = clock();
    
    for (int i = 0; i < NUM_ELEMENTS; i++) {
        snprintf(key, sizeof(key), "key_%d", i);
        snprintf(value, sizeof(value), "value_%d", i);
        htc_insert(htc, key, value);
    }
    
    clock_t insert_time_chain = clock() - start;
    
    start = clock();
    for (int i = 0; i < NUM_ELEMENTS; i++) {
        snprintf(key, sizeof(key), "key_%d", i);
        htc_search(htc, key);
    }
    clock_t search_time_chain = clock() - start;
    
    /* Test open addressing */
    HashTableOpen *hto = hto_create(INITIAL_SIZE);
    start = clock();
    
    for (int i = 0; i < NUM_ELEMENTS; i++) {
        snprintf(key, sizeof(key), "key_%d", i);
        snprintf(value, sizeof(value), "value_%d", i);
        hto_insert(hto, key, value);
    }
    
    clock_t insert_time_open = clock() - start;
    
    start = clock();
    for (int i = 0; i < NUM_ELEMENTS; i++) {
        snprintf(key, sizeof(key), "key_%d", i);
        hto_search(hto, key);
    }
    clock_t search_time_open = clock() - start;
    
    /* Afișare rezultate */
    printf("  %sTest cu %d elemente:%s\n\n", COLOR_CYAN, NUM_ELEMENTS, COLOR_RESET);
    
    printf("  %-25s %15s %15s\n", "Operație", "Chaining", "Open Addressing");
    printf("  ─────────────────────────────────────────────────────────────\n");
    printf("  %-25s %12.3f ms %12.3f ms\n", "Inserare",
           1000.0 * insert_time_chain / CLOCKS_PER_SEC,
           1000.0 * insert_time_open / CLOCKS_PER_SEC);
    printf("  %-25s %12.3f ms %12.3f ms\n", "Căutare",
           1000.0 * search_time_chain / CLOCKS_PER_SEC,
           1000.0 * search_time_open / CLOCKS_PER_SEC);
    printf("  %-25s %15zu %15zu\n", "Dimensiune finală",
           htc->size, hto->size);
    printf("  %-25s %14.2f%% %14.2f%%\n", "Load factor final",
           100 * htc_load_factor(htc), 100 * hto_load_factor(hto));
    
    htc_destroy(htc);
    hto_destroy(hto);
}

void demo_resize(void) {
    printf("\n");
    printf("%s╔═══════════════════════════════════════════════════════════════╗%s\n", COLOR_BLUE, COLOR_RESET);
    printf("%s║     PARTEA 5: DEMONSTRAȚIE REDIMENSIONARE                     ║%s\n", COLOR_BLUE, COLOR_RESET);
    printf("%s╚═══════════════════════════════════════════════════════════════╝%s\n\n", COLOR_BLUE, COLOR_RESET);
    
    HashTableChain *ht = htc_create(5);  /* Dimensiune foarte mică */
    
    printf("  %s► Urmărire redimensionare (inițial size=5):%s\n\n", COLOR_CYAN, COLOR_RESET);
    printf("  %-3s %-15s %8s %8s %10s\n", "#", "Cheie", "Size", "Count", "Load Factor");
    printf("  ─────────────────────────────────────────────────────────────\n");
    
    char key[32], value[32];
    for (int i = 1; i <= 20; i++) {
        snprintf(key, sizeof(key), "element_%d", i);
        snprintf(value, sizeof(value), "%d", i);
        
        size_t old_size = ht->size;
        htc_insert(ht, key, value);
        
        const char *resize_mark = (ht->size != old_size) ? 
            COLOR_RED "← RESIZE" COLOR_RESET : "";
        
        printf("  %3d %-15s %8zu %8zu %10.2f %s\n",
               i, key, ht->size, ht->count, htc_load_factor(ht), resize_mark);
    }
    
    htc_destroy(ht);
}

/* =============================================================================
 * FUNCȚIA PRINCIPALĂ
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("%s%s╔═══════════════════════════════════════════════════════════════╗%s\n", COLOR_BOLD, COLOR_GREEN, COLOR_RESET);
    printf("%s%s║     SĂPTĂMÂNA 11: TABELE DE DISPERSIE - EXEMPLU COMPLET       ║%s\n", COLOR_BOLD, COLOR_GREEN, COLOR_RESET);
    printf("%s%s╚═══════════════════════════════════════════════════════════════╝%s\n", COLOR_BOLD, COLOR_GREEN, COLOR_RESET);
    
    demo_hash_functions();
    demo_chaining();
    demo_open_addressing();
    demo_resize();
    demo_comparison();
    
    printf("\n");
    printf("%s%s╔═══════════════════════════════════════════════════════════════╗%s\n", COLOR_BOLD, COLOR_GREEN, COLOR_RESET);
    printf("%s%s║                    DEMONSTRAȚIE FINALIZATĂ                    ║%s\n", COLOR_BOLD, COLOR_GREEN, COLOR_RESET);
    printf("%s%s╚═══════════════════════════════════════════════════════════════╝%s\n\n", COLOR_BOLD, COLOR_GREEN, COLOR_RESET);
    
    return 0;
}
