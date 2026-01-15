/**
 * =============================================================================
 * SĂPTĂMÂNA 8: ARBORI BINARI DE CĂUTARE (Binary Search Trees)
 * Exemplu Complet și Demonstrativ
 * =============================================================================
 *
 * Acest exemplu demonstrează:
 *   1. Crearea și inițializarea unui BST
 *   2. Inserarea nodurilor cu menținerea proprietății BST
 *   3. Căutarea (iterativă și recursivă)
 *   4. Toate cele 3 tipuri de parcurgeri (inorder, preorder, postorder)
 *   5. Găsirea minimului și maximului
 *   6. Ștergerea nodurilor (toate cele 3 cazuri)
 *   7. Calculul înălțimii și verificarea echilibrului
 *   8. Validarea proprietății BST
 *   9. Eliberarea corectă a memoriei
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -g -o example1 example1.c
 * Execuție:  ./example1
 * Valgrind:  valgrind --leak-check=full ./example1
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>

/* =============================================================================
 * PARTEA 1: DEFINIREA STRUCTURILOR
 * =============================================================================
 */

/**
 * Structura unui nod în Binary Search Tree
 * 
 * Fiecare nod conține:
 * - key: valoarea de căutare (pentru ordonare)
 * - data: pointer către date asociate (generic)
 * - left: pointer către subarborele stâng (valori < key)
 * - right: pointer către subarborele drept (valori > key)
 */
typedef struct BSTNode {
    int key;
    void *data;
    struct BSTNode *left;
    struct BSTNode *right;
} BSTNode;

/**
 * Structura arborelui BST
 * 
 * Encapsulează:
 * - root: pointer către nodul rădăcină
 * - size: numărul total de noduri
 */
typedef struct {
    BSTNode *root;
    size_t size;
} BST;

/* =============================================================================
 * PARTEA 2: FUNCȚII DE CREARE ȘI INIȚIALIZARE
 * =============================================================================
 */

/**
 * Creează un nod nou
 * 
 * @param key Valoarea cheii
 * @param data Pointer către date (poate fi NULL)
 * @return Pointer către nodul creat sau NULL la eroare
 */
BSTNode* bst_create_node(int key, void *data) {
    BSTNode *node = (BSTNode*)malloc(sizeof(BSTNode));
    if (node == NULL) {
        fprintf(stderr, "Eroare: Nu s-a putut aloca memorie pentru nod!\n");
        return NULL;
    }
    
    node->key = key;
    node->data = data;
    node->left = NULL;
    node->right = NULL;
    
    return node;
}

/**
 * Creează un arbore BST gol
 * 
 * @return Pointer către arborele creat sau NULL la eroare
 */
BST* bst_create(void) {
    BST *tree = (BST*)malloc(sizeof(BST));
    if (tree == NULL) {
        fprintf(stderr, "Eroare: Nu s-a putut aloca memorie pentru arbore!\n");
        return NULL;
    }
    
    tree->root = NULL;
    tree->size = 0;
    
    return tree;
}

/* =============================================================================
 * PARTEA 3: INSERARE
 * =============================================================================
 */

/**
 * Inserează un nod în subarbore (recursiv)
 * 
 * @param node Rădăcina subarborelui
 * @param key Cheia de inserat
 * @param data Datele asociate
 * @return Noua rădăcină a subarborelui
 */
BSTNode* bst_insert_node(BSTNode *node, int key, void *data) {
    /* Caz de bază: am găsit locul - creăm nod nou */
    if (node == NULL) {
        return bst_create_node(key, data);
    }
    
    /* Navigăm recursiv în subarbore */
    if (key < node->key) {
        node->left = bst_insert_node(node->left, key, data);
    } else if (key > node->key) {
        node->right = bst_insert_node(node->right, key, data);
    }
    /* Dacă key == node->key, ignorăm duplicatul */
    
    return node;
}

/**
 * Inserează o valoare în arbore
 * 
 * @param tree Pointer către arbore
 * @param key Cheia de inserat
 * @param data Datele asociate
 */
void bst_insert(BST *tree, int key, void *data) {
    if (tree == NULL) return;
    
    tree->root = bst_insert_node(tree->root, key, data);
    tree->size++;
}

/* =============================================================================
 * PARTEA 4: CĂUTARE
 * =============================================================================
 */

/**
 * Caută o cheie în arbore (iterativ)
 * 
 * Complexitate: O(log n) mediu, O(n) defavorabil
 * 
 * @param tree Arborele în care căutăm
 * @param key Cheia căutată
 * @return Pointer către nodul găsit sau NULL
 */
BSTNode* bst_search(BST *tree, int key) {
    if (tree == NULL) return NULL;
    
    BSTNode *current = tree->root;
    
    while (current != NULL) {
        if (key == current->key) {
            return current;  /* Găsit! */
        } else if (key < current->key) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    
    return NULL;  /* Negăsit */
}

/**
 * Caută o cheie în subarbore (recursiv)
 * 
 * @param node Rădăcina subarborelui
 * @param key Cheia căutată
 * @return Pointer către nodul găsit sau NULL
 */
BSTNode* bst_search_recursive(BSTNode *node, int key) {
    /* Caz de bază: arbore vid sau am găsit */
    if (node == NULL || node->key == key) {
        return node;
    }
    
    /* Căutăm în subarborele corespunzător */
    if (key < node->key) {
        return bst_search_recursive(node->left, key);
    }
    return bst_search_recursive(node->right, key);
}

/* =============================================================================
 * PARTEA 5: MINIM ȘI MAXIM
 * =============================================================================
 */

/**
 * Găsește nodul cu valoarea minimă
 * (Cel mai din stânga nod)
 */
BSTNode* bst_find_min(BSTNode *node) {
    if (node == NULL) return NULL;
    
    while (node->left != NULL) {
        node = node->left;
    }
    return node;
}

/**
 * Găsește nodul cu valoarea maximă
 * (Cel mai din dreapta nod)
 */
BSTNode* bst_find_max(BSTNode *node) {
    if (node == NULL) return NULL;
    
    while (node->right != NULL) {
        node = node->right;
    }
    return node;
}

/* =============================================================================
 * PARTEA 6: PARCURGERI (TRAVERSALS)
 * =============================================================================
 */

/**
 * Parcurgere Inorder: Stânga → Rădăcină → Dreapta
 * Rezultat: elemente în ordine SORTATĂ
 */
void bst_inorder(BSTNode *node, void (*visit)(BSTNode*)) {
    if (node == NULL) return;
    
    bst_inorder(node->left, visit);
    visit(node);
    bst_inorder(node->right, visit);
}

/**
 * Parcurgere Preorder: Rădăcină → Stânga → Dreapta
 * Utilă pentru: copierea arborelui
 */
void bst_preorder(BSTNode *node, void (*visit)(BSTNode*)) {
    if (node == NULL) return;
    
    visit(node);
    bst_preorder(node->left, visit);
    bst_preorder(node->right, visit);
}

/**
 * Parcurgere Postorder: Stânga → Dreapta → Rădăcină
 * Utilă pentru: ștergerea arborelui (eliberăm copiii înaintea părintelui)
 */
void bst_postorder(BSTNode *node, void (*visit)(BSTNode*)) {
    if (node == NULL) return;
    
    bst_postorder(node->left, visit);
    bst_postorder(node->right, visit);
    visit(node);
}

/* Funcție de vizitare simplă - afișează cheia */
void print_node(BSTNode *node) {
    printf("%d ", node->key);
}

/* =============================================================================
 * PARTEA 7: ȘTERGERE
 * =============================================================================
 */

/**
 * Șterge un nod din subarbore (Algoritmul Hibbard)
 * 
 * Tratează cele 3 cazuri:
 * 1. Nod frunză (0 copii)
 * 2. Nod cu 1 copil
 * 3. Nod cu 2 copii (înlocuire cu succesorul inorder)
 * 
 * @param node Rădăcina subarborelui
 * @param key Cheia de șters
 * @return Noua rădăcină a subarborelui
 */
BSTNode* bst_delete_node(BSTNode *node, int key) {
    /* Caz de bază: arbore vid */
    if (node == NULL) {
        return NULL;
    }
    
    /* Navigare pentru găsirea nodului */
    if (key < node->key) {
        node->left = bst_delete_node(node->left, key);
    } else if (key > node->key) {
        node->right = bst_delete_node(node->right, key);
    } else {
        /* Am găsit nodul de șters */
        
        /* Cazul 1 & 2: 0 sau 1 copil */
        if (node->left == NULL) {
            BSTNode *temp = node->right;
            free(node);
            return temp;
        }
        if (node->right == NULL) {
            BSTNode *temp = node->left;
            free(node);
            return temp;
        }
        
        /* Cazul 3: 2 copii */
        /* Găsim succesorul inorder (minimul din dreapta) */
        BSTNode *successor = bst_find_min(node->right);
        
        /* Copiem datele succesorului în nodul curent */
        node->key = successor->key;
        node->data = successor->data;
        
        /* Ștergem succesorul din subarbore */
        node->right = bst_delete_node(node->right, successor->key);
    }
    
    return node;
}

/**
 * Șterge o valoare din arbore
 */
void bst_delete(BST *tree, int key) {
    if (tree == NULL) return;
    
    tree->root = bst_delete_node(tree->root, key);
    tree->size--;
}

/* =============================================================================
 * PARTEA 8: ÎNĂLȚIME ȘI ECHILIBRU
 * =============================================================================
 */

/**
 * Calculează înălțimea unui subarbore
 * 
 * Înălțime = numărul de muchii pe cel mai lung drum de la rădăcină la frunză
 * Arbore vid: -1
 * Arbore cu un nod: 0
 */
int bst_height(BSTNode *node) {
    if (node == NULL) {
        return -1;
    }
    
    int left_height = bst_height(node->left);
    int right_height = bst_height(node->right);
    
    return 1 + (left_height > right_height ? left_height : right_height);
}

/**
 * Verifică dacă un subarbore este echilibrat (în sensul AVL)
 * 
 * Un arbore este echilibrat dacă pentru fiecare nod,
 * |înălțime(stânga) - înălțime(dreapta)| <= 1
 */
bool bst_is_balanced(BSTNode *node) {
    if (node == NULL) {
        return true;
    }
    
    int balance_factor = bst_height(node->left) - bst_height(node->right);
    
    if (balance_factor < -1 || balance_factor > 1) {
        return false;
    }
    
    return bst_is_balanced(node->left) && bst_is_balanced(node->right);
}

/* =============================================================================
 * PARTEA 9: VALIDARE BST
 * =============================================================================
 */

/**
 * Verifică dacă un subarbore respectă proprietatea BST
 * Folosește metoda limitelor min/max
 */
bool is_bst_util(BSTNode *node, int min_val, int max_val) {
    if (node == NULL) {
        return true;
    }
    
    /* Verificăm dacă nodul respectă limitele */
    if (node->key <= min_val || node->key >= max_val) {
        return false;
    }
    
    /* Verificăm recursiv subarborii cu limite actualizate */
    return is_bst_util(node->left, min_val, node->key) &&
           is_bst_util(node->right, node->key, max_val);
}

/**
 * Verifică dacă un arbore este BST valid
 */
bool bst_is_valid(BST *tree) {
    if (tree == NULL) return true;
    return is_bst_util(tree->root, INT_MIN, INT_MAX);
}

/* =============================================================================
 * PARTEA 10: AFIȘARE ARBORE (PRETTY PRINT)
 * =============================================================================
 */

/**
 * Afișează arborele vizual (rotit 90° la stânga)
 */
void bst_print_tree_util(BSTNode *node, int space, int indent) {
    if (node == NULL) {
        return;
    }
    
    space += indent;
    
    /* Procesăm mai întâi subarborele drept */
    bst_print_tree_util(node->right, space, indent);
    
    /* Afișăm nodul curent după spațiile necesare */
    printf("\n");
    for (int i = indent; i < space; i++) {
        printf(" ");
    }
    printf("%d", node->key);
    
    /* Procesăm subarborele stâng */
    bst_print_tree_util(node->left, space, indent);
}

void bst_print_tree(BST *tree) {
    if (tree == NULL || tree->root == NULL) {
        printf("(arbore vid)\n");
        return;
    }
    bst_print_tree_util(tree->root, 0, 5);
    printf("\n");
}

/* =============================================================================
 * PARTEA 11: ELIBERARE MEMORIE
 * =============================================================================
 */

/**
 * Eliberează recursiv toate nodurile (POSTORDER!)
 * 
 * IMPORTANT: Trebuie să eliberăm copiii ÎNAINTE de părinte,
 * altfel pierdem pointerii către copii!
 */
void bst_free_tree(BSTNode *node) {
    if (node == NULL) {
        return;
    }
    
    /* Mai întâi eliberăm copiii */
    bst_free_tree(node->left);
    bst_free_tree(node->right);
    
    /* Apoi eliberăm nodul curent */
    /* Dacă data a fost alocată dinamic, eliberați-o aici */
    free(node);
}

/**
 * Distruge un arbore complet
 */
void bst_destroy(BST *tree) {
    if (tree == NULL) {
        return;
    }
    
    bst_free_tree(tree->root);
    free(tree);
}

/* =============================================================================
 * PARTEA 12: FUNCȚII DEMONSTRATIVE
 * =============================================================================
 */

void demo_create_and_insert(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 1: CREARE ȘI INSERARE                             ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Creăm un arbore și inserăm valorile: 50, 30, 70, 20, 40, 60, 80\n\n");
    
    BST *tree = bst_create();
    
    int values[] = {50, 30, 70, 20, 40, 60, 80};
    int n = sizeof(values) / sizeof(values[0]);
    
    for (int i = 0; i < n; i++) {
        printf("Inserăm %d... ", values[i]);
        bst_insert(tree, values[i], NULL);
        printf("OK (dimensiune: %zu)\n", tree->size);
    }
    
    printf("\nStructura arborelui:\n");
    bst_print_tree(tree);
    
    printf("\n");
    printf("              50           \n");
    printf("            /    \\         \n");
    printf("          30      70       \n");
    printf("         /  \\    /  \\      \n");
    printf("       20   40  60   80    \n");
    
    bst_destroy(tree);
}

void demo_search(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 2: CĂUTARE                                        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    BST *tree = bst_create();
    int values[] = {50, 30, 70, 20, 40, 60, 80};
    int n = sizeof(values) / sizeof(values[0]);
    for (int i = 0; i < n; i++) {
        bst_insert(tree, values[i], NULL);
    }
    
    int search_keys[] = {40, 60, 25, 100};
    int m = sizeof(search_keys) / sizeof(search_keys[0]);
    
    for (int i = 0; i < m; i++) {
        int key = search_keys[i];
        BSTNode *result = bst_search(tree, key);
        
        if (result != NULL) {
            printf("✓ Căutare %d: GĂSIT\n", key);
        } else {
            printf("✗ Căutare %d: NEGĂSIT\n", key);
        }
    }
    
    printf("\nCăutarea 40: 50 → 30 → 40 (3 pași)\n");
    printf("Căutarea 60: 50 → 70 → 60 (3 pași)\n");
    
    bst_destroy(tree);
}

void demo_traversals(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 3: PARCURGERI (TRAVERSALS)                        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    BST *tree = bst_create();
    int values[] = {50, 30, 70, 20, 40, 60, 80};
    int n = sizeof(values) / sizeof(values[0]);
    for (int i = 0; i < n; i++) {
        bst_insert(tree, values[i], NULL);
    }
    
    printf("Arbore:\n");
    bst_print_tree(tree);
    
    printf("\nInorder (L-R-D):   ");
    bst_inorder(tree->root, print_node);
    printf("\n→ Rezultat SORTAT!\n\n");
    
    printf("Preorder (R-L-D):  ");
    bst_preorder(tree->root, print_node);
    printf("\n→ Util pentru copiere arbore\n\n");
    
    printf("Postorder (L-D-R): ");
    bst_postorder(tree->root, print_node);
    printf("\n→ Util pentru ștergere arbore (eliberăm copiii înainte de părinte)\n");
    
    bst_destroy(tree);
}

void demo_min_max(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 4: MINIM ȘI MAXIM                                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    BST *tree = bst_create();
    int values[] = {50, 30, 70, 20, 40, 60, 80, 10, 90};
    int n = sizeof(values) / sizeof(values[0]);
    for (int i = 0; i < n; i++) {
        bst_insert(tree, values[i], NULL);
    }
    
    printf("Arbore cu valorile: ");
    bst_inorder(tree->root, print_node);
    printf("\n\n");
    
    BSTNode *min_node = bst_find_min(tree->root);
    BSTNode *max_node = bst_find_max(tree->root);
    
    if (min_node) {
        printf("Minimul: %d (cel mai din stânga nod)\n", min_node->key);
    }
    if (max_node) {
        printf("Maximul: %d (cel mai din dreapta nod)\n", max_node->key);
    }
    
    printf("\nComplexitate: O(h) unde h = înălțimea arborelui\n");
    printf("În cazul echilibrat: O(log n)\n");
    
    bst_destroy(tree);
}

void demo_deletion(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 5: ȘTERGERE (CELE 3 CAZURI)                       ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Cazul 1: Ștergere nod frunză */
    printf("═══ CAZUL 1: Ștergere nod FRUNZĂ ═══\n\n");
    BST *tree1 = bst_create();
    int v1[] = {50, 30, 70, 20, 40};
    for (int i = 0; i < 5; i++) bst_insert(tree1, v1[i], NULL);
    
    printf("Înainte de ștergere 20:\n");
    bst_print_tree(tree1);
    
    bst_delete(tree1, 20);
    
    printf("După ștergerea lui 20 (frunză):\n");
    bst_print_tree(tree1);
    bst_destroy(tree1);
    
    /* Cazul 2: Ștergere nod cu 1 copil */
    printf("\n═══ CAZUL 2: Ștergere nod cu 1 COPIL ═══\n\n");
    BST *tree2 = bst_create();
    int v2[] = {50, 30, 70, 20};
    for (int i = 0; i < 4; i++) bst_insert(tree2, v2[i], NULL);
    
    printf("Înainte de ștergere 30:\n");
    bst_print_tree(tree2);
    
    bst_delete(tree2, 30);
    
    printf("După ștergerea lui 30 (avea doar copilul 20):\n");
    bst_print_tree(tree2);
    printf("→ Nodul 20 ia locul lui 30\n");
    bst_destroy(tree2);
    
    /* Cazul 3: Ștergere nod cu 2 copii */
    printf("\n═══ CAZUL 3: Ștergere nod cu 2 COPII ═══\n\n");
    BST *tree3 = bst_create();
    int v3[] = {50, 30, 70, 20, 40, 60, 80};
    for (int i = 0; i < 7; i++) bst_insert(tree3, v3[i], NULL);
    
    printf("Înainte de ștergere 50 (rădăcina!):\n");
    bst_print_tree(tree3);
    
    bst_delete(tree3, 50);
    
    printf("După ștergerea lui 50:\n");
    bst_print_tree(tree3);
    printf("→ Succesorul inorder (60) ia locul lui 50\n");
    
    printf("\nInorder după ștergere: ");
    bst_inorder(tree3->root, print_node);
    printf("\n→ Încă sortat! Proprietatea BST menținută.\n");
    
    bst_destroy(tree3);
}

void demo_height_balance(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 6: ÎNĂLȚIME ȘI ECHILIBRU                          ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Arbore echilibrat */
    printf("═══ Arbore ECHILIBRAT ═══\n\n");
    BST *balanced = bst_create();
    int v_bal[] = {50, 30, 70, 20, 40, 60, 80};
    for (int i = 0; i < 7; i++) bst_insert(balanced, v_bal[i], NULL);
    
    bst_print_tree(balanced);
    printf("Înălțime: %d\n", bst_height(balanced->root));
    printf("Echilibrat: %s\n", bst_is_balanced(balanced->root) ? "DA ✓" : "NU ✗");
    bst_destroy(balanced);
    
    /* Arbore dezechilibrat (degenerat) */
    printf("\n═══ Arbore DEZECHILIBRAT (degenerat) ═══\n\n");
    BST *degenerate = bst_create();
    int v_deg[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++) bst_insert(degenerate, v_deg[i], NULL);
    
    bst_print_tree(degenerate);
    printf("Înălțime: %d\n", bst_height(degenerate->root));
    printf("Echilibrat: %s\n", bst_is_balanced(degenerate->root) ? "DA ✓" : "NU ✗");
    printf("\n⚠️  Aceasta este practic o listă! Complexitate O(n) în loc de O(log n)\n");
    bst_destroy(degenerate);
}

void demo_validation(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 7: VALIDARE BST                                   ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* BST valid */
    BST *valid = bst_create();
    int v[] = {50, 30, 70, 20, 40};
    for (int i = 0; i < 5; i++) bst_insert(valid, v[i], NULL);
    
    printf("Arbore creat prin inserare normală:\n");
    bst_print_tree(valid);
    printf("Este BST valid: %s\n", bst_is_valid(valid) ? "DA ✓" : "NU ✗");
    bst_destroy(valid);
    
    /* Simulăm un arbore invalid (modificăm manual) */
    printf("\nCreăm manual un arbore INVALID:\n");
    BSTNode *invalid_root = bst_create_node(50, NULL);
    invalid_root->left = bst_create_node(30, NULL);
    invalid_root->right = bst_create_node(70, NULL);
    invalid_root->left->left = bst_create_node(60, NULL);  /* INVALID: 60 > 30, dar e în stânga lui 30! */
    
    printf("       50\n");
    printf("      /  \\\n");
    printf("    30    70\n");
    printf("   /\n");
    printf("  60 ← INVALID! (60 > 30 dar e în stânga lui 30)\n\n");
    
    BST invalid_tree = {invalid_root, 4};
    printf("Este BST valid: %s\n", bst_is_valid(&invalid_tree) ? "DA ✓" : "NU ✗");
    
    /* Eliberăm manual */
    free(invalid_root->left->left);
    free(invalid_root->left);
    free(invalid_root->right);
    free(invalid_root);
}

/* =============================================================================
 * FUNCȚIA MAIN
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     SĂPTĂMÂNA 8: ARBORI BINARI DE CĂUTARE - Exemplu Complet   ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    demo_create_and_insert();
    demo_search();
    demo_traversals();
    demo_min_max();
    demo_deletion();
    demo_height_balance();
    demo_validation();
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     DEMONSTRAȚIE COMPLETĂ!                                    ║\n");
    printf("║                                                               ║\n");
    printf("║     Rulați cu Valgrind pentru a verifica gestiunea memoriei:  ║\n");
    printf("║     $ valgrind --leak-check=full ./example1                   ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}

/* =============================================================================
 * SFÂRȘIT
 * =============================================================================
 */
