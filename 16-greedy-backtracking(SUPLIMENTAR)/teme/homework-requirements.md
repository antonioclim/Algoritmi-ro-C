# Tema Săptămâna 16: Paradigme Algoritmice - Greedy și Backtracking

## 📋 Informații Generale

- **Termen limită:** Sfârșitul săptămânii 17 (înainte de sesiunea de recapitulare)
- **Punctaj:** 100 puncte (10% din nota finală)
- **Limbaj:** C (standard C11)
- **Compilator:** GCC cu flagurile `-Wall -Wextra -std=c11`

---

## 📝 Tema 1: Problema Rucsacului Fracționar cu Variante (50 puncte)

### Descriere

Implementați un sistem complet pentru rezolvarea problemei rucsacului fracționar (Fractional Knapsack) folosind algoritmul **Greedy**. Sistemul trebuie să suporte multiple variante ale problemei și să ofere analiză detaliată a soluției.

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    PROBLEMA RUCSACULUI FRACȚIONAR                           │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   Input:                                                                    │
│   - N obiecte, fiecare cu (nume, greutate, valoare)                        │
│   - Capacitate maximă W a rucsacului                                       │
│   - Optional: obiecte obligatorii marcate cu 'M'                           │
│                                                                             │
│   Algoritm Greedy:                                                          │
│   1. Calculează raportul value/weight pentru fiecare obiect                │
│   2. Sortează descrescător după raport                                     │
│   3. Adaugă obiecte în ordine până la capacitate                           │
│   4. Dacă ultimul obiect nu încape, ia doar o fracțiune                    │
│                                                                             │
│   Output:                                                                   │
│   - Lista obiectelor selectate cu fracțiunile                              │
│   - Valoarea totală maximă obținută                                        │
│   - Greutatea utilizată / capacitate                                       │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### Structura Principală

```c
typedef struct {
    int id;                     /* ID unic */
    char name[64];              /* Numele obiectului */
    double weight;              /* Greutatea (kg) */
    double value;               /* Valoarea (lei) */
    double ratio;               /* value/weight (calculat) */
    bool mandatory;             /* Dacă e obligatoriu */
} Item;

typedef struct {
    int item_id;                /* ID-ul obiectului */
    double fraction;            /* Fracțiunea luată (0.0-1.0) */
    double weight_taken;        /* Greutatea efectiv luată */
    double value_gained;        /* Valoarea obținută */
} SelectedItem;
```

### Cerințe Funcționale

1. **Citire și validare date** (10p)
   - Citire din fișier cu format: `Nume Greutate Valoare [M]`
   - Validare: greutăți și valori pozitive
   - Suport pentru obiecte obligatorii (marcate cu 'M')

2. **Algoritm Greedy corect** (20p)
   - Calculare corectă raport value/weight
   - Sortare descrescătoare după raport
   - Tratare obiecte obligatorii cu prioritate
   - Selecție greedy cu suport pentru fracționare

3. **Varianta cu obiecte obligatorii** (10p)
   - Obiectele marcate cu 'M' trebuie incluse complet
   - Verificare că obiectele obligatorii încap
   - Eroare descriptivă dacă nu încap

4. **Afișare soluție** (10p)
   - Tabel cu obiectele selectate, fracțiunile și valorile
   - Rezumat: valoare totală, greutate utilizată, % capacitate
   - Vizualizare grafică (bara de progres)

### Exemplu de Utilizare

```
$ ./homework1 items.txt -c 5.0

╔═══════════════════════════════════════════════════════════════╗
║         PROBLEMA RUCSACULUI FRACȚIONAR - GREEDY               ║
╚═══════════════════════════════════════════════════════════════╝

── Obiecte Disponibile ──
┌────┬──────────────┬──────────┬──────────┬──────────┬─────────┐
│ ID │ Nume         │ Greutate │ Valoare  │ Ratio    │ Obligat │
├────┼──────────────┼──────────┼──────────┼──────────┼─────────┤
│  1 │ Laptop       │     2.50 │  2000.00 │   800.00 │         │
│  2 │ Telefon      │     0.30 │   800.00 │  2666.67 │    X    │
│  3 │ Tableta      │     0.80 │   600.00 │   750.00 │         │
└────┴──────────────┴──────────┴──────────┴──────────┴─────────┘

── Soluție (Capacitate: 5.0 kg) ──
┌────┬──────────────┬──────────┬──────────┬──────────┐
│ ID │ Obiect       │ Fracție  │ Greutate │ Valoare  │
├────┼──────────────┼──────────┼──────────┼──────────┤
│  2 │ Telefon      │   100.0% │     0.30 │   800.00 │
│  1 │ Laptop       │   100.0% │     2.50 │  2000.00 │
│  3 │ Tableta      │   100.0% │     0.80 │   600.00 │
├────┴──────────────┴──────────┼──────────┼──────────┤
│ TOTAL                        │     3.60 │  3400.00 │
└──────────────────────────────┴──────────┴──────────┘

Valoare maximă: 3400.00 lei
Capacitate utilizată: 72.0%
```

### Format Fișier Input

```
# Comentariu (ignorat)
# Format: Nume Greutate Valoare [M pentru obligatoriu]
Laptop 2.5 2000
Telefon 0.3 800 M
Tableta 0.8 600
Ceas 0.1 300
Casti 0.2 150
```

### Fișier: `homework1_knapsack.c`

---

## 📝 Tema 2: Rezolvator de Labirint cu Backtracking (50 puncte)

### Descriere

Implementați un rezolvator de labirint folosind algoritmul **Backtracking**. Programul trebuie să găsească drumul de la punctul de start (S) la punctul de destinație (E), evitând pereții și afișând soluția vizual.

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    REZOLVATOR DE LABIRINT                                   │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   Reprezentare:                                                             │
│   ┌───────────────────┐                                                     │
│   │ S . # . . . . # . │    S = Start                                       │
│   │ . # # # . # . # . │    E = End                                         │
│   │ . . . # . # . . . │    # = Perete                                      │
│   │ # # . # . # # # . │    . = Culoar                                      │
│   │ . . . # . . . . . │    * = Drum găsit                                  │
│   │ . # # # # # . # # │                                                     │
│   │ . . . . . . . # . │                                                     │
│   │ # # # # . # # # . │                                                     │
│   │ . . . . . . . . . │                                                     │
│   │ # . # # # # # # E │                                                     │
│   └───────────────────┘                                                     │
│                                                                             │
│   Algoritm Backtracking:                                                    │
│   1. Pornește de la S                                                       │
│   2. Marchează poziția curentă ca vizitată                                 │
│   3. Dacă am ajuns la E, success!                                          │
│   4. Încearcă fiecare direcție (sus, dreapta, jos, stânga)                │
│   5. Dacă direcția e validă și nevizitată, recursează                      │
│   6. Dacă nicio direcție nu duce la E, backtrack                           │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### Structura Principală

```c
typedef struct {
    int row;
    int col;
} Position;

typedef struct {
    Position steps[MAX_STEPS];
    int length;
} Path;

typedef struct {
    char grid[MAX_SIZE][MAX_SIZE];
    char solution[MAX_SIZE][MAX_SIZE];
    bool visited[MAX_SIZE][MAX_SIZE];
    int rows, cols;
    Position start, end;
} Maze;
```

### Cerințe Funcționale

1. **Citire și validare labirint** (10p)
   - Citire din fișier (format: '#' pentru pereți, '.' pentru culoare)
   - Detectare automată dimensiuni
   - Identificare S (start) și E (end)
   - Dacă lipsesc S/E, folosește colțurile

2. **Algoritm Backtracking corect** (20p)
   - Implementare DFS recursivă
   - Marcare/demarcare corectă visited
   - Verificare bounds și pereți
   - Colectare drum (path)

3. **Afișare vizuală** (10p)
   - Afișare labirint cu borduri și culori ANSI
   - Marcare drum găsit cu '*'
   - Legendă clară
   - Secvență de pași ca text

4. **Statistici** (10p)
   - Număr de celule vizitate
   - Număr de backtrack-uri
   - Lungimea drumului găsit
   - Timp de execuție

### Exemplu de Utilizare

```
$ ./homework2 maze.txt

╔═══════════════════════════════════════════════════════════════╗
║           REZOLVATOR DE LABIRINT - BACKTRACKING               ║
╚═══════════════════════════════════════════════════════════════╝

── Labirint Original (10 x 10) ──

    ┌─────────────────────┐
    │ S . # . . . . # . . │
    │ . # # # . # . # . # │
    │ . . . # . # . . . . │
    │ # # . # . # # # # . │
    │ . . . # . . . . . . │
    │ . # # # # # . # # . │
    │ . . . . . . . # . . │
    │ # # # # . # # # . # │
    │ . . . . . . . . . # │
    │ # . # # # # # # . E │
    └─────────────────────┘

Start: (0, 0)
End:   (9, 9)

── Soluție Găsită ──

    ┌─────────────────────┐
    │ S * # . . . . # . . │
    │ . * # # . # . # . # │
    │ . * * # . # . . . . │
    │ # # * # . # # # # . │
    │ * * * # . . . . . . │
    │ * # # # # # . # # . │
    │ * * * * * * * # . . │
    │ # # # # . # # # . # │
    │ . . . . . . . * * # │
    │ # . # # # # # # * E │
    └─────────────────────┘

Drum găsit! Lungime: 21 pași
(0,0) → (1,0) → (2,0) → (2,1) → (2,2) → ... → (9,9)

Statistici:
  - Celule vizitate: 45
  - Backtrack-uri: 12
  - Timp: 0.000234 s
```

### Format Fișier Input

```
# Labirint 10x10
# S = start, E = end, # = perete, . = culoar
S.#....#..
.###.#.#.#
...#.#....
##.#.####.
...#......
.#####.##.
.......#..
####.###.#
........#.
#.######.E
```

### Fișier: `homework2_maze.c`

---

## 📊 Criterii de Evaluare

| Criteriu | Puncte |
|----------|--------|
| **Tema 1: Knapsack** | **50** |
| Corectitudine algoritm greedy | 20 |
| Tratare obiecte obligatorii | 10 |
| Citire și validare input | 10 |
| Afișare soluție și vizualizare | 10 |
| **Tema 2: Maze** | **50** |
| Corectitudine backtracking | 20 |
| Colectare și afișare drum | 10 |
| Citire și validare labirint | 10 |
| Statistici și vizualizare | 10 |

### Penalizări

| Penalizare | Puncte |
|------------|--------|
| Warning-uri la compilare | -10p |
| Memory leaks (Valgrind) | -20p |
| Crash la input valid | -30p |
| Lipsă documentație/comentarii | -10p |
| Plagiat | -100p + raport |

### Bonusuri

| Bonus | Puncte |
|-------|--------|
| Comparație Fractional vs 0/1 Knapsack | +5p |
| BFS pentru cel mai scurt drum (Tema 2) | +5p |
| Găsire toate drumurile posibile | +5p |
| Animație pas cu pas (optional) | +5p |

---

## 📤 Predare

### Structura Arhivei

```
NumePrenume_Tema16.zip
├── homework1_knapsack.c
├── homework2_maze.c
├── items.txt           (fișier test pentru Tema 1)
├── maze.txt            (fișier test pentru Tema 2)
├── Makefile            (opțional)
└── README.txt          (descriere scurtă)
```

### Metoda de Predare

1. Arhivați toate fișierele într-un `.zip`
2. Denumiți arhiva: `NumePrenume_Tema16.zip`
3. Încărcați pe platforma de e-learning
4. Verificați că arhiva se deschide corect

### Verificare Înainte de Predare

```bash
# Compilare
gcc -Wall -Wextra -std=c11 -o homework1 homework1_knapsack.c -lm
gcc -Wall -Wextra -std=c11 -o homework2 homework2_maze.c

# Testare
./homework1 items.txt
./homework2 maze.txt

# Memory leaks
valgrind --leak-check=full ./homework1 items.txt
valgrind --leak-check=full ./homework2 maze.txt
```

---

## 💡 Sfaturi

### Pentru Tema 1 (Knapsack)

1. **Înțelegeți paradigma Greedy**: Alegerea locală optimă (raport maxim value/weight) garantează soluția globală optimă doar pentru varianta fracționară.

2. **Atenție la sortare**: Folosiți `qsort` cu o funcție de comparare care tratează corect obiectele obligatorii.

3. **Tratați cazurile limită**: 
   - Capacitate 0
   - Un singur obiect
   - Toate obiectele obligatorii nu încap

4. **Testați cu date variate**:
   - Obiecte cu greutăți foarte mici și mari
   - Rapoarte egale
   - Fracționare necesară

### Pentru Tema 2 (Maze)

1. **Evitați buclele infinite**: Matricea `visited` este esențială!

2. **Ordinea direcțiilor contează**: Sus, dreapta, jos, stânga poate da drumuri diferite.

3. **Backtracking corect**:
   ```c
   visited[r][c] = true;   // Marchează
   if (solve(...)) return true;
   visited[r][c] = false;  // Demarchează la backtrack
   ```

4. **Debugging vizual**: Afișați labirintul după fiecare pas pentru a înțelege comportamentul.

### Resurse Utile

- **CLRS** - Capitolul 16: Greedy Algorithms
- **Sedgewick** - Capitolul pe Backtracking
- Vizualizare labirint: [VisuAlgo](https://visualgo.net)

---

## 🔗 Legătura cu Materialul din Curs

Această temă verifică înțelegerea:

1. **Paradigma Greedy**:
   - Proprietatea alegerii greedy
   - Substructura optimală
   - Când Greedy garantează optim vs. când nu

2. **Tehnica Backtracking**:
   - Spațiul soluțiilor ca arbore
   - Pruning prin verificări (is_valid)
   - Marcaj/demarcaj pentru stare

3. **Complexitate**:
   - Greedy: O(n log n) - dominat de sortare
   - Backtracking: O(4^(n×m)) worst case, mult mai bine cu pruning

---

*Succes!*

*Academia de Studii Economice din București - CSIE*
*Algoritmi și Tehnici de Programare - Săptămâna 16*
*Ianuarie 2026*
