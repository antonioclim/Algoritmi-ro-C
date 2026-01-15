# Săptămâna 13: Algoritmi pe Grafuri – Drumuri Minime

## 🎯 Obiective de Învățare

La finalul acestei săptămâni, studentul va fi capabil să:

1. **Recunoaște** structurile de date utilizate pentru reprezentarea grafurilor ponderate și să identifice contextele în care algoritmii de drumuri minime sunt aplicabili
2. **Înțeleagă** principiile fundamentale ale algoritmilor Dijkstra și Bellman-Ford, inclusiv diferențele de complexitate și constrângerile privind muchiile negative
3. **Aplice** algoritmii de shortest path pentru rezolvarea problemelor practice de rutare și optimizare în rețele
4. **Analizeze** complexitatea temporală și spațială a diferitelor implementări, comparând reprezentările prin matrice de adiacență versus liste de adiacență
5. **Evalueze** alegerea algoritmului optim în funcție de caracteristicile grafului (dens vs. rar, muchii negative, grafuri mari)
6. **Creeze** implementări eficiente și robuste ale algoritmilor de drumuri minime, integrând structuri auxiliare precum priority queues

---

## 📜 Context Istoric

Problema determinării celui mai scurt drum într-un graf reprezintă una dintre cele mai fundamentale întrebări din teoria grafurilor și informatică. Originar formulată în contextul optimizării rețelelor de transport, această problemă a evoluat până la a deveni esențială în domenii variate precum rutarea pachetelor în Internet, navigația GPS, planificarea logistică și analiza rețelelor sociale.

Dezvoltarea algoritmilor eficienți pentru shortest path a marcat un punct de cotitură în istoria informaticii. Înainte de contribuțiile lui Dijkstra și Bellman-Ford, metodele existente erau fie ineficiente pentru grafuri mari, fie incapabile să gestioneze anumite configurații de muchii. Evoluția acestor algoritmi reflectă tranziția de la abordări brute-force la tehnici sofisticate bazate pe programare dinamică și structuri de date optimizate.

Astăzi, acești algoritmi constituie fundamentul protocoalelor de rutare din Internet (OSPF folosește Dijkstra, RIP folosește Bellman-Ford), sistemelor de navigație și al numeroaselor aplicații de optimizare combinatorială.

### Figură Cheie: Edsger Wybe Dijkstra (1930-2002)

Edsger Dijkstra, informatician olandez și laureat al Premiului Turing (1972), a conceput algoritmul care îi poartă numele în 1956, publicându-l în 1959. Lucrând la Centrul de Matematică din Amsterdam, Dijkstra a dezvoltat algoritmul în aproximativ 20 de minute, într-o cafenea, fără a folosi creion și hârtie – demonstrând eleganța gândirii algoritmice pure.

Pe lângă algoritmul de shortest path, Dijkstra a adus contribuții fundamentale în domeniul sistemelor de operare (conceptul de semafor pentru sincronizare), compilatoarelor, programării structurate și al verificării formale a programelor. Celebra sa scrisoare „Go To Statement Considered Harmful" (1968) a transformat practica programării.

> *„Computer science is no more about computers than astronomy is about telescopes."*
> — Edsger W. Dijkstra

### Richard Bellman (1920-1984)

Richard Bellman, matematician american, a dezvoltat independent algoritmul cunoscut sub numele Bellman-Ford (cu contribuții din partea lui Lester Ford Jr.) în 1958. Bellman este mai ales renumit pentru introducerea conceptului de **programare dinamică**, o paradigmă care stă la baza numeroșilor algoritmi moderni.

> *„An optimal policy has the property that whatever the initial state and initial decision are, the remaining decisions must constitute an optimal policy."*
> — Richard Bellman (Principiul Optimalității)

---

## 📚 Fundamente Teoretice

### 1. Problema Drumului Minim (Shortest Path Problem)

Problema shortest path constă în determinarea drumului de cost minim între două noduri într-un graf ponderat. Formal:

```
Fie G = (V, E, w) un graf ponderat, unde:
  - V = mulțimea nodurilor (vertices)
  - E = mulțimea muchiilor (edges)  
  - w: E → ℝ funcția de pondere (weight)

Problema: Găsește drumul P = (v₀, v₁, ..., vₖ) de la sursa s la destinația t
          astfel încât Σ w(vᵢ, vᵢ₊₁) să fie minimă.
```

**Variante ale problemei:**
- **Single-Source Shortest Path (SSSP)**: drumuri minime de la o sursă la toate celelalte noduri
- **Single-Destination**: drumuri minime de la toate nodurile la o destinație
- **Single-Pair**: drum minim între două noduri specifice
- **All-Pairs Shortest Path (APSP)**: drumuri minime între toate perechile de noduri

### 2. Algoritmul lui Dijkstra

Algoritmul Dijkstra rezolvă problema SSSP pentru grafuri cu **muchii de pondere non-negativă**. Utilizează o strategie greedy, selectând la fiecare pas nodul nevizitat cu distanța minimă față de sursă.

```
┌─────────────────────────────────────────────────────────────────────────┐
│                    ALGORITMUL DIJKSTRA - SCHEMA                         │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│   Inițializare:                                                         │
│   ┌─────────────────────────────────────┐                               │
│   │  dist[s] = 0                        │                               │
│   │  dist[v] = ∞  pentru v ≠ s          │                               │
│   │  Q = {toate nodurile}               │                               │
│   └─────────────────────────────────────┘                               │
│                                                                         │
│   Bucla principală:                                                     │
│   ┌─────────────────────────────────────┐                               │
│   │  while Q ≠ ∅:                       │                               │
│   │    u = extract_min(Q)       ←─── selectează nodul cu dist minimă    │
│   │    for each v ∈ adj[u]:            │                               │
│   │      if dist[u] + w(u,v) < dist[v]:│                               │
│   │        dist[v] = dist[u] + w(u,v)  │  ←─── relaxare                │
│   │        pred[v] = u                 │                               │
│   └─────────────────────────────────────┘                               │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

**Complexitate:**
| Implementare | Timp | Spațiu |
|--------------|------|--------|
| Matrice adiacență + căutare liniară | O(V²) | O(V²) |
| Liste adiacență + binary heap | O((V + E) log V) | O(V + E) |
| Liste adiacență + Fibonacci heap | O(V log V + E) | O(V + E) |

### 3. Algoritmul Bellman-Ford

Algoritmul Bellman-Ford rezolvă problema SSSP pentru grafuri care pot conține **muchii cu ponderi negative**, dar fără cicluri negative. Spre deosebire de Dijkstra, poate detecta prezența ciclurilor negative.

```
┌─────────────────────────────────────────────────────────────────────────┐
│                   ALGORITMUL BELLMAN-FORD - SCHEMA                      │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│   Inițializare:                                                         │
│   ┌─────────────────────────────────────┐                               │
│   │  dist[s] = 0                        │                               │
│   │  dist[v] = ∞  pentru v ≠ s          │                               │
│   └─────────────────────────────────────┘                               │
│                                                                         │
│   Relaxare (V-1 iterații):                                              │
│   ┌─────────────────────────────────────┐                               │
│   │  for i = 1 to V-1:                  │                               │
│   │    for each edge (u, v, w):         │                               │
│   │      if dist[u] + w < dist[v]:      │                               │
│   │        dist[v] = dist[u] + w        │                               │
│   │        pred[v] = u                  │                               │
│   └─────────────────────────────────────┘                               │
│                                                                         │
│   Detectare ciclu negativ:                                              │
│   ┌─────────────────────────────────────┐                               │
│   │  for each edge (u, v, w):           │                               │
│   │    if dist[u] + w < dist[v]:        │                               │
│   │      return "CICLU NEGATIV!"        │                               │
│   └─────────────────────────────────────┘                               │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

**Complexitate:** O(V × E)

### 4. Comparație Dijkstra vs. Bellman-Ford

| Criteriu | Dijkstra | Bellman-Ford |
|----------|----------|--------------|
| Muchii negative | ❌ Nu suportă | ✅ Suportă |
| Cicluri negative | Nu detectează | ✅ Detectează |
| Complexitate (liste adj.) | O((V+E) log V) | O(V × E) |
| Grafuri dense | Mai rapid | Mai lent |
| Grafuri rare | Comparabil | Mai lent |
| Paralelizare | Dificilă | Ușoară |
| Implementare | Mai complexă (heap) | Mai simplă |

### 5. Structuri de Date Auxiliare

**Priority Queue (Min-Heap):**
```
                    ┌───┐
                    │ 2 │         ← Nodul cu distanța minimă
                    └─┬─┘
              ┌───────┴───────┐
            ┌─┴─┐           ┌─┴─┐
            │ 5 │           │ 7 │
            └─┬─┘           └─┬─┘
         ┌───┴───┐       ┌───┴───┐
       ┌─┴─┐   ┌─┴─┐   ┌─┴─┐   ┌─┴─┐
       │ 9 │   │ 8 │   │15 │   │12 │
       └───┘   └───┘   └───┘   └───┘

Operații:
  - insert(node, priority): O(log n)
  - extract_min(): O(log n)
  - decrease_key(node, new_priority): O(log n)
```

---

## 🏭 Aplicații Industriale

### 1. Protocoale de Rutare în Rețele (OSPF - Dijkstra)

```c
// Simplificare a logicii OSPF (Open Shortest Path First)
typedef struct {
    uint32_t router_id;
    uint32_t cost;
    uint32_t next_hop;
} RoutingEntry;

void ospf_compute_routing_table(Graph* topology, uint32_t my_id) {
    int* dist = dijkstra(topology, my_id);
    int* pred = get_predecessors();
    
    for (int dest = 0; dest < topology->num_routers; dest++) {
        if (dest != my_id) {
            RoutingEntry entry;
            entry.router_id = dest;
            entry.cost = dist[dest];
            entry.next_hop = find_next_hop(pred, my_id, dest);
            add_to_routing_table(entry);
        }
    }
}
```

### 2. Sisteme de Navigație GPS

```c
// Structură pentru navigație cu constrângeri de timp
typedef struct {
    int intersection_id;
    double latitude;
    double longitude;
    int* adjacent;
    double* travel_times;  // Pot fi negative (bonus pentru autostrăzi)
} NavigationNode;

Path* find_fastest_route(NavigationGraph* map, 
                         int origin, int destination,
                         time_t departure_time) {
    // Bellman-Ford permite "bonusuri" (ponderi negative)
    // pentru rute preferate
    return bellman_ford_path(map, origin, destination);
}
```

### 3. Sisteme de Recomandare (Social Networks)

```c
// Calculul "distanței sociale" între utilizatori
typedef struct {
    int user_id;
    int* connections;
    double* relationship_strength;  // 0.0 - 1.0
} SocialNode;

double social_distance(SocialGraph* network, int user_a, int user_b) {
    // Transformăm relațiile în distanțe (invers proporționale)
    // Folosim Dijkstra pentru a găsi cea mai "apropiată" cale
    return dijkstra_distance(network, user_a, user_b);
}
```

### 4. Optimizare Supply Chain

```c
// Determinarea rutelor optime de livrare
typedef struct {
    int warehouse_id;
    int customer_id;
    double distance;
    double cost;
    double time;
} DeliveryRoute;

DeliveryPlan* optimize_deliveries(SupplyNetwork* network,
                                  int* customer_orders,
                                  int num_orders) {
    // Multi-criteria shortest path
    // Combinăm distanță, cost și timp
    return multi_objective_dijkstra(network, customer_orders, num_orders);
}
```

### 5. Game AI - Pathfinding

```c
// A* (extensie a lui Dijkstra cu euristică)
typedef struct {
    int x, y;
    int g_cost;      // Costul real de la start
    int h_cost;      // Euristică (estimare până la destinație)
    int f_cost;      // g + h
    struct Node* parent;
} PathNode;

Path* a_star_pathfinding(GameMap* map, Point start, Point goal) {
    // A* = Dijkstra + euristică admisibilă
    // Mult mai rapid pentru navigație în jocuri
    PriorityQueue* open_set = pq_create();
    // ... implementare A*
}
```

---

## 💻 Exerciții de Laborator

### Exercițiu 1: Implementare Dijkstra cu Priority Queue

**Obiectiv:** Implementarea completă a algoritmului Dijkstra folosind un min-heap pentru optimizarea selecției nodului cu distanța minimă.

**Cerințe:**
1. Definirea structurii grafului cu liste de adiacență
2. Implementarea min-heap-ului cu operațiile: `insert`, `extract_min`, `decrease_key`
3. Implementarea algoritmului Dijkstra
4. Reconstrucția și afișarea drumului minim
5. Tratarea cazurilor speciale (noduri inaccesibile)

**Fișier:** `src/exercise1.c`

### Exercițiu 2: Implementare Bellman-Ford cu Detectare Cicluri Negative

**Obiectiv:** Implementarea algoritmului Bellman-Ford cu capacitatea de a detecta și raporta ciclurile negative.

**Cerințe:**
1. Reprezentarea grafului cu lista de muchii
2. Implementarea fazei de relaxare (V-1 iterații)
3. Implementarea detectării ciclurilor negative
4. Identificarea nodurilor afectate de cicluri negative
5. Compararea performanței cu Dijkstra pe același graf

**Fișier:** `src/exercise2.c`

---

## 🔧 Compilare și Execuție

```bash
# Compilare completă
make all

# Rulare exemplu demonstrativ
make run

# Compilare și rulare exercițiu 1
make exercise1
./exercise1 < data/graph1.txt

# Compilare și rulare exercițiu 2
make exercise2
./exercise2 < data/graph_negative.txt

# Verificare cu teste automate
make test

# Verificare memory leaks cu Valgrind
make valgrind

# Curățare fișiere generate
make clean

# Ajutor comenzi disponibile
make help
```

---

## 📁 Structura Directorului

```
week-13-graph-algorithms/
├── README.md                              # Acest fișier
├── Makefile                               # Automatizare build
│
├── slides/
│   ├── presentation-week13.html           # Prezentare principală (35+ slides)
│   └── presentation-comparativ.html       # Comparație Pseudocod/C/Python
│
├── src/
│   ├── example1.c                         # Exemplu complet Dijkstra + Bellman-Ford
│   ├── exercise1.c                        # Exercițiu: Dijkstra cu heap
│   └── exercise2.c                        # Exercițiu: Bellman-Ford ciclu negativ
│
├── data/
│   ├── graph1.txt                         # Graf pentru testare Dijkstra
│   ├── graph_negative.txt                 # Graf cu muchii negative
│   └── graph_large.txt                    # Graf mare pentru benchmarking
│
├── tests/
│   ├── test1_input.txt                    # Input test exercițiu 1
│   ├── test1_expected.txt                 # Output așteptat exercițiu 1
│   ├── test2_input.txt                    # Input test exercițiu 2
│   └── test2_expected.txt                 # Output așteptat exercițiu 2
│
├── teme/
│   ├── homework-requirements.md           # Cerințe teme (2 × 50p)
│   └── homework-extended.md               # Provocări bonus (5 × 10p)
│
└── solution/
    ├── exercise1_sol.c                    # Soluție exercițiu 1
    ├── exercise2_sol.c                    # Soluție exercițiu 2
    ├── homework1_sol.c                    # Soluție temă 1
    └── homework2_sol.c                    # Soluție temă 2
```

---

## 📖 Lectură Recomandată

### Resurse Esențiale
- **Cormen, T.H. et al.** - *Introduction to Algorithms* (CLRS), Cap. 24-25
- **Sedgewick, R.** - *Algorithms in C*, Part 5: Graph Algorithms
- **Kleinberg, J. & Tardos, E.** - *Algorithm Design*, Cap. 4

### Resurse Avansate
- **Fredman, M.L. & Tarjan, R.E.** - *Fibonacci Heaps and Their Uses* (1987)
- **Goldberg, A.V.** - *Shortest Path Algorithms: Engineering Aspects* (2001)
- **Delling, D. et al.** - *Engineering Route Planning Algorithms* (2009)

### Resurse Online
- Visualgo - Dijkstra Visualization: https://visualgo.net/en/sssp
- GeeksforGeeks - Shortest Path Algorithms: https://www.geeksforgeeks.org/shortest-path-algorithms/
- CP-Algorithms - Dijkstra & Bellman-Ford: https://cp-algorithms.com/graph/dijkstra.html

---

## ✅ Lista de Auto-Evaluare

Înainte de a considera această săptămână completă, verifică:

- [ ] Pot explica diferența fundamentală între Dijkstra și Bellman-Ford
- [ ] Înțeleg de ce Dijkstra nu funcționează cu muchii negative
- [ ] Pot implementa un min-heap cu operațiile necesare
- [ ] Pot implementa Dijkstra cu complexitate O((V+E) log V)
- [ ] Pot implementa Bellman-Ford și detecta cicluri negative
- [ ] Înțeleg când să folosesc fiecare algoritm în practică
- [ ] Pot reconstrui drumul minim din array-ul de predecesori
- [ ] Pot analiza complexitatea pentru diferite reprezentări ale grafului
- [ ] Înțeleg aplicațiile în protocoale de rutare (OSPF, RIP)
- [ ] Pot adapta algoritmii pentru probleme specifice

---

## 💼 Pregătire pentru Interviuri

### Întrebări Frecvente

1. **„Explică diferența dintre Dijkstra și Bellman-Ford. Când ai folosi fiecare?"**
   - Dijkstra: grafuri fără muchii negative, mai rapid cu heap
   - Bellman-Ford: suportă muchii negative, detectează cicluri negative

2. **„Care este complexitatea lui Dijkstra cu diferite structuri de date?"**
   - Array simplu: O(V²)
   - Binary heap: O((V+E) log V)
   - Fibonacci heap: O(V log V + E)

3. **„Cum detectezi un ciclu negativ într-un graf?"**
   - Rulezi V-1 iterații Bellman-Ford
   - Încă o iterație: dacă se mai relaxează vreo muchie → ciclu negativ

4. **„Implementează Dijkstra pentru un graf reprezentat ca matrice de adiacență."**
   - Focus pe bucla principală și selecția minimului

5. **„Cum ai optimiza Dijkstra pentru un graf foarte mare (milioane de noduri)?"**
   - A* cu euristică admisibilă
   - Bidirectional Dijkstra
   - Contraction Hierarchies

---

## 🔗 Previzualizare Săptămâna 14

**Tema:** Recapitulare și Optimizări Avansate

Săptămâna viitoare vom:
- Recapitula toți algoritmii studiați pe parcursul semestrului
- Explora optimizări avansate pentru structurile de date
- Discuta despre alegerea algoritmului potrivit pentru diferite scenarii
- Pregăti pentru examenul final

---

*Documentație generată pentru cursul ATP - ASE București*
*Săptămâna 13: Algoritmi pe Grafuri - Drumuri Minime*
