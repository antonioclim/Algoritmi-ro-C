/**
 * =============================================================================
 * EXERCIȚIUL 1: Dicționar Român-Englez cu BST
 * =============================================================================
 *
 * OBIECTIV:
 *   Implementați un dicționar român-englez folosind un Binary Search Tree.
 *   Cuvintele sunt ordonate alfabetic după termenul românesc.
 *
 * CERINȚE:
 *   1. Definiți structura pentru o intrare de dicționar
 *   2. Implementați inserarea cu menținerea ordinii alfabetice
 *   3. Implementați căutarea cu feedback pentru cuvinte negăsite
 *   4. Implementați ștergerea unei intrări
 *   5. Salvați dicționarul în fișier text
 *   6. Încărcați dicționarul din fișier text
 *   7. Afișați statistici: număr cuvinte, înălțime arbore
 *
 * EXEMPLU INPUT:
 *   Comandă: add casa house
 *   Comandă: add masă table
 *   Comandă: search casa
 *
 * EXPECTED OUTPUT:
 *   Adăugat: casa -> house
 *   Adăugat: masă -> table
 *   Găsit: casa -> house
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -o exercise1 exercise1.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Lungimea maximă pentru un cuvânt */
#define MAX_WORD_LENGTH 100

/* =============================================================================
 * DEFINIREA TIPURILOR
 * =============================================================================
 */

/**
 * TODO 1: Definiți structura DictEntry pentru o intrare în dicționar
 *
 * Fiecare intrare trebuie să conțină:
 * - word_ro: cuvântul în română (cheia de căutare)
 * - word_en: traducerea în engleză
 * - left: pointer către subarborele stâng
 * - right: pointer către subarborele drept
 *
 * Hint: Folosiți char arrays de dimensiune MAX_WORD_LENGTH
 */

typedef struct DictEntry {
    /* YOUR CODE HERE */
    char word_ro[MAX_WORD_LENGTH];   /* Exemplu - modificați/completați */
    char word_en[MAX_WORD_LENGTH];
    struct DictEntry *left;
    struct DictEntry *right;
} DictEntry;

/**
 * TODO 2: Definiți structura Dictionary pentru arbore
 *
 * Trebuie să conțină:
 * - root: pointer către rădăcină
 * - size: numărul de intrări
 */

typedef struct {
    /* YOUR CODE HERE */
    DictEntry *root;
    size_t size;
} Dictionary;

/* =============================================================================
 * FUNCȚII DE CREARE
 * =============================================================================
 */

/**
 * TODO 3: Implementați funcția de creare a unei intrări noi
 *
 * @param word_ro Cuvântul în română
 * @param word_en Traducerea în engleză
 * @return Pointer către intrarea creată sau NULL la eroare
 *
 * Pași:
 *   1. Alocați memorie pentru DictEntry
 *   2. Verificați dacă alocarea a reușit
 *   3. Copiați cuvintele în structură (folosiți strncpy pentru siguranță)
 *   4. Inițializați pointerii left și right cu NULL
 *   5. Returnați pointerul
 */
DictEntry* dict_create_entry(const char *word_ro, const char *word_en) {
    /* YOUR CODE HERE */
    
    return NULL;  /* Înlocuiți cu implementarea corectă */
}

/**
 * Creează un dicționar gol (implementare furnizată)
 */
Dictionary* dict_create(void) {
    Dictionary *dict = (Dictionary*)malloc(sizeof(Dictionary));
    if (dict == NULL) {
        fprintf(stderr, "Eroare: Nu s-a putut aloca memorie pentru dicționar!\n");
        return NULL;
    }
    dict->root = NULL;
    dict->size = 0;
    return dict;
}

/* =============================================================================
 * FUNCȚII DE INSERARE
 * =============================================================================
 */

/**
 * TODO 4: Implementați inserarea recursivă
 *
 * @param entry Rădăcina subarborelui curent
 * @param word_ro Cuvântul în română (cheie)
 * @param word_en Traducerea în engleză
 * @return Noua rădăcină a subarborelui
 *
 * Pași:
 *   1. Dacă entry este NULL, creați o intrare nouă și returnați-o
 *   2. Comparați word_ro cu entry->word_ro folosind strcmp()
 *   3. Dacă word_ro < entry->word_ro, inserați în stânga
 *   4. Dacă word_ro > entry->word_ro, inserați în dreapta
 *   5. Dacă sunt egale, actualizați traducerea (sau ignorați)
 *   6. Returnați entry
 *
 * Hint: strcmp() returnează <0, 0, sau >0
 */
DictEntry* dict_insert_entry(DictEntry *entry, const char *word_ro, const char *word_en) {
    /* YOUR CODE HERE */
    
    return entry;  /* Înlocuiți cu implementarea corectă */
}

/**
 * Adaugă un cuvânt în dicționar
 */
void dict_add(Dictionary *dict, const char *word_ro, const char *word_en) {
    if (dict == NULL || word_ro == NULL || word_en == NULL) return;
    
    dict->root = dict_insert_entry(dict->root, word_ro, word_en);
    dict->size++;
    
    printf("Adăugat: %s -> %s\n", word_ro, word_en);
}

/* =============================================================================
 * FUNCȚII DE CĂUTARE
 * =============================================================================
 */

/**
 * TODO 5: Implementați căutarea în dicționar
 *
 * @param entry Rădăcina subarborelui
 * @param word_ro Cuvântul de căutat
 * @return Pointer către intrarea găsită sau NULL
 *
 * Pași:
 *   1. Dacă entry este NULL, returnați NULL
 *   2. Comparați word_ro cu entry->word_ro
 *   3. Dacă sunt egale, returnați entry
 *   4. Dacă word_ro < entry->word_ro, căutați în stânga
 *   5. Altfel, căutați în dreapta
 */
DictEntry* dict_search(DictEntry *entry, const char *word_ro) {
    /* YOUR CODE HERE */
    
    return NULL;  /* Înlocuiți cu implementarea corectă */
}

/**
 * Caută și afișează traducerea unui cuvânt
 */
void dict_lookup(Dictionary *dict, const char *word_ro) {
    if (dict == NULL || word_ro == NULL) return;
    
    DictEntry *result = dict_search(dict->root, word_ro);
    
    if (result != NULL) {
        printf("Găsit: %s -> %s\n", result->word_ro, result->word_en);
    } else {
        printf("Negăsit: '%s' nu există în dicționar.\n", word_ro);
        
        /* TODO 6 (BONUS): Sugerați cuvinte similare */
        /* Hint: Căutați cuvinte care încep cu aceleași litere */
    }
}

/* =============================================================================
 * FUNCȚII DE ȘTERGERE
 * =============================================================================
 */

/**
 * Găsește intrarea cu valoarea minimă (cel mai din stânga)
 */
DictEntry* dict_find_min(DictEntry *entry) {
    if (entry == NULL) return NULL;
    
    while (entry->left != NULL) {
        entry = entry->left;
    }
    return entry;
}

/**
 * TODO 7: Implementați ștergerea unei intrări
 *
 * @param entry Rădăcina subarborelui
 * @param word_ro Cuvântul de șters
 * @return Noua rădăcină a subarborelui
 *
 * Tratați cele 3 cazuri:
 *   1. Nod frunză (0 copii) - eliberați și returnați NULL
 *   2. Nod cu 1 copil - eliberați și returnați copilul
 *   3. Nod cu 2 copii - înlocuiți cu succesorul inorder
 *
 * ATENȚIE: Nu uitați să eliberați memoria cu free()!
 */
DictEntry* dict_delete_entry(DictEntry *entry, const char *word_ro) {
    if (entry == NULL) return NULL;
    
    int cmp = strcmp(word_ro, entry->word_ro);
    
    if (cmp < 0) {
        entry->left = dict_delete_entry(entry->left, word_ro);
    } else if (cmp > 0) {
        entry->right = dict_delete_entry(entry->right, word_ro);
    } else {
        /* Am găsit nodul de șters */
        
        /* TODO: Implementați cele 3 cazuri */
        /* YOUR CODE HERE */
        
        /* Cazul 1 & 2: 0 sau 1 copil */
        
        /* Cazul 3: 2 copii - înlocuire cu succesor */
    }
    
    return entry;
}

/**
 * Șterge un cuvânt din dicționar
 */
void dict_remove(Dictionary *dict, const char *word_ro) {
    if (dict == NULL || word_ro == NULL) return;
    
    DictEntry *found = dict_search(dict->root, word_ro);
    if (found == NULL) {
        printf("Eroare: '%s' nu există în dicționar.\n", word_ro);
        return;
    }
    
    dict->root = dict_delete_entry(dict->root, word_ro);
    dict->size--;
    printf("Șters: %s\n", word_ro);
}

/* =============================================================================
 * FUNCȚII DE PARCURGERE ȘI AFIȘARE
 * =============================================================================
 */

/**
 * TODO 8: Implementați afișarea în ordine alfabetică (inorder)
 *
 * @param entry Rădăcina subarborelui
 *
 * Format de afișare: "  cuvânt_ro -> cuvânt_en"
 */
void dict_print_inorder(DictEntry *entry) {
    if (entry == NULL) return;
    
    /* YOUR CODE HERE */
    /* Hint: Parcurgere inorder - stânga, curent, dreapta */
}

/**
 * Afișează tot dicționarul
 */
void dict_print_all(Dictionary *dict) {
    if (dict == NULL || dict->root == NULL) {
        printf("Dicționarul este gol.\n");
        return;
    }
    
    printf("\n=== Dicționar (%zu intrări) ===\n", dict->size);
    dict_print_inorder(dict->root);
    printf("==============================\n");
}

/* =============================================================================
 * FUNCȚII PENTRU FIȘIERE
 * =============================================================================
 */

/**
 * TODO 9: Implementați salvarea dicționarului în fișier
 *
 * @param entry Rădăcina subarborelui
 * @param file Fișierul deschis pentru scriere
 *
 * Format: un cuvânt per linie, "word_ro:word_en"
 * Folosiți parcurgere inorder pentru a salva în ordine alfabetică
 */
void dict_save_entries(DictEntry *entry, FILE *file) {
    if (entry == NULL || file == NULL) return;
    
    /* YOUR CODE HERE */
    /* Hint: fprintf(file, "%s:%s\n", entry->word_ro, entry->word_en); */
}

/**
 * Salvează dicționarul în fișier
 */
int dict_save_to_file(Dictionary *dict, const char *filename) {
    if (dict == NULL || filename == NULL) return -1;
    
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Eroare: Nu s-a putut deschide fișierul '%s' pentru scriere.\n", filename);
        return -1;
    }
    
    dict_save_entries(dict->root, file);
    fclose(file);
    
    printf("Dicționarul a fost salvat în '%s' (%zu intrări).\n", filename, dict->size);
    return 0;
}

/**
 * TODO 10: Implementați încărcarea dicționarului din fișier
 *
 * @param dict Dicționarul în care se încarcă
 * @param filename Numele fișierului
 * @return 0 la succes, -1 la eroare
 *
 * Pași:
 *   1. Deschideți fișierul pentru citire
 *   2. Citiți linie cu linie
 *   3. Parseați fiecare linie (format: "word_ro:word_en")
 *   4. Adăugați în dicționar
 *   5. Închideți fișierul
 *
 * Hint: Folosiți fgets() și strtok() sau strchr()
 */
int dict_load_from_file(Dictionary *dict, const char *filename) {
    if (dict == NULL || filename == NULL) return -1;
    
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Eroare: Nu s-a putut deschide fișierul '%s' pentru citire.\n", filename);
        return -1;
    }
    
    char line[MAX_WORD_LENGTH * 2 + 2];
    int count = 0;
    
    while (fgets(line, sizeof(line), file) != NULL) {
        /* YOUR CODE HERE */
        
        /* Hint: 
         * 1. Eliminați newline-ul de la sfârșitul liniei
         * 2. Găsiți separatorul ':'
         * 3. Extrageți word_ro și word_en
         * 4. Apelați dict_insert_entry()
         */
        
        count++;
    }
    
    fclose(file);
    printf("Încărcate %d intrări din '%s'.\n", count, filename);
    return 0;
}

/* =============================================================================
 * FUNCȚII STATISTICE
 * =============================================================================
 */

/**
 * TODO 11: Calculați înălțimea arborelui
 *
 * @param entry Rădăcina subarborelui
 * @return Înălțimea (-1 pentru arbore vid)
 */
int dict_height(DictEntry *entry) {
    /* YOUR CODE HERE */
    
    return -1;  /* Înlocuiți cu implementarea corectă */
}

/**
 * Afișează statistici despre dicționar
 */
void dict_stats(Dictionary *dict) {
    if (dict == NULL) return;
    
    printf("\n=== Statistici Dicționar ===\n");
    printf("Număr intrări: %zu\n", dict->size);
    printf("Înălțime arbore: %d\n", dict_height(dict->root));
    
    if (dict->root != NULL) {
        DictEntry *min = dict_find_min(dict->root);
        printf("Primul cuvânt (alfabetic): %s\n", min ? min->word_ro : "N/A");
        
        /* TODO 12 (BONUS): Găsiți și ultimul cuvânt (maxim) */
    }
    printf("============================\n");
}

/* =============================================================================
 * ELIBERARE MEMORIE
 * =============================================================================
 */

/**
 * Eliberează recursiv toate intrările (postorder)
 */
void dict_free_entries(DictEntry *entry) {
    if (entry == NULL) return;
    
    dict_free_entries(entry->left);
    dict_free_entries(entry->right);
    free(entry);
}

/**
 * Distruge dicționarul complet
 */
void dict_destroy(Dictionary *dict) {
    if (dict == NULL) return;
    
    dict_free_entries(dict->root);
    free(dict);
}

/* =============================================================================
 * PROGRAM PRINCIPAL
 * =============================================================================
 */

void print_menu(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════╗\n");
    printf("║     DICȚIONAR ROMÂN-ENGLEZ            ║\n");
    printf("╠═══════════════════════════════════════╣\n");
    printf("║  1. Adaugă cuvânt                     ║\n");
    printf("║  2. Caută cuvânt                      ║\n");
    printf("║  3. Șterge cuvânt                     ║\n");
    printf("║  4. Afișează tot dicționarul          ║\n");
    printf("║  5. Salvează în fișier                ║\n");
    printf("║  6. Încarcă din fișier                ║\n");
    printf("║  7. Statistici                        ║\n");
    printf("║  0. Ieșire                            ║\n");
    printf("╚═══════════════════════════════════════╝\n");
    printf("Opțiune: ");
}

int main(void) {
    Dictionary *dict = dict_create();
    if (dict == NULL) {
        return 1;
    }
    
    /* Adăugăm câteva cuvinte inițiale pentru testare */
    printf("Inițializare dicționar cu câteva cuvinte...\n");
    dict_add(dict, "casa", "house");
    dict_add(dict, "masă", "table");
    dict_add(dict, "scaun", "chair");
    dict_add(dict, "carte", "book");
    dict_add(dict, "apă", "water");
    
    int running = 1;
    char word_ro[MAX_WORD_LENGTH];
    char word_en[MAX_WORD_LENGTH];
    char filename[MAX_WORD_LENGTH];
    int option;
    
    while (running) {
        print_menu();
        
        if (scanf("%d", &option) != 1) {
            /* Curățăm input-ul invalid */
            while (getchar() != '\n');
            printf("Opțiune invalidă!\n");
            continue;
        }
        getchar();  /* Consumă newline */
        
        switch (option) {
            case 1:
                printf("Cuvânt română: ");
                if (fgets(word_ro, sizeof(word_ro), stdin) == NULL) break;
                word_ro[strcspn(word_ro, "\n")] = 0;
                
                printf("Traducere engleză: ");
                if (fgets(word_en, sizeof(word_en), stdin) == NULL) break;
                word_en[strcspn(word_en, "\n")] = 0;
                
                dict_add(dict, word_ro, word_en);
                break;
                
            case 2:
                printf("Cuvânt de căutat: ");
                if (fgets(word_ro, sizeof(word_ro), stdin) == NULL) break;
                word_ro[strcspn(word_ro, "\n")] = 0;
                
                dict_lookup(dict, word_ro);
                break;
                
            case 3:
                printf("Cuvânt de șters: ");
                if (fgets(word_ro, sizeof(word_ro), stdin) == NULL) break;
                word_ro[strcspn(word_ro, "\n")] = 0;
                
                dict_remove(dict, word_ro);
                break;
                
            case 4:
                dict_print_all(dict);
                break;
                
            case 5:
                printf("Nume fișier: ");
                if (fgets(filename, sizeof(filename), stdin) == NULL) break;
                filename[strcspn(filename, "\n")] = 0;
                
                dict_save_to_file(dict, filename);
                break;
                
            case 6:
                printf("Nume fișier: ");
                if (fgets(filename, sizeof(filename), stdin) == NULL) break;
                filename[strcspn(filename, "\n")] = 0;
                
                dict_load_from_file(dict, filename);
                break;
                
            case 7:
                dict_stats(dict);
                break;
                
            case 0:
                running = 0;
                printf("La revedere!\n");
                break;
                
            default:
                printf("Opțiune invalidă!\n");
        }
    }
    
    dict_destroy(dict);
    
    return 0;
}

/* =============================================================================
 * PROVOCĂRI BONUS (Opțional)
 * =============================================================================
 *
 * 1. Implementați sugestii de cuvinte similare când căutarea eșuează
 *    (cuvinte care încep cu aceleași litere)
 *
 * 2. Adăugați suport pentru sinonime (un cuvânt poate avea mai multe traduceri)
 *
 * 3. Implementați căutare parțială (găsește toate cuvintele care conțin un substring)
 *
 * 4. Adăugați timestamp pentru fiecare intrare (când a fost adăugată/modificată)
 *
 * 5. Implementați export în format HTML sau JSON
 *
 * =============================================================================
 */
