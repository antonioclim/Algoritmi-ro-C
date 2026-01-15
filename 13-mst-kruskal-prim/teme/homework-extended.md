# Provocări Extinse - Săptămâna 13

## 🚀 Provocări Avansate (Opțional)

Fiecare provocare rezolvată corect: **+10 puncte bonus**

Punctele bonus se adaugă la nota de laborator (maxim 150 din 100).

---

## ⭐ Provocarea 1: A* Search Algorithm (Dificultate: Medie)

### Descriere

Implementați algoritmul A* (A-star), o extensie a lui Dijkstra care folosește o funcție euristică pentru a ghida căutarea mai eficient spre destinație.

### Fundament Teoretic

A* combină:
- **g(n)** = costul real de la start până la nodul n (ca la Dijkstra)
- **h(n)** = estimarea euristică până la destinație
- **f(n) = g(n) + h(n)** = prioritatea în heap

### Cerințe

1. Implementați A* cu euristică Manhattan pentru grile 2D
2. Implementați A* cu euristică Euclidiană pentru grafuri cu coordonate
3. Comparați performanța A* vs Dijkstra (număr de noduri expandate)
4. Demonstrați că euristicile voastre sunt **admisibile** (nu supraestimează)

### Structuri de Date

```c
typedef struct {
    int id;
    double x, y;  // coordonate pentru euristică
} Node;

typedef struct {
    int dest;
    double weight;
    struct AStarEdge* next;
} AStarEdge;

// Priority Queue cu f(n) = g(n) + h(n)
double heuristic_euclidean(Node* a, Node* b);
double heuristic_manhattan(Node* a, Node* b);
```

### Output Așteptat

```
=== A* vs Dijkstra ===
Graf: 100 noduri, 400 muchii
Drum cautat: 0 -> 99

Dijkstra:
  Noduri expandate: 87
  Distanta gasita: 234.5
  Timp: 1.2ms

A* (Euclidian):
  Noduri expandate: 23
  Distanta gasita: 234.5
  Timp: 0.4ms

Imbunatatire: 73.6% mai putine noduri expandate
```

### Puncte Bonus: +10

---

## ⭐ Provocarea 2: Floyd-Warshall All-Pairs (Dificultate: Medie)

### Descriere

Implementați algoritmul Floyd-Warshall pentru găsirea drumurilor minime între **toate** perechile de noduri.

### Algoritm

```
pentru k de la 0 la V-1:
    pentru i de la 0 la V-1:
        pentru j de la 0 la V-1:
            dacă dist[i][k] + dist[k][j] < dist[i][j]:
                dist[i][j] = dist[i][k] + dist[k][j]
                next[i][j] = next[i][k]
```

### Cerințe

1. Implementați Floyd-Warshall cu complexitate O(V³)
2. Reconstruiți drumurile folosind matricea `next`
3. Detectați cicluri negative (verificați diagonala principală)
4. Afișați matricea de distanțe într-un format tabelar frumos

### Output Așteptat

```
=== Floyd-Warshall: All-Pairs Shortest Paths ===

Matrice Distante:
       0      1      2      3      4
  0    0      4      7      5     10
  1   INF     0      3      1      6
  2   INF   INF      0    INF      3
  3   INF   INF      2      0      5
  4   INF   INF    INF    INF      0

Drum 0 -> 4: 0 -> 1 -> 3 -> 2 -> 4 (cost: 10)

Status: Nu exista cicluri negative
```

### Puncte Bonus: +10

---

## ⭐ Provocarea 3: Shortest Path Faster Algorithm - SPFA (Dificultate: Medie-Grea)

### Descriere

Implementați SPFA, o optimizare a Bellman-Ford care în practică rulează mult mai rapid pe grafuri sparse.

### Diferența față de Bellman-Ford

- Bellman-Ford: relaxează TOATE muchiile de V-1 ori
- SPFA: folosește o coadă și relaxează doar muchiile din noduri modificate recent

### Pseudocod

```
SPFA(G, source):
    dist[source] = 0
    queue.push(source)
    in_queue[source] = true
    count[source] = 1
    
    while queue not empty:
        u = queue.pop()
        in_queue[u] = false
        
        for each (u, v, w) in edges:
            if dist[u] + w < dist[v]:
                dist[v] = dist[u] + w
                if not in_queue[v]:
                    queue.push(v)
                    in_queue[v] = true
                    count[v]++
                    if count[v] >= V:
                        return "CICLU NEGATIV"
    
    return dist
```

### Cerințe

1. Implementați SPFA cu detecție de ciclu negativ
2. Comparați performanța cu Bellman-Ford clasic
3. Demonstrați cazuri unde SPFA e semnificativ mai rapid
4. Identificați cazuri worst-case unde SPFA degenerează la O(V·E)

### Output Așteptat

```
=== SPFA vs Bellman-Ford ===
Graf: 1000 noduri, 5000 muchii (sparse)

Bellman-Ford:
  Iteratii muchii: 4,995,000
  Timp: 45ms

SPFA:
  Operatii relaxare: 12,340
  Timp: 3ms

Imbunatatire: 14.5x mai rapid
```

### Puncte Bonus: +10

---

## ⭐ Provocarea 4: Bidirectional Dijkstra (Dificultate: Grea)

### Descriere

Implementați Dijkstra bidirecțional care caută simultan de la sursă și de la destinație, întâlnindu-se la mijloc.

### Concept

1. Rulați Dijkstra înainte de la `source`
2. Rulați Dijkstra înapoi de la `target` (pe graful transpus)
3. Opriți când frontierele se întâlnesc
4. Drumul optim trece printr-un nod de pe frontier

### Complexitate

- Dijkstra simplu: explorează cerc de rază R
- Dijkstra bidirecțional: explorează două cercuri de rază R/2
- Aria cercului: πR² vs 2·π(R/2)² = πR²/2
- **Îmbunătățire teoretică: 2x**

### Cerințe

1. Implementați căutarea bidirecțională
2. Gestionați corect criteriul de oprire
3. Reconstruiți drumul din ambele direcții
4. Măsurați și afișați îmbunătățirea față de Dijkstra simplu

### Atenție la Criterii de Oprire!

**Greșit:** Oprești când un nod e vizitat din ambele direcții
**Corect:** Oprești când `dist_forward[u] + dist_backward[u] >= best_path_so_far`

### Puncte Bonus: +10

---

## ⭐ Provocarea 5: Johnson's Algorithm (Dificultate: Foarte Grea)

### Descriere

Implementați algoritmul lui Johnson pentru all-pairs shortest paths în grafuri cu muchii negative (dar fără cicluri negative).

### De ce Johnson?

- Floyd-Warshall: O(V³) - bun pentru grafuri dense
- V × Dijkstra: O(V·(V+E)·log V) - dar nu merge cu muchii negative!
- Johnson: O(V·(V+E)·log V) - funcționează cu muchii negative!

### Algoritm

1. Adaugă un nod fictiv `q` conectat la toate nodurile cu cost 0
2. Rulează Bellman-Ford din `q` → obții `h[v]` pentru fiecare nod
3. Recalculează ponderile: `w'(u,v) = w(u,v) + h[u] - h[v]` (toate pozitive!)
4. Rulează Dijkstra din fiecare nod cu ponderile noi
5. Corectează distanțele: `dist(u,v) = dist'(u,v) - h[u] + h[v]`

### Cerințe

1. Implementați complet algoritmul Johnson
2. Verificați lipsa ciclurilor negative în pasul Bellman-Ford
3. Demonstrați corectitudinea transformării ponderilor
4. Comparați cu Floyd-Warshall pe grafuri sparse

### Output Așteptat

```
=== Johnson's Algorithm ===
Graf: 500 noduri, 2000 muchii (sparse, cu ponderi negative)

Pas 1: Bellman-Ford pentru potentiale h[]
  Timp: 15ms
  Status: Nu exista ciclu negativ

Pas 2: Reponderate (toate w' >= 0)
  Verificare: OK

Pas 3: 500 x Dijkstra
  Timp: 120ms

Total Johnson: 135ms
Floyd-Warshall ar fi luat: ~2500ms (estimat)

Imbunatatire: 18.5x pentru grafuri sparse
```

### Puncte Bonus: +10

---

## 📊 Sistemul de Puncte Bonus

| Provocări Completate | Total Bonus |
|---------------------|-------------|
| 1 provocare | +10 puncte |
| 2 provocări | +20 puncte |
| 3 provocări | +30 puncte |
| 4 provocări | +40 puncte |
| Toate 5 | +50 puncte + Badge "Graph Master" 🏆 |

---

## 📤 Format Submisie Bonus

```
Nume_Prenume_Grupa_ATP_S13_BONUS.zip
├── challenge1_astar.c
├── challenge2_floyd.c
├── challenge3_spfa.c
├── challenge4_bidirectional.c
├── challenge5_johnson.c
├── test_data/
│   ├── graph_coords.txt    (pentru A*)
│   ├── graph_dense.txt     (pentru Floyd)
│   └── graph_negative.txt  (pentru Johnson)
└── ANALYSIS.md             (comparații, grafice, concluzii)
```

---

## 🎯 Criterii de Evaluare Bonus

Pentru fiecare provocare:

| Criteriu | Pondere |
|----------|---------|
| Implementare corectă | 50% |
| Comparație cu algoritmul de bază | 25% |
| Analiză și documentație | 15% |
| Cod curat, fără warning-uri | 10% |

---

## 💡 Sfaturi pentru Provocări

1. **Începeți cu cea mai accesibilă** - A* sau Floyd-Warshall sunt cele mai directe

2. **Folosiți grafuri de test cunoscute** - unde știți răspunsul corect

3. **Măsurați performanța real** - folosiți `clock()` sau `gettimeofday()`

4. **Documentați observațiile** - când merge mai bine, când degenerează

5. **Citiți literatura** - CLRS capitolele 24-25 pentru detalii

---

**Notă:** Provocările bonus sunt opționale și destinate studenților care doresc să aprofundeze subiectul. Nu afectează negativ nota dacă nu sunt rezolvate.

**Succes și distracție plăcută cu grafurile!** 🚀
