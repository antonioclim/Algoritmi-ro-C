/**
 * =============================================================================
 * SOLUȚIE TEMĂ 2: Arbore AVL Generic cu Funcții Callback
 * =============================================================================
 * 
 * Implementarea unui arbore AVL generic care poate stoca orice tip de date
 * folosind pointeri void* și funcții callback pentru comparare și afișare.
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -o homework2_sol homework2_sol.c
 * Utilizare: ./homework2_sol
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* =============================================================================
 * TIPURI DE DATE ȘI CALLBACK-URI
 * =============================================================================
 */

/* Funcție de comparare: returnează <0 dacă a<b, 0 dacă a==b, >0 dacă a>b */
typedef int (*CompareFunc)(const void*, const void*);

/* Funcție de afișare */
typedef void (*PrintFunc)(const void*);

/* Funcție de eliberare memorie */
typedef void (*FreeFunc)(void*);

/* =============================================================================
 * STRUCTURI DE DATE
 * =============================================================================
 */

typedef struct GenericAVLNode {
    void *data;
    struct GenericAVLNode *left;
    struct GenericAVLNode *right;
    int height;
} GenericAVLNode;

typedef struct {
    GenericAVLNode *root;
    CompareFunc compare;
    PrintFunc print;
    FreeFunc free_data;
    size_t count;
} AVLTree;

/* =============================================================================
 * FUNCȚII AUXILIARE
 * =============================================================================
 */

int max(int a, int b) {
    return (a > b) ? a : b;
}

int height(GenericAVLNode *node) {
    return (node == NULL) ? 0 : node->height;
}

int balance_factor(GenericAVLNode *node) {
    if (node == NULL) return 0;
    return height(node->left) - height(node->right);
}

void update_height(GenericAVLNode *node) {
    if (node != NULL) {
        node->height = 1 + max(height(node->left), height(node->right));
    }
}

GenericAVLNode* create_node(void *data) {
    GenericAVLNode *node = (GenericAVLNode*)malloc(sizeof(GenericAVLNode));
    if (node == NULL) {
        fprintf(stderr, "Eroare: Alocare memorie eșuată!\n");
        exit(EXIT_FAILURE);
    }
    node->data = data;
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    return node;
}

/* =============================================================================
 * ROTAȚII
 * =============================================================================
 */

GenericAVLNode* rotate_right(GenericAVLNode *z) {
    GenericAVLNode *y = z->left;
    GenericAVLNode *T3 = y->right;
    
    y->right = z;
    z->left = T3;
    
    update_height(z);
    update_height(y);
    
    return y;
}

GenericAVLNode* rotate_left(GenericAVLNode *z) {
    GenericAVLNode *y = z->right;
    GenericAVLNode *T2 = y->left;
    
    y->left = z;
    z->right = T2;
    
    update_height(z);
    update_height(y);
    
    return y;
}

/* =============================================================================
 * OPERAȚII ARBORE GENERIC
 * =============================================================================
 */

/**
 * Creează un nou arbore AVL generic.
 */
AVLTree* avl_create(CompareFunc compare, PrintFunc print, FreeFunc free_fn) {
    AVLTree *tree = (AVLTree*)malloc(sizeof(AVLTree));
    if (tree == NULL) {
        fprintf(stderr, "Eroare: Alocare memorie eșuată!\n");
        exit(EXIT_FAILURE);
    }
    tree->root = NULL;
    tree->compare = compare;
    tree->print = print;
    tree->free_data = free_fn;
    tree->count = 0;
    return tree;
}

/**
 * Inserare recursivă în arbore.
 */
GenericAVLNode* avl_insert_helper(GenericAVLNode *node, void *data, 
                                   CompareFunc compare, bool *inserted) {
    if (node == NULL) {
        *inserted = true;
        return create_node(data);
    }
    
    int cmp = compare(data, node->data);
    
    if (cmp < 0) {
        node->left = avl_insert_helper(node->left, data, compare, inserted);
    } else if (cmp > 0) {
        node->right = avl_insert_helper(node->right, data, compare, inserted);
    } else {
        /* Element duplicat - nu inserăm */
        *inserted = false;
        return node;
    }
    
    update_height(node);
    
    int bf = balance_factor(node);
    
    /* Cazul LL */
    if (bf > 1 && compare(data, node->left->data) < 0) {
        return rotate_right(node);
    }
    
    /* Cazul RR */
    if (bf < -1 && compare(data, node->right->data) > 0) {
        return rotate_left(node);
    }
    
    /* Cazul LR */
    if (bf > 1 && compare(data, node->left->data) > 0) {
        node->left = rotate_left(node->left);
        return rotate_right(node);
    }
    
    /* Cazul RL */
    if (bf < -1 && compare(data, node->right->data) < 0) {
        node->right = rotate_right(node->right);
        return rotate_left(node);
    }
    
    return node;
}

/**
 * Inserează un element în arbore.
 */
bool avl_insert(AVLTree *tree, void *data) {
    bool inserted = false;
    tree->root = avl_insert_helper(tree->root, data, tree->compare, &inserted);
    if (inserted) {
        tree->count++;
    }
    return inserted;
}

/**
 * Caută un element în arbore.
 */
void* avl_search(AVLTree *tree, const void *key) {
    GenericAVLNode *current = tree->root;
    
    while (current != NULL) {
        int cmp = tree->compare(key, current->data);
        if (cmp == 0) {
            return current->data;
        } else if (cmp < 0) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    
    return NULL;
}

/**
 * Găsește nodul cu valoarea minimă.
 */
GenericAVLNode* avl_min_node(GenericAVLNode *node) {
    GenericAVLNode *current = node;
    while (current != NULL && current->left != NULL) {
        current = current->left;
    }
    return current;
}

/**
 * Ștergere recursivă din arbore.
 */
GenericAVLNode* avl_delete_helper(GenericAVLNode *root, const void *key,
                                   CompareFunc compare, FreeFunc free_fn,
                                   bool *deleted) {
    if (root == NULL) {
        return root;
    }
    
    int cmp = compare(key, root->data);
    
    if (cmp < 0) {
        root->left = avl_delete_helper(root->left, key, compare, free_fn, deleted);
    } else if (cmp > 0) {
        root->right = avl_delete_helper(root->right, key, compare, free_fn, deleted);
    } else {
        *deleted = true;
        
        if (root->left == NULL || root->right == NULL) {
            GenericAVLNode *temp = root->left ? root->left : root->right;
            
            if (temp == NULL) {
                temp = root;
                root = NULL;
            } else {
                GenericAVLNode *to_free = root;
                root = temp;
                temp = to_free;
            }
            
            if (free_fn && temp->data) {
                free_fn(temp->data);
            }
            free(temp);
        } else {
            GenericAVLNode *temp = avl_min_node(root->right);
            
            /* Salvăm pointer-ul vechi pentru eliberare */
            void *old_data = root->data;
            
            /* Copiem datele (trebuie să facem o copie, nu doar să mutăm pointer-ul) */
            root->data = temp->data;
            temp->data = old_data;
            
            root->right = avl_delete_helper(root->right, temp->data, 
                                             compare, free_fn, deleted);
        }
    }
    
    if (root == NULL) {
        return root;
    }
    
    update_height(root);
    
    int bf = balance_factor(root);
    
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
 * Șterge un element din arbore.
 */
bool avl_delete(AVLTree *tree, const void *key) {
    bool deleted = false;
    tree->root = avl_delete_helper(tree->root, key, tree->compare, 
                                    tree->free_data, &deleted);
    if (deleted) {
        tree->count--;
    }
    return deleted;
}

/**
 * Parcurgere inorder recursivă.
 */
void avl_inorder_helper(GenericAVLNode *node, PrintFunc print) {
    if (node != NULL) {
        avl_inorder_helper(node->left, print);
        print(node->data);
        printf(" ");
        avl_inorder_helper(node->right, print);
    }
}

/**
 * Parcurgere inorder.
 */
void avl_traverse_inorder(AVLTree *tree) {
    avl_inorder_helper(tree->root, tree->print);
    printf("\n");
}

/**
 * Eliberare memorie recursivă.
 */
void avl_free_helper(GenericAVLNode *node, FreeFunc free_fn) {
    if (node != NULL) {
        avl_free_helper(node->left, free_fn);
        avl_free_helper(node->right, free_fn);
        if (free_fn && node->data) {
            free_fn(node->data);
        }
        free(node);
    }
}

/**
 * Distruge arborele și eliberează memoria.
 */
void avl_destroy(AVLTree *tree) {
    avl_free_helper(tree->root, tree->free_data);
    free(tree);
}

/* =============================================================================
 * CALLBACK-URI PENTRU ÎNTREGI
 * =============================================================================
 */

int compare_int(const void *a, const void *b) {
    return *(const int*)a - *(const int*)b;
}

void print_int(const void *data) {
    printf("%d", *(const int*)data);
}

/* =============================================================================
 * CALLBACK-URI PENTRU STRUCTURA STUDENT
 * =============================================================================
 */

typedef struct {
    char name[50];
    float average;
} Student;

int compare_student_by_name(const void *a, const void *b) {
    return strcmp(((const Student*)a)->name, ((const Student*)b)->name);
}

int compare_student_by_average(const void *a, const void *b) {
    float diff = ((const Student*)a)->average - ((const Student*)b)->average;
    if (diff < 0) return -1;
    if (diff > 0) return 1;
    return 0;
}

void print_student(const void *data) {
    const Student *s = (const Student*)data;
    printf("(%s, %.2f)", s->name, s->average);
}

Student* create_student(const char *name, float average) {
    Student *s = (Student*)malloc(sizeof(Student));
    if (s == NULL) {
        fprintf(stderr, "Eroare: Alocare memorie eșuată!\n");
        exit(EXIT_FAILURE);
    }
    strncpy(s->name, name, sizeof(s->name) - 1);
    s->name[sizeof(s->name) - 1] = '\0';
    s->average = average;
    return s;
}

/* =============================================================================
 * CALLBACK-URI PENTRU ȘIRURI DE CARACTERE
 * =============================================================================
 */

int compare_string(const void *a, const void *b) {
    return strcmp((const char*)a, (const char*)b);
}

void print_string(const void *data) {
    printf("\"%s\"", (const char*)data);
}

char* create_string(const char *str) {
    char *s = (char*)malloc(strlen(str) + 1);
    if (s == NULL) {
        fprintf(stderr, "Eroare: Alocare memorie eșuată!\n");
        exit(EXIT_FAILURE);
    }
    strcpy(s, str);
    return s;
}

/* =============================================================================
 * FUNCȚIA MAIN - DEMONSTRAȚIE
 * =============================================================================
 */

int main(void) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     TEMĂ 2: Arbore AVL Generic cu Funcții Callback            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* ═══════════════════════════════════════════════════════════════════════
     * DEMONSTRAȚIE 1: Arbore de întregi
     * ═══════════════════════════════════════════════════════════════════════
     */
    printf("═══ Demonstrație 1: Arbore de Întregi ═══\n\n");
    
    AVLTree *int_tree = avl_create(compare_int, print_int, free);
    
    int values[] = {50, 30, 70, 20, 40, 60, 80, 10, 25, 35};
    int n = sizeof(values) / sizeof(values[0]);
    
    printf("Inserare valori: ");
    for (int i = 0; i < n; i++) {
        int *val = (int*)malloc(sizeof(int));
        *val = values[i];
        avl_insert(int_tree, val);
        printf("%d ", values[i]);
    }
    printf("\n");
    
    printf("Parcurgere inorder: ");
    avl_traverse_inorder(int_tree);
    
    printf("Număr elemente: %zu\n", int_tree->count);
    
    /* Căutare */
    int search_key = 40;
    int *found = (int*)avl_search(int_tree, &search_key);
    printf("Căutare %d: %s\n", search_key, found ? "GĂSIT" : "NEGĂSIT");
    
    search_key = 100;
    found = (int*)avl_search(int_tree, &search_key);
    printf("Căutare %d: %s\n", search_key, found ? "GĂSIT" : "NEGĂSIT");
    
    /* Ștergere */
    int delete_key = 30;
    printf("Ștergere %d: %s\n", delete_key, 
           avl_delete(int_tree, &delete_key) ? "SUCCES" : "EȘUAT");
    
    printf("Parcurgere după ștergere: ");
    avl_traverse_inorder(int_tree);
    
    avl_destroy(int_tree);
    printf("✓ Arbore de întregi eliberat\n\n");
    
    /* ═══════════════════════════════════════════════════════════════════════
     * DEMONSTRAȚIE 2: Arbore de studenți (ordonat după nume)
     * ═══════════════════════════════════════════════════════════════════════
     */
    printf("═══ Demonstrație 2: Arbore de Studenți (după nume) ═══\n\n");
    
    AVLTree *student_tree = avl_create(compare_student_by_name, print_student, free);
    
    avl_insert(student_tree, create_student("Ion", 8.5));
    avl_insert(student_tree, create_student("Ana", 9.2));
    avl_insert(student_tree, create_student("Maria", 7.8));
    avl_insert(student_tree, create_student("Andrei", 8.9));
    avl_insert(student_tree, create_student("Elena", 9.5));
    
    printf("Parcurgere inorder (ordine alfabetică):\n  ");
    avl_traverse_inorder(student_tree);
    
    printf("Număr studenți: %zu\n", student_tree->count);
    
    /* Căutare student */
    Student search_student = {"Ana", 0};
    Student *found_student = (Student*)avl_search(student_tree, &search_student);
    if (found_student) {
        printf("Găsit: %s cu media %.2f\n", found_student->name, found_student->average);
    }
    
    avl_destroy(student_tree);
    printf("✓ Arbore de studenți eliberat\n\n");
    
    /* ═══════════════════════════════════════════════════════════════════════
     * DEMONSTRAȚIE 3: Arbore de șiruri de caractere
     * ═══════════════════════════════════════════════════════════════════════
     */
    printf("═══ Demonstrație 3: Arbore de Șiruri ═══\n\n");
    
    AVLTree *string_tree = avl_create(compare_string, print_string, free);
    
    const char *words[] = {"zebra", "arbore", "calculator", "date", "funcție"};
    int m = sizeof(words) / sizeof(words[0]);
    
    printf("Inserare cuvinte: ");
    for (int i = 0; i < m; i++) {
        avl_insert(string_tree, create_string(words[i]));
        printf("%s ", words[i]);
    }
    printf("\n");
    
    printf("Parcurgere inorder (ordine alfabetică):\n  ");
    avl_traverse_inorder(string_tree);
    
    /* Căutare */
    const char *search_word = "date";
    char *found_word = (char*)avl_search(string_tree, search_word);
    printf("Căutare \"%s\": %s\n", search_word, found_word ? "GĂSIT" : "NEGĂSIT");
    
    avl_destroy(string_tree);
    printf("✓ Arbore de șiruri eliberat\n\n");
    
    /* ═══════════════════════════════════════════════════════════════════════
     * CONCLUZII
     * ═══════════════════════════════════════════════════════════════════════
     */
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("✓ Toate demonstrațiile au fost executate cu succes!\n");
    printf("✓ Arborii generici funcționează cu multiple tipuri de date.\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    
    return 0;
}
