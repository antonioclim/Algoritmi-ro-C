#define _POSIX_C_SOURCE 200809L
/**
 * =============================================================================
 * SOLUȚIE TEMA 1: DICȚIONAR DE CUVINTE CU FRECVENȚĂ
 * =============================================================================
 *
 * DOAR PENTRU INSTRUCTOR - NU DISTRIBUI STUDENȚILOR
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -g -o homework1_sol homework1_sol.c
 * Utilizare: ./homework1_sol <fisier.txt>
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define INITIAL_SIZE      1009   /* Număr prim */
#define MAX_LOAD_FACTOR   2.0
#define MAX_WORD_LENGTH   128

/* =============================================================================
 * STRUCTURI DE DATE
 * =============================================================================
 */

typedef struct WordNode {
    char *word;
    int count;
    struct WordNode *next;
} WordNode;

typedef struct {
    WordNode **buckets;
    size_t size;
    size_t count;
    size_t total_words;  /* Total cuvinte cu repetiții */
} WordTable;

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
 * FUNCȚII WORD TABLE
 * =============================================================================
 */

WordTable* wt_create(size_t size) {
    WordTable *wt = malloc(sizeof(WordTable));
    if (!wt) return NULL;
    
    wt->buckets = calloc(size, sizeof(WordNode*));
    if (!wt->buckets) {
        free(wt);
        return NULL;
    }
    
    wt->size = size;
    wt->count = 0;
    wt->total_words = 0;
    return wt;
}

double wt_load_factor(WordTable *wt) {
    return (double)wt->count / wt->size;
}

void wt_add_word(WordTable *wt, const char *word);

void wt_resize(WordTable *wt, size_t new_size) {
    WordNode **old = wt->buckets;
    size_t old_size = wt->size;
    
    wt->buckets = calloc(new_size, sizeof(WordNode*));
    if (!wt->buckets) {
        wt->buckets = old;
        return;
    }
    
    size_t old_total = wt->total_words;
    wt->size = new_size;
    wt->count = 0;
    wt->total_words = 0;
    
    for (size_t i = 0; i < old_size; i++) {
        WordNode *node = old[i];
        while (node) {
            WordNode *next = node->next;
            
            /* Reinserează cu count-ul existent */
            size_t idx = hash_djb2(node->word) % new_size;
            node->next = wt->buckets[idx];
            wt->buckets[idx] = node;
            wt->count++;
            wt->total_words += node->count;
            
            node = next;
        }
    }
    
    free(old);
    (void)old_total;
}

void wt_add_word(WordTable *wt, const char *word) {
    if (!wt || !word || strlen(word) == 0) return;
    
    size_t index = hash_djb2(word) % wt->size;
    
    /* Caută cuvântul existent */
    WordNode *node = wt->buckets[index];
    while (node) {
        if (strcmp(node->word, word) == 0) {
            node->count++;
            wt->total_words++;
            return;
        }
        node = node->next;
    }
    
    /* Cuvânt nou */
    WordNode *new_node = malloc(sizeof(WordNode));
    if (!new_node) return;
    
    new_node->word = strdup(word);
    new_node->count = 1;
    new_node->next = wt->buckets[index];
    wt->buckets[index] = new_node;
    wt->count++;
    wt->total_words++;
    
    if (wt_load_factor(wt) > MAX_LOAD_FACTOR) {
        wt_resize(wt, wt->size * 2 + 1);
    }
}

void wt_destroy(WordTable *wt) {
    if (!wt) return;
    
    for (size_t i = 0; i < wt->size; i++) {
        WordNode *node = wt->buckets[i];
        while (node) {
            WordNode *next = node->next;
            free(node->word);
            free(node);
            node = next;
        }
    }
    
    free(wt->buckets);
    free(wt);
}

/* =============================================================================
 * FUNCȚII PENTRU TOP N CUVINTE
 * =============================================================================
 */

typedef struct {
    char *word;
    int count;
} WordCount;

int compare_word_count(const void *a, const void *b) {
    const WordCount *wa = (const WordCount*)a;
    const WordCount *wb = (const WordCount*)b;
    return wb->count - wa->count;  /* Descrescător */
}

void wt_print_top_n(WordTable *wt, int n) {
    if (!wt || n <= 0) return;
    
    /* Colectează toate cuvintele */
    WordCount *words = malloc(wt->count * sizeof(WordCount));
    if (!words) return;
    
    size_t idx = 0;
    for (size_t i = 0; i < wt->size; i++) {
        WordNode *node = wt->buckets[i];
        while (node) {
            words[idx].word = node->word;
            words[idx].count = node->count;
            idx++;
            node = node->next;
        }
    }
    
    /* Sortează descrescător */
    qsort(words, wt->count, sizeof(WordCount), compare_word_count);
    
    /* Afișează top N */
    printf("\n=== Top %d cele mai frecvente cuvinte ===\n", n);
    int limit = (int)wt->count < n ? (int)wt->count : n;
    for (int i = 0; i < limit; i++) {
        printf("%d. %s: %d\n", i + 1, words[i].word, words[i].count);
    }
    
    free(words);
}

/* =============================================================================
 * FUNCȚII PENTRU PROCESARE TEXT
 * =============================================================================
 */

/**
 * Convertește un caracter la lowercase
 * Tratează și caracterele românești (basic support)
 */
char to_lower_ro(char c) {
    if (c >= 'A' && c <= 'Z') {
        return c + 32;
    }
    return c;
}

/**
 * Verifică dacă un caracter face parte dintr-un cuvânt
 */
bool is_word_char(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           (c >= '0' && c <= '9') ||
           (unsigned char)c > 127;  /* Caractere românești */
}

/**
 * Extrage și normalizează un cuvânt din buffer
 */
void normalize_word(char *word) {
    if (!word) return;
    
    char *src = word;
    char *dst = word;
    
    while (*src) {
        *dst = to_lower_ro(*src);
        src++;
        dst++;
    }
    *dst = '\0';
}

/**
 * Procesează un fișier text
 */
void process_file(WordTable *wt, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Eroare: Nu s-a putut deschide fișierul '%s'\n", filename);
        return;
    }
    
    char word[MAX_WORD_LENGTH];
    int word_len = 0;
    int c;
    
    while ((c = fgetc(file)) != EOF) {
        if (is_word_char((char)c)) {
            if (word_len < MAX_WORD_LENGTH - 1) {
                word[word_len++] = (char)c;
            }
        } else {
            if (word_len > 0) {
                word[word_len] = '\0';
                normalize_word(word);
                if (strlen(word) >= 2) {  /* Ignoră cuvinte de 1 caracter */
                    wt_add_word(wt, word);
                }
                word_len = 0;
            }
        }
    }
    
    /* Procesează ultimul cuvânt dacă există */
    if (word_len > 0) {
        word[word_len] = '\0';
        normalize_word(word);
        if (strlen(word) >= 2) {
            wt_add_word(wt, word);
        }
    }
    
    fclose(file);
}

/* =============================================================================
 * FUNCȚIA PRINCIPALĂ
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     TEMA 1: DICȚIONAR DE CUVINTE CU FRECVENȚĂ                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    if (argc < 2) {
        /* Mod interactiv pentru testare */
        printf("Utilizare: %s <fisier.txt>\n", argv[0]);
        printf("\nMod demo cu text hardcodat:\n\n");
        
        WordTable *wt = wt_create(INITIAL_SIZE);
        if (!wt) {
            fprintf(stderr, "Eroare: Nu s-a putut crea tabela\n");
            return 1;
        }
        
        /* Text de test */
        const char *demo_words[] = {
            "algoritm", "structura", "date", "hash", "table",
            "algoritm", "implementare", "c", "programare",
            "hash", "functie", "coliziune", "algoritm",
            "structura", "hash", "table", "date", "algoritm"
        };
        
        int num_words = sizeof(demo_words) / sizeof(demo_words[0]);
        for (int i = 0; i < num_words; i++) {
            wt_add_word(wt, demo_words[i]);
        }
        
        wt_print_top_n(wt, 5);
        
        printf("\n=== Statistici ===\n");
        printf("Cuvinte unice: %zu\n", wt->count);
        printf("Total cuvinte: %zu\n", wt->total_words);
        printf("Load factor: %.2f\n", wt_load_factor(wt));
        
        wt_destroy(wt);
        return 0;
    }
    
    /* Procesare fișier */
    WordTable *wt = wt_create(INITIAL_SIZE);
    if (!wt) {
        fprintf(stderr, "Eroare: Nu s-a putut crea tabela\n");
        return 1;
    }
    
    printf("Procesare fișier: %s\n", argv[1]);
    process_file(wt, argv[1]);
    
    if (wt->count == 0) {
        printf("Nu s-au găsit cuvinte în fișier.\n");
        wt_destroy(wt);
        return 0;
    }
    
    int n;
    printf("Introdu N (top cuvinte): ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        n = 10;
    }
    
    wt_print_top_n(wt, n);
    
    printf("\n=== Statistici ===\n");
    printf("Cuvinte unice: %zu\n", wt->count);
    printf("Total cuvinte: %zu\n", wt->total_words);
    printf("Load factor: %.2f\n", wt_load_factor(wt));
    
    wt_destroy(wt);
    printf("\n[INFO] Memorie eliberată.\n");
    
    return 0;
}
