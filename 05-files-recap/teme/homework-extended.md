# Provocări Extinse - Săptămâna 5

## 🚀 Provocări Avansate (Opționale)

Fiecare provocare rezolvată corect: **+10 puncte bonus**

---

## ⭐ Provocarea 1: MinStack - Stivă cu Minimum în O(1)

**Dificultate:** Medie

### Descriere

Implementează o structură de date `MinStack` care suportă toate operațiile standard ale stivei (push, pop, peek) plus o operație `getMin()` care returnează elementul minim din stivă, toate în timp O(1).

### Cerințe

```c
typedef struct MinStack MinStack;

MinStack* minstack_create(void);
void minstack_destroy(MinStack *ms);
void minstack_push(MinStack *ms, int value);
int minstack_pop(MinStack *ms);
int minstack_peek(MinStack *ms);
int minstack_get_min(MinStack *ms);  // O(1)!
bool minstack_is_empty(MinStack *ms);
```

### Exemple

```
minstack_push(5)  → min = 5
minstack_push(3)  → min = 3
minstack_push(7)  → min = 3
minstack_push(2)  → min = 2
minstack_pop()    → returns 2, min = 3
minstack_get_min() → returns 3
```

### Hint

Folosește o stivă auxiliară pentru a ține evidența minimelor. La fiecare push, dacă valoarea este mai mică sau egală cu minimul curent, push și în stiva de minime.

### Puncte Bonus: +10

---

## ⭐ Provocarea 2: Implementare Coadă cu Două Stive

**Dificultate:** Medie

### Descriere

Implementează o structură de date `Queue` (coadă FIFO) folosind DOAR două stive. Nu ai voie să folosești alte structuri de date auxiliare.

### Cerințe

```c
typedef struct QueueFromStacks Queue;

Queue* queue_create(void);
void queue_destroy(Queue *q);
void queue_enqueue(Queue *q, int value);  // Adaugă la coadă
int queue_dequeue(Queue *q);              // Scoate din coadă (FIFO)
int queue_front(Queue *q);                // Peek la primul element
bool queue_is_empty(Queue *q);
```

### Complexitate Așteptată

- `enqueue`: O(1)
- `dequeue`: O(1) amortizat
- `front`: O(1) amortizat

### Hint

Folosește o stivă pentru push și una pentru pop. Transferă elemente între ele doar când e necesar.

### Puncte Bonus: +10

---

## ⭐ Provocarea 3: Validare Expresii HTML

**Dificultate:** Medie-Dificilă

### Descriere

Implementează un validator pentru tag-uri HTML care verifică dacă toate tag-urile sunt corect închise și imbricate.

### Cerințe

```c
typedef enum {
    HTML_VALID,
    HTML_UNCLOSED_TAG,
    HTML_MISMATCHED_TAG,
    HTML_UNEXPECTED_CLOSE
} HTMLValidationResult;

HTMLValidationResult validate_html(const char *html, char *error_tag);
```

### Exemple

```c
validate_html("<html><body></body></html>")  → HTML_VALID
validate_html("<div><p></div></p>")          → HTML_MISMATCHED_TAG, error_tag="div"
validate_html("<span><b></span>")            → HTML_MISMATCHED_TAG, error_tag="b"
validate_html("<div><p></p>")                → HTML_UNCLOSED_TAG, error_tag="div"
```

### Tag-uri Self-Closing (nu necesită închidere)

`<br>`, `<hr>`, `<img>`, `<input>`, `<meta>`, `<link>`

### Puncte Bonus: +10

---

## ⭐ Provocarea 4: Cel Mai Mare Dreptunghi din Histogramă

**Dificultate:** Dificilă

### Descriere

Dat fiind un vector de înălțimi reprezentând o histogramă, găsește aria celui mai mare dreptunghi care poate fi format în histogramă.

### Cerințe

```c
int largest_rectangle_area(int heights[], int n);
```

### Exemplu

```
heights = [2, 1, 5, 6, 2, 3]

    █
   ██
   ██
   ██ █
 █ ████
 ██████
 ------
 2 1 5 6 2 3

Răspuns: 10 (dreptunghiul 5×2 format de coloanele cu înălțimi 5 și 6)
```

### Complexitate Așteptată

O(n) folosind stive!

### Hint

Pentru fiecare bară, găsește cât de mult se poate extinde la stânga și la dreapta. Folosește o stivă pentru a menține indicii barelor în ordine crescătoare a înălțimii.

### Puncte Bonus: +10

---

## ⭐ Provocarea 5: Următorul Element Mai Mare

**Dificultate:** Medie

### Descriere

Pentru fiecare element dintr-un vector, găsește primul element mai mare care apare la dreapta sa. Dacă nu există, returnează -1.

### Cerințe

```c
void next_greater_element(int arr[], int n, int result[]);
```

### Exemplu

```
arr    = [4, 5, 2, 25, 7, 8]
result = [5, 25, 25, -1, 8, -1]

Explicație:
- 4 → următorul mai mare este 5
- 5 → următorul mai mare este 25
- 2 → următorul mai mare este 25
- 25 → nu există → -1
- 7 → următorul mai mare este 8
- 8 → nu există → -1
```

### Complexitate Așteptată

O(n) folosind o stivă!

### Hint

Parcurge vectorul de la dreapta la stânga. Menține o stivă cu candidații pentru "următorul mai mare".

### Puncte Bonus: +10

---

## 📊 Sistemul de Puncte Bonus

| Provocări Rezolvate | Total Bonus |
|---------------------|-------------|
| 1 | +10 puncte |
| 2 | +20 puncte |
| 3 | +30 puncte |
| 4 | +40 puncte |
| Toate 5 | +50 puncte + Badge 🏆 "Stack Master" |

---

## 📤 Instrucțiuni de Trimitere

1. **Fișiere separate:** `bonus1_minstack.c`, `bonus2_queue.c`, etc.
2. **Testează temeinic:** Include și un `main()` cu teste
3. **Comentează:** Explică algoritmul în comentarii
4. **Zero warnings:** `gcc -Wall -Wextra -std=c11`
5. **Zero leaks:** `valgrind --leak-check=full`

---

## 💡 Resurse Utile

- [LeetCode #155 - Min Stack](https://leetcode.com/problems/min-stack/)
- [LeetCode #232 - Implement Queue using Stacks](https://leetcode.com/problems/implement-queue-using-stacks/)
- [LeetCode #84 - Largest Rectangle in Histogram](https://leetcode.com/problems/largest-rectangle-in-histogram/)
- [LeetCode #496 - Next Greater Element I](https://leetcode.com/problems/next-greater-element-i/)

---

*Mult succes! Aceste provocări sunt pregătire excelentă pentru interviuri tehnice! 🎯*
