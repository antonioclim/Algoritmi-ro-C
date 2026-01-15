/**
 * =============================================================================
 * SOLUȚIE TEMĂ 1: Sistem de Gestionare a Fișierelor
 * =============================================================================
 * ATENȚIE: Acest fișier este destinat EXCLUSIV instructorilor!
 * =============================================================================
 */

#define _GNU_SOURCE  /* Pentru strdup */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NAME_LEN 64
#define MAX_PATH_LEN 256

/* =============================================================================
 * STRUCTURI DE DATE
 * =============================================================================
 */

typedef struct FSNode {
    char name[MAX_NAME_LEN];
    int isDirectory;          /* 1 = director, 0 = fișier */
    unsigned long size;       /* Dimensiune în bytes (pentru fișiere) */
    time_t createdAt;         /* Timestamp creare */
    struct FSNode *firstChild;   /* Primul copil (pentru directoare) */
    struct FSNode *nextSibling;  /* Următorul frate */
    struct FSNode *parent;       /* Părintele (opțional, util pentru navigare) */
} FSNode;

/* =============================================================================
 * FUNCȚII DE CREARE
 * =============================================================================
 */

/**
 * Creează un nod nou (fișier sau director)
 */
FSNode* createNode(const char *name, int isDirectory, unsigned long size) {
    FSNode *node = (FSNode*)malloc(sizeof(FSNode));
    if (node == NULL) {
        fprintf(stderr, "Eroare: Alocare memorie eșuată!\n");
        return NULL;
    }
    
    strncpy(node->name, name, MAX_NAME_LEN - 1);
    node->name[MAX_NAME_LEN - 1] = '\0';
    node->isDirectory = isDirectory;
    node->size = isDirectory ? 0 : size;
    node->createdAt = time(NULL);
    node->firstChild = NULL;
    node->nextSibling = NULL;
    node->parent = NULL;
    
    return node;
}

/**
 * Adaugă un copil la un director
 */
int addChild(FSNode *parent, FSNode *child) {
    if (parent == NULL || child == NULL) {
        return -1;
    }
    
    if (!parent->isDirectory) {
        fprintf(stderr, "Eroare: Nu se poate adăuga copil la un fișier!\n");
        return -1;
    }
    
    child->parent = parent;
    
    /* Dacă nu are copii, child devine primul copil */
    if (parent->firstChild == NULL) {
        parent->firstChild = child;
    } else {
        /* Adăugăm la sfârșitul listei de frați */
        FSNode *current = parent->firstChild;
        while (current->nextSibling != NULL) {
            current = current->nextSibling;
        }
        current->nextSibling = child;
    }
    
    return 0;
}

/* =============================================================================
 * FUNCȚII DE CĂUTARE
 * =============================================================================
 */

/**
 * Găsește un nod după cale (ex: "/home/user/doc.txt")
 */
FSNode* findNode(FSNode *root, const char *path) {
    if (root == NULL || path == NULL) {
        return NULL;
    }
    
    /* Facem o copie a căii pentru tokenizare */
    char *pathCopy = strdup(path);
    if (pathCopy == NULL) {
        return NULL;
    }
    
    FSNode *current = root;
    char *token = strtok(pathCopy, "/");
    
    /* Sărim peste primul token dacă e root */
    if (token != NULL && strcmp(token, root->name) == 0) {
        token = strtok(NULL, "/");
    }
    
    while (token != NULL && current != NULL) {
        /* Căutăm printre copiii curenți */
        FSNode *child = current->firstChild;
        FSNode *found = NULL;
        
        while (child != NULL) {
            if (strcmp(child->name, token) == 0) {
                found = child;
                break;
            }
            child = child->nextSibling;
        }
        
        if (found == NULL) {
            free(pathCopy);
            return NULL;  /* Nu am găsit */
        }
        
        current = found;
        token = strtok(NULL, "/");
    }
    
    free(pathCopy);
    return current;
}

/**
 * Șterge un nod și toți descendenții
 */
void deleteNode(FSNode *node) {
    if (node == NULL) {
        return;
    }
    
    /* Ștergem recursiv copiii */
    FSNode *child = node->firstChild;
    while (child != NULL) {
        FSNode *next = child->nextSibling;
        deleteNode(child);
        child = next;
    }
    
    free(node);
}

/**
 * Șterge un nod după cale
 */
int deleteNodeByPath(FSNode *root, const char *path) {
    FSNode *node = findNode(root, path);
    if (node == NULL || node == root) {
        return -1;  /* Nu găsim sau încercăm să ștergem root */
    }
    
    /* Eliminăm din lista de frați a părintelui */
    if (node->parent != NULL) {
        if (node->parent->firstChild == node) {
            node->parent->firstChild = node->nextSibling;
        } else {
            FSNode *sibling = node->parent->firstChild;
            while (sibling != NULL && sibling->nextSibling != node) {
                sibling = sibling->nextSibling;
            }
            if (sibling != NULL) {
                sibling->nextSibling = node->nextSibling;
            }
        }
    }
    
    deleteNode(node);
    return 0;
}

/* =============================================================================
 * FUNCȚII DE AFIȘARE
 * =============================================================================
 */

/**
 * Afișează conținutul directorului recursiv
 */
void listDirectory(FSNode *node, int depth) {
    if (node == NULL) {
        return;
    }
    
    /* Indentare */
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }
    
    /* Afișăm nodul */
    if (node->isDirectory) {
        printf("%s/\n", node->name);
    } else {
        printf("%s (%lu bytes)\n", node->name, node->size);
    }
    
    /* Afișăm copiii (dacă e director) */
    if (node->isDirectory) {
        FSNode *child = node->firstChild;
        while (child != NULL) {
            listDirectory(child, depth + 1);
            child = child->nextSibling;
        }
    }
}

/**
 * Afișează arborele în stil tree
 */
void printTree(FSNode *node, const char *prefix, int isLast) {
    if (node == NULL) {
        return;
    }
    
    printf("%s", prefix);
    printf("%s", isLast ? "└── " : "├── ");
    
    if (node->isDirectory) {
        printf("%s/\n", node->name);
    } else {
        printf("%s (%lu bytes)\n", node->name, node->size);
    }
    
    /* Calculăm noul prefix */
    char newPrefix[MAX_PATH_LEN];
    snprintf(newPrefix, MAX_PATH_LEN, "%s%s", prefix, isLast ? "    " : "│   ");
    
    /* Numărăm copiii pentru a ști care e ultimul */
    int childCount = 0;
    FSNode *child = node->firstChild;
    while (child != NULL) {
        childCount++;
        child = child->nextSibling;
    }
    
    /* Afișăm copiii */
    child = node->firstChild;
    int index = 0;
    while (child != NULL) {
        index++;
        printTree(child, newPrefix, index == childCount);
        child = child->nextSibling;
    }
}

/* =============================================================================
 * FUNCȚII DE STATISTICI
 * =============================================================================
 */

/**
 * Calculează dimensiunea totală recursiv
 */
unsigned long calculateSize(FSNode *node) {
    if (node == NULL) {
        return 0;
    }
    
    unsigned long total = node->size;
    
    if (node->isDirectory) {
        FSNode *child = node->firstChild;
        while (child != NULL) {
            total += calculateSize(child);
            child = child->nextSibling;
        }
    }
    
    return total;
}

/**
 * Numără toate fișierele
 */
int countFiles(FSNode *node) {
    if (node == NULL) {
        return 0;
    }
    
    int count = node->isDirectory ? 0 : 1;
    
    FSNode *child = node->firstChild;
    while (child != NULL) {
        count += countFiles(child);
        child = child->nextSibling;
    }
    
    return count;
}

/**
 * Numără toate directoarele
 */
int countDirectories(FSNode *node) {
    if (node == NULL) {
        return 0;
    }
    
    int count = node->isDirectory ? 1 : 0;
    
    FSNode *child = node->firstChild;
    while (child != NULL) {
        count += countDirectories(child);
        child = child->nextSibling;
    }
    
    return count;
}

/* =============================================================================
 * FUNCȚII DE CĂUTARE AVANSATĂ
 * =============================================================================
 */

/**
 * Căutare după nume (afișează toate potrivirile)
 */
void searchByName(FSNode *node, const char *name, const char *currentPath) {
    if (node == NULL) {
        return;
    }
    
    char fullPath[MAX_PATH_LEN];
    snprintf(fullPath, MAX_PATH_LEN, "%s/%s", currentPath, node->name);
    
    if (strcmp(node->name, name) == 0) {
        printf("  %s\n", fullPath);
    }
    
    FSNode *child = node->firstChild;
    while (child != NULL) {
        searchByName(child, name, fullPath);
        child = child->nextSibling;
    }
}

/**
 * Verifică dacă un fișier are o anumită extensie
 */
int hasExtension(const char *filename, const char *ext) {
    const char *dot = strrchr(filename, '.');
    if (dot == NULL) {
        return 0;
    }
    return strcmp(dot + 1, ext) == 0;
}

/**
 * Căutare după extensie
 */
void searchByExtension(FSNode *node, const char *ext, const char *currentPath) {
    if (node == NULL) {
        return;
    }
    
    char fullPath[MAX_PATH_LEN];
    snprintf(fullPath, MAX_PATH_LEN, "%s/%s", currentPath, node->name);
    
    if (!node->isDirectory && hasExtension(node->name, ext)) {
        printf("  %s (%lu bytes)\n", fullPath, node->size);
    }
    
    FSNode *child = node->firstChild;
    while (child != NULL) {
        searchByExtension(child, ext, fullPath);
        child = child->nextSibling;
    }
}

/* =============================================================================
 * ELIBERAREA MEMORIEI
 * =============================================================================
 */

void freeFileSystem(FSNode *node) {
    deleteNode(node);  /* deleteNode face același lucru */
}

/* =============================================================================
 * FUNCȚIA MAIN - DEMONSTRAȚIE
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     SISTEM DE GESTIONARE A FIȘIERELOR                         ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Creăm structura de directoare */
    FSNode *root = createNode("/", 1, 0);
    FSNode *home = createNode("home", 1, 0);
    FSNode *user1 = createNode("alice", 1, 0);
    FSNode *user2 = createNode("bob", 1, 0);
    FSNode *docs = createNode("documents", 1, 0);
    FSNode *pics = createNode("pictures", 1, 0);
    FSNode *projects = createNode("projects", 1, 0);
    
    /* Fișiere */
    FSNode *thesis = createNode("thesis.pdf", 0, 2048576);
    FSNode *notes = createNode("notes.txt", 0, 1024);
    FSNode *photo1 = createNode("vacation.jpg", 0, 4096000);
    FSNode *photo2 = createNode("profile.png", 0, 512000);
    FSNode *code = createNode("main.c", 0, 8192);
    FSNode *readme = createNode("README.md", 0, 2048);
    
    /* Construim ierarhia */
    addChild(root, home);
    addChild(home, user1);
    addChild(home, user2);
    addChild(user1, docs);
    addChild(user1, pics);
    addChild(user2, projects);
    addChild(docs, thesis);
    addChild(docs, notes);
    addChild(pics, photo1);
    addChild(pics, photo2);
    addChild(projects, code);
    addChild(projects, readme);
    
    /* Afișăm structura */
    printf("📁 Structura sistemului de fișiere:\n\n");
    printTree(root, "", 1);
    
    /* Statistici */
    printf("\n📊 Statistici:\n");
    printf("  Total fișiere: %d\n", countFiles(root));
    printf("  Total directoare: %d\n", countDirectories(root));
    printf("  Dimensiune totală: %.2f MB\n", calculateSize(root) / (1024.0 * 1024.0));
    
    /* Căutări */
    printf("\n🔍 Căutare fișiere .txt:\n");
    searchByExtension(root, "txt", "");
    
    printf("\n🔍 Căutare fișiere .c:\n");
    searchByExtension(root, "c", "");
    
    /* Găsim un nod specific */
    printf("\n📍 Găsire cale '/home/alice/documents':\n");
    FSNode *found = findNode(root, "/home/alice/documents");
    if (found != NULL) {
        printf("  GĂSIT: %s (director: %s)\n", 
               found->name, 
               found->isDirectory ? "DA" : "NU");
        printf("  Dimensiune conținut: %lu bytes\n", calculateSize(found));
    }
    
    /* Eliberăm memoria */
    freeFileSystem(root);
    
    printf("\n✓ Program finalizat cu succes!\n\n");
    
    return 0;
}
