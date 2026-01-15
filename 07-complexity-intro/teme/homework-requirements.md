# Teme Săptămâna 7: Arbori Binari (Binary Trees)

## 📋 Informații Generale

- **Deadline:** Sfârșitul săptămânii 8
- **Punctaj:** 100 puncte (10% din nota finală)
- **Limbaj:** C (standard C11)
- **Compilator:** GCC cu `-Wall -Wextra -std=c11`

---

## 📝 Tema 1: Sistem de Gestionare a Fișierelor (50 puncte)

### Descriere

Implementați un sistem simplificat de gestionare a fișierelor folosind arbori binari. Sistemul va permite crearea, ștergerea și navigarea într-o structură ierarhică de directoare și fișiere.

### Cerințe

1. **Structura de date (10 puncte)**
   - Definiți o structură `FSNode` care conține:
     - Numele fișierului/directorului (max 64 caractere)
     - Tipul (0 = fișier, 1 = director)
     - Dimensiunea în bytes (pentru fișiere)
     - Data creării (timestamp)
     - Pointeri către `firstChild` și `nextSibling`

2. **Operații de bază (15 puncte)**
   - `createNode(name, type, size)` - creează un nod nou
   - `addChild(parent, child)` - adaugă un copil la un director
   - `findNode(root, path)` - găsește un nod după cale (ex: "/home/user/doc.txt")
   - `deleteNode(root, path)` - șterge un nod și toți descendenții

3. **Afișare și statistici (15 puncte)**
   - `listDirectory(node, depth)` - afișează conținutul recursiv cu indentare
   - `calculateSize(node)` - calculează dimensiunea totală (recursiv)
   - `countFiles(node)` - numără toate fișierele
   - `countDirectories(node)` - numără toate directoarele

4. **Căutare (10 puncte)**
   - `searchByName(root, name)` - găsește toate nodurile cu un anumit nume
   - `searchByExtension(root, ext)` - găsește fișierele cu o anumită extensie

### Exemplu de Utilizare

```c
FSNode *root = createNode("/", 1, 0);
FSNode *home = createNode("home", 1, 0);
FSNode *user = createNode("user", 1, 0);
FSNode *doc = createNode("document.txt", 0, 1024);

addChild(root, home);
addChild(home, user);
addChild(user, doc);

listDirectory(root, 0);
// Output:
// /
//   home/
//     user/
//       document.txt (1024 bytes)

printf("Total size: %lu bytes\n", calculateSize(root));
```

### Fișier: `homework1_filesystem.c`

---

## 📝 Tema 2: Calculator de Expresii cu Variabile (50 puncte)

### Descriere

Extindeți arborele de expresii pentru a suporta variabile și funcții matematice simple. Calculatorul va putea evalua expresii complexe cu valori variabile.

### Cerințe

1. **Structura extinsă (10 puncte)**
   - Extindeți `ExprNode` pentru a suporta:
     - Numere (int și double)
     - Variabile (nume de maxim 16 caractere)
     - Operatori binari (+, -, *, /, ^, %)
     - Funcții unare (sin, cos, sqrt, abs, neg)

2. **Parser pentru notație infix (15 puncte)**
   - Implementați algoritmul Shunting Yard
   - Suportați paranteze pentru modificarea priorității
   - Respectați precedența operatorilor: ^ > * / % > + -
   - Asociativitate: ^ este right-to-left, restul left-to-right

3. **Evaluare cu variabile (15 puncte)**
   - `setVariable(name, value)` - setează valoarea unei variabile
   - `getVariable(name)` - obține valoarea unei variabile
   - `evaluate(root)` - evaluează expresia cu variabilele curente
   - Tratați eroarea de variabilă nedefinită

4. **Funcționalități avansate (10 puncte)**
   - Simplificarea expresiilor (x + 0 = x, x * 1 = x, etc.)
   - Derivarea simbolică pentru expresii simple
   - Afișarea expresiei în format LaTeX

### Exemplu de Utilizare

```c
// Parsează expresia infix
ExprNode *expr = parseInfix("(x + 3) * (y - 2)");

// Setează variabilele
setVariable("x", 5);
setVariable("y", 7);

// Evaluează
double result = evaluate(expr);  // (5 + 3) * (7 - 2) = 40

// Afișează în diferite formate
printInfix(expr);    // ((x + 3) * (y - 2))
printLatex(expr);    // (x + 3) \cdot (y - 2)

// Schimbă variabilele și reevaluează
setVariable("x", 10);
result = evaluate(expr);  // (10 + 3) * (7 - 2) = 65
```

### Fișier: `homework2_calculator.c`

---

## 📊 Criterii de Evaluare

| Criteriu | Puncte |
|----------|--------|
| Corectitudine funcțională | 40 |
| Utilizare corectă a arborilor binari | 25 |
| Tratarea cazurilor limită | 15 |
| Calitatea codului (stil, comentarii) | 10 |
| Fără warning-uri la compilare | 10 |

### Penalizări

| Problemă | Penalizare |
|----------|------------|
| Warning-uri la compilare | -10p |
| Memory leaks (detectate cu Valgrind) | -20p |
| Crash pe input valid | -30p |
| Cod copiat / plagiat | -50p până la anulare |

---

## 📤 Modalitate de Predare

1. Creați un director `tema7_NumePrenume`
2. Includeți fișierele sursă și un `Makefile`
3. Adăugați un fișier `README.md` cu:
   - Instrucțiuni de compilare și rulare
   - Exemple de utilizare
   - Dificultăți întâmpinate și soluții
4. Arhivați ca `.zip` și încărcați pe platforma cursului

---

## 💡 Sfaturi

1. **Începeți cu structurile de date** - definiți clar ce câmpuri aveți nevoie
2. **Testați incremental** - verificați fiecare funcție separat înainte de integrare
3. **Folosiți Valgrind** - `valgrind --leak-check=full ./program`
4. **Tratați cazurile limită**:
   - Arbore vid (root == NULL)
   - Un singur nod
   - Căi invalide
   - Împărțire la zero
5. **Comentați codul** - explicați algoritmii complecși
6. **Folosiți funcții auxiliare** - nu puneți totul într-o singură funcție

---

## 📚 Resurse Utile

- [Shunting Yard Algorithm](https://en.wikipedia.org/wiki/Shunting-yard_algorithm)
- [Expression Trees](https://www.geeksforgeeks.org/expression-tree/)
- [Valgrind Quick Start](https://valgrind.org/docs/manual/quick-start.html)
- Exemplele din laborator (`example1.c`)

---

*Succes la implementare!*
