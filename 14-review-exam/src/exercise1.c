/**
 * =============================================================================
 * EXERCIȚIUL 1: Sistem de Gestiune Comenzi
 * Săptămâna 14 - Recapitulare Avansată și Optimizare
 * =============================================================================
 *
 * OBIECTIV:
 *   Implementarea unui sistem complet de gestiune a comenzilor care integrează
 *   multiple structuri de date și algoritmi studiați în semestru:
 *   - Hash table pentru produse (căutare rapidă O(1))
 *   - Linked list pentru itemii dintr-o comandă
 *   - Binary Search Tree pentru comenzi (ordonate după ID)
 *   - Heap pentru prioritizarea comenzilor urgente
 *   - Function pointers pentru sortare și filtrare
 *
 * CERINȚE:
 *   1. Implementați structurile de date specificate
 *   2. Completați funcțiile de bază pentru fiecare structură
 *   3. Integrați structurile într-un sistem funcțional
 *   4. Gestionați corect memoria (fără memory leaks)
 *
 * EXEMPLU INPUT (orders.txt):
 *   P001 Laptop 4500.00 10
 *   P002 Mouse 89.99 50
 *   P003 Keyboard 299.99 25
 *
 * EXEMPLU OUTPUT:
 *   === SISTEM GESTIUNE COMENZI ===
 *   Produse încărcate: 3
 *   Comandă #1001: 2 produse, Total: 4589.99 RON
 *   ...
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -o exercise1 exercise1.c -lm
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

/* =============================================================================
 * CONSTANTE ȘI MACRO-URI
 * =============================================================================
 */

#define MAX_CODE_LEN 16
#define MAX_NAME_LEN 64
#define HASH_SIZE 101           /* Număr prim pentru hash table */
#define MAX_LINE 256

/* Coduri culori ANSI pentru output */
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_RESET   "\x1b[0m"

/* =============================================================================
 * STRUCTURI DE DATE
 * =============================================================================
 */

/**
 * Structura pentru un produs în inventar
 */
typedef struct Product {
    char code[MAX_CODE_LEN];    /* Cod unic produs (ex: "P001") */
    char name[MAX_NAME_LEN];    /* Denumire produs */
    double price;               /* Preț unitar */
    int stock;                  /* Cantitate în stoc */
    struct Product *next;       /* Pointer pentru hash chaining */
} Product;

/**
 * Hash table pentru produse
 */
typedef struct {
    Product *buckets[HASH_SIZE];
    int count;                  /* Număr total de produse */
} ProductHashTable;

/**
 * TODO 1: Completați structura pentru un item dintr-o comandă
 *
 * Un OrderItem trebuie să conțină:
 * - Pointer către produsul comandat
 * - Cantitatea comandată
 * - Pointer către următorul item (linked list)
 *
 * Hint: Folosiți Product* pentru referința la produs
 */
typedef struct OrderItem {
    /* YOUR CODE HERE */
    
} OrderItem;

/**
 * Structura pentru o comandă
 */
typedef struct Order {
    int id;                     /* ID unic comandă */
    char customer[MAX_NAME_LEN];/* Nume client */
    OrderItem *items;           /* Lista de produse comandate */
    int item_count;             /* Număr de produse în comandă */
    double total;               /* Valoare totală comandă */
    int priority;               /* Prioritate (1-10, 10 = urgent) */
    time_t created_at;          /* Timestamp creare */
    struct Order *left;         /* BST - copil stânga */
    struct Order *right;        /* BST - copil dreapta */
} Order;

/**
 * Binary Search Tree pentru comenzi
 */
typedef struct {
    Order *root;
    int count;
} OrderBST;

/**
 * TODO 2: Completați structura pentru priority queue (min-heap)
 *
 * Heap-ul va fi folosit pentru procesarea comenzilor urgente.
 * Trebuie să conțină:
 * - Array de pointeri către comenzi
 * - Capacitatea maximă
 * - Dimensiunea curentă
 *
 * Hint: Comenzile cu prioritate mare (10) au valoare heap mică
 *       pentru a fi procesate primele (min-heap inversat)
 */
typedef struct {
    /* YOUR CODE HERE */
    
} OrderHeap;

/* =============================================================================
 * DECLARAȚII FUNCȚII (PROTOTIPURI)
 * =============================================================================
 */

/* Hash Table Functions */
unsigned int hash_function(const char *key);
ProductHashTable *create_product_table(void);
bool insert_product(ProductHashTable *table, const char *code, 
                    const char *name, double price, int stock);
Product *find_product(ProductHashTable *table, const char *code);
void free_product_table(ProductHashTable *table);

/* Order Functions */
Order *create_order(int id, const char *customer, int priority);
bool add_item_to_order(Order *order, ProductHashTable *products,
                       const char *product_code, int quantity);
void calculate_order_total(Order *order);
void print_order(const Order *order);
void free_order(Order *order);

/* BST Functions */
OrderBST *create_order_bst(void);
Order *insert_order_bst(Order *root, Order *new_order);
Order *find_order_bst(Order *root, int id);
void inorder_traversal(Order *root, void (*visit)(const Order*));
void free_order_bst(Order *root);

/* Heap Functions */
OrderHeap *create_order_heap(int capacity);
bool heap_insert(OrderHeap *heap, Order *order);
Order *heap_extract_min(OrderHeap *heap);
void free_order_heap(OrderHeap *heap);

/* Utility Functions */
int load_products_from_file(ProductHashTable *table, const char *filename);
void print_separator(const char *title);

/* Function Pointer Types */
typedef bool (*OrderFilter)(const Order*);
typedef int (*OrderCompare)(const Order*, const Order*);

/* =============================================================================
 * IMPLEMENTARE: HASH TABLE PENTRU PRODUSE
 * =============================================================================
 */

/**
 * TODO 3: Implementați funcția hash
 *
 * Folosiți algoritmul djb2 pentru hash:
 *   hash = 5381
 *   for each char c in string:
 *       hash = hash * 33 + c
 *   return hash % HASH_SIZE
 *
 * @param key Cheia de hashuit (codul produsului)
 * @return Indexul în hash table (0 la HASH_SIZE-1)
 */
unsigned int hash_function(const char *key) {
    /* YOUR CODE HERE */
    
    return 0;  /* Înlocuiți această linie */
}

/**
 * Creează un hash table gol pentru produse
 */
ProductHashTable *create_product_table(void) {
    ProductHashTable *table = malloc(sizeof(ProductHashTable));
    if (!table) return NULL;
    
    for (int i = 0; i < HASH_SIZE; i++) {
        table->buckets[i] = NULL;
    }
    table->count = 0;
    
    return table;
}

/**
 * TODO 4: Implementați inserarea unui produs în hash table
 *
 * Pași:
 *   1. Verificați dacă produsul există deja (actualizați dacă da)
 *   2. Alocați memorie pentru noul produs
 *   3. Copiați datele în structură
 *   4. Inserați la începutul bucket-ului (chaining)
 *   5. Incrementați contorul
 *
 * @param table Hash table-ul
 * @param code Cod produs (maxim MAX_CODE_LEN-1 caractere)
 * @param name Denumire produs
 * @param price Preț unitar
 * @param stock Cantitate în stoc
 * @return true dacă inserarea a reușit, false altfel
 */
bool insert_product(ProductHashTable *table, const char *code,
                    const char *name, double price, int stock) {
    if (!table || !code || !name) return false;
    
    /* YOUR CODE HERE */
    
    return false;  /* Înlocuiți cu return true după implementare */
}

/**
 * TODO 5: Implementați căutarea unui produs în hash table
 *
 * Pași:
 *   1. Calculați hash-ul codului
 *   2. Parcurgeți lista din bucket-ul corespunzător
 *   3. Comparați codurile folosind strcmp
 *   4. Returnați produsul găsit sau NULL
 *
 * @param table Hash table-ul
 * @param code Codul produsului căutat
 * @return Pointer la produs sau NULL dacă nu există
 */
Product *find_product(ProductHashTable *table, const char *code) {
    if (!table || !code) return NULL;
    
    /* YOUR CODE HERE */
    
    return NULL;
}

/**
 * Eliberează memoria pentru hash table și toate produsele
 */
void free_product_table(ProductHashTable *table) {
    if (!table) return;
    
    for (int i = 0; i < HASH_SIZE; i++) {
        Product *current = table->buckets[i];
        while (current) {
            Product *temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(table);
}

/* =============================================================================
 * IMPLEMENTARE: FUNCȚII PENTRU COMENZI
 * =============================================================================
 */

/**
 * Creează o nouă comandă
 */
Order *create_order(int id, const char *customer, int priority) {
    Order *order = malloc(sizeof(Order));
    if (!order) return NULL;
    
    order->id = id;
    strncpy(order->customer, customer, MAX_NAME_LEN - 1);
    order->customer[MAX_NAME_LEN - 1] = '\0';
    order->items = NULL;
    order->item_count = 0;
    order->total = 0.0;
    order->priority = (priority < 1) ? 1 : (priority > 10) ? 10 : priority;
    order->created_at = time(NULL);
    order->left = order->right = NULL;
    
    return order;
}

/**
 * TODO 6: Implementați adăugarea unui produs la o comandă
 *
 * Pași:
 *   1. Găsiți produsul în hash table
 *   2. Verificați dacă există suficient stoc
 *   3. Alocați un nou OrderItem
 *   4. Completați câmpurile (produs, cantitate)
 *   5. Adăugați la începutul listei de items
 *   6. Actualizați stocul produsului
 *   7. Incrementați item_count
 *
 * @param order Comanda la care se adaugă
 * @param products Hash table cu produse
 * @param product_code Codul produsului de adăugat
 * @param quantity Cantitatea dorită
 * @return true dacă adăugarea a reușit
 */
bool add_item_to_order(Order *order, ProductHashTable *products,
                       const char *product_code, int quantity) {
    if (!order || !products || !product_code || quantity <= 0) return false;
    
    /* YOUR CODE HERE */
    
    return false;
}

/**
 * TODO 7: Implementați calculul totalului unei comenzi
 *
 * Parcurgeți lista de items și calculați:
 *   total = Σ (item->quantity * item->product->price)
 *
 * @param order Comanda pentru care se calculează totalul
 */
void calculate_order_total(Order *order) {
    if (!order) return;
    
    /* YOUR CODE HERE */
    
}

/**
 * Afișează detaliile unei comenzi
 */
void print_order(const Order *order) {
    if (!order) return;
    
    printf(COLOR_BLUE "╔══════════════════════════════════════════════════════════╗\n");
    printf("║  Comanda #%-5d                                           ║\n", order->id);
    printf("╠══════════════════════════════════════════════════════════╣\n" COLOR_RESET);
    printf("  Client: %s\n", order->customer);
    printf("  Prioritate: %d/10 %s\n", order->priority,
           order->priority >= 8 ? COLOR_RED "[URGENT]" COLOR_RESET : "");
    printf("  Produse: %d\n", order->item_count);
    
    OrderItem *item = order->items;
    int idx = 1;
    while (item) {
        /* Verificare că structura OrderItem este completă */
        #ifdef ORDER_ITEM_COMPLETE
        printf("    %d. %s x%d @ %.2f RON = %.2f RON\n",
               idx++, item->product->name, item->quantity,
               item->product->price, item->quantity * item->product->price);
        #else
        printf("    %d. [Item - completați TODO 1]\n", idx++);
        #endif
        item = item->next;
    }
    
    printf(COLOR_GREEN "  TOTAL: %.2f RON\n" COLOR_RESET, order->total);
    printf(COLOR_BLUE "╚══════════════════════════════════════════════════════════╝\n" COLOR_RESET);
}

/**
 * Eliberează memoria pentru o comandă și toate itemurile sale
 */
void free_order(Order *order) {
    if (!order) return;
    
    OrderItem *item = order->items;
    while (item) {
        OrderItem *temp = item;
        item = item->next;
        free(temp);
    }
    free(order);
}

/* =============================================================================
 * IMPLEMENTARE: BINARY SEARCH TREE PENTRU COMENZI
 * =============================================================================
 */

/**
 * Creează un BST gol
 */
OrderBST *create_order_bst(void) {
    OrderBST *bst = malloc(sizeof(OrderBST));
    if (!bst) return NULL;
    
    bst->root = NULL;
    bst->count = 0;
    
    return bst;
}

/**
 * TODO 8: Implementați inserarea unei comenzi în BST
 *
 * Inserarea se face după ID-ul comenzii:
 *   - ID mai mic -> subarbore stâng
 *   - ID mai mare -> subarbore drept
 *   - ID egal -> nu se inserează (returnează root neschimbat)
 *
 * @param root Rădăcina curentă a subarborelui
 * @param new_order Comanda de inserat
 * @return Noua rădăcină a subarborelui
 *
 * Hint: Folosiți recursivitate
 */
Order *insert_order_bst(Order *root, Order *new_order) {
    if (!new_order) return root;
    
    /* YOUR CODE HERE */
    
    return root;  /* Înlocuiți cu implementarea corectă */
}

/**
 * TODO 9: Implementați căutarea unei comenzi în BST
 *
 * @param root Rădăcina BST-ului
 * @param id ID-ul comenzii căutate
 * @return Pointer la comanda găsită sau NULL
 */
Order *find_order_bst(Order *root, int id) {
    /* YOUR CODE HERE */
    
    return NULL;
}

/**
 * Parcurgere inorder a BST-ului
 */
void inorder_traversal(Order *root, void (*visit)(const Order*)) {
    if (!root || !visit) return;
    
    inorder_traversal(root->left, visit);
    visit(root);
    inorder_traversal(root->right, visit);
}

/**
 * Eliberează memoria pentru întreg BST-ul
 */
void free_order_bst(Order *root) {
    if (!root) return;
    
    free_order_bst(root->left);
    free_order_bst(root->right);
    free_order(root);
}

/* =============================================================================
 * IMPLEMENTARE: PRIORITY QUEUE (MIN-HEAP)
 * =============================================================================
 */

/**
 * Funcție helper: Calculează indexul părintelui
 */
static inline int parent(int i) { return (i - 1) / 2; }

/**
 * Funcție helper: Calculează indexul copilului stâng
 */
static inline int left_child(int i) { return 2 * i + 1; }

/**
 * Funcție helper: Calculează indexul copilului drept
 */
static inline int right_child(int i) { return 2 * i + 2; }

/**
 * Funcție helper: Compară două comenzi după prioritate
 * Prioritate mai mare = valoare heap mai mică (pentru min-heap inversat)
 */
static int compare_priority(const Order *a, const Order *b) {
    /* Prioritate mai mare = procesată prima */
    if (a->priority != b->priority) {
        return b->priority - a->priority;  /* Descrescător */
    }
    /* La prioritate egală, comanda mai veche are prioritate */
    return (int)(a->created_at - b->created_at);
}

/**
 * TODO 10: Implementați crearea unui heap pentru comenzi
 *
 * Pași:
 *   1. Alocați structura OrderHeap
 *   2. Alocați array-ul de pointeri (capacity elemente)
 *   3. Inițializați capacity și size
 *
 * @param capacity Capacitatea maximă a heap-ului
 * @return Pointer la heap-ul creat sau NULL
 */
OrderHeap *create_order_heap(int capacity) {
    if (capacity <= 0) return NULL;
    
    /* YOUR CODE HERE */
    
    return NULL;
}

/**
 * TODO 11: Implementați inserarea în heap (heap_insert)
 *
 * Pași:
 *   1. Verificați dacă heap-ul este plin
 *   2. Adăugați elementul la sfârșit (size)
 *   3. Heapify-up: swap cu părintele cât timp este necesar
 *   4. Incrementați size
 *
 * Folosiți compare_priority() pentru comparații
 *
 * @param heap Heap-ul
 * @param order Comanda de inserat
 * @return true dacă inserarea a reușit
 */
bool heap_insert(OrderHeap *heap, Order *order) {
    if (!heap || !order) return false;
    
    /* YOUR CODE HERE */
    
    return false;
}

/**
 * TODO 12: Implementați extragerea minimului din heap
 *
 * Pași:
 *   1. Verificați dacă heap-ul este gol
 *   2. Salvați elementul din rădăcină (index 0)
 *   3. Mutați ultimul element în rădăcină
 *   4. Decrementați size
 *   5. Heapify-down: swap cu copilul mai mic cât timp este necesar
 *   6. Returnați elementul salvat
 *
 * @param heap Heap-ul
 * @return Comanda cu cea mai mare prioritate sau NULL
 */
Order *heap_extract_min(OrderHeap *heap) {
    if (!heap) return NULL;
    
    /* YOUR CODE HERE */
    
    return NULL;
}

/**
 * Eliberează memoria pentru heap (nu și pentru comenzi!)
 */
void free_order_heap(OrderHeap *heap) {
    if (!heap) return;
    
    #ifdef ORDER_HEAP_COMPLETE
    free(heap->orders);
    #endif
    free(heap);
}

/* =============================================================================
 * FUNCȚII UTILITARE
 * =============================================================================
 */

/**
 * Încarcă produse dintr-un fișier text
 * Format: CODE NAME PRICE STOCK
 */
int load_products_from_file(ProductHashTable *table, const char *filename) {
    if (!table || !filename) return -1;
    
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, COLOR_RED "Eroare: Nu pot deschide fișierul %s\n" COLOR_RESET, 
                filename);
        return -1;
    }
    
    char line[MAX_LINE];
    int count = 0;
    
    while (fgets(line, sizeof(line), file)) {
        char code[MAX_CODE_LEN], name[MAX_NAME_LEN];
        double price;
        int stock;
        
        /* Parsare linie */
        if (sscanf(line, "%15s %63s %lf %d", code, name, &price, &stock) == 4) {
            if (insert_product(table, code, name, price, stock)) {
                count++;
            }
        }
    }
    
    fclose(file);
    return count;
}

/**
 * Afișează un separator decorativ
 */
void print_separator(const char *title) {
    printf("\n");
    printf(COLOR_YELLOW "╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║  %-60s ║\n", title ? title : "");
    printf("╚═══════════════════════════════════════════════════════════════╝\n" COLOR_RESET);
    printf("\n");
}

/* =============================================================================
 * FUNCȚII CU FUNCTION POINTERS (BONUS)
 * =============================================================================
 */

/**
 * Filtrează comenzi urgente (prioritate >= 8)
 */
bool filter_urgent(const Order *order) {
    return order && order->priority >= 8;
}

/**
 * Filtrează comenzi mari (total > 1000)
 */
bool filter_large(const Order *order) {
    return order && order->total > 1000.0;
}

/**
 * Compară comenzi după total (pentru qsort)
 */
int compare_by_total(const void *a, const void *b) {
    const Order *oa = *(const Order**)a;
    const Order *ob = *(const Order**)b;
    
    if (oa->total < ob->total) return -1;
    if (oa->total > ob->total) return 1;
    return 0;
}

/* =============================================================================
 * PROGRAM PRINCIPAL
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf(COLOR_GREEN "╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     SĂPTĂMÂNA 14: SISTEM DE GESTIUNE COMENZI                  ║\n");
    printf("║     Exercițiul 1 - Integrare Structuri de Date                ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n" COLOR_RESET);
    
    /* ================================================================
     * PARTEA 1: Inițializare Hash Table pentru Produse
     * ================================================================ */
    print_separator("PARTEA 1: INIȚIALIZARE PRODUSE");
    
    ProductHashTable *products = create_product_table();
    if (!products) {
        fprintf(stderr, "Eroare la crearea hash table-ului!\n");
        return 1;
    }
    
    /* Încercăm să încărcăm din fișier, altfel adăugăm manual */
    int loaded = load_products_from_file(products, "data/products.txt");
    
    if (loaded <= 0) {
        printf("Fișierul nu există, adăugăm produse manual...\n");
        
        /* Adăugare manuală de produse pentru testare */
        insert_product(products, "P001", "Laptop_Gaming", 4500.00, 10);
        insert_product(products, "P002", "Mouse_Wireless", 89.99, 50);
        insert_product(products, "P003", "Tastatura_Mecanica", 299.99, 25);
        insert_product(products, "P004", "Monitor_27inch", 1200.00, 15);
        insert_product(products, "P005", "SSD_1TB", 450.00, 30);
        insert_product(products, "P006", "RAM_16GB", 320.00, 40);
        insert_product(products, "P007", "Casti_Gaming", 180.00, 20);
        insert_product(products, "P008", "Webcam_HD", 150.00, 35);
        
        printf(COLOR_GREEN "✓ Adăugate 8 produse manual\n" COLOR_RESET);
    } else {
        printf(COLOR_GREEN "✓ Încărcate %d produse din fișier\n" COLOR_RESET, loaded);
    }
    
    /* Test căutare produs */
    printf("\nTest căutare produs P001:\n");
    Product *p = find_product(products, "P001");
    if (p) {
        printf("  Găsit: %s - %.2f RON (stoc: %d)\n", p->name, p->price, p->stock);
    } else {
        printf(COLOR_RED "  Produsul nu a fost găsit. Verificați TODO 3-5.\n" COLOR_RESET);
    }
    
    /* ================================================================
     * PARTEA 2: Creare și Populare Comenzi
     * ================================================================ */
    print_separator("PARTEA 2: CREARE COMENZI");
    
    /* Creare BST pentru comenzi */
    OrderBST *order_tree = create_order_bst();
    if (!order_tree) {
        fprintf(stderr, "Eroare la crearea BST-ului!\n");
        free_product_table(products);
        return 1;
    }
    
    /* Creare comenzi de test */
    Order *order1 = create_order(1001, "Ion Popescu", 5);
    Order *order2 = create_order(1002, "Maria Ionescu", 9);  /* Urgent */
    Order *order3 = create_order(1003, "Andrei Georgescu", 3);
    Order *order4 = create_order(1004, "Elena Vasilescu", 10); /* Foarte urgent */
    
    /* Adăugare produse la comenzi */
    printf("Adăugare produse la comenzi...\n");
    
    add_item_to_order(order1, products, "P001", 1);  /* Laptop */
    add_item_to_order(order1, products, "P002", 2);  /* 2x Mouse */
    calculate_order_total(order1);
    
    add_item_to_order(order2, products, "P004", 2);  /* 2x Monitor */
    add_item_to_order(order2, products, "P005", 3);  /* 3x SSD */
    calculate_order_total(order2);
    
    add_item_to_order(order3, products, "P003", 1);  /* Tastatura */
    add_item_to_order(order3, products, "P007", 1);  /* Casti */
    calculate_order_total(order3);
    
    add_item_to_order(order4, products, "P006", 4);  /* 4x RAM */
    add_item_to_order(order4, products, "P008", 2);  /* 2x Webcam */
    calculate_order_total(order4);
    
    /* Inserare în BST */
    order_tree->root = insert_order_bst(order_tree->root, order1);
    order_tree->root = insert_order_bst(order_tree->root, order2);
    order_tree->root = insert_order_bst(order_tree->root, order3);
    order_tree->root = insert_order_bst(order_tree->root, order4);
    order_tree->count = 4;
    
    printf(COLOR_GREEN "✓ Create și inserate %d comenzi în BST\n" COLOR_RESET, 
           order_tree->count);
    
    /* ================================================================
     * PARTEA 3: Afișare Comenzi (Inorder Traversal)
     * ================================================================ */
    print_separator("PARTEA 3: TOATE COMENZILE (INORDER)");
    
    printf("Parcurgere inorder a BST-ului (ordonate după ID):\n");
    inorder_traversal(order_tree->root, print_order);
    
    /* ================================================================
     * PARTEA 4: Căutare în BST
     * ================================================================ */
    print_separator("PARTEA 4: CĂUTARE COMANDĂ");
    
    int search_id = 1002;
    printf("Căutare comandă #%d în BST...\n", search_id);
    
    Order *found = find_order_bst(order_tree->root, search_id);
    if (found) {
        printf(COLOR_GREEN "✓ Găsită!\n" COLOR_RESET);
        print_order(found);
    } else {
        printf(COLOR_RED "✗ Comanda nu a fost găsită. Verificați TODO 9.\n" COLOR_RESET);
    }
    
    /* ================================================================
     * PARTEA 5: Priority Queue pentru Comenzi Urgente
     * ================================================================ */
    print_separator("PARTEA 5: PROCESARE PRIORITARĂ (HEAP)");
    
    OrderHeap *priority_queue = create_order_heap(10);
    
    if (priority_queue) {
        /* Adăugăm toate comenzile în heap */
        heap_insert(priority_queue, order1);
        heap_insert(priority_queue, order2);
        heap_insert(priority_queue, order3);
        heap_insert(priority_queue, order4);
        
        printf("Procesare comenzi în ordinea priorității:\n\n");
        
        Order *next;
        int processing_order = 1;
        while ((next = heap_extract_min(priority_queue)) != NULL) {
            printf("%d. Procesare comanda #%d (prioritate %d) - %s\n",
                   processing_order++, next->id, next->priority, next->customer);
        }
        
        free_order_heap(priority_queue);
    } else {
        printf(COLOR_YELLOW "Heap neimplementat. Completați TODO 10-12.\n" COLOR_RESET);
    }
    
    /* ================================================================
     * PARTEA 6: Statistici Finale
     * ================================================================ */
    print_separator("PARTEA 6: STATISTICI");
    
    printf("Statistici sistem:\n");
    printf("  • Produse în inventar: %d\n", products->count);
    printf("  • Comenzi în sistem: %d\n", order_tree->count);
    printf("  • Valoare totală comenzi: %.2f RON\n",
           order1->total + order2->total + order3->total + order4->total);
    
    /* ================================================================
     * CURĂȚARE MEMORIE
     * ================================================================ */
    print_separator("CURĂȚARE MEMORIE");
    
    printf("Eliberare memorie...\n");
    free_order_bst(order_tree->root);
    free(order_tree);
    free_product_table(products);
    
    printf(COLOR_GREEN "✓ Toată memoria a fost eliberată.\n" COLOR_RESET);
    printf(COLOR_GREEN "✓ Rulați cu Valgrind pentru verificare!\n" COLOR_RESET);
    
    return 0;
}

/* =============================================================================
 * PROVOCĂRI BONUS (Opțional)
 * =============================================================================
 *
 * 1. Implementați funcția de ștergere din BST (delete_order_bst)
 *
 * 2. Adăugați funcție de căutare după client în BST
 *    (parcurgere completă cu filtrare)
 *
 * 3. Implementați persistența datelor:
 *    - save_orders_to_file(OrderBST *tree, const char *filename)
 *    - load_orders_from_file(OrderBST *tree, const char *filename)
 *
 * 4. Adăugați statistici avansate:
 *    - Produsul cel mai vândut
 *    - Clientul cu cele mai multe comenzi
 *    - Media valorii comenzilor
 *
 * 5. Implementați un sistem de reduceri:
 *    - 5% pentru comenzi > 1000 RON
 *    - 10% pentru comenzi > 5000 RON
 *
 * =============================================================================
 */
