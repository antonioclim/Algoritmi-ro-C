#define _POSIX_C_SOURCE 200809L
/**
 * =============================================================================
 * EXERCIȚIUL 1: HASH TABLE CU SEPARATE CHAINING
 * =============================================================================
 *
 * OBIECTIV:
 *   Implementarea unei hash table complete folosind tehnica separate chaining
 *   pentru rezolvarea coliziunilor.
 *
 * CERINȚE:
 *   1. Definirea structurilor HashNode și HashTable
 *   2. Implementarea funcției hash djb2
 *   3. Implementarea funcțiilor: create, insert, search, delete, destroy
 *   4. Redimensionare automată când load factor > 2.0
 *   5. Afișarea statisticilor tabelei
 *   6. Tratarea corectă a duplicatelor (actualizare valoare)
 *
 * EXEMPLU INPUT:
 *   INSERT Ion 10
 *   INSERT Maria 9
 *   SEARCH Ion
 *   DELETE Maria
 *   STATS
 *
 * EXEMPLU OUTPUT:
 *   [INSERT] Ion: 10 -> bucket[3]
 *   [INSERT] Maria: 9 -> bucket[7]
 *   [SEARCH] Ion: 10
 *   [DELETE] Maria: OK
 *   [STATS] Size: 17, Count: 1, Load: 0.06
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -g -o exercise1 exercise1.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* =============================================================================
 * CONSTANTE
 * =============================================================================
 */

#define INITIAL_SIZE      17    /* Dimensiune inițială (număr prim) */
#define MAX_LOAD_FACTOR   2.0   /* Prag pentru redimensionare */
#define MAX_KEY_LENGTH    64    /* Lungimea maximă a unei chei */
#define MAX_VALUE_LENGTH  256   /* Lungimea maximă a unei valori */

/* =============================================================================
 * DEFINIȚII DE TIPURI
 * =============================================================================
 */

/**
 * TODO 1: Definește structura HashNode
 *
 * Un nod din lista înlănțuită trebuie să conțină:
 * - key: pointer la șir de caractere (cheia)
 * - value: pointer la șir de caractere (valoarea)
 * - next: pointer la următorul nod din listă
 *
 * Hint: Folosește char* pentru key și value, struct HashNode* pentru next
 */

/* YOUR CODE HERE - Definește typedef struct HashNode */
typedef struct HashNode {
    /* TODO: Adaugă câmpurile necesare */
    char *key;
    char *value;
    struct HashNode *next;
} HashNode;


/**
 * TODO 2: Definește structura HashTable
 *
 * Tabela de dispersie trebuie să conțină:
 * - buckets: array de pointeri la HashNode (listele înlănțuite)
 * - size: dimensiunea tabelei (numărul de bucket-uri)
 * - count: numărul de elemente din tabelă
 *
 * Hint: buckets este de tip HashNode** (array de pointeri)
 */

/* YOUR CODE HERE - Definește typedef struct HashTable */
typedef struct {
    /* TODO: Adaugă câmpurile necesare */
} HashTable;


/* =============================================================================
 * FUNCȚIA HASH
 * =============================================================================
 */

/**
 * TODO 3: Implementează funcția hash djb2
 *
 * Algoritmul djb2 (Dan Bernstein):
 *   1. Inițializează hash cu 5381
 *   2. Pentru fiecare caracter c din șir:
 *      hash = hash * 33 + c
 *      (sau echivalent: hash = ((hash << 5) + hash) + c)
 *   3. Returnează hash
 *
 * @param str - șirul de caractere de hash-uit
 * @return - valoarea hash (unsigned long)
 *
 * Hint: Folosește unsigned long pentru hash și int pentru caracterul c
 */
unsigned long hash_djb2(const char *str) {
    /* YOUR CODE HERE */
    
    return 0;  /* Înlocuiește cu valoarea corectă */
}


/* =============================================================================
 * FUNCȚII DE BAZĂ
 * =============================================================================
 */

/**
 * TODO 4: Implementează funcția de creare a hash table
 *
 * Pași:
 *   1. Alocă memorie pentru structura HashTable
 *   2. Verifică dacă alocarea a reușit
 *   3. Alocă memorie pentru array-ul de bucket-uri (folosește calloc pentru inițializare cu NULL)
 *   4. Verifică dacă alocarea a reușit (eliberează memoria deja alocată dacă nu)
 *   5. Inițializează size cu valoarea primită și count cu 0
 *   6. Returnează pointer-ul la tabela creată
 *
 * @param size - dimensiunea inițială a tabelei
 * @return - pointer la HashTable sau NULL în caz de eroare
 */
HashTable* ht_create(size_t size) {
    /* YOUR CODE HERE */
    
    return NULL;  /* Înlocuiește cu pointer-ul corect */
}

/**
 * Calculează load factor-ul curent
 * Load factor = număr_elemente / dimensiune_tabelă
 */
double ht_load_factor(HashTable *ht) {
    if (ht == NULL || ht->size == 0) return 0.0;
    return (double)ht->count / ht->size;
}

/**
 * TODO 5: Implementează funcția de redimensionare
 *
 * Pași:
 *   1. Salvează array-ul vechi de bucket-uri și dimensiunea veche
 *   2. Alocă un nou array de bucket-uri cu noua dimensiune
 *   3. Actualizează size și resetează count la 0
 *   4. Parcurge toate bucket-urile vechi:
 *      - Pentru fiecare nod, recalculează indexul pentru noua dimensiune
 *      - Inserează nodul în noul array
 *      - Incrementează count
 *   5. Eliberează array-ul vechi (dar NU nodurile!)
 *
 * @param ht - pointer la hash table
 * @param new_size - noua dimensiune
 *
 * Hint: Nu elibera nodurile, doar le muți în noul array
 */
void ht_resize(HashTable *ht, size_t new_size) {
    if (ht == NULL) return;
    
    /* YOUR CODE HERE */
    
    /* Hint: Structura codului
     *
     * HashNode **old_buckets = ht->buckets;
     * size_t old_size = ht->size;
     *
     * ht->buckets = calloc(new_size, sizeof(HashNode*));
     * if (!ht->buckets) { ... handle error ... }
     *
     * ht->size = new_size;
     * ht->count = 0;
     *
     * for (size_t i = 0; i < old_size; i++) {
     *     HashNode *node = old_buckets[i];
     *     while (node) {
     *         HashNode *next = node->next;
     *         // Recalculează indexul și inserează
     *         ...
     *         node = next;
     *     }
     * }
     *
     * free(old_buckets);
     */
}

/**
 * TODO 6: Implementează funcția de inserare
 *
 * Pași:
 *   1. Calculează indexul: hash(key) % size
 *   2. Caută în listă dacă cheia există deja:
 *      - Dacă da, actualizează valoarea și return
 *   3. Creează un nod nou și copiază cheia și valoarea (folosește strdup)
 *   4. Inserează nodul la ÎNCEPUTUL listei (O(1))
 *   5. Incrementează count
 *   6. Verifică load factor și redimensionează dacă > MAX_LOAD_FACTOR
 *
 * @param ht - pointer la hash table
 * @param key - cheia de inserat
 * @param value - valoarea asociată
 *
 * Hint: Nu uita să eliberezi valoarea veche la actualizare!
 */
void ht_insert(HashTable *ht, const char *key, const char *value) {
    if (ht == NULL || key == NULL) return;
    
    /* YOUR CODE HERE */
    
    /* Hint: Structura codului
     *
     * size_t index = hash_djb2(key) % ht->size;
     *
     * // Caută cheia existentă
     * HashNode *current = ht->buckets[index];
     * while (current) {
     *     if (strcmp(current->key, key) == 0) {
     *         // Actualizează valoarea
     *         ...
     *         return;
     *     }
     *     current = current->next;
     * }
     *
     * // Cheie nouă - creează nod
     * HashNode *new_node = malloc(sizeof(HashNode));
     * new_node->key = strdup(key);
     * new_node->value = strdup(value);
     * new_node->next = ht->buckets[index];  // Inserare la început
     * ht->buckets[index] = new_node;
     * ht->count++;
     *
     * // Verifică pentru redimensionare
     * if (ht_load_factor(ht) > MAX_LOAD_FACTOR) {
     *     ht_resize(ht, ht->size * 2 + 1);
     * }
     */
}

/**
 * TODO 7: Implementează funcția de căutare
 *
 * Pași:
 *   1. Calculează indexul: hash(key) % size
 *   2. Parcurge lista de la acel index
 *   3. Compară fiecare cheie cu cheia căutată
 *   4. Returnează valoarea dacă găsită, NULL altfel
 *
 * @param ht - pointer la hash table
 * @param key - cheia de căutat
 * @return - pointer la valoare sau NULL dacă nu există
 */
const char* ht_search(HashTable *ht, const char *key) {
    if (ht == NULL || key == NULL) return NULL;
    
    /* YOUR CODE HERE */
    
    return NULL;  /* Înlocuiește cu valoarea corectă */
}

/**
 * TODO 8: Implementează funcția de ștergere
 *
 * Pași:
 *   1. Calculează indexul: hash(key) % size
 *   2. Parcurge lista păstrând referința la nodul anterior
 *   3. Când găsești cheia:
 *      - Actualizează pointer-ul previous->next sau buckets[index]
 *      - Eliberează memoria pentru key, value și nod
 *      - Decrementează count
 *      - Returnează true
 *   4. Dacă nu găsești, returnează false
 *
 * @param ht - pointer la hash table
 * @param key - cheia de șters
 * @return - true dacă a fost ștersă, false dacă nu exista
 */
bool ht_delete(HashTable *ht, const char *key) {
    if (ht == NULL || key == NULL) return false;
    
    /* YOUR CODE HERE */
    
    return false;  /* Înlocuiește cu valoarea corectă */
}

/**
 * TODO 9: Implementează funcția de eliberare a memoriei
 *
 * Pași:
 *   1. Pentru fiecare bucket (index de la 0 la size-1):
 *      - Parcurge lista și pentru fiecare nod:
 *        - Eliberează key
 *        - Eliberează value
 *        - Eliberează nodul
 *   2. Eliberează array-ul de bucket-uri
 *   3. Eliberează structura HashTable
 *
 * @param ht - pointer la hash table
 */
void ht_destroy(HashTable *ht) {
    if (ht == NULL) return;
    
    /* YOUR CODE HERE */
}

/* =============================================================================
 * FUNCȚII AUXILIARE (IMPLEMENTATE)
 * =============================================================================
 */

/**
 * Afișează statisticile hash table-ului
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

/**
 * Afișează conținutul hash table-ului
 */
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

/**
 * TODO 10: Completează funcția main pentru a procesa comenzile
 *
 * Comenzile acceptate:
 *   INSERT <key> <value> - inserează o pereche cheie-valoare
 *   SEARCH <key>         - caută o cheie
 *   DELETE <key>         - șterge o cheie
 *   STATS                - afișează statisticile
 *   PRINT                - afișează conținutul
 *   QUIT                 - termină programul
 */
int main(void) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     EXERCIȚIUL 1: HASH TABLE CU SEPARATE CHAINING             ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* TODO: Creează hash table */
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
                /* YOUR CODE HERE - Apelează ht_insert și afișează rezultatul */
                size_t index = hash_djb2(key) % ht->size;
                ht_insert(ht, key, value);
                printf("[INSERT] %s: %s -> bucket[%zu]\n", key, value, index);
            }
        }
        else if (strcmp(command, "SEARCH") == 0) {
            if (scanf("%63s", key) == 1) {
                /* YOUR CODE HERE - Apelează ht_search și afișează rezultatul */
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
                /* YOUR CODE HERE - Apelează ht_delete și afișează rezultatul */
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
    
    /* Eliberează memoria */
    ht_destroy(ht);
    printf("\n[INFO] Memorie eliberată. Program terminat.\n");
    
    return 0;
}

/* =============================================================================
 * PROVOCĂRI BONUS (Opțional)
 * =============================================================================
 *
 * 1. Implementează iterarea peste toate elementele (ht_foreach)
 * 2. Adaugă suport pentru valori de orice tip (void*)
 * 3. Implementează ht_keys() care returnează un array de chei
 * 4. Implementează ht_values() care returnează un array de valori
 * 5. Adaugă funcție de statistică a distribuției (histogramă)
 *
 * =============================================================================
 */
