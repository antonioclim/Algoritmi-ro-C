# Săptămâna 11: Tabele de Dispersie (Hash Tables)

## 🎯 Obiective de Învățare

La finalul acestei săptămâni, studentul va fi capabil să:

1. **Rememoreze** principiile fundamentale ale tabelelor de dispersie și terminologia asociată (bucket, slot, load factor, collision)
2. **Înțeleagă** mecanismele funcțiilor hash și impactul distribuției cheilor asupra performanței
3. **Aplice** tehnicile de rezolvare a coliziunilor: chaining (înlănțuire) și open addressing (adresare deschisă)
4. **Analizeze** complexitatea temporală și spațială pentru operațiile de inserare, căutare și ștergere
5. **Evalueze** compromisurile între diferite strategii de hashing și dimensionarea tabelelor
6. **Creeze** implementări complete de hash table cu funcții hash personalizate și mecanisme robuste de redimensionare

---

## 📜 Context Istoric

### Originile Hashing-ului

Conceptul de hashing își are rădăcinile în anii 1950, când cercetătorii căutau metode eficiente de accesare a datelor. Prima utilizare documentată a tehnicilor de hashing datează din 1953, când **H.P. Luhn** de la IBM a depus un brevet pentru un sistem de indexare bazat pe hashing.

În 1956, **Arnold Dumey** a publicat prima descriere academică completă a hash tables în contextul compilatoarelor, propunând utilizarea restului împărțirii (modulo) ca funcție hash fundamentală. Această metodă simplă, dar eficientă, rămâne și astăzi una dintre cele mai utilizate tehnici.

Evoluția majoră a venit în 1963, când **W.W. Peterson** a formalizat analiza matematică a coliziunilor și a introdus conceptul de *linear probing*. Contribuțiile ulterioare ale lui **Donald Knuth** în volumul 3 al seriei "The Art of Computer Programming" (1973) au consolidat fundamentele teoretice ale hashing-ului, oferind analize riguroase ale diferitelor scheme de adresare.

### Personalitate Cheie: Donald Knuth (1938-)

Donald Ervin Knuth, profesor emerit la Universitatea Stanford, este considerat "părintele analizei algoritmilor". Lucrarea sa monumentală "The Art of Computer Programming" rămâne referința definitivă în domeniu.

Contribuțiile sale la teoria hashing-ului includ:
- Analiza matematică riguroasă a linear probing
- Demonstrarea că *double hashing* oferă distribuție mai uniformă
- Conceptul de *secondary clustering* și impactul său asupra performanței

> *"Premature optimization is the root of all evil."*
> — Donald Knuth

---

## 📚 Fundamente Teoretice

### 1. Anatomia unei Hash Table

O hash table (tabelă de dispersie) este o structură de date care implementează un **dicționar abstract** (ADT Dictionary), permițând asocierea cheilor cu valori.

```
┌─────────────────────────────────────────────────────────────────┐
│                    HASH TABLE ARCHITECTURE                       │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│    Key ──► Hash Function ──► Index ──► Bucket ──► Value         │
│                                                                  │
│    "Alice" ──► h("Alice") ──► 3 ──► table[3] ──► {...}          │
│    "Bob"   ──► h("Bob")   ──► 7 ──► table[7] ──► {...}          │
│    "Carol" ──► h("Carol") ──► 3 ──► COLIZIUNE!                  │
│                                                                  │
├─────────────────────────────────────────────────────────────────┤
│  Index │    0    │    1    │    2    │    3    │    4    │ ...  │
│  ──────┼─────────┼─────────┼─────────┼─────────┼─────────┼───── │
│  Data  │  NULL   │  NULL   │  NULL   │ Alice   │  NULL   │ ...  │
│        │         │         │         │ Carol?  │         │      │
└─────────────────────────────────────────────────────────────────┘
```

**Terminologie esențială:**
- **Bucket/Slot**: poziție individuală în tabelă
- **Load Factor (α)**: raportul n/m (elemente/dimensiune)
- **Collision**: situația când h(k₁) = h(k₂) pentru k₁ ≠ k₂
- **Clustering**: gruparea datelor în zone adiacente

### 2. Funcții Hash

O funcție hash ideală trebuie să satisfacă proprietatea de **distribuție uniformă**: fiecare cheie să aibă probabilitate egală de a fi plasată în oricare bucket.

#### 2.1 Metoda Împărțirii (Division Method)

```c
size_t hash_division(const char *key, size_t table_size) {
    size_t hash = 0;
    while (*key) {
        hash = hash * 31 + (unsigned char)(*key++);
    }
    return hash % table_size;
}
```

**Recomandare**: `table_size` ar trebui să fie un număr prim, nu aproape de puteri ale lui 2.

#### 2.2 Metoda Înmulțirii (Multiplication Method)

```c
#define A 0.6180339887  /* (√5 - 1) / 2 - Golden Ratio */

size_t hash_multiplication(unsigned int key, size_t table_size) {
    double product = key * A;
    double fractional = product - (unsigned long)product;
    return (size_t)(table_size * fractional);
}
```

#### 2.3 Funcții Hash pentru Șiruri de Caractere

| Funcție | Formula | Utilizare |
|---------|---------|-----------|
| **djb2** | hash = hash * 33 + c | Uz general, rapidă |
| **sdbm** | hash = hash * 65599 + c | Distribuție excelentă |
| **FNV-1a** | hash = (hash ^ c) * 16777619 | Foarte uniformă |

```c
/* djb2 - Dan Bernstein */
unsigned long hash_djb2(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;  /* hash * 33 + c */
    return hash;
}

/* sdbm */
unsigned long hash_sdbm(const char *str) {
    unsigned long hash = 0;
    int c;
    while ((c = *str++))
        hash = c + (hash << 6) + (hash << 16) - hash;
    return hash;
}
```

### 3. Strategii de Rezolvare a Coliziunilor

#### 3.1 Chaining (Înlănțuire Separată)

Fiecare bucket conține un pointer către o listă înlănțuită de elemente.

```
┌─────────────────────────────────────────────────────────────┐
│                    SEPARATE CHAINING                         │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  [0] ──► NULL                                               │
│  [1] ──► ┌───────┐    ┌───────┐                             │
│          │"Bob"  │───►│"Eve"  │───► NULL                    │
│          └───────┘    └───────┘                             │
│  [2] ──► NULL                                               │
│  [3] ──► ┌───────┐    ┌───────┐    ┌───────┐               │
│          │"Alice"│───►│"Carol"│───►│"Dave" │───► NULL       │
│          └───────┘    └───────┘    └───────┘               │
│  [4] ──► NULL                                               │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

**Avantaje:**
- Implementare simplă
- Funcționează bine cu load factor > 1
- Ștergerea este directă

**Dezavantaje:**
- Overhead de memorie pentru pointeri
- Performanță degradată de cache (non-contiguous)

#### 3.2 Open Addressing (Adresare Deschisă)

Toate elementele sunt stocate direct în tabel. La coliziune, se caută alt slot.

**Linear Probing:**
```
h(k, i) = (h'(k) + i) mod m
```

**Quadratic Probing:**
```
h(k, i) = (h'(k) + c₁·i + c₂·i²) mod m
```

**Double Hashing:**
```
h(k, i) = (h₁(k) + i·h₂(k)) mod m
```

```
┌─────────────────────────────────────────────────────────────┐
│                    LINEAR PROBING                            │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  Insert "Carol" where h("Carol") = 3, but slot 3 is full:   │
│                                                              │
│  [0]     [1]     [2]     [3]     [4]     [5]     [6]        │
│   -       -       -    "Alice"    -       -       -          │
│                           ▲                                  │
│                           │                                  │
│                     h("Carol")=3                             │
│                           │                                  │
│                           ▼                                  │
│   -       -       -    "Alice" "Carol"   -       -          │
│                                   ▲                          │
│                                   │                          │
│                           Probe: (3+1) mod 7 = 4            │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

### 4. Analiza Complexității

| Operație | Chaining (avg) | Chaining (worst) | Open Addr (avg) | Open Addr (worst) |
|----------|----------------|------------------|-----------------|-------------------|
| Insert   | O(1)           | O(n)             | O(1/(1-α))      | O(n)              |
| Search   | O(1 + α)       | O(n)             | O(1/(1-α))      | O(n)              |
| Delete   | O(1 + α)       | O(n)             | O(1/(1-α))      | O(n)              |

**Notă**: α = load factor = n/m

Pentru **chaining**, performanța medie rămâne O(1) dacă α este constant.
Pentru **open addressing**, performanța se degradează rapid când α → 1.

**Praguri recomandate pentru redimensionare:**
- Chaining: α > 2.0
- Open Addressing: α > 0.7

---

## 🏭 Aplicații Industriale

### 1. Implementare în Limbajul C - POSIX hsearch

```c
#include <search.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    ENTRY item, *found;
    
    /* Creează hash table cu 100 de sloturi */
    hcreate(100);
    
    /* Inserare */
    item.key = "cheie1";
    item.data = (void *)"valoare1";
    hsearch(item, ENTER);
    
    /* Căutare */
    item.key = "cheie1";
    found = hsearch(item, FIND);
    if (found)
        printf("Găsit: %s\n", (char *)found->data);
    
    hdestroy();
    return 0;
}
```

### 2. Python - dict internals

```python
# Python dict folosește open addressing cu perturbări
# Pseudo-implementare simplificată:

PERTURB_SHIFT = 5

def lookup(table, key):
    hash_value = hash(key)
    index = hash_value % len(table)
    perturb = hash_value
    
    while table[index] is not None:
        if table[index].key == key:
            return table[index].value
        index = (5 * index + perturb + 1) % len(table)
        perturb >>= PERTURB_SHIFT
    
    return None
```

### 3. Redis - Hash Implementation

Redis utilizează o implementare hibridă:
- **ziplist** pentru hash-uri mici (< 512 elemente)
- **hashtable** propriu-zisă pentru hash-uri mari

```c
/* Structura dictEntry din Redis */
typedef struct dictEntry {
    void *key;
    union {
        void *val;
        uint64_t u64;
        int64_t s64;
        double d;
    } v;
    struct dictEntry *next;  /* Chaining */
} dictEntry;
```

### 4. Java HashMap

```java
// Java 8+ folosește arbori roșu-negru când listele depășesc 8 elemente
static final int TREEIFY_THRESHOLD = 8;
static final int UNTREEIFY_THRESHOLD = 6;
static final int MIN_TREEIFY_CAPACITY = 64;
```

### 5. Criptografie - Password Hashing

```c
/* bcrypt - utilizat pentru stocarea securizată a parolelor */
#include <crypt.h>

char *hash_password(const char *password) {
    char salt[30];
    /* Generare salt cu prefix $2b$ pentru bcrypt */
    snprintf(salt, sizeof(salt), "$2b$12$%.22s", 
             generate_random_base64());
    return crypt(password, salt);
}
```

---

## 💻 Exerciții de Laborator

### Exercițiul 1: Hash Table cu Chaining

**Obiectiv**: Implementarea unei hash table complete folosind separate chaining.

**Cerințe**:
1. Definirea structurii `HashNode` și `HashTable`
2. Implementarea funcției hash djb2
3. Funcții pentru `insert`, `search`, `delete`
4. Funcție de redimensionare automată când load factor > 2.0
5. Afișarea statisticilor tabelei
6. Tratarea corectă a duplicatelor

**Fișier**: `src/exercise1.c`

### Exercițiul 2: Hash Table cu Open Addressing

**Obiectiv**: Implementarea unei hash table folosind linear probing și double hashing.

**Cerințe**:
1. Implementarea linear probing cu marcaje DELETED
2. Implementarea double hashing
3. Compararea performanței celor două metode
4. Detectarea și raportarea clustering-ului
5. Redimensionare când load factor > 0.7
6. Benchmark cu 10.000+ inserări

**Fișier**: `src/exercise2.c`

---

## 🔧 Compilare și Execuție

```bash
# Construire toate targeturile
make

# Rulare exemplu demonstrativ
make run

# Rulare teste automate
make test

# Verificare memory leaks cu Valgrind
make valgrind

# Curățare artefacte
make clean

# Afișare ajutor
make help
```

**Compilare individuală:**
```bash
gcc -Wall -Wextra -std=c11 -g -o example1 src/example1.c
gcc -Wall -Wextra -std=c11 -g -o exercise1 src/exercise1.c
gcc -Wall -Wextra -std=c11 -g -o exercise2 src/exercise2.c
```

---

## 📁 Structura Directorului

```
week-11-hash-tables/
├── README.md                           # Acest fișier
├── Makefile                            # Automatizare build
│
├── slides/
│   ├── presentation-week11.html        # Prezentare principală (40 slide-uri)
│   └── presentation-comparativ.html    # Comparație Pseudocod/C/Python
│
├── src/
│   ├── example1.c                      # Exemplu complet demonstrativ
│   ├── exercise1.c                     # Exercițiu: Chaining
│   └── exercise2.c                     # Exercițiu: Open Addressing
│
├── data/
│   ├── words_ro.txt                    # 1000 cuvinte românești pentru teste
│   └── names.txt                       # 500 nume pentru teste
│
├── tests/
│   ├── test1_input.txt                 # Input pentru exercițiul 1
│   ├── test1_expected.txt              # Output așteptat exercițiul 1
│   ├── test2_input.txt                 # Input pentru exercițiul 2
│   └── test2_expected.txt              # Output așteptat exercițiul 2
│
├── teme/
│   ├── homework-requirements.md        # Cerințe teme (2 × 50 puncte)
│   └── homework-extended.md            # Provocări bonus (5 × 10 puncte)
│
└── solution/
    ├── exercise1_sol.c                 # Soluție exercițiul 1
    ├── exercise2_sol.c                 # Soluție exercițiul 2
    ├── homework1_sol.c                 # Soluție tema 1
    └── homework2_sol.c                 # Soluție tema 2
```

---

## 📖 Lectură Recomandată

### Resurse Esențiale
- **Cormen, T.H. et al.** - "Introduction to Algorithms" (CLRS), Cap. 11: Hash Tables
- **Knuth, D.E.** - "The Art of Computer Programming", Vol. 3, Cap. 6.4: Hashing
- **Sedgewick, R.** - "Algorithms in C", Parts 1-4, Cap. 14: Hashing

### Resurse Avansate
- **Mitzenmacher, M. & Upfal, E.** - "Probability and Computing", Cap. 5: Balls and Bins
- **Pagh, R. & Rodler, F.** - "Cuckoo Hashing" (ESA 2001)
- **Fotakis, D. et al.** - "Space Efficient Hash Tables with Worst Case Constant Access Time"

### Resurse Online
- [Visualgo - Hash Table Visualization](https://visualgo.net/en/hashtable)
- [GeeksforGeeks - Hashing Data Structure](https://www.geeksforgeeks.org/hashing-data-structure/)
- [MIT OpenCourseWare - Hashing with Chaining](https://ocw.mit.edu/courses/6-006-introduction-to-algorithms-fall-2011/)

---

## ✅ Lista de Auto-Evaluare

Înainte de a considera săptămâna finalizată, verifică:

- [ ] Pot explica diferența dintre chaining și open addressing
- [ ] Înțeleg de ce dimensiunea tabelei ar trebui să fie un număr prim
- [ ] Pot implementa o funcție hash pentru șiruri de caractere
- [ ] Știu să calculez load factor și să decid când să redimensionez
- [ ] Pot analiza complexitatea pentru inserare, căutare și ștergere
- [ ] Înțeleg fenomenul de clustering și cum să-l evit
- [ ] Pot implementa ștergerea corectă în open addressing (DELETED marker)
- [ ] Știu să aleg între chaining și open addressing în funcție de context
- [ ] Pot explica double hashing și avantajele sale
- [ ] Am înțeles implementările din bibliotecile standard (Python dict, Java HashMap)

---

## 💼 Pregătire pentru Interviu

### Întrebări Frecvente

1. **Ce este o hash table și care sunt operațiile de bază?**
   
   O hash table este o structură de date care mapează chei la valori folosind o funcție hash. Operațiile de bază sunt: insert O(1), search O(1), delete O(1) în medie.

2. **Cum gestionezi coliziunile într-o hash table?**
   
   Două strategii principale: (1) Chaining - fiecare bucket conține o listă de elemente; (2) Open Addressing - căutăm alt slot liber (linear/quadratic probing, double hashing).

3. **Ce este load factor și de ce este important?**
   
   Load factor α = n/m (elemente/dimensiune). Determină eficiența: pentru chaining, redimensionăm la α > 2; pentru open addressing, la α > 0.7.

4. **De ce dimensiunea tabelei ar trebui să fie un număr prim?**
   
   Reduce clustering-ul și asigură o distribuție mai uniformă a cheilor, mai ales pentru funcții hash bazate pe modulo.

5. **Care sunt diferențele dintre HashMap și HashSet în Java?**
   
   HashMap stochează perechi cheie-valoare, HashSet stochează doar chei (intern folosește HashMap cu valoare dummy). Ambele au O(1) pentru operații de bază.

---

## 🔗 Săptămâna Următoare

**Săptămâna 12: Grafuri - Concepte de Bază**

Vom explora:
- Reprezentări: matrice de adiacență vs. liste de adiacență
- Parcurgeri: BFS (Breadth-First Search) și DFS (Depth-First Search)
- Aplicații: detectarea ciclurilor, componente conexe
- Grafuri direcționate vs. nedirecționate

Hash tables vor fi utilizate pentru implementarea eficientă a listelor de adiacență!

---

*Ultima actualizare: Ianuarie 2026*
*Curs: Algoritmi și Tehnici de Programare (ATP)*
*Academia de Studii Economice București - CSIE*
