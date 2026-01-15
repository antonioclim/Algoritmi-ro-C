# Provocări Avansate - Săptămâna 11

## 🚀 Provocări Bonus (Opționale)

Fiecare provocare rezolvată corect: **+10 puncte bonus**

---

## ⭐ Provocarea 1: Bloom Filter (Dificultate: Medie)

### Descriere

Un Bloom filter este o structură de date probabilistică eficientă pentru a testa dacă un element aparține unei mulțimi. Poate returna false positives dar niciodată false negatives.

### Cerințe

1. Implementează un Bloom filter cu:
   - Array de biți (nu bytes)
   - k funcții hash diferite (k configurabil)
   - Operații: `add(element)`, `contains(element)`

2. Funcții hash:
   - Folosește formula: `h_i(x) = (h1(x) + i * h2(x)) % m`
   - Unde h1 = djb2, h2 = sdbm

3. Calculează și afișează:
   - Rata teoretică de false positives
   - Rata efectivă (testând cu date cunoscute)

### Exemplu

```c
BloomFilter *bf = bf_create(1000, 7);  // m=1000 biți, k=7 hash functions
bf_add(bf, "hello");
bf_add(bf, "world");

bf_contains(bf, "hello");  // true (definit)
bf_contains(bf, "world");  // true (definit)
bf_contains(bf, "test");   // false (probabil) sau true (false positive)
```

### Puncte Bonus: +10

---

## ⭐ Provocarea 2: Consistent Hashing (Dificultate: Medie-Ridicată)

### Descriere

Consistent hashing este utilizat în sisteme distribuite pentru a distribui date între servere, minimizând redistribuirea la adăugarea/eliminarea unui server.

### Cerințe

1. Implementează un hash ring cu:
   - Noduri (servere) poziționate pe ring
   - Virtual nodes pentru distribuție uniformă
   - Căutare: găsește serverul responsabil pentru o cheie

2. Operații:
   - `add_node(server_name)` - adaugă server
   - `remove_node(server_name)` - elimină server
   - `get_node(key)` - găsește serverul pentru cheie

3. Statistici:
   - Distribuția cheilor între servere
   - Numărul de chei redistribuite la adăugare/eliminare server

### Puncte Bonus: +10

---

## ⭐ Provocarea 3: Perfect Hashing (Dificultate: Ridicată)

### Descriere

Implementează o schemă de perfect hashing pentru un set static de chei cunoscute dinainte, garantând zero coliziuni și acces O(1) worst-case.

### Cerințe

1. Folosește schema FKS (Fredman-Komlós-Szemerédi):
   - Nivel primar: hash table cu chaining
   - Nivel secundar: pentru fiecare bucket cu coliziuni, o hash table perfectă

2. Construcție:
   - Primește array-ul de chei la creare
   - Generează funcții hash până obține zero coliziuni

3. Garantează:
   - Lookup O(1) worst-case
   - Spațiu O(n)

### Puncte Bonus: +10

---

## ⭐ Provocarea 4: Cuckoo Hashing (Dificultate: Medie)

### Descriere

Cuckoo hashing folosește două tabele și două funcții hash. La coliziune, elementul existent este "dat afară" (ca un pui de cuc) și reinsetat în cealaltă tabelă.

### Cerințe

1. Implementează cuckoo hashing cu:
   - Două tabele de aceeași dimensiune
   - Două funcții hash diferite
   - Reinserție recursivă (cu limită de iterații)

2. Operații:
   - `insert(key, value)` - cu relocare la coliziune
   - `search(key)` - verifică ambele poziții
   - `delete(key)` - verifică ambele poziții

3. Rehashing:
   - Când se detectează ciclu, redimensionează ambele tabele

### Exemplu

```
Table1:      Table2:
[0]: A       [0]: D
[1]: -       [1]: B
[2]: C       [2]: -

Insert E where h1(E)=0, h2(E)=1:
- E vrea slot T1[0], ocupat de A
- A este mutat la T2[h2(A)]
- Dacă și acolo e ocupat, continuă procesul
```

### Puncte Bonus: +10

---

## ⭐ Provocarea 5: Hash Table Thread-Safe (Dificultate: Ridicată)

### Descriere

Implementează o hash table sigură pentru utilizare din mai multe thread-uri, folosind mecanisme de sincronizare.

### Cerințe

1. Implementează cu:
   - pthread mutex pentru fiecare bucket (fine-grained locking)
   - SAU un singur mutex pentru întreaga tabelă (coarse-grained)

2. Operații thread-safe:
   - `insert`, `search`, `delete` cu locking adecvat
   - Reader-writer lock pentru performanță (opțional)

3. Testare:
   - Creează N thread-uri care fac operații simultane
   - Verifică corectitudinea și absența race conditions
   - Măsoară performanța vs versiunea single-threaded

### Puncte Bonus: +10

---

## 📊 Sistem de Puncte Bonus

| Provocări Completate | Total Bonus |
|---------------------|-------------|
| 1 | +10 puncte |
| 2 | +20 puncte |
| 3 | +30 puncte |
| 4 | +40 puncte |
| Toate 5 | +50 puncte + Badge "Hash Master" 🏆 |

---

## 📝 Criterii de Evaluare pentru Bonus

Pentru a primi punctele:

1. **Corectitudine** - codul trebuie să funcționeze corect
2. **Eficiență** - complexitatea trebuie să fie cea specificată
3. **Cod curat** - comentat și ușor de înțeles
4. **Testare** - include teste care demonstrează funcționalitatea
5. **Documentație** - README scurt cu explicația implementării

---

## 📤 Predare

- Fișiere: `bonus1_bloom.c`, `bonus2_consistent.c`, etc.
- Include un `README_BONUS.md` cu explicații
- Adaugă în aceeași arhivă cu temele principale

---

## 💡 Resurse pentru Provocări

### Bloom Filter
- [Bloom Filter - Wikipedia](https://en.wikipedia.org/wiki/Bloom_filter)
- [Probabilistic Data Structures](https://www.jasondavies.com/bloomfilter/)

### Consistent Hashing
- [Consistent Hashing - Wikipedia](https://en.wikipedia.org/wiki/Consistent_hashing)
- Paper original: Karger et al. (1997)

### Perfect Hashing
- CLRS Capitolul 11.5
- FKS Paper: Fredman, Komlós, Szemerédi (1984)

### Cuckoo Hashing
- [Cuckoo Hashing - Wikipedia](https://en.wikipedia.org/wiki/Cuckoo_hashing)
- Paper: Pagh & Rodler (2001)

### Thread Safety
- [POSIX Threads Tutorial](https://computing.llnl.gov/tutorials/pthreads/)
- "C Concurrency in Action" - Anthony Williams

---

*Aceste provocări sunt opționale și destinate studenților care doresc să aprofundeze subiectul.*

*Ultima actualizare: Ianuarie 2026*
*Curs: Algoritmi și Tehnici de Programare (ATP)*
