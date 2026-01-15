/**
 * =============================================================================
 * EXERCIȚIUL 1: Operații Fundamentale pe Binary Trees
 * =============================================================================
 *
 * OBIECTIV:
 *   Implementați operațiile de bază pe arbori binari folosind recursivitate.
 *
 * CERINȚE:
 *   1. Completați structura TreeNode
 *   2. Implementați funcția createNode pentru crearea nodurilor
 *   3. Implementați cele 4 tipuri de traversare
 *   4. Implementați calcularea înălțimii și numărarea nodurilor
 *   5. Implementați căutarea unei valori
 *   6. Implementați eliberarea corectă a memoriei
 *
 * EXEMPLU INPUT (din stdin):
 *   7
 *   50 30 70 20 40 60 80
 *   3
 *   40 100 50
 *
 * OUTPUT AȘTEPTAT:
 *   Inorder: 20 30 40 50 60 70 80
 *   Preorder: 50 30 20 40 70 60 80
 *   Postorder: 20 40 30 60 80 70 50
 *   Height: 2
 *   Nodes: 7
 *   Leaves: 4
 *   Search 40: FOUND
 *   Search 100: NOT FOUND
 *   Search 50: FOUND
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -o exercise1 exercise1.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

/* =============================================================================
 * DEFINIREA STRUCTURILOR
 * =============================================================================
 */

/**
 * TODO 1: Completați structura TreeNode
 *
 * Structura trebuie să conțină:
 *   - un câmp 'data' de tip int pentru valoarea nodului
 *   - un pointer 'left' către copilul stâng (de tip struct TreeNode*)
 *   - un pointer 'right' către copilul drept (de tip struct TreeNode*)
 *
 * Hint: Folosiți typedef pentru a putea scrie TreeNode în loc de struct TreeNode
 */

typedef struct TreeNode {
    /* YOUR CODE HERE */
    
} TreeNode;


/* =============================================================================
 * FUNCȚII DE CREARE
 * =============================================================================
 */

/**
 * TODO 2: Implementați funcția createNode
 *
 * Această funcție trebuie să:
 *   1. Aloce memorie pentru un nou TreeNode folosind malloc
 *   2. Verifice dacă alocarea a reușit (malloc poate returna NULL)
 *   3. Inițializeze câmpul data cu valoarea primită ca parametru
 *   4. Inițializeze pointerii left și right cu NULL
 *   5. Returneze pointer-ul către nodul nou creat
 *
 * @param value Valoarea de stocat în nod
 * @return Pointer către nodul creat sau NULL la eroare
 *
 * Hint: TreeNode *node = (TreeNode*)malloc(sizeof(TreeNode));
 */
TreeNode* createNode(int value) {
    /* YOUR CODE HERE */
    
    return NULL;  /* Înlocuiți această linie */
}

/**
 * TODO 3: Implementați funcția buildTreeFromArray
 *
 * Construiește un arbore binar complet dintr-un array.
 * Pentru nodul la indexul i:
 *   - Copilul stâng este la indexul 2*i + 1
 *   - Copilul drept este la indexul 2*i + 2
 *
 * Valoarea -1 în array reprezintă un nod NULL.
 *
 * @param arr Array-ul cu valorile
 * @param index Indexul curent
 * @param size Dimensiunea array-ului
 * @return Pointer către rădăcina subarborelui
 *
 * Pași:
 *   1. Verificați dacă index >= size sau arr[index] == -1 → returnați NULL
 *   2. Creați un nod nou cu valoarea arr[index]
 *   3. Setați node->left = buildTreeFromArray(arr, 2*index + 1, size)
 *   4. Setați node->right = buildTreeFromArray(arr, 2*index + 2, size)
 *   5. Returnați nodul
 */
TreeNode* buildTreeFromArray(int arr[], int index, int size) {
    /* YOUR CODE HERE */
    
    return NULL;  /* Înlocuiți această linie */
}


/* =============================================================================
 * FUNCȚII DE TRAVERSARE
 * =============================================================================
 */

/**
 * TODO 4: Implementați traversarea INORDER (Stânga → Nod → Dreapta)
 *
 * Ordinea operațiilor:
 *   1. Vizitați recursiv subarborele stâng
 *   2. Afișați valoarea nodului curent (printf("%d ", node->data))
 *   3. Vizitați recursiv subarborele drept
 *
 * @param node Rădăcina (sub)arborelui
 *
 * Hint: Nu uitați să verificați dacă node != NULL înainte de orice operație!
 */
void inorderTraversal(TreeNode *node) {
    /* YOUR CODE HERE */
    
}

/**
 * TODO 5: Implementați traversarea PREORDER (Nod → Stânga → Dreapta)
 *
 * Ordinea operațiilor:
 *   1. Afișați valoarea nodului curent ÎNTÂI
 *   2. Vizitați recursiv subarborele stâng
 *   3. Vizitați recursiv subarborele drept
 *
 * @param node Rădăcina (sub)arborelui
 */
void preorderTraversal(TreeNode *node) {
    /* YOUR CODE HERE */
    
}

/**
 * TODO 6: Implementați traversarea POSTORDER (Stânga → Dreapta → Nod)
 *
 * Ordinea operațiilor:
 *   1. Vizitați recursiv subarborele stâng
 *   2. Vizitați recursiv subarborele drept
 *   3. Afișați valoarea nodului curent ULTIMUL
 *
 * IMPORTANT: Această traversare este esențială pentru eliberarea memoriei!
 *
 * @param node Rădăcina (sub)arborelui
 */
void postorderTraversal(TreeNode *node) {
    /* YOUR CODE HERE */
    
}


/* =============================================================================
 * FUNCȚII DE ANALIZĂ
 * =============================================================================
 */

/**
 * TODO 7: Implementați calcularea înălțimii arborelui
 *
 * Înălțimea = numărul de muchii pe cel mai lung drum de la rădăcină la frunză.
 * Convenție: height(NULL) = -1
 *
 * Formula: height(node) = 1 + max(height(left), height(right))
 *
 * @param node Rădăcina (sub)arborelui
 * @return Înălțimea arborelui
 *
 * Pași:
 *   1. Dacă node == NULL, returnați -1
 *   2. Calculați recursiv înălțimea subarborelui stâng
 *   3. Calculați recursiv înălțimea subarborelui drept
 *   4. Returnați 1 + maximul dintre cele două înălțimi
 *
 * Hint pentru max: (a > b) ? a : b
 */
int treeHeight(TreeNode *node) {
    /* YOUR CODE HERE */
    
    return 0;  /* Înlocuiți această linie */
}

/**
 * TODO 8: Implementați numărarea nodurilor
 *
 * Formula: count(node) = 1 + count(left) + count(right)
 *
 * @param node Rădăcina (sub)arborelui
 * @return Numărul total de noduri
 *
 * Pași:
 *   1. Dacă node == NULL, returnați 0
 *   2. Returnați 1 + countNodes(node->left) + countNodes(node->right)
 */
int countNodes(TreeNode *node) {
    /* YOUR CODE HERE */
    
    return 0;  /* Înlocuiți această linie */
}

/**
 * TODO 9: Implementați numărarea frunzelor
 *
 * O frunză este un nod fără copii (left == NULL și right == NULL).
 *
 * @param node Rădăcina (sub)arborelui
 * @return Numărul de frunze
 *
 * Pași:
 *   1. Dacă node == NULL, returnați 0
 *   2. Dacă node este frunză (ambii copii NULL), returnați 1
 *   3. Altfel, returnați countLeaves(left) + countLeaves(right)
 */
int countLeaves(TreeNode *node) {
    /* YOUR CODE HERE */
    
    return 0;  /* Înlocuiți această linie */
}


/* =============================================================================
 * FUNCȚII DE CĂUTARE
 * =============================================================================
 */

/**
 * TODO 10: Implementați căutarea unei valori în arbore
 *
 * Căutare generică (nu BST) - trebuie să verificăm toate nodurile.
 *
 * @param node Rădăcina arborelui
 * @param value Valoarea căutată
 * @return Pointer către nodul găsit sau NULL
 *
 * Pași:
 *   1. Dacă node == NULL, returnați NULL
 *   2. Dacă node->data == value, returnați node
 *   3. Căutați în subarborele stâng; dacă găsiți, returnați
 *   4. Căutați în subarborele drept și returnați rezultatul
 */
TreeNode* searchTree(TreeNode *node, int value) {
    /* YOUR CODE HERE */
    
    return NULL;  /* Înlocuiți această linie */
}


/* =============================================================================
 * ELIBERAREA MEMORIEI
 * =============================================================================
 */

/**
 * TODO 11: Implementați eliberarea memoriei arborelui
 *
 * FOARTE IMPORTANT: Folosiți traversarea POSTORDER!
 * Copiii trebuie eliberați ÎNAINTE de părinte, altfel pierdeți referințele.
 *
 * @param node Rădăcina arborelui de eliberat
 *
 * Pași:
 *   1. Dacă node == NULL, returnați (nu faceți nimic)
 *   2. Eliberați recursiv subarborele stâng
 *   3. Eliberați recursiv subarborele drept
 *   4. Eliberați nodul curent cu free(node)
 */
void freeTree(TreeNode *node) {
    /* YOUR CODE HERE */
    
}


/* =============================================================================
 * FUNCȚIA MAIN - NU MODIFICAȚI
 * =============================================================================
 */

int main(void) {
    int n;
    
    /* Citim numărul de noduri */
    if (scanf("%d", &n) != 1 || n <= 0) {
        fprintf(stderr, "Eroare: Număr invalid de noduri\n");
        return 1;
    }
    
    /* Citim valorile nodurilor */
    int *arr = (int*)malloc(n * sizeof(int));
    if (arr == NULL) {
        fprintf(stderr, "Eroare: Alocare memorie eșuată\n");
        return 1;
    }
    
    for (int i = 0; i < n; i++) {
        if (scanf("%d", &arr[i]) != 1) {
            fprintf(stderr, "Eroare: Nu s-au putut citi valorile\n");
            free(arr);
            return 1;
        }
    }
    
    /* Construim arborele */
    TreeNode *root = buildTreeFromArray(arr, 0, n);
    free(arr);
    
    if (root == NULL) {
        fprintf(stderr, "Eroare: Nu s-a putut construi arborele\n");
        return 1;
    }
    
    /* Afișăm traversările */
    printf("Inorder: ");
    inorderTraversal(root);
    printf("\n");
    
    printf("Preorder: ");
    preorderTraversal(root);
    printf("\n");
    
    printf("Postorder: ");
    postorderTraversal(root);
    printf("\n");
    
    /* Afișăm statisticile */
    printf("Height: %d\n", treeHeight(root));
    printf("Nodes: %d\n", countNodes(root));
    printf("Leaves: %d\n", countLeaves(root));
    
    /* Citim și procesăm căutările */
    int numSearches;
    if (scanf("%d", &numSearches) == 1) {
        for (int i = 0; i < numSearches; i++) {
            int value;
            if (scanf("%d", &value) == 1) {
                TreeNode *found = searchTree(root, value);
                printf("Search %d: %s\n", value, found ? "FOUND" : "NOT FOUND");
            }
        }
    }
    
    /* Eliberăm memoria */
    freeTree(root);
    
    return 0;
}


/* =============================================================================
 * PROVOCĂRI BONUS (OPȚIONAL)
 * =============================================================================
 *
 * Dacă ați terminat exercițiul principal, încercați:
 *
 * BONUS 1: Implementați funcția levelOrderTraversal (necesită o coadă)
 *
 * BONUS 2: Implementați funcția isBalanced care verifică dacă arborele
 *          este echilibrat (diferența înălțimilor subarborilor ≤ 1)
 *
 * BONUS 3: Implementați funcția mirrorTree care creează o copie
 *          în oglindă a arborelui (stânga și dreapta inversate)
 *
 * BONUS 4: Implementați funcția findLCA (Lowest Common Ancestor)
 *          care găsește cel mai apropiat strămoș comun a două noduri
 *
 * =============================================================================
 */
