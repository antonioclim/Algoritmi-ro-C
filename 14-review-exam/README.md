# Săptămâna 14: Recapitulare Avansată și Optimizare

## 🎯 Obiective de Învățare

La finalul acestei săptămâni, studentul va fi capabil să:

1. **Să recunoască** (Remember) - Să identifice structura de date potrivită pentru un anumit tip de problemă și să enumere complexitățile temporale asociate operațiilor fundamentale
2. **Să înțeleagă** (Understand) - Să explice relațiile și compromisurile între diferitele structuri de date (arrays vs linked lists, trees vs hash tables, graphs vs trees)
3. **Să aplice** (Apply) - Să implementeze soluții hibride care combină multiple structuri de date pentru rezolvarea problemelor complexe
4. **Să analizeze** (Analyse) - Să evalueze performanța algoritmilor prin profiling și să identifice bottleneck-uri în cod
5. **Să evalueze** (Evaluate) - Să compare diferite abordări algoritmice și să justifice alegerea optimă pe baza cerințelor specifice
6. **Să creeze** (Create) - Să proiecteze și implementeze sisteme software complete care integrează toate conceptele studiate în semestru

---

## 📜 Context Istoric

### Evoluția Structurilor de Date și Algoritmilor

Istoria structurilor de date moderne începe în anii 1950-1960, odată cu dezvoltarea primelor limbaje de programare de nivel înalt. **FORTRAN** (1957) a introdus conceptul de array-uri, în timp ce **LISP** (1958) a popularizat listele înlănțuite și procesarea simbolică.

Anii 1960 au adus contribuții fundamentale: **Donald Knuth** a început lucrul la monumentala serie "The Art of Computer Programming" (1968), stabilind bazele analizei riguroase a algoritmilor. **C.A.R. Hoare** a inventat algoritmul Quicksort (1960) și a dezvoltat logica Hoare pentru verificarea programelor. **Edsger Dijkstra** a publicat algoritmul său pentru drumuri minime (1959), revoluționând teoria grafurilor.

Decada 1970 a văzut nașterea structurilor de date auto-echilibrate: **Adelson-Velsky și Landis** au inventat arborii AVL (1962), iar **Rudolf Bayer** a creat B-trees (1972). **Robert Tarjan** a adus contribuții majore în analiza amortizată și structuri de date avansate.

### Figura Cheie: Donald Ervin Knuth (1938 - prezent)

```
╔══════════════════════════════════════════════════════════════════════════════╗
║  "Premature optimization is the root of all evil."                          ║
║                                          — Donald Knuth, 1974               ║
╚══════════════════════════════════════════════════════════════════════════════╝
```

Donald Knuth, profesor emerit la Stanford University, este considerat „părintele analizei algoritmilor". Opera sa capitală, "The Art of Computer Programming", rămâne referința definitivă în domeniu. Knuth a creat sistemul TeX pentru tehnoredactare (1978) și limbajul de programare literată WEB. A primit premiul Turing în 1974 pentru contribuțiile sale fundamentale la analiza algoritmilor și la proiectarea limbajelor de programare.

---

## 📚 Fundamente Teoretice

### 1. Taxonomia Structurilor de Date

```
                    STRUCTURI DE DATE
                          │
          ┌───────────────┼───────────────┐
          │               │               │
      LINIARE        IERARHICE       NON-LINIARE
          │               │               │
    ┌─────┼─────┐    ┌────┼────┐    ┌─────┼─────┐
    │     │     │    │    │    │    │     │     │
  Array  List Stack Tree  Heap Hash Graph  Set
              Queue      Trie Table
```

#### Clasificare după Organizare:

| Tip | Structuri | Acces | Inserare | Ștergere |
|-----|-----------|-------|----------|----------|
| **Secvențiale** | Array, Vector | O(1) | O(n) | O(n) |
| **Înlănțuite** | Linked List | O(n) | O(1)* | O(1)* |
| **Arborescente** | BST, AVL, Heap | O(log n) | O(log n) | O(log n) |
| **Bazate pe Hash** | Hash Table | O(1)** | O(1)** | O(1)** |
| **Graf** | Adj. Matrix/List | O(1)/O(V) | O(1)/O(1) | O(1)/O(E) |

*\* Cu referință la poziție; \*\* Caz mediu*

### 2. Matricea de Selecție a Structurilor

```c
/*
 * GHID DE SELECȚIE A STRUCTURILOR DE DATE
 * =======================================
 *
 * Întrebare 1: Aveți nevoie de acces aleator rapid?
 *   DA → Array sau Hash Table
 *   NU → Continuați
 *
 * Întrebare 2: Datele trebuie sortate?
 *   DA → BST, AVL, sau Array sortat
 *   NU → Continuați
 *
 * Întrebare 3: Inserări/ștergeri frecvente?
 *   DA → Linked List sau Tree
 *   NU → Array
 *
 * Întrebare 4: Relații ierarhice?
 *   DA → Tree
 *   NU → Continuați
 *
 * Întrebare 5: Relații complexe între entități?
 *   DA → Graph
 *   NU → Hash Table pentru lookup O(1)
 */
```

### 3. Analiza Comparativă a Complexităților

```
╔═══════════════════════════════════════════════════════════════════════════════╗
║                    COMPLEXITĂȚI TEMPORALE - SUMAR COMPLET                     ║
╠═══════════════════╦═══════════╦═══════════╦═══════════╦═══════════╦══════════╣
║ Structură         ║  Acces    ║  Căutare  ║  Inserare ║  Ștergere ║  Spațiu  ║
╠═══════════════════╬═══════════╬═══════════╬═══════════╬═══════════╬══════════╣
║ Array             ║   O(1)    ║   O(n)    ║   O(n)    ║   O(n)    ║   O(n)   ║
║ Array Sortat      ║   O(1)    ║ O(log n)  ║   O(n)    ║   O(n)    ║   O(n)   ║
║ Linked List       ║   O(n)    ║   O(n)    ║   O(1)    ║   O(1)    ║   O(n)   ║
║ Doubly Linked     ║   O(n)    ║   O(n)    ║   O(1)    ║   O(1)    ║   O(n)   ║
║ Stack             ║   O(n)    ║   O(n)    ║   O(1)    ║   O(1)    ║   O(n)   ║
║ Queue             ║   O(n)    ║   O(n)    ║   O(1)    ║   O(1)    ║   O(n)   ║
║ BST (mediu)       ║ O(log n)  ║ O(log n)  ║ O(log n)  ║ O(log n)  ║   O(n)   ║
║ BST (worst)       ║   O(n)    ║   O(n)    ║   O(n)    ║   O(n)    ║   O(n)   ║
║ AVL Tree          ║ O(log n)  ║ O(log n)  ║ O(log n)  ║ O(log n)  ║   O(n)   ║
║ Binary Heap       ║   O(1)*   ║   O(n)    ║ O(log n)  ║ O(log n)  ║   O(n)   ║
║ Hash Table (med)  ║   N/A     ║   O(1)    ║   O(1)    ║   O(1)    ║   O(n)   ║
║ Hash Table (wrst) ║   N/A     ║   O(n)    ║   O(n)    ║   O(n)    ║   O(n)   ║
║ Graf (Matrice)    ║   O(1)    ║  O(V²)    ║   O(1)    ║   O(1)    ║  O(V²)   ║
║ Graf (Listă)      ║  O(V+E)   ║  O(V+E)   ║   O(1)    ║  O(E)     ║  O(V+E)  ║
╚═══════════════════╩═══════════╩═══════════╩═══════════╩═══════════╩══════════╝
  * Pentru heap: accesul la min/max este O(1)
```

### 4. Tehnici de Optimizare

#### 4.1 Optimizare la Nivel de Algoritm

```c
/*
 * PRINCIPII DE OPTIMIZARE ALGORITMICĂ
 * ===================================
 *
 * 1. REDUCE COMPLEXITY CLASS
 *    - O(n²) → O(n log n): folosește divide-and-conquer
 *    - O(n) → O(log n): folosește căutare binară
 *    - O(log n) → O(1): folosește hash tables
 *
 * 2. AMORTIZED ANALYSIS
 *    - Operații costisitoare rare, compensate de multe operații ieftine
 *    - Exemplu: dynamic array doubling (inserare amortizat O(1))
 *
 * 3. SPACE-TIME TRADEOFFS
 *    - Memoization: schimbă memorie pentru timp
 *    - Compression: schimbă timp pentru memorie
 *
 * 4. LAZY EVALUATION
 *    - Amână calculele până când rezultatul este necesar
 *    - Exemplu: lazy propagation în segment trees
 */
```

#### 4.2 Optimizare la Nivel de Implementare

```c
/* Cache-friendly code */
// GREȘIT: acces pe coloane (cache misses)
for (int j = 0; j < N; j++)
    for (int i = 0; i < N; i++)
        sum += matrix[i][j];

// CORECT: acces pe linii (cache hits)
for (int i = 0; i < N; i++)
    for (int j = 0; j < N; j++)
        sum += matrix[i][j];

/* Branch prediction friendly */
// Sortează datele când e posibil înainte de procesare condițională
```

---

## 🏭 Aplicații Industriale

### 1. Sisteme de Baze de Date

```c
/*
 * INDEX B-TREE (simplificat)
 * Folosit în MySQL, PostgreSQL, SQLite
 */
typedef struct BTreeNode {
    int *keys;              // Array de chei
    struct BTreeNode **children;  // Array de pointeri la copii
    int n;                  // Număr curent de chei
    bool leaf;              // Este nod frunză?
} BTreeNode;

/* Operații: O(log n) pentru insert, delete, search */
```

### 2. Networking și Routing (Linux Kernel)

```c
/*
 * STRUCTURA SOCKET (simplificată)
 * Din kernel Linux
 */
struct socket_list {
    struct socket *sock;
    struct socket_list *next;
    uint32_t hash;          // Pentru lookup rapid
};

/* Hash table pentru file descriptors */
struct socket *fd_lookup(int fd) {
    uint32_t hash = hash_fd(fd);
    struct socket_list *entry = socket_table[hash % TABLE_SIZE];
    while (entry) {
        if (entry->sock->fd == fd)
            return entry->sock;
        entry = entry->next;
    }
    return NULL;
}
```

### 3. Compilatoare (Symbol Tables)

```c
/*
 * SYMBOL TABLE cu hash + chaining
 * Folosit în GCC, Clang
 */
typedef struct Symbol {
    char *name;
    enum { VAR, FUNC, TYPE } kind;
    struct Type *type;
    int scope_level;
    struct Symbol *next;    // Chaining pentru coliziuni
} Symbol;

typedef struct SymbolTable {
    Symbol **buckets;
    size_t size;
    int current_scope;
} SymbolTable;
```

### 4. Sisteme de Fișiere

```c
/*
 * INODE STRUCTURE (ext4 simplificat)
 */
typedef struct Inode {
    uint32_t mode;          // Permisiuni
    uint32_t size;          // Dimensiune fișier
    uint32_t blocks[15];    // Direct + indirect blocks
    // blocks[0-11]: direct
    // blocks[12]: single indirect
    // blocks[13]: double indirect
    // blocks[14]: triple indirect
} Inode;

/* Tree structure pentru directoare */
```

### 5. Game Development (Spatial Partitioning)

```c
/*
 * QUADTREE pentru collision detection
 */
typedef struct QuadTree {
    Rectangle bounds;
    GameObject *objects[MAX_OBJECTS];
    int object_count;
    struct QuadTree *children[4];  // NW, NE, SW, SE
} QuadTree;

void insert(QuadTree *tree, GameObject *obj) {
    if (!intersects(tree->bounds, obj->bounds))
        return;
    
    if (tree->children[0] == NULL) {
        if (tree->object_count < MAX_OBJECTS) {
            tree->objects[tree->object_count++] = obj;
            return;
        }
        subdivide(tree);
    }
    
    for (int i = 0; i < 4; i++)
        insert(tree->children[i], obj);
}
```

---

## 💻 Exerciții de Laborator

### Exercițiu 1: Sistem de Management al Comenzilor

**Obiectiv:** Implementarea unui sistem complet de gestionare a comenzilor care utilizează multiple structuri de date integrate.

**Cerințe:**
1. Definirea structurii `Order` cu câmpuri pentru ID, client, produse, timestamp și status
2. Implementarea unui hash table pentru căutarea rapidă a comenzilor după ID
3. Implementarea unui BST pentru comenzi sortate cronologic
4. Implementarea unei cozi de priorități (heap) pentru procesarea comenzilor urgente
5. Funcții de serializare/deserializare pentru persistența în fișiere binare
6. Funcție de generare raport care traversează toate structurile
7. Funcție de căutare avansată cu multiple criterii

**Input/Output:**
- Citire comenzi din fișier text structurat
- Export rapoarte în format CSV
- Persistență în fișier binar

### Exercițiu 2: Analizor de Rețea Socială

**Obiectiv:** Modelarea și analiza unei rețele sociale folosind structuri de graf.

**Cerințe:**
1. Reprezentarea rețelei ca graf folosind liste de adiacență
2. Implementarea BFS pentru găsirea „gradului de separare" între utilizatori
3. Implementarea DFS pentru detectarea comunităților (componente conexe)
4. Calculul centralității (număr de conexiuni per nod)
5. Algoritmul lui Dijkstra pentru „influența" (drumuri ponderate)
6. Detectarea ciclurilor și identificarea „influencerilor"
7. Exportul vizualizării în format DOT (Graphviz)

**Input/Output:**
- Citire relații din fișier CSV
- Interogări interactive prin meniu
- Export statistici și vizualizări

---

## 🔧 Compilare și Execuție

```bash
# Construire toate targeturile
make

# Construire individuală
make example1
make exercise1
make exercise2

# Execuție exemple
make run

# Rulare teste automate
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
week-14-recapitulare-avansata/
├── README.md                           # Acest fișier
├── Makefile                            # Automatizare build
│
├── slides/
│   ├── presentation-week14.html        # Prezentare principală (40 slide-uri)
│   └── presentation-comparativ.html    # Comparație Pseudocode/C/Python
│
├── src/
│   ├── example1.c                      # Exemplu complet (~500 linii)
│   ├── exercise1.c                     # Exercițiu 1 (12 TODO-uri)
│   └── exercise2.c                     # Exercițiu 2 (15 TODO-uri)
│
├── data/
│   ├── orders.txt                      # Date comenzi pentru exercițiu 1
│   ├── products.txt                    # Catalog produse
│   ├── social_network.csv              # Graf rețea socială
│   └── queries.txt                     # Interogări de test
│
├── tests/
│   ├── test1_input.txt                 # Input test exercițiu 1
│   ├── test1_expected.txt              # Output așteptat exercițiu 1
│   ├── test2_input.txt                 # Input test exercițiu 2
│   └── test2_expected.txt              # Output așteptat exercițiu 2
│
├── teme/
│   ├── homework-requirements.md        # 2 teme (50p fiecare)
│   └── homework-extended.md            # 5 provocări bonus (+10p fiecare)
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

1. **Knuth, D.E.** - "The Art of Computer Programming, Vol. 1-3" (Addison-Wesley)
   - Referința definitivă pentru algoritmi și structuri de date
   
2. **Cormen, T.H. et al.** - "Introduction to Algorithms" (MIT Press)
   - Capitolele de recapitulare pentru fiecare structură studiată

3. **Sedgewick, R.** - "Algorithms in C" (Addison-Wesley)
   - Implementări practice în C

### Avansate

4. **Skiena, S.S.** - "The Algorithm Design Manual" (Springer)
   - Ghid practic pentru selecția algoritmilor

5. **Brass, P.** - "Advanced Data Structures" (Cambridge)
   - Structuri de date specializate

6. **Bentley, J.** - "Programming Pearls" (Addison-Wesley)
   - Tehnici de optimizare și design

### Resurse Online

- **Visualgo.net** - Vizualizări interactive ale algoritmilor
- **Big-O Cheat Sheet** - https://www.bigocheatsheet.com/
- **GeeksforGeeks** - Implementări și explicații detaliate
- **MIT OpenCourseWare 6.006** - Cursuri video gratuite

---

## ✅ Lista de Auto-Evaluare

Verificați că puteți răspunde „DA" la toate întrebările:

- [ ] Pot alege structura de date optimă pentru o problemă dată?
- [ ] Înțeleg compromisurile timp-spațiu pentru fiecare structură?
- [ ] Pot implementa de la zero: linked list, stack, queue, BST, hash table, graph?
- [ ] Pot analiza complexitatea temporală și spațială a unui algoritm?
- [ ] Știu să folosesc Valgrind pentru detectarea memory leaks?
- [ ] Pot optimiza codul pentru cache locality?
- [ ] Înțeleg când să folosesc recursivitate vs iterație?
- [ ] Pot integra multiple structuri de date într-un sistem coerent?
- [ ] Știu să serializez/deserializez structuri complexe în fișiere?
- [ ] Pot implementa algoritmii fundamentali pe grafuri (BFS, DFS, Dijkstra)?

---

## 💼 Pregătire pentru Interviuri

### Întrebări Comune

1. **"Explică diferența dintre ArrayList și LinkedList. Când ai folosi fiecare?"**
   - ArrayList: acces O(1), inserare O(n), memorie contiguă, cache-friendly
   - LinkedList: acces O(n), inserare O(1), overhead per element, inserări frecvente

2. **"Cum ai implementa un LRU Cache?"**
   - Combinație hash table + doubly linked list
   - Hash table pentru lookup O(1)
   - Lista pentru ordinea accesului, mutare la cap la fiecare acces

3. **"Care este diferența dintre BFS și DFS? Dă exemple de utilizare."**
   - BFS: nivel cu nivel, shortest path în grafuri neponderate, cozi
   - DFS: adâncime întâi, detectare cicluri, sortare topologică, stive

4. **"Cum ai proiecta un sistem de autocomplete?"**
   - Trie pentru prefix matching
   - Heap pentru top-K rezultate
   - Cache pentru căutări frecvente

5. **"Explică ce este un hash collision și cum se rezolvă."**
   - Chaining: liste la fiecare bucket
   - Open addressing: linear/quadratic probing, double hashing
   - Trade-offs: memorie vs performanță în worst case

---

## 🔗 Previzualizare Examen Final

Săptămâna 14 marchează sfârșitul semestrului. Examenul final va acoperi:

- **Partea I (40%):** Întrebări teoretice despre complexități și trade-offs
- **Partea II (30%):** Implementare structuri de date de la zero
- **Partea III (30%):** Problemă integrată cu multiple structuri

**Recomandare:** Revedeți toate laboratoarele și rezolvați problemele bonus pentru pregătire optimă.

---

*© 2024-2025 Algoritmi și Tehnici de Programare - ASE București*
*Acest material este destinat exclusiv uzului educațional.*
