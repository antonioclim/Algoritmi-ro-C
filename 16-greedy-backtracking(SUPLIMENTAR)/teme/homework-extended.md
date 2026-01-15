# Provocări Extinse - Săptămâna 16: Greedy și Backtracking

## 🚀 Provocări Avansate (Opționale)

Aceste provocări sunt pentru studenții care doresc să-și testeze limitele și să exploreze aplicații avansate ale paradigmelor Greedy și Backtracking.

Fiecare provocare rezolvată corect: **+10 puncte bonus**

---

## ⭐ Provocarea 1: Compresie Huffman Completă (Dificultate: Medie-Dificilă)

### Descriere

Implementați un sistem complet de compresie/decompresie folosind codificarea Huffman. Algoritmul Huffman este un algoritm **Greedy** clasic care construiește coduri prefix-free optime pentru compresie.

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                        CODIFICAREA HUFFMAN                                  │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   Text: "ABRACADABRA"                                                       │
│                                                                             │
│   Frecvențe:     A=5, B=2, R=2, C=1, D=1                                   │
│                                                                             │
│   Arbore Huffman:            (11)                                          │
│                             /    \                                          │
│                          A(5)    (6)                                        │
│                                 /    \                                      │
│                              (2)     (4)                                    │
│                             /   \   /   \                                   │
│                           C(1) D(1) B(2) R(2)                              │
│                                                                             │
│   Coduri:  A=0, C=100, D=101, B=110, R=111                                │
│                                                                             │
│   Comprimat: 0 110 111 0 100 0 101 0 110 111 0                            │
│              (23 biți vs 88 biți original = 74% compresie)                 │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### Cerințe

```c
/* Structuri necesare */
typedef struct HuffmanNode {
    char character;
    unsigned frequency;
    struct HuffmanNode *left, *right;
} HuffmanNode;

typedef struct {
    char character;
    char code[256];      /* Codul binar ca string */
    int code_length;
} HuffmanCode;

/* Funcții de implementat */
HuffmanNode* build_huffman_tree(char *text);
void generate_codes(HuffmanNode *root, HuffmanCode *codes, int *count);
char* compress(const char *text, HuffmanCode *codes, int code_count);
char* decompress(const char *compressed, HuffmanNode *root);
void free_huffman_tree(HuffmanNode *root);
```

### Exemplu

```
$ ./huffman compress input.txt output.huf
Original:    1024 bytes
Compressed:  412 bytes
Ratio:       59.8%

$ ./huffman decompress output.huf restored.txt
Decompressed successfully!
Verified: Files match ✓
```

### Puncte Bonus: +10

---

## ⭐ Provocarea 2: Knight's Tour (Dificultate: Medie)

### Descriere

Implementați rezolvarea problemei "Turul Calului" (Knight's Tour) folosind **Backtracking**. Un cal de șah trebuie să viziteze fiecare pătrățel al tablei exact o dată.

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         KNIGHT'S TOUR (5×5)                                 │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   Mișcările posibile ale calului:                                          │
│                                                                             │
│        .   2   .   1   .          Numerele indică pozițiile               │
│        3   .   .   .   8          accesibile dintr-un punct               │
│        .   .   ♞   .   .          central                                  │
│        4   .   .   .   7                                                    │
│        .   5   .   6   .                                                    │
│                                                                             │
│   Soluție pentru 5×5:                                                       │
│   ┌────┬────┬────┬────┬────┐                                               │
│   │  1 │ 14 │  9 │ 20 │  3 │                                               │
│   ├────┼────┼────┼────┼────┤                                               │
│   │ 24 │ 19 │  2 │ 15 │ 10 │                                               │
│   ├────┼────┼────┼────┼────┤                                               │
│   │ 13 │  8 │ 25 │  4 │ 21 │                                               │
│   ├────┼────┼────┼────┼────┤                                               │
│   │ 18 │ 23 │  6 │ 11 │ 16 │                                               │
│   ├────┼────┼────┼────┼────┤                                               │
│   │  7 │ 12 │ 17 │ 22 │  5 │                                               │
│   └────┴────┴────┴────┴────┘                                               │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### Cerințe

```c
/* Funcții de implementat */
bool solve_knights_tour(int board[N][N], int n, int start_row, int start_col);
bool is_valid_move(int board[N][N], int n, int row, int col);
int count_onward_moves(int board[N][N], int n, int row, int col);

/* BONUS: Euristică Warnsdorff */
// Alege următoarea mișcare care duce la celula cu cele mai puține mișcări viitoare
bool solve_with_warnsdorff(int board[N][N], int n, int start_row, int start_col);
```

### Exemplu

```
$ ./knight 8
Board size: 8×8
Starting position: (0, 0)

Solving with Backtracking...
Solution found in 0.234 seconds

┌────┬────┬────┬────┬────┬────┬────┬────┐
│  1 │ 60 │ 39 │ 34 │ 31 │ 18 │  9 │ 64 │
├────┼────┼────┼────┼────┼────┼────┼────┤
...

With Warnsdorff heuristic: 0.001 seconds
```

### Puncte Bonus: +10

---

## ⭐ Provocarea 3: Word Search în Grid (Dificultate: Medie)

### Descriere

Implementați căutarea de cuvinte într-un grid de caractere folosind **Backtracking**. Cuvintele pot fi găsite în orice direcție (orizontal, vertical, diagonal) și pot schimba direcția.

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                          WORD SEARCH                                        │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   Grid:                Cuvinte de găsit:                                   │
│   ┌───┬───┬───┬───┐    - CAT ✓                                             │
│   │ C │ A │ T │ S │    - DOG ✓                                             │
│   ├───┼───┼───┼───┤    - STAR ✓                                            │
│   │ D │ O │ G │ X │    - CODE ✓                                            │
│   ├───┼───┼───┼───┤    - PIXEL ✗                                           │
│   │ S │ T │ A │ R │                                                         │
│   ├───┼───┼───┼───┤                                                         │
│   │ C │ O │ D │ E │                                                         │
│   └───┴───┴───┴───┘                                                         │
│                                                                             │
│   Direcții permise (8):                                                     │
│   ↖ ↑ ↗                                                                     │
│   ← • →                                                                     │
│   ↙ ↓ ↘                                                                     │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### Cerințe

```c
typedef struct {
    int start_row, start_col;
    int end_row, end_col;
    char path[100][2];  /* Coordonatele fiecărei litere */
    int path_length;
    bool found;
} SearchResult;

/* Funcții de implementat */
SearchResult find_word(char grid[N][M], int rows, int cols, const char *word);
bool search_from(char grid[N][M], int rows, int cols, 
                 const char *word, int index, int row, int col,
                 bool visited[N][M], SearchResult *result);
void highlight_word(char grid[N][M], SearchResult *result);
```

### Exemplu

```
$ ./wordsearch grid.txt words.txt

Grid loaded: 10×10
Words to find: 5

Searching...
  CAT:   Found at (0,0) → (0,2)
  DOG:   Found at (1,0) → (1,2)
  STAR:  Found at (2,0) → (2,3)
  CODE:  Found at (3,0) → (3,3)
  PIXEL: Not found

Found: 4/5 words
```

### Puncte Bonus: +10

---

## ⭐ Provocarea 4: Subset Sum cu Pruning Avansat (Dificultate: Dificilă)

### Descriere

Implementați problema Subset Sum cu **Backtracking** optimizat. Dându-se o mulțime de numere și o țintă S, găsiți toate submulțimile care au suma exact S.

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                          SUBSET SUM                                         │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   Input: {3, 7, 1, 8, 4, 12, 5}                                            │
│   Target: 15                                                                │
│                                                                             │
│   Soluții:                                                                  │
│   • {3, 7, 1, 4}    = 15 ✓                                                 │
│   • {3, 8, 4}       = 15 ✓                                                 │
│   • {7, 8}          = 15 ✓                                                 │
│   • {3, 7, 5}       = 15 ✓                                                 │
│   • {3, 12}         = 15 ✓                                                 │
│   • {7, 4, 4}       = imposibil (4 apare o dată)                           │
│   • {3, 1, 4, 12}   = 20 ✗                                                 │
│                                                                             │
│   Optimizări (Pruning):                                                     │
│   1. Sortare descrescătoare                                                │
│   2. Dacă sum > target: PRUNE                                              │
│   3. Dacă sum + restul_maxim < target: PRUNE                               │
│   4. Dacă target - sum < minim_rămas: PRUNE                                │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### Cerințe

```c
typedef struct {
    int *elements;      /* Elementele submulțimii */
    int count;          /* Numărul de elemente */
} Subset;

typedef struct {
    Subset solutions[MAX_SOLUTIONS];
    int solution_count;
    unsigned long long nodes_explored;
    unsigned long long pruned;
} SubsetSumResult;

/* Funcții de implementat */
SubsetSumResult find_all_subsets(int *arr, int n, int target);
void find_subsets_helper(int *arr, int n, int index, int current_sum, 
                         int target, int *current, int current_count,
                         SubsetSumResult *result);
bool can_reach_target(int *arr, int n, int index, int remaining);
```

### Exemplu

```
$ ./subsetsum "3,7,1,8,4,12,5" 15

Target: 15
Array: [3, 7, 1, 8, 4, 12, 5]

Finding all subsets...

Solutions found: 6
  [1] {3, 7, 1, 4}
  [2] {3, 8, 4}
  [3] {7, 8}
  [4] {3, 7, 5}
  [5] {3, 12}
  [6] {7, 4, 4}  -- Greșit! (elementul 4 apare o singură dată)

Stats:
  Nodes explored: 127
  Branches pruned: 89
  Time: 0.0001s
```

### Puncte Bonus: +10

---

## ⭐ Provocarea 5: Generator de Crossword (Dificultate: Foarte Dificilă)

### Descriere

Implementați un generator de rebusuri (crossword puzzles) folosind **Backtracking**. Dându-se o listă de cuvinte și un grid de o anumită dimensiune, plasați cuvintele astfel încât să se intersecteze corect.

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                      CROSSWORD GENERATOR                                    │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   Cuvinte: ALGORITHM, CODE, DATA, TREE, LOOP, ARRAY                        │
│                                                                             │
│   Grid generat (10×10):                                                     │
│   ┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┐                                │
│   │ A │ L │ G │ O │ R │ I │ T │ H │ M │ # │                                │
│   ├───┼───┼───┼───┼───┼───┼───┼───┼───┼───┤                                │
│   │ R │ # │ # │ # │ # │ # │ R │ # │ # │ # │                                │
│   ├───┼───┼───┼───┼───┼───┼───┼───┼───┼───┤                                │
│   │ R │ # │ D │ A │ T │ A │ E │ # │ # │ # │                                │
│   ├───┼───┼───┼───┼───┼───┼───┼───┼───┼───┤                                │
│   │ A │ # │ # │ # │ # │ # │ E │ # │ # │ # │                                │
│   ├───┼───┼───┼───┼───┼───┼───┼───┼───┼───┤                                │
│   │ Y │ # │ L │ O │ O │ P │ # │ # │ # │ # │                                │
│   ├───┼───┼───┼───┼───┼───┼───┼───┼───┼───┤                                │
│   │ # │ # │ # │ # │ # │ # │ # │ # │ # │ # │                                │
│   ...                                                                       │
│                                                                             │
│   Constrângeri:                                                             │
│   - Cuvintele se intersectează pe litere comune                            │
│   - Cuvintele nu se ating decât la intersecții                             │
│   - Toate cuvintele trebuie plasate                                        │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### Cerințe

```c
typedef struct {
    int row, col;           /* Poziția de start */
    int direction;          /* 0 = orizontal, 1 = vertical */
    const char *word;
} WordPlacement;

typedef struct {
    char grid[MAX_SIZE][MAX_SIZE];
    int size;
    WordPlacement placements[MAX_WORDS];
    int word_count;
    bool success;
} Crossword;

/* Funcții de implementat */
Crossword generate_crossword(const char **words, int word_count, int grid_size);
bool place_word(Crossword *cw, const char *word, int row, int col, int dir);
bool can_place_word(Crossword *cw, const char *word, int row, int col, int dir);
void remove_word(Crossword *cw, const char *word, int row, int col, int dir);
int count_intersections(Crossword *cw, const char *word, int row, int col, int dir);
```

### Exemplu

```
$ ./crossword words.txt 15

Words: ALGORITHM, BACKTRACKING, CODE, DATA, GREEDY, TREE, LOOP, ARRAY
Grid size: 15×15

Generating crossword...

SUCCESS! All 8 words placed.

   1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
  ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐
1 │B│A│C│K│T│R│A│C│K│I│N│G│#│#│#│
  ├─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┤
2 │#│L│#│#│R│#│R│#│#│#│#│R│#│#│#│
  ├─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┼─┤
...

Clues:
  ACROSS:
    1. Tehnică de explorare exhaustivă (12)
    3. Informații structurate (4)
  DOWN:
    1. Metodă pas cu pas (9)
    2. Paradigmă cu alegere locală (6)
```

### Puncte Bonus: +10

---

## 📊 Sistem de Puncte Bonus

| Provocări Completate | Total Bonus |
|----------------------|-------------|
| 1 provocare | +10 puncte |
| 2 provocări | +20 puncte |
| 3 provocări | +30 puncte |
| 4 provocări | +40 puncte |
| Toate 5 provocările | +50 puncte + Badge "🧠 Algorithm Master" |

### Criterii de Evaluare pentru Bonus

- **Corectitudine** (50%): Algoritmul produce rezultate corecte
- **Eficiență** (25%): Optimizări și pruning implementate
- **Cod curat** (15%): Structură clară, comentarii, modularitate
- **Cazuri limită** (10%): Tratare input invalid, cazuri speciale

---

## 📚 Resurse pentru Provocări

### Huffman Coding
- [Huffman Coding Visualization](https://www.cs.usfca.edu/~galles/visualization/Huffman.html)
- CLRS, Capitolul 16.3

### Knight's Tour
- [Warnsdorff's Algorithm](https://en.wikipedia.org/wiki/Knight%27s_tour)
- [Interactive Demo](https://www.geeksforgeeks.org/warnsdorffs-algorithm-knights-tour-problem/)

### Word Search
- [Backtracking for Word Search](https://leetcode.com/problems/word-search/)

### Subset Sum
- [NP-Complete Problems](https://en.wikipedia.org/wiki/Subset_sum_problem)
- Pruning techniques in backtracking

### Crossword Generation
- [Constraint Satisfaction Problems](https://en.wikipedia.org/wiki/Constraint_satisfaction_problem)
- Arc consistency și forward checking

---

## 💡 Sfaturi Generale

1. **Începeți cu versiunea simplă**: Implementați mai întâi algoritmul de bază fără optimizări.

2. **Adăugați pruning treptat**: După ce funcționează, adăugați optimizări una câte una.

3. **Testați extensiv**: Folosiți cazuri mici pentru debugging, apoi scalați.

4. **Măsurați îmbunătățirile**: Comparați numărul de noduri vizitate cu și fără optimizări.

5. **Documentați**:
   - Ce optimizări ați făcut și de ce
   - Complexitatea teoretică vs. practică
   - Cazuri limită tratate

---

*Succes și distracție plăcută cu provocările avansate!*

*Academia de Studii Economice din București - CSIE*
*Algoritmi și Tehnici de Programare - Provocări Extra*
*Ianuarie 2026*
