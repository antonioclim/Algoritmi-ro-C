/**
 * =============================================================================
 * SOLUȚIE: TEMA 2 - Indexare și Căutare Text
 * =============================================================================
 *
 * NOTĂ: Acest fișier este doar pentru instructori!
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -o homework2_sol homework2_sol.c
 * Utilizare: ./homework2_sol document.txt
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORD_LEN 100

/* Lista de poziții unde apare un cuvânt */
typedef struct Position {
    int line;
    int column;
    struct Position *next;
} Position;

/* Nod în arborele de index */
typedef struct WordIndex {
    char word[MAX_WORD_LEN];
    int count;
    Position *positions;
    struct WordIndex *left;
    struct WordIndex *right;
} WordIndex;

/* Structura principală pentru index */
typedef struct {
    WordIndex *root;
    int unique_words;
    int total_words;
} TextIndex;

/* =============================================================================
 * FUNCȚII PENTRU POZIȚII
 * =============================================================================
 */

Position* create_position(int line, int column) {
    Position *pos = (Position*)malloc(sizeof(Position));
    if (pos) {
        pos->line = line;
        pos->column = column;
        pos->next = NULL;
    }
    return pos;
}

void add_position(WordIndex *word_node, int line, int column) {
    Position *new_pos = create_position(line, column);
    if (!new_pos) return;
    
    /* Adăugăm la sfârșitul listei */
    if (word_node->positions == NULL) {
        word_node->positions = new_pos;
    } else {
        Position *curr = word_node->positions;
        while (curr->next) curr = curr->next;
        curr->next = new_pos;
    }
    word_node->count++;
}

void free_positions(Position *pos) {
    while (pos) {
        Position *next = pos->next;
        free(pos);
        pos = next;
    }
}

/* =============================================================================
 * FUNCȚII PENTRU INDEX
 * =============================================================================
 */

WordIndex* create_word_node(const char *word, int line, int column) {
    WordIndex *node = (WordIndex*)malloc(sizeof(WordIndex));
    if (node) {
        strncpy(node->word, word, MAX_WORD_LEN - 1);
        node->word[MAX_WORD_LEN - 1] = '\0';
        node->count = 1;
        node->positions = create_position(line, column);
        node->left = node->right = NULL;
    }
    return node;
}

TextIndex* create_index(void) {
    TextIndex *idx = (TextIndex*)malloc(sizeof(TextIndex));
    if (idx) {
        idx->root = NULL;
        idx->unique_words = 0;
        idx->total_words = 0;
    }
    return idx;
}

WordIndex* insert_word(WordIndex *node, const char *word, int line, int column, int *is_new) {
    if (node == NULL) {
        *is_new = 1;
        return create_word_node(word, line, column);
    }
    
    int cmp = strcmp(word, node->word);
    
    if (cmp < 0) {
        node->left = insert_word(node->left, word, line, column, is_new);
    } else if (cmp > 0) {
        node->right = insert_word(node->right, word, line, column, is_new);
    } else {
        /* Cuvânt existent - adăugăm poziția */
        add_position(node, line, column);
        *is_new = 0;
    }
    
    return node;
}

void index_add_word(TextIndex *idx, const char *word, int line, int column) {
    int is_new = 0;
    idx->root = insert_word(idx->root, word, line, column, &is_new);
    idx->total_words++;
    if (is_new) idx->unique_words++;
}

/* =============================================================================
 * CĂUTARE
 * =============================================================================
 */

WordIndex* search_word(WordIndex *node, const char *word) {
    if (node == NULL) return NULL;
    
    int cmp = strcmp(word, node->word);
    
    if (cmp == 0) return node;
    if (cmp < 0) return search_word(node->left, word);
    return search_word(node->right, word);
}

void print_word_info(WordIndex *node) {
    if (!node) {
        printf("Cuvânt negăsit.\n");
        return;
    }
    
    printf("Cuvânt: \"%s\"\n", node->word);
    printf("Apariții: %d\n", node->count);
    printf("Poziții: ");
    
    Position *pos = node->positions;
    int shown = 0;
    while (pos && shown < 10) {
        printf("(%d:%d)", pos->line, pos->column);
        if (pos->next && shown < 9) printf(", ");
        pos = pos->next;
        shown++;
    }
    if (node->count > 10) printf("...");
    printf("\n");
}

/* =============================================================================
 * CĂUTARE DUPĂ PREFIX
 * =============================================================================
 */

void find_by_prefix(WordIndex *node, const char *prefix, int *count, int max_results) {
    if (node == NULL || *count >= max_results) return;
    
    int cmp = strncmp(prefix, node->word, strlen(prefix));
    
    /* Căutăm în stânga dacă prefixul ar putea fi acolo */
    if (cmp <= 0) {
        find_by_prefix(node->left, prefix, count, max_results);
    }
    
    /* Verificăm nodul curent */
    if (strncmp(node->word, prefix, strlen(prefix)) == 0 && *count < max_results) {
        printf("  %s (%d apariții)\n", node->word, node->count);
        (*count)++;
    }
    
    /* Căutăm în dreapta */
    if (cmp >= 0 && *count < max_results) {
        find_by_prefix(node->right, prefix, count, max_results);
    }
}

/* =============================================================================
 * TOP CUVINTE
 * =============================================================================
 */

typedef struct {
    char word[MAX_WORD_LEN];
    int count;
} WordCount;

void collect_words(WordIndex *node, WordCount *arr, int *idx, int max_size) {
    if (node == NULL || *idx >= max_size) return;
    
    collect_words(node->left, arr, idx, max_size);
    
    if (*idx < max_size) {
        strcpy(arr[*idx].word, node->word);
        arr[*idx].count = node->count;
        (*idx)++;
    }
    
    collect_words(node->right, arr, idx, max_size);
}

int compare_word_count(const void *a, const void *b) {
    return ((WordCount*)b)->count - ((WordCount*)a)->count;
}

void print_top_words(TextIndex *idx, int n) {
    if (!idx || !idx->root) return;
    
    int max_words = idx->unique_words < 1000 ? idx->unique_words : 1000;
    WordCount *arr = (WordCount*)malloc(max_words * sizeof(WordCount));
    if (!arr) return;
    
    int count = 0;
    collect_words(idx->root, arr, &count, max_words);
    
    qsort(arr, count, sizeof(WordCount), compare_word_count);
    
    printf("\nTop %d cuvinte:\n", n);
    for (int i = 0; i < n && i < count; i++) {
        printf("  %d. \"%s\" - %d apariții\n", i + 1, arr[i].word, arr[i].count);
    }
    
    free(arr);
}

/* =============================================================================
 * STATISTICI
 * =============================================================================
 */

int tree_height(WordIndex *node) {
    if (node == NULL) return -1;
    int lh = tree_height(node->left);
    int rh = tree_height(node->right);
    return 1 + (lh > rh ? lh : rh);
}

void find_longest(WordIndex *node, char *longest) {
    if (node == NULL) return;
    find_longest(node->left, longest);
    if (strlen(node->word) > strlen(longest)) {
        strcpy(longest, node->word);
    }
    find_longest(node->right, longest);
}

void print_stats(TextIndex *idx) {
    printf("\n=== STATISTICI INDEX ===\n");
    printf("Cuvinte unice: %d\n", idx->unique_words);
    printf("Cuvinte totale: %d\n", idx->total_words);
    printf("Înălțime arbore: %d\n", tree_height(idx->root));
    
    char longest[MAX_WORD_LEN] = "";
    find_longest(idx->root, longest);
    printf("Cel mai lung cuvânt: \"%s\" (%zu caractere)\n", longest, strlen(longest));
}

/* =============================================================================
 * CONSTRUIRE INDEX DIN FIȘIER
 * =============================================================================
 */

void normalize_word(char *word) {
    /* Convertim la lowercase */
    for (int i = 0; word[i]; i++) {
        word[i] = tolower((unsigned char)word[i]);
    }
}

int is_word_char(char c) {
    return isalnum((unsigned char)c) || c == '-' || c == '\'';
}

void build_index_from_file(TextIndex *idx, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Eroare: Nu s-a putut deschide '%s'\n", filename);
        return;
    }
    
    char line[1024];
    int line_num = 1;
    
    while (fgets(line, sizeof(line), file)) {
        int col = 1;
        int i = 0;
        
        while (line[i]) {
            /* Sărim peste non-cuvinte */
            while (line[i] && !is_word_char(line[i])) {
                i++;
                col++;
            }
            
            if (!line[i]) break;
            
            /* Extragem cuvântul */
            char word[MAX_WORD_LEN];
            int word_start_col = col;
            int j = 0;
            
            while (line[i] && is_word_char(line[i]) && j < MAX_WORD_LEN - 1) {
                word[j++] = line[i++];
                col++;
            }
            word[j] = '\0';
            
            /* Adăugăm în index dacă e valid */
            if (j >= 2) {  /* Ignorăm cuvinte de 1 caracter */
                normalize_word(word);
                index_add_word(idx, word, line_num, word_start_col);
            }
        }
        
        line_num++;
    }
    
    fclose(file);
    printf("Index construit din '%s'\n", filename);
}

/* =============================================================================
 * ELIBERARE MEMORIE
 * =============================================================================
 */

void free_index_tree(WordIndex *node) {
    if (node == NULL) return;
    free_index_tree(node->left);
    free_index_tree(node->right);
    free_positions(node->positions);
    free(node);
}

void destroy_index(TextIndex *idx) {
    if (idx) {
        free_index_tree(idx->root);
        free(idx);
    }
}

/* =============================================================================
 * MAIN
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    printf("\n=== SISTEM DE INDEXARE TEXT (SOLUȚIE) ===\n\n");
    
    TextIndex *idx = create_index();
    if (!idx) return 1;
    
    if (argc > 1) {
        build_index_from_file(idx, argv[1]);
    } else {
        /* Date de test dacă nu se dă fișier */
        printf("Folosire: %s <fisier.txt>\n", argv[0]);
        printf("Demonstrație cu text de test...\n\n");
        
        index_add_word(idx, "algoritm", 1, 1);
        index_add_word(idx, "arbore", 1, 10);
        index_add_word(idx, "binar", 1, 17);
        index_add_word(idx, "căutare", 2, 1);
        index_add_word(idx, "algoritm", 2, 10);
        index_add_word(idx, "arbore", 3, 5);
        index_add_word(idx, "algoritm", 4, 1);
        index_add_word(idx, "program", 5, 1);
        index_add_word(idx, "programare", 5, 10);
        index_add_word(idx, "proiect", 6, 1);
    }
    
    print_stats(idx);
    print_top_words(idx, 5);
    
    printf("\n=== CĂUTARE ===\n");
    printf("Căutare 'algoritm':\n");
    WordIndex *found = search_word(idx->root, "algoritm");
    print_word_info(found);
    
    printf("\n=== CĂUTARE DUPĂ PREFIX 'pro' ===\n");
    int count = 0;
    find_by_prefix(idx->root, "pro", &count, 10);
    if (count == 0) printf("  (niciun rezultat)\n");
    
    destroy_index(idx);
    printf("\nProgram terminat.\n");
    
    return 0;
}
