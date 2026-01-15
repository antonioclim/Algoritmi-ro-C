/**
 * =============================================================================
 * SOLUȚIE EXERCIȚIUL 1: Sistem de Gestiune Comenzi
 * Săptămâna 14 - Recapitulare Avansată și Optimizare
 * =============================================================================
 *
 * ATENȚIE: Acest fișier conține soluția completă.
 * Folosiți-l doar pentru verificare după ce ați încercat singuri!
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
#define HASH_SIZE 101
#define MAX_LINE 256

#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_RESET   "\x1b[0m"

/* =============================================================================
 * STRUCTURI DE DATE
 * =============================================================================
 */

typedef struct Product {
    char code[MAX_CODE_LEN];
    char name[MAX_NAME_LEN];
    double price;
    int stock;
    struct Product *next;
} Product;

typedef struct {
    Product *buckets[HASH_SIZE];
    int count;
} ProductHashTable;

/* SOLUȚIE TODO 1: Structura OrderItem completă */
typedef struct OrderItem {
    Product *product;           /* Pointer către produsul comandat */
    int quantity;               /* Cantitatea comandată */
    struct OrderItem *next;     /* Pointer către următorul item */
} OrderItem;

typedef struct Order {
    int id;
    char customer[MAX_NAME_LEN];
    OrderItem *items;
    int item_count;
    double total;
    int priority;
    time_t created_at;
    struct Order *left;
    struct Order *right;
} Order;

typedef struct {
    Order *root;
    int count;
} OrderBST;

/* SOLUȚIE TODO 2: Structura OrderHeap completă */
typedef struct {
    Order **orders;             /* Array de pointeri către comenzi */
    int capacity;               /* Capacitatea maximă */
    int size;                   /* Dimensiunea curentă */
} OrderHeap;

/* =============================================================================
 * DECLARAȚII FUNCȚII
 * =============================================================================
 */

unsigned int hash_function(const char *key);
ProductHashTable *create_product_table(void);
bool insert_product(ProductHashTable *table, const char *code, 
                    const char *name, double price, int stock);
Product *find_product(ProductHashTable *table, const char *code);
void free_product_table(ProductHashTable *table);

Order *create_order(int id, const char *customer, int priority);
bool add_item_to_order(Order *order, ProductHashTable *products,
                       const char *product_code, int quantity);
void calculate_order_total(Order *order);
void print_order(const Order *order);
void free_order(Order *order);

OrderBST *create_order_bst(void);
Order *insert_order_bst(Order *root, Order *new_order);
Order *find_order_bst(Order *root, int id);
void inorder_traversal(Order *root, void (*visit)(const Order*));
void free_order_bst(Order *root);

OrderHeap *create_order_heap(int capacity);
bool heap_insert(OrderHeap *heap, Order *order);
Order *heap_extract_min(OrderHeap *heap);
void free_order_heap(OrderHeap *heap);

int load_products_from_file(ProductHashTable *table, const char *filename);
void print_separator(const char *title);

/* =============================================================================
 * IMPLEMENTARE HASH TABLE
 * =============================================================================
 */

/* SOLUȚIE TODO 3: Funcția hash djb2 */
unsigned int hash_function(const char *key) {
    unsigned int hash = 5381;
    int c;
    
    while ((c = *key++)) {
        hash = hash * 33 + c;  /* Sau: hash = ((hash << 5) + hash) + c */
    }
    
    return hash % HASH_SIZE;
}

ProductHashTable *create_product_table(void) {
    ProductHashTable *table = malloc(sizeof(ProductHashTable));
    if (!table) return NULL;
    
    for (int i = 0; i < HASH_SIZE; i++) {
        table->buckets[i] = NULL;
    }
    table->count = 0;
    
    return table;
}

/* SOLUȚIE TODO 4: Inserare în hash table */
bool insert_product(ProductHashTable *table, const char *code,
                    const char *name, double price, int stock) {
    if (!table || !code || !name) return false;
    
    /* Verificăm dacă produsul există deja */
    Product *existing = find_product(table, code);
    if (existing) {
        /* Actualizăm produsul existent */
        strncpy(existing->name, name, MAX_NAME_LEN - 1);
        existing->name[MAX_NAME_LEN - 1] = '\0';
        existing->price = price;
        existing->stock = stock;
        return true;
    }
    
    /* Alocăm memorie pentru noul produs */
    Product *new_product = malloc(sizeof(Product));
    if (!new_product) return false;
    
    /* Copiăm datele */
    strncpy(new_product->code, code, MAX_CODE_LEN - 1);
    new_product->code[MAX_CODE_LEN - 1] = '\0';
    strncpy(new_product->name, name, MAX_NAME_LEN - 1);
    new_product->name[MAX_NAME_LEN - 1] = '\0';
    new_product->price = price;
    new_product->stock = stock;
    
    /* Calculăm indexul și inserăm la începutul bucket-ului */
    unsigned int index = hash_function(code);
    new_product->next = table->buckets[index];
    table->buckets[index] = new_product;
    table->count++;
    
    return true;
}

/* SOLUȚIE TODO 5: Căutare în hash table */
Product *find_product(ProductHashTable *table, const char *code) {
    if (!table || !code) return NULL;
    
    unsigned int index = hash_function(code);
    Product *current = table->buckets[index];
    
    while (current) {
        if (strcmp(current->code, code) == 0) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

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
 * IMPLEMENTARE FUNCȚII COMENZI
 * =============================================================================
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

/* SOLUȚIE TODO 6: Adăugare item la comandă */
bool add_item_to_order(Order *order, ProductHashTable *products,
                       const char *product_code, int quantity) {
    if (!order || !products || !product_code || quantity <= 0) return false;
    
    /* Găsim produsul în hash table */
    Product *product = find_product(products, product_code);
    if (!product) {
        fprintf(stderr, "Produs negăsit: %s\n", product_code);
        return false;
    }
    
    /* Verificăm stocul */
    if (product->stock < quantity) {
        fprintf(stderr, "Stoc insuficient pentru %s (cerut: %d, disponibil: %d)\n",
                product_code, quantity, product->stock);
        return false;
    }
    
    /* Alocăm noul OrderItem */
    OrderItem *item = malloc(sizeof(OrderItem));
    if (!item) return false;
    
    /* Completăm câmpurile */
    item->product = product;
    item->quantity = quantity;
    
    /* Adăugăm la începutul listei */
    item->next = order->items;
    order->items = item;
    
    /* Actualizăm stocul și contorul */
    product->stock -= quantity;
    order->item_count++;
    
    return true;
}

/* SOLUȚIE TODO 7: Calculul totalului */
void calculate_order_total(Order *order) {
    if (!order) return;
    
    order->total = 0.0;
    OrderItem *item = order->items;
    
    while (item) {
        order->total += item->quantity * item->product->price;
        item = item->next;
    }
}

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
        printf("    %d. %s x%d @ %.2f RON = %.2f RON\n",
               idx++, item->product->name, item->quantity,
               item->product->price, item->quantity * item->product->price);
        item = item->next;
    }
    
    printf(COLOR_GREEN "  TOTAL: %.2f RON\n" COLOR_RESET, order->total);
    printf(COLOR_BLUE "╚══════════════════════════════════════════════════════════╝\n" COLOR_RESET);
}

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
 * IMPLEMENTARE BST
 * =============================================================================
 */

OrderBST *create_order_bst(void) {
    OrderBST *bst = malloc(sizeof(OrderBST));
    if (!bst) return NULL;
    
    bst->root = NULL;
    bst->count = 0;
    
    return bst;
}

/* SOLUȚIE TODO 8: Inserare în BST */
Order *insert_order_bst(Order *root, Order *new_order) {
    if (!new_order) return root;
    
    /* Caz de bază: am găsit locul */
    if (!root) {
        return new_order;
    }
    
    /* Recursivitate */
    if (new_order->id < root->id) {
        root->left = insert_order_bst(root->left, new_order);
    } else if (new_order->id > root->id) {
        root->right = insert_order_bst(root->right, new_order);
    }
    /* Dacă ID-ul există deja, nu inserăm */
    
    return root;
}

/* SOLUȚIE TODO 9: Căutare în BST */
Order *find_order_bst(Order *root, int id) {
    if (!root) return NULL;
    
    if (id == root->id) {
        return root;
    } else if (id < root->id) {
        return find_order_bst(root->left, id);
    } else {
        return find_order_bst(root->right, id);
    }
}

void inorder_traversal(Order *root, void (*visit)(const Order*)) {
    if (!root || !visit) return;
    
    inorder_traversal(root->left, visit);
    visit(root);
    inorder_traversal(root->right, visit);
}

void free_order_bst(Order *root) {
    if (!root) return;
    
    free_order_bst(root->left);
    free_order_bst(root->right);
    free_order(root);
}

/* =============================================================================
 * IMPLEMENTARE HEAP
 * =============================================================================
 */

static inline int parent(int i) { return (i - 1) / 2; }
static inline int left_child(int i) { return 2 * i + 1; }
static inline int right_child(int i) { return 2 * i + 2; }

static int compare_priority(const Order *a, const Order *b) {
    if (a->priority != b->priority) {
        return b->priority - a->priority;
    }
    return (int)(a->created_at - b->created_at);
}

/* SOLUȚIE TODO 10: Creare heap */
OrderHeap *create_order_heap(int capacity) {
    if (capacity <= 0) return NULL;
    
    OrderHeap *heap = malloc(sizeof(OrderHeap));
    if (!heap) return NULL;
    
    heap->orders = malloc(capacity * sizeof(Order*));
    if (!heap->orders) {
        free(heap);
        return NULL;
    }
    
    heap->capacity = capacity;
    heap->size = 0;
    
    return heap;
}

/* SOLUȚIE TODO 11: Inserare în heap (heap_insert) */
bool heap_insert(OrderHeap *heap, Order *order) {
    if (!heap || !order) return false;
    
    /* Verificăm dacă heap-ul este plin */
    if (heap->size >= heap->capacity) {
        return false;
    }
    
    /* Adăugăm elementul la sfârșit */
    int i = heap->size;
    heap->orders[i] = order;
    heap->size++;
    
    /* Heapify-up */
    while (i > 0 && compare_priority(heap->orders[i], heap->orders[parent(i)]) < 0) {
        /* Swap */
        Order *temp = heap->orders[i];
        heap->orders[i] = heap->orders[parent(i)];
        heap->orders[parent(i)] = temp;
        
        i = parent(i);
    }
    
    return true;
}

/* SOLUȚIE TODO 12: Extragere din heap */
Order *heap_extract_min(OrderHeap *heap) {
    if (!heap || heap->size == 0) return NULL;
    
    /* Salvăm elementul din rădăcină */
    Order *min = heap->orders[0];
    
    /* Mutăm ultimul element în rădăcină */
    heap->size--;
    heap->orders[0] = heap->orders[heap->size];
    
    /* Heapify-down */
    int i = 0;
    while (true) {
        int smallest = i;
        int left = left_child(i);
        int right = right_child(i);
        
        if (left < heap->size && 
            compare_priority(heap->orders[left], heap->orders[smallest]) < 0) {
            smallest = left;
        }
        
        if (right < heap->size && 
            compare_priority(heap->orders[right], heap->orders[smallest]) < 0) {
            smallest = right;
        }
        
        if (smallest == i) break;
        
        /* Swap */
        Order *temp = heap->orders[i];
        heap->orders[i] = heap->orders[smallest];
        heap->orders[smallest] = temp;
        
        i = smallest;
    }
    
    return min;
}

void free_order_heap(OrderHeap *heap) {
    if (!heap) return;
    free(heap->orders);
    free(heap);
}

/* =============================================================================
 * FUNCȚII UTILITARE
 * =============================================================================
 */

int load_products_from_file(ProductHashTable *table, const char *filename) {
    if (!table || !filename) return -1;
    
    FILE *file = fopen(filename, "r");
    if (!file) {
        return -1;
    }
    
    char line[MAX_LINE];
    int count = 0;
    
    while (fgets(line, sizeof(line), file)) {
        char code[MAX_CODE_LEN], name[MAX_NAME_LEN];
        double price;
        int stock;
        
        if (sscanf(line, "%15s %63s %lf %d", code, name, &price, &stock) == 4) {
            if (insert_product(table, code, name, price, stock)) {
                count++;
            }
        }
    }
    
    fclose(file);
    return count;
}

void print_separator(const char *title) {
    printf("\n");
    printf(COLOR_YELLOW "╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║  %-60s ║\n", title ? title : "");
    printf("╚═══════════════════════════════════════════════════════════════╝\n" COLOR_RESET);
    printf("\n");
}

/* =============================================================================
 * PROGRAM PRINCIPAL
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf(COLOR_GREEN "╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     SĂPTĂMÂNA 14: SISTEM DE GESTIUNE COMENZI                  ║\n");
    printf("║     SOLUȚIE COMPLETĂ                                          ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n" COLOR_RESET);
    
    /* Partea 1: Inițializare Hash Table */
    print_separator("PARTEA 1: INIȚIALIZARE PRODUSE");
    
    ProductHashTable *products = create_product_table();
    if (!products) {
        fprintf(stderr, "Eroare la crearea hash table-ului!\n");
        return 1;
    }
    
    int loaded = load_products_from_file(products, "data/products.txt");
    
    if (loaded <= 0) {
        printf("Adăugăm produse manual...\n");
        insert_product(products, "P001", "Laptop_Gaming", 4500.00, 10);
        insert_product(products, "P002", "Mouse_Wireless", 89.99, 50);
        insert_product(products, "P003", "Tastatura_Mecanica", 299.99, 25);
        insert_product(products, "P004", "Monitor_27inch", 1200.00, 15);
        insert_product(products, "P005", "SSD_1TB", 450.00, 30);
        insert_product(products, "P006", "RAM_16GB", 320.00, 40);
        insert_product(products, "P007", "Casti_Gaming", 180.00, 20);
        insert_product(products, "P008", "Webcam_HD", 150.00, 35);
        printf(COLOR_GREEN "✓ Adăugate 8 produse\n" COLOR_RESET);
    } else {
        printf(COLOR_GREEN "✓ Încărcate %d produse din fișier\n" COLOR_RESET, loaded);
    }
    
    /* Test căutare */
    Product *p = find_product(products, "P001");
    if (p) {
        printf("\nTest căutare P001: %s - %.2f RON (stoc: %d)\n", 
               p->name, p->price, p->stock);
    }
    
    /* Partea 2: Creare Comenzi */
    print_separator("PARTEA 2: CREARE COMENZI");
    
    OrderBST *order_tree = create_order_bst();
    
    Order *order1 = create_order(1001, "Ion Popescu", 5);
    Order *order2 = create_order(1002, "Maria Ionescu", 9);
    Order *order3 = create_order(1003, "Andrei Georgescu", 3);
    Order *order4 = create_order(1004, "Elena Vasilescu", 10);
    
    add_item_to_order(order1, products, "P001", 1);
    add_item_to_order(order1, products, "P002", 2);
    calculate_order_total(order1);
    
    add_item_to_order(order2, products, "P004", 2);
    add_item_to_order(order2, products, "P005", 3);
    calculate_order_total(order2);
    
    add_item_to_order(order3, products, "P003", 1);
    add_item_to_order(order3, products, "P007", 1);
    calculate_order_total(order3);
    
    add_item_to_order(order4, products, "P006", 4);
    add_item_to_order(order4, products, "P008", 2);
    calculate_order_total(order4);
    
    order_tree->root = insert_order_bst(order_tree->root, order1);
    order_tree->root = insert_order_bst(order_tree->root, order2);
    order_tree->root = insert_order_bst(order_tree->root, order3);
    order_tree->root = insert_order_bst(order_tree->root, order4);
    order_tree->count = 4;
    
    printf(COLOR_GREEN "✓ Create %d comenzi\n" COLOR_RESET, order_tree->count);
    
    /* Partea 3: Afișare Comenzi */
    print_separator("PARTEA 3: TOATE COMENZILE (INORDER)");
    inorder_traversal(order_tree->root, print_order);
    
    /* Partea 4: Căutare */
    print_separator("PARTEA 4: CĂUTARE COMANDĂ");
    Order *found = find_order_bst(order_tree->root, 1002);
    if (found) {
        printf(COLOR_GREEN "✓ Găsită comanda #1002\n" COLOR_RESET);
        print_order(found);
    }
    
    /* Partea 5: Priority Queue */
    print_separator("PARTEA 5: PROCESARE PRIORITARĂ (HEAP)");
    
    /* Resetăm pointerii BST pentru heap (ele nu sunt distruse) */
    OrderHeap *pq = create_order_heap(10);
    heap_insert(pq, order1);
    heap_insert(pq, order2);
    heap_insert(pq, order3);
    heap_insert(pq, order4);
    
    printf("Procesare comenzi în ordinea priorității:\n\n");
    
    Order *next;
    int proc_order = 1;
    while ((next = heap_extract_min(pq)) != NULL) {
        printf("%d. Comanda #%d (prioritate %d) - %s - %.2f RON\n",
               proc_order++, next->id, next->priority, next->customer, next->total);
    }
    
    free_order_heap(pq);
    
    /* Partea 6: Statistici */
    print_separator("PARTEA 6: STATISTICI");
    
    double total_value = order1->total + order2->total + order3->total + order4->total;
    printf("  Produse în inventar: %d\n", products->count);
    printf("  Comenzi în sistem: %d\n", order_tree->count);
    printf("  Valoare totală: %.2f RON\n", total_value);
    
    /* Curățare */
    print_separator("CURĂȚARE MEMORIE");
    
    free_order_bst(order_tree->root);
    free(order_tree);
    free_product_table(products);
    
    printf(COLOR_GREEN "✓ Memorie eliberată corect.\n" COLOR_RESET);
    
    return 0;
}
