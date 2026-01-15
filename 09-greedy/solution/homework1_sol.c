/**
 * =============================================================================
 * SOLUȚIE TEMĂ 1: Dicționar cu Arbore AVL
 * =============================================================================
 * 
 * Implementarea unui dicționar cheie-valoare folosind un arbore AVL
 * pentru stocarea eficientă a perechilor (cuvânt, definiție).
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -o homework1_sol homework1_sol.c
 * Utilizare: ./homework1_sol
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* =============================================================================
 * STRUCTURI DE DATE
 * =============================================================================
 */

#define MAX_WORD_LEN 64
#define MAX_DEF_LEN 256

typedef struct DictEntry {
    char word[MAX_WORD_LEN];
    char definition[MAX_DEF_LEN];
    struct DictEntry *left;
    struct DictEntry *right;
    int height;
} DictEntry;

/* =============================================================================
 * FUNCȚII AUXILIARE
 * =============================================================================
 */

int max(int a, int b) {
    return (a > b) ? a : b;
}

int height(DictEntry *node) {
    return (node == NULL) ? 0 : node->height;
}

int balance_factor(DictEntry *node) {
    if (node == NULL) return 0;
    return height(node->left) - height(node->right);
}

void update_height(DictEntry *node) {
    if (node != NULL) {
        node->height = 1 + max(height(node->left), height(node->right));
    }
}

DictEntry* create_entry(const char *word, const char *definition) {
    DictEntry *entry = (DictEntry*)malloc(sizeof(DictEntry));
    if (entry == NULL) {
        fprintf(stderr, "Eroare: Alocare memorie eșuată!\n");
        exit(EXIT_FAILURE);
    }
    
    strncpy(entry->word, word, MAX_WORD_LEN - 1);
    entry->word[MAX_WORD_LEN - 1] = '\0';
    
    strncpy(entry->definition, definition, MAX_DEF_LEN - 1);
    entry->definition[MAX_DEF_LEN - 1] = '\0';
    
    entry->left = NULL;
    entry->right = NULL;
    entry->height = 1;
    
    return entry;
}

/* =============================================================================
 * ROTAȚII
 * =============================================================================
 */

DictEntry* rotate_right(DictEntry *z) {
    DictEntry *y = z->left;
    DictEntry *T3 = y->right;
    
    y->right = z;
    z->left = T3;
    
    update_height(z);
    update_height(y);
    
    return y;
}

DictEntry* rotate_left(DictEntry *z) {
    DictEntry *y = z->right;
    DictEntry *T2 = y->left;
    
    y->left = z;
    z->right = T2;
    
    update_height(z);
    update_height(y);
    
    return y;
}

/* =============================================================================
 * OPERAȚII DICȚIONAR
 * =============================================================================
 */

/**
 * Inserează sau actualizează o intrare în dicționar.
 */
DictEntry* dict_insert(DictEntry *root, const char *word, const char *definition) {
    if (root == NULL) {
        return create_entry(word, definition);
    }
    
    int cmp = strcmp(word, root->word);
    
    if (cmp < 0) {
        root->left = dict_insert(root->left, word, definition);
    } else if (cmp > 0) {
        root->right = dict_insert(root->right, word, definition);
    } else {
        /* Cuvântul există - actualizăm definiția */
        strncpy(root->definition, definition, MAX_DEF_LEN - 1);
        root->definition[MAX_DEF_LEN - 1] = '\0';
        return root;
    }
    
    update_height(root);
    
    int bf = balance_factor(root);
    
    /* Cazul LL */
    if (bf > 1 && strcmp(word, root->left->word) < 0) {
        return rotate_right(root);
    }
    
    /* Cazul RR */
    if (bf < -1 && strcmp(word, root->right->word) > 0) {
        return rotate_left(root);
    }
    
    /* Cazul LR */
    if (bf > 1 && strcmp(word, root->left->word) > 0) {
        root->left = rotate_left(root->left);
        return rotate_right(root);
    }
    
    /* Cazul RL */
    if (bf < -1 && strcmp(word, root->right->word) < 0) {
        root->right = rotate_right(root->right);
        return rotate_left(root);
    }
    
    return root;
}

/**
 * Caută un cuvânt în dicționar.
 */
const char* dict_lookup(DictEntry *root, const char *word) {
    if (root == NULL) {
        return NULL;
    }
    
    int cmp = strcmp(word, root->word);
    
    if (cmp == 0) {
        return root->definition;
    } else if (cmp < 0) {
        return dict_lookup(root->left, word);
    } else {
        return dict_lookup(root->right, word);
    }
}

/**
 * Găsește intrarea cu cuvântul minim (pentru ștergere).
 */
DictEntry* dict_min_entry(DictEntry *node) {
    DictEntry *current = node;
    while (current != NULL && current->left != NULL) {
        current = current->left;
    }
    return current;
}

/**
 * Șterge o intrare din dicționar.
 */
DictEntry* dict_delete(DictEntry *root, const char *word) {
    if (root == NULL) {
        return root;
    }
    
    int cmp = strcmp(word, root->word);
    
    if (cmp < 0) {
        root->left = dict_delete(root->left, word);
    } else if (cmp > 0) {
        root->right = dict_delete(root->right, word);
    } else {
        /* Intrarea de șters găsită */
        if (root->left == NULL || root->right == NULL) {
            DictEntry *temp = root->left ? root->left : root->right;
            
            if (temp == NULL) {
                temp = root;
                root = NULL;
            } else {
                *root = *temp;
            }
            free(temp);
        } else {
            DictEntry *temp = dict_min_entry(root->right);
            strcpy(root->word, temp->word);
            strcpy(root->definition, temp->definition);
            root->right = dict_delete(root->right, temp->word);
        }
    }
    
    if (root == NULL) {
        return root;
    }
    
    update_height(root);
    
    int bf = balance_factor(root);
    
    /* Reechilibrare */
    if (bf > 1 && balance_factor(root->left) >= 0) {
        return rotate_right(root);
    }
    
    if (bf > 1 && balance_factor(root->left) < 0) {
        root->left = rotate_left(root->left);
        return rotate_right(root);
    }
    
    if (bf < -1 && balance_factor(root->right) <= 0) {
        return rotate_left(root);
    }
    
    if (bf < -1 && balance_factor(root->right) > 0) {
        root->right = rotate_right(root->right);
        return rotate_left(root);
    }
    
    return root;
}

/**
 * Afișează toate intrările în ordine alfabetică.
 */
void dict_print_all(DictEntry *root) {
    if (root != NULL) {
        dict_print_all(root->left);
        printf("  %-20s : %s\n", root->word, root->definition);
        dict_print_all(root->right);
    }
}

/**
 * Numără intrările din dicționar.
 */
int dict_count(DictEntry *root) {
    if (root == NULL) return 0;
    return 1 + dict_count(root->left) + dict_count(root->right);
}

/**
 * Eliberează memoria dicționarului.
 */
void dict_free(DictEntry *root) {
    if (root != NULL) {
        dict_free(root->left);
        dict_free(root->right);
        free(root);
    }
}

/* =============================================================================
 * FUNCȚIA MAIN - DEMONSTRAȚIE
 * =============================================================================
 */

int main(void) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     TEMĂ 1: Dicționar cu Arbore AVL                           ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    DictEntry *dict = NULL;
    
    /* Adăugare intrări */
    printf("═══ Adăugare intrări în dicționar ═══\n\n");
    
    dict = dict_insert(dict, "algoritm", 
        "Secvență finită de instrucțiuni bine definite pentru rezolvarea unei probleme");
    dict = dict_insert(dict, "arbore", 
        "Structură de date ierarhică formată din noduri conectate");
    dict = dict_insert(dict, "recursie", 
        "Tehnică de programare în care o funcție se apelează pe sine însăși");
    dict = dict_insert(dict, "heap", 
        "Structură de date arborescentă cu proprietatea de heap");
    dict = dict_insert(dict, "graf", 
        "Structură matematică formată din noduri și muchii");
    dict = dict_insert(dict, "stivă", 
        "Structură de date LIFO (Last In, First Out)");
    dict = dict_insert(dict, "coadă", 
        "Structură de date FIFO (First In, First Out)");
    dict = dict_insert(dict, "hash", 
        "Funcție care mapează date de dimensiune arbitrară la valori de dimensiune fixă");
    dict = dict_insert(dict, "sortare", 
        "Proces de aranjare a elementelor într-o anumită ordine");
    dict = dict_insert(dict, "căutare", 
        "Proces de găsire a unui element într-o colecție de date");
    
    printf("✓ Dicționar populat cu %d intrări\n\n", dict_count(dict));
    
    /* Afișare completă */
    printf("═══ Conținut dicționar (ordine alfabetică) ═══\n\n");
    dict_print_all(dict);
    
    /* Căutare */
    printf("\n═══ Teste de căutare ═══\n\n");
    
    const char *test_words[] = {"algoritm", "arbore", "matrice", "recursie"};
    int n_tests = sizeof(test_words) / sizeof(test_words[0]);
    
    for (int i = 0; i < n_tests; i++) {
        const char *def = dict_lookup(dict, test_words[i]);
        if (def != NULL) {
            printf("✓ '%s': %s\n", test_words[i], def);
        } else {
            printf("✗ '%s': Nu a fost găsit\n", test_words[i]);
        }
    }
    
    /* Actualizare */
    printf("\n═══ Actualizare definiție ═══\n\n");
    dict = dict_insert(dict, "algoritm", 
        "Procedură pas cu pas pentru rezolvarea unei probleme (definiție actualizată)");
    printf("✓ Definiția pentru 'algoritm' a fost actualizată\n");
    printf("  Noua definiție: %s\n", dict_lookup(dict, "algoritm"));
    
    /* Ștergere */
    printf("\n═══ Ștergere intrări ═══\n\n");
    dict = dict_delete(dict, "heap");
    printf("✓ Intrarea 'heap' a fost ștearsă\n");
    printf("  Număr intrări rămase: %d\n", dict_count(dict));
    
    /* Verificare ștergere */
    if (dict_lookup(dict, "heap") == NULL) {
        printf("✓ Confirmare: 'heap' nu mai există în dicționar\n");
    }
    
    /* Afișare finală */
    printf("\n═══ Dicționar final ═══\n\n");
    dict_print_all(dict);
    
    /* Eliberare memorie */
    dict_free(dict);
    
    printf("\n✓ Memorie eliberată. Program terminat cu succes!\n");
    
    return 0;
}
