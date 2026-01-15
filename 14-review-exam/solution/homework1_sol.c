/**
 * =============================================================================
 * TEMA 1: SISTEM DE GESTIUNE BIBLIOTECĂ - SOLUȚIE COMPLETĂ
 * Săptămâna 14: Recapitulare Avansată și Optimizare
 * =============================================================================
 *
 * Această soluție demonstrează integrarea tuturor structurilor de date
 * studiate în semestru:
 *   1. Hash Table pentru indexarea cărților după ISBN
 *   2. AVL Tree pentru gestionarea cititorilor cu echilibrare automată
 *   3. Priority Queue (Min-Heap) pentru gestionarea rezervărilor
 *   4. Linked Lists pentru împrumuturi și chain-uri în hash table
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -o hw1 homework1_sol.c -lm
 * Utilizare: ./hw1
 *
 * Autor: Soluție de referință pentru evaluare
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

/* =============================================================================
 * CONSTANTE ȘI DEFINIȚII
 * =============================================================================
 */

#define ISBN_LENGTH 14          /* 13 cifre + null terminator */
#define MAX_TITLE 128
#define MAX_AUTHOR 64
#define MAX_NAME 64
#define MAX_FACULTY 32
#define HASH_TABLE_SIZE 101     /* Număr prim pentru distribuție uniformă */
#define MAX_LOANS_PER_READER 5
#define LOAN_DURATION_DAYS 14

/* Coduri de culoare ANSI pentru output */
#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[0;32m"
#define COLOR_RED     "\033[0;31m"
#define COLOR_YELLOW  "\033[0;33m"
#define COLOR_CYAN    "\033[0;36m"
#define COLOR_MAGENTA "\033[0;35m"

/* =============================================================================
 * STRUCTURI DE DATE
 * =============================================================================
 */

/* --- Structura pentru o carte --- */
typedef struct Book {
    char isbn[ISBN_LENGTH];
    char title[MAX_TITLE];
    char author[MAX_AUTHOR];
    int year;
    int total_copies;           /* Număr total de exemplare */
    int available_copies;       /* Exemplare disponibile pentru împrumut */
    int borrow_count;           /* Statistică: de câte ori a fost împrumutată */
    struct Book *next;          /* Pentru chaining în hash table */
} Book;

/* --- Hash Table pentru cărți --- */
typedef struct {
    Book *buckets[HASH_TABLE_SIZE];
    int count;                  /* Număr total de cărți distincte */
} BookHashTable;

/* --- Structura pentru un împrumut --- */
typedef struct Loan {
    char isbn[ISBN_LENGTH];
    time_t borrow_date;
    time_t due_date;
    struct Loan *next;
} Loan;

/* --- Nod AVL pentru cititori --- */
typedef struct ReaderNode {
    int reader_id;
    char name[MAX_NAME];
    char faculty[MAX_FACULTY];
    Loan *loans;                /* Lista de împrumuturi active */
    int loan_count;
    int height;                 /* Înălțimea pentru echilibrare AVL */
    struct ReaderNode *left;
    struct ReaderNode *right;
} ReaderNode;

/* --- AVL Tree pentru cititori --- */
typedef struct {
    ReaderNode *root;
    int count;
} ReaderAVLTree;

/* --- Structura pentru o rezervare --- */
typedef struct {
    int reader_id;
    char isbn[ISBN_LENGTH];
    time_t reservation_date;
} Reservation;

/* --- Priority Queue (Min-Heap) pentru rezervări --- */
typedef struct {
    Reservation *heap;
    int capacity;
    int size;
} ReservationHeap;

/* --- Structura principală a bibliotecii --- */
typedef struct {
    BookHashTable *books;
    ReaderAVLTree *readers;
    ReservationHeap *reservations;
} Library;

/* =============================================================================
 * FUNCȚII HASH TABLE PENTRU CĂRȚI
 * =============================================================================
 */

/**
 * Funcția hash djb2 - algoritm clasic pentru stringuri
 * 
 * Complexitate: O(n) unde n = lungimea stringului
 */
static unsigned int hash_isbn(const char *isbn) {
    unsigned int hash = 5381;
    int c;
    while ((c = *isbn++)) {
        hash = ((hash << 5) + hash) + c;  /* hash * 33 + c */
    }
    return hash % HASH_TABLE_SIZE;
}

/**
 * Creează un hash table gol pentru cărți
 */
static BookHashTable *create_book_table(void) {
    BookHashTable *table = malloc(sizeof(BookHashTable));
    if (!table) return NULL;
    
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        table->buckets[i] = NULL;
    }
    table->count = 0;
    return table;
}

/**
 * Caută o carte după ISBN în hash table
 * 
 * Complexitate: O(1) în medie, O(n) în cel mai rău caz
 */
static Book *find_book_in_table(BookHashTable *table, const char *isbn) {
    if (!table || !isbn) return NULL;
    
    unsigned int index = hash_isbn(isbn);
    Book *current = table->buckets[index];
    
    while (current) {
        if (strcmp(current->isbn, isbn) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

/**
 * Inserează o carte în hash table
 * 
 * Folosește chaining pentru coliziuni
 */
static bool insert_book_in_table(BookHashTable *table, Book *book) {
    if (!table || !book) return false;
    
    /* Verifică dacă există deja */
    if (find_book_in_table(table, book->isbn)) {
        return false;
    }
    
    unsigned int index = hash_isbn(book->isbn);
    
    /* Inserare la începutul lanțului */
    book->next = table->buckets[index];
    table->buckets[index] = book;
    table->count++;
    
    return true;
}

/**
 * Șterge o carte din hash table
 * 
 * Returnează pointer-ul la carte (apelantul trebuie să elibereze memoria)
 */
static Book *remove_book_from_table(BookHashTable *table, const char *isbn) {
    if (!table || !isbn) return NULL;
    
    unsigned int index = hash_isbn(isbn);
    Book *current = table->buckets[index];
    Book *prev = NULL;
    
    while (current) {
        if (strcmp(current->isbn, isbn) == 0) {
            if (prev) {
                prev->next = current->next;
            } else {
                table->buckets[index] = current->next;
            }
            current->next = NULL;
            table->count--;
            return current;
        }
        prev = current;
        current = current->next;
    }
    return NULL;
}

/**
 * Eliberează memoria hash table-ului
 */
static void free_book_table(BookHashTable *table) {
    if (!table) return;
    
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        Book *current = table->buckets[i];
        while (current) {
            Book *next = current->next;
            free(current);
            current = next;
        }
    }
    free(table);
}

/* =============================================================================
 * FUNCȚII AVL TREE PENTRU CITITORI
 * =============================================================================
 */

/**
 * Returnează înălțimea unui nod (sau 0 dacă NULL)
 */
static int avl_height(ReaderNode *node) {
    return node ? node->height : 0;
}

/**
 * Calculează factorul de echilibru
 * 
 * balance_factor = height(left) - height(right)
 * Trebuie să fie în [-1, 0, 1] pentru arbore AVL valid
 */
static int avl_balance_factor(ReaderNode *node) {
    return node ? avl_height(node->left) - avl_height(node->right) : 0;
}

/**
 * Actualizează înălțimea unui nod
 */
static void avl_update_height(ReaderNode *node) {
    if (node) {
        int left_h = avl_height(node->left);
        int right_h = avl_height(node->right);
        node->height = (left_h > right_h ? left_h : right_h) + 1;
    }
}

/**
 * Rotație la dreapta (pentru dezechilibru stânga-stânga)
 * 
 *       y                x
 *      / \              / \
 *     x   C    =>      A   y
 *    / \                  / \
 *   A   B                B   C
 */
static ReaderNode *avl_rotate_right(ReaderNode *y) {
    ReaderNode *x = y->left;
    ReaderNode *B = x->right;
    
    x->right = y;
    y->left = B;
    
    avl_update_height(y);
    avl_update_height(x);
    
    return x;
}

/**
 * Rotație la stânga (pentru dezechilibru dreapta-dreapta)
 * 
 *     x                  y
 *    / \                / \
 *   A   y      =>      x   C
 *      / \            / \
 *     B   C          A   B
 */
static ReaderNode *avl_rotate_left(ReaderNode *x) {
    ReaderNode *y = x->right;
    ReaderNode *B = y->left;
    
    y->left = x;
    x->right = B;
    
    avl_update_height(x);
    avl_update_height(y);
    
    return y;
}

/**
 * Reechilibrează un nod AVL dacă este necesar
 * 
 * Cazuri posibile:
 *   - LL (Left-Left): rotație dreapta
 *   - RR (Right-Right): rotație stânga
 *   - LR (Left-Right): rotație stânga pe copil, apoi dreapta
 *   - RL (Right-Left): rotație dreapta pe copil, apoi stânga
 */
static ReaderNode *avl_rebalance(ReaderNode *node) {
    if (!node) return NULL;
    
    avl_update_height(node);
    int balance = avl_balance_factor(node);
    
    /* Caz LL: dezechilibru spre stânga */
    if (balance > 1 && avl_balance_factor(node->left) >= 0) {
        return avl_rotate_right(node);
    }
    
    /* Caz LR: dezechilibru stânga-dreapta */
    if (balance > 1 && avl_balance_factor(node->left) < 0) {
        node->left = avl_rotate_left(node->left);
        return avl_rotate_right(node);
    }
    
    /* Caz RR: dezechilibru spre dreapta */
    if (balance < -1 && avl_balance_factor(node->right) <= 0) {
        return avl_rotate_left(node);
    }
    
    /* Caz RL: dezechilibru dreapta-stânga */
    if (balance < -1 && avl_balance_factor(node->right) > 0) {
        node->right = avl_rotate_right(node->right);
        return avl_rotate_left(node);
    }
    
    return node;
}

/**
 * Creează un nou nod pentru cititor
 */
static ReaderNode *create_reader_node(int reader_id, const char *name, 
                                       const char *faculty) {
    ReaderNode *node = malloc(sizeof(ReaderNode));
    if (!node) return NULL;
    
    node->reader_id = reader_id;
    strncpy(node->name, name, MAX_NAME - 1);
    node->name[MAX_NAME - 1] = '\0';
    strncpy(node->faculty, faculty, MAX_FACULTY - 1);
    node->faculty[MAX_FACULTY - 1] = '\0';
    node->loans = NULL;
    node->loan_count = 0;
    node->height = 1;
    node->left = NULL;
    node->right = NULL;
    
    return node;
}

/**
 * Inserare în AVL Tree cu reechilibrare
 * 
 * Complexitate: O(log n)
 */
static ReaderNode *avl_insert(ReaderNode *node, int reader_id, 
                               const char *name, const char *faculty,
                               bool *success) {
    /* Caz de bază: pozitie găsită */
    if (!node) {
        ReaderNode *new_node = create_reader_node(reader_id, name, faculty);
        if (new_node) *success = true;
        return new_node;
    }
    
    /* Navigare recursivă */
    if (reader_id < node->reader_id) {
        node->left = avl_insert(node->left, reader_id, name, faculty, success);
    } else if (reader_id > node->reader_id) {
        node->right = avl_insert(node->right, reader_id, name, faculty, success);
    } else {
        /* ID deja există */
        *success = false;
        return node;
    }
    
    /* Reechilibrare */
    return avl_rebalance(node);
}

/**
 * Caută un cititor după ID
 * 
 * Complexitate: O(log n)
 */
static ReaderNode *avl_search(ReaderNode *node, int reader_id) {
    if (!node) return NULL;
    
    if (reader_id < node->reader_id) {
        return avl_search(node->left, reader_id);
    } else if (reader_id > node->reader_id) {
        return avl_search(node->right, reader_id);
    }
    return node;
}

/**
 * Găsește nodul cu valoarea minimă (pentru ștergere)
 */
static ReaderNode *avl_find_min(ReaderNode *node) {
    while (node && node->left) {
        node = node->left;
    }
    return node;
}

/**
 * Șterge un cititor din AVL Tree
 * 
 * Complexitate: O(log n)
 */
static ReaderNode *avl_delete(ReaderNode *node, int reader_id, 
                               ReaderNode **deleted) {
    if (!node) return NULL;
    
    if (reader_id < node->reader_id) {
        node->left = avl_delete(node->left, reader_id, deleted);
    } else if (reader_id > node->reader_id) {
        node->right = avl_delete(node->right, reader_id, deleted);
    } else {
        /* Nod găsit */
        *deleted = node;
        
        /* Caz 1: Nod fără copii sau cu un singur copil */
        if (!node->left || !node->right) {
            ReaderNode *temp = node->left ? node->left : node->right;
            
            if (!temp) {
                /* Nod frunză */
                return NULL;
            } else {
                /* Un copil - îl promovăm */
                return temp;
            }
        }
        
        /* Caz 2: Nod cu doi copii */
        /* Găsim succesorul inorder (cel mai mic din subarborele drept) */
        ReaderNode *successor = avl_find_min(node->right);
        
        /* Copiem datele succesorului */
        node->reader_id = successor->reader_id;
        strncpy(node->name, successor->name, MAX_NAME);
        strncpy(node->faculty, successor->faculty, MAX_FACULTY);
        node->loans = successor->loans;
        node->loan_count = successor->loan_count;
        
        /* Ștergem succesorul */
        ReaderNode *dummy = NULL;
        node->right = avl_delete(node->right, successor->reader_id, &dummy);
        
        /* Nu eliberăm nodul original - doar l-am refolosit */
        *deleted = NULL;
    }
    
    return avl_rebalance(node);
}

/**
 * Traversare inorder pentru afișare
 */
static void avl_inorder_print(ReaderNode *node) {
    if (!node) return;
    
    avl_inorder_print(node->left);
    printf("  ID: %d | %s | %s | Împrumuturi: %d\n",
           node->reader_id, node->name, node->faculty, node->loan_count);
    avl_inorder_print(node->right);
}

/**
 * Eliberează memoria unui nod și a împrumuturilor sale
 */
static void free_reader_node(ReaderNode *node) {
    if (!node) return;
    
    /* Eliberăm lista de împrumuturi */
    Loan *loan = node->loans;
    while (loan) {
        Loan *next = loan->next;
        free(loan);
        loan = next;
    }
    
    free(node);
}

/**
 * Eliberează întregul arbore AVL
 */
static void free_avl_tree(ReaderNode *node) {
    if (!node) return;
    
    free_avl_tree(node->left);
    free_avl_tree(node->right);
    free_reader_node(node);
}

/* =============================================================================
 * FUNCȚII PRIORITY QUEUE (MIN-HEAP) PENTRU REZERVĂRI
 * =============================================================================
 */

/**
 * Creează un heap pentru rezervări
 */
static ReservationHeap *create_reservation_heap(int capacity) {
    ReservationHeap *heap = malloc(sizeof(ReservationHeap));
    if (!heap) return NULL;
    
    heap->heap = malloc(sizeof(Reservation) * capacity);
    if (!heap->heap) {
        free(heap);
        return NULL;
    }
    
    heap->capacity = capacity;
    heap->size = 0;
    return heap;
}

/**
 * Funcții helper pentru indici în heap
 */
static int heap_parent(int i) { return (i - 1) / 2; }
static int heap_left(int i) { return 2 * i + 1; }
static int heap_right(int i) { return 2 * i + 2; }

/**
 * Interschimbă două rezervări
 */
static void heap_swap(Reservation *a, Reservation *b) {
    Reservation temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * Heapify-up după inserare
 */
static void heap_bubble_up(ReservationHeap *heap, int i) {
    while (i > 0) {
        int parent = heap_parent(i);
        /* Min-heap: părintele trebuie să fie mai mic (data mai veche) */
        if (heap->heap[parent].reservation_date <= heap->heap[i].reservation_date) {
            break;
        }
        heap_swap(&heap->heap[parent], &heap->heap[i]);
        i = parent;
    }
}

/**
 * Heapify-down după extragere
 */
static void heap_bubble_down(ReservationHeap *heap, int i) {
    while (1) {
        int smallest = i;
        int left = heap_left(i);
        int right = heap_right(i);
        
        if (left < heap->size && 
            heap->heap[left].reservation_date < heap->heap[smallest].reservation_date) {
            smallest = left;
        }
        if (right < heap->size && 
            heap->heap[right].reservation_date < heap->heap[smallest].reservation_date) {
            smallest = right;
        }
        
        if (smallest == i) break;
        
        heap_swap(&heap->heap[i], &heap->heap[smallest]);
        i = smallest;
    }
}

/**
 * Inserează o rezervare în heap
 * 
 * Complexitate: O(log n)
 */
static bool heap_insert_reservation(ReservationHeap *heap, int reader_id,
                                     const char *isbn, time_t date) {
    if (!heap || heap->size >= heap->capacity) return false;
    
    int i = heap->size;
    heap->heap[i].reader_id = reader_id;
    strncpy(heap->heap[i].isbn, isbn, ISBN_LENGTH);
    heap->heap[i].reservation_date = date;
    heap->size++;
    
    heap_bubble_up(heap, i);
    return true;
}

/**
 * Extrage rezervarea cu prioritate maximă (cea mai veche)
 * 
 * Complexitate: O(log n)
 */
static bool heap_extract_min(ReservationHeap *heap, Reservation *out) {
    if (!heap || heap->size == 0) return false;
    
    *out = heap->heap[0];
    heap->heap[0] = heap->heap[heap->size - 1];
    heap->size--;
    
    if (heap->size > 0) {
        heap_bubble_down(heap, 0);
    }
    return true;
}

/**
 * Caută prima rezervare pentru o carte specifică
 */
static bool heap_find_reservation_for_isbn(ReservationHeap *heap, 
                                           const char *isbn,
                                           Reservation *out) {
    if (!heap) return false;
    
    /* Căutare liniară - heap-ul nu este optimizat pentru căutare */
    int oldest_idx = -1;
    time_t oldest_time = 0;
    
    for (int i = 0; i < heap->size; i++) {
        if (strcmp(heap->heap[i].isbn, isbn) == 0) {
            if (oldest_idx < 0 || heap->heap[i].reservation_date < oldest_time) {
                oldest_idx = i;
                oldest_time = heap->heap[i].reservation_date;
            }
        }
    }
    
    if (oldest_idx >= 0) {
        *out = heap->heap[oldest_idx];
        
        /* Ștergem această rezervare */
        heap->heap[oldest_idx] = heap->heap[heap->size - 1];
        heap->size--;
        
        if (oldest_idx < heap->size) {
            heap_bubble_down(heap, oldest_idx);
            heap_bubble_up(heap, oldest_idx);
        }
        return true;
    }
    return false;
}

/**
 * Eliberează memoria heap-ului
 */
static void free_reservation_heap(ReservationHeap *heap) {
    if (!heap) return;
    free(heap->heap);
    free(heap);
}

/* =============================================================================
 * FUNCȚII PRINCIPALE ALE BIBLIOTECII
 * =============================================================================
 */

/**
 * Creează o nouă bibliotecă
 */
Library *create_library(void) {
    Library *lib = malloc(sizeof(Library));
    if (!lib) return NULL;
    
    lib->books = create_book_table();
    if (!lib->books) {
        free(lib);
        return NULL;
    }
    
    lib->readers = malloc(sizeof(ReaderAVLTree));
    if (!lib->readers) {
        free_book_table(lib->books);
        free(lib);
        return NULL;
    }
    lib->readers->root = NULL;
    lib->readers->count = 0;
    
    lib->reservations = create_reservation_heap(1000);
    if (!lib->reservations) {
        free_book_table(lib->books);
        free(lib->readers);
        free(lib);
        return NULL;
    }
    
    printf(COLOR_GREEN "✓ Biblioteca creată cu succes!" COLOR_RESET "\n");
    return lib;
}

/**
 * Adaugă o carte în bibliotecă
 * 
 * Dacă cartea există deja, incrementează numărul de exemplare
 */
bool add_book(Library *lib, const char *isbn, const char *title,
              const char *author, int year, int copies) {
    if (!lib || !isbn || !title || !author || copies <= 0) return false;
    
    /* Verificăm dacă cartea există */
    Book *existing = find_book_in_table(lib->books, isbn);
    if (existing) {
        existing->total_copies += copies;
        existing->available_copies += copies;
        printf(COLOR_CYAN "  Adăugate %d exemplare la '%s' (total: %d)\n" 
               COLOR_RESET, copies, existing->title, existing->total_copies);
        return true;
    }
    
    /* Creem o carte nouă */
    Book *book = malloc(sizeof(Book));
    if (!book) return false;
    
    strncpy(book->isbn, isbn, ISBN_LENGTH - 1);
    book->isbn[ISBN_LENGTH - 1] = '\0';
    strncpy(book->title, title, MAX_TITLE - 1);
    book->title[MAX_TITLE - 1] = '\0';
    strncpy(book->author, author, MAX_AUTHOR - 1);
    book->author[MAX_AUTHOR - 1] = '\0';
    book->year = year;
    book->total_copies = copies;
    book->available_copies = copies;
    book->borrow_count = 0;
    book->next = NULL;
    
    if (!insert_book_in_table(lib->books, book)) {
        free(book);
        return false;
    }
    
    printf(COLOR_GREEN "  ✓ Carte adăugată: '%s' de %s (%d exemplare)\n" 
           COLOR_RESET, title, author, copies);
    return true;
}

/**
 * Caută o carte după ISBN
 */
Book *find_book(Library *lib, const char *isbn) {
    if (!lib || !isbn) return NULL;
    return find_book_in_table(lib->books, isbn);
}

/**
 * Șterge o carte din bibliotecă
 * 
 * Nu permite ștergerea dacă există exemplare împrumutate
 */
bool remove_book(Library *lib, const char *isbn) {
    if (!lib || !isbn) return false;
    
    Book *book = find_book_in_table(lib->books, isbn);
    if (!book) {
        printf(COLOR_RED "  ✗ Cartea cu ISBN %s nu a fost găsită\n" 
               COLOR_RESET, isbn);
        return false;
    }
    
    if (book->available_copies < book->total_copies) {
        printf(COLOR_RED "  ✗ Nu se poate șterge - există exemplare împrumutate\n" 
               COLOR_RESET);
        return false;
    }
    
    Book *removed = remove_book_from_table(lib->books, isbn);
    if (removed) {
        printf(COLOR_YELLOW "  Carte ștearsă: '%s'\n" COLOR_RESET, removed->title);
        free(removed);
        return true;
    }
    return false;
}

/**
 * Înregistrează un cititor nou
 */
bool register_reader(Library *lib, int reader_id, const char *name,
                     const char *faculty) {
    if (!lib || !name || !faculty || reader_id <= 0) return false;
    
    bool success = false;
    lib->readers->root = avl_insert(lib->readers->root, reader_id, name, 
                                     faculty, &success);
    
    if (success) {
        lib->readers->count++;
        printf(COLOR_GREEN "  ✓ Cititor înregistrat: %s (ID: %d)\n" 
               COLOR_RESET, name, reader_id);
    } else {
        printf(COLOR_RED "  ✗ ID-ul %d există deja în sistem\n" 
               COLOR_RESET, reader_id);
    }
    return success;
}

/**
 * Caută un cititor după ID
 */
ReaderNode *find_reader(Library *lib, int reader_id) {
    if (!lib || reader_id <= 0) return NULL;
    return avl_search(lib->readers->root, reader_id);
}

/**
 * Dezînregistrează un cititor
 * 
 * Nu permite dezînregistrarea dacă există împrumuturi active
 */
bool unregister_reader(Library *lib, int reader_id) {
    if (!lib || reader_id <= 0) return false;
    
    ReaderNode *reader = avl_search(lib->readers->root, reader_id);
    if (!reader) {
        printf(COLOR_RED "  ✗ Cititorul cu ID %d nu a fost găsit\n" 
               COLOR_RESET, reader_id);
        return false;
    }
    
    if (reader->loan_count > 0) {
        printf(COLOR_RED "  ✗ Cititorul are %d împrumuturi active\n" 
               COLOR_RESET, reader->loan_count);
        return false;
    }
    
    ReaderNode *deleted = NULL;
    lib->readers->root = avl_delete(lib->readers->root, reader_id, &deleted);
    
    if (deleted) {
        printf(COLOR_YELLOW "  Cititor dezînregistrat: %s\n" 
               COLOR_RESET, deleted->name);
        free_reader_node(deleted);
        lib->readers->count--;
        return true;
    }
    return false;
}

/**
 * Împrumută o carte unui cititor
 */
bool borrow_book(Library *lib, int reader_id, const char *isbn) {
    if (!lib || reader_id <= 0 || !isbn) return false;
    
    /* Verificăm cititorul */
    ReaderNode *reader = avl_search(lib->readers->root, reader_id);
    if (!reader) {
        printf(COLOR_RED "  ✗ Cititorul cu ID %d nu a fost găsit\n" 
               COLOR_RESET, reader_id);
        return false;
    }
    
    /* Verificăm limita de împrumuturi */
    if (reader->loan_count >= MAX_LOANS_PER_READER) {
        printf(COLOR_RED "  ✗ Limită atinsă - %s are deja %d împrumuturi\n" 
               COLOR_RESET, reader->name, MAX_LOANS_PER_READER);
        return false;
    }
    
    /* Verificăm cartea */
    Book *book = find_book_in_table(lib->books, isbn);
    if (!book) {
        printf(COLOR_RED "  ✗ Cartea cu ISBN %s nu a fost găsită\n" 
               COLOR_RESET, isbn);
        return false;
    }
    
    if (book->available_copies <= 0) {
        printf(COLOR_YELLOW "  Nu sunt exemplare disponibile. Doriți să rezervați?\n" 
               COLOR_RESET);
        return false;
    }
    
    /* Verificăm dacă cititorul a împrumutat deja această carte */
    Loan *check = reader->loans;
    while (check) {
        if (strcmp(check->isbn, isbn) == 0) {
            printf(COLOR_RED "  ✗ Cititorul a împrumutat deja această carte\n" 
                   COLOR_RESET);
            return false;
        }
        check = check->next;
    }
    
    /* Creăm împrumutul */
    Loan *loan = malloc(sizeof(Loan));
    if (!loan) return false;
    
    strncpy(loan->isbn, isbn, ISBN_LENGTH);
    loan->borrow_date = time(NULL);
    loan->due_date = loan->borrow_date + LOAN_DURATION_DAYS * 24 * 3600;
    loan->next = reader->loans;
    reader->loans = loan;
    reader->loan_count++;
    
    /* Actualizăm cartea */
    book->available_copies--;
    book->borrow_count++;
    
    printf(COLOR_GREEN "  ✓ %s a împrumutat '%s'\n" COLOR_RESET, 
           reader->name, book->title);
    printf("     Termen retur: %s", ctime(&loan->due_date));
    
    return true;
}

/**
 * Returnează o carte împrumutată
 */
bool return_book(Library *lib, int reader_id, const char *isbn) {
    if (!lib || reader_id <= 0 || !isbn) return false;
    
    /* Găsim cititorul */
    ReaderNode *reader = avl_search(lib->readers->root, reader_id);
    if (!reader) {
        printf(COLOR_RED "  ✗ Cititorul cu ID %d nu a fost găsit\n" 
               COLOR_RESET, reader_id);
        return false;
    }
    
    /* Căutăm împrumutul */
    Loan *prev = NULL;
    Loan *current = reader->loans;
    while (current) {
        if (strcmp(current->isbn, isbn) == 0) {
            /* Găsit - îl eliminăm din listă */
            if (prev) {
                prev->next = current->next;
            } else {
                reader->loans = current->next;
            }
            reader->loan_count--;
            
            /* Găsim cartea */
            Book *book = find_book_in_table(lib->books, isbn);
            if (book) {
                book->available_copies++;
                
                /* Verificăm dacă există rezervări */
                Reservation res;
                if (heap_find_reservation_for_isbn(lib->reservations, isbn, &res)) {
                    ReaderNode *next_reader = avl_search(lib->readers->root, 
                                                          res.reader_id);
                    if (next_reader) {
                        printf(COLOR_MAGENTA "  📬 Notificare: %s poate împrumuta '%s'\n" 
                               COLOR_RESET, next_reader->name, book->title);
                    }
                }
                
                printf(COLOR_GREEN "  ✓ %s a returnat '%s'\n" COLOR_RESET, 
                       reader->name, book->title);
                
                /* Verificăm dacă a fost întârziat */
                time_t now = time(NULL);
                if (now > current->due_date) {
                    double days_late = difftime(now, current->due_date) / (24 * 3600);
                    printf(COLOR_YELLOW "     ⚠ Întârziere: %.0f zile\n" 
                           COLOR_RESET, days_late);
                }
            }
            
            free(current);
            return true;
        }
        prev = current;
        current = current->next;
    }
    
    printf(COLOR_RED "  ✗ Împrumutul nu a fost găsit\n" COLOR_RESET);
    return false;
}

/**
 * Rezervă o carte (dacă nu este disponibilă)
 */
bool reserve_book(Library *lib, int reader_id, const char *isbn) {
    if (!lib || reader_id <= 0 || !isbn) return false;
    
    /* Verificăm cititorul */
    ReaderNode *reader = avl_search(lib->readers->root, reader_id);
    if (!reader) {
        printf(COLOR_RED "  ✗ Cititorul cu ID %d nu a fost găsit\n" 
               COLOR_RESET, reader_id);
        return false;
    }
    
    /* Verificăm cartea */
    Book *book = find_book_in_table(lib->books, isbn);
    if (!book) {
        printf(COLOR_RED "  ✗ Cartea cu ISBN %s nu a fost găsită\n" 
               COLOR_RESET, isbn);
        return false;
    }
    
    if (book->available_copies > 0) {
        printf(COLOR_CYAN "  Există exemplare disponibile. Folosiți borrow_book().\n" 
               COLOR_RESET);
        return false;
    }
    
    /* Adăugăm rezervarea */
    if (heap_insert_reservation(lib->reservations, reader_id, isbn, time(NULL))) {
        printf(COLOR_GREEN "  ✓ Rezervare creată pentru '%s' de %s\n" COLOR_RESET, 
               book->title, reader->name);
        return true;
    }
    
    printf(COLOR_RED "  ✗ Eroare la crearea rezervării\n" COLOR_RESET);
    return false;
}

/**
 * Listează cărțile împrumutate de un cititor
 */
void list_borrowed_by_reader(Library *lib, int reader_id) {
    if (!lib || reader_id <= 0) return;
    
    ReaderNode *reader = avl_search(lib->readers->root, reader_id);
    if (!reader) {
        printf(COLOR_RED "  Cititorul cu ID %d nu a fost găsit\n" COLOR_RESET, reader_id);
        return;
    }
    
    printf("\n" COLOR_CYAN "═══════════════════════════════════════════════\n");
    printf("  ÎMPRUMUTURI PENTRU: %s\n", reader->name);
    printf("═══════════════════════════════════════════════" COLOR_RESET "\n");
    
    if (reader->loan_count == 0) {
        printf("  (niciun împrumut activ)\n");
        return;
    }
    
    Loan *loan = reader->loans;
    int idx = 1;
    time_t now = time(NULL);
    
    while (loan) {
        Book *book = find_book_in_table(lib->books, loan->isbn);
        printf("  %d. ", idx++);
        
        if (book) {
            printf("'%s' de %s", book->title, book->author);
        } else {
            printf("[ISBN: %s]", loan->isbn);
        }
        
        if (now > loan->due_date) {
            printf(COLOR_RED " [ÎNTÂRZIAT!]" COLOR_RESET);
        } else {
            double days_left = difftime(loan->due_date, now) / (24 * 3600);
            printf(COLOR_GREEN " [%d zile rămase]" COLOR_RESET, (int)days_left);
        }
        printf("\n");
        
        loan = loan->next;
    }
    printf("\n");
}

/**
 * Traversare helper pentru colectarea cărților întârziate
 */
typedef struct {
    Book **books;
    double *days_overdue;
    int *reader_ids;
    int count;
    int capacity;
    time_t now;
    int threshold_days;
} OverdueContext;

static void collect_overdue_helper(ReaderNode *reader, OverdueContext *ctx, 
                                    BookHashTable *book_table) {
    if (!reader) return;
    
    collect_overdue_helper(reader->left, ctx, book_table);
    
    Loan *loan = reader->loans;
    while (loan) {
        time_t overdue_threshold = ctx->now - ctx->threshold_days * 24 * 3600;
        if (loan->due_date < overdue_threshold && ctx->count < ctx->capacity) {
            Book *book = find_book_in_table(book_table, loan->isbn);
            if (book) {
                ctx->books[ctx->count] = book;
                ctx->days_overdue[ctx->count] = difftime(ctx->now, loan->due_date) / (24 * 3600);
                ctx->reader_ids[ctx->count] = reader->reader_id;
                ctx->count++;
            }
        }
        loan = loan->next;
    }
    
    collect_overdue_helper(reader->right, ctx, book_table);
}

/**
 * Listează cărțile cu termen depășit
 */
void list_overdue_books(Library *lib, int days) {
    if (!lib) return;
    
    printf("\n" COLOR_RED "═══════════════════════════════════════════════\n");
    printf("  CĂRȚI CU TERMEN DEPĂȘIT (>%d zile)\n", days);
    printf("═══════════════════════════════════════════════" COLOR_RESET "\n");
    
    OverdueContext ctx;
    ctx.capacity = 100;
    ctx.count = 0;
    ctx.now = time(NULL);
    ctx.threshold_days = days;
    ctx.books = malloc(sizeof(Book*) * ctx.capacity);
    ctx.days_overdue = malloc(sizeof(double) * ctx.capacity);
    ctx.reader_ids = malloc(sizeof(int) * ctx.capacity);
    
    if (!ctx.books || !ctx.days_overdue || !ctx.reader_ids) {
        free(ctx.books);
        free(ctx.days_overdue);
        free(ctx.reader_ids);
        return;
    }
    
    collect_overdue_helper(lib->readers->root, &ctx, lib->books);
    
    if (ctx.count == 0) {
        printf("  (nicio carte întârziată)\n");
    } else {
        for (int i = 0; i < ctx.count; i++) {
            ReaderNode *reader = avl_search(lib->readers->root, ctx.reader_ids[i]);
            printf("  • '%s' - %.0f zile întârziere\n", 
                   ctx.books[i]->title, ctx.days_overdue[i]);
            if (reader) {
                printf("    Împrumutat de: %s (ID: %d)\n", 
                       reader->name, reader->reader_id);
            }
        }
    }
    
    free(ctx.books);
    free(ctx.days_overdue);
    free(ctx.reader_ids);
    printf("\n");
}

/**
 * Comparator pentru sortarea cărților după popularitate
 */
static int compare_by_popularity(const void *a, const void *b) {
    const Book *book_a = *(const Book **)a;
    const Book *book_b = *(const Book **)b;
    return book_b->borrow_count - book_a->borrow_count;  /* Descrescător */
}

/**
 * Listează cele mai populare cărți
 */
void list_popular_books(Library *lib, int top_n) {
    if (!lib || top_n <= 0) return;
    
    printf("\n" COLOR_MAGENTA "═══════════════════════════════════════════════\n");
    printf("  TOP %d CELE MAI POPULARE CĂRȚI\n", top_n);
    printf("═══════════════════════════════════════════════" COLOR_RESET "\n");
    
    /* Colectăm toate cărțile */
    Book **all_books = malloc(sizeof(Book*) * lib->books->count);
    if (!all_books) return;
    
    int count = 0;
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        Book *book = lib->books->buckets[i];
        while (book) {
            all_books[count++] = book;
            book = book->next;
        }
    }
    
    /* Sortăm după popularitate */
    qsort(all_books, count, sizeof(Book*), compare_by_popularity);
    
    /* Afișăm top N */
    int to_show = (count < top_n) ? count : top_n;
    for (int i = 0; i < to_show; i++) {
        printf("  %d. '%s' de %s - %d împrumuturi\n",
               i + 1, all_books[i]->title, all_books[i]->author,
               all_books[i]->borrow_count);
    }
    
    if (count == 0) {
        printf("  (nicio carte în bibliotecă)\n");
    }
    
    free(all_books);
    printf("\n");
}

/* =============================================================================
 * PERSISTENȚA DATELOR (SALVARE/ÎNCĂRCARE FIȘIER BINAR)
 * =============================================================================
 */

/* Structură header pentru fișier */
typedef struct {
    char magic[8];          /* "LIBV1.0\0" */
    int book_count;
    int reader_count;
    int reservation_count;
} FileHeader;

/**
 * Salvează biblioteca în fișier binar
 */
bool save_library(Library *lib, const char *filename) {
    if (!lib || !filename) return false;
    
    FILE *f = fopen(filename, "wb");
    if (!f) {
        perror("Eroare la deschiderea fișierului");
        return false;
    }
    
    /* Scriem header-ul */
    FileHeader header;
    strncpy(header.magic, "LIBV1.0", 8);
    header.book_count = lib->books->count;
    header.reader_count = lib->readers->count;
    header.reservation_count = lib->reservations->size;
    
    fwrite(&header, sizeof(FileHeader), 1, f);
    
    /* Scriem cărțile */
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        Book *book = lib->books->buckets[i];
        while (book) {
            fwrite(book->isbn, ISBN_LENGTH, 1, f);
            fwrite(book->title, MAX_TITLE, 1, f);
            fwrite(book->author, MAX_AUTHOR, 1, f);
            fwrite(&book->year, sizeof(int), 1, f);
            fwrite(&book->total_copies, sizeof(int), 1, f);
            fwrite(&book->available_copies, sizeof(int), 1, f);
            fwrite(&book->borrow_count, sizeof(int), 1, f);
            book = book->next;
        }
    }
    
    /* Scriem cititorii (traversare inorder pentru ordine consistentă) */
    /* Folosim o stivă pentru traversare iterativă */
    ReaderNode **stack = malloc(sizeof(ReaderNode*) * lib->readers->count * 2);
    int stack_top = 0;
    ReaderNode *current = lib->readers->root;
    
    while (current || stack_top > 0) {
        while (current) {
            stack[stack_top++] = current;
            current = current->left;
        }
        
        current = stack[--stack_top];
        
        /* Scriem datele cititorului */
        fwrite(&current->reader_id, sizeof(int), 1, f);
        fwrite(current->name, MAX_NAME, 1, f);
        fwrite(current->faculty, MAX_FACULTY, 1, f);
        fwrite(&current->loan_count, sizeof(int), 1, f);
        
        /* Scriem împrumuturile */
        Loan *loan = current->loans;
        while (loan) {
            fwrite(loan->isbn, ISBN_LENGTH, 1, f);
            fwrite(&loan->borrow_date, sizeof(time_t), 1, f);
            fwrite(&loan->due_date, sizeof(time_t), 1, f);
            loan = loan->next;
        }
        
        current = current->right;
    }
    free(stack);
    
    /* Scriem rezervările */
    for (int i = 0; i < lib->reservations->size; i++) {
        fwrite(&lib->reservations->heap[i].reader_id, sizeof(int), 1, f);
        fwrite(lib->reservations->heap[i].isbn, ISBN_LENGTH, 1, f);
        fwrite(&lib->reservations->heap[i].reservation_date, sizeof(time_t), 1, f);
    }
    
    fclose(f);
    printf(COLOR_GREEN "✓ Biblioteca salvată în '%s'\n" COLOR_RESET, filename);
    return true;
}

/**
 * Încarcă biblioteca din fișier binar
 */
bool load_library(Library **lib_ptr, const char *filename) {
    if (!lib_ptr || !filename) return false;
    
    FILE *f = fopen(filename, "rb");
    if (!f) {
        perror("Eroare la deschiderea fișierului");
        return false;
    }
    
    /* Citim header-ul */
    FileHeader header;
    if (fread(&header, sizeof(FileHeader), 1, f) != 1) {
        fclose(f);
        return false;
    }
    
    /* Verificăm magic number */
    if (strncmp(header.magic, "LIBV1.0", 7) != 0) {
        printf(COLOR_RED "Fișier invalid sau corupt\n" COLOR_RESET);
        fclose(f);
        return false;
    }
    
    /* Creăm biblioteca nouă */
    Library *lib = create_library();
    if (!lib) {
        fclose(f);
        return false;
    }
    
    /* Citim cărțile */
    for (int i = 0; i < header.book_count; i++) {
        char isbn[ISBN_LENGTH], title[MAX_TITLE], author[MAX_AUTHOR];
        int year, total, available, borrow_count;
        
        fread(isbn, ISBN_LENGTH, 1, f);
        fread(title, MAX_TITLE, 1, f);
        fread(author, MAX_AUTHOR, 1, f);
        fread(&year, sizeof(int), 1, f);
        fread(&total, sizeof(int), 1, f);
        fread(&available, sizeof(int), 1, f);
        fread(&borrow_count, sizeof(int), 1, f);
        
        Book *book = malloc(sizeof(Book));
        if (book) {
            strncpy(book->isbn, isbn, ISBN_LENGTH);
            strncpy(book->title, title, MAX_TITLE);
            strncpy(book->author, author, MAX_AUTHOR);
            book->year = year;
            book->total_copies = total;
            book->available_copies = available;
            book->borrow_count = borrow_count;
            book->next = NULL;
            insert_book_in_table(lib->books, book);
        }
    }
    
    /* Citim cititorii */
    for (int i = 0; i < header.reader_count; i++) {
        int reader_id, loan_count;
        char name[MAX_NAME], faculty[MAX_FACULTY];
        
        fread(&reader_id, sizeof(int), 1, f);
        fread(name, MAX_NAME, 1, f);
        fread(faculty, MAX_FACULTY, 1, f);
        fread(&loan_count, sizeof(int), 1, f);
        
        bool success = false;
        lib->readers->root = avl_insert(lib->readers->root, reader_id, 
                                         name, faculty, &success);
        if (success) {
            lib->readers->count++;
            
            /* Citim împrumuturile */
            ReaderNode *reader = avl_search(lib->readers->root, reader_id);
            for (int j = 0; j < loan_count && reader; j++) {
                Loan *loan = malloc(sizeof(Loan));
                if (loan) {
                    fread(loan->isbn, ISBN_LENGTH, 1, f);
                    fread(&loan->borrow_date, sizeof(time_t), 1, f);
                    fread(&loan->due_date, sizeof(time_t), 1, f);
                    loan->next = reader->loans;
                    reader->loans = loan;
                    reader->loan_count++;
                }
            }
        }
    }
    
    /* Citim rezervările */
    for (int i = 0; i < header.reservation_count; i++) {
        int reader_id;
        char isbn[ISBN_LENGTH];
        time_t date;
        
        fread(&reader_id, sizeof(int), 1, f);
        fread(isbn, ISBN_LENGTH, 1, f);
        fread(&date, sizeof(time_t), 1, f);
        
        heap_insert_reservation(lib->reservations, reader_id, isbn, date);
    }
    
    fclose(f);
    
    *lib_ptr = lib;
    printf(COLOR_GREEN "✓ Biblioteca încărcată din '%s'\n" COLOR_RESET, filename);
    printf("  Cărți: %d, Cititori: %d, Rezervări: %d\n",
           header.book_count, header.reader_count, header.reservation_count);
    
    return true;
}

/**
 * Eliberează toate resursele bibliotecii
 */
void free_library(Library *lib) {
    if (!lib) return;
    
    free_book_table(lib->books);
    free_avl_tree(lib->readers->root);
    free(lib->readers);
    free_reservation_heap(lib->reservations);
    free(lib);
    
    printf(COLOR_GREEN "✓ Memoria eliberată\n" COLOR_RESET);
}

/* =============================================================================
 * FUNCȚII DE AFIȘARE ȘI STATISTICI
 * =============================================================================
 */

/**
 * Afișează statisticile bibliotecii
 */
void print_library_stats(Library *lib) {
    if (!lib) return;
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║              STATISTICI BIBLIOTECĂ                            ║\n");
    printf("╠═══════════════════════════════════════════════════════════════╣\n");
    printf("║  Cărți distincte:        %5d                                 ║\n", 
           lib->books->count);
    printf("║  Cititori înregistrați:  %5d                                 ║\n", 
           lib->readers->count);
    printf("║  Rezervări în așteptare: %5d                                 ║\n", 
           lib->reservations->size);
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
}

/**
 * Afișează toți cititorii
 */
void print_all_readers(Library *lib) {
    if (!lib) return;
    
    printf("\n" COLOR_CYAN "═══════════════════════════════════════════════\n");
    printf("  TOȚI CITITORII ÎNREGISTRAȚI\n");
    printf("═══════════════════════════════════════════════" COLOR_RESET "\n");
    
    if (lib->readers->count == 0) {
        printf("  (niciun cititor înregistrat)\n");
    } else {
        avl_inorder_print(lib->readers->root);
    }
    printf("\n");
}

/* =============================================================================
 * FUNCȚIA MAIN - DEMONSTRAȚIE COMPLETĂ
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     TEMA 1: SISTEM DE GESTIUNE BIBLIOTECĂ                     ║\n");
    printf("║              Soluție Completă                                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* --- PARTEA 1: Crearea bibliotecii --- */
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("  PARTEA 1: INIȚIALIZARE\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    Library *lib = create_library();
    if (!lib) {
        fprintf(stderr, "Eroare la crearea bibliotecii!\n");
        return 1;
    }
    
    /* --- PARTEA 2: Adăugare cărți --- */
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("  PARTEA 2: ADĂUGARE CĂRȚI\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    add_book(lib, "9789731032771", "Ion", "Liviu Rebreanu", 1920, 3);
    add_book(lib, "9789731031234", "Enigma Otiliei", "George Calinescu", 1938, 2);
    add_book(lib, "9789731035678", "Morometii", "Marin Preda", 1955, 4);
    add_book(lib, "9789731039012", "Maitreyi", "Mircea Eliade", 1933, 2);
    add_book(lib, "9789731033456", "La tiganci", "Mircea Eliade", 1969, 1);
    add_book(lib, "9789731037890", "Baltagul", "Mihail Sadoveanu", 1930, 3);
    add_book(lib, "9789731036543", "Ultima noapte...", "Camil Petrescu", 1930, 2);
    add_book(lib, "9789731034567", "Padurea spanzuratilor", "Liviu Rebreanu", 1922, 2);
    
    /* --- PARTEA 3: Înregistrare cititori --- */
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("  PARTEA 3: ÎNREGISTRARE CITITORI\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    register_reader(lib, 1001, "Ion Popescu", "Informatica");
    register_reader(lib, 1002, "Maria Ionescu", "Economie");
    register_reader(lib, 1003, "Andrei Georgescu", "Drept");
    register_reader(lib, 1004, "Elena Dumitrescu", "Litere");
    register_reader(lib, 1005, "Mihai Constantinescu", "Informatica");
    
    /* Test: ID duplicat */
    register_reader(lib, 1001, "Alt Nume", "Alta Facultate");
    
    /* --- PARTEA 4: Împrumuturi --- */
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("  PARTEA 4: ÎMPRUMUTURI\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    borrow_book(lib, 1001, "9789731032771");  /* Ion */
    borrow_book(lib, 1001, "9789731031234");  /* Enigma Otiliei */
    borrow_book(lib, 1002, "9789731035678");  /* Morometii */
    borrow_book(lib, 1003, "9789731039012");  /* Maitreyi */
    borrow_book(lib, 1004, "9789731032771");  /* Ion - alt exemplar */
    borrow_book(lib, 1005, "9789731037890");  /* Baltagul */
    
    /* Test: carte deja împrumutată */
    borrow_book(lib, 1001, "9789731032771");
    
    /* --- PARTEA 5: Rezervări --- */
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("  PARTEA 5: REZERVĂRI\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    /* Împrumutăm ultimul exemplar din Maitreyi */
    borrow_book(lib, 1005, "9789731039012");  /* Ultimul exemplar */
    
    /* Încercăm să rezervăm */
    reserve_book(lib, 1001, "9789731039012");  /* Rezervare pentru Maitreyi */
    reserve_book(lib, 1002, "9789731039012");  /* Alta rezervare */
    
    /* --- PARTEA 6: Rapoarte --- */
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("  PARTEA 6: RAPOARTE\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    
    list_borrowed_by_reader(lib, 1001);
    list_borrowed_by_reader(lib, 1005);
    
    list_popular_books(lib, 5);
    
    /* Pentru test întârzieri, ar trebui să modificăm due_date în trecut */
    /* În producție, acest lucru s-ar întâmpla natural cu trecerea timpului */
    list_overdue_books(lib, 0);  /* Afișăm orice întârziere */
    
    /* --- PARTEA 7: Returnări --- */
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("  PARTEA 7: RETURNĂRI\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    return_book(lib, 1003, "9789731039012");  /* Maitreyi - declanșează notificare */
    
    /* --- PARTEA 8: Afișare cititori --- */
    print_all_readers(lib);
    
    /* --- PARTEA 9: Statistici finale --- */
    print_library_stats(lib);
    
    /* --- PARTEA 10: Persistență --- */
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("  PARTEA 10: PERSISTENȚĂ\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    save_library(lib, "biblioteca.dat");
    
    /* Test: încărcăm biblioteca salvată */
    Library *lib2 = NULL;
    if (load_library(&lib2, "biblioteca.dat")) {
        print_library_stats(lib2);
        free_library(lib2);
    }
    
    /* --- CLEANUP --- */
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("  CLEANUP\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    free_library(lib);
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     DEMONSTRAȚIE COMPLETĂ                                     ║\n");
    printf("║                                                               ║\n");
    printf("║  Structuri integrate:                                         ║\n");
    printf("║    ✓ Hash Table (cărți, căutare O(1))                         ║\n");
    printf("║    ✓ AVL Tree (cititori, căutare O(log n))                    ║\n");
    printf("║    ✓ Min-Heap (rezervări, prioritate O(log n))                ║\n");
    printf("║    ✓ Linked List (împrumuturi, lanțuri hash)                  ║\n");
    printf("║                                                               ║\n");
    printf("║  Rulați cu Valgrind pentru verificare memory leaks:           ║\n");
    printf("║    valgrind --leak-check=full ./hw1                           ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}

/* =============================================================================
 * END OF FILE
 * =============================================================================
 */
