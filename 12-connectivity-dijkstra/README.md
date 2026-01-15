# Săptămâna 12: Grafuri — Noțiuni Fundamentale

## 🎯 Obiective de Învățare

La finalul acestei săptămâni, studenții vor fi capabili să:

1. **Reține** (Remember): Definească noțiunile de graf orientat, graf neorientat, nod, muchie, arc și gradul unui nod
2. **Înțeleagă** (Understand): Explice diferențele între reprezentarea prin matrice de adiacență și liste de adiacență, precum și compromisurile spațiu-timp asociate
3. **Aplice** (Apply): Implementeze algoritmi de parcurgere BFS (Breadth-First Search) și DFS (Depth-First Search) pentru explorarea sistematică a grafurilor
4. **Analizeze** (Analyse): Compare complexitățile algoritmice O(V+E) vs O(V²) pentru diferite reprezentări și operații
5. **Evalueze** (Evaluate): Selecteze reprezentarea optimă a grafului în funcție de densitatea acestuia și operațiile predominante
6. **Creeze** (Create): Proiecteze soluții bazate pe grafuri pentru probleme practice precum rețele sociale, hărți rutiere și dependențe între sarcini

---

## 📜 Context Istoric

Teoria grafurilor își are originea într-o problemă aparent simplă: **Cele șapte poduri din Königsberg** (1736). Matematicianul elvețian **Leonhard Euler** a demonstrat că este imposibil să traversezi toate cele șapte poduri ale orașului Königsberg (astăzi Kaliningrad, Rusia) exact o singură dată și să te întorci la punctul de plecare. Această demonstrație a pus bazele unei discipline matematice complet noi.

Evoluția teoriei grafurilor a fost marcată de contribuții fundamentale: **Gustav Kirchhoff** (1847) a aplicat grafurile în analiza circuitelor electrice, **Arthur Cayley** (1857) a studiat arborii pentru enumerarea izomerilor chimici, iar **Dénes König** (1936) a publicat primul tratat sistematic de teoria grafurilor. În era computațională, **Edsger Dijkstra** (1959) și **Robert Floyd** (1962) au dezvoltat algoritmi fundamentali pentru drumuri minime.

Astăzi, grafurile constituie coloana vertebrală a infrastructurii digitale: de la rutarea pachetelor în Internet (protocoale BGP, OSPF) până la recomandările din rețelele sociale, de la compilatoare (grafuri de dependențe) până la sistemele de navigație GPS.

### Figură Cheie: Leonhard Euler (1707–1783)

Euler a fost unul dintre cei mai prolifici matematicieni din istorie, cu contribuții în calcul, teoria numerelor, mecanică și optică. Articolul său din 1736 despre podurile din Königsberg, intitulat *"Solutio problematis ad geometriam situs pertinentis"*, este considerat prima lucrare de teoria grafurilor și topologie.

> *„Pentru a ajunge la cunoaștere, trebuie să studiezi; pentru a ajunge la înțelepciune, trebuie să observi."* — Leonhard Euler

---

## 📚 Fundamente Teoretice

### 1. Definiții și Terminologie

Un **graf** G = (V, E) este o structură algebrică formată din:
- **V** (Vertices/Nodes): Mulțimea nodurilor (vârfurilor)
- **E** (Edges/Arcs): Mulțimea muchiilor care conectează perechi de noduri

```
         Graf Neorientat                    Graf Orientat (Digraf)
         
            (A)                                 (A)
           / | \                               ↙   ↘
         (B)-(C)-(D)                         (B) → (C) → (D)
           \ | /                               ↘   ↗
            (E)                                 (E)
            
    Muchie {B,C}: nedirectată           Arc (B,C): de la B spre C
    grad(C) = 4                         grad+(C) = 2 (intrare)
                                        grad-(C) = 1 (ieșire)
```

**Terminologie esențială:**
- **Graf simplu**: Fără bucle (self-loops) și fără muchii multiple
- **Graf complet Kₙ**: Toate perechile de noduri sunt conectate; |E| = n(n-1)/2
- **Graf bipartit**: Nodurile pot fi partiționate în două mulțimi disjuncte
- **Graf conex**: Există drum între oricare două noduri
- **Componente conexe**: Submulțimi maximale conexe

### 2. Reprezentări în Memorie

#### 2.1 Matricea de Adiacență

```
    A B C D E                    Implementare C:
A [ 0 1 1 0 0 ]                 int adj[MAX_V][MAX_V];
B [ 1 0 1 0 1 ]                 
C [ 1 1 0 1 1 ]                 // Verificare muchie (u,v): O(1)
D [ 0 0 1 0 0 ]                 if (adj[u][v]) { ... }
E [ 0 1 1 0 0 ]                 
                                // Spațiu: O(V²)
```

**Avantaje:**
- Verificarea existenței muchiei: O(1)
- Adăugare/ștergere muchie: O(1)
- Simplitate conceptuală

**Dezavantaje:**
- Spațiu: O(V²) — ineficient pentru grafuri rare (sparse)
- Enumerarea vecinilor: O(V) chiar dacă are puțini vecini

#### 2.2 Liste de Adiacență

```
Nod → Vecini                     Implementare C:
A  → [B, C]                      typedef struct Node {
B  → [A, C, E]                       int vertex;
C  → [A, B, D, E]                    struct Node* next;
D  → [C]                         } Node;
E  → [B, C]                      Node* adjList[MAX_V];
                                 
                                 // Spațiu: O(V + E)
```

**Avantaje:**
- Spațiu: O(V + E) — eficient pentru grafuri rare
- Enumerarea vecinilor: O(grad(v))
- Iterare naturală pentru BFS/DFS

**Dezavantaje:**
- Verificarea existenței muchiei: O(grad(v))
- Overhead pentru pointeri

### 3. Complexități Comparative

| Operație | Matrice de Adiacență | Liste de Adiacență |
|----------|---------------------|-------------------|
| Spațiu | O(V²) | O(V + E) |
| Adaugă muchie | O(1) | O(1) |
| Șterge muchie | O(1) | O(grad(v)) |
| Verifică muchie | O(1) | O(grad(v)) |
| Enumerează vecini | O(V) | O(grad(v)) |
| BFS/DFS complet | O(V²) | O(V + E) |

**Regula practică:** Folosiți liste de adiacență când E << V² (grafuri rare/sparse).

---

## 🔍 Algoritmii de Parcurgere

### BFS (Breadth-First Search)

Parcurgerea în lățime explorează graful nivel cu nivel, folosind o coadă (queue) FIFO.

```
Pseudocod BFS(G, start):
    creează coadă Q
    marchează start ca vizitat
    Q.enqueue(start)
    
    cât timp Q nu este goală:
        u = Q.dequeue()
        procesează(u)
        pentru fiecare vecin v al lui u:
            dacă v nu este vizitat:
                marchează v ca vizitat
                Q.enqueue(v)
```

**Proprietăți:**
- Găsește drumul cel mai scurt (în număr de muchii) de la sursă
- Complexitate: O(V + E) cu liste de adiacență
- Folosește O(V) memorie suplimentară pentru coadă și vectorul de vizitare

**Aplicații:** Drumuri minime în grafuri neponderate, verificarea conectivității, niveluri în arbori, algoritmi de flux.

### DFS (Depth-First Search)

Parcurgerea în adâncime explorează cât mai departe posibil înainte de backtracking, folosind recursivitate sau o stivă (stack) LIFO.

```
Pseudocod DFS(G, u):
    marchează u ca vizitat
    procesează(u)
    pentru fiecare vecin v al lui u:
        dacă v nu este vizitat:
            DFS(G, v)
```

**Proprietăți:**
- Explorează complet o ramură înainte de a trece la alta
- Complexitate: O(V + E) cu liste de adiacență
- Folosește O(V) pentru stiva de apeluri (atenție la stack overflow pentru grafuri mari!)

**Aplicații:** Detectarea ciclurilor, sortare topologică, componente tare conexe, generarea labirinturilor.

---

## 🏭 Aplicații Industriale

### 1. Rețele Sociale (Facebook, LinkedIn)

```c
// Sugestii de prieteni: prieteni ai prietenilor la distanță 2
void suggest_friends(Graph* g, int user_id) {
    int dist[MAX_USERS];
    bfs_with_distance(g, user_id, dist);
    
    for (int i = 0; i < g->n; i++) {
        if (dist[i] == 2 && !are_friends(g, user_id, i)) {
            printf("Sugestie: Utilizator %d\n", i);
        }
    }
}
```

### 2. Sistemele de Navigație GPS

```c
// Reprezentarea hărții ca graf ponderat
typedef struct {
    int destination;
    double distance_km;
    double time_min;
} Road;

// Graf: intersecții = noduri, străzi = muchii ponderate
// Dijkstra/A* pentru ruta optimă
```

### 3. Compilatoare — Grafuri de Dependențe

```c
// Makefile: ordinea de compilare prin sortare topologică
// main.o depinde de utils.o și math.o
// 
// Dependențe:          Ordine de build (sortare topologică):
// main.o → utils.o     1. math.o
// main.o → math.o      2. utils.o  
// utils.o → math.o     3. main.o
```

### 4. Protocoale de Rutare Internet

```c
// OSPF (Open Shortest Path First)
// Fiecare router menține un graf al rețelei
// Dijkstra pentru calculul căilor optime

typedef struct {
    int router_id;
    AdjList* neighbors;
    int routing_table[MAX_ROUTERS];
} OSPFRouter;
```

### 5. Sisteme de Recomandări

```c
// Graful bipartit utilizatori-produse
// Recomandare: produse cumpărate de utilizatori similari
//
// User1 ──┬── ProductA
//         └── ProductB
// User2 ──┬── ProductB  ← overlap cu User1
//         └── ProductC  ← recomandare pentru User1
```

---

## 💻 Exerciții de Laborator

### Exercițiu 1: Implementarea Grafului cu Liste de Adiacență

**Cerințe:**
1. Implementați structura de date pentru graf cu liste de adiacență
2. Funcții pentru: creare graf, adăugare muchie, afișare, eliberare memorie
3. Citiți graful dintr-un fișier în format: prima linie N M (noduri, muchii), urmate de M linii cu perechi u v
4. Verificați dacă graful este conex

**Fișiere:** `exercise1.c`, `data/graph1.txt`

### Exercițiu 2: BFS și DFS — Parcurgeri Complete

**Cerințe:**
1. Implementați BFS iterativ cu coadă
2. Implementați DFS recursiv și iterativ (cu stivă)
3. Afișați ordinea de vizitare și nivelul fiecărui nod (pentru BFS)
4. Detectați și afișați componentele conexe
5. Calculați distanța de la un nod sursă la toate celelalte

**Fișiere:** `exercise2.c`, `data/graph2.txt`, `data/graph3.txt`

---

## 🔧 Compilare și Execuție

```bash
# Compilare toate sursele
make

# Compilare fișier specific
make example1
make exercise1
make exercise2

# Rulare exemplu demonstrativ
make run

# Testare automată
make test

# Verificare memory leaks cu Valgrind
make valgrind

# Curățare fișiere generate
make clean

# Afișare ajutor
make help
```

---

## 📁 Structura Directorului

```
week-12-graphs-basics/
├── README.md                           # Acest fișier
├── Makefile                            # Automatizare compilare
│
├── slides/
│   ├── presentation-week12.html        # Prezentare interactivă (35-40 slide-uri)
│   └── presentation-comparativ.html    # Comparație Pseudocod→C→Python
│
├── src/
│   ├── example1.c                      # Exemplu complet (~450 linii)
│   ├── exercise1.c                     # Exercițiu 1: Liste de adiacență
│   └── exercise2.c                     # Exercițiu 2: BFS și DFS
│
├── data/
│   ├── graph1.txt                      # Graf de test pentru exercițiul 1
│   ├── graph2.txt                      # Graf pentru BFS/DFS
│   └── graph3.txt                      # Graf cu componente multiple
│
├── tests/
│   ├── test1_input.txt                 # Input test exercițiu 1
│   ├── test1_expected.txt              # Output așteptat exercițiu 1
│   ├── test2_input.txt                 # Input test exercițiu 2
│   └── test2_expected.txt              # Output așteptat exercițiu 2
│
├── teme/
│   ├── homework-requirements.md        # Cerințe teme (2 × 50p)
│   └── homework-extended.md            # Provocări bonus (5 × 10p)
│
└── solution/
    ├── exercise1_sol.c                 # Soluție completă exercițiu 1
    ├── exercise2_sol.c                 # Soluție completă exercițiu 2
    ├── homework1_sol.c                 # Soluție temă 1
    └── homework2_sol.c                 # Soluție temă 2
```

---

## 📖 Lectură Recomandată

### Esențiale
- **Cormen, T. et al.** — *Introduction to Algorithms* (CLRS), Capitolele 22-23
- **Sedgewick, R.** — *Algorithms in C*, Part 5: Graph Algorithms
- **Knuth, D.** — *The Art of Computer Programming*, Vol. 1, Section 2.3

### Avansate
- **Tarjan, R.** — *Data Structures and Network Algorithms* (SIAM)
- **Even, S.** — *Graph Algorithms* (Cambridge University Press)
- **Bondy, J.A. & Murty, U.S.R.** — *Graph Theory* (Springer)

### Resurse Online
- [Visualgo — Graph Traversal](https://visualgo.net/en/dfsbfs) — Vizualizări interactive BFS/DFS
- [GeeksforGeeks — Graph Data Structure](https://www.geeksforgeeks.org/graph-data-structure-and-algorithms/)
- [CP-Algorithms — Graphs](https://cp-algorithms.com/graph/breadth-first-search.html)

---

## ✅ Lista de Auto-Evaluare

Verificați-vă cunoștințele bifând fiecare punct pe care îl stăpâniți:

- [ ] Pot defini formal un graf și pot explica diferența între graf orientat și neorientat
- [ ] Înțeleg compromisurile spațiu-timp între matricea de adiacență și listele de adiacență
- [ ] Pot implementa ambele reprezentări în C de la zero
- [ ] Pot implementa BFS iterativ cu coadă și pot calcula distanțe
- [ ] Pot implementa DFS atât recursiv cât și iterativ cu stivă
- [ ] Pot detecta componentele conexe ale unui graf
- [ ] Pot analiza complexitatea O(V+E) și pot explica când se aplică
- [ ] Pot identifica aplicații practice ale grafurilor în sisteme reale
- [ ] Pot depana probleme de memorie în structuri de date cu pointeri
- [ ] Știu când să aleg BFS vs DFS în funcție de problema dată

---

## 💼 Pregătire pentru Interviuri

### Întrebări frecvente la interviurile tehnice:

1. **Care este diferența între BFS și DFS? Când folosești fiecare?**
   - BFS: drumuri minime, niveluri, explorare pe straturi
   - DFS: cicluri, backtracking, explorare exhaustivă

2. **Cum detectezi un ciclu într-un graf orientat vs. neorientat?**
   - Neorientat: DFS, dacă găsești un vecin vizitat (diferit de părinte) → ciclu
   - Orientat: DFS cu colorare (alb/gri/negru) sau sortare topologică

3. **Care este complexitatea BFS/DFS și de ce?**
   - O(V + E) cu liste de adiacență: vizităm fiecare nod o dată și fiecare muchie o dată

4. **Cum găsești componente conexe?**
   - Rulează BFS/DFS din fiecare nod nevizitat; fiecare apel găsește o componentă

5. **Când folosești matrice de adiacență vs. liste?**
   - Matrice: grafuri dense (E ≈ V²), verificări frecvente de existență muchie
   - Liste: grafuri rare (E << V²), parcurgeri frecvente

---

## 🔗 Previzualizare Săptămâna 13

Săptămâna viitoare vom studia **Algoritmi pe Grafuri — Drumuri Minime**, incluzând:

- **Dijkstra's Algorithm**: Drumul cel mai scurt de la o sursă în grafuri cu ponderi pozitive
- **Bellman-Ford Algorithm**: Gestionarea ponderilor negative și detectarea ciclurilor negative
- **Floyd-Warshall Algorithm**: Drumuri minime între toate perechile de noduri
- **Aplicații**: Sisteme GPS, rutare în rețele, analiza rețelelor sociale

Pregătiți-vă revizuind:
- Structura de date heap (pentru Dijkstra eficient)
- Principiul relaxării muchiilor
- Programarea dinamică (pentru Floyd-Warshall)

---

*Dezvoltat pentru cursul **Algoritmi și Tehnici de Programare** — ASE București, CSIE*
