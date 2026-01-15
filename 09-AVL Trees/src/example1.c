/**
 * =============================================================================
 * SĂPTĂMÂNA 09: ARBORI AVL (Adelson-Velsky și Landis)
 * Exemplu Complet de Demonstrație
 * =============================================================================
 *
 * Acest exemplu demonstrează:
 *   1. Structura și crearea nodurilor AVL
 *   2. Calculul înălțimii și al balance factor
 *   3. Cele patru tipuri de rotații (LL, RR, LR, RL)
 *   4. Inserarea cu auto-echilibrare
 *   5. Ștergerea cu auto-echilibrare
 *   6. Căutarea în arbore
 *   7. Parcurgerile (inorder, preorder, postorder)
 *   8. Afișarea vizuală a arborelui
 *   9. Validarea proprietății AVL
 *  10. Eliberarea corectă a memoriei
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -o example1 example1.c
 * Utilizare: ./example1
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* =============================================================================
 * CONSTANTE ȘI DEFINIȚII
 * =============================================================================
 */

#define MAX_TREE_HEIGHT 100

/* Culori ANSI pentru output frumos în terminal */
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[0;31m"
#define COLOR_GREEN   "\033[0;32m"
#define COLOR_YELLOW  "\033[0;33m"
#define COLOR_BLUE    "\033[0;34m"
#define COLOR_MAGENTA "\033[0;35m"
#define COLOR_CYAN    "\033[0;36m"
#define COLOR_BOLD    "\033[1m"

/* =============================================================================
 * STRUCTURA NODULUI AVL
 * =============================================================================
 */

/**
 * Structura unui nod în arborele AVL.
 * Pe lângă câmpurile standard BST (key, left, right),
 * stocăm și înălțimea pentru calcul eficient al balance factor.
 */
typedef struct AVLNode {
    int key;                    /* Valoarea stocată în nod */
    struct AVLNode *left;       /* Pointer către copilul stâng */
    struct AVLNode *right;      /* Pointer către copilul drept */
    int height;                 /* Înălțimea nodului (1 pentru frunze) */
} AVLNode;

/* =============================================================================
 * FUNCȚII AUXILIARE
 * =============================================================================
 */

/**
 * Returnează maximul dintre două numere întregi.
 */
int max(int a, int b) {
    return (a > b) ? a : b;
}

/**
 * Returnează înălțimea unui nod.
 * Convenție: înălțimea unui nod NULL este 0.
 * 
 * @param node Pointer către nod (poate fi NULL)
 * @return Înălțimea nodului sau 0 dacă node este NULL
 */
int height(AVLNode *node) {
    if (node == NULL) {
        return 0;
    }
    return node->height;
}

/**
 * Calculează balance factor pentru un nod.
 * bf = height(left) - height(right)
 * 
 * Valori valide AVL: -1, 0, +1
 * 
 * @param node Pointer către nod
 * @return Balance factor sau 0 dacă node este NULL
 */
int balance_factor(AVLNode *node) {
    if (node == NULL) {
        return 0;
    }
    return height(node->left) - height(node->right);
}

/**
 * Actualizează înălțimea unui nod bazat pe copiii săi.
 * Trebuie apelată după orice modificare a structurii.
 */
void update_height(AVLNode *node) {
    if (node != NULL) {
        node->height = 1 + max(height(node->left), height(node->right));
    }
}

/**
 * Creează un nou nod AVL cu cheia specificată.
 * Nodul nou este inițializat ca frunză (height = 1).
 * 
 * @param key Valoarea de stocat
 * @return Pointer către nodul nou creat
 */
AVLNode* create_node(int key) {
    AVLNode *node = (AVLNode*)malloc(sizeof(AVLNode));
    if (node == NULL) {
        fprintf(stderr, COLOR_RED "Eroare: Alocare memorie eșuată!\n" COLOR_RESET);
        exit(EXIT_FAILURE);
    }
    node->key = key;
    node->left = NULL;
    node->right = NULL;
    node->height = 1;  /* Nodul nou este o frunză */
    return node;
}

/* =============================================================================
 * PARTEA 1: ROTAȚII
 * =============================================================================
 */

/**
 * Rotație simplă la dreapta (Right Rotation).
 * Folosită pentru cazul LL (Left-Left).
 * 
 *       z                    y
 *      / \                  / \
 *     y   T4    =>        x    z
 *    / \                      / \
 *   x   T3                  T3   T4
 * 
 * @param z Nodul dezechilibrat (rădăcina subarborelui)
 * @return Noua rădăcină a subarborelui (y)
 */
AVLNode* rotate_right(AVLNode *z) {
    printf(COLOR_CYAN "  [Rotație DREAPTA pe nodul %d]\n" COLOR_RESET, z->key);
    
    AVLNode *y = z->left;
    AVLNode *T3 = y->right;
    
    /* Efectuăm rotația */
    y->right = z;
    z->left = T3;
    
    /* Actualizăm înălțimile (ordinea este importantă!) */
    update_height(z);  /* z este acum mai jos, actualizăm primul */
    update_height(y);  /* y este noua rădăcină */
    
    return y;
}

/**
 * Rotație simplă la stânga (Left Rotation).
 * Folosită pentru cazul RR (Right-Right).
 * 
 *     z                      y
 *    / \                    / \
 *  T1   y       =>        z    x
 *      / \               / \
 *    T2   x            T1   T2
 * 
 * @param z Nodul dezechilibrat
 * @return Noua rădăcină a subarborelui (y)
 */
AVLNode* rotate_left(AVLNode *z) {
    printf(COLOR_CYAN "  [Rotație STÂNGA pe nodul %d]\n" COLOR_RESET, z->key);
    
    AVLNode *y = z->right;
    AVLNode *T2 = y->left;
    
    /* Efectuăm rotația */
    y->left = z;
    z->right = T2;
    
    /* Actualizăm înălțimile */
    update_height(z);
    update_height(y);
    
    return y;
}

/* =============================================================================
 * PARTEA 2: INSERARE CU AUTO-ECHILIBRARE
 * =============================================================================
 */

/**
 * Inserează o cheie în arborele AVL și menține echilibrul.
 * 
 * Algoritmul:
 *   1. Inserare BST standard (recursiv)
 *   2. Actualizare înălțime pe drumul înapoi
 *   3. Verificare balance factor
 *   4. Aplicare rotație dacă este necesar
 * 
 * @param node Rădăcina subarborelui curent
 * @param key Cheia de inserat
 * @return Noua rădăcină a subarborelui (poate fi diferită după rotații)
 */
AVLNode* insert(AVLNode *node, int key) {
    /* ═══════════════════════════════════════════════════════════════════════
     * PASUL 1: Inserare BST standard
     * ═══════════════════════════════════════════════════════════════════════
     */
    if (node == NULL) {
        printf(COLOR_GREEN "  + Creare nod nou: %d\n" COLOR_RESET, key);
        return create_node(key);
    }
    
    if (key < node->key) {
        node->left = insert(node->left, key);
    } else if (key > node->key) {
        node->right = insert(node->right, key);
    } else {
        /* Cheile duplicate nu sunt permise */
        printf(COLOR_YELLOW "  ! Cheia %d există deja\n" COLOR_RESET, key);
        return node;
    }
    
    /* ═══════════════════════════════════════════════════════════════════════
     * PASUL 2: Actualizare înălțime
     * ═══════════════════════════════════════════════════════════════════════
     */
    update_height(node);
    
    /* ═══════════════════════════════════════════════════════════════════════
     * PASUL 3: Calcul balance factor și detectare dezechilibru
     * ═══════════════════════════════════════════════════════════════════════
     */
    int bf = balance_factor(node);
    
    /* ═══════════════════════════════════════════════════════════════════════
     * PASUL 4: Aplicare rotații pentru reechilibrare
     * ═══════════════════════════════════════════════════════════════════════
     */
    
    /* Cazul LL (Left-Left): bf > 1 și cheia inserată în subarborele stâng-stâng */
    if (bf > 1 && key < node->left->key) {
        printf(COLOR_MAGENTA "  → Cazul LL detectat la nodul %d (bf=%d)\n" COLOR_RESET, 
               node->key, bf);
        return rotate_right(node);
    }
    
    /* Cazul RR (Right-Right): bf < -1 și cheia inserată în subarborele drept-drept */
    if (bf < -1 && key > node->right->key) {
        printf(COLOR_MAGENTA "  → Cazul RR detectat la nodul %d (bf=%d)\n" COLOR_RESET, 
               node->key, bf);
        return rotate_left(node);
    }
    
    /* Cazul LR (Left-Right): bf > 1 și cheia inserată în subarborele stâng-drept */
    if (bf > 1 && key > node->left->key) {
        printf(COLOR_MAGENTA "  → Cazul LR detectat la nodul %d (bf=%d)\n" COLOR_RESET, 
               node->key, bf);
        node->left = rotate_left(node->left);
        return rotate_right(node);
    }
    
    /* Cazul RL (Right-Left): bf < -1 și cheia inserată în subarborele drept-stâng */
    if (bf < -1 && key < node->right->key) {
        printf(COLOR_MAGENTA "  → Cazul RL detectat la nodul %d (bf=%d)\n" COLOR_RESET, 
               node->key, bf);
        node->right = rotate_right(node->right);
        return rotate_left(node);
    }
    
    /* Arborele este deja echilibrat */
    return node;
}

/* =============================================================================
 * PARTEA 3: ȘTERGERE CU AUTO-ECHILIBRARE
 * =============================================================================
 */

/**
 * Găsește nodul cu valoarea minimă dintr-un subarbore.
 * Utilizat pentru găsirea succesorului inorder la ștergere.
 */
AVLNode* min_value_node(AVLNode *node) {
    AVLNode *current = node;
    while (current->left != NULL) {
        current = current->left;
    }
    return current;
}

/**
 * Șterge o cheie din arborele AVL și menține echilibrul.
 * 
 * Cazuri de ștergere:
 *   1. Nodul este frunză: ștergere directă
 *   2. Nodul are un copil: înlocuire cu copilul
 *   3. Nodul are doi copii: înlocuire cu succesorul inorder
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
    if (root == NULL) {
        printf(COLOR_YELLOW "  ! Cheia %d nu a fost găsită\n" COLOR_RESET, key);
        return root;
    }
    
    if (key < root->key) {
        root->left = delete_node(root->left, key);
    } else if (key > root->key) {
        root->right = delete_node(root->right, key);
    } else {
        /* Nodul de șters găsit */
        printf(COLOR_RED "  - Ștergere nod: %d\n" COLOR_RESET, key);
        
        /* Cazul 1 & 2: Nod cu zero sau un copil */
        if (root->left == NULL || root->right == NULL) {
            AVLNode *temp = root->left ? root->left : root->right;
            
            if (temp == NULL) {
                /* Cazul 1: Fără copii (frunză) */
                temp = root;
                root = NULL;
            } else {
                /* Cazul 2: Un singur copil */
                *root = *temp;  /* Copiem conținutul copilului */
            }
            free(temp);
        } else {
            /* Cazul 3: Nod cu doi copii */
            /* Găsim succesorul inorder (cel mai mic din subarborele drept) */
            AVLNode *temp = min_value_node(root->right);
            
            printf(COLOR_CYAN "  → Înlocuire cu succesorul inorder: %d\n" COLOR_RESET, 
                   temp->key);
            
            /* Copiem cheia succesorului */
            root->key = temp->key;
            
            /* Ștergem succesorul */
            root->right = delete_node(root->right, temp->key);
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
    update_height(root);
    
    /* ═══════════════════════════════════════════════════════════════════════
     * PASUL 3: Reechilibrare (similar cu inserarea, dar folosim bf al copilului)
     * ═══════════════════════════════════════════════════════════════════════
     */
    int bf = balance_factor(root);
    
    /* Cazul LL */
    if (bf > 1 && balance_factor(root->left) >= 0) {
        printf(COLOR_MAGENTA "  → Reechilibrare LL după ștergere\n" COLOR_RESET);
        return rotate_right(root);
    }
    
    /* Cazul LR */
    if (bf > 1 && balance_factor(root->left) < 0) {
        printf(COLOR_MAGENTA "  → Reechilibrare LR după ștergere\n" COLOR_RESET);
        root->left = rotate_left(root->left);
        return rotate_right(root);
    }
    
    /* Cazul RR */
    if (bf < -1 && balance_factor(root->right) <= 0) {
        printf(COLOR_MAGENTA "  → Reechilibrare RR după ștergere\n" COLOR_RESET);
        return rotate_left(root);
    }
    
    /* Cazul RL */
    if (bf < -1 && balance_factor(root->right) > 0) {
        printf(COLOR_MAGENTA "  → Reechilibrare RL după ștergere\n" COLOR_RESET);
        root->right = rotate_right(root->right);
        return rotate_left(root);
    }
    
    return root;
}

/* =============================================================================
 * PARTEA 4: CĂUTARE
 * =============================================================================
 */

/**
 * Caută o cheie în arborele AVL (varianta recursivă).
 * 
 * @param root Rădăcina arborelui
 * @param key Cheia căutată
 * @return Pointer către nod sau NULL dacă nu există
 */
AVLNode* search(AVLNode *root, int key) {
    if (root == NULL || root->key == key) {
        return root;
    }
    
    if (key < root->key) {
        return search(root->left, key);
    }
    return search(root->right, key);
}

/**
 * Caută o cheie în arborele AVL (varianta iterativă).
 */
AVLNode* search_iterative(AVLNode *root, int key) {
    while (root != NULL && root->key != key) {
        if (key < root->key) {
            root = root->left;
        } else {
            root = root->right;
        }
    }
    return root;
}

/* =============================================================================
 * PARTEA 5: PARCURGERI (TRAVERSALS)
 * =============================================================================
 */

/**
 * Parcurgere inorder (Stânga - Rădăcină - Dreapta).
 * Afișează elementele în ordine crescătoare.
 */
void inorder(AVLNode *root) {
    if (root != NULL) {
        inorder(root->left);
        printf("%d ", root->key);
        inorder(root->right);
    }
}

/**
 * Parcurgere preorder (Rădăcină - Stânga - Dreapta).
 * Utilă pentru serializare/copiere arbore.
 */
void preorder(AVLNode *root) {
    if (root != NULL) {
        printf("%d ", root->key);
        preorder(root->left);
        preorder(root->right);
    }
}

/**
 * Parcurgere postorder (Stânga - Dreapta - Rădăcină).
 * Utilă pentru ștergerea arborelui.
 */
void postorder(AVLNode *root) {
    if (root != NULL) {
        postorder(root->left);
        postorder(root->right);
        printf("%d ", root->key);
    }
}

/* =============================================================================
 * PARTEA 6: AFIȘARE VIZUALĂ
 * =============================================================================
 */

/**
 * Afișează arborele rotit la 90° (subarborele drept sus).
 * Fiecare nivel este indentat pentru vizualizare clară.
 */
void print_tree_helper(AVLNode *root, int space, int indent) {
    if (root == NULL) {
        return;
    }
    
    space += indent;
    
    /* Procesăm mai întâi subarborele drept */
    print_tree_helper(root->right, space, indent);
    
    /* Afișăm nodul curent */
    printf("\n");
    for (int i = indent; i < space; i++) {
        printf(" ");
    }
    
    int bf = balance_factor(root);
    const char *color = (bf == 0) ? COLOR_GREEN : 
                        (bf == 1 || bf == -1) ? COLOR_YELLOW : COLOR_RED;
    
    printf("%s%d%s(h=%d,bf=%d)", color, root->key, COLOR_RESET, 
           root->height, bf);
    
    /* Procesăm subarborele stâng */
    print_tree_helper(root->left, space, indent);
}

/**
 * Wrapper pentru afișarea arborelui.
 */
void print_tree(AVLNode *root) {
    printf("\n" COLOR_BOLD "═══ Structura Arborelui AVL ═══" COLOR_RESET);
    if (root == NULL) {
        printf("\n  (arbore gol)\n");
    } else {
        print_tree_helper(root, 0, 6);
        printf("\n");
    }
    printf(COLOR_BOLD "═══════════════════════════════\n" COLOR_RESET);
}

/**
 * Afișează arborele în stil ASCII cu linii.
 */
void print_tree_ascii(AVLNode *root, char *prefix, bool is_left) {
    if (root == NULL) return;
    
    printf("%s", prefix);
    printf("%s", is_left ? "├── " : "└── ");
    
    int bf = balance_factor(root);
    const char *color = (bf == 0) ? COLOR_GREEN : 
                        (bf == 1 || bf == -1) ? COLOR_YELLOW : COLOR_RED;
    printf("%s%d%s\n", color, root->key, COLOR_RESET);
    
    char new_prefix[256];
    snprintf(new_prefix, sizeof(new_prefix), "%s%s", prefix, is_left ? "│   " : "    ");
    
    if (root->left || root->right) {
        print_tree_ascii(root->left, new_prefix, true);
        print_tree_ascii(root->right, new_prefix, false);
    }
}

/* =============================================================================
 * PARTEA 7: VALIDARE ȘI STATISTICI
 * =============================================================================
 */

/**
 * Verifică dacă arborele respectă proprietatea AVL.
 * 
 * @return true dacă arborele este valid AVL, false altfel
 */
bool is_valid_avl(AVLNode *root) {
    if (root == NULL) {
        return true;
    }
    
    int bf = balance_factor(root);
    
    if (bf < -1 || bf > 1) {
        printf(COLOR_RED "  ✗ Nod invalid: %d (bf=%d)\n" COLOR_RESET, root->key, bf);
        return false;
    }
    
    return is_valid_avl(root->left) && is_valid_avl(root->right);
}

/**
 * Numără nodurile din arbore.
 */
int count_nodes(AVLNode *root) {
    if (root == NULL) {
        return 0;
    }
    return 1 + count_nodes(root->left) + count_nodes(root->right);
}

/**
 * Calculează suma tuturor cheilor.
 */
int sum_keys(AVLNode *root) {
    if (root == NULL) {
        return 0;
    }
    return root->key + sum_keys(root->left) + sum_keys(root->right);
}

/**
 * Găsește valoarea minimă din arbore.
 */
int find_min(AVLNode *root) {
    if (root == NULL) {
        return -1;  /* sau INT_MAX */
    }
    while (root->left != NULL) {
        root = root->left;
    }
    return root->key;
}

/**
 * Găsește valoarea maximă din arbore.
 */
int find_max(AVLNode *root) {
    if (root == NULL) {
        return -1;  /* sau INT_MIN */
    }
    while (root->right != NULL) {
        root = root->right;
    }
    return root->key;
}

/**
 * Afișează statisticile arborelui.
 */
void print_statistics(AVLNode *root) {
    printf("\n" COLOR_BOLD "═══ Statistici Arbore ═══" COLOR_RESET "\n");
    printf("  Număr noduri:     %d\n", count_nodes(root));
    printf("  Înălțime:         %d\n", height(root));
    printf("  Valoare minimă:   %d\n", find_min(root));
    printf("  Valoare maximă:   %d\n", find_max(root));
    printf("  Suma cheilor:     %d\n", sum_keys(root));
    printf("  Valid AVL:        %s\n", is_valid_avl(root) ? 
           COLOR_GREEN "DA ✓" COLOR_RESET : COLOR_RED "NU ✗" COLOR_RESET);
    printf(COLOR_BOLD "═════════════════════════\n" COLOR_RESET);
}

/* =============================================================================
 * PARTEA 8: ELIBERARE MEMORIE
 * =============================================================================
 */

/**
 * Eliberează toată memoria alocată pentru arbore.
 * Folosește parcurgere postorder pentru a elibera copiii înainte de părinte.
 */
void free_tree(AVLNode *root) {
    if (root != NULL) {
        free_tree(root->left);
        free_tree(root->right);
        free(root);
    }
}

/* =============================================================================
 * FUNCȚII DEMONSTRATIVE
 * =============================================================================
 */

void demo_part1_basic_operations(void) {
    printf("\n");
    printf(COLOR_BOLD "╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 1: OPERAȚII DE BAZĂ - INSERARE                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n" COLOR_RESET);
    
    AVLNode *root = NULL;
    
    /* Inserăm elemente care vor provoca diferite tipuri de rotații */
    int keys[] = {30, 20, 40, 10, 25, 35, 50, 5, 15, 45};
    int n = sizeof(keys) / sizeof(keys[0]);
    
    printf("\n" COLOR_CYAN "Inserare secvențială: " COLOR_RESET);
    for (int i = 0; i < n; i++) {
        printf("%d ", keys[i]);
    }
    printf("\n\n");
    
    for (int i = 0; i < n; i++) {
        printf(COLOR_YELLOW "► Inserare %d:\n" COLOR_RESET, keys[i]);
        root = insert(root, keys[i]);
    }
    
    print_tree(root);
    print_statistics(root);
    
    /* Parcurgeri */
    printf(COLOR_CYAN "Parcurgere Inorder:   " COLOR_RESET);
    inorder(root);
    printf("\n");
    
    printf(COLOR_CYAN "Parcurgere Preorder:  " COLOR_RESET);
    preorder(root);
    printf("\n");
    
    printf(COLOR_CYAN "Parcurgere Postorder: " COLOR_RESET);
    postorder(root);
    printf("\n");
    
    free_tree(root);
}

void demo_part2_rotations(void) {
    printf("\n");
    printf(COLOR_BOLD "╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 2: DEMONSTRAȚIE ROTAȚII                           ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n" COLOR_RESET);
    
    /* Demonstrație Cazul RR */
    printf("\n" COLOR_MAGENTA "═══ Cazul RR (Inserare: 10, 20, 30) ═══\n" COLOR_RESET);
    AVLNode *root_rr = NULL;
    root_rr = insert(root_rr, 10);
    root_rr = insert(root_rr, 20);
    printf(COLOR_YELLOW "Înainte de inserare 30:\n" COLOR_RESET);
    print_tree(root_rr);
    root_rr = insert(root_rr, 30);
    printf(COLOR_GREEN "După inserare 30 (rotație RR aplicată):\n" COLOR_RESET);
    print_tree(root_rr);
    free_tree(root_rr);
    
    /* Demonstrație Cazul LL */
    printf("\n" COLOR_MAGENTA "═══ Cazul LL (Inserare: 30, 20, 10) ═══\n" COLOR_RESET);
    AVLNode *root_ll = NULL;
    root_ll = insert(root_ll, 30);
    root_ll = insert(root_ll, 20);
    printf(COLOR_YELLOW "Înainte de inserare 10:\n" COLOR_RESET);
    print_tree(root_ll);
    root_ll = insert(root_ll, 10);
    printf(COLOR_GREEN "După inserare 10 (rotație LL aplicată):\n" COLOR_RESET);
    print_tree(root_ll);
    free_tree(root_ll);
    
    /* Demonstrație Cazul LR */
    printf("\n" COLOR_MAGENTA "═══ Cazul LR (Inserare: 30, 10, 20) ═══\n" COLOR_RESET);
    AVLNode *root_lr = NULL;
    root_lr = insert(root_lr, 30);
    root_lr = insert(root_lr, 10);
    printf(COLOR_YELLOW "Înainte de inserare 20:\n" COLOR_RESET);
    print_tree(root_lr);
    root_lr = insert(root_lr, 20);
    printf(COLOR_GREEN "După inserare 20 (rotație LR aplicată):\n" COLOR_RESET);
    print_tree(root_lr);
    free_tree(root_lr);
    
    /* Demonstrație Cazul RL */
    printf("\n" COLOR_MAGENTA "═══ Cazul RL (Inserare: 10, 30, 20) ═══\n" COLOR_RESET);
    AVLNode *root_rl = NULL;
    root_rl = insert(root_rl, 10);
    root_rl = insert(root_rl, 30);
    printf(COLOR_YELLOW "Înainte de inserare 20:\n" COLOR_RESET);
    print_tree(root_rl);
    root_rl = insert(root_rl, 20);
    printf(COLOR_GREEN "După inserare 20 (rotație RL aplicată):\n" COLOR_RESET);
    print_tree(root_rl);
    free_tree(root_rl);
}

void demo_part3_deletion(void) {
    printf("\n");
    printf(COLOR_BOLD "╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 3: ȘTERGERE CU REECHILIBRARE                       ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n" COLOR_RESET);
    
    AVLNode *root = NULL;
    
    /* Construim un arbore */
    int keys[] = {50, 30, 70, 20, 40, 60, 80, 10, 25, 35, 45};
    int n = sizeof(keys) / sizeof(keys[0]);
    
    for (int i = 0; i < n; i++) {
        root = insert(root, keys[i]);
    }
    
    printf("\n" COLOR_CYAN "Arbore inițial:\n" COLOR_RESET);
    print_tree(root);
    
    /* Ștergem câteva noduri pentru a provoca reechilibrări */
    printf("\n" COLOR_YELLOW "► Ștergere nod frunză (10):\n" COLOR_RESET);
    root = delete_node(root, 10);
    print_tree(root);
    
    printf("\n" COLOR_YELLOW "► Ștergere nod cu un copil (20):\n" COLOR_RESET);
    root = delete_node(root, 20);
    print_tree(root);
    
    printf("\n" COLOR_YELLOW "► Ștergere nod cu doi copii (30):\n" COLOR_RESET);
    root = delete_node(root, 30);
    print_tree(root);
    
    printf("\n" COLOR_YELLOW "► Ștergere rădăcină (50):\n" COLOR_RESET);
    root = delete_node(root, 50);
    print_tree(root);
    
    print_statistics(root);
    
    free_tree(root);
}

void demo_part4_search(void) {
    printf("\n");
    printf(COLOR_BOLD "╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 4: CĂUTARE ÎN ARBORE                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n" COLOR_RESET);
    
    AVLNode *root = NULL;
    int keys[] = {50, 25, 75, 10, 35, 60, 90};
    int n = sizeof(keys) / sizeof(keys[0]);
    
    for (int i = 0; i < n; i++) {
        root = insert(root, keys[i]);
    }
    
    printf("\n" COLOR_CYAN "Arbore construit:\n" COLOR_RESET);
    print_tree(root);
    
    /* Căutări */
    int search_keys[] = {35, 60, 100, 10, 42};
    int m = sizeof(search_keys) / sizeof(search_keys[0]);
    
    printf("\n" COLOR_CYAN "Rezultate căutare:\n" COLOR_RESET);
    for (int i = 0; i < m; i++) {
        AVLNode *result = search(root, search_keys[i]);
        if (result != NULL) {
            printf("  Cheia %d: " COLOR_GREEN "GĂSITĂ ✓" COLOR_RESET "\n", search_keys[i]);
        } else {
            printf("  Cheia %d: " COLOR_RED "NEGĂSITĂ ✗" COLOR_RESET "\n", search_keys[i]);
        }
    }
    
    free_tree(root);
}

void demo_part5_comparison_with_bst(void) {
    printf("\n");
    printf(COLOR_BOLD "╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 5: COMPARAȚIE AVL vs BST NEECHILIBRAT             ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n" COLOR_RESET);
    
    printf("\n" COLOR_CYAN "Inserare secvențială 1-10 în AVL:\n" COLOR_RESET);
    
    AVLNode *avl_root = NULL;
    for (int i = 1; i <= 10; i++) {
        avl_root = insert(avl_root, i);
    }
    
    printf("\n" COLOR_GREEN "Rezultat AVL (echilibrat):\n" COLOR_RESET);
    print_tree(avl_root);
    printf("  Înălțime AVL: %d (optimă pentru 10 noduri: ~4)\n", height(avl_root));
    
    printf("\n" COLOR_YELLOW "BST neechilibrat ar avea înălțimea: 10 (degenerat în listă)\n" COLOR_RESET);
    
    printf("\n" COLOR_CYAN "Implicații pentru performanță:\n" COLOR_RESET);
    printf("  AVL - Căutare: O(log n) = O(4) pași\n");
    printf("  BST - Căutare: O(n) = O(10) pași\n");
    printf("  Îmbunătățire: ~%.1fx mai rapid!\n", 10.0 / 4.0);
    
    free_tree(avl_root);
}

void demo_part6_ascii_visualization(void) {
    printf("\n");
    printf(COLOR_BOLD "╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 6: VIZUALIZARE ASCII ALTERNATIVĂ                  ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n" COLOR_RESET);
    
    AVLNode *root = NULL;
    int keys[] = {50, 30, 70, 20, 40, 60, 80};
    int n = sizeof(keys) / sizeof(keys[0]);
    
    for (int i = 0; i < n; i++) {
        root = insert(root, keys[i]);
    }
    
    printf("\n" COLOR_CYAN "Arbore în format ASCII:\n" COLOR_RESET);
    printf("\n");
    print_tree_ascii(root, "", false);
    
    printf("\n" COLOR_CYAN "Legendă culori:\n" COLOR_RESET);
    printf("  " COLOR_GREEN "Verde" COLOR_RESET "   - bf = 0 (perfect echilibrat)\n");
    printf("  " COLOR_YELLOW "Galben" COLOR_RESET "  - bf = ±1 (ușor dezechilibrat, valid)\n");
    printf("  " COLOR_RED "Roșu" COLOR_RESET "    - |bf| > 1 (invalid, necesită rotație)\n");
    
    free_tree(root);
}

/* =============================================================================
 * FUNCȚIA MAIN
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf(COLOR_BOLD COLOR_CYAN);
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     SĂPTĂMÂNA 09: ARBORI AVL - EXEMPLU COMPLET                ║\n");
    printf("║     Algoritmi și Tehnici de Programare                        ║\n");
    printf("║     ASE București - CSIE                                      ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf(COLOR_RESET);
    
    demo_part1_basic_operations();
    demo_part2_rotations();
    demo_part3_deletion();
    demo_part4_search();
    demo_part5_comparison_with_bst();
    demo_part6_ascii_visualization();
    
    printf("\n");
    printf(COLOR_BOLD COLOR_GREEN);
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     DEMONSTRAȚIE COMPLETĂ - SUCCES!                           ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf(COLOR_RESET);
    printf("\n");
    
    return 0;
}
