# Săptămâna 20: Programare Paralelă și Concurentă în C

## 🎯 Obiective de Învățare

La finalul acestei săptămâni, studentul va fi capabil să:

1. **Rememoreze** diferența fundamentală dintre concurență și paralelism; primitivele POSIX Threads (pthread_create, pthread_join, mutex, semaphore, condition variable); condițiile necesare pentru apariția race conditions și deadlock; formularea Legii lui Amdahl pentru speedup teoretic maxim

2. **Înțeleagă** de ce lock-urile sunt necesare pentru protejarea secțiunilor critice; cum thread-urile unei aplicații partajează același spațiu de adresare; overhead-ul sincronizării și impactul său asupra performanței; modelul de memorie și vizibilitatea modificărilor între thread-uri

3. **Aplice** implementarea completă în C a: thread pool cu coadă de task-uri, pattern-ul producer-consumer cu bounded buffer, parallel merge sort cu threshold adaptiv, structuri lock-free folosind C11 atomics

4. **Analizeze** identificarea potențialelor race conditions într-un cod dat; calculul speedup-ului teoretic vs. empiric; impactul granularității lock-urilor asupra performanței; analiza corectitudinii unui protocol de sincronizare

5. **Evalueze** alegerea optimă între mutex, spinlock și operații atomice în funcție de durata secțiunii critice; determinarea numărului optim de thread-uri pentru o sarcină dată; trade-off-uri între simplitate și performanță în design-ul concurrent

6. **Creeze** aplicație paralelă completă cu scalare demonstrabilă, sistem producer-consumer robust, și implementări lock-free ale structurilor de date fundamentale

---

## 📜 Context Istoric

### Originile Programării Concurente

Programarea concurentă și-a găsit rădăcinile în anii 1960, când sistemele de operare au început să implementeze time-sharing pentru a permite mai multor utilizatori să folosească simultan un singur calculator. Această necesitate a condus la dezvoltarea primitivelor de sincronizare care stau și astăzi la baza sistemelor moderne.

Conceptul de **semafor** a fost introdus de Edsger Dijkstra în 1965 la Technische Hogeschool Eindhoven, oferind prima abstracție formală pentru sincronizarea proceselor. Celebra problemă a "Filozofilor la cină" (Dining Philosophers), propusă tot de Dijkstra, rămâne până astăzi un exemplu didactic fundamental pentru ilustrarea problemelor de deadlock și starvation.

Evoluția procesoarelor multi-core din anii 2000 a transformat programarea paralelă dintr-un domeniu de nișă într-o necesitate universală. Legea lui Moore continuă să ofere mai multe tranzistoare, dar nu și frecvențe mai mari - ceea ce înseamnă că performanța trebuie obținută prin paralelism.

### Personalitate Cheie: Edsger Wybe Dijkstra (1930-2002)

```
╔═══════════════════════════════════════════════════════════════════════════════╗
║                          EDSGER WYBE DIJKSTRA                                  ║
║                        "Părintele Calculului Structurat"                       ║
╠═══════════════════════════════════════════════════════════════════════════════╣
║                                                                               ║
║   • Născut în Rotterdam, Olanda (11 Mai 1930)                                ║
║   • Primul programator din Olanda (autodenumire în 1952)                     ║
║   • Premiul Turing 1972 pentru contribuții fundamentale                      ║
║   • Profesor la UT Austin (1984-2002)                                        ║
║                                                                               ║
║   Contribuții majore:                                                         ║
║   • Algoritmul shortest path (Dijkstra, 1956)                                ║
║   • Semafoare și sincronizare procese (1965)                                 ║
║   • THE multiprogramming system (1968)                                       ║
║   • Programarea structurată ("Go To Statement Considered Harmful")           ║
║   • Self-stabilizing systems                                                  ║
║                                                                               ║
╚═══════════════════════════════════════════════════════════════════════════════╝
```

> *"Simplicity is prerequisite for reliability."*
> — Edsger W. Dijkstra, EWD498 (1975)

### Gene Myron Amdahl (1922-2015)

Gene Amdahl, arhitect la IBM și fondator al Amdahl Corporation, a formulat în 1967 celebra lege care îi poartă numele. **Legea lui Amdahl** stabilește limita teoretică a speedup-ului obținut prin paralelizare:

```
Speedup(n) = 1 / (S + P/n)

unde:
  S = fracțiunea secvențială (non-paralelizabilă)
  P = fracțiunea paralelizabilă (S + P = 1)
  n = numărul de procesoare/thread-uri
```

Această lege demonstrează că chiar cu un număr infinit de procesoare, speedup-ul este limitat de porțiunea secvențială a programului. Dacă 5% din cod este secvențial, speedup-ul maxim este 20x, indiferent de resursele disponibile.

### Leslie Lamport (1941-)

Leslie Lamport, laureat al Premiului Turing 2013, a adus contribuții fundamentale la teoria sistemelor distribuite:

- **Lamport timestamps** (1978) - ordonarea evenimentelor în sisteme distribuite
- **Algoritmul bakery** (1974) - excludere mutuală fără suport hardware atomic
- **Paxos** (1998) - protocol de consens distribuit

> *"A distributed system is one in which the failure of a computer you didn't even know existed can render your own computer unusable."*
> — Leslie Lamport

---

## 📚 Fundamente Teoretice

### 1. Concurență vs. Paralelism

Distincția între acești doi termeni este crucială pentru înțelegerea corectă a domeniului:

```
╔═══════════════════════════════════════════════════════════════════════════════╗
║                       CONCURENȚĂ vs. PARALELISM                                ║
╠═══════════════════════════════════════════════════════════════════════════════╣
║                                                                               ║
║   CONCURENȚĂ: Gestionarea mai multor task-uri                                 ║
║   ─────────────────────────────────────────────                               ║
║   • Task-urile EXISTĂ simultan în sistem                                      ║
║   • Execuție POATE fi întreținută (interleaved) pe un singur core             ║
║   • Focalizare pe STRUCTURA programului                                       ║
║   • Exemplu: server web handling 1000 conexiuni pe un CPU                     ║
║                                                                               ║
║        Task A:  ████░░░░░░████░░░░░░████                                      ║
║        Task B:  ░░░░████░░░░░░████░░░░░░                                      ║
║        Task C:  ░░░░░░░░████░░░░░░████░░                                      ║
║                 ─────────────────────────►  timp                              ║
║                      (un singur CPU)                                          ║
║                                                                               ║
╠═══════════════════════════════════════════════════════════════════════════════╣
║                                                                               ║
║   PARALELISM: Execuție simultană reală                                        ║
║   ────────────────────────────────────────                                    ║
║   • Task-urile se EXECUTĂ efectiv în același moment                           ║
║   • Necesită multiple unități de procesare                                    ║
║   • Focalizare pe PERFORMANȚĂ (speedup)                                       ║
║   • Exemplu: procesare imagini pe 8 core-uri                                  ║
║                                                                               ║
║        CPU 0:  ████████████████████████                                       ║
║        CPU 1:  ████████████████████████                                       ║
║        CPU 2:  ████████████████████████                                       ║
║        CPU 3:  ████████████████████████                                       ║
║                 ─────────────────────────►  timp                              ║
║                      (paralel real)                                           ║
║                                                                               ║
╚═══════════════════════════════════════════════════════════════════════════════╝
```

**În C și POSIX:**
- Concurența se realizează prin **thread-uri** (lightweight processes)
- Paralelismul necesită multiple **CPU cores** + thread-uri
- Un program poate fi concurrent fără a fi paralel
- Paralelismul implică întotdeauna concurență

### 2. POSIX Threads (pthreads)

Standardul POSIX.1c (1995) definește interfața pentru thread-uri în sistemele UNIX-like:

```c
/* Structura de bază a unui thread POSIX */
#include <pthread.h>

pthread_t thread_id;              /* Identificatorul thread-ului */
pthread_attr_t attr;              /* Atribute (stack size, etc.) */
pthread_mutex_t mutex;            /* Mutex pentru excludere mutuală */
pthread_cond_t cond;              /* Condition variable */
sem_t semaphore;                  /* Semafor POSIX */
```

**Ciclul de viață al unui thread:**

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                        THREAD LIFECYCLE                                      │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   ┌──────────┐   pthread_create()   ┌──────────┐                           │
│   │  START   │─────────────────────►│  READY   │◄─────────────┐            │
│   └──────────┘                      └────┬─────┘              │            │
│                                          │                    │            │
│                                   scheduler│             pthread_yield()   │
│                                          ▼                    │            │
│                                    ┌──────────┐               │            │
│                                    │ RUNNING  │───────────────┘            │
│                                    └────┬─────┘                            │
│                           ┌─────────────┼─────────────┐                    │
│                           │             │             │                    │
│                  pthread_mutex_lock() return/exit  detach                  │
│                   (blocked)             │                                  │
│                           ▼             ▼             ▼                    │
│                    ┌──────────┐   ┌──────────┐  ┌──────────┐              │
│                    │ BLOCKED  │   │TERMINATED│  │ DETACHED │              │
│                    └────┬─────┘   └──────────┘  └──────────┘              │
│                         │               │                                  │
│                  unlock │        pthread_join()                            │
│                         │               │                                  │
│                         └───────────────►                                  │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 3. Race Conditions

O **race condition** apare când rezultatul unui program depinde de ordinea relativă a execuției operațiilor din thread-uri diferite:

```c
/* EXEMPLU CLASIC DE RACE CONDITION */
int counter = 0;  /* Variabilă partajată */

void* increment(void* arg) {
    for (int i = 0; i < 1000000; i++) {
        counter++;  /* RACE CONDITION! */
        /*
         * counter++ se traduce în:
         *   1. LOAD  counter -> register
         *   2. ADD   register + 1
         *   3. STORE register -> counter
         *
         * Între acești pași, alt thread poate interveni!
         */
    }
    return NULL;
}

/* Cu 2 thread-uri:
 * Așteptat: counter = 2,000,000
 * Rezultat: counter ∈ [1,000,000 .. 2,000,000] (nedeterminist!)
 */
```

**Secvență problematică (interleaving):**

```
Thread A                Thread B                counter
─────────────────────────────────────────────────────────
LOAD counter (0)                                   0
                        LOAD counter (0)           0
ADD 1 (reg=1)                                      0
                        ADD 1 (reg=1)              0
STORE 1                                            1
                        STORE 1                    1   ← PIERDERE!
```

### 4. Primitive de Sincronizare

#### 4.1 Mutex (Mutual Exclusion)

```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* safe_increment(void* arg) {
    for (int i = 0; i < 1000000; i++) {
        pthread_mutex_lock(&mutex);    /* Intrare secțiune critică */
        counter++;                      /* Operație protejată */
        pthread_mutex_unlock(&mutex);  /* Ieșire secțiune critică */
    }
    return NULL;
}
```

#### 4.2 Semaphore

```c
#include <semaphore.h>

sem_t sem;
sem_init(&sem, 0, N);  /* Semafor cu N permise */

sem_wait(&sem);  /* P(sem): decrementare sau blocare dacă 0 */
/* Secțiune critică */
sem_post(&sem);  /* V(sem): incrementare și deblocare waiter */
```

#### 4.3 Condition Variables

```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int data_ready = 0;

/* Thread producător */
pthread_mutex_lock(&mutex);
data_ready = 1;
pthread_cond_signal(&cond);  /* Semnalează un waiter */
pthread_mutex_unlock(&mutex);

/* Thread consumator */
pthread_mutex_lock(&mutex);
while (!data_ready) {  /* WHILE, nu IF - spurious wakeups! */
    pthread_cond_wait(&cond, &mutex);  /* Eliberează mutex + așteaptă */
}
/* Procesează data */
pthread_mutex_unlock(&mutex);
```

### 5. Deadlock

**Deadlock** apare când două sau mai multe thread-uri așteaptă permanent resurse deținute de celelalte.

**Cele 4 condiții Coffman** (necesare și suficiente):

```
╔═══════════════════════════════════════════════════════════════════════════════╗
║                        CONDIȚII COFFMAN PENTRU DEADLOCK                        ║
╠═══════════════════════════════════════════════════════════════════════════════╣
║                                                                               ║
║   1. MUTUAL EXCLUSION (Excludere mutuală)                                     ║
║      Resursa poate fi deținută de un singur thread la un moment dat           ║
║                                                                               ║
║   2. HOLD AND WAIT (Deținere și așteptare)                                    ║
║      Thread-ul deține resurse și așteaptă altele                              ║
║                                                                               ║
║   3. NO PREEMPTION (Fără preempție)                                           ║
║      Resursa nu poate fi luată forțat de la deținător                         ║
║                                                                               ║
║   4. CIRCULAR WAIT (Așteptare circulară)                                      ║
║      T1 → T2 → T3 → ... → Tn → T1                                            ║
║                                                                               ║
╠═══════════════════════════════════════════════════════════════════════════════╣
║   Pentru a PREVENI deadlock: elimină cel puțin o condiție!                    ║
╚═══════════════════════════════════════════════════════════════════════════════╝
```

**Exemplu clasic - Dining Philosophers:**

```
                    🍴 Fork 0 🍴
              🧑 Phil 0      🧑 Phil 1
           🍴 Fork 4              🍴 Fork 1
              🧑 Phil 4      🧑 Phil 2
                    🍴 Fork 3 🍴
                    🧑 Phil 3
                    
    Dacă fiecare filosof ia furculița din stânga simultan:
    Phil0 holds Fork0, waits Fork1
    Phil1 holds Fork1, waits Fork2
    Phil2 holds Fork2, waits Fork3
    Phil3 holds Fork3, waits Fork4
    Phil4 holds Fork4, waits Fork0   ← CICLU = DEADLOCK!
```

### 6. C11 Atomics

C11 introduce operații atomice garantate hardware:

```c
#include <stdatomic.h>

atomic_int counter = 0;

/* Incrementare atomică - nu necesită mutex! */
atomic_fetch_add(&counter, 1);

/* Compare-and-swap (CAS) - fundația lock-free */
int expected = 10;
atomic_compare_exchange_strong(&counter, &expected, 20);
/* Dacă counter == 10: counter = 20, returnează true
 * Dacă counter != 10: expected = counter, returnează false */
```

### 7. Legea lui Amdahl - Formalizare

```
╔═══════════════════════════════════════════════════════════════════════════════╗
║                           LEGEA LUI AMDAHL                                     ║
╠═══════════════════════════════════════════════════════════════════════════════╣
║                                                                               ║
║                              1                                                ║
║   Speedup(n) = ─────────────────────────                                      ║
║                 (1 - P) + P/n                                                 ║
║                                                                               ║
║   unde: P = fracțiunea paralelizabilă                                         ║
║         n = numărul de thread-uri/procesoare                                  ║
║                                                                               ║
╠═══════════════════════════════════════════════════════════════════════════════╣
║   EXEMPLU: P = 0.95 (95% paralelizabil)                                       ║
║   ───────────────────────────────────────                                     ║
║   n = 1:    Speedup = 1.00x                                                   ║
║   n = 2:    Speedup = 1.90x                                                   ║
║   n = 4:    Speedup = 3.48x                                                   ║
║   n = 8:    Speedup = 5.93x                                                   ║
║   n = 16:   Speedup = 9.14x                                                   ║
║   n = 32:   Speedup = 12.55x                                                  ║
║   n = ∞:    Speedup = 20.00x (limita!)                                        ║
║                                                                               ║
║      Speedup                                                                  ║
║         │                                                                     ║
║      20 │─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ ─ limită teoretică     ║
║         │                                    ....····                         ║
║      15 │                         ....····                                    ║
║         │                  ..····                                             ║
║      10 │           ..····                                                    ║
║         │      .····                                                          ║
║       5 │  .···                                                               ║
║         │.·                                                                   ║
║       0 └──────────────────────────────────────────────► n                    ║
║           1    4    8   16   32   64  128  256                                ║
║                                                                               ║
╚═══════════════════════════════════════════════════════════════════════════════╝
```

---

## 🏭 Aplicații Industriale

### 1. Web Servers (nginx, Apache)

Serverele web moderne folosesc modele hibride thread/event-driven pentru a gestiona mii de conexiuni simultane.

### 2. Baze de Date (PostgreSQL, MySQL)

Sisteme de gestiune a bazelor de date folosesc thread pools pentru procesarea query-urilor în paralel, cu lock-uri fine-grained pentru accesul la pagini de date.

### 3. Game Engines (Unity, Unreal)

Motoarele de jocuri moderne paralelizează: fizică, AI, rendering, networking pe thread-uri separate cu sincronizare la frame boundary.

### 4. Procesare Big Data (Apache Spark)

Frameworks de procesare distribuită folosesc paradigma Map-Reduce pentru procesarea paralelă a datelor masive.

---

## 💻 Exerciții de Laborator

### Exercițiul 1: Log Processor cu Producer-Consumer

**Obiectiv**: Implementarea unui sistem de procesare log-uri folosind pattern-ul producer-consumer cu bounded buffer.

**Cerințe**:
1. Definirea structurii `BoundedBuffer` cu mutex și condition variables
2. N thread-uri producător care citesc fișiere log
3. M thread-uri consumator care procesează și agregă statistici
4. Terminare grațioasă când toate fișierele au fost procesate
5. Afișare statistici per thread (linii procesate, timp)

**Fișiere**: `src/exercise1.c`, `data/log_entries.txt`

### Exercițiul 2: Parallel Quick Sort

**Obiectiv**: Implementarea unui Quick Sort paralel cu threshold adaptiv pentru fallback la varianta secvențială.

**Cerințe**:
1. Quick Sort secvențial pentru comparație
2. Quick Sort paralel cu creare thread-uri pentru partiții
3. Threshold configurabil pentru a evita overhead-ul pe array-uri mici
4. Limitare număr maxim de thread-uri active
5. Benchmark comparativ și calcul speedup

**Fișiere**: `src/exercise2.c`, `data/large_numbers.txt`

---

## 🔧 Compilare și Execuție

```bash
# Construire toate targeturile
make

# Rulare exemplu demonstrativ
make run

# Compilare și rulare exercițiu 1
make exercise1
./exercise1

# Compilare și rulare exercițiu 2  
make exercise2
./exercise2

# Rulare benchmark
make benchmark

# Verificare memory leaks și race conditions
make valgrind
make helgrind

# Rulare teste automate
make test

# Curățare
make clean

# Ajutor
make help
```

---

## 🐳 Rulare în Docker

```bash
# Construiește imaginea
docker build -t atp-week20 .

# Rulează containerul interactiv
docker run -it --rm -v $(pwd):/workspace atp-week20

# În container:
make all && make run

# Sau cu docker-compose (include benchmark tools)
docker-compose up -d
docker-compose exec atp bash
make benchmark
```

---

## 📁 Structura Directorului

```
20-parallel-concurrent/
├── README.md                           # Acest fișier (500+ linii)
├── Makefile                            # Automatizare build (250+ linii)
├── Dockerfile                          # Container standalone
├── docker-compose.yml                  # Setup cu tools
│
├── slides/
│   ├── presentation-week20.html        # Prezentare principală
│   └── presentation-comparativ.html    # Comparație Pseudocod/C/Python
│
├── src/
│   ├── example1.c                      # Exemplu complet (~900 linii)
│   ├── exercise1.c                     # Producer-Consumer
│   └── exercise2.c                     # Parallel Quick Sort
│
├── data/
│   ├── large_numbers.txt               # 100K numere pentru sortare
│   └── log_entries.txt                 # Log simulat pentru procesare
│
├── benchmarks/
│   ├── benchmark_threads.c             # Benchmark paralel
│   ├── benchmark_sequential.c          # Benchmark secvențial
│   └── plot_results.py                 # Vizualizare rezultate
│
├── tests/
│   ├── test1_input.txt
│   ├── test1_expected.txt
│   ├── test2_input.txt
│   └── test2_expected.txt
│
├── python_comparison/
│   ├── threading_python.py             # Echivalent threading
│   └── multiprocessing_python.py       # Echivalent multiprocessing
│
├── teme/
│   ├── homework-requirements.md        # Cerințe teme (2 × 50p)
│   └── homework-extended.md            # Provocări bonus (5 × 10p)
│
└── solution/
    ├── exercise1_sol.c
    ├── exercise2_sol.c
    ├── homework1_sol.c
    └── homework2_sol.c
```

---

## 📖 Bibliografie Recomandată

### Resurse Esențiale

- **Butenhof, D.R.** - "Programming with POSIX Threads", Addison-Wesley, 1997
- **Herlihy, M. & Shavit, N.** - "The Art of Multiprocessor Programming", Morgan Kaufmann, 2012
- **Tanenbaum, A.S.** - "Modern Operating Systems", Cap. 2: Processes and Threads

### Resurse Avansate

- **Williams, A.** - "C++ Concurrency in Action", Manning, 2019
- **Goetz, B. et al.** - "Java Concurrency in Practice", Addison-Wesley, 2006
- **ISO/IEC 9899:2011** - C11 Standard, Section 7.17 (Atomics)

### Resurse Online

- [POSIX Threads Programming](https://computing.llnl.gov/tutorials/pthreads/)
- [Preshing on Programming - Lock-Free](https://preshing.com/archives/)
- [C11 Atomic Operations](https://en.cppreference.com/w/c/atomic)

---

## ✅ Lista de Auto-Verificare

Înainte de a considera săptămâna finalizată, verifică:

- [ ] Pot explica diferența dintre concurență și paralelism
- [ ] Știu să creez și să gestionez thread-uri cu pthread
- [ ] Înțeleg de ce mutex-urile sunt necesare și cum să le folosesc corect
- [ ] Pot identifica potențiale race conditions într-un cod dat
- [ ] Știu să implementez producer-consumer cu bounded buffer
- [ ] Înțeleg condițiile Coffman pentru deadlock
- [ ] Pot folosi condition variables corect (pattern-ul while)
- [ ] Știu să calculez speedup teoretic cu Legea lui Amdahl
- [ ] Pot implementa structuri lock-free simple cu C11 atomics
- [ ] Am înțeles când să folosesc mutex vs. spinlock vs. atomics

---

## 💼 Pregătire pentru Interviu

### Întrebări Frecvente

1. **Ce este o race condition și cum o previi?**
   
   O race condition apare când rezultatul depinde de timing-ul execuției thread-urilor. Se previne prin sincronizare: mutex pentru secțiuni critice, operații atomice pentru variabile simple, sau structuri lock-free.

2. **Care sunt condițiile necesare pentru deadlock?**
   
   Cele 4 condiții Coffman: (1) Excludere mutuală, (2) Deținere și așteptare, (3) Fără preempție, (4) Așteptare circulară. Eliminarea oricăreia previne deadlock-ul.

3. **Ce este Legea lui Amdahl?**
   
   Speedup(n) = 1 / ((1-P) + P/n), unde P este fracțiunea paralelizabilă. Stabilește limita teoretică a speedup-ului: dacă 5% este secvențial, speedup maxim = 20x.

4. **Când folosești mutex vs. spinlock vs. atomics?**
   
   Mutex: secțiuni critice lungi, posibilitate de blocking I/O. Spinlock: secțiuni foarte scurte, overhead context switch prea mare. Atomics: operații simple (increment, compare-swap) pe variabile individuale.

5. **Ce este un thread pool și de ce este util?**
   
   Colecție de thread-uri pre-create care procesează task-uri dintr-o coadă. Evită overhead-ul creării/distrugerii thread-urilor, permite controlul concurenței, reutilizează resurse.

---

## 🔗 Recapitulare și Perspectivă

### Conexiune cu Săptămânile Anterioare

Această săptămână reprezintă **capstone** al extensiilor 15-20:

- **Săpt. 15 (Sortare)**: Parallel Merge Sort demonstrează aplicarea directă
- **Săpt. 17 (Big Data)**: Structurile probabilistice beneficiază de lock-free updates
- **Săpt. 18 (ML)**: Antrenarea paralelă accelerează convergența
- **Săpt. 19 (IoT)**: Pipeline-urile de streaming sunt inherent concurente

### Direcții de Aprofundare

Pentru studenții interesați să continue explorarea:

1. **GPU Computing** - CUDA, OpenCL pentru paralelism masiv
2. **Distributed Systems** - Raft, Paxos pentru consens
3. **Reactive Programming** - RxJava, Project Reactor
4. **Actor Model** - Erlang, Akka pentru concurență message-passing
5. **Software Transactional Memory** - Alternative la lock-uri

---

## 🔧 Hardware Real (Opțional)

Kit-uri disponibile la Biblioteca Facultății pentru experimentare:

### ESP32 DevKit

- **Dual-core** Tensilica Xtensa LX6 @ 240MHz
- FreeRTOS integrat pentru multitasking real
- Perfect pentru demonstrarea concurenței pe sistem embedded

### Arduino Due

- ARM Cortex-M3 @ 84MHz
- DMA pentru transferuri paralele
- 96KB SRAM pentru buffere mai mari

**Avantaje hardware real:**
- Observare directă a efectelor timing-ului
- Debugging cu logic analyzer
- Înțelegerea interrupt-urilor și priority inversion

**Instrucțiuni de împrumut:** Contactați biblioteca cu carnetul de student.

---

*Acest material face parte din cursul "Algoritmi și Tehnici de Programare" (ATP)*
*Academia de Studii Economice din București - CSIE*
*Ultima actualizare: Ianuarie 2026*
