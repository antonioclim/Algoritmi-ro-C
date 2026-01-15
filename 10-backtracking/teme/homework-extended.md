# Provocări Extinse - Săptămâna 10

## 🚀 Provocări Avansate (Opțional)

Fiecare provocare rezolvată corect: **+10 puncte bonus**

---

## ⭐ Provocarea 1: Median Finder cu Două Heap-uri (Dificultate: Medie)

### Descriere

Implementați o structură de date care menține mediana curentă a unui stream de numere în timp O(log n) per inserare.

### Cerințe

```c
typedef struct {
    MaxHeap *lower;  /* Jumătatea inferioară */
    MinHeap *upper;  /* Jumătatea superioară */
} MedianFinder;

MedianFinder* mf_create(void);
void mf_destroy(MedianFinder *mf);
void mf_add_num(MedianFinder *mf, int num);
double mf_find_median(MedianFinder *mf);
```

### Logica

- `lower` (max-heap) conține jumătatea mică a numerelor
- `upper` (min-heap) conține jumătatea mare
- Invariant: |size(lower) - size(upper)| ≤ 1
- Mediana: dacă dimensiuni egale, media vârfurilor; altfel, vârful heap-ului mai mare

### Exemplu

```
Adaug 1: median = 1
Adaug 2: median = 1.5
Adaug 3: median = 2
Adaug 4: median = 2.5
```

### Puncte Bonus: +10

---

## ⭐ Provocarea 2: D-ary Heap (Dificultate: Medie-Avansată)

### Descriere

Implementați un heap d-ar (fiecare nod are d copii în loc de 2) și comparați performanța cu heap-ul binar pentru diverse valori ale lui d.

### Cerințe

```c
typedef struct {
    int *data;
    int size;
    int capacity;
    int d;  /* Numărul de copii per nod */
} DaryHeap;

DaryHeap* dary_create(int d, int capacity);
void dary_insert(DaryHeap *h, int key);
int dary_extract_max(DaryHeap *h);
void dary_decrease_key(DaryHeap *h, int i, int new_key);
```

### Formule de Navigare

- `parent(i) = (i - 1) / d`
- `child_k(i) = d * i + k + 1` pentru k ∈ {0, 1, ..., d-1}

### Experimentare

Măsurați timpul pentru:
- 100,000 inserări
- 100,000 extrageri
- Cu d = 2, 3, 4, 8, 16

### Puncte Bonus: +10

---

## ⭐ Provocarea 3: Merge K Sorted Lists (Dificultate: Medie)

### Descriere

Dată o listă de k liste sortate, îmbinați-le într-o singură listă sortată folosind un min-heap.

### Cerințe

```c
typedef struct ListNode {
    int val;
    struct ListNode *next;
} ListNode;

ListNode* merge_k_lists(ListNode **lists, int k);
```

### Algoritm

1. Creați un min-heap cu primele elemente din fiecare listă
2. Extrageți minimul și adăugați-l la rezultat
3. Dacă lista din care provine minimul mai are elemente, inserați următorul element în heap
4. Repetați până heap-ul este gol

### Complexitate

- Timp: O(n log k) unde n = total elemente
- Spațiu: O(k) pentru heap

### Puncte Bonus: +10

---

## ⭐ Provocarea 4: Heap Sort Iterativ (Dificultate: Medie)

### Descriere

Implementați heap sort complet iterativ (fără recursivitate în heapify).

### Cerințe

```c
void heapify_iterative(int arr[], int n, int i);
void heap_sort_iterative(int arr[], int n);
```

### Constrângeri

- NU folosiți recursivitate
- Demonstrați că funcționează corect cu teste
- Comparați performanța cu versiunea recursivă

### Hint

Înlocuiți recursivitatea cu o buclă `while` care actualizează indexul curent.

### Puncte Bonus: +10

---

## ⭐ Provocarea 5: Task Scheduler cu Deadline-uri (Dificultate: Avansată)

### Descriere

Implementați un scheduler care maximizează profitul din task-uri cu deadline-uri, folosind un heap.

### Problemă

Date n task-uri, fiecare cu:
- `profit[i]` - profitul dacă task-ul e completat
- `deadline[i]` - deadline (în unități de timp)

Fiecare task durează 1 unitate de timp. Găsiți subset-ul de task-uri care maximizează profitul total.

### Cerințe

```c
typedef struct {
    int id;
    int profit;
    int deadline;
} Task;

int max_profit_scheduling(Task tasks[], int n);
```

### Algoritm (Job Sequencing)

1. Sortați task-urile descrescător după profit
2. Găsiți deadline-ul maxim
3. Creați array de slot-uri libere
4. Pentru fiecare task, ocupați cel mai târziu slot disponibil ≤ deadline

### Exemplu

```
Tasks: [(1, 100, 2), (2, 19, 1), (3, 27, 2), (4, 25, 1), (5, 15, 3)]
Output: Profit maxim = 142 (task-urile 1, 3, 5)
```

### Puncte Bonus: +10

---

## 📊 Sistemul de Puncte Bonus

| Provocări Completate | Total Bonus |
|---------------------|-------------|
| 1 | +10 puncte |
| 2 | +20 puncte |
| 3 | +30 puncte |
| 4 | +40 puncte |
| Toate 5 | +50 puncte + Badge "Heap Master" 🏆 |

---

## 📋 Cerințe pentru Fiecare Provocare

1. **Cod funcțional** - Compilează și rulează corect
2. **Teste** - Includeți cazuri de test în main()
3. **Complexitate** - Comentați complexitatea temporală și spațială
4. **Documentație** - Explicați algoritmul în comentarii
5. **Memory safe** - Verificat cu Valgrind

---

## 📤 Predare Provocări

Fișiere separate pentru fiecare provocare:
- `bonus1_median.c`
- `bonus2_dary.c`
- `bonus3_merge_k.c`
- `bonus4_iterative.c`
- `bonus5_scheduler.c`

---

## 💡 Sfaturi Generale

1. **Începeți cu provocările mai simple** (1, 4)
2. **Testați cu cazuri limită** (array gol, un element, multe duplicate)
3. **Desenați diagramele** înainte de a codifica
4. **Folosiți printf pentru debugging** - afișați starea heap-ului
5. **Revedeți materialul** - Provocările testează înțelegerea profundă

---

## 🏆 Hall of Fame

Studenții care completează toate cele 5 provocări vor fi menționați în:
- Lista "Heap Masters" a cursului
- Referință pentru proiecte de cercetare
- Recomandări pentru internship-uri

---

*Succes! Aceste provocări vă vor pregăti pentru interviuri tehnice și probleme reale.*
