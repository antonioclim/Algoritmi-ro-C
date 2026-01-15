# Săptămâna 5: Stive (Stack)

## 🎯 Obiective de Învățare

La finalul acestei săptămâni, studentul va fi capabil să:

1. **Recunoască** principiul LIFO (Last In, First Out) și să identifice scenarii în care stiva este structura de date optimă
2. **Înțeleagă** mecanismele interne ale operațiilor `push`, `pop`, `peek` și modul în care acestea modifică starea stivei
3. **Aplice** implementări de stive folosind atât vectori statici/dinamici cât și liste simplu înlănțuite
4. **Analizeze** complexitatea temporală și spațială a fiecărei implementări, identificând compromisurile (trade-offs) între acestea
5. **Evalueze** corectitudinea algoritmilor bazați pe stive prin testare sistematică și analiză a cazurilor limită
6. **Creeze** aplicații complete care utilizează stive pentru rezolvarea problemelor reale: evaluarea expresiilor, verificarea parantezelor, parcurgerea DFS

---

## 📜 Context Istoric

### Originea Conceptului

Conceptul de **stack** își are rădăcinile în matematica formală și teoria automatelor, fiind formalizat în anii 1950-1960. Termenul provine din analogia cu un teanc de farfurii (stack of plates) unde ultima farfurie pusă este prima care poate fi luată - principiul LIFO.

În anul 1946, **Alan Turing** a descris în lucrarea sa despre ACE (Automatic Computing Engine) un mecanism de "stivă de adrese de întoarcere" pentru gestionarea apelurilor de subrutine. Această idee fundamentală a dus la dezvoltarea conceptului de **call stack**, esențial în toate limbajele de programare moderne.

**Friedrich L. Bauer** și **Klaus Samelson** au primit în 1988 Premiul IEEE Computer Pioneer Award pentru "inventarea stivei" în contextul traducerii limbajelor de programare. Ei au demonstrat în 1957 cum stivele pot fi folosite pentru evaluarea expresiilor aritmetice și pentru implementarea compilatoarelor.

### Figura Cheie: Edsger W. Dijkstra (1930-2002)

Dijkstra, informatician olandez și laureat al Premiului Turing (1972), a contribuit semnificativ la formalizarea utilizării stivelor în algoritmică. Algoritmul său **Shunting Yard** (1961) folosește două stive pentru a converti expresii din notație infixă în notație postfixă (RPN - Reverse Polish Notation).

> *"Simplitatea este o mare virtute, dar necesită multă muncă pentru a o atinge și educație pentru a o aprecia. Și pentru a înrăutăți lucrurile: complexitatea se vinde mai bine."*
> — Edsger W. Dijkstra

---

## 📚 Fundamente Teoretice

### 1. Principiul LIFO și Operațiile Fundamentale

Stiva este o structură de date **abstractă** (Abstract Data Type - ADT) care respectă disciplina LIFO. Operațiile canonice sunt:

```
┌─────────────────────────────────────────────────────────────┐
│                    OPERAȚII PE STIVĂ                        │
├─────────────────────────────────────────────────────────────┤
│  push(x)  │ Adaugă elementul x în vârful stivei            │
│  pop()    │ Elimină și returnează elementul din vârf        │
│  peek()   │ Returnează elementul din vârf fără eliminare    │
│  isEmpty()│ Verifică dacă stiva este goală                  │
│  isFull() │ Verifică dacă stiva este plină (pt. vectori)    │
│  size()   │ Returnează numărul de elemente                  │
└─────────────────────────────────────────────────────────────┘

           ┌───┐
           │ 5 │ ← TOP (vârf)    push(5): se adaugă aici
           ├───┤                  pop():   se elimină de aici
           │ 3 │
           ├───┤
           │ 8 │
           ├───┤
           │ 1 │ ← BAZĂ
           └───┘
```

### 2. Implementare pe Vector (Array-Based Stack)

Această implementare folosește un vector de dimensiune fixă sau dinamică și un index `top` care indică poziția curentă a vârfului.

```c
#define MAX_SIZE 100

typedef struct {
    int data[MAX_SIZE];    // Vector de stocare
    int top;               // Index vârf (-1 = stivă goală)
} ArrayStack;

// Inițializare: top = -1 (stiva goală)
// Push: top++; data[top] = x;
// Pop:  return data[top--];
```

**Diagrama memoriei:**

```
Adresă    Index    Conținut     
────────────────────────────────
0x1000    [0]      1           
0x1004    [1]      8           
0x1008    [2]      3           
0x100C    [3]      5  ← top=3  
0x1010    [4]      ?  (nedefinit)
   ...     ...     ...
0x118C    [99]     ?           
────────────────────────────────
```

### 3. Implementare pe Listă Înlănțuită (Linked Stack)

Această implementare folosește noduri alocate dinamic, unde vârful stivei este întotdeauna capul listei.

```c
typedef struct Node {
    int data;
    struct Node *next;
} Node;

typedef struct {
    Node *top;    // Pointer la vârf (NULL = stivă goală)
    size_t size;  // Număr de elemente (opțional)
} LinkedStack;
```

**Diagrama structurii:**

```
LinkedStack              Noduri alocate dinamic
┌─────────┐             ┌──────┬──────┐   ┌──────┬──────┐
│ top ────┼────────────►│  5   │ next─┼──►│  3   │ next─┼──►...──► NULL
└─────────┘             └──────┴──────┘   └──────┴──────┘
                         (vârf/TOP)
```

### 4. Analiza Complexității

| Operație | Vector (static) | Vector (dinamic) | Listă înlănțuită |
|----------|-----------------|------------------|------------------|
| push()   | O(1)            | O(1) amortizat   | O(1)             |
| pop()    | O(1)            | O(1)             | O(1)             |
| peek()   | O(1)            | O(1)             | O(1)             |
| isEmpty()| O(1)            | O(1)             | O(1)             |
| Spațiu   | O(n) fix        | O(n) variabil    | O(n) + overhead  |

**Trade-offs:**

- **Vector static**: Rapid, fără overhead de pointeri, dar dimensiune fixă
- **Vector dinamic**: Flexibil, dar realocare costisitoare când se umple
- **Listă înlănțuită**: Dimensiune nelimitată, dar overhead de memorie (pointeri) și cache miss-uri

---

## 🏭 Aplicații Industriale

### 1. Call Stack în Sistemele de Operare

Fiecare thread de execuție are propriul **call stack** care stochează:
- Adresa de întoarcere (return address)
- Parametrii funcției
- Variabilele locale

```c
// Când apelăm o funcție:
void func_a(void) {
    int local_a = 10;     // Pe stack
    func_b();             // Push return address pe stack
}                         // Pop la return

// Call stack:
// ┌────────────────┐
// │ func_b frame   │ ← Stack Pointer (SP)
// ├────────────────┤
// │ func_a frame   │
// ├────────────────┤
// │ main frame     │
// └────────────────┘ ← Stack Base
```

### 2. Evaluarea Expresiilor și Calculatoare

Calculatoarele HP și limbajul PostScript folosesc **Reverse Polish Notation** (RPN) cu stive:

```c
// Expresia infixă: (3 + 4) * 5
// Expresia postfixă (RPN): 3 4 + 5 *

// Evaluare RPN cu stivă:
// Input: 3 → push(3)           Stack: [3]
// Input: 4 → push(4)           Stack: [3, 4]
// Input: + → pop, pop, push(7) Stack: [7]
// Input: 5 → push(5)           Stack: [7, 5]
// Input: * → pop, pop, push(35) Stack: [35]
// Rezultat: 35
```

### 3. Funcția Undo/Redo în Editoare

```c
typedef struct {
    Stack *undo_stack;    // Istoricul acțiunilor
    Stack *redo_stack;    // Acțiuni anulate
    Document *doc;
} Editor;

void perform_action(Editor *e, Action *a) {
    apply_action(e->doc, a);
    push(e->undo_stack, a);
    clear(e->redo_stack);  // Șterge redo la acțiune nouă
}

void undo(Editor *e) {
    Action *a = pop(e->undo_stack);
    reverse_action(e->doc, a);
    push(e->redo_stack, a);
}
```

### 4. Parsarea HTML/XML

Verificarea corectitudinii tag-urilor folosind stive:

```c
// Input: <html><body><p>Text</p></body></html>

// Procesare:
// <html>  → push("html")         Stack: [html]
// <body>  → push("body")         Stack: [html, body]
// <p>     → push("p")            Stack: [html, body, p]
// </p>    → pop, verifică "p"    Stack: [html, body]
// </body> → pop, verifică "body" Stack: [html]
// </html> → pop, verifică "html" Stack: []
// Rezultat: Document valid (stiva goală la final)
```

### 5. Algoritmul DFS (Depth-First Search)

```c
void dfs_iterativ(Graph *g, int start) {
    bool visited[MAX_VERTICES] = {false};
    Stack s;
    stack_init(&s);
    
    push(&s, start);
    
    while (!stack_is_empty(&s)) {
        int v = pop(&s);
        
        if (!visited[v]) {
            visited[v] = true;
            process_vertex(v);
            
            // Adaugă vecinii nevizitați
            for (int i = g->adj_count[v] - 1; i >= 0; i--) {
                int neighbor = g->adj[v][i];
                if (!visited[neighbor]) {
                    push(&s, neighbor);
                }
            }
        }
    }
}
```

---

## 💻 Exerciții de Laborator

### Exercițiul 1: Verificator de Paranteze

**Obiectiv:** Implementarea unui verificator care determină dacă o expresie are parantezele corect închise.

**Cerințe:**
1. Suportă trei tipuri de paranteze: `()`, `[]`, `{}`
2. Verifică echilibrarea și ordinea corectă
3. Returnează poziția primei erori sau -1 pentru succes
4. Gestionează șiruri goale și fără paranteze

**Exemple:**
- `"{[()]}"` → Valid
- `"{[(])}"` → Invalid (poziția 3)
- `"((("` → Invalid (paranteze neînchise)

### Exercițiul 2: Convertor Infix → Postfix

**Obiectiv:** Implementarea algoritmului Shunting Yard pentru convertirea expresiilor.

**Cerințe:**
1. Suportă operatorii: `+`, `-`, `*`, `/`, `^`
2. Respectă precedența și asociativitatea operatorilor
3. Gestionează paranteze
4. Funcționează cu operanzi multi-caracter (numere, variabile)

**Prioritate operatori:**
| Operator | Prioritate | Asociativitate |
|----------|------------|----------------|
| ^        | 3          | Dreapta        |
| *, /     | 2          | Stânga         |
| +, -     | 1          | Stânga         |

---

## 🔧 Compilare și Execuție

```bash
# Compilare exemplu complet
make example1
./example1

# Compilare și rulare exerciții
make exercise1
./exercise1

make exercise2
./exercise2

# Verificare memorie cu Valgrind
make valgrind

# Rulare teste automate
make test

# Curățare fișiere binare
make clean
```

---

## 📁 Structura Directorului

```
week-05-stive/
├── README.md                           # Acest fișier
├── Makefile                            # Automatizare compilare
│
├── slides/
│   ├── presentation-week05.html        # Prezentare principală (35+ slide-uri)
│   └── presentation-comparativ.html    # Comparație Pseudocod/C/Python
│
├── src/
│   ├── example1.c                      # Exemplu complet demonstrativ
│   ├── exercise1.c                     # Verificator de paranteze (TODO)
│   └── exercise2.c                     # Convertor Infix→Postfix (TODO)
│
├── data/
│   ├── expressions.txt                 # Expresii de test pentru evaluare
│   └── brackets.txt                    # Șiruri de paranteze pentru validare
│
├── tests/
│   ├── test1_input.txt                 # Input test exercițiul 1
│   ├── test1_expected.txt              # Output așteptat exercițiul 1
│   ├── test2_input.txt                 # Input test exercițiul 2
│   └── test2_expected.txt              # Output așteptat exercițiul 2
│
├── teme/
│   ├── homework-requirements.md        # Cerințe teme (2 × 50 puncte)
│   └── homework-extended.md            # Provocări bonus (5 × 10 puncte)
│
└── solution/
    ├── exercise1_sol.c                 # Soluție exercițiul 1
    ├── exercise2_sol.c                 # Soluție exercițiul 2
    ├── homework1_sol.c                 # Soluție tema 1
    └── homework2_sol.c                 # Soluție tema 2
```

---

## 📖 Lectură Recomandată

### Esențiale
- **Cormen, T.H. et al.** - *Introduction to Algorithms* (CLRS), Capitolul 10.1: Stacks and Queues
- **Sedgewick, R.** - *Algorithms in C*, Parts 1-4, Capitolul 4: Abstract Data Types

### Avansate
- **Knuth, D.E.** - *The Art of Computer Programming*, Vol. 1, Secțiunea 2.2.1: Stacks
- **Aho, A.V., Ullman, J.D.** - *Principles of Compiler Design*, Capitolele despre parsare

### Resurse Online
- [Visualgo - Vizualizare Stive](https://visualgo.net/en/list) - Animații interactive
- [GeeksforGeeks - Stack Data Structure](https://www.geeksforgeeks.org/stack-data-structure/)
- [CP-Algorithms - Stack Applications](https://cp-algorithms.com/)

---

## ✅ Lista de Auto-Evaluare

Înainte de a considera săptămâna completă, verifică că poți:

- [ ] Explica principiul LIFO și diferența față de FIFO
- [ ] Implementa o stivă pe vector fără a consulta notițele
- [ ] Implementa o stivă pe listă înlănțuită de la zero
- [ ] Analiza când să folosești vector vs. listă înlănțuită
- [ ] Scrie algoritmul de verificare a parantezelor
- [ ] Converti expresii din notație infixă în postfixă manual
- [ ] Evalua expresii postfixe folosind o stivă
- [ ] Identifica și corecta memory leaks în implementările cu pointeri
- [ ] Explica rolul call stack-ului în execuția programelor
- [ ] Aplica stive pentru implementarea DFS iterativ

---

## 💼 Pregătire pentru Interviuri

### Întrebări Frecvente

1. **"Implementează o stivă care suportă operația getMin() în O(1)."**
   - Soluție: Folosește o stivă auxiliară pentru minime

2. **"Cum ai implementa o coadă folosind două stive?"**
   - Soluție: O stivă pentru push, una pentru pop, transfer la nevoie

3. **"Verifică dacă un șir de paranteze este valid."**
   - Soluție: Stivă cu push pentru deschidere, pop+verificare pentru închidere

4. **"Evaluează o expresie postfixă."**
   - Soluție: Push operanzi, la operator: pop, calculează, push rezultat

5. **"Care sunt dezavantajele unei stive implementate pe vector?"**
   - Răspuns: Dimensiune fixă (sau cost realocare), risipa de memorie dacă supradimensionată

---

## 🔗 Previzualizare Săptămâna Viitoare

**Săptămâna 6: Cozi (Queue)**

Vom explora structura de date complementară stivei - coada, care funcționează pe principiul FIFO (First In, First Out). Vom studia:
- Implementări pe vector circular și listă înlănțuită
- Double-ended queue (deque)
- Priority queue și heap
- Aplicații: BFS, simulări, sistemele de operare (task scheduling)

---

*Acest material face parte din cursul "Algoritmi și Tehnici de Programare" - Universitatea de Științe Economice, Facultatea de Cibernetică, Statistică și Informatică Economică.*
