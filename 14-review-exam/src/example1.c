/**
 * =============================================================================
 * SĂPTĂMÂNA 14: RECAPITULARE AVANSATĂ ȘI OPTIMIZARE
 * Exemplu Complet - Integrare Structuri de Date și Algoritmi
 * =============================================================================
 *
 * Acest exemplu demonstrează:
 *   1. Comparația complexității diferitelor structuri de date
 *   2. Implementarea eficientă a structurilor fundamentale
 *   3. Alegerea optimă a structurii pentru diferite scenarii
 *   4. Tehnici de optimizare și best practices
 *   5. Debugging și profiling în C
 *   6. Patterns frecvente în interviuri tehnice
 *
 * Structuri demonstrate:
 *   - Arrays (cu qsort și bsearch)
 *   - Linked Lists
 *   - Stacks și Queues
 *   - Binary Search Trees (BST)
 *   - Hash Tables
 *   - Graphs (BFS, DFS)
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -O2 -o example1 example1.c -lm
 * Utilizare: ./example1
 *
 * Autor: ATP Course - ASE București
 * Versiune: 1.0 - Ianuarie 2026
 *
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

#define MAX_NAME_LEN 64
#define HASH_TABLE_SIZE 101
#define MAX_VERTICES 20
#define BENCHMARK_SIZE 10000

/* Coduri ANSI pentru output colorat */
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"

/* =============================================================================
 * STRUCTURI DE DATE
 * =============================================================================
 */

/* Structură generică pentru benchmark */
typedef struct {
    char name[32];
    int id;
    double score;
} Record;

/* Nod pentru Linked List */
typedef struct ListNode {
    int data;
    struct ListNode* next;
} ListNode;

/* Nod pentru Binary Search Tree */
typedef struct BSTNode {
    int key;
    char value[MAX_NAME_LEN];
    struct BSTNode* left;
    struct BSTNode* right;
} BSTNode;

/* Intrare în Hash Table (separate chaining) */
typedef struct HashEntry {
    char key[MAX_NAME_LEN];
    int value;
    struct HashEntry* next;
} HashEntry;

/* Hash Table */
typedef struct {
    HashEntry* buckets[HASH_TABLE_SIZE];
    int size;
} HashTable;

/* Stack implementat cu array */
typedef struct {
    int* data;
    int top;
    int capacity;
} Stack;

/* Queue implementată cu array circular */
typedef struct {
    int* data;
    int front;
    int rear;
    int size;
    int capacity;
} Queue;

/* Graf reprezentat cu listă de adiacență */
typedef struct {
    int adj[MAX_VERTICES][MAX_VERTICES];
    int vertices;
} Graph;

/* =============================================================================
 * FUNCȚII AUXILIARE PENTRU OUTPUT
 * =============================================================================
 */

static void print_header(const char* title) {
    printf("\n");
    printf(CYAN BOLD "╔═══════════════════════════════════════════════════════════════╗\n" RESET);
    printf(CYAN BOLD "║  %-61s║\n" RESET, title);
    printf(CYAN BOLD "╚═══════════════════════════════════════════════════════════════╝\n" RESET);
    printf("\n");
}

static void print_subheader(const char* title) {
    printf(YELLOW "── %s ──\n" RESET, title);
}

static void print_success(const char* msg) {
    printf(GREEN "  ✓ %s\n" RESET, msg);
}

static void print_info(const char* msg) {
    printf(BLUE "  ℹ %s\n" RESET, msg);
}

/* =============================================================================
 * PARTEA 1: ARRAYS - qsort și bsearch
 * =============================================================================
 */

/* Funcție de comparare pentru qsort (întregi) */
static int compare_int(const void* a, const void* b) {
    return (*(const int*)a - *(const int*)b);
}

/* Funcție de comparare pentru Record după score */
static int compare_record_by_score(const void* a, const void* b) {
    const Record* ra = (const Record*)a;
    const Record* rb = (const Record*)b;
    if (ra->score < rb->score) return -1;
    if (ra->score > rb->score) return 1;
    return 0;
}

/* Funcție de comparare pentru Record după nume */
static int compare_record_by_name(const void* a, const void* b) {
    const Record* ra = (const Record*)a;
    const Record* rb = (const Record*)b;
    return strcmp(ra->name, rb->name);
}

static void demo_arrays(void) {
    print_header("PARTEA 1: Arrays cu qsort și bsearch");
    
    /* Demonstrație sortare întregi */
    print_subheader("Sortare array de întregi");
    int numbers[] = {64, 25, 12, 22, 11, 90, 45, 33, 78, 54};
    int n = sizeof(numbers) / sizeof(numbers[0]);
    
    printf("  Array original:  ");
    for (int i = 0; i < n; i++) printf("%d ", numbers[i]);
    printf("\n");
    
    qsort(numbers, n, sizeof(int), compare_int);
    
    printf("  Array sortat:    ");
    for (int i = 0; i < n; i++) printf("%d ", numbers[i]);
    printf("\n");
    print_success("Sortare realizată cu qsort în O(n log n)");
    
    /* Demonstrație căutare binară */
    print_subheader("Căutare binară cu bsearch");
    int key = 45;
    int* found = bsearch(&key, numbers, n, sizeof(int), compare_int);
    if (found) {
        printf("  Elementul %d găsit la adresa %p\n", key, (void*)found);
        printf("  Index în array: %ld\n", found - numbers);
    }
    print_success("Căutare realizată cu bsearch în O(log n)");
    
    /* Demonstrație sortare structuri */
    print_subheader("Sortare structuri complexe");
    Record students[] = {
        {"Popescu Ion", 101, 8.75},
        {"Ionescu Maria", 102, 9.25},
        {"Georgescu Ana", 103, 7.50},
        {"Dumitrescu Vlad", 104, 9.00},
        {"Constantinescu Elena", 105, 8.25}
    };
    int num_students = sizeof(students) / sizeof(students[0]);
    
    printf("\n  Studenți sortați după scor:\n");
    qsort(students, num_students, sizeof(Record), compare_record_by_score);
    for (int i = 0; i < num_students; i++) {
        printf("    %d. %-25s %.2f\n", i + 1, students[i].name, students[i].score);
    }
    
    printf("\n  Studenți sortați alfabetic:\n");
    qsort(students, num_students, sizeof(Record), compare_record_by_name);
    for (int i = 0; i < num_students; i++) {
        printf("    %d. %-25s %.2f\n", i + 1, students[i].name, students[i].score);
    }
    print_success("Sortare multi-criteriu demonstrată");
}

/* =============================================================================
 * PARTEA 2: LINKED LISTS
 * =============================================================================
 */

static ListNode* list_create_node(int data) {
    ListNode* node = malloc(sizeof(ListNode));
    if (node) {
        node->data = data;
        node->next = NULL;
    }
    return node;
}

static void list_insert_front(ListNode** head, int data) {
    ListNode* node = list_create_node(data);
    if (node) {
        node->next = *head;
        *head = node;
    }
}

static void list_insert_sorted(ListNode** head, int data) {
    ListNode* node = list_create_node(data);
    if (!node) return;
    
    if (*head == NULL || (*head)->data >= data) {
        node->next = *head;
        *head = node;
        return;
    }
    
    ListNode* current = *head;
    while (current->next && current->next->data < data) {
        current = current->next;
    }
    node->next = current->next;
    current->next = node;
}

static ListNode* list_reverse(ListNode* head) {
    ListNode* prev = NULL;
    ListNode* current = head;
    ListNode* next = NULL;
    
    while (current) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    return prev;
}

static void list_print(ListNode* head, const char* label) {
    printf("  %s: ", label);
    ListNode* current = head;
    while (current) {
        printf("%d", current->data);
        if (current->next) printf(" → ");
        current = current->next;
    }
    printf(" → NULL\n");
}

static void list_free(ListNode* head) {
    while (head) {
        ListNode* temp = head;
        head = head->next;
        free(temp);
    }
}

static void demo_linked_list(void) {
    print_header("PARTEA 2: Linked Lists");
    
    ListNode* list = NULL;
    
    /* Inserare la început */
    print_subheader("Inserare la început O(1)");
    for (int i = 5; i >= 1; i--) {
        list_insert_front(&list, i * 10);
    }
    list_print(list, "Listă    ");
    print_success("5 noduri inserate la început");
    
    /* Inserare sortată */
    print_subheader("Inserare sortată O(n)");
    list_free(list);
    list = NULL;
    int values[] = {30, 10, 50, 20, 40};
    for (int i = 0; i < 5; i++) {
        list_insert_sorted(&list, values[i]);
    }
    list_print(list, "Sortat   ");
    print_success("Lista menține ordinea la inserare");
    
    /* Inversare listă */
    print_subheader("Inversare listă O(n)");
    list = list_reverse(list);
    list_print(list, "Inversat ");
    print_success("Lista inversată in-place");
    
    list_free(list);
}

/* =============================================================================
 * PARTEA 3: STACK ȘI QUEUE
 * =============================================================================
 */

static Stack* stack_create(int capacity) {
    Stack* s = malloc(sizeof(Stack));
    if (s) {
        s->data = malloc(capacity * sizeof(int));
        s->top = -1;
        s->capacity = capacity;
    }
    return s;
}

static bool stack_push(Stack* s, int value) {
    if (s->top >= s->capacity - 1) return false;
    s->data[++s->top] = value;
    return true;
}

static bool stack_pop(Stack* s, int* value) {
    if (s->top < 0) return false;
    *value = s->data[s->top--];
    return true;
}

static void stack_free(Stack* s) {
    if (s) {
        free(s->data);
        free(s);
    }
}

static Queue* queue_create(int capacity) {
    Queue* q = malloc(sizeof(Queue));
    if (q) {
        q->data = malloc(capacity * sizeof(int));
        q->front = 0;
        q->rear = -1;
        q->size = 0;
        q->capacity = capacity;
    }
    return q;
}

static bool queue_enqueue(Queue* q, int value) {
    if (q->size >= q->capacity) return false;
    q->rear = (q->rear + 1) % q->capacity;
    q->data[q->rear] = value;
    q->size++;
    return true;
}

static bool queue_dequeue(Queue* q, int* value) {
    if (q->size == 0) return false;
    *value = q->data[q->front];
    q->front = (q->front + 1) % q->capacity;
    q->size--;
    return true;
}

static void queue_free(Queue* q) {
    if (q) {
        free(q->data);
        free(q);
    }
}

static void demo_stack_queue(void) {
    print_header("PARTEA 3: Stack și Queue");
    
    /* Stack - evaluare expresie postfix */
    print_subheader("Stack: Evaluare expresie postfix");
    Stack* stack = stack_create(100);
    
    /* Evaluăm: 3 4 + 5 * = (3+4)*5 = 35 */
    printf("  Expresie: 3 4 + 5 * = (3+4)*5\n");
    printf("  Pași:\n");
    
    stack_push(stack, 3);
    printf("    Push 3: stack = [3]\n");
    
    stack_push(stack, 4);
    printf("    Push 4: stack = [3, 4]\n");
    
    int a, b;
    stack_pop(stack, &b);
    stack_pop(stack, &a);
    stack_push(stack, a + b);
    printf("    '+': Pop 4,3 → Push %d: stack = [%d]\n", a + b, a + b);
    
    stack_push(stack, 5);
    printf("    Push 5: stack = [7, 5]\n");
    
    stack_pop(stack, &b);
    stack_pop(stack, &a);
    stack_push(stack, a * b);
    printf("    '*': Pop 5,7 → Push %d: stack = [%d]\n", a * b, a * b);
    
    int result;
    stack_pop(stack, &result);
    printf("  " GREEN "Rezultat: %d" RESET "\n", result);
    print_success("Stack folosit pentru evaluare postfix");
    
    stack_free(stack);
    
    /* Queue - BFS simulare */
    print_subheader("Queue: Simulare procesare în ordine FIFO");
    Queue* queue = queue_create(100);
    
    printf("  Procesare comenzi în ordine:\n");
    const char* orders[] = {"CMD-001", "CMD-002", "CMD-003", "CMD-004"};
    
    for (int i = 0; i < 4; i++) {
        queue_enqueue(queue, i);
        printf("    Enqueue: %s\n", orders[i]);
    }
    
    printf("  Procesare:\n");
    int idx;
    while (queue_dequeue(queue, &idx)) {
        printf("    Dequeue: Procesez %s\n", orders[idx]);
    }
    print_success("Queue folosită pentru procesare FIFO");
    
    queue_free(queue);
}

/* =============================================================================
 * PARTEA 4: BINARY SEARCH TREE
 * =============================================================================
 */

static BSTNode* bst_create_node(int key, const char* value) {
    BSTNode* node = malloc(sizeof(BSTNode));
    if (node) {
        node->key = key;
        strncpy(node->value, value, MAX_NAME_LEN - 1);
        node->value[MAX_NAME_LEN - 1] = '\0';
        node->left = node->right = NULL;
    }
    return node;
}

static BSTNode* bst_insert(BSTNode* root, int key, const char* value) {
    if (!root) return bst_create_node(key, value);
    
    if (key < root->key) {
        root->left = bst_insert(root->left, key, value);
    } else if (key > root->key) {
        root->right = bst_insert(root->right, key, value);
    }
    return root;
}

static BSTNode* bst_search(BSTNode* root, int key) {
    if (!root || root->key == key) return root;
    
    if (key < root->key) return bst_search(root->left, key);
    return bst_search(root->right, key);
}

static void bst_inorder(BSTNode* root) {
    if (root) {
        bst_inorder(root->left);
        printf("    [%d] %s\n", root->key, root->value);
        bst_inorder(root->right);
    }
}

static void bst_free(BSTNode* root) {
    if (root) {
        bst_free(root->left);
        bst_free(root->right);
        free(root);
    }
}

static void demo_bst(void) {
    print_header("PARTEA 4: Binary Search Tree");
    
    print_subheader("Construire BST cu studenți (ID → Nume)");
    BSTNode* bst = NULL;
    
    struct { int id; const char* name; } students[] = {
        {105, "Popescu Ion"},
        {102, "Ionescu Maria"},
        {108, "Georgescu Ana"},
        {101, "Dumitrescu Vlad"},
        {107, "Constantinescu Elena"},
        {103, "Marinescu Alexandru"}
    };
    int n = sizeof(students) / sizeof(students[0]);
    
    for (int i = 0; i < n; i++) {
        bst = bst_insert(bst, students[i].id, students[i].name);
        printf("  Inserare: ID=%d, Nume=%s\n", students[i].id, students[i].name);
    }
    
    print_subheader("Traversare Inorder (sortat după ID)");
    bst_inorder(bst);
    print_success("BST oferă sortare automată la inserare");
    
    print_subheader("Căutare O(log n) - average case");
    int search_id = 107;
    BSTNode* found = bst_search(bst, search_id);
    if (found) {
        printf("  Găsit ID %d: %s\n", search_id, found->value);
    }
    print_success("Căutare eficientă în arbore balansat");
    
    bst_free(bst);
}

/* =============================================================================
 * PARTEA 5: HASH TABLE
 * =============================================================================
 */

static unsigned int hash_function(const char* key) {
    unsigned int hash = 5381;  /* djb2 algorithm */
    int c;
    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % HASH_TABLE_SIZE;
}

static HashTable* hash_create(void) {
    HashTable* ht = calloc(1, sizeof(HashTable));
    return ht;
}

static void hash_insert(HashTable* ht, const char* key, int value) {
    unsigned int index = hash_function(key);
    
    HashEntry* entry = malloc(sizeof(HashEntry));
    if (!entry) return;
    
    strncpy(entry->key, key, MAX_NAME_LEN - 1);
    entry->value = value;
    entry->next = ht->buckets[index];
    ht->buckets[index] = entry;
    ht->size++;
}

static int hash_search(HashTable* ht, const char* key, bool* found) {
    unsigned int index = hash_function(key);
    HashEntry* entry = ht->buckets[index];
    
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            *found = true;
            return entry->value;
        }
        entry = entry->next;
    }
    *found = false;
    return 0;
}

static void hash_free(HashTable* ht) {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        HashEntry* entry = ht->buckets[i];
        while (entry) {
            HashEntry* temp = entry;
            entry = entry->next;
            free(temp);
        }
    }
    free(ht);
}

static void demo_hash_table(void) {
    print_header("PARTEA 5: Hash Table");
    
    print_subheader("Creare și populare hash table");
    HashTable* ht = hash_create();
    
    struct { const char* name; int grade; } grades[] = {
        {"Algoritmi", 9},
        {"Structuri de Date", 8},
        {"Programare C", 10},
        {"Matematica", 7},
        {"Baze de Date", 9}
    };
    int n = sizeof(grades) / sizeof(grades[0]);
    
    for (int i = 0; i < n; i++) {
        hash_insert(ht, grades[i].name, grades[i].grade);
        printf("  Insert: \"%s\" → %d (hash=%u)\n", 
               grades[i].name, grades[i].grade, 
               hash_function(grades[i].name));
    }
    
    print_subheader("Căutare O(1) - average case");
    const char* search_keys[] = {"Programare C", "Fizica", "Algoritmi"};
    for (int i = 0; i < 3; i++) {
        bool found;
        int value = hash_search(ht, search_keys[i], &found);
        if (found) {
            printf("  " GREEN "✓" RESET " \"%s\" = %d\n", search_keys[i], value);
        } else {
            printf("  " RED "✗" RESET " \"%s\" nu există\n", search_keys[i]);
        }
    }
    print_success("Hash table oferă acces O(1) amortizat");
    
    hash_free(ht);
}

/* =============================================================================
 * PARTEA 6: GRAFURI - BFS ȘI DFS
 * =============================================================================
 */

static void graph_init(Graph* g, int vertices) {
    g->vertices = vertices;
    for (int i = 0; i < vertices; i++) {
        for (int j = 0; j < vertices; j++) {
            g->adj[i][j] = 0;
        }
    }
}

static void graph_add_edge(Graph* g, int src, int dest) {
    g->adj[src][dest] = 1;
    g->adj[dest][src] = 1;  /* Graf neorientat */
}

static void graph_bfs(Graph* g, int start) {
    bool visited[MAX_VERTICES] = {false};
    int queue[MAX_VERTICES];
    int front = 0, rear = 0;
    
    visited[start] = true;
    queue[rear++] = start;
    
    printf("  BFS de la nodul %d: ", start);
    
    while (front < rear) {
        int current = queue[front++];
        printf("%d ", current);
        
        for (int i = 0; i < g->vertices; i++) {
            if (g->adj[current][i] && !visited[i]) {
                visited[i] = true;
                queue[rear++] = i;
            }
        }
    }
    printf("\n");
}

static void dfs_recursive(Graph* g, int node, bool* visited) {
    visited[node] = true;
    printf("%d ", node);
    
    for (int i = 0; i < g->vertices; i++) {
        if (g->adj[node][i] && !visited[i]) {
            dfs_recursive(g, i, visited);
        }
    }
}

static void graph_dfs(Graph* g, int start) {
    bool visited[MAX_VERTICES] = {false};
    printf("  DFS de la nodul %d: ", start);
    dfs_recursive(g, start, visited);
    printf("\n");
}

static void demo_graphs(void) {
    print_header("PARTEA 6: Grafuri - BFS și DFS");
    
    Graph g;
    graph_init(&g, 7);
    
    print_subheader("Construire graf neorientat");
    printf("  Structura grafului:\n");
    printf("       0 ── 1 ── 2\n");
    printf("       │    │    │\n");
    printf("       3 ── 4 ── 5\n");
    printf("            │\n");
    printf("            6\n\n");
    
    graph_add_edge(&g, 0, 1);
    graph_add_edge(&g, 0, 3);
    graph_add_edge(&g, 1, 2);
    graph_add_edge(&g, 1, 4);
    graph_add_edge(&g, 2, 5);
    graph_add_edge(&g, 3, 4);
    graph_add_edge(&g, 4, 5);
    graph_add_edge(&g, 4, 6);
    
    print_subheader("Breadth-First Search (BFS)");
    graph_bfs(&g, 0);
    print_info("BFS explorează pe niveluri (ideal pentru distanțe minime)");
    
    print_subheader("Depth-First Search (DFS)");
    graph_dfs(&g, 0);
    print_info("DFS explorează în adâncime (ideal pentru căi, cicluri)");
}

/* =============================================================================
 * PARTEA 7: COMPARAȚIE COMPLEXITĂȚI
 * =============================================================================
 */

static void demo_complexity_comparison(void) {
    print_header("PARTEA 7: Comparație Complexități");
    
    printf("  ╔═══════════════════════════════════════════════════════════════╗\n");
    printf("  ║ Structură      │ Inserare │ Căutare  │ Ștergere │ Spațiu     ║\n");
    printf("  ╠═══════════════════════════════════════════════════════════════╣\n");
    printf("  ║ Array sortat   │ O(n)     │ O(log n) │ O(n)     │ O(n)       ║\n");
    printf("  ║ Linked List    │ O(1)*    │ O(n)     │ O(1)*    │ O(n)       ║\n");
    printf("  ║ Stack/Queue    │ O(1)     │ O(n)     │ O(1)     │ O(n)       ║\n");
    printf("  ║ BST (average)  │ O(log n) │ O(log n) │ O(log n) │ O(n)       ║\n");
    printf("  ║ BST (worst)    │ O(n)     │ O(n)     │ O(n)     │ O(n)       ║\n");
    printf("  ║ AVL Tree       │ O(log n) │ O(log n) │ O(log n) │ O(n)       ║\n");
    printf("  ║ Hash Table     │ O(1)*    │ O(1)*    │ O(1)*    │ O(n)       ║\n");
    printf("  ║ Heap           │ O(log n) │ O(n)     │ O(log n) │ O(n)       ║\n");
    printf("  ╚═══════════════════════════════════════════════════════════════╝\n");
    printf("  * amortizat\n\n");
    
    print_subheader("Ghid de selecție");
    printf("  • " CYAN "Acces rapid după index" RESET " → Array\n");
    printf("  • " CYAN "Inserări/ștergeri frecvente la început" RESET " → Linked List\n");
    printf("  • " CYAN "LIFO (undo, parsare)" RESET " → Stack\n");
    printf("  • " CYAN "FIFO (task queue, BFS)" RESET " → Queue\n");
    printf("  • " CYAN "Date sortate cu căutare rapidă" RESET " → BST/AVL\n");
    printf("  • " CYAN "Căutare O(1) după cheie" RESET " → Hash Table\n");
    printf("  • " CYAN "Extragere min/max rapid" RESET " → Heap\n");
    printf("  • " CYAN "Relații între entități" RESET " → Graph\n");
}

/* =============================================================================
 * PARTEA 8: BEST PRACTICES ȘI DEBUGGING
 * =============================================================================
 */

static void demo_best_practices(void) {
    print_header("PARTEA 8: Best Practices și Debugging");
    
    print_subheader("Erori comune de evitat");
    printf("  " RED "✗" RESET " Buffer overflow - verifică limitele array\n");
    printf("  " RED "✗" RESET " Memory leaks - eliberează ce aloci\n");
    printf("  " RED "✗" RESET " Use after free - nu accesa memorie eliberată\n");
    printf("  " RED "✗" RESET " NULL pointer dereference - verifică pointerii\n");
    printf("  " RED "✗" RESET " Integer overflow - folosește tipuri corespunzătoare\n");
    
    print_subheader("Comenzi Valgrind esențiale");
    printf("  valgrind --leak-check=full ./program\n");
    printf("  valgrind --track-origins=yes ./program\n");
    
    print_subheader("Comenzi GDB esențiale");
    printf("  ╔═══════════════════════════════════════════════════════════════╗\n");
    printf("  ║ Comandă        │ Descriere                                    ║\n");
    printf("  ╠═══════════════════════════════════════════════════════════════╣\n");
    printf("  ║ break main     │ Pune breakpoint la funcția main              ║\n");
    printf("  ║ run            │ Pornește execuția                            ║\n");
    printf("  ║ next (n)       │ Execută linia curentă                        ║\n");
    printf("  ║ step (s)       │ Intră în funcție                             ║\n");
    printf("  ║ print var      │ Afișează valoarea variabilei                 ║\n");
    printf("  ║ backtrace (bt) │ Afișează stack-ul de apeluri                 ║\n");
    printf("  ║ continue (c)   │ Continuă până la următorul breakpoint        ║\n");
    printf("  ╚═══════════════════════════════════════════════════════════════╝\n");
}

/* =============================================================================
 * PROGRAMUL PRINCIPAL
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf(MAGENTA BOLD);
    printf("╔═══════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                                                                           ║\n");
    printf("║    SĂPTĂMÂNA 14: RECAPITULARE AVANSATĂ ȘI OPTIMIZARE                      ║\n");
    printf("║    Algoritmi și Tehnici de Programare - ASE București                     ║\n");
    printf("║                                                                           ║\n");
    printf("║    Exemplu Complet: Integrare Structuri de Date și Algoritmi              ║\n");
    printf("║                                                                           ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════════════╝\n");
    printf(RESET "\n");
    
    /* Execută toate demonstrațiile */
    demo_arrays();
    demo_linked_list();
    demo_stack_queue();
    demo_bst();
    demo_hash_table();
    demo_graphs();
    demo_complexity_comparison();
    demo_best_practices();
    
    /* Concluzie */
    printf("\n");
    printf(GREEN BOLD);
    printf("╔═══════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                    DEMONSTRAȚIE COMPLETĂ!                                 ║\n");
    printf("║                                                                           ║\n");
    printf("║  Ați parcurs toate structurile de date fundamentale:                      ║\n");
    printf("║  • Arrays cu qsort/bsearch    • BST și traversări                         ║\n");
    printf("║  • Linked Lists               • Hash Tables                               ║\n");
    printf("║  • Stacks și Queues           • Grafuri cu BFS/DFS                        ║\n");
    printf("║                                                                           ║\n");
    printf("║  „Premature optimization is the root of all evil" - Donald Knuth          ║\n");
    printf("║                                                                           ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════════════╝\n");
    printf(RESET "\n");
    
    return 0;
}
