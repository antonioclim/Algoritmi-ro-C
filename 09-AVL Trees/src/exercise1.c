/**
 * =============================================================================
 * EXERCIȚIUL 1: Arbore AVL de Bază
 * =============================================================================
 *
 * OBIECTIV:
 *   Implementarea completă a unui arbore AVL cu operații de inserare,
 *   parcurgeri și afișare. Acest exercițiu acoperă conceptele fundamentale
 *   ale arborilor AVL auto-echilibrați.
 *
 * CERINȚE:
 *   1. Definirea structurii AVLNode
 *   2. Implementarea funcțiilor auxiliare (height, balance_factor, max)
 *   3. Implementarea rotațiilor (rotate_left, rotate_right)
 *   4. Implementarea funcției insert cu auto-echilibrare
 *   5. Implementarea parcurgerilor (inorder, preorder, postorder)
 *   6. Afișarea arborelui cu factorul de echilibru
 *
 * EXEMPLU INPUT (din stdin sau fișier):
 *   10
 *   50 30 70 20 40 60 80 10 25 35
 *
 * EXPECTED OUTPUT:
 *   Arbore AVL construit cu 10 noduri
 *   Înălțime: 4
 *   Parcurgere inorder: 10 20 25 30 35 40 50 60 70 80
 *   Factor echilibru rădăcină: 0
 *   Valid AVL: DA
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -o exercise1 exercise1.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* =============================================================================
 * DEFINIȚII DE TIPURI
 * =============================================================================
 */

/**
 * TODO 1: Definește structura AVLNode
 *
 * Structura trebuie să conțină:
 *   - key: valoarea stocată (int)
 *   - left: pointer către copilul stâng
 *   - right: pointer către copilul drept
 *   - height: înălțimea nodului (int)
 *
 * Hint: Folosește typedef pentru a simplifica declarațiile
 */

/* YOUR CODE HERE - Definește structura AVLNode */
typedef struct AVLNode {
    int key;
    struct AVLNode *left;
    struct AVLNode *right;
    int height;
} AVLNode;


/* =============================================================================
 * FUNCȚII AUXILIARE
 * =============================================================================
 */

/**
 * TODO 2: Implementează funcția max
 *
 * Returnează maximul dintre două numere întregi.
 *
 * @param a Primul număr
 * @param b Al doilea număr
 * @return Maximul dintre a și b
 */
int max(int a, int b) {
    /* YOUR CODE HERE */
    return 0;  /* Înlocuiește această linie */
}

/**
 * TODO 3: Implementează funcția height
 *
 * Returnează înălțimea unui nod.
 * IMPORTANT: Dacă nodul este NULL, returnează 0.
 *
 * @param node Pointer către nod (poate fi NULL)
 * @return Înălțimea nodului sau 0 dacă NULL
 */
int height(AVLNode *node) {
    /* YOUR CODE HERE */
    return 0;  /* Înlocuiește această linie */
}

/**
 * TODO 4: Implementează funcția balance_factor
 *
 * Calculează factorul de echilibru: bf = height(left) - height(right)
 *
 * Valori valide AVL: -1, 0, +1
 * Valori invalide: |bf| > 1 (necesită reechilibrare)
 *
 * @param node Pointer către nod
 * @return Factorul de echilibru sau 0 dacă NULL
 */
int balance_factor(AVLNode *node) {
    /* YOUR CODE HERE */
    return 0;  /* Înlocuiește această linie */
}

/**
 * TODO 5: Implementează funcția create_node
 *
 * Creează un nou nod AVL cu cheia specificată.
 * - Alocă memorie pentru nod
 * - Verifică dacă alocarea a reușit
 * - Inițializează key cu valoarea primită
 * - Inițializează left și right cu NULL
 * - Inițializează height cu 1 (nod frunză)
 *
 * @param key Valoarea de stocat în nod
 * @return Pointer către nodul nou creat
 */
AVLNode* create_node(int key) {
    /* YOUR CODE HERE */
    return NULL;  /* Înlocuiește această linie */
}

/* =============================================================================
 * ROTAȚII
 * =============================================================================
 */

/**
 * TODO 6: Implementează funcția rotate_right (Rotație la Dreapta)
 *
 * Aplicată pentru cazul LL (Left-Left).
 *
 *       z                    y
 *      / \                  / \
 *     y   T4    =>        x    z
 *    / \                      / \
 *   x   T3                  T3   T4
 *
 * Pași:
 *   1. Salvează y = z->left
 *   2. Salvează T3 = y->right
 *   3. Efectuează rotația: y->right = z, z->left = T3
 *   4. Actualizează înălțimile (z primul, apoi y)
 *   5. Returnează y (noua rădăcină)
 *
 * @param z Nodul dezechilibrat
 * @return Noua rădăcină a subarborelui
 */
AVLNode* rotate_right(AVLNode *z) {
    /* YOUR CODE HERE */
    return NULL;  /* Înlocuiește această linie */
}

/**
 * TODO 7: Implementează funcția rotate_left (Rotație la Stânga)
 *
 * Aplicată pentru cazul RR (Right-Right).
 *
 *     z                      y
 *    / \                    / \
 *  T1   y       =>        z    x
 *      / \               / \
 *    T2   x            T1   T2
 *
 * Pași:
 *   1. Salvează y = z->right
 *   2. Salvează T2 = y->left
 *   3. Efectuează rotația: y->left = z, z->right = T2
 *   4. Actualizează înălțimile (z primul, apoi y)
 *   5. Returnează y (noua rădăcină)
 *
 * @param z Nodul dezechilibrat
 * @return Noua rădăcină a subarborelui
 */
AVLNode* rotate_left(AVLNode *z) {
    /* YOUR CODE HERE */
    return NULL;  /* Înlocuiește această linie */
}

/* =============================================================================
 * INSERARE
 * =============================================================================
 */

/**
 * TODO 8: Implementează funcția insert cu auto-echilibrare
 *
 * Algoritmul:
 *   1. Inserare BST standard (recursiv)
 *      - Dacă node == NULL, creează nod nou
 *      - Dacă key < node->key, inserează în stânga
 *      - Dacă key > node->key, inserează în dreapta
 *      - Dacă key == node->key, returnează node (duplicate nepermise)
 *
 *   2. Actualizează înălțimea nodului curent
 *
 *   3. Calculează balance factor
 *
 *   4. Verifică cele 4 cazuri de dezechilibru:
 *      - LL: bf > 1 && key < node->left->key  => rotate_right
 *      - RR: bf < -1 && key > node->right->key => rotate_left
 *      - LR: bf > 1 && key > node->left->key  => rotate_left(left), rotate_right
 *      - RL: bf < -1 && key < node->right->key => rotate_right(right), rotate_left
 *
 * @param node Rădăcina subarborelui curent
 * @param key Cheia de inserat
 * @return Noua rădăcină a subarborelui
 */
AVLNode* insert(AVLNode *node, int key) {
    /* PASUL 1: Inserare BST standard */
    /* YOUR CODE HERE */
    
    
    /* PASUL 2: Actualizare înălțime */
    /* YOUR CODE HERE */
    
    
    /* PASUL 3: Calcul balance factor */
    /* YOUR CODE HERE */
    
    
    /* PASUL 4: Verificare și aplicare rotații */
    
    /* Cazul LL */
    /* YOUR CODE HERE */
    
    /* Cazul RR */
    /* YOUR CODE HERE */
    
    /* Cazul LR */
    /* YOUR CODE HERE */
    
    /* Cazul RL */
    /* YOUR CODE HERE */
    
    return node;
}

/* =============================================================================
 * PARCURGERI
 * =============================================================================
 */

/**
 * TODO 9: Implementează parcurgerea inorder
 *
 * Ordine: Stânga -> Rădăcină -> Dreapta
 * Rezultat: Elementele în ordine crescătoare
 *
 * @param root Rădăcina arborelui
 */
void inorder(AVLNode *root) {
    /* YOUR CODE HERE */
}

/**
 * TODO 10: Implementează parcurgerea preorder
 *
 * Ordine: Rădăcină -> Stânga -> Dreapta
 *
 * @param root Rădăcina arborelui
 */
void preorder(AVLNode *root) {
    /* YOUR CODE HERE */
}

/**
 * TODO 11: Implementează parcurgerea postorder
 *
 * Ordine: Stânga -> Dreapta -> Rădăcină
 *
 * @param root Rădăcina arborelui
 */
void postorder(AVLNode *root) {
    /* YOUR CODE HERE */
}

/* =============================================================================
 * FUNCȚII UTILITARE (FURNIZATE)
 * =============================================================================
 */

/**
 * Numără nodurile din arbore.
 */
int count_nodes(AVLNode *root) {
    if (root == NULL) return 0;
    return 1 + count_nodes(root->left) + count_nodes(root->right);
}

/**
 * Verifică dacă arborele respectă proprietatea AVL.
 */
bool is_valid_avl(AVLNode *root) {
    if (root == NULL) return true;
    
    int bf = balance_factor(root);
    if (bf < -1 || bf > 1) return false;
    
    return is_valid_avl(root->left) && is_valid_avl(root->right);
}

/**
 * Afișează arborele cu indentare.
 */
void print_tree_helper(AVLNode *root, int space) {
    if (root == NULL) return;
    
    space += 5;
    print_tree_helper(root->right, space);
    
    printf("\n");
    for (int i = 5; i < space; i++) printf(" ");
    printf("%d(bf=%d)", root->key, balance_factor(root));
    
    print_tree_helper(root->left, space);
}

void print_tree(AVLNode *root) {
    printf("\n=== Structura Arborelui ===");
    print_tree_helper(root, 0);
    printf("\n===========================\n");
}

/**
 * Eliberează memoria arborelui.
 */
void free_tree(AVLNode *root) {
    if (root != NULL) {
        free_tree(root->left);
        free_tree(root->right);
        free(root);
    }
}

/* =============================================================================
 * FUNCȚIA MAIN
 * =============================================================================
 */

int main(void) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     EXERCIȚIUL 1: Arbore AVL de Bază                          ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    AVLNode *root = NULL;
    int n, key;
    
    /* Citire date */
    printf("Introduceți numărul de elemente: ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        fprintf(stderr, "Eroare: număr invalid de elemente\n");
        return 1;
    }
    
    printf("Introduceți %d elemente: ", n);
    for (int i = 0; i < n; i++) {
        if (scanf("%d", &key) != 1) {
            fprintf(stderr, "Eroare: element invalid\n");
            free_tree(root);
            return 1;
        }
        
        /* TODO 12: Apelează funcția insert și actualizează root */
        /* YOUR CODE HERE */
        root = insert(root, key);
    }
    
    /* Afișare rezultate */
    printf("\nArbore AVL construit cu %d noduri\n", count_nodes(root));
    printf("Înălțime: %d\n", height(root));
    
    printf("Parcurgere inorder: ");
    inorder(root);
    printf("\n");
    
    printf("Parcurgere preorder: ");
    preorder(root);
    printf("\n");
    
    printf("Parcurgere postorder: ");
    postorder(root);
    printf("\n");
    
    if (root != NULL) {
        printf("Factor echilibru rădăcină: %d\n", balance_factor(root));
    }
    
    printf("Valid AVL: %s\n", is_valid_avl(root) ? "DA" : "NU");
    
    /* Afișare vizuală */
    print_tree(root);
    
    /* Eliberare memorie */
    free_tree(root);
    
    printf("\n✓ Exercițiu completat!\n");
    
    return 0;
}

/* =============================================================================
 * PROVOCĂRI BONUS (Opțional)
 * =============================================================================
 *
 * 1. Implementează funcția count_leaves() care numără frunzele arborelui
 *
 * 2. Implementează funcția level_order() pentru parcurgere pe niveluri (BFS)
 *
 * 3. Adaugă afișare colorată pentru noduri în funcție de balance factor
 *
 * 4. Implementează funcția is_bst() care verifică proprietatea BST
 *
 * =============================================================================
 */
