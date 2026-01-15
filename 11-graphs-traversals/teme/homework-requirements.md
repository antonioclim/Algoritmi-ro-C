# Temă Săptămâna 11: Tabele de Dispersie (Hash Tables)

## 📋 Informații Generale

- **Termen limită:** Sfârșitul săptămânii 12
- **Punctaj:** 100 puncte (10% din nota finală)
- **Limbaj:** C (standard C11)
- **Compilator:** GCC cu flag-urile `-Wall -Wextra -std=c11`

---

## 📝 Tema 1: Dicționar de Cuvinte cu Frecvență (50 puncte)

### Descriere

Implementați un dicționar care numără frecvența cuvintelor dintr-un text. Programul trebuie să citească un fișier text, să extragă toate cuvintele și să contorizeze de câte ori apare fiecare cuvânt.

### Cerințe

1. **Structura de date** (10p)
   - Implementați o hash table cu separate chaining
   - Cheia este cuvântul (convertit la lowercase)
   - Valoarea este frecvența (int)

2. **Funcția hash** (10p)
   - Implementați djb2 sau sdbm
   - Distribuție uniformă pentru cuvinte în limba română

3. **Preprocesare text** (10p)
   - Conversia la lowercase
   - Eliminarea semnelor de punctuație
   - Tratarea caracterelor românești (ă, â, î, ș, ț)

4. **Afișare rezultate** (10p)
   - Top N cele mai frecvente cuvinte (N citit de la stdin)
   - Format: `cuvânt: frecvență`
   - Sortare descrescătoare după frecvență

5. **Statistici** (10p)
   - Numărul total de cuvinte unice
   - Numărul total de cuvinte (cu repetiții)
   - Load factor-ul final al tabelei

### Exemplu Utilizare

```bash
$ ./homework1 text.txt
Introdu N (top cuvinte): 5

=== Top 5 cele mai frecvente cuvinte ===
1. de: 45
2. și: 38
3. în: 32
4. la: 28
5. cu: 25

=== Statistici ===
Cuvinte unice: 523
Total cuvinte: 2847
Load factor: 1.52
```

### Fișier: `homework1_dictionar.c`

---

## 📝 Tema 2: Cache LRU (Least Recently Used) (50 puncte)

### Descriere

Implementați un cache LRU folosind combinația hash table + doubly linked list. Cache-ul trebuie să permită accesul O(1) la elemente și să elimine automat cel mai vechi element când capacitatea este depășită.

### Cerințe

1. **Structura de date** (15p)
   - Hash table pentru acces O(1) la elemente
   - Doubly linked list pentru ordinea de utilizare
   - Fiecare nod conține: key, value, prev, next

2. **Operații cache** (20p)
   - `get(key)`: returnează valoarea și marchează ca recent folosit
   - `put(key, value)`: inserează/actualizează și marchează ca recent
   - `evict()`: elimină cel mai vechi element când cache-ul e plin

3. **Capacitate configurabilă** (5p)
   - Capacitatea se specifică la creare
   - Verificare la fiecare inserare

4. **Statistici hit/miss** (10p)
   - Contorizează cache hits și cache misses
   - Calculează hit rate: hits / (hits + misses) * 100

### Exemplu Utilizare

```bash
$ ./homework2
Capacitate cache: 3

> PUT A 1
Cache: [A:1]

> PUT B 2
Cache: [B:2] -> [A:1]

> PUT C 3
Cache: [C:3] -> [B:2] -> [A:1]

> GET A
Hit! Value: 1
Cache: [A:1] -> [C:3] -> [B:2]

> PUT D 4
Evicted: B
Cache: [D:4] -> [A:1] -> [C:3]

> STATS
Hits: 1, Misses: 0, Hit Rate: 100.00%
```

### Fișier: `homework2_lru_cache.c`

---

## 📊 Criterii de Evaluare

| Criteriu | Puncte |
|----------|--------|
| Corectitudine funcțională | 40 |
| Utilizare corectă a hash tables | 25 |
| Tratarea cazurilor limită | 15 |
| Calitatea codului (stil, comentarii) | 10 |
| Fără warning-uri la compilare | 10 |

### Penalizări

| Situație | Penalizare |
|----------|------------|
| Warning-uri la compilare | -10p |
| Memory leaks (Valgrind) | -20p |
| Crash pe input valid | -30p |
| Plagiat | -50p până la -100p |

---

## 📤 Instrucțiuni de Predare

1. **Nume fișiere:**
   - `homework1_dictionar.c`
   - `homework2_lru_cache.c`

2. **Format arhivă:**
   - `Nume_Prenume_Grupa_S11.zip`
   - Conține DOAR fișierele .c

3. **Compilare:**
   ```bash
   gcc -Wall -Wextra -std=c11 -o homework1 homework1_dictionar.c
   gcc -Wall -Wextra -std=c11 -o homework2 homework2_lru_cache.c
   ```

4. **Testare Valgrind:**
   ```bash
   valgrind --leak-check=full ./homework1 text.txt
   valgrind --leak-check=full ./homework2
   ```

---

## 💡 Sfaturi

1. **Începe cu structurile de date** - definește clar HashNode, HashTable și Entry
2. **Testează funcția hash** separat înainte de a o integra
3. **Folosește Valgrind** de la început pentru a detecta memory leaks
4. **Comentează codul** - explică algoritmul și deciziile de design
5. **Tratează cazurile limită:**
   - Fișier inexistent sau gol
   - Chei duplicate
   - Cache plin
   - Valori NULL
6. **Nu uita să eliberezi memoria** - fiecare `malloc` are nevoie de `free`

---

## 📚 Resurse Utile

- Capitolul 11 din CLRS (Introduction to Algorithms)
- [GeeksforGeeks - LRU Cache](https://www.geeksforgeeks.org/lru-cache-implementation/)
- Exemplele din laborator (`example1.c`)

---

*Ultima actualizare: Ianuarie 2026*
*Curs: Algoritmi și Tehnici de Programare (ATP)*
*Academia de Studii Economice București - CSIE*
