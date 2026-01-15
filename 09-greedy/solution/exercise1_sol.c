/**
 * =============================================================================
 * SOLUȚIE EXERCIȚIUL 1: Arbore AVL de Bază
 * =============================================================================
 * 
 * ATENȚIE: Acest fișier conține soluția completă.
 *          Studenții ar trebui să încerce să rezolve exercițiul
 *          înainte de a consulta această soluție.
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

/* SOLUȚIE TODO 1: Structura AVLNode */
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

/* SOLUȚIE TODO 2: Funcția max */
int max(int a, int b) {
    return (a > b) ? a : b;
}

/* SOLUȚIE TODO 3: Funcția height */
int height(AVLNode *node) {
    if (node == NULL) {
        return 0;
    }
    return node->height;
}

/* SOLUȚIE TODO 4: Funcția balance_factor */
int balance_factor(AVLNode *node) {
    if (node == NULL) {
        return 0;
    }
    return height(node->left) - height(node->right);
}

/* SOLUȚIE TODO 5: Funcția create_node */
AVLNode* create_node(int key) {
    AVLNode *node = (AVLNode*)malloc(sizeof(AVLNode));
    if (node == NULL) {
        fprintf(stderr, "Eroare: Alocare memorie eșuată!\n");
        exit(EXIT_FAILURE);
    }
    node->key = key;
    node->left = NULL;
    node->right = NULL;
    node->height = 1;  /* Nodul nou este o frunză */
    return node;
}

/* =============================================================================
 * ROTAȚII
 * =============================================================================
 */

/* SOLUȚIE TODO 6: Rotație la Dreapta */
AVLNode* rotate_right(AVLNode *z) {
    AVLNode *y = z->left;
    AVLNode *T3 = y->right;
    
    /* Efectuăm rotația */
    y->right = z;
    z->left = T3;
    
    /* Actualizăm înălțimile (ordinea este importantă!) */
    z->height = 1 + max(height(z->left), height(z->right));
    y->height = 1 + max(height(y->left), height(y->right));
    
    return y;  /* Returnăm noua rădăcină */
}

/* SOLUȚIE TODO 7: Rotație la Stânga */
AVLNode* rotate_left(AVLNode *z) {
    AVLNode *y = z->right;
    AVLNode *T2 = y->left;
    
    /* Efectuăm rotația */
    y->left = z;
    z->right = T2;
    
    /* Actualizăm înălțimile */
    z->height = 1 + max(height(z->left), height(z->right));
    y->height = 1 + max(height(y->left), height(y->right));
    
    return y;  /* Returnăm noua rădăcină */
}

/* =============================================================================
 * INSERARE
 * =============================================================================
 */

/* SOLUȚIE TODO 8: Funcția insert cu auto-echilibrare */
AVLNode* insert(AVLNode *node, int key) {
    /* PASUL 1: Inserare BST standard */
    if (node == NULL) {
        return create_node(key);
    }
    
    if (key < node->key) {
        node->left = insert(node->left, key);
    } else if (key > node->key) {
        node->right = insert(node->right, key);
    } else {
        /* Cheile duplicate nu sunt permise */
        return node;
    }
    
    /* PASUL 2: Actualizare înălțime */
    node->height = 1 + max(height(node->left), height(node->right));
    
    /* PASUL 3: Calcul balance factor */
    int bf = balance_factor(node);
    
    /* PASUL 4: Verificare și aplicare rotații */
    
    /* Cazul LL */
    if (bf > 1 && key < node->left->key) {
        return rotate_right(node);
    }
    
    /* Cazul RR */
    if (bf < -1 && key > node->right->key) {
        return rotate_left(node);
    }
    
    /* Cazul LR */
    if (bf > 1 && key > node->left->key) {
        node->left = rotate_left(node->left);
        return rotate_right(node);
    }
    
    /* Cazul RL */
    if (bf < -1 && key < node->right->key) {
        node->right = rotate_right(node->right);
        return rotate_left(node);
    }
    
    return node;
}

/* =============================================================================
 * PARCURGERI
 * =============================================================================
 */

/* SOLUȚIE TODO 9: Parcurgere inorder */
void inorder(AVLNode *root) {
    if (root != NULL) {
        inorder(root->left);
        printf("%d ", root->key);
        inorder(root->right);
    }
}

/* SOLUȚIE TODO 10: Parcurgere preorder */
void preorder(AVLNode *root) {
    if (root != NULL) {
        printf("%d ", root->key);
        preorder(root->left);
        preorder(root->right);
    }
}

/* SOLUȚIE TODO 11: Parcurgere postorder */
void postorder(AVLNode *root) {
    if (root != NULL) {
        postorder(root->left);
        postorder(root->right);
        printf("%d ", root->key);
    }
}

/* =============================================================================
 * FUNCȚII UTILITARE
 * =============================================================================
 */

int count_nodes(AVLNode *root) {
    if (root == NULL) return 0;
    return 1 + count_nodes(root->left) + count_nodes(root->right);
}

bool is_valid_avl(AVLNode *root) {
    if (root == NULL) return true;
    
    int bf = balance_factor(root);
    if (bf < -1 || bf > 1) return false;
    
    return is_valid_avl(root->left) && is_valid_avl(root->right);
}

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
        
        /* SOLUȚIE TODO 12: Apelul funcției insert */
        root = insert(root, key);
    }
    
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
    
    print_tree(root);
    
    free_tree(root);
    
    printf("\n✓ Exercițiu completat!\n");
    
    return 0;
}
