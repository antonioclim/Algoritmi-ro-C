# Săptămâna 09: Arbori AVL (Adelson-Velsky și Landis)

## 🎯 Obiective de Învățare

La finalul acestei săptămâni, studenții vor fi capabili să:

1. **Recunoască** caracteristicile definitorii ale unui arbore AVL și diferențele față de un BST standard
2. **Înțeleagă** conceptul de balance factor și rolul său în menținerea echilibrului arborelui
3. **Aplice** cele patru tipuri de rotații (LL, RR, LR, RL) pentru reechilibrarea arborelui
4. **Analizeze** complexitatea temporală a operațiilor pe arbori AVL versus arbori BST neechilibrați
5. **Evalueze** când utilizarea unui arbore AVL este preferabilă față de alte structuri de date
6. **Creeze** implementări complete ale operațiilor de inserare și ștergere cu auto-echilibrare

---

## 📜 Context Istoric

Arborii AVL reprezintă prima structură de date de tip arbore binar de căutare auto-echilibrat, fiind inventați în 1962 de către doi matematicieni sovietici: **Georgy Adelson-Velsky** și **Evgenii Landis**. Denumirea "AVL" provine din inițialele numelor celor doi inventatori.

Contextul apariției acestei structuri de date este strâns legat de provocările computaționale ale anilor '60. În acea perioadă, cercetătorii din domeniul informaticii se confruntau cu problema degenerării arborilor binari de căutare în liste liniare, ceea ce ducea la performanțe catastrofale pentru operațiile de căutare - de la O(log n) la O(n) în cel mai rău caz.

Adelson-Velsky și Landis au publicat soluția lor în lucrarea *"An algorithm for the organization of information"* în revista sovietică *Doklady Akademii Nauk SSSR*. Această contribuție a deschis calea pentru dezvoltarea ulterioară a altor arbori echilibrați precum Red-Black trees (1972), B-trees (1970) și Splay trees (1985).

### Personalitate Cheie: Georgy Maximovich Adelson-Velsky (1922-2014)

Georgy Adelson-Velsky a fost un matematician și informatician sovietic, considerat unul dintre pionierii informaticii din Uniunea Sovietică. Pe lângă contribuția sa la arborii AVL, Adelson-Velsky este cunoscut pentru:

- Co-crearea programului de șah **Kaissa**, primul program de calculator care a câștigat un campionat mondial de șah pentru computere (1974)
- Contribuții fundamentale la teoria jocurilor și inteligența artificială
- Formarea unei generații întregi de informaticieni sovietici

> *"Echilibrul nu este un scop în sine, ci un mijloc de a garanta eficiența în orice circumstanță."*
> — Georgy Adelson-Velsky

---

## 📚 Fundamente Teoretice

### 1. Definiția Arborelui AVL

Un arbore AVL este un **arbore binar de căutare** (BST) care satisface o proprietate suplimentară de echilibru: pentru **fiecare nod** din arbore, diferența absolutăîntre înălțimile subarborelui stâng și drept este cel mult 1.

```
Proprietatea AVL:
┌─────────────────────────────────────────────────────────────┐
│  |height(left_subtree) - height(right_subtree)| ≤ 1        │
│                                                             │
│  Pentru FIECARE nod din arbore                              │
└─────────────────────────────────────────────────────────────┘
```

**Reprezentare vizuală - Arbore AVL valid:**

```
              50 (bf=0)
             /  \
      (bf=1) 30   70 (bf=-1)
            /  \    \
          20   40   80
         /
        10
```

**Arbore BST INVALID ca AVL (dezechilibrat):**

```
              50 (bf=2)  ← INVALID!
             /
           30 (bf=1)
           /
         20 (bf=1)
         /
        10
```

### 2. Balance Factor (Factor de Echilibru)

**Balance factor** (bf) al unui nod este definit ca:

```
bf(node) = height(left_subtree) - height(right_subtree)
```

| Balance Factor | Interpretare |
|----------------|--------------|
| bf = -1 | Subarbore drept mai înalt cu 1 nivel |
| bf = 0 | Subarbori perfect echilibrați |
| bf = +1 | Subarbore stâng mai înalt cu 1 nivel |
| bf < -1 sau bf > +1 | **DEZECHILIBRU** - necesită rotație |

**Implementare în C:**

```c
typedef struct AVLNode {
    int key;
    struct AVLNode *left;
    struct AVLNode *right;
    int height;  /* Înălțimea nodului */
} AVLNode;

int height(AVLNode *node) {
    return (node == NULL) ? 0 : node->height;
}

int balance_factor(AVLNode *node) {
    if (node == NULL) return 0;
    return height(node->left) - height(node->right);
}
```

### 3. Cele Patru Tipuri de Rotații

Rotațiile sunt operațiile fundamentale prin care un arbore AVL își restaurează echilibrul după inserare sau ștergere.

#### 3.1 Rotație Simplă la Dreapta (Right Rotation / LL Case)

Aplicată când: **bf > 1** și **bf(left_child) ≥ 0**

```
    Înainte (LL):              După rotație:
         z (bf=2)                    y
        /                           / \
       y (bf≥0)        →           x   z
      /
     x

    z devine copil drept al lui y
```

**Implementare:**

```c
AVLNode* rotate_right(AVLNode *z) {
    AVLNode *y = z->left;
    AVLNode *T2 = y->right;
    
    /* Efectuează rotația */
    y->right = z;
    z->left = T2;
    
    /* Actualizează înălțimile */
    z->height = 1 + max(height(z->left), height(z->right));
    y->height = 1 + max(height(y->left), height(y->right));
    
    return y;  /* Noua rădăcină */
}
```

#### 3.2 Rotație Simplă la Stânga (Left Rotation / RR Case)

Aplicată când: **bf < -1** și **bf(right_child) ≤ 0**

```
    Înainte (RR):              După rotație:
       z (bf=-2)                     y
        \                           / \
         y (bf≤0)      →           z   x
          \
           x

    z devine copil stâng al lui y
```

**Implementare:**

```c
AVLNode* rotate_left(AVLNode *z) {
    AVLNode *y = z->right;
    AVLNode *T2 = y->left;
    
    /* Efectuează rotația */
    y->left = z;
    z->right = T2;
    
    /* Actualizează înălțimile */
    z->height = 1 + max(height(z->left), height(z->right));
    y->height = 1 + max(height(y->left), height(y->right));
    
    return y;  /* Noua rădăcină */
}
```

#### 3.3 Rotație Dublă Stânga-Dreapta (LR Case)

Aplicată când: **bf > 1** și **bf(left_child) < 0**

```
    Înainte (LR):        Pas 1 (Left):       Pas 2 (Right):
         z                    z                    x
        /                    /                    / \
       y          →         x          →        y   z
        \                  /
         x                y

    Rotație stânga pe y, apoi rotație dreapta pe z
```

#### 3.4 Rotație Dublă Dreapta-Stânga (RL Case)

Aplicată când: **bf < -1** și **bf(right_child) > 0**

```
    Înainte (RL):        Pas 1 (Right):      Pas 2 (Left):
       z                    z                    x
        \                    \                  / \
         y        →           x        →      z   y
        /                      \
       x                        y

    Rotație dreapta pe y, apoi rotație stânga pe z
```

### 4. Tabel Decizional pentru Rotații

| bf(nod) | bf(copil) | Tip Rotație | Acțiune |
|---------|-----------|-------------|---------|
| > 1 | ≥ 0 | LL | rotate_right(nod) |
| > 1 | < 0 | LR | rotate_left(left), rotate_right(nod) |
| < -1 | ≤ 0 | RR | rotate_left(nod) |
| < -1 | > 0 | RL | rotate_right(right), rotate_left(nod) |

### 5. Analiza Complexității

| Operație | AVL Tree | BST (caz mediu) | BST (cel mai rău caz) |
|----------|----------|-----------------|----------------------|
| Căutare | O(log n) | O(log n) | O(n) |
| Inserare | O(log n) | O(log n) | O(n) |
| Ștergere | O(log n) | O(log n) | O(n) |
| Spațiu | O(n) | O(n) | O(n) |

**Avantajul garantat:** Arborii AVL oferă **garanție** pentru O(log n) în cel mai rău caz, spre deosebire de BST-urile standard care pot degenera.

**Înălțimea maximă a unui arbore AVL:**

```
h_max ≈ 1.44 * log₂(n + 2)
```

Pentru un arbore AVL cu 1.000.000 de noduri:
- Înălțime maximă AVL: ~29 niveluri
- Înălțime maximă BST dezechilibrat: 999.999 niveluri

---

## 🏭 Aplicații Industriale

### 1. Baze de Date - Indexare

Sistemele de gestiune a bazelor de date folosesc variante ale arborilor echilibrați pentru indexare. Deși B-trees sunt mai comuni pentru storage pe disc, AVL trees sunt utilizați pentru indexuri în memorie.

```c
/* Exemplu: Index pe coloana "id" dintr-o tabelă */
typedef struct IndexEntry {
    int key;           /* Valoarea coloanei indexate */
    size_t row_offset; /* Poziția în fișierul de date */
} IndexEntry;

typedef struct AVLIndex {
    IndexEntry entry;
    struct AVLIndex *left, *right;
    int height;
} AVLIndex;

/* Căutare rapidă: găsește rândul cu id=42 */
AVLIndex* find_row(AVLIndex *index, int id) {
    if (index == NULL || index->entry.key == id)
        return index;
    if (id < index->entry.key)
        return find_row(index->left, id);
    return find_row(index->right, id);
}
```

### 2. Compilatoare - Symbol Tables

Compilatoarele moderne utilizează arbori echilibrați pentru tabela de simboluri, permițând căutări rapide ale identificatorilor.

```c
typedef struct Symbol {
    char name[64];
    enum { VAR, FUNC, TYPE } kind;
    int scope_level;
    /* alte metadate... */
} Symbol;

typedef struct SymbolNode {
    Symbol symbol;
    struct SymbolNode *left, *right;
    int height;
} SymbolNode;

/* Căutare simbol în compilator */
Symbol* lookup_symbol(SymbolNode *table, const char *name) {
    if (table == NULL) return NULL;
    int cmp = strcmp(name, table->symbol.name);
    if (cmp == 0) return &table->symbol;
    if (cmp < 0) return lookup_symbol(table->left, name);
    return lookup_symbol(table->right, name);
}
```

### 3. Sisteme de Operare - Process Scheduling

Schedulerele moderne (ex: CFS în Linux) folosesc arbori echilibrați pentru gestiunea proceselor.

```c
/* Simplificare conceptuală a Completely Fair Scheduler */
typedef struct Process {
    int pid;
    unsigned long vruntime;  /* Timp virtual de execuție */
    /* alte câmpuri... */
} Process;

typedef struct RunQueue {
    Process proc;
    struct RunQueue *left, *right;
    int height;
} RunQueue;

/* Procesul cu cel mai mic vruntime = următorul de executat */
Process* get_next_process(RunQueue *rq) {
    if (rq == NULL) return NULL;
    if (rq->left == NULL) return &rq->proc;
    return get_next_process(rq->left);
}
```

### 4. Sisteme GIS - Căutare Spațială

Aplicațiile de cartografie folosesc variante ale arborilor echilibrați pentru căutări eficiente.

```c
typedef struct Location {
    double latitude;
    double longitude;
    char name[128];
} Location;

/* Comparație după latitudine pentru arbore 1D */
int compare_lat(const Location *a, const Location *b) {
    if (a->latitude < b->latitude) return -1;
    if (a->latitude > b->latitude) return 1;
    return 0;
}
```

### 5. Jocuri Video - Collision Detection

Engine-urile de jocuri folosesc arbori echilibrați pentru organizarea obiectelor în spațiu.

```c
typedef struct GameObject {
    int id;
    float x, y, z;
    float bounding_radius;
} GameObject;

/* Arbore ordonat după coordonata X pentru broad-phase collision */
typedef struct SpatialNode {
    GameObject *obj;
    struct SpatialNode *left, *right;
    int height;
} SpatialNode;
```

---

## 💻 Exerciții de Laborator

### Exercițiul 1: Arbore AVL de Bază

**Obiectiv:** Implementarea completă a unui arbore AVL cu operații de inserare și afișare.

**Cerințe:**
1. Definirea structurii `AVLNode` cu câmpurile: `key`, `left`, `right`, `height`
2. Implementarea funcțiilor auxiliare: `height()`, `balance_factor()`, `max()`
3. Implementarea celor două rotații simple: `rotate_left()`, `rotate_right()`
4. Implementarea funcției `insert()` cu auto-echilibrare
5. Implementarea parcurgerilor: inorder, preorder, postorder
6. Afișarea arborelui cu indicarea factorului de echilibru

**Input:** Secvență de numere întregi din fișier `data/numbers.txt`

**Output așteptat:**
```
Arbore AVL construit cu 10 noduri
Înălțime: 4
Parcurgere inorder: 10 20 25 30 35 40 45 50 55 60
Factor echilibru rădăcină: 0
```

### Exercițiul 2: Arbore AVL Complet cu Ștergere

**Obiectiv:** Extinderea implementării cu operația de ștergere și funcționalități avansate.

**Cerințe:**
1. Implementarea funcției `delete_node()` cu reechilibrare
2. Găsirea succesorului inorder pentru ștergerea nodurilor cu doi copii
3. Implementarea funcției `search()` pentru căutare
4. Calcularea și afișarea statisticilor arborelui
5. Validarea proprietății AVL după fiecare operație
6. Meniu interactiv pentru operații CRUD

**Input:** Comenzi din fișier sau input interactiv

**Output așteptat:**
```
=== Meniu AVL Tree ===
1. Inserare
2. Ștergere
3. Căutare
4. Afișare
5. Statistici
6. Validare AVL
0. Ieșire
```

---

## 🔧 Compilare și Execuție

```bash
# Compilare toate targeturile
make

# Rulare exemplu demonstrativ
make run

# Compilare individuală
make example1
make exercise1
make exercise2

# Rulare cu Valgrind pentru verificare memorie
make valgrind

# Rulare teste automate
make test

# Curățare fișiere generate
make clean

# Afișare ajutor
make help
```

---

## 📁 Structura Directorului

```
week-09-avl-trees/
├── README.md                           # Acest fișier
├── Makefile                            # Automatizare build
│
├── slides/
│   ├── presentation-week09.html        # Prezentare principală (35+ slide-uri)
│   └── presentation-comparativ.html    # Comparație Pseudocod/C/Python
│
├── src/
│   ├── example1.c                      # Exemplu complet demonstrativ
│   ├── exercise1.c                     # Exercițiu: AVL de bază
│   └── exercise2.c                     # Exercițiu: AVL complet
│
├── data/
│   ├── numbers.txt                     # Date de test pentru inserare
│   └── operations.txt                  # Secvență de operații
│
├── tests/
│   ├── test1_input.txt                 # Input test exercițiu 1
│   ├── test1_expected.txt              # Output așteptat exercițiu 1
│   ├── test2_input.txt                 # Input test exercițiu 2
│   └── test2_expected.txt              # Output așteptat exercițiu 2
│
├── teme/
│   ├── homework-requirements.md        # Cerințe temă (2 x 50p)
│   └── homework-extended.md            # Provocări bonus (5 x 10p)
│
└── solution/
    ├── exercise1_sol.c                 # Soluție exercițiu 1
    ├── exercise2_sol.c                 # Soluție exercițiu 2
    ├── homework1_sol.c                 # Soluție temă 1
    └── homework2_sol.c                 # Soluție temă 2
```

---

## 📖 Lectură Recomandată

### Esențial
- **Cormen, Leiserson, Rivest, Stein** - *Introduction to Algorithms*, Capitolul 13 (Red-Black Trees include comparații cu AVL)
- **Sedgewick, Wayne** - *Algorithms*, Secțiunea 3.3 (Balanced Search Trees)
- **Knuth** - *The Art of Computer Programming*, Vol. 3, Secțiunea 6.2.3

### Avansat
- **Adelson-Velsky, Landis** - *An algorithm for the organization of information* (1962) - lucrarea originală
- **Pfaff** - *Performance Analysis of BSTs in System Software* - analiză comparativă modernă

### Resurse Online
- [Visualgo - AVL Tree Visualization](https://visualgo.net/en/bst) - animații interactive
- [GeeksforGeeks - AVL Tree](https://www.geeksforgeeks.org/avl-tree-set-1-insertion/) - tutoriale pas cu pas
- [MIT OpenCourseWare 6.006](https://ocw.mit.edu/courses/electrical-engineering-and-computer-science/6-006-introduction-to-algorithms-fall-2011/) - prelegeri video

---

## ✅ Lista de Auto-evaluare

După parcurgerea acestui material, verifică dacă poți:

- [ ] Defini formal proprietatea AVL și balance factor
- [ ] Calcula manual înălțimea și factorul de echilibru pentru orice nod
- [ ] Identifica tipul de dezechilibru (LL, RR, LR, RL) dintr-o diagramă
- [ ] Executa manual cele patru tipuri de rotații
- [ ] Implementa în C funcția de inserare cu auto-echilibrare
- [ ] Implementa în C funcția de ștergere cu auto-echilibrare
- [ ] Demonstra că înălțimea unui arbore AVL este O(log n)
- [ ] Compara avantajele/dezavantajele AVL vs Red-Black vs BST
- [ ] Alege structura de date potrivită pentru un scenariu dat
- [ ] Depana probleme de memorie în implementări cu arbori

---

## 💼 Pregătire pentru Interviu

**Întrebări frecvente la interviuri tehnice:**

1. **Ce este un arbore AVL și de ce este util?**
   - Răspuns așteptat: BST auto-echilibrat, garantează O(log n) pentru toate operațiile

2. **Care este diferența dintre AVL și Red-Black trees?**
   - AVL: mai strict echilibrat, căutări mai rapide
   - Red-Black: inserări/ștergeri mai rapide, echilibru mai relaxat

3. **Când ai alege AVL în loc de hash table?**
   - Când ai nevoie de ordine (range queries, min/max)
   - Când cheile nu au funcție hash bună

4. **Implementează funcția de rotație la dreapta.**
   - Demonstrează înțelegerea manipulării pointerilor

5. **Care este complexitatea spațială a unui arbore AVL?**
   - O(n) pentru noduri + overhead pentru pointeri și înălțime

---

## 🔗 Previzualizare Săptămâna Următoare

**Săptămâna 10: Heap-uri și Priority Queues**

În săptămâna următoare vom explora heap-urile binare, o structură de date fundamentală pentru implementarea cozilor cu prioritate. Vom studia:
- Proprietatea heap (max-heap și min-heap)
- Operațiile heapify-up și heapify-down
- Construirea eficientă a unui heap în O(n)
- Algoritmul Heap Sort
- Aplicații: Dijkstra, selecția elementului k

Heap-urile oferă o alternativă interesantă la arborii AVL pentru anumite cazuri de utilizare, în special când avem nevoie doar de acces rapid la elementul minim/maxim.
