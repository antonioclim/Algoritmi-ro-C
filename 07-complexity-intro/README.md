# Săptămâna 7: Arbori Binari (Binary Trees)

## 🎯 Obiective de Învățare

La finalul acestei săptămâni, veți fi capabili să:

1. **Cunoașteți** (Remember): Definiți structura unui binary tree și terminologia asociată (root, leaf, parent, child, sibling, depth, height)
2. **Înțelegeți** (Understand): Explicați diferențele dintre tipurile de traversare (inorder, preorder, postorder, level-order) și cazurile lor de utilizare
3. **Aplicați** (Apply): Implementați operații fundamentale pe binary trees folosind recursivitate în limbajul C
4. **Analizați** (Analyse): Evaluați complexitatea temporală și spațială a algoritmilor pe arbori binari
5. **Evaluați** (Evaluate): Comparați diferite reprezentări și abordări pentru rezolvarea problemelor cu arbori
6. **Creați** (Create): Proiectați soluții complete pentru probleme practice folosind binary trees

---

## 📜 Context Istoric

### Originile Arborilor Binari

Conceptul de **binary tree** își are rădăcinile în matematica secolului XIX, când matematicieni precum **Arthur Cayley** (1821-1895) au studiat structurile arborescente în contextul teoriei grafurilor. Termenul "arbore" a fost utilizat pentru prima dată în sens matematic de Gustav Kirchhoff în 1847, în studiul circuitelor electrice.

Formalizarea modernă a arborilor binari ca structuri de date a avut loc în anii 1950-1960, odată cu dezvoltarea limbajelor de programare și a algoritmilor. **Donald Knuth** a contribuit semnificativ la sistematizarea cunoștințelor despre arbori în volumul său monumental "The Art of Computer Programming" (1968), unde a definit riguros terminologia și algoritmii fundamentali.

Traversările arborilor binari - **inorder**, **preorder** și **postorder** - au fost studiate extensiv în contextul compilatoarelor și al evaluării expresiilor matematice. Notația poloneză (prefix) și notația poloneză inversă (postfix), dezvoltate de logicianul Jan Łukasiewicz în anii 1920, au găsit aplicații directe în traversările arborilor de expresii.

### Personalitate Cheie: Donald Ervin Knuth (1938-)

Donald Knuth, profesor emerit la Stanford University, este considerat "părintele analizei algoritmilor". Opera sa principală, **"The Art of Computer Programming"**, reprezintă cea mai completă și riguroasă tratare a structurilor de date și algoritmilor.

> *"An algorithm must be seen to be believed, and the best way to learn what computers can do is to learn how to program."*
> — Donald E. Knuth

Contribuțiile sale la studiul arborilor binari includ:
- Formalizarea algoritmilor de traversare
- Analiza complexității operațiilor pe arbori
- Dezvoltarea conceptului de "threaded binary trees"

---

## 📚 Fundamente Teoretice

### 1. Definiția și Terminologia Binary Tree

Un **binary tree** este o structură de date ierarhică în care fiecare nod are cel mult doi copii, denumiți convențional **copilul stâng** (left child) și **copilul drept** (right child).

```
                    ┌─────────┐
                    │  ROOT   │ ← Rădăcina (root) - nivelul 0
                    │   (A)   │
                    └────┬────┘
                         │
           ┌─────────────┴─────────────┐
           │                           │
      ┌────┴────┐                ┌─────┴────┐
      │    B    │                │    C     │ ← Nivelul 1
      │ (copil  │                │ (copil   │
      │  stâng) │                │  drept)  │
      └────┬────┘                └─────┬────┘
           │                           │
    ┌──────┴──────┐             ┌──────┴──────┐
    │             │             │             │
┌───┴───┐    ┌────┴───┐    ┌────┴───┐   ┌─────┴───┐
│   D   │    │   E    │    │   F    │   │    G    │ ← Nivelul 2
│(leaf) │    │        │    │(leaf)  │   │ (leaf)  │
└───────┘    └────┬───┘    └────────┘   └─────────┘
                  │
            ┌─────┴─────┐
            │     H     │ ← Nivelul 3 (Adâncimea maximă = 3)
            │  (leaf)   │
            └───────────┘
```

**Terminologie fundamentală:**

| Termen | Definiție | Exemplu din diagramă |
|--------|-----------|---------------------|
| **Root** (rădăcină) | Nodul de la care pornesc toate ramurile | A |
| **Leaf** (frunză) | Nod fără copii | D, F, G, H |
| **Parent** (părinte) | Nodul care are copii | B este parent pentru D și E |
| **Child** (copil) | Nod direct conectat sub un parent | D și E sunt copiii lui B |
| **Sibling** (frate) | Noduri cu același parent | D și E sunt siblings |
| **Depth** (adâncime) | Numărul de muchii de la root la nod | Depth(H) = 3 |
| **Height** (înălțime) | Maximul adâncimilor tuturor nodurilor | Height(tree) = 3 |
| **Subtree** (subarbore) | Arbore format dintr-un nod și descendenții săi | Subarborele cu rădăcina B |

### 2. Reprezentarea în C

Structura standard pentru un nod de binary tree în C:

```c
/**
 * Structura unui nod în binary tree
 */
typedef struct TreeNode {
    int data;                    /* Valoarea stocată în nod */
    struct TreeNode *left;       /* Pointer către copilul stâng */
    struct TreeNode *right;      /* Pointer către copilul drept */
} TreeNode;

/**
 * Funcție de creare a unui nod nou
 * 
 * @param value Valoarea de stocat în nod
 * @return Pointer către nodul nou creat sau NULL la eroare
 */
TreeNode* createNode(int value) {
    TreeNode *newNode = (TreeNode*)malloc(sizeof(TreeNode));
    if (newNode == NULL) {
        fprintf(stderr, "Eroare: Alocare memorie eșuată!\n");
        return NULL;
    }
    newNode->data = value;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}
```

### 3. Algoritmi de Traversare

Traversarea unui binary tree înseamnă vizitarea sistematică a tuturor nodurilor. Există patru metode principale:

#### 3.1 Inorder Traversal (Stânga → Rădăcină → Dreapta)

```
Algoritm INORDER(node):
    DACĂ node ≠ NULL ATUNCI
        INORDER(node.left)      // Vizitează subarborele stâng
        PROCESEAZĂ(node.data)   // Procesează nodul curent
        INORDER(node.right)     // Vizitează subarborele drept
```

**Rezultat pentru arborele de mai sus:** D → B → H → E → A → F → C → G

```c
void inorderTraversal(TreeNode *node) {
    if (node != NULL) {
        inorderTraversal(node->left);
        printf("%d ", node->data);
        inorderTraversal(node->right);
    }
}
```

#### 3.2 Preorder Traversal (Rădăcină → Stânga → Dreapta)

```
Algoritm PREORDER(node):
    DACĂ node ≠ NULL ATUNCI
        PROCESEAZĂ(node.data)   // Procesează nodul curent PRIMUL
        PREORDER(node.left)     // Vizitează subarborele stâng
        PREORDER(node.right)    // Vizitează subarborele drept
```

**Rezultat:** A → B → D → E → H → C → F → G

**Utilizări:** Copierea structurii arborelui, serializare, evaluarea expresiilor prefix.

#### 3.3 Postorder Traversal (Stânga → Dreapta → Rădăcină)

```
Algoritm POSTORDER(node):
    DACĂ node ≠ NULL ATUNCI
        POSTORDER(node.left)    // Vizitează subarborele stâng
        POSTORDER(node.right)   // Vizitează subarborele drept
        PROCESEAZĂ(node.data)   // Procesează nodul curent ULTIMUL
```

**Rezultat:** D → H → E → B → F → G → C → A

**Utilizări:** Ștergerea arborelui (dealocarea memoriei), evaluarea expresiilor postfix.

#### 3.4 Level-order Traversal (Breadth-First)

Traversează arborele nivel cu nivel, de la stânga la dreapta.

**Rezultat:** A → B → C → D → E → F → G → H

**Implementare:** Necesită utilizarea unei structuri de tip queue.

### Comparație Complexitate

| Traversare | Timp | Spațiu (stivă) | Spațiu (queue) |
|------------|------|----------------|----------------|
| Inorder | O(n) | O(h) | - |
| Preorder | O(n) | O(h) | - |
| Postorder | O(n) | O(h) | - |
| Level-order | O(n) | - | O(w) |

Unde: n = număr noduri, h = înălțime arbore, w = lățimea maximă (2^h în cazul cel mai rău)

---

## 🏭 Aplicații Industriale

### 1. Compilatoare - Abstract Syntax Trees (AST)

Compilatoarele folosesc binary trees pentru reprezentarea expresiilor matematice:

```c
/* Reprezentarea expresiei: (3 + 5) * 2 */
/*
           [*]
          /   \
        [+]   [2]
       /   \
     [3]   [5]
*/

typedef struct ExprNode {
    char operator;           /* '+', '-', '*', '/' sau '\0' pentru numere */
    int value;               /* Valoarea dacă e operand */
    struct ExprNode *left;
    struct ExprNode *right;
} ExprNode;

int evaluateExpression(ExprNode *node) {
    if (node->operator == '\0') {
        return node->value;  /* Nod frunză - returnează valoarea */
    }
    
    int leftVal = evaluateExpression(node->left);
    int rightVal = evaluateExpression(node->right);
    
    switch (node->operator) {
        case '+': return leftVal + rightVal;
        case '-': return leftVal - rightVal;
        case '*': return leftVal * rightVal;
        case '/': return rightVal != 0 ? leftVal / rightVal : 0;
        default: return 0;
    }
}
```

### 2. Sisteme de Fișiere - Directory Trees

```c
/* Model simplificat al unui sistem de fișiere */
typedef struct FSNode {
    char name[256];
    int isDirectory;
    struct FSNode *firstChild;   /* Primul copil (pentru directoare) */
    struct FSNode *nextSibling;  /* Următorul frate */
} FSNode;

void listDirectory(FSNode *dir, int depth) {
    if (dir == NULL) return;
    
    for (int i = 0; i < depth; i++) printf("  ");
    printf("%s%s\n", dir->name, dir->isDirectory ? "/" : "");
    
    if (dir->isDirectory) {
        listDirectory(dir->firstChild, depth + 1);
    }
    listDirectory(dir->nextSibling, depth);
}
```

### 3. Decision Trees în Machine Learning

```c
typedef struct DecisionNode {
    char question[256];          /* Întrebarea de decizie */
    char classification[64];     /* Clasificarea finală (pentru frunze) */
    int threshold;               /* Prag pentru decizii numerice */
    struct DecisionNode *yes;    /* Ramura "da" */
    struct DecisionNode *no;     /* Ramura "nu" */
} DecisionNode;

const char* classify(DecisionNode *node, int features[], int idx) {
    if (node->yes == NULL && node->no == NULL) {
        return node->classification;
    }
    
    if (features[idx] > node->threshold) {
        return classify(node->yes, features, idx + 1);
    } else {
        return classify(node->no, features, idx + 1);
    }
}
```

### 4. Jocuri - Arbori de Decizie AI (Minimax)

```c
typedef struct GameNode {
    int boardState[9];           /* Starea tablei de joc (Tic-Tac-Toe) */
    int score;                   /* Scorul evaluării */
    struct GameNode *moves[9];   /* Mișcările posibile */
    int moveCount;
} GameNode;

int minimax(GameNode *node, int depth, int isMaximizing) {
    if (depth == 0 || isTerminal(node)) {
        return evaluateBoard(node->boardState);
    }
    
    if (isMaximizing) {
        int maxEval = INT_MIN;
        for (int i = 0; i < node->moveCount; i++) {
            int eval = minimax(node->moves[i], depth - 1, 0);
            maxEval = (eval > maxEval) ? eval : maxEval;
        }
        return maxEval;
    } else {
        int minEval = INT_MAX;
        for (int i = 0; i < node->moveCount; i++) {
            int eval = minimax(node->moves[i], depth - 1, 1);
            minEval = (eval < minEval) ? eval : minEval;
        }
        return minEval;
    }
}
```

### 5. HTML DOM - Document Object Model

Browserele web reprezintă documentele HTML ca arbori:

```c
typedef struct DOMNode {
    char tagName[32];            /* "div", "span", "p", etc. */
    char id[64];
    char className[128];
    char textContent[1024];
    struct DOMNode *firstChild;
    struct DOMNode *nextSibling;
    struct DOMNode *parent;
} DOMNode;

DOMNode* getElementById(DOMNode *root, const char *id) {
    if (root == NULL) return NULL;
    
    if (strcmp(root->id, id) == 0) {
        return root;
    }
    
    DOMNode *found = getElementById(root->firstChild, id);
    if (found != NULL) return found;
    
    return getElementById(root->nextSibling, id);
}
```

---

## 💻 Exerciții de Laborator

### Exercițiul 1: Operații Fundamentale pe Binary Trees

**Obiectiv:** Implementarea operațiilor de bază pe arbori binari.

**Cerințe:**
1. Crearea nodurilor și construirea unui arbore
2. Implementarea celor 4 tipuri de traversare
3. Calcularea înălțimii și numărului de noduri
4. Căutarea unei valori în arbore
5. Eliberarea corectă a memoriei

**Fișier:** `src/exercise1.c`

### Exercițiul 2: Arbori de Expresii

**Obiectiv:** Construirea și evaluarea arborilor de expresii matematice.

**Cerințe:**
1. Parsarea unei expresii matematice în notație infix
2. Construirea arborelui de expresii
3. Evaluarea expresiei folosind traversarea postorder
4. Afișarea expresiei în notație prefix și postfix
5. Simplificarea expresiilor (bonus)

**Fișier:** `src/exercise2.c`

---

## 🔧 Compilare și Execuție

### Comenzi Make disponibile:

```bash
# Compilează toate țintele
make

# Compilează și rulează exemplul demonstrativ
make run

# Compilează și rulează exercițiul 1
make run-ex1

# Compilează și rulează exercițiul 2
make run-ex2

# Execută testele automate
make test

# Verifică memory leaks cu Valgrind
make valgrind

# Curăță fișierele generate
make clean

# Afișează ajutorul
make help
```

### Compilare manuală:

```bash
# Compilare cu toate warning-urile activate
gcc -Wall -Wextra -std=c11 -g -o example1 src/example1.c

# Rulare cu Valgrind pentru verificarea memoriei
valgrind --leak-check=full --show-leak-kinds=all ./example1
```

---

## 📁 Structura Directorului

```
week-07-arbori-binari/
├── README.md                           # Acest fișier
├── Makefile                            # Automatizare compilare
│
├── slides/
│   ├── presentation-week07.html        # Prezentarea principală (35-40 slide-uri)
│   └── presentation-comparativ.html    # Comparație Pseudocode → C → Python
│
├── src/
│   ├── example1.c                      # Exemplu complet demonstrativ
│   ├── exercise1.c                     # Exercițiu: Operații fundamentale
│   └── exercise2.c                     # Exercițiu: Arbori de expresii
│
├── data/
│   ├── tree_input.txt                  # Date de intrare pentru construcție arbore
│   └── expressions.txt                 # Expresii matematice pentru testare
│
├── tests/
│   ├── test1_input.txt                 # Input test exercițiu 1
│   ├── test1_expected.txt              # Output așteptat exercițiu 1
│   ├── test2_input.txt                 # Input test exercițiu 2
│   └── test2_expected.txt              # Output așteptat exercițiu 2
│
├── teme/
│   ├── homework-requirements.md        # Cerințe teme (2 × 50 puncte)
│   └── homework-extended.md            # Provocări bonus (+10 puncte fiecare)
│
└── solution/
    ├── exercise1_sol.c                 # Soluție exercițiu 1
    ├── exercise2_sol.c                 # Soluție exercițiu 2
    ├── homework1_sol.c                 # Soluție temă 1
    └── homework2_sol.c                 # Soluție temă 2
```

---

## 📖 Lectură Recomandată

### Esențiale

1. **Knuth, D.E.** - *The Art of Computer Programming, Vol. 1: Fundamental Algorithms*, Cap. 2.3 "Trees" (Ed. 3, 1997)
2. **Cormen, T.H. et al.** - *Introduction to Algorithms*, Cap. 10 "Elementary Data Structures" și Cap. 12 "Binary Search Trees" (Ed. 3, MIT Press)
3. **Sedgewick, R.** - *Algorithms in C*, Parts 1-4, Cap. 5 "Trees" (Addison-Wesley)

### Avansate

4. **Aho, A.V., Lam, M.S., Sethi, R., Ullman, J.D.** - *Compilers: Principles, Techniques, and Tools* ("Dragon Book"), Cap. 5 "Syntax-Directed Translation"
5. **Weiss, M.A.** - *Data Structures and Algorithm Analysis in C* (Ed. 2, Pearson)

### Resurse Online

- [Visualgo - Binary Tree Visualization](https://visualgo.net/en/bst) - Vizualizări interactive
- [GeeksforGeeks - Binary Tree](https://www.geeksforgeeks.org/binary-tree-data-structure/) - Tutoriale și exerciții
- [CS50 - Data Structures](https://cs50.harvard.edu/x/2024/weeks/5/) - Curs Harvard gratuit

---

## ✅ Lista de Autoevaluare

Verificați-vă cunoștințele răspunzând la următoarele întrebări:

- [ ] Pot defini terminologia de bază a arborilor binari (root, leaf, height, depth)?
- [ ] Știu să implementez structura `TreeNode` în C cu pointeri?
- [ ] Pot implementa toate cele 4 tipuri de traversare (inorder, preorder, postorder, level-order)?
- [ ] Înțeleg diferența dintre traversarea depth-first și breadth-first?
- [ ] Pot calcula înălțimea și numărul de noduri ale unui arbore recursiv?
- [ ] Știu să eliberez corect memoria unui arbore (dealocarea cu postorder)?
- [ ] Pot construi un arbore de expresii dintr-o expresie matematică?
- [ ] Înțeleg complexitatea temporală O(n) și spațială O(h) a operațiilor?
- [ ] Pot identifica cazurile de utilizare în aplicații reale?
- [ ] Știu să folosesc Valgrind pentru verificarea memory leaks?

---

## 💼 Pregătire pentru Interviuri

### Întrebări frecvente la interviuri tehnice:

1. **"Care este diferența dintre un arbore binar și un BST (Binary Search Tree)?"**
   - *Arbore binar:* Fiecare nod are maxim 2 copii, fără restricții pe valori
   - *BST:* Arbore binar cu proprietatea că valorile din subarborele stâng < rădăcină < valorile din subarborele drept

2. **"Cum verifici dacă doi arbori binari sunt identici?"**
   - Traversare simultană recursivă, comparând valorile și structura

3. **"Care este complexitatea de timp și spațiu pentru traversarea unui arbore?"**
   - Timp: O(n) - vizităm fiecare nod o dată
   - Spațiu: O(h) pentru stivă recursivă, unde h = înălțimea arborelui

4. **"Cum găsești cel mai mic strămoș comun (LCA) a două noduri?"**
   - Metodă recursivă: verifică dacă nodurile sunt în subarbori diferiți

5. **"Cum serializezi și deserializezi un arbore binar?"**
   - Preorder traversal cu marcaje pentru noduri NULL, apoi reconstrucție

---

## 🔗 Previzualizare Săptămâna 8

**Tema:** Binary Search Trees (BST) - Arbori Binari de Căutare

Săptămâna viitoare vom explora:
- Proprietatea de ordonare a BST
- Operații de search, insert și delete în O(log n) (cazul mediu)
- Probleme de dezechilibru și soluții (introducere în AVL)
- Aplicații: dicționare, baze de date, autocomplete

---

*Generat pentru cursul ATP - Academia de Studii Economice București*
*Săptămâna 7 | Versiune: 1.0 | Data: 2025*
