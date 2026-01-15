# Teme Extinse Săptămâna 12: Provocări Avansate pe Grafuri

## 🎯 Prezentare Generală

Aceste exerciții bonus sunt destinate studenților care doresc să-și aprofundeze cunoștințele despre algoritmi pe grafuri. Fiecare exercițiu valorează **10 puncte bonus** (maxim 50 puncte bonus).

**Important:** Punctele bonus se adaugă peste nota temelor obligatorii, dar nota finală nu poate depăși 100%.

---

## 🏆 Bonus 1: Graf Bipartit (10 puncte)

### Descriere

Un **graf bipartit** este un graf ale cărui noduri pot fi împărțite în două mulțimi disjuncte astfel încât fiecare muchie conectează un nod dintr-o mulțime cu un nod din cealaltă mulțime.

Implementați un algoritm care:
1. Verifică dacă un graf este bipartit
2. Dacă este, afișează cele două partiții
3. Dacă nu este, afișează ciclul de lungime impară care demonstrează non-bipartitismul

### Cerințe Tehnice

```c
typedef struct {
    bool is_bipartite;
    int* partition;      /* 0 sau 1 pentru fiecare nod */
    int* odd_cycle;      /* Ciclul de lungime impară (dacă există) */
    int odd_cycle_length;
} BipartiteResult;

BipartiteResult* check_bipartite(Graph* g);
```

### Algoritm Sugerat

Folosiți BFS cu colorare în 2 culori. Graful este bipartit dacă și numai dacă nu există conflict de culoare (adică nu găsim o muchie între noduri de aceeași culoare).

### Complexitate Așteptată

- Timp: O(V + E)
- Spațiu: O(V)

### Fișier: `bonus1_bipartite.c`

---

## 🏆 Bonus 2: Poduri și Puncte de Articulație (10 puncte)

### Descriere

Un **punct de articulație** (cut vertex) este un nod a cărui eliminare deconectează graful.

Un **pod** (bridge) este o muchie a cărei eliminare deconectează graful.

Implementați algoritmul lui Tarjan pentru a găsi toate punctele de articulație și podurile dintr-un graf neorientat.

### Cerințe Tehnice

```c
typedef struct {
    int* articulation_points;
    int num_articulation_points;
    int (*bridges)[2];  /* Array de perechi (u, v) */
    int num_bridges;
} CriticalElements;

CriticalElements* find_critical(Graph* g);
```

### Algoritm Sugerat

Algoritmul lui Tarjan folosește DFS și menține:
- `disc[u]` = momentul descoperirii nodului u
- `low[u]` = cel mai mic `disc` accesibil din subarborele lui u

Un nod u este punct de articulație dacă:
- u este rădăcina DFS și are ≥ 2 copii în arborele DFS, SAU
- u nu este rădăcină și există un copil v cu `low[v] >= disc[u]`

O muchie (u,v) este pod dacă `low[v] > disc[u]`.

### Complexitate Așteptată

- Timp: O(V + E)
- Spațiu: O(V)

### Fișier: `bonus2_critical.c`

---

## 🏆 Bonus 3: Componente Tare Conexe - Kosaraju (10 puncte)

### Descriere

Într-un graf orientat, o **componentă tare conexă** (SCC - Strongly Connected Component) este o mulțime maximală de noduri în care există drum de la orice nod la orice alt nod.

Implementați algoritmul lui Kosaraju pentru a găsi toate componentele tare conexe.

### Cerințe Tehnice

```c
typedef struct {
    int num_scc;
    int* component;  /* component[i] = ID-ul SCC pentru nodul i */
    int** scc_nodes; /* scc_nodes[i] = lista nodurilor din SCC i */
    int* scc_sizes;  /* dimensiunea fiecărei SCC */
} SCCResult;

SCCResult* kosaraju_scc(Graph* g);
```

### Algoritm (3 pași)

1. Efectuați DFS și salvați nodurile într-o stivă în ordinea finalizării
2. Transpuneți graful (inversați toate arcele)
3. Efectuați DFS pe graful transpus în ordinea din stivă; fiecare arbore DFS este o SCC

### Complexitate Așteptată

- Timp: O(V + E)
- Spațiu: O(V + E)

### Fișier: `bonus3_scc.c`

---

## 🏆 Bonus 4: Drumuri Hamiltoniene (10 puncte)

### Descriere

Un **drum Hamiltonian** este un drum care vizitează fiecare nod exact o dată.

Un **ciclu Hamiltonian** este un drum Hamiltonian care se întoarce la nodul de start.

Implementați un algoritm backtracking pentru a găsi drumuri și cicluri Hamiltoniene.

### Cerințe Tehnice

```c
typedef struct {
    bool has_hamiltonian_path;
    bool has_hamiltonian_cycle;
    int* path;         /* Un drum Hamiltonian valid */
    int path_length;
    int total_paths;   /* Numărul total de drumuri (opțional) */
} HamiltonianResult;

HamiltonianResult* find_hamiltonian(Graph* g);
```

### Algoritm Sugerat

Backtracking:
1. Începeți de la un nod și marcați-l ca vizitat
2. Recursiv, încercați să extindeți drumul către vecinii nevizitați
3. Dacă drumul are V noduri, verificați dacă există și ciclu
4. Backtrack: demarcați nodul și încercați altă cale

### Observații

- Problema este NP-completă, deci exponențială în cel mai rău caz
- Pentru grafuri mici (≤15 noduri) este acceptabilă
- Pentru grafuri mari, returnați doar existența, nu toate drumurile

### Complexitate

- Worst case: O(V!)
- Optimizări: pruning bazat pe gradul nodurilor

### Fișier: `bonus4_hamiltonian.c`

---

## 🏆 Bonus 5: Algoritmul Floyd-Warshall (10 puncte)

### Descriere

**Floyd-Warshall** calculează drumurile cele mai scurte între **toate** perechile de noduri, chiar și pentru grafuri cu muchii negative (dar fără cicluri negative).

### Cerințe Tehnice

```c
typedef struct {
    int** dist;        /* dist[i][j] = distanța minimă de la i la j */
    int** next;        /* next[i][j] = următorul nod pe drumul de la i la j */
    bool has_neg_cycle;
} FloydWarshallResult;

FloydWarshallResult* floyd_warshall(Graph* g);
void print_path(FloydWarshallResult* result, int source, int dest);
void print_distance_matrix(FloydWarshallResult* result, int n);
```

### Algoritm

```
pentru k de la 0 la V-1:
    pentru i de la 0 la V-1:
        pentru j de la 0 la V-1:
            dacă dist[i][k] + dist[k][j] < dist[i][j]:
                dist[i][j] = dist[i][k] + dist[k][j]
                next[i][j] = next[i][k]
```

### Detectarea Ciclurilor Negative

După algoritmul principal, verificați dacă `dist[i][i] < 0` pentru orice i.

### Complexitate

- Timp: O(V³)
- Spațiu: O(V²)

### Fișier: `bonus5_floyd_warshall.c`

---

## 📊 Criterii de Evaluare Bonus

| Criteriu | Puncte |
|----------|--------|
| Corectitudine algoritmului | 4 |
| Complexitate optimă | 2 |
| Tratarea cazurilor speciale | 2 |
| Calitatea codului | 2 |

### Cazuri Speciale de Verificat

Pentru fiecare bonus:
- Graf gol (0 noduri)
- Graf cu un singur nod
- Graf complet
- Graf cu componente multiple
- Graf cu ciclu
- Graf fără muchii

---

## 📤 Submisie

1. Fiecare bonus se trimite într-un fișier separat
2. Includeți în fiecare fișier:
   - Comentarii explicative pentru algoritm
   - Funcția `main()` cu teste
   - Analiza complexității
3. Compilați cu: `gcc -Wall -Wextra -std=c11 -o bonus_X bonus_X.c`

---

## 📚 Resurse Suplimentare

### Cărți
- Cormen et al. - "Introduction to Algorithms", Cap. 22-26
- Sedgewick - "Algorithms in C, Part 5: Graph Algorithms"

### Cursuri Online
- MIT OpenCourseWare: 6.006 Introduction to Algorithms
- Coursera: Algorithms Specialization (Stanford)

### Vizualizări
- [VisuAlgo - Graph Algorithms](https://visualgo.net/en/graphds)
- [Graph Algorithm Visualizer](https://www.cs.usfca.edu/~galles/visualization/Algorithms.html)

---

## 💡 Sfaturi

1. **Începeți cu pseudocodul** înainte de implementare
2. **Desenați exemple** pentru a înțelege algoritmul
3. **Testați pe cazuri mici** înainte de cazuri complexe
4. **Verificați cu Valgrind** pentru memory leaks
5. **Comparați cu implementări de referință** din literatura de specialitate

---

*Mult succes! Aceste probleme vă vor pregăti pentru interviuri tehnice și competiții de programare.*
