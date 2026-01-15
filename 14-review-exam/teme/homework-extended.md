# Provocări Extinse - Săptămâna 14

## 🚀 Provocări Avansate (Opțional)

Fiecare provocare rezolvată corect: **+10 puncte bonus**

Aceste provocări sunt destinate studenților care doresc să aprofundeze conceptele și să-și demonstreze competențele avansate de programare.

---

## ⭐ Provocare 1: Compresie de Date cu Huffman (Dificultate: Medie)

### Descriere

Implementați algoritmul de compresie Huffman pentru fișiere text. Acest algoritm folosește un arbore binar și o coadă de priorități pentru a crea coduri de lungime variabilă.

### Cerințe

```c
typedef struct HuffmanNode {
    char ch;                    // Caracterul (doar pentru frunze)
    int freq;                   // Frecvența
    struct HuffmanNode *left;
    struct HuffmanNode *right;
} HuffmanNode;

typedef struct {
    char codes[256][32];        // Codul Huffman pentru fiecare caracter
    int code_lengths[256];      // Lungimea fiecărui cod
    HuffmanNode *root;          // Rădăcina arborelui
} HuffmanTable;

// Funcții de implementat
HuffmanTable *build_huffman_table(const char *text);
char *compress(const char *text, HuffmanTable *table, int *compressed_size);
char *decompress(const char *compressed, int size, HuffmanTable *table);
void free_huffman_table(HuffmanTable *table);
```

### Algoritm

1. Calculați frecvența fiecărui caracter în text
2. Creați un nod frunză pentru fiecare caracter
3. Construiți un min-heap cu toate nodurile
4. Cât timp heap-ul are mai mult de un nod:
   - Extrageți cele două noduri cu frecvența minimă
   - Creați un nod părinte cu frecvența = suma copiilor
   - Inserați părintele în heap
5. Nodul rămas este rădăcina arborelui Huffman
6. Generați codurile parcurgând arborele (stânga=0, dreapta=1)

### Exemplu

```
Text: "AAABBC"
Frecvențe: A=3, B=2, C=1
Coduri: A=0, B=10, C=11
Comprimat: 000101011 (9 biți vs 48 biți original)
Raport compresie: 81.25%
```

### Bonus Points: +10

---

## ⭐ Provocare 2: Arbore Roșu-Negru (Dificultate: Ridicată)

### Descriere

Implementați un arbore roșu-negru (Red-Black Tree) complet, cu operații de inserare și ștergere care mențin proprietățile de echilibru.

### Cerințe

```c
typedef enum { RED, BLACK } Color;

typedef struct RBNode {
    int key;
    void *data;
    Color color;
    struct RBNode *parent;
    struct RBNode *left;
    struct RBNode *right;
} RBNode;

typedef struct {
    RBNode *root;
    RBNode *nil;    // Sentinel pentru frunze
    int count;
} RBTree;

// Funcții de implementat
RBTree *rb_create(void);
bool rb_insert(RBTree *tree, int key, void *data);
bool rb_delete(RBTree *tree, int key);
RBNode *rb_search(RBTree *tree, int key);
void rb_inorder(RBTree *tree, void (*visit)(RBNode*));
bool rb_verify(RBTree *tree);  // Verifică proprietățile RB
void rb_free(RBTree *tree);
```

### Proprietăți Red-Black

1. Fiecare nod este roșu sau negru
2. Rădăcina este întotdeauna neagră
3. Toate frunzele (NIL) sunt negre
4. Dacă un nod este roșu, ambii copii sunt negri
5. Toate căile de la rădăcină la frunze au același număr de noduri negre

### Rotații Necesare

- `left_rotate(RBTree *tree, RBNode *x)`
- `right_rotate(RBTree *tree, RBNode *x)`
- `rb_insert_fixup(RBTree *tree, RBNode *z)`
- `rb_delete_fixup(RBTree *tree, RBNode *x)`

### Bonus Points: +10

---

## ⭐ Provocare 3: LRU Cache cu O(1) (Dificultate: Medie)

### Descriere

Implementați un cache LRU (Least Recently Used) cu complexitate O(1) pentru toate operațiile, folosind combinația dintre hash table și doubly linked list.

### Cerințe

```c
typedef struct LRUNode {
    int key;
    int value;
    struct LRUNode *prev;
    struct LRUNode *next;
} LRUNode;

typedef struct {
    int capacity;
    int size;
    LRUNode *head;          // Cel mai recent folosit
    LRUNode *tail;          // Cel mai vechi
    LRUNode **hash_table;   // Acces O(1) după cheie
    int hash_size;
} LRUCache;

// Funcții de implementat
LRUCache *lru_create(int capacity);
int lru_get(LRUCache *cache, int key);      // Returnează -1 dacă nu există
void lru_put(LRUCache *cache, int key, int value);
void lru_free(LRUCache *cache);

// Funcții auxiliare
void move_to_front(LRUCache *cache, LRUNode *node);
void evict_lru(LRUCache *cache);
```

### Comportament

- `get(key)`: Returnează valoarea și mută elementul în față
- `put(key, value)`: Adaugă/actualizează și mută în față
- Când cache-ul este plin, se elimină elementul cel mai vechi (tail)

### Exemplu

```c
LRUCache *cache = lru_create(3);

lru_put(cache, 1, 100);  // cache: [1]
lru_put(cache, 2, 200);  // cache: [2, 1]
lru_put(cache, 3, 300);  // cache: [3, 2, 1]

lru_get(cache, 1);       // Returnează 100, cache: [1, 3, 2]

lru_put(cache, 4, 400);  // Evict 2, cache: [4, 1, 3]
lru_get(cache, 2);       // Returnează -1 (evicted)
```

### Bonus Points: +10

---

## ⭐ Provocare 4: Trie cu Autocomplete (Dificultate: Medie)

### Descriere

Implementați o structură Trie (Prefix Tree) cu funcționalitate de autocomplete, utilă pentru un motor de căutare sau un editor de text.

### Cerințe

```c
#define ALPHABET_SIZE 26

typedef struct TrieNode {
    struct TrieNode *children[ALPHABET_SIZE];
    bool is_end_of_word;
    int frequency;          // Câte căutări pentru acest cuvânt
    char *full_word;        // Cuvântul complet (doar în noduri terminale)
} TrieNode;

typedef struct {
    TrieNode *root;
    int word_count;
} Trie;

// Funcții de implementat
Trie *trie_create(void);
bool trie_insert(Trie *trie, const char *word);
bool trie_search(Trie *trie, const char *word);
bool trie_starts_with(Trie *trie, const char *prefix);
char **trie_autocomplete(Trie *trie, const char *prefix, int *count);
char **trie_top_suggestions(Trie *trie, const char *prefix, int top_n, int *count);
void trie_increment_frequency(Trie *trie, const char *word);
void trie_free(Trie *trie);
```

### Funcționalități

1. **Inserare**: Adaugă cuvinte în Trie
2. **Căutare**: Verifică dacă un cuvânt există
3. **Prefix**: Verifică dacă există cuvinte cu un anumit prefix
4. **Autocomplete**: Returnează toate cuvintele cu un anumit prefix
5. **Top Suggestions**: Returnează cele mai căutate cuvinte cu prefix dat

### Exemplu

```c
Trie *dict = trie_create();

trie_insert(dict, "program");
trie_insert(dict, "programare");
trie_insert(dict, "programator");
trie_insert(dict, "proiect");
trie_insert(dict, "profesor");

int count;
char **suggestions = trie_autocomplete(dict, "prog", &count);
// Returnează: ["program", "programare", "programator"]

// Simulare căutări
for (int i = 0; i < 100; i++) trie_increment_frequency(dict, "program");
for (int i = 0; i < 50; i++) trie_increment_frequency(dict, "programare");

char **top = trie_top_suggestions(dict, "prog", 2, &count);
// Returnează: ["program", "programare"] (ordonate după frecvență)
```

### Bonus Points: +10

---

## ⭐ Provocare 5: Bloom Filter (Dificultate: Medie-Ridicată)

### Descriere

Implementați un Bloom Filter, o structură de date probabilistică eficientă pentru verificarea apartenenței la o mulțime, cu trade-off între spațiu și acuratețe.

### Cerințe

```c
typedef struct {
    unsigned char *bit_array;   // Array de biți
    size_t size;                // Dimensiunea în biți
    int num_hash_functions;     // Numărul de funcții hash (k)
    int elements_count;         // Număr de elemente adăugate
} BloomFilter;

// Funcții de implementat
BloomFilter *bloom_create(size_t size, int num_hash_functions);
void bloom_add(BloomFilter *filter, const char *element);
bool bloom_might_contain(BloomFilter *filter, const char *element);
double bloom_false_positive_rate(BloomFilter *filter);
void bloom_free(BloomFilter *filter);

// Funcții hash (implementați cel puțin 3 diferite)
unsigned int hash1(const char *str, size_t size);
unsigned int hash2(const char *str, size_t size);
unsigned int hash3(const char *str, size_t size);
```

### Caracteristici

- **False Negatives**: Imposibile (dacă spune "nu", sigur nu este)
- **False Positives**: Posibile (dacă spune "poate", nu e sigur)
- **Rata de False Positives**: `(1 - e^(-kn/m))^k`
  - k = număr de funcții hash
  - n = număr de elemente
  - m = dimensiune în biți

### Aplicații Practice

- Verificare rapidă dacă un URL a fost vizitat
- Cache pentru query-uri de bază de date
- Detectare spam sau malware
- Verificare parole compromise

### Exemplu

```c
// Creare Bloom Filter pentru 1000 de elemente, rata FP < 1%
// Dimensiune optimă: m ≈ -n*ln(p) / (ln(2)^2) ≈ 9585 biți
// Număr optim funcții hash: k = (m/n) * ln(2) ≈ 7

BloomFilter *filter = bloom_create(10000, 7);

bloom_add(filter, "password123");
bloom_add(filter, "admin");
bloom_add(filter, "letmein");

if (bloom_might_contain(filter, "password123")) {
    printf("Parolă potențial compromisă!\n");
}

if (!bloom_might_contain(filter, "MySecureP@ss!")) {
    printf("Parolă sigur nu este în lista de compromis.\n");
}

printf("Rata estimată false positives: %.4f%%\n", 
       bloom_false_positive_rate(filter) * 100);
```

### Bonus Points: +10

---

## 📊 Sistem de Puncte Bonus

| Provocări Completate | Total Bonus |
|---------------------|-------------|
| 1 provocare | +10 puncte |
| 2 provocări | +20 puncte |
| 3 provocări | +30 puncte |
| 4 provocări | +40 puncte |
| Toate 5 | +50 puncte + Badge "Algoritmi Master" 🏆 |

---

## 📋 Cerințe pentru Validare

Pentru ca o provocare să fie considerată completă:

1. ✅ Codul compilează fără avertismente
2. ✅ Toate funcțiile specificate sunt implementate
3. ✅ Nu există memory leaks (verificat cu Valgrind)
4. ✅ Complexitățile de timp sunt respectate
5. ✅ Include funcția `main()` cu demonstrație completă
6. ✅ Comentarii explicative pentru algoritmi

---

## 💡 Sfaturi

- **Huffman**: Începeți cu calculul frecvențelor și construcția heap-ului
- **RB-Tree**: Înțelegeți mai întâi rotațiile, apoi fixup-urile
- **LRU Cache**: Desenați structura pe hârtie înainte de implementare
- **Trie**: Folosiți recursivitate pentru autocomplete
- **Bloom Filter**: Testați cu diferite dimensiuni pentru a vedea trade-off-ul

---

*Aceste provocări sunt opționale dar foarte valoroase pentru înțelegerea profundă a algoritmilor!*
