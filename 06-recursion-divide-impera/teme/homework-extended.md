# Provocări Extinse - Săptămâna 6

## 🚀 Provocări Avansate (Opționale)

Fiecare provocare rezolvată corect: **+10 puncte bonus**

---

## ⭐ Provocarea 1: Deque Generic cu Macro-uri (Dificultate: Medie)

### Descriere

Implementați un **double-ended queue (deque)** generic folosind macro-uri C pentru a permite stocarea oricărui tip de date.

### Cerințe

1. Definiți macro-uri pentru declararea structurii deque pentru un tip specific
2. Implementați toate cele 4 operații: push_front, push_back, pop_front, pop_back
3. Folosiți circular buffer ca implementare internă
4. Demonstrați utilizarea cu int, double și un struct personalizat

### Schelet de Cod

```c
// Macro pentru declararea tipului
#define DEQUE_DECLARE(TYPE, NAME) \
    typedef struct { \
        TYPE *data; \
        int front, rear, count, capacity; \
    } NAME##_deque_t; \
    \
    void NAME##_deque_init(NAME##_deque_t *dq, int cap); \
    bool NAME##_push_front(NAME##_deque_t *dq, TYPE item); \
    bool NAME##_push_back(NAME##_deque_t *dq, TYPE item); \
    bool NAME##_pop_front(NAME##_deque_t *dq, TYPE *result); \
    bool NAME##_pop_back(NAME##_deque_t *dq, TYPE *result); \
    void NAME##_deque_destroy(NAME##_deque_t *dq);

// Utilizare:
DEQUE_DECLARE(int, int)
DEQUE_DECLARE(double, double)
DEQUE_DECLARE(Point, point)  // struct Point { int x, y; }
```

### Puncte Bonus: +10

---

## ⭐ Provocarea 2: Queue cu Două Stacks (Dificultate: Medie)

### Descriere

Implementați o **queue** folosind **doar două stacks**. Aceasta este o întrebare clasică de interviu!

### Cerințe

1. Structura `TwoStackQueue` conține doar două stacks
2. Implementați enqueue și dequeue cu complexitate amortizată O(1)
3. Nu aveți voie să folosiți alte structuri de date auxiliare
4. Demonstrați funcționarea cu un set de operații

### Hint

- Stack-ul "input" primește elementele noi (enqueue)
- Stack-ul "output" oferă elementele pentru dequeue
- Când output e gol, transferați totul din input în output (inversează ordinea!)

### Complexitate Cerută

- enqueue: O(1)
- dequeue: O(1) amortizat (worst case O(n), dar amortizat O(1))

### Puncte Bonus: +10

---

## ⭐ Provocarea 3: Circular Buffer Lock-Free (Dificultate: Grea)

### Descriere

Implementați un **circular buffer thread-safe** fără mutex-uri, folosind doar operații atomice. Acesta este un pattern folosit în kernel-uri și sisteme real-time.

### Cerințe

1. Folosiți `<stdatomic.h>` pentru operații atomice
2. Un singur producător, un singur consumator (SPSC - Single Producer Single Consumer)
3. Nu folosiți mutex, semaphore sau alte primitive de sincronizare
4. Demonstrați cu două thread-uri: producer și consumer

### Schelet de Cod

```c
#include <stdatomic.h>

typedef struct {
    int *buffer;
    int capacity;
    atomic_size_t head;  // Scris doar de producer
    atomic_size_t tail;  // Scris doar de consumer
} LockFreeQueue;

// Producer scrie la head, citește tail
// Consumer citește head, scrie la tail
// Ordinea memory_order este crucială!
```

### Resurse Utile

- C11 memory model
- memory_order_acquire / memory_order_release
- Lamport's "Proving the Correctness of Multiprocess Programs"

### Puncte Bonus: +10

---

## ⭐ Provocarea 4: Hot Potato Simulation (Dificultate: Ușoară)

### Descriere

Implementați simularea jocului **"Hot Potato"** (Cartoful Fierbinte) folosind o queue circulară.

### Regulile Jocului

1. N jucători stau în cerc
2. Un "cartof" este pasat de la un jucător la altul
3. După exact K pase, jucătorul care ține cartoful este eliminat
4. Procesul continuă până rămâne un singur jucător (câștigătorul)

### Cerințe

1. Folosiți o queue pentru a simula cercul de jucători
2. Afișați fiecare eliminare
3. Returnați câștigătorul
4. Generalizați pentru orice N și K

### Exemplu

```
Jucători: Alice, Bob, Carol, David, Eve
K = 3

Runda 1: Eliminat Carol (pase: Alice→Bob→Carol)
Runda 2: Eliminat Alice (pase: David→Eve→Alice)
Runda 3: Eliminat Eve (pase: Bob→David→Eve)
Runda 4: Eliminat Bob (pase: David→Bob→David... wait)

Câștigător: David
```

### Notă Matematică

Pentru K=1, aceasta este problema lui **Josephus**! Există o formulă directă.

### Puncte Bonus: +10

---

## ⭐ Provocarea 5: LRU Cache cu Queue și HashMap (Dificultate: Grea)

### Descriere

Implementați un **LRU (Least Recently Used) Cache** eficient, combinând o **deque** cu un **hash table** pentru acces O(1).

### Cerințe

1. Operații `get(key)` și `put(key, value)` în O(1)
2. Când cache-ul e plin, elimină elementul cel mai puțin recent folosit
3. La fiecare acces, elementul devine "cel mai recent"
4. Capacitate configurabilă

### Structura Sugerată

```c
typedef struct LRUNode {
    int key;
    int value;
    struct LRUNode *prev;
    struct LRUNode *next;
} LRUNode;

typedef struct {
    int capacity;
    int size;
    LRUNode *head;  // Cel mai recent
    LRUNode *tail;  // Cel mai vechi
    LRUNode **hash_table;  // Pentru acces O(1)
    int hash_size;
} LRUCache;
```

### Operații

- `get(key)`: Returnează valoarea și mută nodul în față (MRU)
- `put(key, value)`: Adaugă/actualizează și mută în față; dacă e plin, elimină din coadă (LRU)

### Puncte Bonus: +10

---

## 📊 Sistem de Puncte Bonus

| Provocări Completate | Total Bonus |
|---------------------|-------------|
| 1 | +10 puncte |
| 2 | +20 puncte |
| 3 | +30 puncte |
| 4 | +40 puncte |
| Toate 5 | +50 puncte + Insigna "Queue Master" 🏆 |

---

## 📤 Predare Provocări

- Fișiere separate pentru fiecare provocare: `challenge1.c`, `challenge2.c`, etc.
- Includeți în fiecare fișier:
  - Comentarii explicative pentru algoritmul folosit
  - Funcție `main()` cu demonstrație
  - Analiza complexității (timp și spațiu)

---

## 💡 Sfaturi pentru Provocări

1. **Provocarea 3** necesită înțelegerea modelului de memorie C11 - citiți despre memory ordering înainte de a începe.

2. **Provocarea 5** combină mai multe concepte - asigurați-vă că înțelegeți hash tables înainte de a încerca.

3. Toate provocările pot fi testate local - scrieți teste comprehensive!

4. Dacă vă blocați, întrebați pe forum sau la consultații - provocările sunt menite să fie... provocatoare!

---

*Mult succes! Aceste provocări vă vor ajuta să vă pregătiți pentru interviuri tehnice.*
