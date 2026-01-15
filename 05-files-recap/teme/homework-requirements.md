# Teme Săptămâna 5: Stive (Stack)

## 📋 Informații Generale

- **Termen limită:** Sfârșitul săptămânii 6
- **Punctaj:** 100 puncte (10% din nota finală)
- **Limbaj:** C (standard C11)
- **Compilator:** GCC cu flagurile `-Wall -Wextra -std=c11`

---

## 📝 Tema 1: Calculator de Expresii Matematice (50 puncte)

### Descriere

Implementează un calculator complet care poate evalua expresii matematice complexe folosind algoritmul Shunting Yard și evaluarea postfixă.

### Cerințe Funcționale

1. **Parsare expresii (10p)**
   - Suportă numere întregi multi-cifră (ex: 123, 45)
   - Suportă numere negative (ex: -5, -123)
   - Ignoră spațiile din expresie

2. **Operatori aritmetici (15p)**
   - Adunare `+` (precedență 1, asociativ stânga)
   - Scădere `-` (precedență 1, asociativ stânga)
   - Înmulțire `*` (precedență 2, asociativ stânga)
   - Împărțire `/` (precedență 2, asociativ stânga)
   - Modulo `%` (precedență 2, asociativ stânga)
   - Putere `^` (precedență 3, asociativ dreapta)

3. **Paranteze și precedență (10p)**
   - Suportă paranteze imbricate la orice nivel
   - Respectă corect precedența operatorilor
   - Respectă asociativitatea (stânga/dreapta)

4. **Gestionare erori (10p)**
   - Detectează și raportează expresii invalide
   - Gestionează împărțirea cu zero
   - Verifică paranteze neechilibrate

5. **Interfață utilizator (5p)**
   - Mod interactiv (citește expresii de la stdin)
   - Mod batch (citește din fișier)
   - Afișare clară a rezultatelor și erorilor

### Exemple de Utilizare

```bash
$ ./calculator
Calculator expresii matematice
> 3 + 4 * 5
Rezultat: 23

> (3 + 4) * 5
Rezultat: 35

> 100 / 0
Eroare: Împărțire cu zero!

> 2 ^ 3 ^ 2
Rezultat: 512

> (1 + 2
Eroare: Paranteze neechilibrate!

> quit
```

### Fișier: `homework1_calculator.c`

---

## 📝 Tema 2: Browser History Navigator (50 puncte)

### Descriere

Implementează un sistem de navigare similar cu istoricul unui browser web, cu funcționalități complete de back, forward și gestiune a paginilor.

### Cerințe Funcționale

1. **Structuri de date (10p)**
   - Implementează o stivă pentru istoricul "back"
   - Implementează o stivă pentru istoricul "forward"
   - Structură pentru reprezentarea unei pagini (URL, titlu, timestamp)

2. **Navigare de bază (15p)**
   - `visit(url)` - vizitează o pagină nouă
   - `back()` - mergi la pagina anterioară
   - `forward()` - mergi la pagina următoare
   - `current()` - afișează pagina curentă

3. **Funcționalități avansate (15p)**
   - `back(n)` - mergi n pagini înapoi
   - `forward(n)` - mergi n pagini înainte
   - `history()` - afișează întregul istoric
   - `clear()` - șterge istoricul

4. **Gestionare corectă a stării (10p)**
   - Când vizitezi o pagină nouă, șterge istoricul forward
   - Limitează dimensiunea istoricului (max 100 pagini)
   - Gestionează cazurile limită (back pe prima pagină, etc.)

### Exemplu de Utilizare

```bash
$ ./browser
Browser History Simulator

> visit google.com
Navigat la: google.com

> visit github.com
Navigat la: github.com

> visit stackoverflow.com
Navigat la: stackoverflow.com

> back
Înapoi la: github.com

> back
Înapoi la: google.com

> forward
Înainte la: github.com

> visit youtube.com
Navigat la: youtube.com
(Istoricul forward a fost șters)

> history
1. google.com
2. github.com
3. youtube.com [curent]

> back 2
Înapoi la: google.com

> quit
```

### Fișier: `homework2_browser.c`

---

## 📊 Criterii de Evaluare

| Criteriu | Puncte |
|----------|--------|
| Corectitudinea funcțională | 40 |
| Utilizarea corectă a stivelor | 25 |
| Gestionarea cazurilor limită | 15 |
| Calitatea codului (claritate, comentarii) | 10 |
| Compilare fără warning-uri | 10 |

### Penalizări

| Penalizare | Puncte |
|------------|--------|
| Warning-uri la compilare | -10 |
| Memory leaks (detectate cu Valgrind) | -20 |
| Crash pe input valid | -30 |
| Plagiat | -50 (minim) |

---

## 📤 Instrucțiuni de Trimitere

1. **Nume fișiere:** `homework1_calculator.c` și `homework2_browser.c`
2. **Compilare obligatorie:** `gcc -Wall -Wextra -std=c11 -o program program.c`
3. **Testare memorie:** `valgrind --leak-check=full ./program`
4. **Format:** Trimite doar fișierele `.c`, nu executabilele
5. **Platformă:** Încarcă pe platforma de cursuri până la termen

---

## 💡 Sfaturi

1. **Începe cu structura de date** - definește clar stivele și funcțiile lor
2. **Testează incremental** - verifică fiecare funcționalitate separat
3. **Folosește constante** - definește `MAX_SIZE`, `MAX_URL_LEN`, etc.
4. **Comentează codul** - explică logica algoritmilor
5. **Gestionează memoria** - dacă folosești `malloc()`, nu uita de `free()`
6. **Verifică cu Valgrind** - zero memory leaks este obligatoriu
7. **Citește specificațiile** - respectă exact cerințele

---

## ❓ Întrebări Frecvente

**Q: Pot folosi biblioteci externe?**
A: Nu. Folosește doar biblioteca standard C.

**Q: Cum gestionez numerele foarte mari?**
A: Folosește `long long` dacă este necesar, dar `int` este suficient pentru teste.

**Q: Ce se întâmplă dacă utilizatorul introduce caractere invalide?**
A: Afișează un mesaj de eroare și continuă (nu termina programul).

**Q: Pot modifica structura fișierelor?**
A: Da, atâta timp cât respecti interfața cerută și numele fișierelor.

---

*Succes la implementare! 🚀*
