# Tema Săptămâna 8: Arbori Binari de Căutare

## 📋 Informații Generale

- **Termen limită:** Sfârșitul săptămânii 9
- **Punctaj:** 100 puncte (10% din nota finală)
- **Limbaj:** C (standard C11)
- **Compilator:** GCC cu flagurile `-Wall -Wextra -std=c11`

---

## 📝 Tema 1: Sistem de Gestionare a Studenților (50 puncte)

### Descriere

Implementați un sistem de gestionare a studenților pentru o universitate folosind un Binary Search Tree. Studenții sunt organizați după **numărul matricol** (cheie unică).

### Structura Student

```c
typedef struct Student {
    int nr_matricol;           /* Cheie unică de căutare */
    char nume[50];
    char prenume[50];
    char facultate[100];
    int an_studiu;             /* 1-4 */
    float medie;               /* 1.00 - 10.00 */
    struct Student *left;
    struct Student *right;
} Student;
```

### Cerințe Funcționale

1. **Inserare student** (10p)
   - Adăugați un student nou în BST
   - Mențineți ordinea după `nr_matricol`
   - Tratați cazul de duplicat (actualizare date)

2. **Căutare student** (5p)
   - Căutare după numărul matricol
   - Returnați studentul găsit sau mesaj de eroare

3. **Ștergere student** (10p)
   - Implementați corect toate cele 3 cazuri de ștergere
   - Eliberați memoria corespunzător

4. **Afișare studenți** (5p)
   - Afișare în ordine crescătoare după nr. matricol (inorder)
   - Format: `[NR_MATRICOL] NUME PRENUME - An X, Media: Y.YY`

5. **Statistici** (10p)
   - Numărul total de studenți
   - Media generală a tuturor studenților
   - Studentul cu cea mai mare medie (bursier)
   - Studenți pe ani de studiu (histogramă)

6. **Filtrare** (10p)
   - Afișați toți studenții cu media peste un prag dat
   - Afișați toți studenții dintr-o anumită facultate
   - Afișați studenții dintr-un interval de numere matricole (range query)

### Exemplu de Utilizare

```
$ ./homework1

=== SISTEM GESTIONARE STUDENȚI ===

1. Adaugă student
2. Caută student
3. Șterge student
4. Afișează toți studenții
5. Statistici
6. Filtrare
0. Ieșire

Opțiune: 1
Nr. matricol: 12345
Nume: Popescu
Prenume: Ion
Facultate: CSIE
An: 2
Medie: 8.75
Student adăugat cu succes!
```

### Fișier: `homework1_studenti.c`

---

## 📝 Tema 2: Indexare și Căutare Text (50 puncte)

### Descriere

Implementați un sistem de indexare pentru un fișier text mare. Sistemul construiește un BST unde fiecare nod conține un **cuvânt** și **lista de poziții** unde apare în text.

### Structura Index

```c
typedef struct Position {
    int line;
    int column;
    struct Position *next;
} Position;

typedef struct WordIndex {
    char word[100];
    int count;                  /* Număr de apariții */
    Position *positions;        /* Listă de poziții */
    struct WordIndex *left;
    struct WordIndex *right;
} WordIndex;
```

### Cerințe Funcționale

1. **Construire index** (15p)
   - Citiți un fișier text
   - Extrageți cuvintele (ignorați punctuația)
   - Convertiți la lowercase pentru uniformitate
   - Construiți BST-ul de indexare

2. **Căutare cuvânt** (10p)
   - Găsiți un cuvânt în index
   - Afișați numărul de apariții și pozițiile
   - Complexitate O(log n + k) unde k = număr poziții

3. **Top cuvinte** (10p)
   - Afișați cele mai frecvente N cuvinte
   - Hint: Colectați într-un array, sortați după count

4. **Căutare prefix** (10p)
   - Găsiți toate cuvintele care încep cu un prefix dat
   - Exemplu: prefix "pro" → "program", "profesor", "proiect"

5. **Statistici text** (5p)
   - Număr total de cuvinte unice
   - Număr total de cuvinte (inclusiv repetiții)
   - Cuvântul cel mai lung
   - Înălțimea arborelui de index

### Exemplu de Utilizare

```
$ ./homework2 document.txt

=== INDEX TEXT ===
Fișier: document.txt
Cuvinte unice: 1234
Cuvinte totale: 5678
Înălțime arbore: 12

Căutare: algoritm
  Găsit: 15 apariții
  Poziții: (5:12), (12:3), (45:7), ...

Top 5 cuvinte:
  1. "de" - 234 apariții
  2. "și" - 189 apariții
  3. "în" - 156 apariții
  ...
```

### Fișier: `homework2_index.c`

---

## 📊 Criterii de Evaluare

| Criteriu | Puncte |
|----------|--------|
| Corectitudine funcțională | 40 |
| Utilizare corectă BST | 25 |
| Gestiunea cazurilor limită | 15 |
| Calitatea codului | 10 |
| Fără warning-uri la compilare | 10 |

### Penalizări

- **-10p**: Warning-uri la compilare
- **-20p**: Memory leaks (verificate cu Valgrind)
- **-30p**: Crash la input valid
- **-50p**: Plagiat (ambele lucrări primesc 0)

---

## 📤 Predare

### Format

- Fișierele sursă: `homework1_studenti.c`, `homework2_index.c`
- Fișier README cu instrucțiuni de compilare și rulare
- (Opțional) Fișiere de test proprii

### Unde

- Platforma universitară de assignment
- SAU prin email la adresa indicată de profesor

### Verificare înainte de predare

```bash
# Compilare fără erori
gcc -Wall -Wextra -std=c11 -o homework1 homework1_studenti.c
gcc -Wall -Wextra -std=c11 -o homework2 homework2_index.c

# Verificare memory leaks
valgrind --leak-check=full ./homework1
valgrind --leak-check=full ./homework2 test.txt
```

---

## 💡 Sfaturi

1. **Începeți devreme!** BST-urile necesită atenție la pointeri și memorie.

2. **Testați incremental** - implementați și testați câte o funcție.

3. **Desenați pe hârtie** - vizualizați arborele pentru debugging.

4. **Folosiți Valgrind** frecvent pentru a detecta leak-urile din timp.

5. **Citiți documentația** - funcțiile standard C (`strcmp`, `strtok`, `fgets`) vă pot economisi timp.

6. **Tratați cazurile limită**:
   - Arbore gol
   - Un singur nod
   - Ștergere rădăcină
   - Fișier gol sau inexistent
   - Input invalid de la utilizator

---

*Succes!*

*Academia de Studii Economice din București - CSIE*
*Algoritmi și Tehnici de Programare*
