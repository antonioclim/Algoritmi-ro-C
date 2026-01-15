# Provocări Extinse - Săptămâna 7

## 🚀 Provocări Avansate (Opțional)

Fiecare provocare rezolvată corect aduce **+10 puncte bonus**.

---

## ⭐ Provocarea 1: Serializare și Deserializare (Dificultate: Medie)

### Descriere

Implementați funcții pentru a salva un arbore binar într-un fișier și a-l reconstrui ulterior. Acest lucru este esențial pentru persistența datelor.

### Cerințe

```c
/**
 * Serializează arborele într-un fișier
 * Format recomandat: preorder cu marcaje pentru NULL
 * Exemplu: "50 30 20 # # 40 # # 70 60 # # 80 # #"
 * unde # reprezintă NULL
 */
void serializeTree(TreeNode *root, const char *filename);

/**
 * Deserializează arborele din fișier
 * Reconstruiește exact structura originală
 */
TreeNode* deserializeTree(const char *filename);
```

### Testare

```c
// Arbore original
TreeNode *original = createSampleTree();

// Salvare
serializeTree(original, "tree.dat");

// Reconstrucție
TreeNode *loaded = deserializeTree("tree.dat");

// Verificare
assert(areIdentical(original, loaded) == 1);
```

### Puncte bonus: +10

---

## ⭐ Provocarea 2: Lowest Common Ancestor (LCA) (Dificultate: Medie-Grea)

### Descriere

Implementați algoritmul pentru găsirea celui mai apropiat strămoș comun (LCA) a două noduri. Aceasta este o problemă clasică de interviu.

### Cerințe

```c
/**
 * Găsește cel mai apropiat strămoș comun al nodurilor cu valorile a și b
 * 
 * Exemplu pentru arborele:
 *        3
 *       / \
 *      5   1
 *     / \ / \
 *    6  2 0  8
 *      / \
 *     7   4
 * 
 * LCA(5, 1) = 3
 * LCA(5, 4) = 5
 * LCA(6, 4) = 5
 * LCA(7, 8) = 3
 */
TreeNode* findLCA(TreeNode *root, int a, int b);

/**
 * Calculează distanța dintre două noduri
 * Distanța = numărul de muchii pe calea dintre ele
 */
int distanceBetweenNodes(TreeNode *root, int a, int b);
```

### Algoritm sugerat

1. Recursiv, verificați dacă nodul curent este unul dintre cele căutate
2. Căutați în subarborele stâng și drept
3. Dacă ambele subarbori returnează non-NULL, nodul curent este LCA
4. Altfel, returnați rezultatul non-NULL

### Puncte bonus: +10

---

## ⭐ Provocarea 3: Verificare Arbore BST Valid (Dificultate: Medie)

### Descriere

Implementați o funcție care verifică dacă un arbore binar este un Binary Search Tree valid.

### Cerințe

```c
/**
 * Verifică dacă arborele respectă proprietatea BST:
 * - Toate valorile din subarborele stâng < valoarea nodului
 * - Toate valorile din subarborele drept > valoarea nodului
 * - Ambii subarbori sunt BST valizi
 * 
 * ATENȚIE: Nu e suficient să verificați doar copiii direcți!
 * 
 * Exemplu arbore INVALID (deși copiii direcți par ok):
 *       10
 *      /  \
 *     5    15
 *    / \
 *   1   12   <- 12 > 10, invalid!
 */
int isValidBST(TreeNode *root);

/**
 * Versiune cu range-uri explicite
 */
int isValidBSTHelper(TreeNode *node, int minVal, int maxVal);
```

### Hint

Folosiți limite min/max care se propagă în jos:
- Pentru copilul stâng: maxVal devine valoarea curentă
- Pentru copilul drept: minVal devine valoarea curentă

### Puncte bonus: +10

---

## ⭐ Provocarea 4: Arbore din Traversări (Dificultate: Grea)

### Descriere

Reconstruiți arborele binar având doar rezultatele traversărilor inorder și preorder.

### Cerințe

```c
/**
 * Construiește arborele din traversările inorder și preorder
 * 
 * Exemplu:
 * Inorder:  [4, 2, 5, 1, 6, 3, 7]
 * Preorder: [1, 2, 4, 5, 3, 6, 7]
 * 
 * Arbore rezultat:
 *        1
 *       / \
 *      2   3
 *     / \ / \
 *    4  5 6  7
 * 
 * @param inorder Array cu traversarea inorder
 * @param preorder Array cu traversarea preorder
 * @param size Dimensiunea array-urilor (trebuie să fie egale)
 * @return Rădăcina arborelui reconstruit
 */
TreeNode* buildTreeFromTraversals(int inorder[], int preorder[], int size);
```

### Algoritm

1. Primul element din preorder este rădăcina
2. Găsiți acest element în inorder - împarte array-ul în stânga și dreapta
3. Recursiv, construiți subarborele stâng și drept
4. Folosiți indecși pentru a evita copierea array-urilor

### Puncte bonus: +10

---

## ⭐ Provocarea 5: Arbore cu Fire (Threaded Binary Tree) (Dificultate: Grea)

### Descriere

Implementați un arbore binar cu fire (threaded) care permite traversare inorder fără recursie și fără stack.

### Cerințe

```c
/**
 * Structura pentru Threaded Binary Tree
 * 
 * Un nod "threaded" folosește pointerii NULL pentru a indica
 * predecesorul/succesorul în ordine inorder:
 * - Dacă right este NULL, îl folosim pentru a indica succesorul inorder
 * - Un flag indică dacă pointerul este "thread" sau legătură normală
 */
typedef struct ThreadedNode {
    int data;
    struct ThreadedNode *left;
    struct ThreadedNode *right;
    int leftThread;   /* 1 dacă left e thread, 0 dacă e copil */
    int rightThread;  /* 1 dacă right e thread, 0 dacă e copil */
} ThreadedNode;

/**
 * Convertește un arbore binar normal în arbore threaded
 */
ThreadedNode* convertToThreaded(TreeNode *root);

/**
 * Traversare inorder FĂRĂ recursie și FĂRĂ stack auxiliar
 * Complexitate spațiu: O(1)
 */
void inorderThreaded(ThreadedNode *root);

/**
 * Găsește succesorul inorder al unui nod
 */
ThreadedNode* inorderSuccessor(ThreadedNode *node);
```

### Avantaje

- Traversare în O(n) timp și O(1) spațiu
- Nu necesită stack (important pentru sisteme embedded)
- Accesează succesorul în O(1) pentru orice nod

### Puncte bonus: +10

---

## 📊 Sistem de Puncte Bonus

| Provocări Completate | Total Bonus |
|---------------------|-------------|
| 1 | +10 puncte |
| 2 | +20 puncte |
| 3 | +30 puncte |
| 4 | +40 puncte |
| Toate 5 | +50 puncte + Badge 🏆 "Binary Tree Master" |

---

## 📝 Reguli pentru Provocări

1. **Corectitudine** - Codul trebuie să funcționeze corect pentru toate cazurile de test
2. **Eficiență** - Respectați complexitatea cerută (unde este specificată)
3. **Memorie** - Fără memory leaks (verificat cu Valgrind)
4. **Documentație** - Comentați algoritmul folosit
5. **Teste** - Includeți minimum 3 cazuri de test pentru fiecare funcție

---

## 🎯 Sfaturi pentru Provocări

### Pentru Provocarea 1 (Serializare):
- Preorder + marcaje NULL este cea mai simplă abordare
- Aveți grijă la parsing-ul fișierului

### Pentru Provocarea 2 (LCA):
- Gândiți-vă recursiv: când un nod este LCA?
- Cazuri speciale: unul dintre noduri este strămoșul celuilalt

### Pentru Provocarea 3 (Valid BST):
- Nu verificați doar copiii direcți!
- Propagați limite min/max

### Pentru Provocarea 4 (Din traversări):
- Primul element preorder = rădăcină
- Poziția în inorder separă stânga de dreapta

### Pentru Provocarea 5 (Threaded):
- Este o optimizare clasică, studiați diagrame vizuale
- Morris Traversal este o alternativă similară

---

*Mult succes! Provocările vă vor ajuta să înțelegeți profund structura arborilor binari.*
