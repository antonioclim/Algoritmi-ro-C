# Tema Săptămâna 10: Heap-uri și Cozi cu Priorități

## 📋 Informații Generale

- **Termen limită:** Sfârșitul săptămânii 11
- **Punctaj:** 100 puncte (10% din nota finală)
- **Limbaj:** C (standard C11)
- **Compilator:** GCC cu `-Wall -Wextra -std=c11`

---

## 📝 Tema 1: Sistem de Gestionare a Proceselor (50 puncte)

### Descriere

Implementați un simulator simplificat de planificare a proceselor folosind o priority queue bazată pe max-heap. Sistemul trebuie să gestioneze procese cu diferite priorități și să le execute în ordinea priorității.

### Cerințe

1. **Structura Process** (5p)
   ```c
   typedef struct {
       int pid;           /* Process ID unic */
       char name[32];     /* Numele procesului */
       int priority;      /* Prioritate (0-100, 100 = cea mai mare) */
       int burst_time;    /* Timp de execuție în milisecunde */
       int arrival_time;  /* Timpul de sosire */
   } Process;
   ```

2. **Structura ProcessQueue** (5p)
   - Array dinamic de procese
   - Dimensiune și capacitate
   - Funcții de redimensionare automată

3. **Funcții obligatorii** (25p)
   - `ProcessQueue* pqueue_create(int capacity)` - Creare (3p)
   - `void pqueue_destroy(ProcessQueue *pq)` - Eliberare memorie (2p)
   - `void pqueue_insert(ProcessQueue *pq, Process p)` - Inserare (5p)
   - `Process pqueue_extract_max(ProcessQueue *pq)` - Extragere (5p)
   - `void pqueue_increase_priority(ProcessQueue *pq, int pid, int new_priority)` - Modificare prioritate (5p)
   - `void pqueue_print(ProcessQueue *pq)` - Afișare (5p)

4. **Simulare** (10p)
   - Citire procese de la stdin
   - Simulare execuție: extrage procesul cu cea mai mare prioritate, "execută" pentru burst_time
   - Afișare ordine de execuție și statistici

5. **Calitatea codului** (5p)
   - Comentarii și documentație
   - Gestionare erori
   - Stil consistent

### Exemplu de Utilizare

```
Input:
3
firefox 80 500 0
terminal 60 100 10
system_update 95 2000 5

Output:
=== Simulare Planificare Procese ===
Proces adăugat: system_update (prioritate: 95)
Proces adăugat: firefox (prioritate: 80)
Proces adăugat: terminal (prioritate: 60)

Ordine execuție:
1. system_update (prioritate: 95, timp: 2000ms)
2. firefox (prioritate: 80, timp: 500ms)
3. terminal (prioritate: 60, timp: 100ms)

Timp total: 2600ms
```

### Fișier: `homework1_procese.c`

---

## 📝 Tema 2: Algoritmul Kth Largest cu Heap (50 puncte)

### Descriere

Implementați o structură de date care menține eficient cele mai mari k elemente dintr-un stream de numere folosind un min-heap de dimensiune k.

### Cerințe

1. **Structura KthLargest** (5p)
   ```c
   typedef struct {
       int *heap;      /* Min-heap de dimensiune k */
       int k;          /* Numărul de elemente de păstrat */
       int size;       /* Dimensiunea curentă (≤ k) */
   } KthLargest;
   ```

2. **Funcții obligatorii** (30p)
   - `KthLargest* kth_create(int k, int *nums, int n)` - Creare și inițializare cu array (8p)
   - `void kth_destroy(KthLargest *kl)` - Eliberare (2p)
   - `int kth_add(KthLargest *kl, int val)` - Adaugă element și returnează al k-lea cel mai mare (10p)
   - `void kth_print(KthLargest *kl)` - Afișare heap (5p)
   - `int kth_get_kth_largest(KthLargest *kl)` - Returnează al k-lea (5p)

3. **Logica add()** (10p)
   - Dacă size < k: inserează în heap
   - Altfel: dacă val > heap[0] (minimul), înlocuiește și heapify
   - Returnează heap[0] (al k-lea cel mai mare)

4. **Program de test** (5p)
   - Testare cu diverse valori de k
   - Verificare corectitudine prin comparație cu sortare naivă

### Exemplu de Utilizare

```
Input:
k = 3
Array inițial: [4, 5, 8, 2]
Adăugări: 3, 5, 10, 9, 4

Output:
După inițializare (k=3): Min-heap = [4, 5, 8], kth = 4
Adaug 3: kth = 4 (3 nu intră în top 3)
Adaug 5: kth = 5 (noul top 3: 5, 5, 8)
Adaug 10: kth = 5 (top 3: 5, 8, 10)
Adaug 9: kth = 8 (top 3: 8, 9, 10)
Adaug 4: kth = 8 (top 3 rămâne: 8, 9, 10)
```

### Complexitate

- `kth_create()`: O(n log k) pentru construcție
- `kth_add()`: O(log k) per adăugare
- Spațiu: O(k)

### Fișier: `homework2_kth.c`

---

## 📊 Criterii de Evaluare

| Criteriu | Puncte |
|----------|--------|
| Corectitudine funcțională | 40 |
| Utilizare corectă heap | 25 |
| Gestionare cazuri limită | 15 |
| Calitatea codului | 10 |
| Fără warning-uri compilator | 10 |

### Penalizări

- -10p: Warning-uri la compilare
- -20p: Memory leaks (verificat cu Valgrind)
- -30p: Crash pe input valid
- -50p: Plagiat (copiere de la colegi sau surse online)

---

## 📤 Predare

1. Încărcați fișierele `homework1_procese.c` și `homework2_kth.c` pe platforma de curs
2. Asigurați-vă că programele compilează fără warning-uri
3. Verificați cu Valgrind că nu există memory leaks:
   ```bash
   valgrind --leak-check=full ./homework1_procese
   valgrind --leak-check=full ./homework2_kth
   ```

---

## 💡 Sfaturi

1. **Începeți cu structura de date** - Definiți clar heap-ul și operațiile
2. **Testați incremental** - Verificați fiecare funcție separat
3. **Folosiți macro-uri** - Pentru PARENT, LEFT_CHILD, RIGHT_CHILD
4. **Gestionați memoria** - Fiecare `malloc` trebuie să aibă un `free`
5. **Comentați codul** - Explicați logica complexă
6. **Citiți cerințele** - De două ori înainte de a scrie cod

---

## 🔗 Resurse Utile

- Curs: Săptămâna 10 - Heap-uri și Cozi cu Priorități
- Laborator: `src/example1.c` pentru exemple de implementare
- Prezentare: `slides/presentation-week10.html`
- Online: [Visualgo Heap](https://visualgo.net/en/heap)

---

*Acest material a fost creat pentru cursul „Algoritmi și Tehnici de Programare" - ASE București*
