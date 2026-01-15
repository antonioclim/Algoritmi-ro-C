# Tema Săptămâna 15: Algoritmi de Sortare Avansați

## 📋 Informații Generale

- **Termen limită:** Sfârșitul săptămânii 16
- **Punctaj:** 100 puncte (10% din nota finală)
- **Limbaj:** C (standard C11)
- **Compilator:** GCC cu flagurile `-Wall -Wextra -std=c11 -O2`

---

## 📝 Tema 1: Sistem de Sortare Multi-Algoritm Adaptiv (50 puncte)

### Descriere

Implementați un sistem inteligent de sortare care selectează automat algoritmul optim în funcție de caracteristicile datelor de intrare. Sistemul trebuie să analizeze input-ul și să aleagă între Quick Sort, Merge Sort, Counting Sort sau Insertion Sort.

### Euristica de Selecție

```c
/* Detectare caracteristici */
typedef struct {
    int n;                  /* Dimensiune */
    int min_val, max_val;   /* Interval valori */
    double sorted_ratio;    /* Procent elemente în ordine */
    bool has_duplicates;    /* Multe duplicate? */
} DataCharacteristics;

/* Reguli de selecție:
 * - n < 20: Insertion Sort
 * - sorted_ratio > 0.8: Insertion Sort (aproape sortat)
 * - (max - min) < 2*n: Counting Sort (interval mic)
 * - Altfel: Quick Sort cu median-of-three
 */
```

### Cerințe Funcționale

1. **Analiză date** (10p)
   - Funcție `analyze_data()` care determină caracteristicile
   - Calculare: min, max, sorted_ratio
   - Detectare duplicate frecvente

2. **Selecție algoritm** (10p)
   - Funcție `select_algorithm()` bazată pe euristici
   - Returnează enum cu algoritmul recomandat
   - Logging al deciziei

3. **Implementare algoritmi** (15p)
   - Quick Sort cu median-of-three
   - Insertion Sort
   - Counting Sort
   - Merge Sort (fallback)

4. **Sortare unificată** (10p)
   - Funcție `adaptive_sort()` care combină toate
   - Interfață uniformă: `void sort(int arr[], int n)`

5. **Statistici** (5p)
   - Timp de execuție
   - Algoritmul selectat
   - Comparație cu alegerea naivă

### Exemplu de Utilizare

```
$ ./homework1 test.txt
Analiză date:
  Dimensiune: 10000
  Interval: [0, 500]
  Sorted ratio: 0.15
  Duplicate ratio: 0.85

Algoritm selectat: Counting Sort (interval mic, multe duplicate)

Rezultate:
  Timp adaptive: 2.3 ms
  Timp Quick Sort: 8.1 ms
  Speedup: 3.5x
```

### Fișier: `homework1_adaptive.c`

---

## 📝 Tema 2: Shell Sort cu Analiza Secvențelor de Gap (50 puncte)

### Descriere

Implementați Shell Sort cu multiple secvențe de gap-uri și analizați empiric performanța fiecăreia. Shell Sort este o generalizare a Insertion Sort care permite interschimbarea elementelor aflate la distanță.

### Secvențe de Gap de Implementat

```c
/* 1. Secvența originală Shell (1959): N/2, N/4, ..., 1 */
int shell_original(int n, int k) {
    return n / (1 << k);  /* n / 2^k */
}

/* 2. Secvența Hibbard (1963): 2^k - 1 -> 1, 3, 7, 15, 31, ... */
int hibbard(int k) {
    return (1 << k) - 1;
}

/* 3. Secvența Sedgewick (1986): 4^k + 3*2^(k-1) + 1 */
int sedgewick(int k) {
    if (k == 0) return 1;
    return (1 << (2*k)) + 3 * (1 << (k-1)) + 1;
}

/* 4. Secvența Ciura (2001): 1, 4, 10, 23, 57, 132, 301, 701, ... */
int ciura[] = {1, 4, 10, 23, 57, 132, 301, 701, 1750};
```

### Cerințe Funcționale

1. **Implementare Shell Sort generic** (15p)
   - Funcție parametrizată cu secvența de gap
   - Contorizare comparații și mutări

2. **Implementare secvențe** (15p)
   - Toate cele 4 secvențe de mai sus
   - Funcție de generare secvență pentru orice n

3. **Benchmark comparativ** (10p)
   - Testare pe array-uri de: 1K, 10K, 100K elemente
   - Tipuri date: random, sortat, inversat
   - Tabel cu rezultate

4. **Analiză** (5p)
   - Determinați cea mai bună secvență pentru fiecare scenariu
   - Explicați de ce (în comentarii sau README)

5. **Vizualizare** (5p)
   - Afișare ASCII a procesului pentru array mic
   - Grafic simplificat al timpilor

### Exemplu de Utilizare

```
$ ./homework2 10000
═══════════════════════════════════════════════════════════════
                SHELL SORT - ANALIZA SECVENȚELOR
═══════════════════════════════════════════════════════════════

Dimensiune: 10000 elemente

┌─────────────┬──────────────┬──────────────┬──────────────┐
│  Secvență   │    Random    │    Sorted    │   Reversed   │
├─────────────┼──────────────┼──────────────┼──────────────┤
│ Shell orig. │   45.2 ms    │    2.1 ms    │   48.7 ms    │
│ Hibbard     │   32.1 ms    │    1.8 ms    │   35.4 ms    │
│ Sedgewick   │   25.8 ms    │    1.5 ms    │   28.2 ms    │
│ Ciura       │   23.4 ms    │    1.4 ms    │   26.1 ms    │
└─────────────┴──────────────┴──────────────┴──────────────┘

Recomandare: Secvența Ciura pentru uz general
```

### Fișier: `homework2_shellsort.c`

---

## 📊 Criterii de Evaluare

| Criteriu | Puncte |
|----------|--------|
| Corectitudine funcțională | 40 |
| Implementare euristici / secvențe | 25 |
| Calitatea analizei empirice | 15 |
| Calitatea codului | 10 |
| Fără warning-uri la compilare | 10 |

### Penalizări

- **-10p**: Warning-uri la compilare
- **-20p**: Memory leaks (verificate cu Valgrind)
- **-30p**: Rezultate de sortare incorecte
- **-50p**: Plagiat

---

## 📤 Predare

### Format

- Fișierele sursă: `homework1_adaptive.c`, `homework2_shellsort.c`
- Fișier README.md cu:
  - Instrucțiuni de compilare și rulare
  - Explicația euristicilor (Tema 1)
  - Analiza rezultatelor (Tema 2)

### Verificare înainte de predare

```bash
# Compilare fără erori
gcc -Wall -Wextra -std=c11 -O2 -o homework1 homework1_adaptive.c
gcc -Wall -Wextra -std=c11 -O2 -o homework2 homework2_shellsort.c -lm

# Rulare teste
./homework1 --test
./homework2 --test

# Verificare memory leaks
valgrind --leak-check=full ./homework1 sample.txt
valgrind --leak-check=full ./homework2 1000
```

---

## 💡 Sfaturi

1. **Pentru Tema 1:**
   - Testați euristicile pe diverse tipuri de date
   - Nu uitați că analiza datelor are și ea un cost O(n)
   - Threshold-urile exacte pot fi ajustate empiric

2. **Pentru Tema 2:**
   - Începeți cu secvența originală, apoi adăugați celelalte
   - Folosiți `clock()` pentru măsurători precise
   - Faceți media pe mai multe rulări pentru rezultate stabile

3. **General:**
   - Comentați codul pentru a explica deciziile
   - Gestionați corect alocările de memorie
   - Testați pe diverse dimensiuni de input

---

*Succes!*

*Academia de Studii Economice din București - CSIE*
*Algoritmi și Tehnici de Programare*
