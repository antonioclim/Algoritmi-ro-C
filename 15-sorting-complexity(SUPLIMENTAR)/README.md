# Săptămâna 15: Algoritmi de Sortare Avansați și Analiza Complexității

## 🎯 Obiective de Învățare

La finalul acestei săptămâni, veți fi capabili să:

1. **Rețineți** (Remember): Definiți notațiile asimptotice (O, Ω, Θ) și enumerați complexitățile standard ale algoritmilor de sortare (Quick Sort, Merge Sort, Heap Sort, Counting Sort, Radix Sort)
2. **Înțelegeți** (Understand): Explicați de ce Quick Sort are O(n²) în cazul cel mai defavorabil dar O(n log n) în medie, și cum alegerea pivotului aleator îmbunătățește performanța în practică
3. **Aplicați** (Apply): Implementați în C algoritmii: Quick Sort (cu variante de pivot), Merge Sort (top-down), Heap Sort (in-place), Counting Sort și Radix Sort (LSD)
4. **Analizați** (Analyse): Comparați empiric performanța algoritmilor pe diferite distribuții de date (random, sortat, inversat, aproape sortat) și interpretați rezultatele
5. **Evaluați** (Evaluate): Alegeți algoritmul optim în funcție de caracteristicile datelor de intrare, constrângerile de memorie și cerințele de stabilitate
6. **Creați** (Create): Proiectați și implementați un algoritm hibrid (Introsort) care combină avantajele mai multor algoritmi, adaptându-se dinamic la caracteristicile input-ului

---

## 📜 Context Istoric

### Evoluția Algoritmilor de Sortare

Sortarea datelor reprezintă una dintre cele mai fundamentale probleme în istoria informaticii. Primele calculatoare electronice din anii 1940 erau utilizate frecvent pentru sortarea datelor din recensământuri și evidențe comerciale, ceea ce a stimulat cercetarea intensivă în acest domeniu.

În anii 1950-1960, informatica teoretică a stabilit fundamentele analizei algoritmilor. **Donald Knuth** a documentat extensiv istoria și teoria sortării în volumul 3 al monumentalei sale lucrări "The Art of Computer Programming" (1973), care rămâne referința definitorie în domeniu.

Algoritmii de sortare pot fi clasificați în două categorii majore: algoritmi bazați pe **comparații** (Quick Sort, Merge Sort, Heap Sort) care au limita inferioară teoretică de Ω(n log n), și algoritmi **non-comparativi** (Counting Sort, Radix Sort, Bucket Sort) care pot atinge O(n) în condiții specifice.

### Evoluția Cronologică

```
    ┌─────────────────────────────────────────────────────────────────┐
    │  CRONOLOGIA ALGORITMILOR DE SORTARE                             │
    ├─────────────────────────────────────────────────────────────────┤
    │                                                                 │
    │  1945 │ Merge Sort - John von Neumann                          │
    │       │ Primul algoritm de sortare O(n log n)                   │
    │       │                                                         │
    │  1954 │ Radix Sort (modern) - Harold H. Seward                 │
    │       │ Sortare în timp liniar pentru întregi                   │
    │       │                                                         │
    │  1959 │ Shell Sort - Donald Shell                               │
    │       │ Îmbunătățirea Insertion Sort cu gap-uri                 │
    │       │                                                         │
    │  1961 │ Quick Sort - C.A.R. Hoare                              │
    │       │ Cel mai rapid algoritm în practică                      │
    │       │                                                         │
    │  1964 │ Heap Sort - J.W.J. Williams                             │
    │       │ Sortare în O(n log n) garantat, in-place               │
    │       │                                                         │
    │  1969 │ Introsort - David Musser                               │
    │       │ Algoritm hibrid adaptiv                                 │
    │       │                                                         │
    └─────────────────────────────────────────────────────────────────┘
```

---

### Figură Cheie: Charles Antony Richard Hoare (1934-)

```
    ┌─────────────────────────────────────────────┐
    │  Sir Charles Antony Richard Hoare           │
    │  "Tony" Hoare                                │
    │  Informatician Britanic                      │
    │                                             │
    │  • Născut în Colombo, Sri Lanka (1934)      │
    │  • Inventator al Quick Sort (1961)          │
    │  • Creatorul logicii Hoare pentru verificare│
    │  • Premiul Turing (1980)                    │
    │  • Profesor la Oxford                       │
    │  • Fellow la Microsoft Research Cambridge   │
    └─────────────────────────────────────────────┘
```

Tony Hoare a inventat Quick Sort în 1961, când lucra la National Physical Laboratory din Marea Britanie. Algoritmul a apărut din necesitatea practică de a sorta cuvinte pentru un proiect de traducere automată rusă-engleză. Hoare a demonstrat că, deși cazul cel mai defavorabil este O(n²), performanța medie de O(n log n) și constanta mică fac Quick Sort să fie cel mai rapid algoritm de sortare în practică pentru majoritatea scenariilor.

> *"There are two ways of constructing a software design: One way is to make it so simple that there are obviously no deficiencies, and the other way is to make it so complicated that there are no obvious deficiencies. The first method is far more difficult."*
> — C.A.R. Hoare, Premiul Turing Lecture, 1980

---

### Figură Cheie: John von Neumann (1903-1957)

```
    ┌─────────────────────────────────────────────┐
    │  John von Neumann                           │
    │  Matematician și Polimator                  │
    │                                             │
    │  • Născut în Budapesta, Ungaria (1903)      │
    │  • Inventator al Merge Sort (1945)          │
    │  • Arhitectura von Neumann pentru calculatoare│
    │  • Contribuții în mecanica cuantică         │
    │  • Teoria jocurilor                         │
    │  • Membru fondator Princeton IAS            │
    └─────────────────────────────────────────────┘
```

Von Neumann a dezvoltat Merge Sort în 1945 ca parte a programării primului calculator electronic EDVAC. Merge Sort a fost primul algoritm demonstrat să aibă complexitate O(n log n), stabilind un standard pentru eficiența sortării.

---

## 📚 Fundamente Teoretice

### 1. Notații Asimptotice

Analiza complexității algoritmilor folosește notații asimptotice pentru a descrie comportamentul funcțiilor pentru input-uri mari:

```
    ┌─────────────────────────────────────────────────────────────────┐
    │  NOTAȚII ASIMPTOTICE                                            │
    ├─────────────────────────────────────────────────────────────────┤
    │                                                                 │
    │  O(g(n)) - "Big O" - Limita superioară                         │
    │     f(n) = O(g(n)) dacă ∃ c > 0, n₀ : f(n) ≤ c·g(n), ∀n ≥ n₀  │
    │     Exemplu: 3n² + 2n + 1 = O(n²)                              │
    │                                                                 │
    │  Ω(g(n)) - "Big Omega" - Limita inferioară                     │
    │     f(n) = Ω(g(n)) dacă ∃ c > 0, n₀ : f(n) ≥ c·g(n), ∀n ≥ n₀  │
    │     Exemplu: Orice sortare bazată pe comparații este Ω(n log n)│
    │                                                                 │
    │  Θ(g(n)) - "Big Theta" - Limită strânsă                        │
    │     f(n) = Θ(g(n)) dacă f(n) = O(g(n)) și f(n) = Ω(g(n))       │
    │     Exemplu: Merge Sort este Θ(n log n) în toate cazurile      │
    │                                                                 │
    └─────────────────────────────────────────────────────────────────┘
```

### 2. Tabel Comparativ al Algoritmilor de Sortare

```
    ┌──────────────────┬───────────────┬───────────────┬───────────────┬────────────┬────────────┐
    │ Algoritm         │ Best Case     │ Average Case  │ Worst Case    │ Spațiu     │ Stabil?    │
    ├──────────────────┼───────────────┼───────────────┼───────────────┼────────────┼────────────┤
    │ Quick Sort       │ O(n log n)    │ O(n log n)    │ O(n²)         │ O(log n)   │ NU         │
    │ Merge Sort       │ O(n log n)    │ O(n log n)    │ O(n log n)    │ O(n)       │ DA         │
    │ Heap Sort        │ O(n log n)    │ O(n log n)    │ O(n log n)    │ O(1)       │ NU         │
    │ Counting Sort    │ O(n + k)      │ O(n + k)      │ O(n + k)      │ O(k)       │ DA         │
    │ Radix Sort       │ O(d·(n + k))  │ O(d·(n + k))  │ O(d·(n + k))  │ O(n + k)   │ DA         │
    │ Shell Sort       │ O(n log n)    │ O(n^{4/3})    │ O(n^{3/2})    │ O(1)       │ NU         │
    │ Insertion Sort   │ O(n)          │ O(n²)         │ O(n²)         │ O(1)       │ DA         │
    │ Introsort        │ O(n log n)    │ O(n log n)    │ O(n log n)    │ O(log n)   │ NU         │
    └──────────────────┴───────────────┴───────────────┴───────────────┴────────────┴────────────┘
    
    Notă: k = intervalul valorilor, d = numărul de cifre
```

### 3. Quick Sort - Detalii

**Principiul Divide et Impera**:
1. Alege un **pivot** din array
2. **Partiționează**: mută elementele mai mici la stânga, mai mari la dreapta
3. **Recursiv** sortează cele două subarray-uri

```
    ┌─────────────────────────────────────────────────────────────┐
    │  QUICK SORT - PARTIȚIONARE                                   │
    ├─────────────────────────────────────────────────────────────┤
    │                                                             │
    │   Array inițial:  [3, 8, 2, 5, 1, 4, 7, 6]                  │
    │                                                             │
    │   Pivot = 6 (ultimul element)                               │
    │                                                             │
    │   Partiționare (schema Lomuto):                             │
    │   i = -1 (index pentru elemente mai mici)                   │
    │                                                             │
    │   j=0: arr[0]=3 < 6? DA → i=0, swap(arr[0], arr[0])        │
    │   j=1: arr[1]=8 < 6? NU                                     │
    │   j=2: arr[2]=2 < 6? DA → i=1, swap(arr[1], arr[2])        │
    │        [3, 2, 8, 5, 1, 4, 7, 6]                             │
    │   j=3: arr[3]=5 < 6? DA → i=2, swap(arr[2], arr[3])        │
    │        [3, 2, 5, 8, 1, 4, 7, 6]                             │
    │   j=4: arr[4]=1 < 6? DA → i=3, swap(arr[3], arr[4])        │
    │        [3, 2, 5, 1, 8, 4, 7, 6]                             │
    │   j=5: arr[5]=4 < 6? DA → i=4, swap(arr[4], arr[5])        │
    │        [3, 2, 5, 1, 4, 8, 7, 6]                             │
    │   j=6: arr[6]=7 < 6? NU                                     │
    │                                                             │
    │   Final: swap(arr[5], arr[7]) - punem pivotul la poziția   │
    │        [3, 2, 5, 1, 4] [6] [7, 8]                          │
    │        ───────────────  ─  ───────                          │
    │           < pivot       =   > pivot                         │
    │                                                             │
    └─────────────────────────────────────────────────────────────┘
```

**Alegerea Pivotului - Impactul asupra Performanței**:

```
    ┌─────────────────────────────────────────────────────────────┐
    │  STRATEGII DE ALEGERE A PIVOTULUI                           │
    ├─────────────────────────────────────────────────────────────┤
    │                                                             │
    │  1. Pivot = ultimul element (Lomuto)                        │
    │     ✗ Worst case pe array sortat: O(n²)                    │
    │     ✓ Simplu de implementat                                 │
    │                                                             │
    │  2. Pivot = primul element                                  │
    │     ✗ Worst case pe array sortat: O(n²)                    │
    │     ✗ La fel de vulnerabil                                  │
    │                                                             │
    │  3. Pivot ALEATOR (randomizat)                             │
    │     ✓ Expected time: O(n log n)                            │
    │     ✓ Evită worst case sistematic                          │
    │                                                             │
    │  4. Median-of-three (median din first, middle, last)       │
    │     ✓ Performanță bună în practică                         │
    │     ✓ Evită worst case pe date sortate                     │
    │     ✗ Poate fi manipulat de adversar                       │
    │                                                             │
    └─────────────────────────────────────────────────────────────┘
```

**Implementare C - Quick Sort Randomizat**:

```c
/* Generează index aleator în intervalul [low, high] */
int random_pivot(int low, int high) {
    return low + rand() % (high - low + 1);
}

/* Partiționare cu pivot aleator */
int partition_random(int arr[], int low, int high) {
    int pivot_idx = random_pivot(low, high);
    swap(&arr[pivot_idx], &arr[high]);  /* Mutăm pivotul la final */
    
    int pivot = arr[high];
    int i = low - 1;
    
    for (int j = low; j < high; j++) {
        if (arr[j] <= pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return i + 1;
}

/* Quick Sort recursiv */
void quick_sort(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition_random(arr, low, high);
        quick_sort(arr, low, pi - 1);
        quick_sort(arr, pi + 1, high);
    }
}
```

### 4. Merge Sort - Detalii

**Principiul**: Divide array-ul în jumătăți până la elemente singulare, apoi merge (interclasează) subliste sortate.

```
    ┌─────────────────────────────────────────────────────────────┐
    │  MERGE SORT - VIZUALIZARE                                    │
    ├─────────────────────────────────────────────────────────────┤
    │                                                             │
    │   [38, 27, 43, 3, 9, 82, 10]                                │
    │              │                                               │
    │     ┌────────┴────────┐                                     │
    │     │                 │                                      │
    │  [38, 27, 43, 3]  [9, 82, 10]                               │
    │        │              │                                      │
    │    ┌───┴───┐     ┌────┴───┐                                 │
    │    │       │     │        │                                  │
    │ [38,27] [43,3] [9,82]  [10]                                 │
    │    │       │     │        │                                  │
    │   ┌┴┐    ┌┴┐   ┌┴┐       │                                  │
    │  [38][27][43][3][9][82] [10]                                │
    │   └┬┘    └┬┘   └┬┘       │                                  │
    │    │       │     │        │                                  │
    │ [27,38] [3,43] [9,82]  [10]   ← MERGE nivel 1              │
    │    │       │     │        │                                  │
    │    └───┬───┘     └────┬───┘                                 │
    │        │              │                                      │
    │  [3,27,38,43]  [9,10,82]      ← MERGE nivel 2              │
    │        │              │                                      │
    │        └──────┬───────┘                                     │
    │               │                                              │
    │    [3, 9, 10, 27, 38, 43, 82]  ← MERGE final               │
    │                                                             │
    └─────────────────────────────────────────────────────────────┘
```

### 5. Heap Sort - Detalii

**Proprietatea Heap (Max-Heap)**: Pentru orice nod i, `A[parent(i)] ≥ A[i]`

```
    ┌─────────────────────────────────────────────────────────────┐
    │  HEAP SORT - FAZE                                            │
    ├─────────────────────────────────────────────────────────────┤
    │                                                             │
    │  FAZA 1: BUILD-MAX-HEAP                                     │
    │  ────────────────────────                                   │
    │  Array: [4, 10, 3, 5, 1]                                    │
    │                                                             │
    │           4                    10                           │
    │          / \                  /  \                          │
    │        10   3      →        5    3                          │
    │        / \                 / \                              │
    │       5   1               4   1                             │
    │                                                             │
    │  FAZA 2: EXTRACT-MAX REPETAT                               │
    │  ────────────────────────────                               │
    │  1. Swap root cu ultimul element                           │
    │  2. Reduce heap size                                        │
    │  3. Heapify root                                            │
    │  4. Repetă                                                  │
    │                                                             │
    │  [10,5,3,4,1] → swap → [1,5,3,4│10] → heapify → [5,4,3,1│10]│
    │  [5,4,3,1│10] → swap → [1,4,3│5,10] → heapify → [4,1,3│5,10]│
    │  ...                                                        │
    │  Rezultat: [1, 3, 4, 5, 10]                                 │
    │                                                             │
    └─────────────────────────────────────────────────────────────┘
```

### 6. Algoritmi Non-Comparativi

**Counting Sort** - Pentru valori în interval limitat [0, k]:

```
    ┌─────────────────────────────────────────────────────────────┐
    │  COUNTING SORT - EXEMPLU                                     │
    ├─────────────────────────────────────────────────────────────┤
    │                                                             │
    │  Input: [4, 2, 2, 8, 3, 3, 1]   k = 8                       │
    │                                                             │
    │  Pas 1: Numărare                                            │
    │  count[0..8] = [0, 1, 2, 2, 1, 0, 0, 0, 1]                  │
    │                    1  2  3  4           8                   │
    │                                                             │
    │  Pas 2: Prefix sums (pentru poziționare)                    │
    │  count[0..8] = [0, 1, 3, 5, 6, 6, 6, 6, 7]                  │
    │                                                             │
    │  Pas 3: Plasare în output (de la dreapta la stânga)        │
    │  Output: [1, 2, 2, 3, 3, 4, 8]                              │
    │                                                             │
    │  Complexitate: O(n + k)   Stabil: DA                        │
    │                                                             │
    └─────────────────────────────────────────────────────────────┘
```

**Radix Sort (LSD)** - Sortează cifră cu cifră, de la cea mai puțin semnificativă:

```
    ┌─────────────────────────────────────────────────────────────┐
    │  RADIX SORT LSD - EXEMPLU                                    │
    ├─────────────────────────────────────────────────────────────┤
    │                                                             │
    │  Input: [170, 45, 75, 90, 802, 24, 2, 66]                   │
    │                                                             │
    │  Sortare după unități (d=1):                                │
    │  [170, 90, 802, 2, 24, 45, 75, 66]                          │
    │     0   0    2  2   4   5   5   6                           │
    │                                                             │
    │  Sortare după zeci (d=2):                                   │
    │  [802, 2, 24, 45, 66, 170, 75, 90]                          │
    │     0  0   2   4   6    7   7   9                           │
    │                                                             │
    │  Sortare după sute (d=3):                                   │
    │  [2, 24, 45, 66, 75, 90, 170, 802]                          │
    │   0   0   0   0   0   0    1    8                           │
    │                                                             │
    │  Rezultat SORTAT!                                           │
    │                                                             │
    └─────────────────────────────────────────────────────────────┘
```

---

## 🏭 Aplicații Industriale

### 1. Sistemele de Baze de Date (PostgreSQL, MySQL)

```c
/* Simplificare: Sortarea în database queries */
/* PostgreSQL folosește o combinație de Quick Sort și Merge Sort */

typedef struct SortTuple {
    void *datum;           /* Valoarea de sortat */
    int32_t tuple_index;   /* Index în tabela originală */
} SortTuple;

/* External Sort pentru date care nu încap în memorie:
   1. Citește chunk-uri în memorie
   2. Sortează fiecare chunk cu Quick Sort
   3. Scrie "runs" sortate pe disc
   4. Merge runs folosind k-way merge
*/
```

### 2. Linux Kernel - qsort() din glibc

Implementarea standard C folosește **Introsort**:
- Începe cu Quick Sort
- Dacă recursiunea depășește 2·log₂(n), trece la Heap Sort
- Pentru array-uri mici (< 16 elemente), folosește Insertion Sort

### 3. Motoarele de Căutare

```
    ┌─────────────────────────────────────────────────────────────┐
    │  SORTARE ÎN MOTOARE DE CĂUTARE                              │
    ├─────────────────────────────────────────────────────────────┤
    │                                                             │
    │  1. Indexare: Sortarea posting lists (docID, frequency)    │
    │     → Radix Sort pentru docID-uri (întregi)                │
    │                                                             │
    │  2. Ranking: Sortarea rezultatelor după scor               │
    │     → Partial Sort (Heap) pentru top-K rezultate           │
    │                                                             │
    │  3. Merge de index-uri distribuite                          │
    │     → K-way Merge Sort extern                               │
    │                                                             │
    └─────────────────────────────────────────────────────────────┘
```

### 4. Algoritmi de Compresie

BZIP2 folosește **Burrows-Wheeler Transform** care necesită sortarea tuturor rotațiilor unui string - operație intensivă rezolvată cu variante optimizate de suffix array sorting.

---

## 💻 Exerciții de Laborator

### Exercițiul 1: Benchmark Comparativ de Sortare

**Obiectiv**: Implementați și comparați empiric performanța algoritmilor Quick Sort, Merge Sort și Heap Sort pe diferite tipuri de date.

**Cerințe**:
1. Implementați cele trei algoritmi cu semnătură uniformă
2. Generați array-uri de test: random, sortat crescător, sortat descrescător, aproape sortat
3. Măsurați timpul de execuție și numărul de comparații
4. Generați un raport tabelar cu rezultatele
5. Analizați și comentați rezultatele observate

**Fișiere**: `src/exercise1.c`, `data/random_10k.txt`, `data/sorted_10k.txt`

### Exercițiul 2: Sortare Externă (External Merge Sort)

**Obiectiv**: Implementați sortarea unui fișier de date care nu încape în memorie folosind External Merge Sort.

**Cerințe**:
1. Citiți date în chunk-uri de dimensiune fixă
2. Sortați fiecare chunk în memorie și salvați ca "run" pe disc
3. Implementați k-way merge pentru combinarea runs
4. Simulați limitarea memoriei (buffer de max 1000 elemente)
5. Validați rezultatul final

**Fișiere**: `src/exercise2.c`, `data/large_unsorted.txt`

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

# Compilează și rulează exercițiul 2
make exercise2
./exercise2

# Rulează testele automate
make test

# Verificare memory leaks
make valgrind

# Benchmark complet
make benchmark

# Curățare
make clean

# Ajutor
make help
```

---

## 📁 Structura Directorului

```
15-sorting-complexity/
├── README.md                           # Acest document
├── Makefile                            # Automatizare build
│
├── slides/
│   ├── presentation-week15.html        # Prezentare principală (35+ slides)
│   └── presentation-comparativ.html    # Comparație Pseudocod/C/Python
│
├── src/
│   ├── example1.c                      # Exemplu complet sortare (~800 linii)
│   ├── exercise1.c                     # Exercițiu: Benchmark comparativ
│   └── exercise2.c                     # Exercițiu: External Sort
│
├── data/
│   ├── random_10k.txt                  # Date aleatoare
│   ├── sorted_10k.txt                  # Date sortate crescător
│   ├── reversed_10k.txt                # Date sortate descrescător
│   └── nearly_sorted.txt               # Date aproape sortate
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
- **Cormen, Leiserson, Rivest, Stein** - *Introduction to Algorithms*, 4th Ed., Capitolele 7-9 (Quicksort, Sorting in Linear Time, Medians and Order Statistics)
- **Sedgewick, Wayne** - *Algorithms*, 4th Ed., Secțiunile 2.2-2.5
- **Knuth** - *The Art of Computer Programming*, Vol. 3: *Sorting and Searching*, Capitolul 5

### Avansate
- **Hoare, C.A.R.** - *Quicksort*, Computer Journal (1962) - Paper-ul original
- **Musser, D.R.** - *Introspective Sorting and Selection Algorithms*, Software: Practice and Experience (1997)
- **Sedgewick, R.** - *Analysis of Shellsort and Related Algorithms*, ESA (1996)

### Resurse Online
- [Visualgo - Sorting](https://visualgo.net/en/sorting) - Animații interactive
- [Sorting Algorithm Animations](https://www.toptal.com/developers/sorting-algorithms) - Comparații vizuale
- [Big-O Cheat Sheet](https://www.bigocheatsheet.com/) - Referință complexități

---

## ✅ Lista de Auto-Verificare

Înainte de a considera această săptămână completă, verificați că:

- [ ] Pot defini și diferenția notațiile O, Ω și Θ
- [ ] Pot explica de ce Quick Sort are O(n²) worst-case și cum se evită
- [ ] Pot implementa Quick Sort cu pivot aleator și median-of-three
- [ ] Pot implementa Merge Sort și înțeleg recursivitatea divide-et-impera
- [ ] Pot implementa Heap Sort și funcția heapify
- [ ] Știu când Counting Sort și Radix Sort sunt alegeri mai bune
- [ ] Pot analiza empiric performanța pe diferite tipuri de input
- [ ] Înțeleg conceptul de stabilitate și când este important
- [ ] Pot alege algoritmul optim pentru un scenariu dat
- [ ] Știu cum funcționează Introsort și de ce este folosit în bibliotecile standard

---

## 💼 Pregătire pentru Interviu

### Întrebări Frecvente

1. **Care este diferența între complexitatea worst-case și average-case pentru Quick Sort?**
   - Worst-case: O(n²) - apare când pivotul e mereu minim sau maxim (array sortat cu pivot la capăt)
   - Average-case: O(n log n) - partiționare aproximativ echilibrată
   - Soluții: pivot aleator, median-of-three

2. **Când ai alege Merge Sort în loc de Quick Sort?**
   - Când ai nevoie de stabilitate
   - Când ai nevoie de complexitate garantată O(n log n)
   - Pentru linked lists (Quick Sort e mai complicat)
   - Pentru external sorting (date pe disc)

3. **Explică Heap Sort în 3 propoziții.**
   - Construiești un max-heap din array în O(n)
   - Repetat extragi maximul (rădăcina) și îl pui la final
   - Complexitate garantată O(n log n), in-place, dar nu e stabil

4. **Când ai folosi Counting Sort?**
   - Când valorile sunt întregi într-un interval limitat [0, k] unde k = O(n)
   - Când ai nevoie de sortare stabilă
   - Exemplu: sortarea notelor studenților (1-10), vârstelor (0-150)

5. **Ce este Introsort și de ce e folosit în std::sort?**
   - Algoritm hibrid: începe cu Quick Sort
   - Dacă recursiunea depășește 2·log(n), trece la Heap Sort
   - Pentru partition-uri mici (<16), folosește Insertion Sort
   - Combină viteza Quick Sort cu garanția O(n log n) a Heap Sort

---

## 🔗 Preview Săptămâna Următoare

**Săptămâna 16: Paradigme Algoritmice - Greedy și Backtracking**

Vom studia două abordări fundamentale pentru rezolvarea problemelor:
- Algoritmi **Greedy**: alegeri locale optimale (Activity Selection, Fractional Knapsack)
- **Backtracking**: explorare sistematică cu revenire (N-Queens, Sudoku)
- Comparație cu programarea dinamică
- Aplicații practice: planificare, puzzle-uri, optimizare

Pregătiți-vă recapitulând recursivitatea și gândind la probleme care necesită explorarea tuturor posibilităților.

---

*Acest material face parte din cursul "Algoritmi și Tehnici de Programare" (ATP)*  
*Academia de Studii Economice din București - CSIE*  
*Actualizat: Ianuarie 2026*
