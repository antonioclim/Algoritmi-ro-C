/**
 * =============================================================================
 * SOLUȚIE EXERCIȚIUL 2: Arbore AVL Complet cu Ștergere
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
#include <limits.h>

/* =============================================================================
 * STRUCTURĂ ȘI FUNCȚII DE BAZĂ
 * =============================================================================
 */

typedef struct AVLNode {
    int key;
    struct AVLNode *left;
    struct AVLNode *right;
    int height;
} AVLNode;

int max(int a, int b) {
    return (a > b) ? a : b;
}

int height(AVLNode *node) {
    return (node == NULL) ? 0 : node->height;
}

int balance_factor(AVLNode *node) {
    if (node == NULL) return 0;
    return height(node->left) - height(node->right);
}

void update_height(AVLNode *node) {
    if (node != NULL) {
        node->height = 1 + max(height(node->left), height(node->right));
    }
}

AVLNode* create_node(int key) {
    AVLNode *node = (AVLNode*)malloc(sizeof(AVLNode));
    if (node == NULL) {
        fprintf(stderr, "Eroare: Alocare memorie eșuată!\n");
        exit(EXIT_FAILURE);
    }
    node->key = key;
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    return node;
}

AVLNode* rotate_right(AVLNode *z) {
    AVLNode *y = z->left;
    AVLNode *T3 = y->right;
    
    y->right = z;
    z->left = T3;
    
    update_height(z);
    update_height(y);
    
    return y;
}

AVLNode* rotate_left(AVLNode *z) {
    AVLNode *y = z->right;
    AVLNode *T2 = y->left;
    
    y->left = z;
    z->right = T2;
    
    update_height(z);
    update_height(y);
    
    return y;
}

AVLNode* insert(AVLNode *node, int key) {
    if (node == NULL) return create_node(key);
    
    if (key < node->key)
        node->left = insert(node->left, key);
    else if (key > node->key)
        node->right = insert(node->right, key);
    else
        return node;
    
    update_height(node);
    
    int bf = balance_factor(node);
    
    if (bf > 1 && key < node->left->key)
        return rotate_right(node);
    if (bf < -1 && key > node->right->key)
        return rotate_left(node);
    if (bf > 1 && key > node->left->key) {
        node->left = rotate_left(node->left);
        return rotate_right(node);
    }
    if (bf < -1 && key < node->right->key) {
        node->right = rotate_right(node->right);
        return rotate_left(node);
    }
    
    return node;
}

void inorder(AVLNode *root) {
    if (root != NULL) {
        inorder(root->left);
        printf("%d ", root->key);
        inorder(root->right);
    }
}

/* =============================================================================
 * SOLUȚII PENTRU FUNCȚIILE DE IMPLEMENTAT
 * =============================================================================
 */

/* SOLUȚIE TODO 1: min_value_node */
AVLNode* min_value_node(AVLNode *node) {
    AVLNode *current = node;
    while (current != NULL && current->left != NULL) {
        current = current->left;
    }
    return current;
}

/* SOLUȚIE TODO 2: max_value_node */
AVLNode* max_value_node(AVLNode *node) {
    AVLNode *current = node;
    while (current != NULL && current->right != NULL) {
        current = current->right;
    }
    return current;
}

/* SOLUȚIE TODO 3: search */
AVLNode* search(AVLNode *root, int key) {
    if (root == NULL || root->key == key) {
        return root;
    }
    
    if (key < root->key) {
        return search(root->left, key);
    }
    return search(root->right, key);
}

/* SOLUȚIE TODO 4: delete_node cu reechilibrare */
AVLNode* delete_node(AVLNode *root, int key) {
    /* PASUL 1: Ștergere BST standard */
    if (root == NULL) {
        return root;
    }
    
    if (key < root->key) {
        root->left = delete_node(root->left, key);
    } else if (key > root->key) {
        root->right = delete_node(root->right, key);
    } else {
        /* Nodul de șters găsit */
        
        /* SOLUȚIE TODO 5: Cazul cu zero sau un copil */
        if (root->left == NULL || root->right == NULL) {
            AVLNode *temp = root->left ? root->left : root->right;
            
            if (temp == NULL) {
                /* Nod frunză */
                temp = root;
                root = NULL;
            } else {
                /* Un singur copil - copiem conținutul */
                *root = *temp;
            }
            free(temp);
        } else {
            /* SOLUȚIE TODO 6: Cazul cu doi copii */
            AVLNode *temp = min_value_node(root->right);
            root->key = temp->key;
            root->right = delete_node(root->right, temp->key);
        }
    }
    
    if (root == NULL) {
        return root;
    }
    
    /* PASUL 2: Actualizare înălțime */
    update_height(root);
    
    /* PASUL 3: Reechilibrare */
    int bf = balance_factor(root);
    
    /* SOLUȚIE TODO 7: Cele 4 cazuri de rotație */
    
    /* Cazul LL */
    if (bf > 1 && balance_factor(root->left) >= 0) {
        return rotate_right(root);
    }
    
    /* Cazul LR */
    if (bf > 1 && balance_factor(root->left) < 0) {
        root->left = rotate_left(root->left);
        return rotate_right(root);
    }
    
    /* Cazul RR */
    if (bf < -1 && balance_factor(root->right) <= 0) {
        return rotate_left(root);
    }
    
    /* Cazul RL */
    if (bf < -1 && balance_factor(root->right) > 0) {
        root->right = rotate_right(root->right);
        return rotate_left(root);
    }
    
    return root;
}

/* SOLUȚIE TODO 8: count_nodes */
int count_nodes(AVLNode *root) {
    if (root == NULL) {
        return 0;
    }
    return 1 + count_nodes(root->left) + count_nodes(root->right);
}

/* SOLUȚIE TODO 9: count_leaves */
int count_leaves(AVLNode *root) {
    if (root == NULL) {
        return 0;
    }
    if (root->left == NULL && root->right == NULL) {
        return 1;
    }
    return count_leaves(root->left) + count_leaves(root->right);
}

/* SOLUȚIE TODO 10: sum_keys */
int sum_keys(AVLNode *root) {
    if (root == NULL) {
        return 0;
    }
    return root->key + sum_keys(root->left) + sum_keys(root->right);
}

/* SOLUȚIE TODO 11: is_valid_avl */
bool is_valid_avl(AVLNode *root) {
    if (root == NULL) {
        return true;
    }
    
    int bf = balance_factor(root);
    if (bf < -1 || bf > 1) {
        return false;
    }
    
    return is_valid_avl(root->left) && is_valid_avl(root->right);
}

/* SOLUȚIE TODO 12: is_valid_bst */
bool is_valid_bst_helper(AVLNode *root, int min_val, int max_val) {
    if (root == NULL) {
        return true;
    }
    
    if (root->key <= min_val || root->key >= max_val) {
        return false;
    }
    
    return is_valid_bst_helper(root->left, min_val, root->key) &&
           is_valid_bst_helper(root->right, root->key, max_val);
}

bool is_valid_bst(AVLNode *root) {
    return is_valid_bst_helper(root, INT_MIN, INT_MAX);
}

/* =============================================================================
 * FUNCȚII UTILITARE
 * =============================================================================
 */

void print_tree_helper(AVLNode *root, int space) {
    if (root == NULL) return;
    space += 6;
    print_tree_helper(root->right, space);
    printf("\n");
    for (int i = 6; i < space; i++) printf(" ");
    printf("%d(bf=%d)", root->key, balance_factor(root));
    print_tree_helper(root->left, space);
}

void print_tree(AVLNode *root) {
    printf("\n══════ Structura Arborelui ══════");
    if (root == NULL) {
        printf("\n  (arbore gol)");
    } else {
        print_tree_helper(root, 0);
    }
    printf("\n═════════════════════════════════\n");
}

void print_statistics(AVLNode *root) {
    printf("\n══════ Statistici Arbore ══════\n");
    printf("  Număr noduri:    %d\n", count_nodes(root));
    printf("  Număr frunze:    %d\n", count_leaves(root));
    printf("  Înălțime:        %d\n", height(root));
    printf("  Suma cheilor:    %d\n", sum_keys(root));
    
    AVLNode *min_node = (root != NULL) ? min_value_node(root) : NULL;
    AVLNode *max_node = (root != NULL) ? max_value_node(root) : NULL;
    
    printf("  Valoare minimă:  %d\n", min_node ? min_node->key : 0);
    printf("  Valoare maximă:  %d\n", max_node ? max_node->key : 0);
    printf("  Valid AVL:       %s\n", is_valid_avl(root) ? "DA" : "NU");
    printf("  Valid BST:       %s\n", is_valid_bst(root) ? "DA" : "NU");
    printf("═══════════════════════════════\n");
}

void free_tree(AVLNode *root) {
    if (root != NULL) {
        free_tree(root->left);
        free_tree(root->right);
        free(root);
    }
}

void print_menu(void) {
    printf("\n═══════════════════════════════════════\n");
    printf("         MENIU ARBORE AVL\n");
    printf("═══════════════════════════════════════\n");
    printf("  1. Inserare element\n");
    printf("  2. Ștergere element\n");
    printf("  3. Căutare element\n");
    printf("  4. Afișare arbore\n");
    printf("  5. Statistici\n");
    printf("  6. Parcurgere inorder\n");
    printf("  7. Validare AVL\n");
    printf("  0. Ieșire\n");
    printf("═══════════════════════════════════════\n");
    printf("Opțiune: ");
}

/* =============================================================================
 * FUNCȚIA MAIN
 * =============================================================================
 */

int main(void) {
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     EXERCIȚIUL 2: Arbore AVL Complet cu Ștergere              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    AVLNode *root = NULL;
    int choice, key;
    bool running = true;
    
    while (running) {
        print_menu();
        
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("Opțiune invalidă. Încercați din nou.\n");
            continue;
        }
        
        switch (choice) {
            case 1:
                printf("Introduceți valoarea de inserat: ");
                if (scanf("%d", &key) == 1) {
                    root = insert(root, key);
                    printf("✓ Valoarea %d a fost inserată.\n", key);
                }
                break;
                
            case 2:
                printf("Introduceți valoarea de șters: ");
                if (scanf("%d", &key) == 1) {
                    AVLNode *found = search(root, key);
                    if (found != NULL) {
                        root = delete_node(root, key);
                        printf("✓ Valoarea %d a fost ștearsă.\n", key);
                    } else {
                        printf("✗ Valoarea %d nu există în arbore.\n", key);
                    }
                }
                break;
                
            case 3:
                printf("Introduceți valoarea de căutat: ");
                if (scanf("%d", &key) == 1) {
                    AVLNode *result = search(root, key);
                    if (result != NULL) {
                        printf("✓ Valoarea %d a fost GĂSITĂ.\n", key);
                    } else {
                        printf("✗ Valoarea %d NU a fost găsită.\n", key);
                    }
                }
                break;
                
            case 4:
                print_tree(root);
                break;
                
            case 5:
                print_statistics(root);
                break;
                
            case 6:
                printf("Parcurgere inorder: ");
                inorder(root);
                printf("\n");
                break;
                
            case 7:
                printf("Arbore valid AVL: %s\n", is_valid_avl(root) ? "DA ✓" : "NU ✗");
                printf("Arbore valid BST: %s\n", is_valid_bst(root) ? "DA ✓" : "NU ✗");
                break;
                
            case 0:
                running = false;
                printf("La revedere!\n");
                break;
                
            default:
                printf("Opțiune invalidă. Alegeți 0-7.\n");
        }
    }
    
    free_tree(root);
    
    printf("\n✓ Memorie eliberată. Program terminat.\n");
    
    return 0;
}
