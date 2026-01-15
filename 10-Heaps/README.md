# Săptămâna 10: Heap-uri și Cozi cu Priorități

## 🎯 Obiective de Învățare

La finalul acestei săptămâni, studentul va fi capabil să:

1. **Memoreze** proprietățile fundamentale ale structurii de date heap (max-heap și min-heap), inclusiv proprietatea de formă și proprietatea de ordine
2. **Înțeleagă** diferențele conceptuale dintre heap-uri, arbori binari de căutare și cozi cu priorități, precum și relația de reprezentare în memorie
3. **Aplice** operațiile de bază: inserare, extragere maximă/minimă, și heapify asupra unui vector de date arbitrar
4. **Analizeze** complexitatea temporală a algoritmilor heap-sort și a operațiilor pe priority queue în diferite scenarii de utilizare
5. **Evalueze** eficiența implementărilor alternative (heap binar vs. heap d-ar, array vs. pointeri) și să justifice alegeri de design
6. **Creeze** aplicații complete care utilizează heap-uri pentru rezolvarea problemelor de planificare, selecție și sortare

---

## 📜 Context Istoric

Structura de date **heap** își are originea în anul 1964, când informaticianul britanic **J.W.J. Williams** a introdus conceptul în contextul dezvoltării algoritmului de sortare **Heap Sort**. Publicația sa seminală în Communications of the ACM a reprezentat un moment de cotitură în algoritmică, oferind o metodă de sortare cu complexitate O(n log n) care opera in-place, fără a necesita memorie auxiliară semnificativă.

La scurt timp după aceea, în 1964, **Robert W. Floyd** a adus îmbunătățiri fundamentale algoritmului, dezvoltând metoda de construcție bottom-up a heap-ului cu complexitate O(n), demonstrând că un vector arbitrar poate fi transformat într-un heap în timp liniar. Această contribuție a fost parte a cercetărilor sale mai largi în domeniul algoritmilor și analizei lor, pentru care a primit ulterior prestigiosul Premiu Turing în 1978.

Conceptul de **priority queue** (coadă cu priorități) a evoluat în paralel, având rădăcini în teoria cozilor de așteptare dezvoltată în telecomunicații. Implementarea eficientă a cozilor cu priorități prin heap-uri a revoluționat numeroase domenii: de la planificarea proceselor în sisteme de operare (unde UNIX a adoptat heap-uri pentru scheduling încă din anii 1970) până la algoritmi fundamentali în teoria grafurilor precum Dijkstra și Prim.

### Figura Cheie: J.W.J. Williams (1930-2012)

John William Joseph Williams a fost un informatician britanic care a lucrat la UK Atomic Energy Authority și ulterior la University of Sussex. Contribuția sa majoră rămâne inventarea heap-ului binar și a algoritmului Heap Sort, care continuă să fie predate în curriculumul universitar de informatică peste tot în lume.

> *"Simplitatea structurii heap derivă din exploatarea elegantă a proprietăților arborilor binari compleți - o demonstrație că cele mai puternice abstractizări sunt adesea cele mai simple."*
> — J.W.J. Williams

---

## 📚 Fundamente Teoretice

### 1. Definiția și Proprietățile Heap-ului

Un **heap binar** este un arbore binar complet care satisface **proprietatea heap**:

- **Max-Heap**: Pentru orice nod `i` diferit de rădăcină, `A[parent(i)] ≥ A[i]`
- **Min-Heap**: Pentru orice nod `i` diferit de rădăcină, `A[parent(i)] ≤ A[i]`

**Proprietatea de formă**: Un heap este întotdeauna un arbore binar complet - toate nivelurile sunt pline cu excepția posibilă a ultimului, care este umplut de la stânga la dreapta.

```
         Reprezentare Max-Heap         │        Reprezentare în Array
                                       │
              ┌───┐                     │    Index:  0   1   2   3   4   5   6
              │90 │                     │    Array: [90, 85, 80, 70, 60, 50, 30]
              └─┬─┘                     │
         ┌─────┴─────┐                  │    Relații de navigare:
       ┌─┴─┐       ┌─┴─┐                │    parent(i)     = (i-1) / 2
       │85 │       │80 │                │    left_child(i) = 2*i + 1
       └─┬─┘       └─┬─┘                │    right_child(i)= 2*i + 2
      ┌──┴──┐     ┌──┴──┐               │
    ┌─┴─┐ ┌─┴─┐ ┌─┴─┐ ┌─┴─┐             │
    │70 │ │60 │ │50 │ │30 │             │
    └───┘ └───┘ └───┘ └───┘             │
```

### 2. Operația Heapify (Sift-Down)

Operația **heapify** (sau sift-down) restaurează proprietatea heap pentru un subarbore, presupunând că subarborii stâng și drept sunt deja heap-uri valide:

```c
/**
 * Restaurează proprietatea max-heap pentru nodul la indexul i.
 * Presupune că subarborii sunt deja max-heap-uri.
 *
 * @param arr   - vectorul reprezentând heap-ul
 * @param n     - dimensiunea heap-ului
 * @param i     - indexul nodului de verificat
 */
void max_heapify(int arr[], int n, int i) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n && arr[left] > arr[largest])
        largest = left;

    if (right < n && arr[right] > arr[largest])
        largest = right;

    if (largest != i) {
        swap(&arr[i], &arr[largest]);
        max_heapify(arr, n, largest);  // Recursie pe subarborele afectat
    }
}
```

### 3. Complexitatea Operațiilor pe Heap

| Operație | Complexitate Timp | Observații |
|----------|-------------------|------------|
| **Inserare** | O(log n) | Percolate up de la frunză la rădăcină |
| **Extract-Max/Min** | O(log n) | Heapify de la rădăcină în jos |
| **Peek (Get-Max/Min)** | O(1) | Rădăcina este întotdeauna extremul |
| **Build-Heap** | O(n) | Construcție bottom-up (Floyd) |
| **Heap-Sort** | O(n log n) | n extracții, fiecare O(log n) |
| **Increase-Key** | O(log n) | Sift-up după modificare |
| **Delete** | O(log n) | Înlocuire cu ultimul + heapify |

**Demonstrație Build-Heap O(n)**:

Construcția bottom-up pornește de la nodurile non-frunză (indicii n/2-1 până la 0). Analiza arată că suma operațiilor de heapify este mărginită de n, nu de n log n, datorită distribuției nodurilor pe niveluri:

```
Nivel h: max n/2^(h+1) noduri, fiecare necesită O(h) operații
Total = Σ(h=0 la log n) (n/2^(h+1)) * O(h) = O(n)
```

---

## 🏭 Aplicații Industriale

### 1. Planificarea Proceselor în Sisteme de Operare

Kernel-ul Linux utilizează heap-uri pentru implementarea **Completely Fair Scheduler (CFS)**:

```c
/* Structură simplificată din kernel - rb_tree bazat pe vruntime */
struct sched_entity {
    u64 vruntime;           /* Virtual runtime - cheie pentru heap */
    struct rb_node run_node; /* Nod în red-black tree (min-heap) */
};

/* Selectarea următorului proces - O(log n) */
struct task_struct* pick_next_task_fair(struct rq *rq) {
    struct sched_entity *se = rb_entry(rb_first(&rq->cfs_tasks),
                                       struct sched_entity, run_node);
    return task_of(se);
}
```

### 2. Algoritmul Dijkstra cu Priority Queue

Implementarea eficientă a celui mai scurt drum în grafuri ponderate:

```c
/* Dijkstra cu min-heap - O((V+E) log V) */
void dijkstra(Graph *g, int src, int dist[]) {
    MinHeap *pq = create_min_heap(g->V);
    
    for (int v = 0; v < g->V; v++)
        dist[v] = INT_MAX;
    dist[src] = 0;
    
    insert_min_heap(pq, src, 0);
    
    while (!is_empty(pq)) {
        int u = extract_min(pq);
        for (Edge *e = g->adj[u]; e; e = e->next) {
            int v = e->dest, weight = e->weight;
            if (dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                decrease_key(pq, v, dist[v]);  /* O(log V) */
            }
        }
    }
}
```

### 3. Huffman Coding - Compresie de Date

Algoritmul Huffman utilizează un min-heap pentru construcția arborelui de codificare:

```c
/* Construirea arborelui Huffman */
HuffmanNode* build_huffman_tree(char data[], int freq[], int size) {
    MinHeap *heap = create_and_build_min_heap(data, freq, size);
    
    while (heap->size > 1) {
        HuffmanNode *left = extract_min(heap);
        HuffmanNode *right = extract_min(heap);
        
        HuffmanNode *internal = create_node('$', left->freq + right->freq);
        internal->left = left;
        internal->right = right;
        
        insert_min_heap(heap, internal);
    }
    return extract_min(heap);  /* Rădăcina arborelui */
}
```

### 4. Event-Driven Simulation

Simulările discrete utilizează heap-uri pentru gestionarea evenimentelor ordonate temporal:

```c
/* Event loop în simulare */
typedef struct {
    double timestamp;
    EventType type;
    void *data;
} Event;

void simulation_loop(EventHeap *eq, double end_time) {
    while (!heap_empty(eq)) {
        Event *e = heap_extract_min(eq);
        if (e->timestamp > end_time) break;
        
        current_time = e->timestamp;
        process_event(e);  /* Poate adăuga noi evenimente în heap */
    }
}
```

### 5. Median Maintenance cu Două Heap-uri

Tehnica de menținere a medianei într-un stream de date:

```c
/* Streaming median - O(log n) per inserare */
typedef struct {
    MaxHeap *lower;  /* Jumătatea inferioară */
    MinHeap *upper;  /* Jumătatea superioară */
} MedianFinder;

double find_median(MedianFinder *mf) {
    if (mf->lower->size > mf->upper->size)
        return max_heap_peek(mf->lower);
    else if (mf->upper->size > mf->lower->size)
        return min_heap_peek(mf->upper);
    else
        return (max_heap_peek(mf->lower) + min_heap_peek(mf->upper)) / 2.0;
}
```

---

## 💻 Exerciții de Laborator

### Exercițiu 1: Implementare Priority Queue cu Max-Heap

**Cerințe:**
1. Implementați structura de date `PriorityQueue` bazată pe max-heap
2. Funcții obligatorii: `pq_create()`, `pq_insert()`, `pq_extract_max()`, `pq_peek()`, `pq_is_empty()`, `pq_destroy()`
3. Redimensionare automată când capacitatea este depășită
4. Gestionare corectă a memoriei (fără memory leaks)
5. Testare cu cel puțin 1000 de elemente

### Exercițiu 2: Algoritmul Heap Sort

**Cerințe:**
1. Implementați funcția `build_max_heap()` cu complexitate O(n)
2. Implementați `heap_sort()` pentru sortare in-place
3. Comparați performanța cu `qsort()` din biblioteca standard
4. Demonstrați stabilitatea/instabilitatea algoritmului
5. Tratați cazurile speciale: vector gol, un element, elemente duplicate

---

## 🔧 Compilare și Execuție

```bash
# Compilare completă
make all

# Rulare exemplu demonstrativ
make run

# Testare automată exerciții
make test

# Verificare memory leaks
make valgrind

# Curățare artefacte
make clean

# Afișare ajutor
make help
```

---

## 📁 Structura Directorului

```
week-10-heaps/
├── README.md                           # Acest document
├── Makefile                            # Automatizare build
│
├── slides/
│   ├── presentation-week10.html        # Prezentare principală (35-40 slide-uri)
│   └── presentation-comparativ.html    # Comparație Pseudocod → C → Python
│
├── src/
│   ├── example1.c                      # Exemplu complet funcțional (~450 linii)
│   ├── exercise1.c                     # Exercițiu Priority Queue (TODO)
│   └── exercise2.c                     # Exercițiu Heap Sort (TODO)
│
├── data/
│   ├── numbers_random.txt              # 1000 numere aleatoare
│   └── numbers_sorted.txt              # 1000 numere sortate (caz limită)
│
├── tests/
│   ├── test1_input.txt                 # Input pentru testare exercițiu 1
│   ├── test1_expected.txt              # Output așteptat exercițiu 1
│   ├── test2_input.txt                 # Input pentru testare exercițiu 2
│   └── test2_expected.txt              # Output așteptat exercițiu 2
│
├── teme/
│   ├── homework-requirements.md        # 2 teme (câte 50 puncte)
│   └── homework-extended.md            # 5 provocări bonus (+10 puncte fiecare)
│
└── solution/
    ├── exercise1_sol.c                 # Soluție completă exercițiu 1
    ├── exercise2_sol.c                 # Soluție completă exercițiu 2
    ├── homework1_sol.c                 # Soluție temă 1
    └── homework2_sol.c                 # Soluție temă 2
```

---

## 📖 Lectură Recomandată

### Esențial
- **Cormen, Leiserson, Rivest, Stein** - *Introduction to Algorithms*, Cap. 6: Heapsort
- **Sedgewick, Wayne** - *Algorithms*, Secțiunea 2.4: Priority Queues
- **Knuth** - *The Art of Computer Programming*, Vol. 3, Secțiunea 5.2.3

### Avansat
- **Fredman, Tarjan** - *Fibonacci Heaps and Their Uses in Improved Network Optimization Algorithms* (1987)
- **Brodal** - *Worst-case efficient priority queues* (1996)

### Resurse Online
- [Visualgo - Heap Visualization](https://visualgo.net/en/heap)
- [GeeksforGeeks - Binary Heap](https://www.geeksforgeeks.org/binary-heap/)
- [MIT OCW 6.006 - Heaps and Heap Sort](https://ocw.mit.edu/courses/6-006-introduction-to-algorithms-spring-2020/)

---

## ✅ Lista de Auto-Evaluare

Înainte de a considera această săptămână completă, verificați:

- [ ] Pot explica diferența dintre max-heap și min-heap
- [ ] Înțeleg de ce reprezentarea în array este eficientă pentru heap-uri
- [ ] Pot implementa `heapify` (sift-down) atât iterativ cât și recursiv
- [ ] Știu de ce `build_heap` are complexitate O(n) și pot argumenta formal
- [ ] Pot implementa `heap_sort` complet și corect
- [ ] Înțeleg când și de ce să folosesc priority queue în loc de sortare completă
- [ ] Pot analiza complexitatea spațială și temporală a tuturor operațiilor
- [ ] Am testat codul cu Valgrind și nu am memory leaks
- [ ] Pot compara heap-uri cu alte structuri de date (BST, array sortat)
- [ ] Cunosc cel puțin 3 aplicații industriale ale heap-urilor

---

## 💼 Pregătire pentru Interviu

### Întrebări Frecvente

1. **Care este diferența dintre heap și binary search tree?**
   - Heap: proprietate de ordine parțială (părinte vs. copii), arbore complet
   - BST: proprietate de ordine totală (stânga < rădăcină < dreapta), nu neapărat echilibrat

2. **De ce heap sort nu este stabil?**
   - Operația de swap la extragere poate modifica ordinea relativă a elementelor egale

3. **Când ai folosi heap în loc de array sortat pentru priority queue?**
   - Heap: inserări frecvente O(log n), extrageri frecvente O(log n)
   - Array sortat: inserare O(n), dar căutare binară O(log n)

4. **Cum ai implementa un min-max heap?**
   - Niveluri alternante: niveluri pare sunt min-heap, niveluri impare sunt max-heap

5. **Explică Fibonacci Heap și avantajele sale.**
   - Decrease-key amortizat O(1), util în Dijkstra pentru grafuri dense

---

## 🔗 Previzualizare Săptămâna Următoare

**Săptămâna 11: Hash Tables și Funcții de Dispersie**

Vom explora structurile de date bazate pe hashing, incluzând:
- Funcții hash și proprietățile lor (uniformitate, determinism)
- Rezolvarea coliziunilor: chaining vs. open addressing
- Implementarea unui hash table generic în C
- Aplicații: dicționare, cache-uri, indexare baze de date

---

*Acest material a fost creat pentru cursul „Algoritmi și Tehnici de Programare" - Academia de Studii Economice București, Facultatea CSIE.*
