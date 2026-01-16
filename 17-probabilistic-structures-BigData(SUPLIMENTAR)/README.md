# Săptămâna 17: Structuri de Date Probabilistice pentru Big Data

## 🎯 Obiective de Învățare

La finalul acestui laborator, studenții vor fi capabili să:

1. **Recunoască** definițiile și proprietățile fundamentale ale structurilor probabilistice: Bloom Filter (membership testing cu false positives), HyperLogLog (estimare cardinalitate), Count-Min Sketch (estimare frecvențe), Skip List (căutare probabilistică O(log n))
2. **Înțeleagă** trade-off-ul esențial spațiu vs. acuratețe în procesarea Big Data: de ce acceptăm erori controlate pentru a procesa seturi de date care depășesc capacitatea memoriei disponibile
3. **Aplice** implementarea completă în C a: Bloom Filter cu multiple funcții hash, HyperLogLog pentru cardinalitate, Count-Min Sketch pentru frecvențe în stream-uri, Skip List pentru căutare și inserare
4. **Analizeze** calculul parametrilor optimi (număr de funcții hash, dimensiune bitmap, precizie) pentru atingerea unei rate de eroare țintă
5. **Evalueze** alegerea structurii potrivite pentru scenarii practice: membership testing vs. frequency counting vs. cardinality estimation
6. **Creeze** un sistem complet de deduplicare pentru stream de URL-uri cu statistici de cardinalitate și detecție heavy hitters

---

## 📜 Context Istoric

### Originile Algoritmilor Probabilistici

Algoritmii probabilistici au apărut ca răspuns la o problemă fundamentală în informatică: cum procesăm eficient date care depășesc capacitatea memoriei disponibile? În anii 1970, creșterea exponențială a volumului de date a făcut evidentă necesitatea unor abordări alternative la structurile deterministe clasice.

Ideea revoluționară a fost aceea de a sacrifica certitudinea absolută în favoarea eficienței spațiale. În loc să stocăm fiecare element explicit, folosim reprezentări compacte care pot da răspunsuri greșite cu o probabilitate mică, dar controlabilă. Această paradigmă a deschis calea către procesarea Big Data modernă.

### Figură Cheie: Burton Howard Bloom (n. 1949)

```
    ┌─────────────────────────────────────────────────────────────────┐
    │  Burton Howard Bloom                                            │
    │  Informatician american, pionier al structurilor probabilistice │
    │                                                                 │
    │  • Absolvent MIT (1970)                                         │
    │  • A introdus Bloom Filter în teza de doctorat                  │
    │  • Paper seminal: "Space/Time Trade-offs in Hash Coding         │
    │    with Allowable Errors" (Communications of the ACM, 1970)     │
    │  • Ideea: acceptă false positives pentru economie de spațiu     │
    │  • Influență: baze de date, rețele, securitate informatică      │
    └─────────────────────────────────────────────────────────────────┘
```

> *"Space/time trade-offs that arise from the use of hashing with allowable errors can have significant practical importance."*
> — Burton H. Bloom, Communications of the ACM, 1970

### Figură Cheie: Philippe Flajolet (1948-2011)

```
    ┌─────────────────────────────────────────────────────────────────┐
    │  Philippe Flajolet                                              │
    │  Matematician și informatician francez                          │
    │                                                                 │
    │  • Cercetător INRIA (Institut National de Recherche)            │
    │  • Pionier în analiza combinatorică a algoritmilor              │
    │  • Co-autor HyperLogLog (2007, cu Fusy, Gandouet, Meunier)      │
    │  • Paper: "HyperLogLog: the analysis of a near-optimal          │
    │    cardinality estimation algorithm"                            │
    │  • Contribuții: LogLog, PCSA, streaming algorithms              │
    └─────────────────────────────────────────────────────────────────┘
```

> *"The main result of this paper is a cardinality estimation algorithm that uses only m = 2^p counters, yet achieves a standard error of approximately 1.04/√m."*
> — Philippe Flajolet et al., AOFA 2007

### Alte Contribuții Importante

- **Graham Cormode & S. Muthukrishnan** (2005): Count-Min Sketch pentru estimarea frecvențelor în streaming
- **William Pugh** (1990): Skip List, o alternativă probabilistică la arborii echilibrați
- **Moses Charikar** (2002): Locality-Sensitive Hashing pentru similaritate aproximativă

---

## 📚 Fundamente Teoretice

### 1. Funcții Hash pentru Structuri Probabilistice

Funcțiile hash sunt fundamentul tuturor structurilor probabilistice. O funcție hash bună trebuie să distribuie uniform cheile în spațiul de output, minimizând coliziunile.

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    PROPRIETĂȚI FUNCȚII HASH                                  │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   Distribuție Uniformă                                                      │
│   ┌──────────────────────────────────────────────────────────────┐          │
│   │  Input: "abc", "abd", "abe", "abf"                           │          │
│   │                                                              │          │
│   │  Hash scăzut:     Hash înalt:                                │          │
│   │  ████████░░░░     ████░░░░████     (înălțimea = frecvența)   │          │
│   │  0123456789       0123456789                                 │          │
│   │  (clustering)     (uniform)                                  │          │
│   └──────────────────────────────────────────────────────────────┘          │
│                                                                             │
│   Tehnica Double Hashing pentru k funcții hash:                             │
│   ┌──────────────────────────────────────────────────────────────┐          │
│   │  hash_i(x) = hash1(x) + i × hash2(x)    pentru i = 0..k-1    │          │
│   │                                                              │          │
│   │  Avantaj: generăm k hash-uri din doar 2 hash-uri de bază     │          │
│   │  Paper: Kirsch & Mitzenmacher, "Less Hashing, Same Perf."    │          │
│   └──────────────────────────────────────────────────────────────┘          │
│                                                                             │
│   Implementări comune:                                                      │
│   • FNV-1a (Fowler-Noll-Vo): simplu, rapid, bun pentru stringuri            │
│   • MurmurHash3: excelent pentru hash tables, acceptă seed                  │
│   • xxHash: foarte rapid pentru date mari                                   │
│   • DJB2: clasic, simplu de implementat                                     │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

**Structură în C pentru Hash Utilities**:

```c
typedef struct {
    uint64_t (*hash1)(const void *data, size_t len);
    uint64_t (*hash2)(const void *data, size_t len);
    uint32_t num_hashes;
} HashFamily;

/* Funcția i-a din familia de hash-uri */
static inline uint64_t hash_i(const HashFamily *hf, 
                              const void *data, size_t len, 
                              uint32_t i) {
    return hf->hash1(data, len) + i * hf->hash2(data, len);
}
```

### 2. Bloom Filter

Bloom Filter-ul este o structură de date probabilistică pentru testarea apartenenței (membership testing). Răspunde la întrebarea: "Am văzut acest element înainte?"

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         BLOOM FILTER - MECANISMUL                            │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   INSERARE (element "cat"):                                                 │
│   ┌──────────────────────────────────────────────────────────────┐          │
│   │  "cat" ──► h1("cat")=3, h2("cat")=7, h3("cat")=12            │          │
│   │                                                              │          │
│   │  Bitmap ÎNAINTE: [0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0]           │          │
│   │  Poziții setate:      ↓         ↓           ↓                │          │
│   │  Bitmap DUPĂ:    [0 0 0 1 0 0 0 1 0 0 0 0 1 0 0 0]           │          │
│   │                        3         7          12               │          │
│   └──────────────────────────────────────────────────────────────┘          │
│                                                                             │
│   CĂUTARE (element "dog"):                                                  │
│   ┌──────────────────────────────────────────────────────────────┐          │
│   │  "dog" ──► h1("dog")=2, h2("dog")=7, h3("dog")=15            │          │
│   │                                                              │          │
│   │  Bitmap actual:  [0 0 0 1 0 0 0 1 0 0 0 0 1 0 0 0]           │          │
│   │  Verificăm:          ↓         ✓               ↓             │          │
│   │                      2=0       7=1            15=0            │          │
│   │                      ✗ FALSE                  ✗ FALSE         │          │
│   │                                                              │          │
│   │  Concluzie: "dog" SIGUR NU E în set (poziția 2 și 15 sunt 0) │          │
│   └──────────────────────────────────────────────────────────────┘          │
│                                                                             │
│   FALSE POSITIVE (element "rat"):                                           │
│   ┌──────────────────────────────────────────────────────────────┐          │
│   │  Să presupunem: h1("rat")=3, h2("rat")=7, h3("rat")=12       │          │
│   │  (Coincidență cu pozițiile setate de "cat"!)                 │          │
│   │                                                              │          │
│   │  Bitmap:  [0 0 0 1 0 0 0 1 0 0 0 0 1 0 0 0]                  │          │
│   │                 ✓         ✓          ✓                        │          │
│   │                                                              │          │
│   │  Concluzie: "rat" PROBABIL în set (dar greșit - false +)     │          │
│   └──────────────────────────────────────────────────────────────┘          │
│                                                                             │
│   PROPRIETĂȚI:                                                              │
│   • FALSE NEGATIVES: IMPOSIBILE (dacă spunem NU, e garantat NU)             │
│   • FALSE POSITIVES: Posibile, dar cu probabilitate controlabilă            │
│   • ȘTERGERE: NU este suportată (ar putea crea false negatives)             │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

**Formulele pentru Parametri Optimi**:

```
    Dați: n = număr așteptat de elemente
          p = rata de false positives dorită (ex: 0.01 = 1%)

    Calculăm:
    ┌────────────────────────────────────────────────────────────────┐
    │                                                                │
    │   m = - (n × ln(p)) / (ln(2))²                                 │
    │                                                                │
    │   unde m = numărul de biți în bitmap                           │
    │                                                                │
    │   k = (m / n) × ln(2) ≈ 0.693 × (m / n)                        │
    │                                                                │
    │   unde k = numărul optim de funcții hash                       │
    │                                                                │
    └────────────────────────────────────────────────────────────────┘

    Exemplu numeric:
    n = 1,000,000 elemente, p = 1% false positives
    m = -(1,000,000 × ln(0.01)) / (ln(2))²
    m ≈ 9,585,058 biți ≈ 1.2 MB
    k = (9,585,058 / 1,000,000) × 0.693 ≈ 7 funcții hash

    Compară: HashSet ar necesita ~50-100 MB pentru 1M stringuri!
```

**Structura în C**:

```c
typedef struct {
    uint8_t *bitmap;           /* Array de biți */
    size_t size_bits;          /* Dimensiunea în biți (m) */
    size_t num_hashes;         /* Numărul de funcții hash (k) */
    size_t elements_added;     /* Contor elemente adăugate */
    double target_fpr;         /* Rata de false positives țintă */
} BloomFilter;
```

### 3. Count-Min Sketch

Count-Min Sketch estimează frecvența elementelor într-un stream de date, folosind mult mai puțină memorie decât o numărare exactă.

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                     COUNT-MIN SKETCH - STRUCTURA                             │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   Matrice de contoare: depth × width                                        │
│   ┌──────────────────────────────────────────────────────────────┐          │
│   │                                                              │          │
│   │   Row 0 (h0): [ 0 ][ 2 ][ 0 ][ 5 ][ 1 ][ 0 ][ 3 ][ 0 ]       │          │
│   │   Row 1 (h1): [ 1 ][ 0 ][ 4 ][ 0 ][ 0 ][ 2 ][ 0 ][ 1 ]       │          │
│   │   Row 2 (h2): [ 0 ][ 3 ][ 0 ][ 1 ][ 0 ][ 0 ][ 2 ][ 0 ]       │          │
│   │   Row 3 (h3): [ 2 ][ 0 ][ 0 ][ 3 ][ 0 ][ 1 ][ 0 ][ 0 ]       │          │
│   │                                                              │          │
│   │   depth = 4 rânduri (d)                                      │          │
│   │   width = 8 coloane (w)                                      │          │
│   │   Memorie: 4 × 8 × sizeof(counter) = 32 contoare             │          │
│   │                                                              │          │
│   └──────────────────────────────────────────────────────────────┘          │
│                                                                             │
│   ADĂUGARE ("cat" apare de 3 ori):                                          │
│   ┌──────────────────────────────────────────────────────────────┐          │
│   │   h0("cat") mod 8 = 3  →  counters[0][3] += 3                │          │
│   │   h1("cat") mod 8 = 2  →  counters[1][2] += 3                │          │
│   │   h2("cat") mod 8 = 6  →  counters[2][6] += 3                │          │
│   │   h3("cat") mod 8 = 0  →  counters[3][0] += 3                │          │
│   └──────────────────────────────────────────────────────────────┘          │
│                                                                             │
│   ESTIMARE frecvență ("cat"):                                               │
│   ┌──────────────────────────────────────────────────────────────┐          │
│   │   count = MIN( counters[0][3],                               │          │
│   │                counters[1][2],                               │          │
│   │                counters[2][6],                               │          │
│   │                counters[3][0] )                              │          │
│   │                                                              │          │
│   │   De ce MIN? Pentru că coliziunile ADAUGĂ la contor,         │          │
│   │   deci valoarea minimă e cea mai apropiată de realitate.     │          │
│   │                                                              │          │
│   │   Garanție: estimate ≥ count_real (niciodată subestimăm)     │          │
│   │             estimate ≤ count_real + ε × N                    │          │
│   │             (supraestimăm cu maxim ε × N)                    │          │
│   └──────────────────────────────────────────────────────────────┘          │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

**Parametri și Garanții**:

```
    Parametri de input:
    ε (epsilon) = eroarea relativă maximă tolerată
    δ (delta)   = probabilitatea de a depăși eroarea ε

    Dimensiuni calculate:
    width (w)  = ⌈e/ε⌉         unde e ≈ 2.718
    depth (d)  = ⌈ln(1/δ)⌉

    Exemplu:
    ε = 0.01 (eroare 1%), δ = 0.01 (99% încredere)
    w = ⌈2.718/0.01⌉ = 272 coloane
    d = ⌈ln(100)⌉ = 5 rânduri
    Total: 272 × 5 = 1360 contoare (vs. milioane pentru exact)
```

**Structura în C**:

```c
typedef struct {
    int64_t **counters;    /* Matrice depth × width */
    size_t depth;          /* Numărul de rânduri (d) */
    size_t width;          /* Numărul de coloane (w) */
    double epsilon;        /* Eroarea relativă */
    double delta;          /* Probabilitatea de eroare */
    size_t total_count;    /* Suma totală a elementelor */
} CountMinSketch;
```

### 4. HyperLogLog

HyperLogLog estimează cardinalitatea (numărul de elemente distincte) folosind doar ~1.5KB de memorie pentru a număra miliarde de elemente unice.

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                      HYPERLOGLOG - INTUIȚIA                                  │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   OBSERVAȚIA CHEIE: Dacă hash-urile sunt uniform distribuite,               │
│   probabilitatea de a vedea un hash cu k zerouri la început                 │
│   scade exponențial: P(k zerouri) = 1/2^k                                   │
│                                                                             │
│   ┌──────────────────────────────────────────────────────────────┐          │
│   │   hash("elem1") = 0010110...  → poziția primului 1 = 3       │          │
│   │   hash("elem2") = 0001011...  → poziția primului 1 = 4       │          │
│   │   hash("elem3") = 0000001...  → poziția primului 1 = 7       │          │
│   │   hash("elem4") = 1010001...  → poziția primului 1 = 1       │          │
│   │                                                              │          │
│   │   max_position = 7                                           │          │
│   │   Estimare cardinalitate ≈ 2^7 = 128 elemente                │          │
│   └──────────────────────────────────────────────────────────────┘          │
│                                                                             │
│   PROBLEMA: Variație mare cu un singur registru                             │
│   SOLUȚIA:  Folosim M registre și facem media                               │
│                                                                             │
│   ALGORITMUL COMPLET:                                                       │
│   ┌──────────────────────────────────────────────────────────────┐          │
│   │   1. Calculează hash(element) pe 64 biți                     │          │
│   │                                                              │          │
│   │   2. Folosește primii p biți pentru a alege registrul        │          │
│   │      index = hash >> (64 - p)                                │          │
│   │      (avem M = 2^p registre)                                 │          │
│   │                                                              │          │
│   │   3. Numără leading zeros în restul biților + 1              │          │
│   │      w = hash << p   (shift left p biți)                     │          │
│   │      rank = count_leading_zeros(w) + 1                       │          │
│   │                                                              │          │
│   │   4. Actualizează registrul: reg[index] = max(reg[index], rank)│          │
│   │                                                              │          │
│   │   5. Estimare: E = α × M² / Σ(2^(-reg[j]))                   │          │
│   │      unde α este o constantă de corecție                     │          │
│   └──────────────────────────────────────────────────────────────┘          │
│                                                                             │
│   EXEMPLU cu p=4 (16 registre):                                             │
│   ┌──────────────────────────────────────────────────────────────┐          │
│   │   hash = 0x5A3B... = 0101 1010 0011 1011...                  │          │
│   │                      ════                                    │          │
│   │                      index = 5 (primii 4 biți)               │          │
│   │                                                              │          │
│   │   rest = 1010 0011 1011...                                   │          │
│   │   leading zeros = 0 → rank = 1                               │          │
│   │                                                              │          │
│   │   registers[5] = max(registers[5], 1)                        │          │
│   └──────────────────────────────────────────────────────────────┘          │
│                                                                             │
│   MEMORIE: Cu p=14, avem 2^14 = 16384 registre × 6 biți ≈ 12 KB             │
│   EROARE:  Eroare standard ≈ 1.04/√M = 1.04/128 ≈ 0.8%                      │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

**Structura în C**:

```c
typedef struct {
    uint8_t *registers;     /* Array de M registre */
    size_t num_registers;   /* M = 2^precision */
    uint8_t precision;      /* p, tipic între 4 și 16 */
    double alpha;           /* Constantă de corecție bias */
} HyperLogLog;

/*
 * Constantele alpha pentru diferite valori de M:
 * M = 16:   alpha = 0.673
 * M = 32:   alpha = 0.697
 * M = 64:   alpha = 0.709
 * M >= 128: alpha = 0.7213 / (1 + 1.079/M)
 */
```

### 5. Skip List

Skip List este o structură de date probabilistică care oferă operații de căutare, inserare și ștergere în O(log n) timp mediu, ca alternativă la arborii echilibrați.

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         SKIP LIST - VIZUALIZARE                              │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   Nivel 3: HEAD ───────────────────────────────────────── 25 ────────► NIL  │
│              │                                             │                │
│   Nivel 2: HEAD ─────────── 9 ─────────────── 19 ──────── 25 ────────► NIL  │
│              │               │                 │           │                │
│   Nivel 1: HEAD ──── 6 ──── 9 ──── 12 ──── 19 ──── 21 ── 25 ── 26 ──► NIL   │
│              │       │       │       │       │       │     │     │          │
│   Nivel 0: HEAD ─ 3 ─ 6 ─ 7 ─ 9 ─ 12 ─ 17 ─ 19 ─ 21 ─ 25 ─ 26 ─► NIL       │
│                                                                             │
│   Căutare element 19:                                                       │
│   ┌──────────────────────────────────────────────────────────────┐          │
│   │   1. Start la HEAD, nivel 3                                  │          │
│   │   2. 25 > 19, coboară la nivel 2                             │          │
│   │   3. HEAD → 9, 9 < 19, avansăm                               │          │
│   │   4. 9 → 19, 19 == 19, GĂSIT!                                │          │
│   │                                                              │          │
│   │   Pași: 4 (vs. 6 pentru căutare liniară)                     │          │
│   └──────────────────────────────────────────────────────────────┘          │
│                                                                             │
│   PROBABILITATEA NIVELURILOR:                                               │
│   ┌──────────────────────────────────────────────────────────────┐          │
│   │   La inserare, nivelul unui nod se decide probabilistic:     │          │
│   │                                                              │          │
│   │   level = 1                                                  │          │
│   │   while (random() < p && level < MAX_LEVEL):                 │          │
│   │       level++                                                │          │
│   │                                                              │          │
│   │   Cu p = 0.5:                                                │          │
│   │   P(nivel 1) = 0.5                                           │          │
│   │   P(nivel 2) = 0.25                                          │          │
│   │   P(nivel 3) = 0.125                                         │          │
│   │   ...                                                        │          │
│   │                                                              │          │
│   │   Rezultat: aproximativ jumătate din noduri la fiecare nivel │          │
│   │   → structură similară cu arbore binar echilibrat            │          │
│   └──────────────────────────────────────────────────────────────┘          │
│                                                                             │
│   COMPLEXITĂȚI:                                                             │
│   • Căutare: O(log n) așteptat                                              │
│   • Inserare: O(log n) așteptat                                             │
│   • Ștergere: O(log n) așteptat                                             │
│   • Spațiu: O(n) așteptat (≈ 2n pointeri total)                             │
│                                                                             │
│   AVANTAJE față de arbori AVL/Red-Black:                                    │
│   • Implementare mai simplă                                                 │
│   • Fără rotații complexe                                                   │
│   • Performanță cache-friendly pentru range queries                         │
│   • Ușor de paralelizat (lock-free implementations)                         │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

**Structura în C**:

```c
#define MAX_LEVEL 16

typedef struct SkipNode {
    int key;
    void *value;
    struct SkipNode **forward;  /* Array de pointeri la niveluri */
    int level;                  /* Nivelul maxim al acestui nod */
} SkipNode;

typedef struct {
    SkipNode *header;           /* Nodul sentinel de început */
    int max_level;              /* Nivelul maxim permis */
    int current_level;          /* Nivelul maxim curent în listă */
    size_t size;                /* Numărul de elemente */
    float probability;          /* p pentru determinarea nivelului */
} SkipList;
```

---

## 🏭 Aplicații Industriale

### 1. Google Chrome Safe Browsing (Bloom Filter)

Chrome folosește Bloom Filter-uri pentru a verifica rapid dacă un URL ar putea fi malițios, fără a trimite fiecare URL la server.

```c
/*
 * Pseudocod simplificat al verificării Safe Browsing
 */
typedef struct {
    BloomFilter *local_filter;  /* ~1MB pe disc */
    time_t last_update;
} SafeBrowsingCache;

bool check_url_safety(SafeBrowsingCache *cache, const char *url) {
    /* Pas 1: Verificare locală rapidă cu Bloom Filter */
    if (!bloom_check(cache->local_filter, url, strlen(url))) {
        /* Dacă NU e în Bloom Filter, sigur e safe */
        return true;  /* URL SIGUR */
    }
    
    /* Pas 2: Possible positive - verifică cu serverul */
    /* (Bloom poate da false positive, deci confirmăm) */
    return verify_with_google_server(url);
}
```

### 2. Redis PFCOUNT (HyperLogLog)

Redis implementează HyperLogLog nativ pentru numărarea vizitatorilor unici:

```
> PFADD visitors:2024-01-15 "user1" "user2" "user3"
(integer) 1
> PFADD visitors:2024-01-15 "user2" "user4" "user5"
(integer) 1
> PFCOUNT visitors:2024-01-15
(integer) 5
> MEMORY USAGE visitors:2024-01-15
(integer) 12304   // ~12KB pentru milioane de utilizatori!
```

### 3. Network Traffic Analysis (Count-Min Sketch)

Detectarea atacurilor DDoS prin identificarea IP-urilor cu trafic anormal:

```c
/*
 * Heavy Hitters Detection pentru trafic de rețea
 */
typedef struct {
    CountMinSketch *sketch;
    uint64_t threshold;
    char **suspected_ips;
    size_t num_suspected;
} DDoSDetector;

void process_packet(DDoSDetector *detector, const char *src_ip) {
    /* Incrementează contorul pentru acest IP */
    cms_add(detector->sketch, src_ip, strlen(src_ip), 1);
    
    /* Verifică dacă depășește threshold-ul */
    uint64_t estimated_count = cms_estimate(detector->sketch, 
                                            src_ip, strlen(src_ip));
    
    if (estimated_count > detector->threshold) {
        flag_suspicious_ip(detector, src_ip, estimated_count);
    }
}
```

### 4. Redis Sorted Sets (Skip List)

Redis folosește Skip List pentru implementarea seturilor sortate:

```
> ZADD leaderboard 100 "Alice" 85 "Bob" 95 "Carol"
(integer) 3
> ZRANGE leaderboard 0 -1 WITHSCORES
1) "Bob"
2) "85"
3) "Carol"
4) "95"
5) "Alice"
6) "100"
> ZRANK leaderboard "Carol"
(integer) 1
```

---

## 💻 Exerciții de Laborator

### Exercițiul 1: Sistem de Deduplicare URL-uri

**Obiectiv**: Implementarea unui sistem care procesează un stream de URL-uri și identifică duplicatele folosind Bloom Filter, raportând statistici de cardinalitate cu HyperLogLog.

**Cerințe**:
1. Implementați un Bloom Filter cu parametri configurabili (n așteptat, FPR țintă)
2. Calculați automat dimensiunea optimă a bitmap-ului și numărul de hash-uri
3. Procesați un fișier cu URL-uri linie cu linie
4. Raportați:
   - Numărul total de URL-uri procesate
   - Numărul de URL-uri duplicate detectate
   - Rata de false positives estimată vs. teoretică
   - Estimare cardinalitate cu HyperLogLog

**Fișiere**: `src/exercise1.c`, `data/urls_100k.txt`

**Exemplu output**:
```
=== URL Deduplication Report ===
Total URLs processed:     100,000
Unique URLs (estimated):   78,432
Duplicates detected:       21,568
Bloom Filter FPR target:     1.00%
Bloom Filter FPR actual:     0.97%
Memory used (Bloom):      117.2 KB
Memory used (HLL):          1.5 KB
```

### Exercițiul 2: Analizator de Frecvențe în Stream

**Obiectiv**: Implementarea unui sistem care identifică "heavy hitters" (elementele cele mai frecvente) într-un stream de date folosind Count-Min Sketch.

**Cerințe**:
1. Implementați Count-Min Sketch cu parametri ε și δ configurabili
2. Procesați un stream de cuvinte din fișier
3. Identificați Top-K cele mai frecvente cuvinte
4. Comparați cu numărarea exactă (pentru validare)
5. Raportați eroarea relativă pentru fiecare heavy hitter

**Fișiere**: `src/exercise2.c`, `data/words_stream.txt`

**Exemplu output**:
```
=== Heavy Hitters Analysis ===
Stream size:          1,000,000 words
CMS parameters:       ε=0.01, δ=0.01
CMS memory:           5.3 KB

Top-10 Heavy Hitters:
Rank  Word          Estimated    Actual    Error
----  ----          ---------    ------    -----
  1   the             45,123     45,089    0.08%
  2   and             32,456     32,401    0.17%
  3   to              28,901     28,876    0.09%
  ...
```

---

## 🔧 Compilare și Execuție

```bash
# Construirea tuturor executabilelor
make all

# Rularea exemplului demonstrativ complet
make run

# Rularea exercițiului 1 (URL Deduplication)
make exercise1
./exercise1 data/urls_100k.txt

# Rularea exercițiului 2 (Heavy Hitters)
make exercise2
./exercise2 data/words_stream.txt

# Testare automată
make test

# Verificare memory leaks cu Valgrind
make valgrind

# Benchmark de performanță
make benchmark

# Curățare fișiere generate
make clean

# Afișare ajutor
make help
```

---

## 🐳 Rulare în Docker

```bash
# Construiește imaginea
docker build -t atp-week17 .

# Rulează containerul interactiv
docker run -it --rm -v $(pwd):/workspace atp-week17

# În container:
make all && make run
```

---

## 📁 Structura Directorului

```
17-bigdata-probabilistic/
├── README.md                           # Acest fișier
├── Makefile                            # Automatizare compilare
├── Dockerfile                          # Container standalone
├── docker-compose.yml                  # Setup complet
│
├── slides/
│   ├── presentation-week17.html        # Prezentare principală (40+ slide-uri)
│   └── presentation-comparativ.html    # Comparație Pseudocod/C/Python
│
├── src/
│   ├── example1.c                      # Demo complet toate structurile
│   ├── exercise1.c                     # URL Deduplication (cu TODO)
│   └── exercise2.c                     # Heavy Hitters (cu TODO)
│
├── data/
│   ├── urls_100k.txt                   # 100K URL-uri pentru testare
│   ├── words_stream.txt                # Stream de cuvinte
│   ├── ip_addresses.txt                # Adrese IP pentru network analysis
│   └── access_log.csv                  # Log accese web
│
├── tests/
│   ├── test1_input.txt
│   ├── test1_expected.txt
│   ├── test2_input.txt
│   └── test2_expected.txt
│
├── python_comparison/
│   ├── bloom_filter.py                 # Implementare Python Bloom
│   ├── hyperloglog.py                  # Implementare Python HLL
│   └── countmin_sketch.py              # Implementare Python CMS
│
├── teme/
│   ├── homework-requirements.md        # 2 teme (50p fiecare)
│   └── homework-extended.md            # 5 provocări bonus
│
└── solution/
    ├── exercise1_sol.c                 # Soluție URL Deduplication
    ├── exercise2_sol.c                 # Soluție Heavy Hitters
    ├── homework1_sol.c                 # Soluție Network Traffic Analyzer
    └── homework2_sol.c                 # Soluție Skip List Database
```

---

## 📖 Bibliografie Recomandată

### Resurse Esențiale
- **Burton H. Bloom** - *"Space/Time Trade-offs in Hash Coding with Allowable Errors"*, Communications of the ACM, 1970
- **Philippe Flajolet et al.** - *"HyperLogLog: the analysis of a near-optimal cardinality estimation algorithm"*, AOFA 2007
- **Graham Cormode & S. Muthukrishnan** - *"An Improved Data Stream Summary: The Count-Min Sketch and its Applications"*, Journal of Algorithms, 2005
- **William Pugh** - *"Skip Lists: A Probabilistic Alternative to Balanced Trees"*, Communications of the ACM, 1990

### Resurse Avansate
- **Kirsch & Mitzenmacher** - *"Less Hashing, Same Performance"*, Building a Better Bloom Filter, 2006
- **Appleby** - MurmurHash3 Reference Implementation, GitHub
- **Stefan Heule et al.** - *"HyperLogLog in Practice"*, Google Research, 2013

### Resurse Online
- [Redis HyperLogLog Internals](https://redis.io/docs/data-types/hyperloglogs/)
- [Bloom Filter Calculator](https://hur.st/bloomfilter/)
- [Probabilistic Data Structures for Web Analytics](https://highlyscalable.wordpress.com/2012/05/01/probabilistic-structures-web-analytics-data-mining/)

---

## ✅ Lista de Auto-Evaluare

Înainte de a considera laboratorul finalizat, verifică că poți:

- [ ] Explica diferența dintre false positives și false negatives în Bloom Filter
- [ ] Calcula parametrii optimi (m, k) pentru un Bloom Filter cu n și FPR date
- [ ] Implementa funcții hash multiple folosind tehnica double hashing
- [ ] Explica de ce Count-Min Sketch folosește minimul peste rânduri
- [ ] Descrie intuiția din spatele algoritmului HyperLogLog
- [ ] Implementa o Skip List funcțională cu inserare și căutare
- [ ] Compara trade-off-urile între structurile probabilistice și cele exacte
- [ ] Alege structura potrivită pentru un scenariu dat (membership vs. frequency vs. cardinality)
- [ ] Compila codul fără warnings cu `-Wall -Wextra -std=c11`
- [ ] Verifica corectitudinea cu Valgrind (zero memory leaks)

---

## 💼 Pregătire pentru Interviuri

### Întrebări Frecvente

1. **Ce este un Bloom Filter și care sunt limitările lui?**
   
   *Răspuns așteptat*: Un Bloom Filter este o structură de date probabilistică pentru membership testing care folosește un bitmap și multiple funcții hash. Poate răspunde "probabil da" sau "sigur nu" la întrebarea dacă un element a fost adăugat. Limitările principale sunt: (1) nu suportă ștergeri (Counting Bloom Filter rezolvă asta), (2) poate produce false positives (dar niciodată false negatives), (3) nu poate enumera elementele stocate.

2. **Cum funcționează HyperLogLog și de ce e atât de eficient din punct de vedere al memoriei?**
   
   *Răspuns așteptat*: HyperLogLog exploatează observația că într-un set de hash-uri uniform distribuite, probabilitatea de a vedea un hash cu k zerouri la început este 1/2^k. Prin menținerea poziției maxime a primului bit 1 în multiple registre (buckete) și aplicarea mediei armonice, poate estima cardinalitatea cu ~0.8% eroare folosind doar ~12KB pentru miliarde de elemente.

3. **Când ai folosi Count-Min Sketch în loc de un hash map pentru numărare?**
   
   *Răspuns așteptat*: Count-Min Sketch este preferat când: (1) datele sunt prea multe pentru a încăpea în memorie, (2) nu avem nevoie de numărare exactă, ci de identificarea heavy hitters, (3) procesăm stream-uri și nu putem stoca toate elementele. Trade-off-ul este spațiu vs. acuratețe - CMS poate supraestima (niciodată subestima) frecvențele.

4. **Care sunt avantajele Skip List față de arborii AVL sau Red-Black?**
   
   *Răspuns așteptat*: Skip List oferă: (1) implementare semnificativ mai simplă (fără rotații), (2) performanță comparabilă O(log n) în medie, (3) range queries mai eficiente datorită structurii secvențiale la nivel 0, (4) mai ușor de paralelizat (există implementări lock-free). Dezavantaj: nu are garanții worst-case (probabilistic vs. determinist).

5. **Cum ai proiecta un sistem de deduplicare pentru 1 miliard de URL-uri pe zi?**

   *Răspuns așteptat*: Arhitectură în două niveluri: (1) Bloom Filter local pe fiecare server (~1GB pentru 1 miliard URL-uri la 1% FPR) pentru filtrare rapidă - majoritatea URL-urilor vor fi noi și vor trece rapid, (2) pentru potențialele duplicate (positive din Bloom), verificare într-o bază de date distribuită sau HyperLogLog pentru statistici agregate. Acest design minimizează accesele la storage scump.

---

## 🔗 Preview Săptămâna Următoare

**Săptămâna 18: Fundamente Machine Learning în C**

Vom implementa algoritmi fundamentali de machine learning de la zero în C: regresie liniară cu gradient descent, clustering K-Means cu inițializare K-Means++, clasificare K-NN, și perceptronul simplu. Vom înțelege matematica din spatele acestor algoritmi fără a ne baza pe biblioteci externe.

Concepte cheie: gradient descent, funcții de cost, convergență, hiperparametri, train/test split.

---

## 🔧 Hardware Real (Opțional)

Kit-uri Arduino 32-bit disponibile la Biblioteca Facultății pentru studenții care doresc să experimenteze cu aplicații embedded ale structurilor probabilistice:

- **ESP32 DevKit** - WiFi/BLE inclus, 520KB SRAM
- **Arduino Due** - ARM Cortex-M3, 96KB SRAM

### Aplicații Embedded

1. **Bloom Filter pe ESP32** pentru deduplicare mesaje MQTT
2. **HyperLogLog pe Arduino** pentru numărare dispozitive BLE detectate
3. **Count-Min Sketch** pentru analiza traficului de rețea în timp real

*Notă*: Toate exercițiile de laborator sunt proiectate să ruleze complet în Docker, hardware-ul fizic fiind opțional pentru extinderi avansate.

---

*Acest material face parte din cursul "Algoritmi și Tehnici de Programare" (ATP)*
*Academia de Studii Economice din București - CSIE*
*Actualizat: Ianuarie 2026*
