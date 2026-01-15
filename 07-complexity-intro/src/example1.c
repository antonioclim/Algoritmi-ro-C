/**
 * =============================================================================
 * SĂPTĂMÂNA 7: ARBORI BINARI (BINARY TREES)
 * Exemplu Complet Demonstrativ
 * =============================================================================
 *
 * Acest exemplu demonstrează:
 *   1. Definirea structurii TreeNode
 *   2. Crearea și construcția arborilor binari
 *   3. Cele 4 tipuri de traversare (inorder, preorder, postorder, level-order)
 *   4. Calcularea înălțimii și numărarea nodurilor
 *   5. Căutarea valorilor în arbore
 *   6. Eliberarea corectă a memoriei
 *   7. Afișarea vizuală a structurii arborelui
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -g -o example1 example1.c
 * Rulare: ./example1
 * Verificare memorie: valgrind --leak-check=full ./example1
 *
 * =============================================================================
 * Curs: Algoritmi și Tehnici de Programare (ATP)
 * Academia de Studii Economice - CSIE, București
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* =============================================================================
 * PARTEA 1: DEFINIREA STRUCTURILOR DE DATE
 * =============================================================================
 */

/**
 * Structura unui nod în binary tree
 * 
 * Fiecare nod conține:
 * - data: valoarea stocată (în acest exemplu, un întreg)
 * - left: pointer către copilul stâng (sau NULL)
 * - right: pointer către copilul drept (sau NULL)
 */
typedef struct TreeNode {
    int data;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

/**
 * Structura pentru queue (necesară pentru level-order traversal)
 * Implementare simplă cu array circular
 */
#define MAX_QUEUE_SIZE 100

typedef struct Queue {
    TreeNode *items[MAX_QUEUE_SIZE];
    int front;
    int rear;
    int count;
} Queue;

/* =============================================================================
 * PARTEA 2: FUNCȚII UTILITARE PENTRU QUEUE
 * =============================================================================
 */

/**
 * Inițializează o coadă goală
 */
void initQueue(Queue *q) {
    q->front = 0;
    q->rear = -1;
    q->count = 0;
}

/**
 * Verifică dacă coada este goală
 */
int isQueueEmpty(Queue *q) {
    return q->count == 0;
}

/**
 * Adaugă un element în coadă (enqueue)
 */
void enqueue(Queue *q, TreeNode *node) {
    if (q->count >= MAX_QUEUE_SIZE) {
        fprintf(stderr, "Eroare: Coada este plină!\n");
        return;
    }
    q->rear = (q->rear + 1) % MAX_QUEUE_SIZE;
    q->items[q->rear] = node;
    q->count++;
}

/**
 * Scoate un element din coadă (dequeue)
 */
TreeNode* dequeue(Queue *q) {
    if (isQueueEmpty(q)) {
        return NULL;
    }
    TreeNode *node = q->items[q->front];
    q->front = (q->front + 1) % MAX_QUEUE_SIZE;
    q->count--;
    return node;
}

/* =============================================================================
 * PARTEA 3: FUNCȚII DE CREARE ȘI GESTIONARE NODURI
 * =============================================================================
 */

/**
 * Creează un nod nou cu valoarea specificată
 * 
 * @param value Valoarea de stocat în nod
 * @return Pointer către nodul nou creat sau NULL la eroare de alocare
 */
TreeNode* createNode(int value) {
    TreeNode *newNode = (TreeNode*)malloc(sizeof(TreeNode));
    
    if (newNode == NULL) {
        fprintf(stderr, "Eroare: Nu s-a putut aloca memorie pentru nod!\n");
        return NULL;
    }
    
    newNode->data = value;
    newNode->left = NULL;
    newNode->right = NULL;
    
    return newNode;
}

/**
 * Construiește un arbore binar din array (reprezentare completă)
 * 
 * Array-ul reprezintă arborele nivel cu nivel:
 * - Pentru nodul la index i:
 *   - Copilul stâng este la index 2*i + 1
 *   - Copilul drept este la index 2*i + 2
 * 
 * @param arr Array-ul cu valorile nodurilor (-1 reprezintă NULL)
 * @param index Indexul curent în array
 * @param size Dimensiunea array-ului
 * @return Pointer către rădăcina subarborelui creat
 */
TreeNode* buildTreeFromArray(int arr[], int index, int size) {
    if (index >= size || arr[index] == -1) {
        return NULL;
    }
    
    TreeNode *node = createNode(arr[index]);
    if (node == NULL) {
        return NULL;
    }
    
    node->left = buildTreeFromArray(arr, 2 * index + 1, size);
    node->right = buildTreeFromArray(arr, 2 * index + 2, size);
    
    return node;
}

/* =============================================================================
 * PARTEA 4: FUNCȚII DE TRAVERSARE
 * =============================================================================
 */

/**
 * Traversare INORDER (Stânga → Nod → Dreapta)
 * 
 * Pentru un BST, această traversare produce elementele în ordine sortată.
 * 
 * @param node Rădăcina (sub)arborelui de traversat
 */
void inorderTraversal(TreeNode *node) {
    if (node != NULL) {
        inorderTraversal(node->left);    /* 1. Vizităm subarborele stâng */
        printf("%d ", node->data);        /* 2. Procesăm nodul curent */
        inorderTraversal(node->right);   /* 3. Vizităm subarborele drept */
    }
}

/**
 * Traversare PREORDER (Nod → Stânga → Dreapta)
 * 
 * Utilă pentru: copierea structurii, serializare, generare expresii prefix.
 * 
 * @param node Rădăcina (sub)arborelui de traversat
 */
void preorderTraversal(TreeNode *node) {
    if (node != NULL) {
        printf("%d ", node->data);        /* 1. Procesăm nodul curent ÎNTÂI */
        preorderTraversal(node->left);   /* 2. Vizităm subarborele stâng */
        preorderTraversal(node->right);  /* 3. Vizităm subarborele drept */
    }
}

/**
 * Traversare POSTORDER (Stânga → Dreapta → Nod)
 * 
 * ESENȚIALĂ pentru eliberarea memoriei - copiii trebuie eliberați
 * înainte de părinte!
 * 
 * @param node Rădăcina (sub)arborelui de traversat
 */
void postorderTraversal(TreeNode *node) {
    if (node != NULL) {
        postorderTraversal(node->left);  /* 1. Vizităm subarborele stâng */
        postorderTraversal(node->right); /* 2. Vizităm subarborele drept */
        printf("%d ", node->data);        /* 3. Procesăm nodul curent ULTIMUL */
    }
}

/**
 * Traversare LEVEL-ORDER (Breadth-First Search)
 * 
 * Vizitează nodurile nivel cu nivel, de la stânga la dreapta.
 * Necesită o structură auxiliară de tip queue.
 * 
 * @param root Rădăcina arborelui
 */
void levelOrderTraversal(TreeNode *root) {
    if (root == NULL) {
        return;
    }
    
    Queue q;
    initQueue(&q);
    enqueue(&q, root);
    
    while (!isQueueEmpty(&q)) {
        TreeNode *current = dequeue(&q);
        printf("%d ", current->data);
        
        if (current->left != NULL) {
            enqueue(&q, current->left);
        }
        if (current->right != NULL) {
            enqueue(&q, current->right);
        }
    }
}

/**
 * Traversare level-order cu afișarea nivelurilor separate
 * 
 * @param root Rădăcina arborelui
 */
void levelOrderWithLevels(TreeNode *root) {
    if (root == NULL) {
        printf("(arbore vid)\n");
        return;
    }
    
    Queue q;
    initQueue(&q);
    enqueue(&q, root);
    
    int level = 0;
    
    while (!isQueueEmpty(&q)) {
        int levelSize = q.count;
        printf("  Nivel %d: ", level);
        
        for (int i = 0; i < levelSize; i++) {
            TreeNode *current = dequeue(&q);
            printf("%d ", current->data);
            
            if (current->left != NULL) {
                enqueue(&q, current->left);
            }
            if (current->right != NULL) {
                enqueue(&q, current->right);
            }
        }
        printf("\n");
        level++;
    }
}

/* =============================================================================
 * PARTEA 5: FUNCȚII DE ANALIZĂ A ARBORELUI
 * =============================================================================
 */

/**
 * Calculează înălțimea arborelui
 * 
 * Înălțimea = numărul de muchii pe cel mai lung drum de la rădăcină la frunză.
 * Convenție: height(NULL) = -1, height(frunză) = 0
 * 
 * @param node Rădăcina (sub)arborelui
 * @return Înălțimea arborelui
 */
int treeHeight(TreeNode *node) {
    if (node == NULL) {
        return -1;  /* Arbore vid are înălțimea -1 */
    }
    
    int leftHeight = treeHeight(node->left);
    int rightHeight = treeHeight(node->right);
    
    /* Returnăm maximul dintre înălțimile subarborilor + 1 */
    return 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
}

/**
 * Numără toate nodurile din arbore
 * 
 * @param node Rădăcina (sub)arborelui
 * @return Numărul total de noduri
 */
int countNodes(TreeNode *node) {
    if (node == NULL) {
        return 0;
    }
    return 1 + countNodes(node->left) + countNodes(node->right);
}

/**
 * Numără doar frunzele (noduri fără copii)
 * 
 * @param node Rădăcina (sub)arborelui
 * @return Numărul de frunze
 */
int countLeaves(TreeNode *node) {
    if (node == NULL) {
        return 0;
    }
    
    /* Verificăm dacă nodul curent este frunză */
    if (node->left == NULL && node->right == NULL) {
        return 1;
    }
    
    return countLeaves(node->left) + countLeaves(node->right);
}

/**
 * Numără nodurile interne (noduri cu cel puțin un copil)
 * 
 * @param node Rădăcina (sub)arborelui
 * @return Numărul de noduri interne
 */
int countInternalNodes(TreeNode *node) {
    if (node == NULL) {
        return 0;
    }
    
    /* Frunzele nu sunt noduri interne */
    if (node->left == NULL && node->right == NULL) {
        return 0;
    }
    
    return 1 + countInternalNodes(node->left) + countInternalNodes(node->right);
}

/**
 * Calculează suma tuturor valorilor din arbore
 * 
 * @param node Rădăcina (sub)arborelui
 * @return Suma valorilor
 */
int sumTree(TreeNode *node) {
    if (node == NULL) {
        return 0;
    }
    return node->data + sumTree(node->left) + sumTree(node->right);
}

/**
 * Găsește valoarea maximă din arbore
 * 
 * @param node Rădăcina (sub)arborelui
 * @return Valoarea maximă sau INT_MIN pentru arbore vid
 */
int findMax(TreeNode *node) {
    if (node == NULL) {
        return -999999;  /* Valoare „sentinel" pentru arbore vid */
    }
    
    int leftMax = findMax(node->left);
    int rightMax = findMax(node->right);
    
    int max = node->data;
    if (leftMax > max) max = leftMax;
    if (rightMax > max) max = rightMax;
    
    return max;
}

/**
 * Găsește valoarea minimă din arbore
 * 
 * @param node Rădăcina (sub)arborelui
 * @return Valoarea minimă sau INT_MAX pentru arbore vid
 */
int findMin(TreeNode *node) {
    if (node == NULL) {
        return 999999;  /* Valoare „sentinel" pentru arbore vid */
    }
    
    int leftMin = findMin(node->left);
    int rightMin = findMin(node->right);
    
    int min = node->data;
    if (leftMin < min) min = leftMin;
    if (rightMin < min) min = rightMin;
    
    return min;
}

/* =============================================================================
 * PARTEA 6: FUNCȚII DE CĂUTARE
 * =============================================================================
 */

/**
 * Caută o valoare în arbore (căutare generică, nu BST)
 * 
 * @param node Rădăcina arborelui
 * @param value Valoarea căutată
 * @return Pointer către nodul găsit sau NULL dacă nu există
 */
TreeNode* searchTree(TreeNode *node, int value) {
    if (node == NULL) {
        return NULL;
    }
    
    if (node->data == value) {
        return node;
    }
    
    /* Căutăm în subarborele stâng */
    TreeNode *found = searchTree(node->left, value);
    if (found != NULL) {
        return found;
    }
    
    /* Dacă nu am găsit în stânga, căutăm în dreapta */
    return searchTree(node->right, value);
}

/**
 * Verifică dacă o valoare există în arbore
 * 
 * @param node Rădăcina arborelui
 * @param value Valoarea căutată
 * @return 1 dacă valoarea există, 0 altfel
 */
int containsValue(TreeNode *node, int value) {
    return searchTree(node, value) != NULL;
}

/**
 * Calculează adâncimea unui nod cu valoare specificată
 * 
 * @param node Rădăcina arborelui
 * @param value Valoarea căutată
 * @param depth Adâncimea curentă
 * @return Adâncimea nodului sau -1 dacă nu există
 */
int findDepth(TreeNode *node, int value, int depth) {
    if (node == NULL) {
        return -1;
    }
    
    if (node->data == value) {
        return depth;
    }
    
    int leftDepth = findDepth(node->left, value, depth + 1);
    if (leftDepth != -1) {
        return leftDepth;
    }
    
    return findDepth(node->right, value, depth + 1);
}

/* =============================================================================
 * PARTEA 7: FUNCȚII DE AFIȘARE VIZUALĂ
 * =============================================================================
 */

/**
 * Afișează arborele ca structură orizontală (rotită 90° în sensul acelor de ceasornic)
 * 
 * @param node Rădăcina (sub)arborelui
 * @param space Spațiul de indentare curent
 */
void printTreeHorizontal(TreeNode *node, int space) {
    if (node == NULL) {
        return;
    }
    
    /* Creștem distanța între niveluri */
    space += 6;
    
    /* Procesăm mai întâi subarborele drept */
    printTreeHorizontal(node->right, space);
    
    /* Afișăm nodul curent cu indentare */
    printf("\n");
    for (int i = 6; i < space; i++) {
        printf(" ");
    }
    printf("%d\n", node->data);
    
    /* Procesăm subarborele stâng */
    printTreeHorizontal(node->left, space);
}

/**
 * Funcție wrapper pentru afișarea vizuală a arborelui
 */
void printTree(TreeNode *root) {
    printf("\n  Structura arborelui (rotită 90° spre dreapta):\n");
    printf("  ────────────────────────────────────────────\n");
    printTreeHorizontal(root, 0);
    printf("  ────────────────────────────────────────────\n\n");
}

/* =============================================================================
 * PARTEA 8: FUNCȚII DE VERIFICARE ȘI COMPARARE
 * =============================================================================
 */

/**
 * Verifică dacă arborele este echilibrat (diferența înălțimilor ≤ 1)
 * 
 * @param node Rădăcina (sub)arborelui
 * @return 1 dacă este echilibrat, 0 altfel
 */
int isBalanced(TreeNode *node) {
    if (node == NULL) {
        return 1;  /* Arbore vid este echilibrat */
    }
    
    int leftHeight = treeHeight(node->left);
    int rightHeight = treeHeight(node->right);
    
    int diff = leftHeight - rightHeight;
    if (diff < 0) diff = -diff;  /* Valoare absolută */
    
    if (diff > 1) {
        return 0;
    }
    
    /* Verificăm recursiv și subarborii */
    return isBalanced(node->left) && isBalanced(node->right);
}

/**
 * Verifică dacă doi arbori sunt identici
 * 
 * @param a Rădăcina primului arbore
 * @param b Rădăcina celui de-al doilea arbore
 * @return 1 dacă sunt identici, 0 altfel
 */
int areIdentical(TreeNode *a, TreeNode *b) {
    /* Ambii sunt NULL - identici */
    if (a == NULL && b == NULL) {
        return 1;
    }
    
    /* Doar unul este NULL - diferiți */
    if (a == NULL || b == NULL) {
        return 0;
    }
    
    /* Verificăm valoarea și recursiv subarborii */
    return (a->data == b->data) &&
           areIdentical(a->left, b->left) &&
           areIdentical(a->right, b->right);
}

/**
 * Creează o copie a arborelui (folosind preorder)
 * 
 * @param node Rădăcina arborelui de copiat
 * @return Pointer către rădăcina copiei
 */
TreeNode* copyTree(TreeNode *node) {
    if (node == NULL) {
        return NULL;
    }
    
    TreeNode *newNode = createNode(node->data);
    if (newNode == NULL) {
        return NULL;
    }
    
    newNode->left = copyTree(node->left);
    newNode->right = copyTree(node->right);
    
    return newNode;
}

/**
 * Creează imaginea în oglindă a arborelui
 * 
 * @param node Rădăcina arborelui
 * @return Pointer către rădăcina arborelui oglindit
 */
TreeNode* mirrorTree(TreeNode *node) {
    if (node == NULL) {
        return NULL;
    }
    
    TreeNode *newNode = createNode(node->data);
    if (newNode == NULL) {
        return NULL;
    }
    
    /* Inversăm stânga și dreapta */
    newNode->left = mirrorTree(node->right);
    newNode->right = mirrorTree(node->left);
    
    return newNode;
}

/* =============================================================================
 * PARTEA 9: ELIBERAREA MEMORIEI
 * =============================================================================
 */

/**
 * Eliberează toată memoria ocupată de arbore
 * 
 * IMPORTANT: Folosește traversarea POSTORDER!
 * Copiii trebuie eliberați ÎNAINTE de părinte.
 * 
 * @param node Rădăcina arborelui de eliberat
 */
void freeTree(TreeNode *node) {
    if (node == NULL) {
        return;
    }
    
    /* POSTORDER: mai întâi copiii */
    freeTree(node->left);
    freeTree(node->right);
    
    /* Apoi nodul curent */
    free(node);
}

/* =============================================================================
 * PARTEA 10: FUNCȚII DEMO
 * =============================================================================
 */

void demo_part1_creation(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 1: CREAREA ARBORELUI                              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("  Construim un arbore binar manual:\n\n");
    printf("           50\n");
    printf("          /  \\\n");
    printf("        30    70\n");
    printf("       /  \\   /  \\\n");
    printf("      20  40 60  80\n");
    printf("     /\n");
    printf("    10\n\n");
    
    /* Construcție manuală */
    TreeNode *root = createNode(50);
    root->left = createNode(30);
    root->right = createNode(70);
    root->left->left = createNode(20);
    root->left->right = createNode(40);
    root->right->left = createNode(60);
    root->right->right = createNode(80);
    root->left->left->left = createNode(10);
    
    printf("  Arbore creat cu succes!\n");
    printTree(root);
    
    freeTree(root);
}

void demo_part2_traversals(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 2: TRAVERSĂRI                                     ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Construim arborele */
    int arr[] = {1, 2, 3, 4, 5, 6, 7};
    int size = sizeof(arr) / sizeof(arr[0]);
    TreeNode *root = buildTreeFromArray(arr, 0, size);
    
    printf("  Arborele folosit:\n\n");
    printf("           1\n");
    printf("          / \\\n");
    printf("         2   3\n");
    printf("        / \\ / \\\n");
    printf("       4  5 6  7\n\n");
    
    printf("  ┌─────────────────────────────────────────────────────────────┐\n");
    printf("  │ Inorder   (L→N→R): ");
    inorderTraversal(root);
    printf("\n");
    printf("  │ Preorder  (N→L→R): ");
    preorderTraversal(root);
    printf("\n");
    printf("  │ Postorder (L→R→N): ");
    postorderTraversal(root);
    printf("\n");
    printf("  │ Level-order (BFS): ");
    levelOrderTraversal(root);
    printf("\n");
    printf("  └─────────────────────────────────────────────────────────────┘\n\n");
    
    printf("  Traversare level-order cu niveluri separate:\n");
    levelOrderWithLevels(root);
    
    freeTree(root);
}

void demo_part3_analysis(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 3: ANALIZA ARBORELUI                              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Construim un arbore mai complex */
    TreeNode *root = createNode(50);
    root->left = createNode(30);
    root->right = createNode(70);
    root->left->left = createNode(20);
    root->left->right = createNode(40);
    root->right->left = createNode(60);
    root->right->right = createNode(80);
    root->left->left->left = createNode(10);
    root->right->right->right = createNode(90);
    
    printTree(root);
    
    printf("  ┌─────────────────────────────────────────────────────────────┐\n");
    printf("  │ Statistici ale arborelui:                                   │\n");
    printf("  ├─────────────────────────────────────────────────────────────┤\n");
    printf("  │ Înălțime:            %d                                      │\n", treeHeight(root));
    printf("  │ Număr total noduri:  %d                                      │\n", countNodes(root));
    printf("  │ Număr frunze:        %d                                      │\n", countLeaves(root));
    printf("  │ Noduri interne:      %d                                      │\n", countInternalNodes(root));
    printf("  │ Suma valorilor:      %d                                    │\n", sumTree(root));
    printf("  │ Valoare maximă:      %d                                     │\n", findMax(root));
    printf("  │ Valoare minimă:      %d                                     │\n", findMin(root));
    printf("  │ Este echilibrat:     %s                                    │\n", isBalanced(root) ? "DA" : "NU");
    printf("  └─────────────────────────────────────────────────────────────┘\n\n");
    
    freeTree(root);
}

void demo_part4_search(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 4: CĂUTARE ÎN ARBORE                              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    int arr[] = {50, 30, 70, 20, 40, 60, 80};
    int size = sizeof(arr) / sizeof(arr[0]);
    TreeNode *root = buildTreeFromArray(arr, 0, size);
    
    printTree(root);
    
    int searchValues[] = {40, 60, 100, 50};
    int numSearches = sizeof(searchValues) / sizeof(searchValues[0]);
    
    printf("  Rezultate căutare:\n\n");
    
    for (int i = 0; i < numSearches; i++) {
        int val = searchValues[i];
        TreeNode *found = searchTree(root, val);
        int depth = findDepth(root, val, 0);
        
        if (found != NULL) {
            printf("  ✓ Valoarea %d GĂSITĂ la adâncimea %d\n", val, depth);
        } else {
            printf("  ✗ Valoarea %d NU a fost găsită\n", val);
        }
    }
    printf("\n");
    
    freeTree(root);
}

void demo_part5_operations(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 5: OPERAȚII PE ARBORI                             ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Arbore original */
    TreeNode *original = createNode(1);
    original->left = createNode(2);
    original->right = createNode(3);
    original->left->left = createNode(4);
    original->left->right = createNode(5);
    
    printf("  Arbore original:\n");
    printf("  Inorder: ");
    inorderTraversal(original);
    printf("\n\n");
    
    /* Copie */
    TreeNode *copy = copyTree(original);
    printf("  Copie a arborelui:\n");
    printf("  Inorder: ");
    inorderTraversal(copy);
    printf("\n");
    printf("  Sunt identice? %s\n\n", areIdentical(original, copy) ? "DA" : "NU");
    
    /* Oglindă */
    TreeNode *mirror = mirrorTree(original);
    printf("  Arbore oglindit:\n");
    printf("  Inorder: ");
    inorderTraversal(mirror);
    printf("\n");
    printf("  Original vs Oglindă identice? %s\n\n", areIdentical(original, mirror) ? "DA" : "NU");
    
    /* Eliberăm toate arborii */
    freeTree(original);
    freeTree(copy);
    freeTree(mirror);
}

void demo_part6_memory(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║      PARTEA 6: GESTIONAREA MEMORIEI                           ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("  Creăm un arbore cu 7 noduri...\n");
    
    int arr[] = {10, 20, 30, 40, 50, 60, 70};
    TreeNode *root = buildTreeFromArray(arr, 0, 7);
    
    printf("  Arbore creat: %d noduri\n\n", countNodes(root));
    
    printf("  Eliberăm memoria folosind postorder traversal...\n");
    printf("  (Postorder asigură că copiii sunt eliberați înainte de părinte)\n\n");
    
    freeTree(root);
    root = NULL;  /* Evităm dangling pointer */
    
    printf("  ✓ Memorie eliberată cu succes!\n");
    printf("  ✓ Pointer setat la NULL pentru siguranță\n\n");
    
    printf("  SFAT: Rulați programul cu Valgrind pentru verificare:\n");
    printf("        valgrind --leak-check=full ./example1\n\n");
}

/* =============================================================================
 * FUNCȚIA MAIN
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     SĂPTĂMÂNA 7: ARBORI BINARI (BINARY TREES)                 ║\n");
    printf("║     Exemplu Complet Demonstrativ                              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    demo_part1_creation();
    demo_part2_traversals();
    demo_part3_analysis();
    demo_part4_search();
    demo_part5_operations();
    demo_part6_memory();
    
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     PROGRAM FINALIZAT CU SUCCES!                              ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    return 0;
}
