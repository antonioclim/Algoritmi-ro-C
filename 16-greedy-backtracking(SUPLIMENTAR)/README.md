# Săptămâna 16: Paradigme Algoritmice - Greedy și Backtracking

## 🎯 Obiective de Învățare

La finalul acestei săptămâni, veți fi capabili să:

1. **Rețineți** (Remember): Definiți caracteristicile problemelor rezolvabile cu Greedy versus Backtracking; enumerați proprietatea alegerii greedy și substructura optimală
2. **Înțelegeți** (Understand): Explicați de ce Greedy nu găsește mereu soluția optimă și identificați condițiile în care este garantat să funcționeze
3. **Aplicați** (Apply): Implementați în C algoritmi Greedy (Fractional Knapsack, Activity Selection) și Backtracking (N-Queens, Sudoku Solver)
4. **Analizați** (Analyse): Comparați complexitatea timp/spațiu pentru diferite probleme și evaluați efectul pruning-ului în backtracking
5. **Evaluați** (Evaluate): Determinați dacă o problemă nouă este rezolvabilă cu Greedy, Backtracking sau necesită programare dinamică
6. **Creați** (Create): Proiectați soluții complete pentru probleme combinatoriale complexe, integrând euristic optime de tăiere a spațiului de căutare

---

## 📜 Context Istoric

### Evoluția Paradigmelor Algoritmice

Dezvoltarea metodelor sistematice de rezolvare a problemelor de optimizare reprezintă unul dintre pilonii fundamentali ai informaticii teoretice. Paradigma **Greedy** (lacomă) își are originile în metodele de optimizare liniară din anii 1940-1950, în timp ce **Backtracking** a fost formalizat în contextul inteligenței artificiale și al rezolvării problemelor combinatoriale în anii 1960.

Conceptul de "alegere lacomă" a apărut natural în contextul programării liniare și al metodei simplex, dezvoltată de **George Dantzig** în 1947. Deși metoda simplex nu este un algoritm greedy pur, principiul de îmbunătățire iterativă prin alegeri locale optime a influențat profund gândirea algoritmică ulterioară.

Termenul "backtracking" a fost formalizat de **D.H. Lehmer** în anii 1950 în contextul generării permutărilor, dar tehnica a fost folosită intuitiv încă din secolul al XIX-lea pentru probleme precum cele opt regine (N-Queens). **Robert W. Floyd** și **J. Nievergelt** au contribuit la formalizarea teoretică în anii 1960-1970.

---

### Figura Cheie: George Bernard Dantzig (1914-2005)

```
    ┌─────────────────────────────────────────────────────────────┐
    │  George Bernard Dantzig                                      │
    │  "Părintele Programării Liniare"                             │
    │                                                              │
    │  • PhD în Matematică, UC Berkeley (1946)                     │
    │  • Inventatorul metodei Simplex (1947)                       │
    │  • Profesor Stanford University (1966-2005)                  │
    │  • National Medal of Science (1975)                          │
    │  • Contribuții la optimizare, cercetare operațională         │
    └─────────────────────────────────────────────────────────────┘
```

**Contribuție Majoră**: Dantzig a dezvoltat metoda simplex pentru programarea liniară, care a revoluționat optimizarea în industrie, logistică și economie. Principiile sale de optimizare iterativă au influențat direct gândirea algoritmică în paradigma Greedy.

> *"Linear programming can be viewed as part of a great revolutionary development which has given mankind the ability to state general goals and to lay out a path of detailed decisions to take in order to 'best' achieve its goals."*  
> — George Dantzig, 1963

---

### Figura Cheie: Robert W. Floyd (1936-2001)

```
    ┌─────────────────────────────────────────────────────────────┐
    │  Robert W. Floyd                                             │
    │  Pionier al Analizei Algoritmilor                            │
    │                                                              │
    │  • Autodidact în informatică (fără PhD formal)               │
    │  • Profesor Stanford University (1968-2001)                  │
    │  • Premiul Turing (1978)                                     │
    │  • Algoritm Floyd-Warshall, Heap Sort optimizat              │
    │  • Contribuții la verificarea programelor                    │
    └─────────────────────────────────────────────────────────────┘
```

**Contribuție Majoră**: Floyd a formalizat tehnicile de backtracking și a contribuit la analiza riguroasă a complexității algoritmilor. Algoritmul Floyd-Warshall pentru drumuri minime și contribuțiile sale la construcția heap-urilor în timp liniar rămân fundamentale.

> *"The establishment of formal standards for proofs about programs... leads to more reliable programs."*  
> — Robert W. Floyd, Turing Award Lecture, 1978

---

## 📚 Fundamente Teoretice

### PARTEA I: PARADIGMA GREEDY

### 1. Principiul Alegerii Locale Optime

Un algoritm **Greedy** (lacom) construiește soluția pas cu pas, făcând la fiecare etapă **alegerea care pare optim în acel moment**, fără a reconsidera deciziile anterioare.

```
    ┌─────────────────────────────────────────────────────────────────┐
    │  PRINCIPIUL GREEDY                                              │
    ├─────────────────────────────────────────────────────────────────┤
    │                                                                 │
    │   Pentru problema de optimizare P:                              │
    │                                                                 │
    │   1. DESCOMPUNE problema în subprobleme                         │
    │   2. La fiecare pas, ALEGE opțiunea LOCAL OPTIMĂ                │
    │   3. NICIODATĂ nu reconsidera alegerile făcute                  │
    │   4. SPERĂ că alegerile locale duc la optim global              │
    │                                                                 │
    │   Complexitate tipică: O(n log n) sau O(n)                      │
    └─────────────────────────────────────────────────────────────────┘
```

### 2. Condiții de Corectitudine pentru Greedy

Pentru ca un algoritm Greedy să producă soluția **optimă globală**, problema trebuie să satisfacă două proprietăți:

```
    ┌───────────────────────────────────────────────────────────────────┐
    │  PROPRIETATEA ALEGERII GREEDY (Greedy Choice Property)           │
    ├───────────────────────────────────────────────────────────────────┤
    │                                                                   │
    │  Există o soluție optimă care ÎNCEPE cu alegerea greedy.          │
    │                                                                   │
    │  Formal: Dacă G este alegerea greedy și O* este o soluție         │
    │  optimă care nu include G, atunci putem construi O' ≥ O*          │
    │  care INCLUDE G.                                                  │
    └───────────────────────────────────────────────────────────────────┘

    ┌───────────────────────────────────────────────────────────────────┐
    │  SUBSTRUCTURA OPTIMALĂ (Optimal Substructure)                     │
    ├───────────────────────────────────────────────────────────────────┤
    │                                                                   │
    │  Soluția optimă a problemei CONȚINE soluțiile optime              │
    │  ale subproblemelor sale.                                         │
    │                                                                   │
    │  Formal: Dacă S* este soluția optimă pentru P, iar                │
    │  P = P₁ ∪ P₂, atunci S* = S₁* ∪ S₂* unde Sᵢ* sunt                │
    │  soluțiile optime pentru Pᵢ.                                      │
    └───────────────────────────────────────────────────────────────────┘
```

### 3. Contraexemplu: Când Greedy Eșuează

**Problema Restului (Coin Change)** - varianta generală:

```
    Monede disponibile: {1, 3, 4}
    Suma de dat rest: 6
    
    GREEDY (alege cea mai mare monedă posibilă):
        6 - 4 = 2
        2 - 1 = 1  
        1 - 1 = 0
        Total: 3 monede {4, 1, 1}
    
    OPTIM:
        6 - 3 = 3
        3 - 3 = 0
        Total: 2 monede {3, 3}  ← Mai bun!
    
    Concluzie: Greedy NU garantează optim pentru această problemă.
```

---

### 4. Activity Selection Problem

Una dintre problemele clasice unde Greedy **garantează** soluția optimă.

**Problema**: Având n activități cu timpi de start și finish, selectați numărul maxim de activități compatibile (care nu se suprapun).

```
    ┌──────────────────────────────────────────────────────────────────┐
    │  ACTIVITY SELECTION - EXEMPLU                                    │
    ├──────────────────────────────────────────────────────────────────┤
    │                                                                  │
    │  Activități: A1(1,4), A2(3,5), A3(0,6), A4(5,7), A5(3,9),       │
    │              A6(5,9), A7(6,10), A8(8,11), A9(8,12), A10(2,14)   │
    │                                                                  │
    │  Timeline:                                                       │
    │  0  1  2  3  4  5  6  7  8  9  10 11 12 13 14                   │
    │  |--|--|--|--|--|--|--|--|--|--|--|--|--|--|                     │
    │     [====A1====]                                                 │
    │        [====A2====]                                              │
    │  [========A3========]                                            │
    │              [==A4==]                                            │
    │        [========A5========]                                      │
    │              [======A6======]                                    │
    │                 [======A7======]                                 │
    │                       [====A8====]                               │
    │                       [======A9======]                           │
    │     [================A10================]                        │
    │                                                                  │
    │  SOLUȚIE GREEDY (sortare după finish time):                      │
    │  Selectăm: A1(1,4) → A4(5,7) → A8(8,11) = 3 activități          │
    └──────────────────────────────────────────────────────────────────┘
```

**Implementare în C**:

```c
typedef struct {
    int id;
    int start;
    int finish;
    char name[32];
} Activity;

/* Funcție de comparare pentru qsort - după timpul de terminare */
int compare_by_finish(const void *a, const void *b) {
    return ((Activity*)a)->finish - ((Activity*)b)->finish;
}

/**
 * Selectează activitățile compatibile folosind algoritmul Greedy.
 * 
 * @param activities Array de activități
 * @param n Numărul de activități
 * @param selected Array pentru rezultat (alocat de apelant)
 * @return Numărul de activități selectate
 *
 * Complexitate: O(n log n) pentru sortare + O(n) pentru selecție
 */
int activity_selection(Activity *activities, int n, int *selected) {
    /* Sortare după timpul de finish */
    qsort(activities, n, sizeof(Activity), compare_by_finish);
    
    int count = 0;
    int last_finish = 0;
    
    for (int i = 0; i < n; i++) {
        /* Alegerea greedy: prima activitate compatibilă */
        if (activities[i].start >= last_finish) {
            selected[count++] = activities[i].id;
            last_finish = activities[i].finish;
        }
    }
    
    return count;
}
```

**Demonstrația corectitudinii** (schiță):

1. **Alegerea Greedy**: Activitatea cu cel mai mic finish time poate fi întotdeauna inclusă într-o soluție optimă (lăsând maximum de timp pentru restul activităților).

2. **Substructura Optimală**: După selectarea primei activități, problema se reduce la selectarea activităților care încep după finish-ul ei - o subproblemă de același tip.

---

### 5. Fractional Knapsack Problem

**Problema**: Dat un rucsac cu capacitate W și n obiecte cu greutăți și valori, selectați obiecte (sau fracțiuni din ele) pentru a maximiza valoarea totală.

```
    ┌───────────────────────────────────────────────────────────────┐
    │  FRACTIONAL KNAPSACK - EXEMPLU                                │
    ├───────────────────────────────────────────────────────────────┤
    │                                                               │
    │  Capacitate rucsac: W = 50 kg                                 │
    │                                                               │
    │  Obiecte:                                                     │
    │  ┌────────┬──────────┬─────────┬──────────────┐              │
    │  │ Obiect │ Greutate │ Valoare │ Valoare/kg   │              │
    │  ├────────┼──────────┼─────────┼──────────────┤              │
    │  │   A    │   10 kg  │  60 lei │  6.0 lei/kg  │              │
    │  │   B    │   20 kg  │ 100 lei │  5.0 lei/kg  │              │
    │  │   C    │   30 kg  │ 120 lei │  4.0 lei/kg  │              │
    │  └────────┴──────────┴─────────┴──────────────┘              │
    │                                                               │
    │  GREEDY (sortare după value/weight ratio):                    │
    │    1. Luăm A complet:   10 kg, +60 lei  (rămas: 40 kg)       │
    │    2. Luăm B complet:   20 kg, +100 lei (rămas: 20 kg)       │
    │    3. Luăm 2/3 din C:   20 kg, +80 lei  (rămas: 0 kg)        │
    │                                                               │
    │  TOTAL: 240 lei (optim!)                                      │
    └───────────────────────────────────────────────────────────────┘
```

**Implementare în C**:

```c
typedef struct {
    int id;
    double weight;
    double value;
    double ratio;       /* value/weight */
    double fraction;    /* Fracțiunea luată (0.0 - 1.0) */
} KnapsackItem;

int compare_by_ratio(const void *a, const void *b) {
    double diff = ((KnapsackItem*)b)->ratio - ((KnapsackItem*)a)->ratio;
    return (diff > 0) - (diff < 0);  /* Descrescător */
}

/**
 * Rezolvă Fractional Knapsack folosind Greedy.
 *
 * @param items Array de obiecte
 * @param n Numărul de obiecte
 * @param capacity Capacitatea rucsacului
 * @return Valoarea maximă obținută
 *
 * Complexitate: O(n log n)
 */
double fractional_knapsack(KnapsackItem *items, int n, double capacity) {
    /* Calculează ratios și sortează descrescător */
    for (int i = 0; i < n; i++) {
        items[i].ratio = items[i].value / items[i].weight;
        items[i].fraction = 0.0;
    }
    qsort(items, n, sizeof(KnapsackItem), compare_by_ratio);
    
    double total_value = 0.0;
    double remaining = capacity;
    
    for (int i = 0; i < n && remaining > 0; i++) {
        if (items[i].weight <= remaining) {
            /* Luăm obiectul complet */
            items[i].fraction = 1.0;
            total_value += items[i].value;
            remaining -= items[i].weight;
        } else {
            /* Luăm o fracțiune */
            items[i].fraction = remaining / items[i].weight;
            total_value += items[i].value * items[i].fraction;
            remaining = 0;
        }
    }
    
    return total_value;
}
```

**Notă**: Pentru varianta **0/1 Knapsack** (obiectele nu pot fi fragmentate), Greedy **NU** garantează soluția optimă - se folosește Programare Dinamică.

---

### PARTEA II: PARADIGMA BACKTRACKING

### 6. Explorarea Sistematică a Spațiului Soluțiilor

**Backtracking** este o tehnică de căutare exhaustivă care explorează sistematic toate soluțiile posibile, abandonând ramurile care nu pot duce la o soluție validă.

```
    ┌─────────────────────────────────────────────────────────────────┐
    │  PRINCIPIUL BACKTRACKING                                        │
    ├─────────────────────────────────────────────────────────────────┤
    │                                                                 │
    │  Spațiul soluțiilor = ARBORE DE DECIZIE                         │
    │                                                                 │
    │                        [START]                                  │
    │                       /   |   \                                 │
    │                      /    |    \                                │
    │                   [D1]  [D2]  [D3]    ← Nivel 1: Prima decizie  │
    │                  / | \    |     |                               │
    │                 ...       X     ...   ← X = Tăiere (pruning)    │
    │                                                                 │
    │   1. CONSTRUIEȘTE soluția parțială pas cu pas                   │
    │   2. La fiecare pas, verifică dacă soluția e "promițătoare"     │
    │   3. Dacă DA → continuă în adâncime                             │
    │   4. Dacă NU → ÎNTOARCE-TE și încearcă altă ramură (backtrack)  │
    │   5. Când găsești soluție completă → salvează/afișează          │
    └─────────────────────────────────────────────────────────────────┘
```

**Template General Backtracking**:

```c
/**
 * Template general pentru backtracking.
 *
 * @param solution Soluția parțială curentă
 * @param step Pasul curent (adâncimea în arbore)
 */
void backtrack(Solution *solution, int step) {
    /* Caz de bază: soluție completă */
    if (is_complete(solution, step)) {
        process_solution(solution);
        return;
    }
    
    /* Generează toate opțiunile pentru pasul curent */
    for (each option in get_candidates(solution, step)) {
        /* Verifică dacă opțiunea e promițătoare */
        if (is_promising(solution, step, option)) {
            /* Efectuează alegerea */
            make_choice(solution, step, option);
            
            /* Explorează recursiv */
            backtrack(solution, step + 1);
            
            /* BACKTRACK: anulează alegerea */
            undo_choice(solution, step, option);
        }
    }
}
```

---

### 7. Problema N-Queens (N Regine)

**Problema**: Plasați N regine pe o tablă de șah N×N astfel încât nicio regină să nu atace altă regină (pe linie, coloană sau diagonală).

```
    ┌─────────────────────────────────────────────────────────────────┐
    │  N-QUEENS: SOLUȚIE PENTRU N=8                                   │
    ├─────────────────────────────────────────────────────────────────┤
    │                                                                 │
    │     0   1   2   3   4   5   6   7                               │
    │   ┌───┬───┬───┬───┬───┬───┬───┬───┐                            │
    │ 0 │   │   │   │   │ Q │   │   │   │  Poziții: col[row]         │
    │   ├───┼───┼───┼───┼───┼───┼───┼───┤                            │
    │ 1 │   │   │   │   │   │   │ Q │   │  col[0] = 4                │
    │   ├───┼───┼───┼───┼───┼───┼───┼───┤  col[1] = 6                │
    │ 2 │   │ Q │   │   │   │   │   │   │  col[2] = 1                │
    │   ├───┼───┼───┼───┼───┼───┼───┼───┤  col[3] = 3                │
    │ 3 │   │   │   │ Q │   │   │   │   │  col[4] = 5                │
    │   ├───┼───┼───┼───┼───┼───┼───┼───┤  col[5] = 7                │
    │ 4 │   │   │   │   │   │ Q │   │   │  col[6] = 2                │
    │   ├───┼───┼───┼───┼───┼───┼───┼───┤  col[7] = 0                │
    │ 5 │   │   │   │   │   │   │   │ Q │                            │
    │   ├───┼───┼───┼───┼───┼───┼───┼───┤  Verificare atacuri:       │
    │ 6 │   │   │ Q │   │   │   │   │   │  • Linie: implicit OK      │
    │   ├───┼───┼───┼───┼───┼───┼───┼───┤    (o regină/rând)        │
    │ 7 │ Q │   │   │   │   │   │   │   │  • Coloană: col[i] ≠ col[j]│
    │   └───┴───┴───┴───┴───┴───┴───┴───┘  • Diagonală:              │
    │                                        |i-j| ≠ |col[i]-col[j]| │
    └─────────────────────────────────────────────────────────────────┘
```

**Implementare în C**:

```c
#define MAX_N 20

/**
 * Verifică dacă regina de pe rândul 'row' este sigură.
 *
 * @param col Array cu pozițiile reginelor (col[i] = coloana reginei de pe rândul i)
 * @param row Rândul curent de verificat
 * @return true dacă poziția e sigură, false altfel
 */
bool is_safe(int col[], int row) {
    for (int i = 0; i < row; i++) {
        /* Verifică aceeași coloană */
        if (col[i] == col[row]) return false;
        
        /* Verifică diagonale */
        if (abs(row - i) == abs(col[row] - col[i])) return false;
    }
    return true;
}

/**
 * Rezolvă N-Queens folosind backtracking.
 *
 * @param col Array pentru soluție
 * @param row Rândul curent
 * @param n Dimensiunea tablei
 * @param count Pointer la contorul de soluții
 */
void solve_n_queens(int col[], int row, int n, int *count) {
    if (row == n) {
        /* Soluție completă găsită */
        (*count)++;
        print_solution(col, n);
        return;
    }
    
    /* Încearcă fiecare coloană pentru rândul curent */
    for (int c = 0; c < n; c++) {
        col[row] = c;
        
        if (is_safe(col, row)) {
            solve_n_queens(col, row + 1, n, count);
        }
        /* Backtrack implicit: col[row] va fi suprascris */
    }
}
```

**Numărul de soluții pentru diferite N**:

| N | Soluții | Soluții unice (fără rotații/reflecții) |
|---|---------|----------------------------------------|
| 4 | 2 | 1 |
| 5 | 10 | 2 |
| 6 | 4 | 1 |
| 7 | 40 | 6 |
| 8 | 92 | 12 |
| 9 | 352 | 46 |
| 10 | 724 | 92 |

---

### 8. Sudoku Solver

**Problema**: Completați un grid 9×9 astfel încât fiecare rând, coloană și subgrid 3×3 să conțină cifrele 1-9 fără repetiție.

```
    ┌─────────────────────────────────────────────────────────────────┐
    │  SUDOKU - REPREZENTARE ȘI CONSTRÂNGERI                          │
    ├─────────────────────────────────────────────────────────────────┤
    │                                                                 │
    │  Input (0 = celulă goală):           Soluție:                   │
    │                                                                 │
    │  ┌───────┬───────┬───────┐          ┌───────┬───────┬───────┐  │
    │  │ 5 3 0 │ 0 7 0 │ 0 0 0 │          │ 5 3 4 │ 6 7 8 │ 9 1 2 │  │
    │  │ 6 0 0 │ 1 9 5 │ 0 0 0 │          │ 6 7 2 │ 1 9 5 │ 3 4 8 │  │
    │  │ 0 9 8 │ 0 0 0 │ 0 6 0 │          │ 1 9 8 │ 3 4 2 │ 5 6 7 │  │
    │  ├───────┼───────┼───────┤          ├───────┼───────┼───────┤  │
    │  │ 8 0 0 │ 0 6 0 │ 0 0 3 │          │ 8 5 9 │ 7 6 1 │ 4 2 3 │  │
    │  │ 4 0 0 │ 8 0 3 │ 0 0 1 │          │ 4 2 6 │ 8 5 3 │ 7 9 1 │  │
    │  │ 7 0 0 │ 0 2 0 │ 0 0 6 │          │ 7 1 3 │ 9 2 4 │ 8 5 6 │  │
    │  ├───────┼───────┼───────┤          ├───────┼───────┼───────┤  │
    │  │ 0 6 0 │ 0 0 0 │ 2 8 0 │          │ 9 6 1 │ 5 3 7 │ 2 8 4 │  │
    │  │ 0 0 0 │ 4 1 9 │ 0 0 5 │          │ 2 8 7 │ 4 1 9 │ 6 3 5 │  │
    │  │ 0 0 0 │ 0 8 0 │ 0 7 9 │          │ 3 4 5 │ 2 8 6 │ 1 7 9 │  │
    │  └───────┴───────┴───────┘          └───────┴───────┴───────┘  │
    │                                                                 │
    │  Constrângeri pentru cifra d în celula (r,c):                   │
    │  1. d nu apare în rândul r                                      │
    │  2. d nu apare în coloana c                                     │
    │  3. d nu apare în subgrid-ul 3×3 corespunzător                  │
    └─────────────────────────────────────────────────────────────────┘
```

**Implementare în C**:

```c
#define SIZE 9

/**
 * Verifică dacă cifra num poate fi plasată în celula (row, col).
 */
bool is_valid_sudoku(int grid[SIZE][SIZE], int row, int col, int num) {
    /* Verifică rândul */
    for (int c = 0; c < SIZE; c++) {
        if (grid[row][c] == num) return false;
    }
    
    /* Verifică coloana */
    for (int r = 0; r < SIZE; r++) {
        if (grid[r][col] == num) return false;
    }
    
    /* Verifică subgrid-ul 3×3 */
    int box_row = (row / 3) * 3;
    int box_col = (col / 3) * 3;
    for (int r = box_row; r < box_row + 3; r++) {
        for (int c = box_col; c < box_col + 3; c++) {
            if (grid[r][c] == num) return false;
        }
    }
    
    return true;
}

/**
 * Găsește prima celulă goală.
 *
 * @return true dacă există celulă goală (coordonatele în *row, *col)
 */
bool find_empty(int grid[SIZE][SIZE], int *row, int *col) {
    for (*row = 0; *row < SIZE; (*row)++) {
        for (*col = 0; *col < SIZE; (*col)++) {
            if (grid[*row][*col] == 0) return true;
        }
    }
    return false;
}

/**
 * Rezolvă Sudoku folosind backtracking.
 *
 * @return true dacă s-a găsit o soluție
 */
bool solve_sudoku(int grid[SIZE][SIZE]) {
    int row, col;
    
    /* Caz de bază: nu mai sunt celule goale */
    if (!find_empty(grid, &row, &col)) {
        return true;  /* Soluție completă */
    }
    
    /* Încearcă cifrele 1-9 */
    for (int num = 1; num <= 9; num++) {
        if (is_valid_sudoku(grid, row, col, num)) {
            grid[row][col] = num;  /* Plasează cifra */
            
            if (solve_sudoku(grid)) {
                return true;  /* Soluție găsită */
            }
            
            grid[row][col] = 0;  /* BACKTRACK */
        }
    }
    
    return false;  /* Trigger backtrack la nivelul anterior */
}
```

---

### 9. Generarea Combinatorică

Backtracking este tehnica fundamentală pentru generarea sistematică a structurilor combinatorice.

```
    ┌─────────────────────────────────────────────────────────────────┐
    │  GENERARE COMBINATORICĂ                                          │
    ├─────────────────────────────────────────────────────────────────┤
    │                                                                 │
    │  PERMUTĂRI P(n): n! aranjamente ale tuturor elementelor         │
    │  P(3) = {123, 132, 213, 231, 312, 321}                         │
    │                                                                 │
    │  COMBINĂRI C(n,k): selecții de k elemente din n                 │
    │  C(4,2) = {12, 13, 14, 23, 24, 34}                             │
    │                                                                 │
    │  SUBMULȚIMI: toate submulțimile unei mulțimi (2^n)              │
    │  S({1,2}) = {∅, {1}, {2}, {1,2}}                               │
    │                                                                 │
    │  PRODUS CARTEZIAN: combinații din mai multe mulțimi             │
    │  {a,b} × {1,2} = {(a,1), (a,2), (b,1), (b,2)}                  │
    └─────────────────────────────────────────────────────────────────┘
```

**Implementare Permutări**:

```c
/**
 * Generează toate permutările unui array.
 */
void generate_permutations(int arr[], int start, int n) {
    if (start == n - 1) {
        print_array(arr, n);  /* Permutare completă */
        return;
    }
    
    for (int i = start; i < n; i++) {
        swap(&arr[start], &arr[i]);      /* Alege elementul pentru poziția start */
        generate_permutations(arr, start + 1, n);  /* Recursia */
        swap(&arr[start], &arr[i]);      /* BACKTRACK */
    }
}
```

**Implementare Combinări**:

```c
/**
 * Generează toate combinările de k elemente din n.
 */
void generate_combinations(int arr[], int n, int k, int start, 
                           int combo[], int combo_idx) {
    if (combo_idx == k) {
        print_array(combo, k);  /* Combinare completă */
        return;
    }
    
    for (int i = start; i < n; i++) {
        combo[combo_idx] = arr[i];
        generate_combinations(arr, n, k, i + 1, combo, combo_idx + 1);
        /* Backtrack implicit prin neschimbarea combo[] */
    }
}
```

---

### 10. Comparație Greedy vs. Backtracking vs. Programare Dinamică

```
    ┌─────────────────────────────────────────────────────────────────────────┐
    │               COMPARAȚIE PARADIGME ALGORITMICE                          │
    ├───────────────┬───────────────────┬─────────────────┬───────────────────┤
    │   Criteriu    │      GREEDY       │   BACKTRACKING  │ PROGRAMARE DIN.   │
    ├───────────────┼───────────────────┼─────────────────┼───────────────────┤
    │ Strategie     │ Alegere locală    │ Explorare       │ Subprobleme       │
    │               │ optimă            │ exhaustivă      │ suprapuse         │
    ├───────────────┼───────────────────┼─────────────────┼───────────────────┤
    │ Garantează    │ NU (doar dacă     │ DA (dacă există)│ DA                │
    │ optim global? │ prop. greedy)     │                 │                   │
    ├───────────────┼───────────────────┼─────────────────┼───────────────────┤
    │ Complexitate  │ O(n log n) tipic  │ O(k^n) worst    │ O(n×W) tipic      │
    │ tipică        │                   │                 │ (pseudopolin.)    │
    ├───────────────┼───────────────────┼─────────────────┼───────────────────┤
    │ Spațiu        │ O(1) - O(n)       │ O(n) recursie   │ O(n) - O(n²)      │
    ├───────────────┼───────────────────┼─────────────────┼───────────────────┤
    │ Când să       │ Substructură opt. │ Toate soluțiile │ Subprobleme       │
    │ folosești     │ + prop. greedy    │ / Constrângeri  │ se repetă         │
    ├───────────────┼───────────────────┼─────────────────┼───────────────────┤
    │ Exemple       │ Activity Select.  │ N-Queens        │ 0/1 Knapsack      │
    │               │ Fract. Knapsack   │ Sudoku          │ LCS, Edit Dist.   │
    │               │ Huffman, Dijkstra │ Graph Coloring  │ Floyd-Warshall    │
    └───────────────┴───────────────────┴─────────────────┴───────────────────┘
```

---

## 🏭 Aplicații Industriale

### 1. Planificarea Resurselor (Resource Scheduling)

Algoritmii greedy sunt folosiți extensiv în sistemele de planificare:

```c
/*
 * Exemplu simplificat: Planificarea task-urilor pe server
 * Strategia Greedy: Shortest Job First (SJF)
 */
typedef struct {
    int task_id;
    int duration;
    int deadline;
} ServerTask;

int compare_sjf(const void *a, const void *b) {
    return ((ServerTask*)a)->duration - ((ServerTask*)b)->duration;
}

void schedule_tasks_greedy(ServerTask *tasks, int n) {
    qsort(tasks, n, sizeof(ServerTask), compare_sjf);
    
    int current_time = 0;
    for (int i = 0; i < n; i++) {
        printf("Task %d: start=%d, end=%d\n", 
               tasks[i].task_id, current_time, 
               current_time + tasks[i].duration);
        current_time += tasks[i].duration;
    }
}
```

### 2. Satisfacerea Constrângerilor (Constraint Satisfaction)

Backtracking este fundamental în sistemele expert și AI:

```c
/*
 * Exemplu: Colorarea grafului pentru scheduling
 * Aplicație: Alocare registre în compilatoare,
 *           Alocare frecvențe radio
 */
#define MAX_COLORS 4

bool can_color(int graph[MAX_V][MAX_V], int colors[], int v, int c, int n) {
    for (int i = 0; i < n; i++) {
        if (graph[v][i] && colors[i] == c) return false;
    }
    return true;
}

bool graph_coloring(int graph[MAX_V][MAX_V], int n, int num_colors,
                    int colors[], int v) {
    if (v == n) return true;  /* Toate nodurile colorate */
    
    for (int c = 1; c <= num_colors; c++) {
        if (can_color(graph, colors, v, c, n)) {
            colors[v] = c;
            if (graph_coloring(graph, n, num_colors, colors, v + 1))
                return true;
            colors[v] = 0;  /* Backtrack */
        }
    }
    return false;
}
```

### 3. Compresie de Date - Huffman Coding

Algoritmul Huffman (greedy) este baza multor formate de compresie:

```
    HUFFMAN CODING - PRINCIPIU
    
    Text: "ABRACADABRA"
    Frecvențe: A=5, B=2, R=2, C=1, D=1
    
    Arbore Huffman:
                  (11)
                 /    \
              (6)      A(5)
             /    \
          (3)     (3)
         /   \   /   \
       C(1) D(1) B(2) R(2)
    
    Coduri: A=1, B=001, R=000, C=010, D=011
    
    Original: 11 caractere × 8 biți = 88 biți
    Comprimat: 5×1 + 2×3 + 2×3 + 1×3 + 1×3 = 23 biți
    Raport compresie: 74%
```

---

## 💻 Exerciții de Laborator

### Exercițiul 1: Planificator de Activități Greedy

**Obiectiv**: Implementați un sistem complet de planificare a activităților folosind algoritmul Activity Selection.

**Cerințe**:
1. Citiți activitățile din fișierul `data/activities.txt`
2. Implementați sortarea după timpul de terminare
3. Aplicați algoritmul greedy pentru selecție
4. Afișați activitățile selectate cu statistici complete
5. Bonus: Implementați varianta cu profit maxim (Weighted Job Scheduling)

**Format fișier input**:
```
5
Curs 08:00 10:00
Lab 09:00 11:00
Seminar 10:30 12:00
Pranz 12:00 13:00
Sport 11:00 14:00
```

**Fișiere**: `src/exercise1.c`, `data/activities.txt`

---

### Exercițiul 2: Sudoku Solver cu Statistici

**Obiectiv**: Implementați un solver de Sudoku cu afișare pas-cu-pas a procesului de backtracking.

**Cerințe**:
1. Citiți puzzle-ul din `data/sudoku_easy.txt` sau `data/sudoku_hard.txt`
2. Implementați verificarea completă a constrângerilor
3. Afișați fiecare pas de backtracking (opțional, flag verbose)
4. Colectați statistici: noduri vizitate, timp de execuție, backtrack-uri
5. Afișați soluția cu formată frumos cu borduri

**Format fișier input**:
```
5 3 0 0 7 0 0 0 0
6 0 0 1 9 5 0 0 0
0 9 8 0 0 0 0 6 0
8 0 0 0 6 0 0 0 3
4 0 0 8 0 3 0 0 1
7 0 0 0 2 0 0 0 6
0 6 0 0 0 0 2 8 0
0 0 0 4 1 9 0 0 5
0 0 0 0 8 0 0 7 9
```

**Fișiere**: `src/exercise2.c`, `data/sudoku_easy.txt`, `data/sudoku_hard.txt`

---

## 🔧 Compilare și Execuție

```bash
# Construiește toate targeturile
make all

# Rulează exemplul demonstrativ
make run

# Rulează exercițiile
make run-ex1
make run-ex2

# Testare automată
make test

# Verificare memory leaks
make valgrind

# Compilare soluții
make solutions

# Curățare
make clean

# Ajutor
make help
```

---

## 📁 Structura Directorului

```
16-greedy-backtracking/
├── README.md                    # Acest fișier
├── Makefile                     # Automatizare build
├── slides/
│   ├── presentation-week16.html # Prezentare principală
│   └── presentation-comparativ.html  # Comparație Pseudocod/C/Python
├── src/
│   ├── example1.c               # Exemplu complet demonstrativ
│   ├── exercise1.c              # Activity Selection (TODO)
│   └── exercise2.c              # Sudoku Solver (TODO)
├── solution/
│   ├── exercise1_sol.c          # Soluție exercițiu 1
│   ├── exercise2_sol.c          # Soluție exercițiu 2
│   ├── homework1_sol.c          # Soluție temă 1
│   └── homework2_sol.c          # Soluție temă 2
├── teme/
│   ├── homework-requirements.md # Cerințe temă obligatorie
│   └── homework-extended.md     # Provocări bonus
├── data/
│   ├── activities.txt           # Date pentru Activity Selection
│   ├── knapsack.txt             # Date pentru Knapsack
│   ├── sudoku_easy.txt          # Sudoku dificultate ușoară
│   └── sudoku_hard.txt          # Sudoku dificultate ridicată
└── tests/
    ├── test1_input.txt          # Input test 1
    ├── test1_expected.txt       # Output așteptat test 1
    ├── test2_input.txt          # Input test 2
    └── test2_expected.txt       # Output așteptat test 2
```

---

## 📖 Bibliografie Recomandată

### Esențiale

- **Thomas H. Cormen et al.** - *Introduction to Algorithms*, Capitolele 15-16 (Greedy Algorithms, Dynamic Programming)
- **Steven S. Skiena** - *The Algorithm Design Manual*, Capitolul 7 (Combinatorial Search and Heuristic Methods)
- **Robert Sedgewick** - *Algorithms*, Secțiunile despre Backtracking și Greedy

### Avansate

- **Donald E. Knuth** - *The Art of Computer Programming*, Vol. 4A (Combinatorial Algorithms)
- **Jon Kleinberg, Éva Tardos** - *Algorithm Design*, Capitolele 4-6

### Resurse Online

- [GeeksforGeeks - Greedy Algorithms](https://www.geeksforgeeks.org/greedy-algorithms/) - Colecție extensivă de probleme și soluții
- [Visualgo - Backtracking](https://visualgo.net/en/recursion) - Vizualizări interactive
- [CP-Algorithms](https://cp-algorithms.com/) - Implementări eficiente pentru competiții

---

## ✅ Lista de Auto-Verificare

Înainte de a considera această săptămână completă, verificați că:

### Cunoștințe Teoretice
- [ ] Puteți explica diferența dintre Greedy și Backtracking
- [ ] Știți când Greedy garantează soluția optimă (proprietatea alegerii greedy + substructură optimală)
- [ ] Înțelegeți conceptul de "pruning" în backtracking
- [ ] Puteți estima complexitatea algoritmilor de backtracking

### Abilități Practice
- [ ] Puteți implementa Activity Selection fără referință
- [ ] Puteți implementa Fractional Knapsack fără referință
- [ ] Puteți implementa N-Queens solver complet
- [ ] Puteți implementa Sudoku solver funcțional

### Depanare și Optimizare
- [ ] Codul compilează fără warnings cu `-Wall -Wextra`
- [ ] Ați verificat cu Valgrind că nu există memory leaks
- [ ] Algoritmii voștri produc rezultate corecte pe toate testele
- [ ] Înțelegeți cum să optimizați backtracking-ul prin pruning avansat

---

## 💼 Pregătire pentru Interviu

### Întrebări Frecvente

1. **Când folosești Greedy vs. Programare Dinamică?**
   - Greedy: când alegerea locală optimă garantează optim global (proprietatea greedy)
   - DP: când subproblemele se suprapun și Greedy nu garantează optim
   - Exemplu clasic: Fractional Knapsack (Greedy) vs. 0/1 Knapsack (DP)

2. **Cum optimizezi un algoritm de backtracking?**
   - Ordonarea candidaților (Most Constrained Variable first)
   - Propagarea constrângerilor (Forward Checking)
   - Pruning avansat (tăierea ramurilor imposibile cât mai devreme)
   - Memoizare pentru subprobleme repetate

3. **Complexitatea N-Queens?**
   - Worst case: O(N!) - verificăm toate permutările
   - Cu pruning: mult mai bine în practică
   - Spațiu: O(N) pentru recursie

4. **Dă un exemplu de problemă unde Greedy eșuează**
   - Coin Change cu monede arbitrare: {1, 3, 4}, suma=6
   - Greedy: 4+1+1=3 monede, Optim: 3+3=2 monede

5. **Cum detectezi că o problemă poate fi rezolvată cu Greedy?**
   - Verifici proprietatea alegerii greedy (exchange argument)
   - Verifici substructura optimală
   - Construiești un contraexemplu dacă nu ești sigur

---

## 🔗 Preview Săptămâna Următoare

**Revizuire și Pregătire Examen**

Săptămâna următoare vom recapitula toate conceptele studiate în acest semestru, cu accent pe:
- Exerciții integrative care combină mai multe tehnici
- Probleme de interviu și competiții
- Strategii de abordare a problemelor noi
- Analiza trade-off-urilor între diferite soluții

---

*Acest material face parte din cursul "Algoritmi și Tehnici de Programare" (ATP)*  
*Academia de Studii Economice din București - CSIE*  
*Actualizat: Ianuarie 2026*
