# Săptămâna 6: Cozi (Queues)

## 🎯 Obiective de Învățare

La finalul acestei săptămâni, studentul va fi capabil să:

1. **Cunoaștere (Remember)**: Să definească structura de date queue și să enumere operațiile fundamentale (enqueue, dequeue, peek, isEmpty, isFull)
2. **Înțelegere (Understand)**: Să explice principiul FIFO (First In, First Out) și diferențele față de structura stack (LIFO)
3. **Aplicare (Apply)**: Să implementeze o queue folosind atât reprezentarea cu array static cât și lista simplu înlănțuită
4. **Analiză (Analyse)**: Să compare complexitățile temporale și spațiale ale diferitelor implementări de queues și să identifice trade-off-urile
5. **Evaluare (Evaluate)**: Să selecteze implementarea optimă de queue pentru un context specific (circular buffer vs. linked list vs. deque)
6. **Creare (Create)**: Să proiecteze și să implementeze un sistem de procesare a task-urilor folosind priority queue

---

## 📜 Context Istoric

### Originea Conceptului de Queue

Conceptul de **queue** în informatică își are rădăcinile în teoria cozilor de așteptare (queueing theory), dezvoltată de matematicianul danez **Agner Krarup Erlang** la începutul secolului XX. Lucrând pentru compania telefonică Copenhagen Telephone Company, Erlang a studiat problema congestiei în rețelele telefonice, punând bazele teoriei matematice a sistemelor de așteptare.

În anii 1950-1960, odată cu dezvoltarea primelor sisteme de operare, queue-urile au devenit structuri fundamentale pentru **job scheduling** și **buffer management**. Sistemul de operare CTSS (Compatible Time-Sharing System) dezvoltat la MIT în 1961 a fost printre primele care au implementat queue-uri pentru gestionarea proceselor multiple.

Implementarea **circular buffer** a apărut ca o optimizare esențială în sistemele embedded și în procesarea semnalelor digitale (DSP), unde eficiența memoriei este critică. Richard Hamming și Claude Shannon de la Bell Labs au contribuit semnificativ la formalizarea acestor concepte în contextul teoriei informației.

### Personalitate Cheie: Agner Krarup Erlang (1878-1929)

```
╔═══════════════════════════════════════════════════════════════════════════════╗
║  AGNER KRARUP ERLANG                                                          ║
║  Matematician danez, părintele teoriei cozilor de așteptare                   ║
╠═══════════════════════════════════════════════════════════════════════════════╣
║                                                                               ║
║  • Născut: 1 ianuarie 1878, Lonborg, Danemarca                               ║
║  • Studii: Universitatea din Copenhaga (matematică)                          ║
║  • Contribuții majore:                                                        ║
║    - Formula Erlang B (probabilitatea de blocare)                            ║
║    - Formula Erlang C (timp de așteptare în coadă)                           ║
║    - Unitatea "erlang" pentru intensitatea traficului                        ║
║                                                                               ║
║  Citat celebru:                                                               ║
║  "Calculele care stau la baza rețelelor telefonice pot fi aplicate          ║
║   oricărui sistem în care entitățile așteaptă să fie servite."              ║
║                                                                               ║
╚═══════════════════════════════════════════════════════════════════════════════╝
```

---

## 📚 Fundamente Teoretice

### 1. Principiul FIFO (First In, First Out)

O **queue** este o colecție ordonată de elemente în care adăugarea se face la un capăt (**rear/back**) iar eliminarea la celălalt (**front**). Această disciplină de acces se numește **FIFO**:

```
    ENQUEUE                                                    DEQUEUE
       │                                                          │
       ▼                                                          ▼
    ╔══════╦══════╦══════╦══════╦══════╦══════╗
    ║  E   ║  D   ║  C   ║  B   ║  A   ║      ║
    ╚══════╩══════╩══════╩══════╩══════╩══════╝
      REAR                               FRONT
    (inserare)                        (extragere)
    
    Ordinea de sosire:    A → B → C → D → E
    Ordinea de ieșire:    A → B → C → D → E  (aceeași!)
```

### 2. Operațiile Fundamentale

| Operație | Descriere | Complexitate Timp | Complexitate Spațiu |
|----------|-----------|-------------------|---------------------|
| `enqueue(x)` | Adaugă elementul x la rear | O(1) | O(1) |
| `dequeue()` | Elimină și returnează elementul din front | O(1) | O(1) |
| `peek()` / `front()` | Returnează elementul din front fără eliminare | O(1) | O(1) |
| `isEmpty()` | Verifică dacă queue este goală | O(1) | O(1) |
| `isFull()` | Verifică dacă queue este plină (array fix) | O(1) | O(1) |
| `size()` | Returnează numărul de elemente | O(1) | O(1) |

### 3. Implementări Alternative

#### 3.1 Array Static (Circular Buffer)

Implementarea cea mai eficientă din punct de vedere al memoriei pentru queue-uri de dimensiune fixă:

```
    Circular Buffer cu capacitate 6:
    
    Index:    0     1     2     3     4     5
           ╔═════╦═════╦═════╦═════╦═════╦═════╗
           ║  X  ║  A  ║  B  ║  C  ║     ║     ║
           ╚═════╩═════╩═════╩═════╩═════╩═════╝
                   ↑                 ↑
                 front             rear
    
    După enqueue('D'):
           ╔═════╦═════╦═════╦═════╦═════╦═════╗
           ║  X  ║  A  ║  B  ║  C  ║  D  ║     ║
           ╚═════╩═════╩═════╩═════╩═════╩═════╝
                   ↑                       ↑
                 front                   rear
    
    Calculul indexului circular:
    next_index = (current_index + 1) % capacity
```

#### 3.2 Lista Simplu Înlănțuită (Linked List Queue)

```
    front                                              rear
      │                                                  │
      ▼                                                  ▼
    ╔═══════════╗    ╔═══════════╗    ╔═══════════╗    ╔═══════════╗
    ║ data: 'A' ║    ║ data: 'B' ║    ║ data: 'C' ║    ║ data: 'D' ║
    ║ next: ────╫───►║ next: ────╫───►║ next: ────╫───►║ next: NULL║
    ╚═══════════╝    ╚═══════════╝    ╚═══════════╝    ╚═══════════╝
```

#### 3.3 Comparație Implementări

| Criteriu | Circular Buffer | Linked List |
|----------|-----------------|-------------|
| Memorie per element | sizeof(T) | sizeof(T) + sizeof(pointer) |
| Capacitate | Fixă | Dinamică |
| Cache locality | Excelentă | Slabă |
| Fragmentare memorie | Nu | Da |
| Overhead alocare | Nu (pre-alocat) | Da (per operație) |
| Utilizare tipică | Buffere I/O, embedded | Cozi de mesaje, general |

---

## 🏭 Aplicații Industriale

### 1. Buffere I/O în Sisteme de Operare

```c
/* Exemplu simplificat din kernel Linux - circular buffer pentru UART */
struct uart_ring_buffer {
    unsigned char buffer[UART_BUFFER_SIZE];
    volatile unsigned int head;  /* Index pentru scriere (ISR) */
    volatile unsigned int tail;  /* Index pentru citire (user) */
};

static inline void uart_put_char(struct uart_ring_buffer *rb, unsigned char c) {
    unsigned int next = (rb->head + 1) % UART_BUFFER_SIZE;
    if (next != rb->tail) {  /* Buffer nu e plin */
        rb->buffer[rb->head] = c;
        rb->head = next;
    }
}
```

### 2. Message Queues în POSIX

```c
/* Comunicare inter-proces folosind POSIX message queues */
#include <mqueue.h>

mqd_t mq = mq_open("/myqueue", O_CREAT | O_RDWR, 0644, NULL);

/* Producător */
mq_send(mq, message, strlen(message) + 1, priority);

/* Consumator */
mq_receive(mq, buffer, MAX_SIZE, NULL);
```

### 3. Event Loop în Node.js (Callback Queue)

```javascript
// Event loop-ul Node.js folosește multiple queue-uri
// pentru diferite tipuri de callback-uri

// 1. Timer Queue (setTimeout, setInterval)
setTimeout(() => console.log('Timer'), 0);

// 2. I/O Callback Queue
fs.readFile('file.txt', (err, data) => {
    console.log('I/O Complete');
});

// 3. Microtask Queue (Promise callbacks)
Promise.resolve().then(() => console.log('Promise'));

// Ordinea: Microtasks → Timers → I/O → Check
```

### 4. Print Spooler în Windows

```c
/* Windows Print Spooler API - job queue management */
HANDLE hPrinter;
OpenPrinter(L"HP LaserJet", &hPrinter, NULL);

DOC_INFO_1 docInfo = {L"Document", NULL, L"RAW"};
DWORD jobId = StartDocPrinter(hPrinter, 1, (LPBYTE)&docInfo);

/* Job-ul intră în queue-ul printerului */
WritePrinter(hPrinter, data, dataSize, &bytesWritten);
EndDocPrinter(hPrinter);
```

### 5. BFS (Breadth-First Search) pentru Grafuri

```c
/* BFS folosește o queue pentru a vizita nodurile nivel cu nivel */
void bfs(Graph *g, int start) {
    Queue *q = queue_create();
    bool visited[MAX_NODES] = {false};
    
    queue_enqueue(q, start);
    visited[start] = true;
    
    while (!queue_is_empty(q)) {
        int current = queue_dequeue(q);
        process_node(current);
        
        for (int neighbor : g->adjacent[current]) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                queue_enqueue(q, neighbor);
            }
        }
    }
}
```

---

## 💻 Exerciții de Laborator

### Exercițiu 1: Circular Buffer pentru Procesare Comenzi

**Obiectiv**: Implementarea unui sistem de procesare a comenzilor dintr-un restaurant folosind un circular buffer.

**Cerințe**:
- Structura `Order` cu: id, client_name, items[], timestamp
- Implementare circular buffer cu capacitate configurabilă
- Operații: `add_order()`, `process_order()`, `peek_next_order()`, `orders_waiting()`
- Tratarea cazurilor de buffer plin și buffer gol
- Statistici: timp mediu de așteptare, comenzi procesate/oră

**Concepte testate**: circular indexing, modulo arithmetic, boundary conditions

### Exercițiu 2: Priority Queue pentru Task Scheduler

**Obiectiv**: Implementarea unui scheduler de task-uri cu priorități multiple folosind array de queues.

**Cerințe**:
- 4 nivele de prioritate: CRITICAL, HIGH, NORMAL, LOW
- Structura `Task` cu: id, priority, description, deadline
- Implementare: array de 4 linked-list queues
- Operații: `schedule_task()`, `execute_next()`, `cancel_task()`, `reschedule()`
- Round-robin între task-urile de aceeași prioritate
- Aging mechanism pentru prevenirea starvation

**Concepte testate**: multiple queues, priority scheduling, linked lists

---

## 🔧 Compilare și Execuție

```bash
# Compilare toate fișierele
make all

# Rulare exemplu demonstrativ
make run

# Rulare teste automate
make test

# Verificare memory leaks cu Valgrind
make valgrind

# Curățare fișiere generate
make clean

# Afișare ajutor
make help
```

### Compilare Individuală

```bash
# Exemplu complet
gcc -Wall -Wextra -std=c11 -g -o example1 src/example1.c

# Exercițiul 1
gcc -Wall -Wextra -std=c11 -g -o exercise1 src/exercise1.c

# Exercițiul 2
gcc -Wall -Wextra -std=c11 -g -o exercise2 src/exercise2.c
```

---

## 📁 Structura Directorului

```
week-06-queues/
├── README.md                          # Acest fișier
├── Makefile                           # Automatizare build
│
├── slides/
│   ├── presentation-week06.html       # Prezentare principală (35-40 slide-uri)
│   └── presentation-comparativ.html   # Comparație Pseudocod/C/Python
│
├── src/
│   ├── example1.c                     # Exemplu complet funcțional
│   ├── exercise1.c                    # Exercițiu circular buffer
│   └── exercise2.c                    # Exercițiu priority queue
│
├── data/
│   ├── orders.txt                     # Date test pentru exercise1
│   └── tasks.txt                      # Date test pentru exercise2
│
├── tests/
│   ├── test1_input.txt                # Input test exercițiu 1
│   ├── test1_expected.txt             # Output așteptat exercițiu 1
│   ├── test2_input.txt                # Input test exercițiu 2
│   └── test2_expected.txt             # Output așteptat exercițiu 2
│
├── teme/
│   ├── homework-requirements.md       # Cerințe teme (2 × 50 puncte)
│   └── homework-extended.md           # Provocări bonus (+10p fiecare)
│
└── solution/
    ├── exercise1_sol.c                # Soluție exercițiu 1
    ├── exercise2_sol.c                # Soluție exercițiu 2
    ├── homework1_sol.c                # Soluție temă 1
    └── homework2_sol.c                # Soluție temă 2
```

---

## 📖 Bibliografie Recomandată

### Cărți Esențiale

1. **Cormen, T.H., Leiserson, C.E., Rivest, R.L., Stein, C.** - *Introduction to Algorithms* (4th Edition), MIT Press, 2022
   - Capitolul 10: Elementary Data Structures (secțiunea Queues)

2. **Sedgewick, R., Wayne, K.** - *Algorithms* (4th Edition), Addison-Wesley, 2011
   - Capitolul 1.3: Bags, Queues, and Stacks

3. **Kernighan, B.W., Ritchie, D.M.** - *The C Programming Language* (2nd Edition), Prentice Hall, 1988
   - Exemplele de structuri de date

### Resurse Avansate

4. **Knuth, D.E.** - *The Art of Computer Programming, Vol. 1: Fundamental Algorithms* (3rd Edition)
   - Secțiunea 2.2.1: Stacks, Queues, and Deques

5. **Tanenbaum, A.S.** - *Modern Operating Systems* (4th Edition)
   - Capitolul despre Scheduling și Buffer Management

### Resurse Online

- [Visualgo - Queue Visualization](https://visualgo.net/en/list) - Animații interactive
- [GeeksforGeeks - Queue Data Structure](https://www.geeksforgeeks.org/queue-data-structure/) - Tutoriale și probleme
- [Linux Kernel - kfifo](https://www.kernel.org/doc/htmldocs/kernel-api/kfifo.html) - Implementare kernel

---

## ✅ Lista de Auto-Evaluare

Înainte de a considera această săptămână completă, verifică:

- [ ] Pot defini ce este o queue și principiul FIFO fără să mă uit în notițe
- [ ] Înțeleg diferența fundamentală între queue și stack
- [ ] Pot implementa o queue folosind un array static (circular buffer)
- [ ] Pot implementa o queue folosind o listă simplu înlănțuită
- [ ] Știu cum să calculez indexul următor în circular buffer: `(i + 1) % capacity`
- [ ] Pot explica când este preferabil circular buffer vs. linked list
- [ ] Am implementat și testat ambele exerciții de laborator
- [ ] Codul meu compilează fără warnings cu `-Wall -Wextra`
- [ ] Am verificat cu Valgrind că nu am memory leaks
- [ ] Pot descrie cel puțin 3 aplicații practice ale queue-urilor

---

## 💼 Pregătire pentru Interviuri

### Întrebări Frecvente

1. **Ce este o queue și care este principiul de funcționare?**
   
   *Răspuns model*: O queue este o structură de date liniară care urmează principiul FIFO (First In, First Out). Elementele sunt adăugate la un capăt (rear) și eliminate de la celălalt (front), similar cu o coadă la casă.

2. **Cum ai implementa o queue cu operații O(1) pentru enqueue și dequeue?**
   
   *Răspuns model*: Folosind un circular buffer cu doi pointeri (front și rear) sau o listă simplu înlănțuită cu pointeri la ambele capete. Circular buffer-ul evită mutarea elementelor după dequeue.

3. **Care este diferența între queue și deque?**
   
   *Răspuns model*: Queue permite inserare doar la rear și ștergere doar la front. Deque (double-ended queue) permite inserare și ștergere la ambele capete, oferind flexibilitate pentru implementarea atât a stack cât și a queue.

4. **Cum ai detecta dacă un circular buffer este plin sau gol?**
   
   *Răspuns model*: Metodă 1: păstrăm un contor separat pentru numărul de elemente. Metodă 2: sacrificăm o poziție - buffer plin când `(rear + 1) % capacity == front`, gol când `front == rear`.

5. **Descrie o situație reală în care ai folosi o priority queue.**
   
   *Răspuns model*: Un sistem de triage în urgențe medicale - pacienții sunt procesați în ordinea gravității, nu a sosirii. Implementarea poate fi cu heap (eficient) sau array de queue-uri (simplu).

---

## 🔗 Previzualizare Săptămâna Următoare

**Săptămâna 7: Arbori Binari (Binary Trees)**

Vom explora structuri de date ierarhice care extind conceptele de linked lists. Subiectele includ:

- Terminologie: rădăcină, noduri, frunze, înălțime, adâncime
- Reprezentarea în memorie a arborilor binari
- Parcurgeri: preorder, inorder, postorder, level-order (folosind queue!)
- Arbori binari de căutare (Binary Search Trees - BST)
- Operații: inserare, căutare, ștergere

**Conexiune cu săptămâna curentă**: Parcurgerea level-order (BFS) a arborilor folosește exact structura queue pe care am studiat-o astăzi!

---

*Ultima actualizare: Ianuarie 2025*
*Curs: Algoritmi și Tehnici de Programare (ATP)*
*Facultatea de Cibernetică, Statistică și Informatică Economică*
*Academia de Studii Economice din București*
