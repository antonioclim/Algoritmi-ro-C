# Săptămâna 8: Arbori Binari de Căutare (Binary Search Trees)

## 🎯 Obiective de Învățare

La finalul acestei săptămâni, veți fi capabili să:

1. **Rețineți** (Remember): Definiți proprietatea fundamentală a unui BST și enumerați operațiile de bază (search, insert, delete)
2. **Înțelegeți** (Understand): Explicați cum proprietatea de ordonare determină eficiența căutării și de ce complexitatea medie este O(log n)
3. **Aplicați** (Apply): Implementați algoritmii de inserare, căutare și ștergere în C, gestionând corect pointerii și memoria
4. **Analizați** (Analyse): Comparați performanța BST cu liste înlănțuite și array-uri sortate, identificând scenariile optime pentru fiecare
5. **Evaluați** (Evaluate): Determinați când un BST devine dezechilibrat și estimați impactul asupra performanței
6. **Creați** (Create): Proiectați o aplicație completă (dicționar, index) folosind BST cu operații avansate precum găsirea succesorului/predecesorului

---

## 📜 Context Istoric

### Originile Arborilor Binari de Căutare

Conceptul de **Binary Search Tree** își are rădăcinile în dezvoltarea algoritmilor de căutare din anii 1960. Deși ideea de căutare binară în array-uri era cunoscută încă din 1946 (datorită lui John Mauchly), structura dinamică a BST a apărut ca răspuns la necesitatea de a menține date sortate cu inserări și ștergeri eficiente.

Primele implementări formale au fost documentate de **Andrew Donald Booth** și **Kathleen Booth** în 1959 la Birkbeck College, Londra. Aceștia au studiat structuri arborescente pentru indexarea informațiilor în sistemele de recuperare a datelor (information retrieval).

Analiza matematică riguroasă a BST a fost realizată de **Thomas N. Hibbard** în 1962, care a demonstrat că algoritmul de ștergere standard (cu înlocuirea prin successor inorder) menține proprietățile arborelui. Lucrarea sa "Some Combinatorial Properties of Certain Trees With Applications to Searching and Sorting" a pus bazele teoretice pentru înțelegerea performanței medii.

### Evoluția către Arbori Echilibrați

Problema dezechilibrului BST a fost recunoscută rapid. În 1962, **Georgy Adelson-Velsky** și **Evgenii Landis** (URSS) au inventat arborii AVL - prima structură de date auto-echilibrată. Aceasta a deschis drumul pentru red-black trees (1972, Rudolf Bayer), B-trees și alte variante moderne.

---

### Figură Cheie: Thomas N. Hibbard (1932-2017)

```
    ┌─────────────────────────────────────────────┐
    │  Thomas N. Hibbard                          │
    │  Matematician și Informatician American     │
    │                                             │
    │  • PhD în Matematică, Princeton (1957)      │
    │  • Profesor la UCLA, apoi UC San Diego     │
    │  • Contribuții fundamentale la BST          │
    │  • Pionier în analiza probabilistică        │
    └─────────────────────────────────────────────┘
```

**Contribuție Majoră**: Algoritmul Hibbard pentru ștergere în BST, care menține structura validă a arborelui prin înlocuirea nodului cu succesorul său inorder.

> *"The deletion algorithm presented here has the property that it preserves  
> the binary search tree property regardless of which node is deleted."*  
> — T.N. Hibbard, Communications of the ACM, 1962

---

## 📚 Fundamente Teoretice

### 1. Definiția și Proprietatea BST

Un **Binary Search Tree** este un arbore binar în care fiecare nod satisface **proprietatea BST**:

```
        Pentru orice nod N cu valoarea K:
        
        ┌─────────────────────────────────────────────────┐
        │   Toate valorile din     <  K  <    Toate       │
        │   subarborele STÂNG              valorile din   │
        │                                  subarborele    │
        │                                  DREPT          │
        └─────────────────────────────────────────────────┘
```

**Exemplu Vizual**:

```
              50
            /    \
          30      70
         /  \    /  \
       20   40  60   80
      /              / \
    10             75   90
    
    Parcurgere Inorder: 10, 20, 30, 40, 50, 60, 70, 75, 80, 90
    (Rezultat SORTAT - proprietate fundamentală!)
```

**Structura Nodului în C**:

```c
typedef struct BSTNode {
    int key;                    /* Cheia de căutare */
    void *data;                 /* Date asociate (generic) */
    struct BSTNode *left;       /* Subarbore stâng (valori < key) */
    struct BSTNode *right;      /* Subarbore drept (valori > key) */
} BSTNode;

typedef struct {
    BSTNode *root;              /* Rădăcina arborelui */
    size_t size;                /* Numărul de noduri */
    int (*compare)(const void*, const void*);  /* Funcția de comparare */
} BST;
```

### 2. Operația de Căutare (Search)

Algoritmul de căutare exploatează proprietatea BST pentru a elimina jumătate din candidați la fiecare pas:

```
    SEARCH(root, key):
    ┌─────────────────────────────────────────┐
    │  current = root                         │
    │  WHILE current ≠ NULL:                  │
    │      IF key == current.key:             │
    │          RETURN current     ← GĂSIT!    │
    │      ELSE IF key < current.key:         │
    │          current = current.left  ←      │
    │      ELSE:                              │
    │          current = current.right →      │
    │  RETURN NULL               ← NEGĂSIT    │
    └─────────────────────────────────────────┘
```

**Implementare Iterativă** (preferată pentru eficiență):

```c
BSTNode* bst_search(BST *tree, int key) {
    BSTNode *current = tree->root;
    
    while (current != NULL) {
        if (key == current->key) {
            return current;         /* Găsit */
        } else if (key < current->key) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    
    return NULL;  /* Negăsit */
}
```

**Implementare Recursivă** (mai elegantă, dar folosește stack):

```c
BSTNode* bst_search_recursive(BSTNode *node, int key) {
    if (node == NULL || node->key == key) {
        return node;
    }
    
    if (key < node->key) {
        return bst_search_recursive(node->left, key);
    }
    return bst_search_recursive(node->right, key);
}
```

### 3. Operația de Inserare (Insert)

Inserarea menține proprietatea BST și adaugă nodul ca frunză:

```
    INSERT(root, key):
    ┌─────────────────────────────────────────┐
    │  IF root == NULL:                       │
    │      RETURN new_node(key)               │
    │                                         │
    │  IF key < root.key:                     │
    │      root.left = INSERT(root.left, key) │
    │  ELSE IF key > root.key:                │
    │      root.right = INSERT(root.right,key)│
    │  /* key == root.key: duplicat ignorat   │
    │                                         │
    │  RETURN root                            │
    └─────────────────────────────────────────┘
```

**Vizualizare Inserare**:

```
    Inserare 35 în arborele:
    
          50                    50
        /    \                /    \
      30      70    →       30      70
     /  \                  /  \
   20   40               20   40
                            /
                          35  ← NOU
```

### 4. Operația de Ștergere (Delete) - Algoritmul Hibbard

Ștergerea este cea mai complexă operație, cu trei cazuri:

```
    ┌─────────────────────────────────────────────────────────────┐
    │  CAZUL 1: Nod FRUNZĂ (fără copii)                          │
    │  → Simplu: eliberăm nodul, setăm pointerul părintelui NULL │
    │                                                             │
    │        50              50                                   │
    │       /  \            /  \                                  │
    │     30    70   →    30    70      (ștergem 20)              │
    │    /                                                        │
    │  [20]                                                       │
    └─────────────────────────────────────────────────────────────┘
    
    ┌─────────────────────────────────────────────────────────────┐
    │  CAZUL 2: Nod cu UN SINGUR copil                            │
    │  → Înlocuim nodul cu copilul său                            │
    │                                                             │
    │        50              50                                   │
    │       /  \            /  \                                  │
    │    [30]   70   →    20    70      (ștergem 30)              │
    │    /                                                        │
    │  20                                                         │
    └─────────────────────────────────────────────────────────────┘
    
    ┌─────────────────────────────────────────────────────────────┐
    │  CAZUL 3: Nod cu DOI copii - Algoritmul Hibbard             │
    │  → Găsim SUCCESORUL INORDER (cel mai mic din dreapta)       │
    │  → Copiem valoarea succesorului în nodul de șters           │
    │  → Ștergem succesorul (care are max 1 copil)                │
    │                                                             │
    │        [50]             60                                  │
    │       /    \           /  \                                 │
    │     30      70   →   30    70     (ștergem 50)              │
    │    /  \    /        /  \    \     (succesor = 60)           │
    │  20   40 60       20   40   80                              │
    │            \                                                │
    │            80                                               │
    └─────────────────────────────────────────────────────────────┘
```

### 5. Analiza Complexității

| Operație | Caz Mediu | Caz Defavorabil | Spațiu |
|----------|-----------|-----------------|--------|
| Search   | O(log n)  | O(n)*           | O(1)   |
| Insert   | O(log n)  | O(n)*           | O(1)   |
| Delete   | O(log n)  | O(n)*           | O(1)   |
| Min/Max  | O(log n)  | O(n)*           | O(1)   |
| Inorder  | O(n)      | O(n)            | O(h)** |

*\* Cazul defavorabil apare când arborele degenerează într-o listă*  
*\*\* h = înălțimea arborelui*

**Dezechilibrul - Problema Fundamentală**:

```
    Inserare secvență sortată: 10, 20, 30, 40, 50
    
    Arbore echilibrat       vs    Arbore degenerat (listă)
    (inserare aleatorie)          (inserare sortată)
    
          30                        10
         /  \                        \
       20    40                      20
      /        \                      \
    10          50                    30
                                       \
    h = 2, O(log n)                   40
                                        \
                                        50
                                        
                                    h = 4, O(n)
```

---

## 🏭 Aplicații Industriale

### 1. Implementarea std::map în C++ (GCC libstdc++)

Standard Template Library folosește Red-Black Trees (un tip de BST auto-echilibrat):

```cpp
// Intern în libstdc++, simplificat
template<typename Key, typename Value>
class map {
private:
    struct RBNode {
        Key key;
        Value value;
        RBNode *left, *right, *parent;
        enum { RED, BLACK } color;
    };
    
    RBNode *root;
    
public:
    Value& operator[](const Key& key) {
        RBNode *node = find(key);
        if (!node) {
            node = insert(key, Value());
            rebalance(node);  // Menține proprietatea RB
        }
        return node->value;
    }
};
```

### 2. Indexare în Baze de Date (SQLite B-Tree)

SQLite folosește B-Trees (generalizare a BST pentru disk I/O):

```c
/* Simplificare din sqlite3.c */
typedef struct BtCursor {
    Btree *pBtree;
    BtShared *pBt;
    MemPage *pPage;     /* Pagina curentă */
    int idx;            /* Index în pagină */
} BtCursor;

int sqlite3BtreeMoveto(BtCursor *pCur, const void *pKey, 
                       i64 nKey, int bias, int *pRes) {
    /* Navigare BST-like prin paginile B-tree */
    while (pCur->pPage->leaf == 0) {
        int cmp = compare(pKey, pCur->pPage->cells[pCur->idx]);
        if (cmp < 0) {
            moveToLeftChild(pCur);
        } else {
            moveToRightChild(pCur);
        }
    }
    /* ... */
}
```

### 3. Sistemul de Fișiere Linux (ext4 extent tree)

Kernel-ul Linux folosește arbori pentru gestionarea extent-urilor:

```c
/* Din fs/ext4/extents.c - simplificat */
struct ext4_extent_header {
    __le16 eh_magic;
    __le16 eh_entries;
    __le16 eh_max;
    __le16 eh_depth;    /* 0 = frunze, >0 = noduri interne */
};

static struct ext4_extent *
ext4_ext_binsearch(struct ext4_extent_header *eh, ext4_lblk_t block) {
    struct ext4_extent *l, *r, *m;
    
    l = EXT_FIRST_EXTENT(eh) + 1;
    r = EXT_LAST_EXTENT(eh);
    
    /* Căutare binară în nodurile sortate */
    while (l <= r) {
        m = l + (r - l) / 2;
        if (block < le32_to_cpu(m->ee_block))
            r = m - 1;
        else
            l = m + 1;
    }
    return l - 1;
}
```

### 4. Autocompletare în IDE-uri (Trie + BST hibrid)

Visual Studio Code și alte IDE-uri moderne:

```typescript
// Simplificare conceptuală
interface SymbolNode {
    name: string;
    kind: SymbolKind;
    left: SymbolNode | null;
    right: SymbolNode | null;
}

class SymbolIndex {
    private root: SymbolNode | null = null;
    
    search(prefix: string): Symbol[] {
        const results: Symbol[] = [];
        this.searchSubtree(this.root, prefix, results);
        return results.slice(0, 10);  // Top 10 sugestii
    }
    
    private searchSubtree(node: SymbolNode | null, 
                          prefix: string, 
                          results: Symbol[]): void {
        if (!node) return;
        
        const cmp = prefix.localeCompare(node.name.slice(0, prefix.length));
        
        if (cmp <= 0) this.searchSubtree(node.left, prefix, results);
        if (node.name.startsWith(prefix)) results.push(node);
        if (cmp >= 0) this.searchSubtree(node.right, prefix, results);
    }
}
```

---

## 💻 Exerciții de Laborator

### Exercițiul 1: Dicționar cu BST

**Obiectiv**: Implementați un dicționar român-englez folosind BST.

**Cerințe**:
1. Definiți structura pentru o intrare de dicționar (cuvânt RO, traducere EN)
2. Implementați funcția de inserare care menține ordinea alfabetică
3. Implementați căutarea cu feedback pentru cuvinte negăsite
4. Implementați ștergerea unei intrări
5. Salvați/încărcați dicționarul din fișier text
6. Afișați statistici: număr cuvinte, înălțimea arborelui

**Fișiere**: `src/exercise1.c`, `data/dictionar.txt`

### Exercițiul 2: Arbore de Expresii Aritmetice

**Obiectiv**: Construiți și evaluați expresii aritmetice folosind arbori binari.

**Cerințe**:
1. Parseați expresii în notație infixă (cu paranteze)
2. Construiți arborele de expresie
3. Evaluați expresia prin parcurgere postorder
4. Afișați expresia în notații: infix, prefix, postfix
5. Simplificați expresii (ex: `x * 1 = x`, `x + 0 = x`)
6. Derivați expresii simple (bonus)

**Fișiere**: `src/exercise2.c`

---

## 🔧 Compilare și Execuție

```bash
# Construiește toate targeturile
make all

# Rulează exemplul demonstrativ
make run

# Compilează și rulează exercițiul 1
make exercise1
./exercise1

# Rulează testele automate
make test

# Verificare memory leaks
make valgrind

# Curățare
make clean

# Ajutor
make help
```

---

## 📁 Structura Directorului

```
week-08-binary-search-trees/
├── README.md                           # Acest document
├── Makefile                            # Automatizare build
│
├── slides/
│   ├── presentation-week08.html        # Prezentare principală (35+ slides)
│   └── presentation-comparativ.html    # Comparație Pseudocod/C/Python
│
├── src/
│   ├── example1.c                      # Exemplu complet BST (~450 linii)
│   ├── exercise1.c                     # Exercițiu: Dicționar
│   └── exercise2.c                     # Exercițiu: Arbore expresii
│
├── data/
│   ├── dictionar.txt                   # Date pentru exercițiul 1
│   └── expresii.txt                    # Expresii pentru exercițiul 2
│
├── tests/
│   ├── test1_input.txt                 # Input test exercițiu 1
│   ├── test1_expected.txt              # Output așteptat
│   ├── test2_input.txt                 # Input test exercițiu 2
│   └── test2_expected.txt              # Output așteptat
│
├── teme/
│   ├── homework-requirements.md        # Cerințe temă (100p)
│   └── homework-extended.md            # Provocări bonus (+50p)
│
└── solution/
    ├── exercise1_sol.c                 # Soluție exercițiu 1
    ├── exercise2_sol.c                 # Soluție exercițiu 2
    ├── homework1_sol.c                 # Soluție temă 1
    └── homework2_sol.c                 # Soluție temă 2
```

---

## 📖 Bibliografie Recomandată

### Esențiale
- **Cormen, Leiserson, Rivest, Stein** - *Introduction to Algorithms*, Cap. 12 "Binary Search Trees"
- **Sedgewick, Wayne** - *Algorithms*, 4th Ed., Secțiunea 3.2
- **Knuth** - *The Art of Computer Programming*, Vol. 3, Secțiunea 6.2.2

### Avansate
- **Adelson-Velsky, Landis** - *An algorithm for the organization of information* (1962) - Paper-ul original AVL
- **Hibbard** - *Some Combinatorial Properties of Certain Trees* (1962) - Analiza BST
- **Tarjan** - *Data Structures and Network Algorithms* - Red-Black Trees

### Resurse Online
- [Visualgo - BST Visualization](https://visualgo.net/en/bst) - Animații interactive
- [GeeksforGeeks - BST](https://www.geeksforgeeks.org/binary-search-tree-data-structure/) - Tutoriale și probleme
- [MIT OpenCourseWare 6.006](https://ocw.mit.edu/courses/6-006-introduction-to-algorithms-spring-2020/) - Lecții video

---

## ✅ Lista de Auto-Verificare

Înainte de a considera această săptămână completă, verificați că:

- [ ] Pot defini proprietatea BST și pot verifica dacă un arbore dat este BST valid
- [ ] Pot implementa căutarea atât iterativ cât și recursiv
- [ ] Pot implementa inserarea menținând proprietatea BST
- [ ] Pot implementa ștergerea corect pentru toate cele 3 cazuri
- [ ] Înțeleg diferența între complexitatea medie O(log n) și cea defavorabilă O(n)
- [ ] Pot găsi minimul, maximul, succesorul și predecesorul unui nod
- [ ] Pot implementa toate cele 3 parcurgeri (inorder, preorder, postorder)
- [ ] Pot calcula înălțimea și pot verifica echilibrul unui arbore
- [ ] Știu când să aleg BST vs. alte structuri de date
- [ ] Pot elibera corect toată memoria unui BST fără memory leaks

---

## 💼 Pregătire pentru Interviu

### Întrebări Frecvente

1. **Care este diferența dintre un Binary Tree și un Binary Search Tree?**
   - Binary Tree: orice arbore cu max 2 copii per nod
   - BST: adaugă proprietatea de ordonare (stânga < rădăcină < dreapta)

2. **Cum verifici dacă un arbore binar este BST valid?**
   - Parcurgere inorder trebuie să producă secvență sortată crescător
   - SAU: verificare recursivă cu limite min/max pentru fiecare nod

3. **Care este complexitatea căutării într-un BST? De ce?**
   - Medie: O(log n) - înjumătățim candidații la fiecare pas
   - Defavorabil: O(n) - arbore degenerat în listă (inserare sortată)

4. **Cum funcționează ștergerea unui nod cu doi copii?**
   - Găsim succesorul inorder (cel mai mic din subarborele drept)
   - Copiem valoarea succesorului în nodul de șters
   - Ștergem succesorul (care are maxim un copil)

5. **Când ai alege BST în loc de hash table?**
   - Când ai nevoie de date ordonate (range queries)
   - Când ai nevoie de min/max eficient
   - Când memoria este limitată (BST nu alocă în avans)
   - Când cheile nu au o funcție de hash bună

---

## 🔗 Preview Săptămâna Următoare

**Săptămâna 9: Arbori AVL și Auto-Echilibrare**

Vom studia prima structură de date auto-echilibrată din istorie:
- Factorul de echilibru și proprietatea AVL
- Rotații: single (LL, RR) și duble (LR, RL)
- Menținerea echilibrului la inserare și ștergere
- Comparație cu Red-Black Trees

Pregătiți-vă revizuind algoritmul de inserare din această săptămână și gândindu-vă cum s-ar putea "corecta" un arbore dezechilibrat.

---

*Acest material face parte din cursul "Algoritmi și Tehnici de Programare" (ATP)*  
*Academia de Studii Economice din București - CSIE*  
*Actualizat: Ianuarie 2026*
