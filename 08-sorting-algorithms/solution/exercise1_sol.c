/**
 * =============================================================================
 * SOLUȚIE: EXERCIȚIUL 1 - Dicționar Român-Englez cu BST
 * =============================================================================
 *
 * NOTĂ: Acest fișier este doar pentru instructori!
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -o exercise1_sol exercise1_sol.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORD_LENGTH 100

/* =============================================================================
 * DEFINIREA TIPURILOR
 * =============================================================================
 */

typedef struct DictEntry {
    char word_ro[MAX_WORD_LENGTH];
    char word_en[MAX_WORD_LENGTH];
    struct DictEntry *left;
    struct DictEntry *right;
} DictEntry;

typedef struct {
    DictEntry *root;
    size_t size;
} Dictionary;

/* =============================================================================
 * FUNCȚII DE CREARE
 * =============================================================================
 */

DictEntry* dict_create_entry(const char *word_ro, const char *word_en) {
    DictEntry *entry = (DictEntry*)malloc(sizeof(DictEntry));
    if (entry == NULL) {
        fprintf(stderr, "Eroare: Nu s-a putut aloca memorie!\n");
        return NULL;
    }
    
    strncpy(entry->word_ro, word_ro, MAX_WORD_LENGTH - 1);
    entry->word_ro[MAX_WORD_LENGTH - 1] = '\0';
    
    strncpy(entry->word_en, word_en, MAX_WORD_LENGTH - 1);
    entry->word_en[MAX_WORD_LENGTH - 1] = '\0';
    
    entry->left = NULL;
    entry->right = NULL;
    
    return entry;
}

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
 * INSERARE
 * =============================================================================
 */

DictEntry* dict_insert_entry(DictEntry *entry, const char *word_ro, const char *word_en) {
    if (entry == NULL) {
        return dict_create_entry(word_ro, word_en);
    }
    
    int cmp = strcmp(word_ro, entry->word_ro);
    
    if (cmp < 0) {
        entry->left = dict_insert_entry(entry->left, word_ro, word_en);
    } else if (cmp > 0) {
        entry->right = dict_insert_entry(entry->right, word_ro, word_en);
    } else {
        /* Cuvânt existent - actualizăm traducerea */
        strncpy(entry->word_en, word_en, MAX_WORD_LENGTH - 1);
        entry->word_en[MAX_WORD_LENGTH - 1] = '\0';
    }
    
    return entry;
}

void dict_add(Dictionary *dict, const char *word_ro, const char *word_en) {
    if (dict == NULL || word_ro == NULL || word_en == NULL) return;
    
    /* Verificăm dacă e duplicat pentru a nu incrementa size */
    DictEntry *existing = NULL;
    DictEntry *current = dict->root;
    while (current != NULL) {
        int cmp = strcmp(word_ro, current->word_ro);
        if (cmp == 0) {
            existing = current;
            break;
        } else if (cmp < 0) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    
    dict->root = dict_insert_entry(dict->root, word_ro, word_en);
    
    if (existing == NULL) {
        dict->size++;
    }
    
    printf("Adăugat: %s -> %s\n", word_ro, word_en);
}

/* =============================================================================
 * CĂUTARE
 * =============================================================================
 */

DictEntry* dict_search(DictEntry *entry, const char *word_ro) {
    if (entry == NULL) {
        return NULL;
    }
    
    int cmp = strcmp(word_ro, entry->word_ro);
    
    if (cmp == 0) {
        return entry;
    } else if (cmp < 0) {
        return dict_search(entry->left, word_ro);
    } else {
        return dict_search(entry->right, word_ro);
    }
}

/* Găsește cuvinte care încep cu un prefix (pentru sugestii) */
void dict_find_by_prefix(DictEntry *entry, const char *prefix, int *count, int max_suggestions) {
    if (entry == NULL || *count >= max_suggestions) return;
    
    /* Verificăm dacă cuvântul curent începe cu prefixul */
    if (strncmp(entry->word_ro, prefix, strlen(prefix)) == 0) {
        printf("  Sugestie: %s -> %s\n", entry->word_ro, entry->word_en);
        (*count)++;
    }
    
    /* Continuăm în subarborii relevanți */
    int cmp = strncmp(prefix, entry->word_ro, strlen(prefix));
    
    if (cmp <= 0) {
        dict_find_by_prefix(entry->left, prefix, count, max_suggestions);
    }
    if (cmp >= 0 && *count < max_suggestions) {
        dict_find_by_prefix(entry->right, prefix, count, max_suggestions);
    }
}

void dict_lookup(Dictionary *dict, const char *word_ro) {
    if (dict == NULL || word_ro == NULL) return;
    
    DictEntry *result = dict_search(dict->root, word_ro);
    
    if (result != NULL) {
        printf("Găsit: %s -> %s\n", result->word_ro, result->word_en);
    } else {
        printf("Negăsit: '%s' nu există în dicționar.\n", word_ro);
        
        /* Sugerăm cuvinte similare dacă avem un prefix de cel puțin 2 caractere */
        if (strlen(word_ro) >= 2) {
            char prefix[3];
            strncpy(prefix, word_ro, 2);
            prefix[2] = '\0';
            
            int count = 0;
            printf("Cuvinte care încep cu '%s':\n", prefix);
            dict_find_by_prefix(dict->root, prefix, &count, 5);
            
            if (count == 0) {
                printf("  (nicio sugestie disponibilă)\n");
            }
        }
    }
}

/* =============================================================================
 * ȘTERGERE
 * =============================================================================
 */

DictEntry* dict_find_min(DictEntry *entry) {
    if (entry == NULL) return NULL;
    while (entry->left != NULL) {
        entry = entry->left;
    }
    return entry;
}

DictEntry* dict_find_max(DictEntry *entry) {
    if (entry == NULL) return NULL;
    while (entry->right != NULL) {
        entry = entry->right;
    }
    return entry;
}

DictEntry* dict_delete_entry(DictEntry *entry, const char *word_ro) {
    if (entry == NULL) return NULL;
    
    int cmp = strcmp(word_ro, entry->word_ro);
    
    if (cmp < 0) {
        entry->left = dict_delete_entry(entry->left, word_ro);
    } else if (cmp > 0) {
        entry->right = dict_delete_entry(entry->right, word_ro);
    } else {
        /* Am găsit nodul de șters */
        
        /* Cazul 1 & 2: 0 sau 1 copil */
        if (entry->left == NULL) {
            DictEntry *temp = entry->right;
            free(entry);
            return temp;
        }
        if (entry->right == NULL) {
            DictEntry *temp = entry->left;
            free(entry);
            return temp;
        }
        
        /* Cazul 3: 2 copii */
        DictEntry *successor = dict_find_min(entry->right);
        strcpy(entry->word_ro, successor->word_ro);
        strcpy(entry->word_en, successor->word_en);
        entry->right = dict_delete_entry(entry->right, successor->word_ro);
    }
    
    return entry;
}

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
 * PARCURGERE ȘI AFIȘARE
 * =============================================================================
 */

void dict_print_inorder(DictEntry *entry) {
    if (entry == NULL) return;
    
    dict_print_inorder(entry->left);
    printf("  %s -> %s\n", entry->word_ro, entry->word_en);
    dict_print_inorder(entry->right);
}

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
 * FIȘIERE
 * =============================================================================
 */

void dict_save_entries(DictEntry *entry, FILE *file) {
    if (entry == NULL || file == NULL) return;
    
    dict_save_entries(entry->left, file);
    fprintf(file, "%s:%s\n", entry->word_ro, entry->word_en);
    dict_save_entries(entry->right, file);
}

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
        /* Eliminăm newline */
        line[strcspn(line, "\n")] = '\0';
        
        /* Găsim separatorul */
        char *separator = strchr(line, ':');
        if (separator == NULL) continue;
        
        *separator = '\0';
        char *word_ro = line;
        char *word_en = separator + 1;
        
        /* Eliminăm spații la început și sfârșit */
        while (*word_ro && isspace(*word_ro)) word_ro++;
        while (*word_en && isspace(*word_en)) word_en++;
        
        if (strlen(word_ro) > 0 && strlen(word_en) > 0) {
            dict->root = dict_insert_entry(dict->root, word_ro, word_en);
            dict->size++;
            count++;
        }
    }
    
    fclose(file);
    printf("Încărcate %d intrări din '%s'.\n", count, filename);
    return 0;
}

/* =============================================================================
 * STATISTICI
 * =============================================================================
 */

int dict_height(DictEntry *entry) {
    if (entry == NULL) {
        return -1;
    }
    
    int left_h = dict_height(entry->left);
    int right_h = dict_height(entry->right);
    
    return 1 + (left_h > right_h ? left_h : right_h);
}

void dict_stats(Dictionary *dict) {
    if (dict == NULL) return;
    
    printf("\n=== Statistici Dicționar ===\n");
    printf("Număr intrări: %zu\n", dict->size);
    printf("Înălțime arbore: %d\n", dict_height(dict->root));
    
    if (dict->root != NULL) {
        DictEntry *min = dict_find_min(dict->root);
        DictEntry *max = dict_find_max(dict->root);
        printf("Primul cuvânt (alfabetic): %s\n", min ? min->word_ro : "N/A");
        printf("Ultimul cuvânt (alfabetic): %s\n", max ? max->word_ro : "N/A");
    }
    printf("============================\n");
}

/* =============================================================================
 * ELIBERARE MEMORIE
 * =============================================================================
 */

void dict_free_entries(DictEntry *entry) {
    if (entry == NULL) return;
    
    dict_free_entries(entry->left);
    dict_free_entries(entry->right);
    free(entry);
}

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
    printf("║     DICȚIONAR ROMÂN-ENGLEZ (SOLUȚIE)  ║\n");
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
    if (dict == NULL) return 1;
    
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
            while (getchar() != '\n');
            printf("Opțiune invalidă!\n");
            continue;
        }
        getchar();
        
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
