/**
 * =============================================================================
 * EXERCIȚIUL 2: Arbore AVL Complet cu Ștergere
 * =============================================================================
 *
 * OBIECTIV:
 *   Extinderea implementării AVL cu operația de ștergere cu reechilibrare,
 *   căutare și funcționalități avansate precum meniu interactiv și statistici.
 *
 * CERINȚE:
 *   1. Implementarea funcției delete_node() cu reechilibrare
 *   2. Găsirea succesorului inorder pentru noduri cu doi copii
 *   3. Implementarea funcției search()
 *   4. Calcularea și afișarea statisticilor arborelui
 *   5. Validarea proprietății AVL după fiecare operație
 *   6. Implementarea unui meniu interactiv pentru operații CRUD
 *
 * EXEMPLU INPUT:
 *   1
 *   50
 *   1
 *   30
 *   1
 *   70
 *   3
 *   30
 *   2
 *   30
 *   4
 *   5
 *   0
 *
 * EXPECTED OUTPUT:
 *   [output corespunzător operațiilor din meniu]
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -o exercise2 exercise2.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

/* =============================================================================
 * STRUCTURĂ ȘI FUNCȚII DE BAZĂ (FURNIZATE)
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
 * FUNCȚII DE IMPLEMENTAT
 * =============================================================================
 */

/**
 * TODO 1: Implementează funcția min_value_node
 *
 * Găsește nodul cu valoarea minimă dintr-un subarbore.
 * Acesta este cel mai din stânga nod (parcurgere spre stânga până la NULL).
 *
 * Utilizare: Pentru găsirea succesorului inorder la ștergerea
 *            unui nod cu doi copii.
 *
 * @param node Rădăcina subarborelui
 * @return Pointer către nodul cu valoarea minimă
 */
AVLNode* min_value_node(AVLNode *node) {
    /* YOUR CODE HERE */
    /* Hint: Parcurge în stânga până când left == NULL */
    return NULL;  /* Înlocuiește această linie */
}

/**
 * TODO 2: Implementează funcția max_value_node
 *
 * Găsește nodul cu valoarea maximă dintr-un subarbore.
 *
 * @param node Rădăcina subarborelui
 * @return Pointer către nodul cu valoarea maximă
 */
AVLNode* max_value_node(AVLNode *node) {
    /* YOUR CODE HERE */
    /* Hint: Similar cu min_value_node, dar spre dreapta */
    return NULL;  /* Înlocuiește această linie */
}

/**
 * TODO 3: Implementează funcția search
 *
 * Caută o cheie în arborele AVL.
 *
 * Algoritmul:
 *   - Dacă root == NULL sau root->key == key, returnează root
 *   - Dacă key < root->key, caută în stânga
 *   - Dacă key > root->key, caută în dreapta
 *
 * @param root Rădăcina arborelui
 * @param key Cheia căutată
 * @return Pointer către nodul găsit sau NULL
 */
AVLNode* search(AVLNode *root, int key) {
    /* YOUR CODE HERE */
    return NULL;  /* Înlocuiește această linie */
}

/**
 * TODO 4: Implementează funcția delete_node cu reechilibrare
 *
 * Algoritmul de ștergere:
 *   1. Căutare BST standard pentru nodul de șters
 *
 *   2. Trei cazuri de ștergere:
 *      a) Nod frunză (fără copii): ștergere directă
 *      b) Nod cu un copil: înlocuire cu copilul
 *      c) Nod cu doi copii: 
 *         - Găsește succesorul inorder (min din dreapta)
 *         - Copiază cheia succesorului în nodul curent
 *         - Șterge succesorul din subarborele drept
 *
 *   3. Actualizează înălțimea
 *
 *   4. Verifică balance factor și aplică rotații:
 *      - LL: bf > 1 && balance_factor(left) >= 0
 *      - LR: bf > 1 && balance_factor(left) < 0
 *      - RR: bf < -1 && balance_factor(right) <= 0
 *      - RL: bf < -1 && balance_factor(right) > 0
 *
 * @param root Rădăcina subarborelui
 * @param key Cheia de șters
 * @return Noua rădăcină a subarborelui
 */
AVLNode* delete_node(AVLNode *root, int key) {
    /* ═══════════════════════════════════════════════════════════════════════
     * PASUL 1: Ștergere BST standard
     * ═══════════════════════════════════════════════════════════════════════
     */
    
    /* Caz de bază: arbore gol */
    if (root == NULL) {
        return root;
    }
    
    /* Navigare către nodul de șters */
    if (key < root->key) {
        /* YOUR CODE HERE - recursie în stânga */
        
    } else if (key > root->key) {
        /* YOUR CODE HERE - recursie în dreapta */
        
    } else {
        /* Nodul de șters găsit */
        
        /* TODO 5: Implementează cazul cu zero sau un copil */
        if (root->left == NULL || root->right == NULL) {
            /* YOUR CODE HERE */
            /* Hint: 
             *   - temp = root->left dacă există, altfel root->right
             *   - Dacă temp == NULL (frunză): eliberează root, returnează NULL
             *   - Altfel: copiază conținutul lui temp în root, eliberează temp
             */
            
        } else {
            /* TODO 6: Implementează cazul cu doi copii */
            /* YOUR CODE HERE */
            /* Hint:
             *   - Găsește succesorul inorder (min din dreapta)
             *   - Copiază cheia succesorului în root->key
             *   - Șterge succesorul din subarborele drept
             */
            
        }
    }
    
    /* Dacă arborele avea un singur nod */
    if (root == NULL) {
        return root;
    }
    
    /* ═══════════════════════════════════════════════════════════════════════
     * PASUL 2: Actualizare înălțime
     * ═══════════════════════════════════════════════════════════════════════
     */
    /* YOUR CODE HERE */
    
    
    /* ═══════════════════════════════════════════════════════════════════════
     * PASUL 3: Reechilibrare
     * ═══════════════════════════════════════════════════════════════════════
     */
    int bf = balance_factor(root);
    
    /* TODO 7: Implementează cele 4 cazuri de rotație */
    
    /* Cazul LL */
    if (bf > 1 && balance_factor(root->left) >= 0) {
        /* YOUR CODE HERE */
    }
    
    /* Cazul LR */
    if (bf > 1 && balance_factor(root->left) < 0) {
        /* YOUR CODE HERE */
    }
    
    /* Cazul RR */
    if (bf < -1 && balance_factor(root->right) <= 0) {
        /* YOUR CODE HERE */
    }
    
    /* Cazul RL */
    if (bf < -1 && balance_factor(root->right) > 0) {
        /* YOUR CODE HERE */
    }
    
    return root;
}

/* =============================================================================
 * FUNCȚII STATISTICI (DE IMPLEMENTAT)
 * =============================================================================
 */

/**
 * TODO 8: Implementează funcția count_nodes
 *
 * Numără toate nodurile din arbore.
 *
 * @param root Rădăcina arborelui
 * @return Numărul de noduri
 */
int count_nodes(AVLNode *root) {
    /* YOUR CODE HERE */
    return 0;  /* Înlocuiește această linie */
}

/**
 * TODO 9: Implementează funcția count_leaves
 *
 * Numără nodurile frunză (fără copii).
 *
 * @param root Rădăcina arborelui
 * @return Numărul de frunze
 */
int count_leaves(AVLNode *root) {
    /* YOUR CODE HERE */
    return 0;  /* Înlocuiește această linie */
}

/**
 * TODO 10: Implementează funcția sum_keys
 *
 * Calculează suma tuturor cheilor din arbore.
 *
 * @param root Rădăcina arborelui
 * @return Suma cheilor
 */
int sum_keys(AVLNode *root) {
    /* YOUR CODE HERE */
    return 0;  /* Înlocuiește această linie */
}

/**
 * TODO 11: Implementează funcția is_valid_avl
 *
 * Verifică dacă arborele respectă proprietatea AVL.
 * (|balance_factor| <= 1 pentru fiecare nod)
 *
 * @param root Rădăcina arborelui
 * @return true dacă valid AVL, false altfel
 */
bool is_valid_avl(AVLNode *root) {
    /* YOUR CODE HERE */
    return true;  /* Înlocuiește această linie */
}

/**
 * TODO 12: Implementează funcția is_valid_bst
 *
 * Verifică dacă arborele respectă proprietatea BST.
 * (toate cheile din stânga < rădăcină < toate cheile din dreapta)
 *
 * Hint: Folosește parametri min și max pentru a verifica intervalul valid.
 *
 * @param root Rădăcina arborelui
 * @param min Valoarea minimă permisă
 * @param max Valoarea maximă permisă
 * @return true dacă valid BST, false altfel
 */
bool is_valid_bst_helper(AVLNode *root, int min_val, int max_val) {
    /* YOUR CODE HERE */
    return true;  /* Înlocuiește această linie */
}

bool is_valid_bst(AVLNode *root) {
    return is_valid_bst_helper(root, INT_MIN, INT_MAX);
}

/* =============================================================================
 * FUNCȚII UTILITARE (FURNIZATE)
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
            /* Curăță buffer-ul în caz de input invalid */
            while (getchar() != '\n');
            printf("Opțiune invalidă. Încercați din nou.\n");
            continue;
        }
        
        switch (choice) {
            case 1:  /* Inserare */
                printf("Introduceți valoarea de inserat: ");
                if (scanf("%d", &key) == 1) {
                    root = insert(root, key);
                    printf("✓ Valoarea %d a fost inserată.\n", key);
                }
                break;
                
            case 2:  /* Ștergere */
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
                
            case 3:  /* Căutare */
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
                
            case 4:  /* Afișare */
                print_tree(root);
                break;
                
            case 5:  /* Statistici */
                print_statistics(root);
                break;
                
            case 6:  /* Parcurgere inorder */
                printf("Parcurgere inorder: ");
                inorder(root);
                printf("\n");
                break;
                
            case 7:  /* Validare */
                printf("Arbore valid AVL: %s\n", is_valid_avl(root) ? "DA ✓" : "NU ✗");
                printf("Arbore valid BST: %s\n", is_valid_bst(root) ? "DA ✓" : "NU ✗");
                break;
                
            case 0:  /* Ieșire */
                running = false;
                printf("La revedere!\n");
                break;
                
            default:
                printf("Opțiune invalidă. Alegeți 0-7.\n");
        }
    }
    
    /* Eliberare memorie */
    free_tree(root);
    
    printf("\n✓ Memorie eliberată. Program terminat.\n");
    
    return 0;
}

/* =============================================================================
 * PROVOCĂRI BONUS (Opțional)
 * =============================================================================
 *
 * 1. Implementează funcția range_query(root, min, max) care returnează
 *    toate elementele din intervalul [min, max].
 *
 * 2. Implementează funcția kth_smallest(root, k) care găsește al k-lea
 *    cel mai mic element din arbore.
 *
 * 3. Implementează funcția lca(root, a, b) care găsește cel mai apropiat
 *    strămoș comun (Lowest Common Ancestor) pentru nodurile cu cheile a și b.
 *
 * 4. Adaugă funcționalitate de salvare/încărcare arbore din fișier.
 *
 * 5. Implementează un iterator pentru parcurgerea arborelui fără recursie.
 *
 * =============================================================================
 */
