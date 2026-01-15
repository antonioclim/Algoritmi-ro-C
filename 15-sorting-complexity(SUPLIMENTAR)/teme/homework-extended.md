# Provocări Extinse - Săptămâna 15

## 🚀 Provocări Avansate (Opționale)

Fiecare provocare rezolvată corect: **+10 puncte bonus**

---

## ⭐ Provocarea 1: Introsort - Algoritmul Hibrid Profesional (Dificultate: Medie)

### Descriere

Implementați **Introsort**, algoritmul folosit în `std::sort()` din C++ STL. Introsort combină avantajele a trei algoritmi:
- **Quick Sort** pentru performanță medie
- **Heap Sort** când recursiunea devine prea adâncă (evită O(n²) worst-case)
- **Insertion Sort** pentru sub-array-uri mici (overhead mai mic)

### Cerințe

```c
/**
 * Sortează array-ul folosind Introsort
 * 
 * @param arr Array-ul de sortat
 * @param n Numărul de elemente
 * 
 * Strategie:
 * - Dacă n < 16: folosește Insertion Sort
 * - Dacă depth_limit == 0: folosește Heap Sort
 * - Altfel: Quick Sort cu pivot median-of-three, recursiv
 * 
 * depth_limit inițial = 2 * floor(log2(n))
 */
void introsort(int *arr, size_t n);

/* Funcții auxiliare necesare */
void insertion_sort_range(int *arr, size_t left, size_t right);
void heap_sort_range(int *arr, size_t left, size_t right);
int median_of_three(int *arr, size_t left, size_t right);
void introsort_util(int *arr, size_t left, size_t right, int depth_limit);
```

### Exemplu

```
Input:  [64, 34, 25, 12, 22, 11, 90, 5, 77, 30, ...]  (10000 elemente)

Comportament:
- depth_limit = 2 * log2(10000) ≈ 26
- Quick Sort partiționează în sub-array-uri
- Când depth == 0, switch la Heap Sort
- Când size < 16, switch la Insertion Sort

Output: [5, 11, 12, 22, 25, 30, 34, 64, 77, 90, ...]
Timp: ~O(n log n) garantat (nu există worst-case pătratic)
```

### Puncte Bonus: +10

---

## ⭐ Provocarea 2: Radix Sort MSD pentru Șiruri de Caractere (Dificultate: Medie-Dificilă)

### Descriere

Implementați **Radix Sort MSD (Most Significant Digit first)** pentru sortarea eficientă a unui array de șiruri de caractere. Spre deosebire de LSD, MSD pornește de la caracterul cel mai semnificativ și funcționează ca un Trie implicit.

### Cerințe

```c
/**
 * Sortează array de șiruri folosind Radix Sort MSD
 * 
 * @param strings Array de pointeri către șiruri
 * @param n Numărul de șiruri
 * 
 * Algoritmul:
 * 1. Grupează șirurile după primul caracter (256 bucket-uri)
 * 2. Recursiv, sortează fiecare grup după următorul caracter
 * 3. Tratează cazurile: șiruri de lungimi diferite, '\0' < orice caracter
 */
void radix_sort_msd_strings(char **strings, size_t n);

/* Funcția recursivă */
void msd_radix_sort_util(char **strings, size_t lo, size_t hi, int depth);

/* Counting sort pentru un caracter specific */
void counting_sort_by_char(char **strings, size_t lo, size_t hi, int depth);
```

### Exemplu

```
Input: ["banana", "apple", "cherry", "apricot", "blueberry", "avocado"]

Pasul 1 (primul caracter):
  'a': ["apple", "apricot", "avocado"]
  'b': ["banana", "blueberry"]
  'c': ["cherry"]

Pasul 2 (al doilea caracter pentru grupul 'a'):
  'p': ["apple", "apricot"]
  'v': ["avocado"]

... continuă recursiv

Output: ["apple", "apricot", "avocado", "banana", "blueberry", "cherry"]
```

### Puncte Bonus: +10

---

## ⭐ Provocarea 3: Parallel Merge Sort (Simulat cu Threading) (Dificultate: Dificilă)

### Descriere

Implementați o versiune **paralelizată** a Merge Sort folosind biblioteca POSIX threads (`pthread.h`). Algoritmul trebuie să împartă munca între mai multe thread-uri pentru array-uri mari.

### Cerințe

```c
#include <pthread.h>

#define MAX_THREADS 8

typedef struct {
    int *arr;
    int *temp;
    size_t left;
    size_t right;
    int depth;        /* Controlează când să creăm thread-uri noi */
} ThreadArgs;

/**
 * Parallel Merge Sort
 * 
 * @param arr Array-ul de sortat
 * @param n Numărul de elemente
 * @param num_threads Numărul maxim de thread-uri
 * 
 * Strategia:
 * - La adâncime mică, creează thread-uri pentru jumătăți
 * - La adâncime mare (depth > log2(num_threads)), sortare secvențială
 * - Thread principal așteaptă finalizarea și face merge-ul final
 */
void parallel_merge_sort(int *arr, size_t n, int num_threads);

/* Funcția executată de fiecare thread */
void* thread_merge_sort(void *arg);

/* Merge standard */
void merge(int *arr, int *temp, size_t left, size_t mid, size_t right);
```

### Exemplu

```
Input: Array cu 1.000.000 elemente
Threads: 4

Thread 0: sortează [0, 250000)
Thread 1: sortează [250000, 500000)
Thread 2: sortează [500000, 750000)
Thread 3: sortează [750000, 1000000)

După sincronizare:
- Merge (Thread 0 result, Thread 1 result)
- Merge (Thread 2 result, Thread 3 result)
- Merge final

Speedup: ~2-3x pe mașină quad-core
```

### Compilare

```bash
gcc -Wall -pthread -o parallel_sort parallel_sort.c
```

### Puncte Bonus: +10

---

## ⭐ Provocarea 4: Adaptive Sort cu Detectare Pattern-uri (Dificultate: Medie)

### Descriere

Implementați un algoritm de sortare **adaptiv** care analizează datele de intrare și alege automat strategia optimă. Algoritmul trebuie să detecteze:
- Array deja sortat (sau aproape sortat)
- Array sortat în ordine inversă
- Multe duplicate
- Distribuție uniformă vs. clustered

### Cerințe

```c
typedef enum {
    PATTERN_RANDOM,
    PATTERN_SORTED,
    PATTERN_REVERSE_SORTED,
    PATTERN_NEARLY_SORTED,
    PATTERN_MANY_DUPLICATES,
    PATTERN_FEW_UNIQUE
} DataPattern;

typedef struct {
    DataPattern pattern;
    double sorted_ratio;      /* 0.0 - 1.0 */
    double unique_ratio;      /* unique_count / n */
    int min_value;
    int max_value;
    size_t run_count;         /* Număr de secvențe sortate crescător */
} DataAnalysis;

/**
 * Analizează caracteristicile array-ului
 * Complexitate: O(n) - o singură parcurgere
 */
DataAnalysis analyze_data(const int *arr, size_t n);

/**
 * Alege algoritmul optim bazat pe analiză
 */
typedef enum {
    ALG_INSERTION_SORT,
    ALG_QUICK_SORT,
    ALG_MERGE_SORT,
    ALG_COUNTING_SORT,
    ALG_RADIX_SORT,
    ALG_TIMSORT
} SortAlgorithm;

SortAlgorithm choose_algorithm(const DataAnalysis *analysis, size_t n);

/**
 * Sortează adaptiv
 */
void adaptive_sort(int *arr, size_t n);
```

### Euristici de Decizie

```
┌─────────────────────────────────────────────────────────────┐
│ Condiție                          │ Algoritm Ales           │
├───────────────────────────────────┼─────────────────────────┤
│ sorted_ratio > 0.9                │ Insertion Sort          │
│ sorted_ratio < 0.1 (reverse)      │ Reverse + Insertion     │
│ n < 50                            │ Insertion Sort          │
│ (max - min) < 2*n                 │ Counting Sort           │
│ unique_ratio < 0.1                │ Counting Sort           │
│ Altfel                            │ Quick Sort (median-3)   │
└───────────────────────────────────┴─────────────────────────┘
```

### Puncte Bonus: +10

---

## ⭐ Provocarea 5: External Sort cu Run Generation Optimizată (Dificultate: Dificilă)

### Descriere

Îmbunătățiți algoritmul de **External Merge Sort** din exercițiul 2 prin tehnica de **Replacement Selection** pentru generarea run-urilor. Această metodă produce run-uri de lungime medie 2M (unde M = dimensiunea heap-ului), comparativ cu exact M în metoda standard.

### Cerințe

```c
#define HEAP_SIZE 1000

typedef struct {
    int value;
    int run_number;  /* Rulează în care aparține */
} HeapElement;

/**
 * Generează run-uri folosind Replacement Selection
 * 
 * Algoritmul:
 * 1. Umple heap-ul cu primele M elemente
 * 2. Extrage minimul și scrie-l în run-ul curent
 * 3. Citește următorul element din input
 * 4. Dacă noul element >= ultimul scris: adaugă-l în heap
 *    Altfel: marchează-l pentru run-ul următor
 * 5. Când toate elementele din heap sunt marcate pentru 
 *    run-ul următor, începe un nou run
 * 
 * @param input_file Fișierul de intrare (mare)
 * @param output_prefix Prefixul pentru fișierele de run
 * @return Numărul de run-uri generate
 */
int replacement_selection(const char *input_file, const char *output_prefix);

/**
 * K-way merge pentru fișierele de run
 */
void merge_runs(const char *output_prefix, int num_runs, const char *final_output);

/**
 * Min-heap cu run tracking
 */
void heap_insert(HeapElement *heap, int *size, HeapElement elem);
HeapElement heap_extract_min(HeapElement *heap, int *size);
```

### Exemplu

```
Input: fișier cu 10.000.000 numere
Heap Size: 1000

Metoda Standard:
- Generează ~10.000 run-uri de exact 1000 elemente

Replacement Selection:
- Generează ~5.000 run-uri de lungime medie 2000
- Reduce înjumătățire numărul de faze de merge!

Beneficiu:
- Mai puține I/O operations
- Timp total redus cu 20-40%
```

### Puncte Bonus: +10

---

## 📊 Sistem de Puncte Bonus

| Provocări Completate | Total Bonus |
|----------------------|-------------|
| 1 | +10 puncte |
| 2 | +20 puncte |
| 3 | +30 puncte |
| 4 | +40 puncte |
| Toate 5 | +50 puncte + Badge "🏆 Sorting Master" |

---

## 📝 Criterii de Evaluare pentru Provocări

1. **Corectitudine** (40%)
   - Algoritmul produce rezultate corecte
   - Trece toate testele furnizate

2. **Eficiență** (30%)
   - Complexitatea declarată este respectată
   - Nu există operații redundante

3. **Calitatea Codului** (20%)
   - Cod clar, bine comentat
   - Fără memory leaks

4. **Documentație** (10%)
   - Explicarea abordării
   - Analiza complexității

---

## 💡 Sfaturi

1. **Provocarea 1 (Introsort)**: Studiați implementarea din GCC/Clang pentru inspirație
2. **Provocarea 2 (MSD Radix)**: Atenție la terminatorul de șir '\0'
3. **Provocarea 3 (Parallel)**: Folosiți `pthread_create` și `pthread_join`
4. **Provocarea 4 (Adaptive)**: O analiză bună în O(n) este cheia
5. **Provocarea 5 (External)**: Testați cu fișiere reale de 100MB+

---

## 🔗 Resurse Utile

- [Introsort - Wikipedia](https://en.wikipedia.org/wiki/Introsort)
- [MSD Radix Sort - Algorithms, 4th Edition](https://algs4.cs.princeton.edu/)
- [POSIX Threads Tutorial](https://computing.llnl.gov/tutorials/pthreads/)
- [Polyphase Merge Sort](https://en.wikipedia.org/wiki/Polyphase_merge_sort)

---

*Succes și distracție plăcută cu provocările avansate!*

*Academia de Studii Economice din București - CSIE*
*Algoritmi și Tehnici de Programare*
