/**
 * =============================================================================
 * SOLUȚIE EXERCIȚIU 1: Operații Fundamentale pe Binary Trees
 * =============================================================================
 * ATENȚIE: Acest fișier este destinat EXCLUSIV instructorilor!
 * Nu distribuiți studenților înainte de deadline.
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

/* =============================================================================
 * STRUCTURA NODULUI - SOLUȚIE TODO 1
 * =============================================================================
 */

typedef struct TreeNode {
    int data;
    struct TreeNode *left;
    struct TreeNode *right;
} TreeNode;

/* =============================================================================
 * FUNCȚII DE CREARE - SOLUȚIE TODO 2-3
 * =============================================================================
 */

/**
 * SOLUȚIE TODO 2: createNode
 */
TreeNode* createNode(int value) {
    TreeNode *newNode = (TreeNode*)malloc(sizeof(TreeNode));
    
    if (newNode == NULL) {
        fprintf(stderr, "Eroare: Nu s-a putut aloca memorie!\n");
        return NULL;
    }
    
    newNode->data = value;
    newNode->left = NULL;
    newNode->right = NULL;
    
    return newNode;
}

/**
 * SOLUȚIE TODO 3: buildTreeFromArray
 */
TreeNode* buildTreeFromArray(int arr[], int index, int size) {
    /* Verificăm dacă indexul este valid și valoarea nu e -1 (NULL) */
    if (index >= size || arr[index] == -1) {
        return NULL;
    }
    
    /* Creăm nodul curent */
    TreeNode *node = createNode(arr[index]);
    if (node == NULL) {
        return NULL;
    }
    
    /* Construim recursiv subarborii */
    node->left = buildTreeFromArray(arr, 2 * index + 1, size);
    node->right = buildTreeFromArray(arr, 2 * index + 2, size);
    
    return node;
}

/* =============================================================================
 * FUNCȚII DE TRAVERSARE - SOLUȚIE TODO 4-6
 * =============================================================================
 */

/**
 * SOLUȚIE TODO 4: inorderTraversal (L → N → R)
 */
void inorderTraversal(TreeNode *node) {
    if (node != NULL) {
        inorderTraversal(node->left);    /* 1. Stânga */
        printf("%d ", node->data);        /* 2. Nod */
        inorderTraversal(node->right);   /* 3. Dreapta */
    }
}

/**
 * SOLUȚIE TODO 5: preorderTraversal (N → L → R)
 */
void preorderTraversal(TreeNode *node) {
    if (node != NULL) {
        printf("%d ", node->data);        /* 1. Nod */
        preorderTraversal(node->left);   /* 2. Stânga */
        preorderTraversal(node->right);  /* 3. Dreapta */
    }
}

/**
 * SOLUȚIE TODO 6: postorderTraversal (L → R → N)
 */
void postorderTraversal(TreeNode *node) {
    if (node != NULL) {
        postorderTraversal(node->left);  /* 1. Stânga */
        postorderTraversal(node->right); /* 2. Dreapta */
        printf("%d ", node->data);        /* 3. Nod */
    }
}

/* =============================================================================
 * FUNCȚII DE ANALIZĂ - SOLUȚIE TODO 7-9
 * =============================================================================
 */

/**
 * SOLUȚIE TODO 7: treeHeight
 */
int treeHeight(TreeNode *node) {
    if (node == NULL) {
        return -1;  /* Arbore vid */
    }
    
    int leftHeight = treeHeight(node->left);
    int rightHeight = treeHeight(node->right);
    
    /* Returnăm 1 + maximul înălțimilor subarborilor */
    return 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
}

/**
 * SOLUȚIE TODO 8: countNodes
 */
int countNodes(TreeNode *node) {
    if (node == NULL) {
        return 0;
    }
    return 1 + countNodes(node->left) + countNodes(node->right);
}

/**
 * SOLUȚIE TODO 9: countLeaves
 */
int countLeaves(TreeNode *node) {
    if (node == NULL) {
        return 0;
    }
    
    /* Verificăm dacă e frunză */
    if (node->left == NULL && node->right == NULL) {
        return 1;
    }
    
    return countLeaves(node->left) + countLeaves(node->right);
}

/* =============================================================================
 * FUNCȚIE DE CĂUTARE - SOLUȚIE TODO 10
 * =============================================================================
 */

/**
 * SOLUȚIE TODO 10: searchTree
 */
TreeNode* searchTree(TreeNode *node, int value) {
    if (node == NULL) {
        return NULL;
    }
    
    if (node->data == value) {
        return node;
    }
    
    /* Căutăm în stânga */
    TreeNode *found = searchTree(node->left, value);
    if (found != NULL) {
        return found;
    }
    
    /* Căutăm în dreapta */
    return searchTree(node->right, value);
}

/* =============================================================================
 * ELIBERARE MEMORIE - SOLUȚIE TODO 11
 * =============================================================================
 */

/**
 * SOLUȚIE TODO 11: freeTree
 * IMPORTANT: Postorder - copiii înainte de părinte!
 */
void freeTree(TreeNode *node) {
    if (node == NULL) {
        return;
    }
    
    freeTree(node->left);   /* 1. Eliberăm subarborele stâng */
    freeTree(node->right);  /* 2. Eliberăm subarborele drept */
    free(node);             /* 3. Eliberăm nodul curent */
}

/* =============================================================================
 * FUNCȚIA MAIN
 * =============================================================================
 */

int main(void) {
    int n;
    
    if (scanf("%d", &n) != 1 || n <= 0) {
        fprintf(stderr, "Eroare: Număr invalid de noduri\n");
        return 1;
    }
    
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
    
    TreeNode *root = buildTreeFromArray(arr, 0, n);
    free(arr);
    
    if (root == NULL) {
        fprintf(stderr, "Eroare: Nu s-a putut construi arborele\n");
        return 1;
    }
    
    printf("Inorder: ");
    inorderTraversal(root);
    printf("\n");
    
    printf("Preorder: ");
    preorderTraversal(root);
    printf("\n");
    
    printf("Postorder: ");
    postorderTraversal(root);
    printf("\n");
    
    printf("Height: %d\n", treeHeight(root));
    printf("Nodes: %d\n", countNodes(root));
    printf("Leaves: %d\n", countLeaves(root));
    
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
    
    freeTree(root);
    
    return 0;
}
