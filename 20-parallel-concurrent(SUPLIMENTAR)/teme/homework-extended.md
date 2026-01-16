# =============================================================================
# PROVOCĂRI EXTINSE SĂPTĂMÂNA 20: Programare Paralelă și Concurentă
# =============================================================================
# Academia de Studii Economice București - CSIE
# Algoritmi și Tehnici de Programare (ATP)
# Aceste provocări sunt OPȚIONALE pentru studenții avansați
# =============================================================================

## 🏆 Provocări pentru Studenții Avansați

Aceste exerciții extinse sunt destinate studenților care doresc să aprofundeze 
conceptele de programare paralelă și concurentă. Fiecare provocare poate adăuga 
**până la 10 puncte bonus** la nota finală pe laborator.

---

## 🔥 Provocarea 1: Read-Write Lock (10p bonus)

### Obiectiv

Implementați un **Read-Write Lock** (RW Lock) de la zero, fără a folosi 
`pthread_rwlock_*`. Acest tip de lock permite:
- **Multiple readers** simultani (dacă nu există writers)
- **Un singur writer** exclusiv (fără readers sau alți writers)

### Specificații

```c
typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t readers_ok;
    pthread_cond_t writer_ok;
    int active_readers;
    int waiting_readers;
    int active_writers;
    int waiting_writers;
    bool writer_priority;   /* Dacă true, writers au prioritate */
} RWLock;

/* API */
RWLock* rwlock_create(bool writer_priority);
void rwlock_read_lock(RWLock *lock);
void rwlock_read_unlock(RWLock *lock);
void rwlock_write_lock(RWLock *lock);
void rwlock_write_unlock(RWLock *lock);
void rwlock_destroy(RWLock *lock);
```

### Cerințe

1. **Reader preference** vs **Writer preference**:
   - Reader preference: readers au prioritate → posibilă starvation writers
   - Writer preference: writers au prioritate → posibilă starvation readers

2. **Demonstrație**:
   - Simulați un cache cu operații frecvente de citire și rare de scriere
   - Afișați statistici: citiri/scrieri, timp așteptare

3. **Comparație**:
   - Benchmark: RWLock vs. Mutex simplu
   - Demonstrați avantajul pentru workload read-heavy

### Punctaj

| Criteriu | Puncte |
|----------|--------|
| Implementare corectă | 5p |
| Ambele moduri (reader/writer priority) | 2p |
| Demonstrație și benchmark | 2p |
| Documentație | 1p |

---

## 🔥 Provocarea 2: Barrier cu Faze (10p bonus)

### Obiectiv

Implementați o **barieră refolosibilă cu faze** care permite sincronizarea 
repetată a N thread-uri fără recreare.

### Specificații

```c
typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int count;          /* Câte thread-uri trebuie să ajungă */
    int waiting;        /* Câte au ajuns */
    int phase;          /* Faza curentă (0, 1, 2, ...) */
} Barrier;

/* API */
Barrier* barrier_create(int count);
int barrier_wait(Barrier *b);  /* Returnează faza curentă */
void barrier_destroy(Barrier *b);
```

### Cerințe

1. **Refolosibilă**: După ce toate thread-urile au trecut, bariera se resetează
2. **Phase tracking**: Fiecare thread știe în ce fază se află
3. **Demonstrație**: Algoritm iterativ în faze (ex: Jacobi iteration)

### Aplicație: Simulare Căldură 2D

```
Simulează transferul de căldură într-o grilă 2D:
1. Faza 1: Fiecare thread calculează noile valori pentru porțiunea sa
2. Barrier: Sincronizare
3. Faza 2: Fiecare thread copiază valorile noi în matricea principală
4. Barrier: Sincronizare
5. Repetă până la convergență
```

### Punctaj

| Criteriu | Puncte |
|----------|--------|
| Barrier funcțională | 4p |
| Refolosibilă corect | 3p |
| Aplicație demonstrativă | 2p |
| Documentație | 1p |

---

## 🔥 Provocarea 3: Work Stealing Thread Pool (10p bonus)

### Obiectiv

Implementați un thread pool cu **work stealing** - fiecare worker are propria 
coadă, iar când rămâne fără task-uri, "fură" de la alți workers.

### Arhitectură

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                          WORK STEALING POOL                                 │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   Worker 0              Worker 1              Worker 2              Worker 3│
│   ┌──────────┐          ┌──────────┐          ┌──────────┐          ┌──────┐│
│   │ Task A   │◀─────────│  (empty) │ STEAL    │ Task D   │          │Task F││
│   │ Task B   │          │          │◀─────────│ Task E   │          │      ││
│   │ Task C   │          └──────────┘          └──────────┘          └──────┘│
│   └──────────┘                                                              │
│        │                                                                    │
│        ▼                                                                    │
│   Local work: pop from bottom (LIFO - cache friendly)                       │
│   Steal: pop from top of victim's queue (FIFO - load balance)               │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### Specificații

```c
typedef struct {
    Task **tasks;
    size_t capacity;
    size_t head;    /* Pentru steal (FIFO) */
    size_t tail;    /* Pentru local work (LIFO) */
    pthread_mutex_t mutex;
} DequePerWorker;

typedef struct {
    pthread_t *workers;
    DequePerWorker *queues;  /* Câte o coadă per worker */
    size_t num_workers;
    volatile bool shutdown;
} WorkStealingPool;

/* API */
WorkStealingPool* ws_pool_create(size_t num_workers);
void ws_pool_submit(WorkStealingPool *pool, Task task);
void ws_pool_shutdown(WorkStealingPool *pool);
```

### Cerințe

1. **Deque thread-safe**: Double-ended queue cu pop de la ambele capete
2. **Work stealing**: Când coada locală e goală, fură de la un vecin aleator
3. **Load balancing**: Demonstrați distribuția uniformă a sarcinilor

### Demonstrație: Fibonacci Recursiv

```c
/* Fibonacci cu fork-join */
int fib(WorkStealingPool *pool, int n) {
    if (n < 2) return n;
    
    Future *f1 = ws_submit(pool, fib_task, n-1);
    Future *f2 = ws_submit(pool, fib_task, n-2);
    
    return future_get(f1) + future_get(f2);
}
```

### Punctaj

| Criteriu | Puncte |
|----------|--------|
| Deque per worker | 3p |
| Work stealing corect | 4p |
| Load balancing demonstrat | 2p |
| Documentație | 1p |

---

## 🔥 Provocarea 4: Lock-free Queue (10p bonus)

### Obiectiv

Implementați o **coadă FIFO lock-free** folosind doar operații atomice 
(fără mutex-uri), bazată pe algoritmul Michael & Scott.

### Structură

```c
#include <stdatomic.h>

typedef struct Node {
    void *value;
    _Atomic(struct Node*) next;
} Node;

typedef struct {
    _Atomic(Node*) head;
    _Atomic(Node*) tail;
} LockFreeQueue;

/* API */
LockFreeQueue* lfq_create(void);
void lfq_enqueue(LockFreeQueue *q, void *value);
void* lfq_dequeue(LockFreeQueue *q);  /* NULL dacă goală */
void lfq_destroy(LockFreeQueue *q);
```

### Algoritmul Michael & Scott (Simplificat)

```c
void lfq_enqueue(LockFreeQueue *q, void *value) {
    Node *node = malloc(sizeof(Node));
    node->value = value;
    atomic_store(&node->next, NULL);
    
    while (true) {
        Node *tail = atomic_load(&q->tail);
        Node *next = atomic_load(&tail->next);
        
        if (tail == atomic_load(&q->tail)) {  /* Verifică consistență */
            if (next == NULL) {
                if (atomic_compare_exchange_weak(&tail->next, &next, node)) {
                    atomic_compare_exchange_weak(&q->tail, &tail, node);
                    return;
                }
            } else {
                /* Tail a rămas în urmă, ajută-l să avanseze */
                atomic_compare_exchange_weak(&q->tail, &tail, next);
            }
        }
    }
}
```

### Cerințe

1. **Lock-free**: Niciun mutex, doar atomics
2. **ABA Problem**: Discutați și eventual rezolvați
3. **Benchmark**: Comparație cu coada bazată pe mutex

### Punctaj

| Criteriu | Puncte |
|----------|--------|
| Enqueue lock-free | 3p |
| Dequeue lock-free | 3p |
| ABA discutat/rezolvat | 2p |
| Benchmark vs. mutex | 2p |

---

## 🔥 Provocarea 5: Mini Actor Model (10p bonus)

### Obiectiv

Implementați un sistem simplu de **actori** - entități concurente care comunică 
exclusiv prin mesaje, fără memorie partajată directă.

### Concepte Actor Model

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                              ACTOR MODEL                                    │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   ┌─────────┐  Message   ┌─────────┐  Message   ┌─────────┐                │
│   │ Actor A │ ─────────▶ │ Actor B │ ─────────▶ │ Actor C │                │
│   │         │            │         │            │         │                │
│   │ Mailbox │            │ Mailbox │            │ Mailbox │                │
│   │ [M1,M2] │            │ [M3]    │            │ []      │                │
│   │         │            │         │            │         │                │
│   │ State   │            │ State   │            │ State   │                │
│   └─────────┘            └─────────┘            └─────────┘                │
│                                                                             │
│   Principii:                                                                │
│   • Un actor procesează UN mesaj la un moment dat                           │
│   • Starea internă e accesibilă DOAR de actorul însuși                     │
│   • Comunicarea se face EXCLUSIV prin mesaje                                │
│   • Procesarea mesajelor poate genera alte mesaje                           │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### Specificații

```c
/* Tipuri de mesaje */
typedef struct {
    int type;
    void *data;
    size_t data_size;
    struct Actor *sender;
} Message;

/* Actor */
typedef void (*MessageHandler)(struct Actor *self, Message *msg);

typedef struct Actor {
    char name[64];
    MessageHandler handler;
    void *state;
    MessageQueue *mailbox;
    pthread_t thread;
    volatile bool running;
} Actor;

/* Actor System */
typedef struct {
    Actor **actors;
    size_t num_actors;
    pthread_mutex_t registry_mutex;
} ActorSystem;

/* API */
ActorSystem* actor_system_create(void);
Actor* actor_spawn(ActorSystem *sys, const char *name, 
                   MessageHandler handler, void *initial_state);
void actor_send(Actor *to, Message *msg);
void actor_system_shutdown(ActorSystem *sys);
```

### Aplicație: Ping-Pong

```c
void ping_handler(Actor *self, Message *msg) {
    if (msg->type == MSG_PING) {
        printf("[%s] Received PING from %s\n", self->name, msg->sender->name);
        Message reply = {MSG_PONG, NULL, 0, self};
        actor_send(msg->sender, &reply);
    }
}

void pong_handler(Actor *self, Message *msg) {
    if (msg->type == MSG_PONG) {
        printf("[%s] Received PONG from %s\n", self->name, msg->sender->name);
        int *count = (int*)self->state;
        if (++(*count) < 10) {
            Message reply = {MSG_PING, NULL, 0, self};
            actor_send(msg->sender, &reply);
        }
    }
}
```

### Aplicație Avansată: Ring de Actori

```
Creați un ring de N actori unde:
1. Actor 0 trimite un mesaj cu un contor=M
2. Fiecare actor decrementează și trimite mai departe
3. Când contorul ajunge la 0, se afișează numărul de hop-uri
```

### Punctaj

| Criteriu | Puncte |
|----------|--------|
| Actor spawn/destroy | 2p |
| Message queue per actor | 3p |
| Actor send/receive | 3p |
| Aplicație demonstrativă | 2p |

---

## 📊 Sistem de Punctare Total

| Provocare | Dificultate | Puncte Bonus |
|-----------|-------------|--------------|
| Read-Write Lock | ⭐⭐⭐ | 10p |
| Barrier cu Faze | ⭐⭐ | 10p |
| Work Stealing | ⭐⭐⭐⭐ | 10p |
| Lock-free Queue | ⭐⭐⭐⭐⭐ | 10p |
| Actor Model | ⭐⭐⭐⭐ | 10p |

**Maximum posibil**: 50 puncte bonus (echivalent cu o temă întreagă)

---

## 📚 Resurse pentru Provocări

### Lock-free Programming
- **Michael & Scott** - *Simple, Fast, and Practical Non-Blocking Concurrent Queue Algorithms* (1996)
- **Herlihy** - *A Methodology for Implementing Highly Concurrent Data Objects* (1993)

### Actor Model
- **Hewitt, Bishop, Steiger** - *A Universal Modular Actor Formalism* (1973)
- **Armstrong** - *Making reliable distributed systems in the presence of software errors* (2003) - Erlang OTP

### Work Stealing
- **Blumofe & Leiserson** - *Scheduling Multithreaded Computations by Work Stealing* (1999)

---

## ⚠️ Recomandări

1. **Începeți simplu**: Implementați mai întâi o versiune cu lock-uri, apoi optimizați
2. **Testați extensiv**: Race conditions apar sub load mare
3. **Folosiți sanitizers**: `-fsanitize=thread` este prietenul vostru
4. **Documentați**: Explicați deciziile de design și problemele întâmpinate

---

*Material pentru cursul "Algoritmi și Tehnici de Programare" (ATP)*
*Academia de Studii Economice din București - CSIE*
*Actualizat: Ianuarie 2026*
