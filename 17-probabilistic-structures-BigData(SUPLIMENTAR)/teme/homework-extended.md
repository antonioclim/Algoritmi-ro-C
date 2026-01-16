# Provocări Extinse: Structuri de Date Probabilistice

## Prezentare Generală

Aceste provocări sunt destinate studenților avansați care doresc să aprofundeze subiectul structurilor de date probabilistice. Fiecare provocare poate fi realizată independent și oferă puncte bonus la nota finală.

---

## Provocarea 1: Quotient Filter - Alternativă la Bloom Filter (20 puncte bonus)

### Context Teoretic

**Quotient Filter** este o structură de date introdusă de Bender et al. (2012) care oferă avantaje față de Bloom Filter:
- Suport nativ pentru ștergere
- Cache-friendly (localitate de referință)
- Fuziuni eficiente între filtre
- Compresie naturală la încărcare mică

### Arhitectura

```
┌─────────────────────────────────────────────────────────────────┐
│                      Quotient Filter                             │
├─────────────────────────────────────────────────────────────────┤
│  hash(x) = fingerprint f                                        │
│                                                                  │
│  f = q || r   (quotient || remainder)                           │
│      │    │                                                      │
│      │    └── stored in slot                                    │
│      └─────── determines slot index                             │
│                                                                  │
│  Metadata bits per slot:                                         │
│    - is_occupied: slot is canonical home for some element       │
│    - is_continuation: slot holds shifted element                │
│    - is_shifted: element not in canonical position              │
├─────────────────────────────────────────────────────────────────┤
│  Slot 0   │ Slot 1   │ Slot 2   │ ... │ Slot 2^q - 1            │
│  [O|C|S|r]│ [O|C|S|r]│ [O|C|S|r]│     │ [O|C|S|r]               │
└─────────────────────────────────────────────────────────────────┘
```

### Cerințe de Implementare

```c
typedef struct {
    uint64_t *slots;          // Fiecare slot: 3 biți metadata + r biți remainder
    uint8_t quotient_bits;    // q biți pentru quotient
    uint8_t remainder_bits;   // r biți pentru remainder  
    size_t num_slots;         // 2^q sloturi
    size_t num_elements;
} QuotientFilter;

// Operații de bază
QuotientFilter *qf_create(uint8_t q, uint8_t r);
bool qf_insert(QuotientFilter *qf, uint64_t fingerprint);
bool qf_lookup(const QuotientFilter *qf, uint64_t fingerprint);
bool qf_delete(QuotientFilter *qf, uint64_t fingerprint);
void qf_free(QuotientFilter *qf);

// Operații avansate
QuotientFilter *qf_merge(const QuotientFilter *qf1, const QuotientFilter *qf2);
double qf_load_factor(const QuotientFilter *qf);
void qf_resize(QuotientFilter **qf);  // Double the size
```

### Algoritmi Cheie

**Insert:**
1. Calculează `q = fingerprint >> r_bits`, `r = fingerprint & ((1 << r_bits) - 1)`
2. Găsește poziția canonică `slot[q]`
3. Dacă slot-ul este gol, inserează direct
4. Altfel, găsește "run" și inserează în ordine sortată a remainder-elor
5. Actualizează biții de metadata

**Lookup:**
1. Calculează q și r
2. Verifică `is_occupied` la `slot[q]`
3. Dacă nu, elementul nu există
4. Altfel, caută în run-ul asociat

### Criterii de Evaluare

| Criteriu | Puncte |
|----------|--------|
| Insert corect cu linear probing | 5 |
| Lookup corect cu metadata | 5 |
| Delete cu compactare | 5 |
| Merge între filtre | 3 |
| Benchmark vs Bloom | 2 |

---

## Provocarea 2: Cuckoo Filter - Hash-uri Multiple cu Relocare (20 puncte bonus)

### Context Teoretic

**Cuckoo Filter** (Fan et al., 2014) combină avantajele Cuckoo Hashing cu filtrarea probabilistică:
- FPR mai mic decât Bloom Filter la aceeași memorie
- Suport pentru ștergere
- Lookup-uri mai rapide (O(1) amortizat)

### Arhitectura

```
┌─────────────────────────────────────────────────────────────────┐
│                       Cuckoo Filter                              │
├─────────────────────────────────────────────────────────────────┤
│  Fiecare bucket conține b fingerprints (tipic b=4)              │
│                                                                  │
│  Bucket 0        Bucket 1        Bucket 2        ...            │
│  ┌───┬───┬───┬───┐ ┌───┬───┬───┬───┐ ┌───┬───┬───┬───┐         │
│  │ f │ f │ f │ f │ │ f │ f │ f │ f │ │ f │ f │ f │ f │         │
│  └───┴───┴───┴───┘ └───┴───┴───┴───┘ └───┴───┴───┴───┘         │
│                                                                  │
│  Insert x:                                                       │
│    f = fingerprint(x)                                           │
│    i1 = hash(x) % num_buckets                                   │
│    i2 = (i1 ^ hash(f)) % num_buckets   // Partial-key cuckoo    │
│                                                                  │
│    Încearcă bucket[i1], apoi bucket[i2]                         │
│    Dacă ambele pline → kick existing fingerprint și relocează   │
└─────────────────────────────────────────────────────────────────┘
```

### Cerințe de Implementare

```c
#define BUCKET_SIZE 4
#define MAX_KICKS 500

typedef struct {
    uint8_t fingerprints[BUCKET_SIZE];
} CuckooBucket;

typedef struct {
    CuckooBucket *buckets;
    size_t num_buckets;
    size_t num_elements;
    uint8_t fingerprint_bits;
} CuckooFilter;

// Operații
CuckooFilter *cf_create(size_t capacity, uint8_t fp_bits);
bool cf_insert(CuckooFilter *cf, const void *data, size_t len);
bool cf_lookup(const CuckooFilter *cf, const void *data, size_t len);
bool cf_delete(CuckooFilter *cf, const void *data, size_t len);
double cf_load_factor(const CuckooFilter *cf);
void cf_free(CuckooFilter *cf);

// Helper
uint8_t cf_fingerprint(const void *data, size_t len, uint8_t bits);
size_t cf_alternate_index(size_t index, uint8_t fp, size_t num_buckets);
```

### Algoritmul de Relocare

```c
bool cf_insert(CuckooFilter *cf, const void *data, size_t len) {
    uint8_t f = cf_fingerprint(data, len, cf->fingerprint_bits);
    size_t i1 = hash(data, len) % cf->num_buckets;
    size_t i2 = cf_alternate_index(i1, f, cf->num_buckets);
    
    // Încearcă i1 sau i2
    if (bucket_has_space(cf, i1)) { add_to_bucket(cf, i1, f); return true; }
    if (bucket_has_space(cf, i2)) { add_to_bucket(cf, i2, f); return true; }
    
    // Relocare (kicks)
    size_t i = (rand() % 2) ? i1 : i2;
    for (int n = 0; n < MAX_KICKS; n++) {
        // Swap random fingerprint from bucket[i] with f
        int slot = rand() % BUCKET_SIZE;
        uint8_t tmp = cf->buckets[i].fingerprints[slot];
        cf->buckets[i].fingerprints[slot] = f;
        f = tmp;
        
        i = cf_alternate_index(i, f, cf->num_buckets);
        if (bucket_has_space(cf, i)) {
            add_to_bucket(cf, i, f);
            return true;
        }
    }
    
    return false;  // Table full, need to resize
}
```

### Criterii de Evaluare

| Criteriu | Puncte |
|----------|--------|
| Insert cu relocare | 6 |
| Lookup corect | 4 |
| Delete | 4 |
| FPR < Bloom echivalent | 3 |
| Benchmark performance | 3 |

---

## Provocarea 3: Spectral Bloom Filter pentru Frecvențe (15 puncte bonus)

### Context Teoretic

**Spectral Bloom Filter** (Cohen & Matias, 2003) extinde Bloom Filter pentru a estima frecvența elementelor, nu doar apartenența. Similar cu Count-Min Sketch dar cu o structură diferită.

### Variante de Implementat

1. **SBF-RM (Recurring Minimum):**
   - Contoare în loc de biți
   - Estimare = minimul contoarelor

2. **SBF-MG (Minimal Increase):**
   - Incrementează doar contoarele cu valoare minimă
   - Reduce eroarea de supraevaluare

### Cerințe de Implementare

```c
typedef struct {
    uint32_t *counters;
    size_t size;
    size_t num_hashes;
    uint64_t total_insertions;
} SpectralBloomFilter;

// SBF-RM: Recurring Minimum
SpectralBloomFilter *sbf_rm_create(size_t m, size_t k);
void sbf_rm_add(SpectralBloomFilter *sbf, const void *data, size_t len, uint32_t count);
uint32_t sbf_rm_estimate(const SpectralBloomFilter *sbf, const void *data, size_t len);

// SBF-MG: Minimal Increase
void sbf_mg_add(SpectralBloomFilter *sbf, const void *data, size_t len);
uint32_t sbf_mg_estimate(const SpectralBloomFilter *sbf, const void *data, size_t len);

void sbf_free(SpectralBloomFilter *sbf);
```

### Comparație cu CMS

| Aspect | Spectral BF | Count-Min Sketch |
|--------|-------------|------------------|
| Structură | 1D array | 2D matrix |
| Estimare | min(counters) | min(rows) |
| Memorie (aceeași precizie) | Similar | Similar |
| False positives | Da | Nu (doar overestimate) |

### Criterii de Evaluare

| Criteriu | Puncte |
|----------|--------|
| SBF-RM corect | 5 |
| SBF-MG corect | 5 |
| Comparație cu CMS | 3 |
| Benchmark eroare | 2 |

---

## Provocarea 4: MinHash pentru Similaritate Jaccard (15 puncte bonus)

### Context Teoretic

**MinHash** (Broder, 1997) estimează similaritatea Jaccard între mulțimi:

```
J(A, B) = |A ∩ B| / |A ∪ B|
```

Folosit pentru:
- Detecția plagiatului
- Recomandări (utilizatori similari)
- Deduplicare documente (Near-Duplicate Detection)

### Algoritm

```
Pentru k funcții hash h₁, h₂, ..., hₖ:
  sig(A) = [min(h₁(A)), min(h₂(A)), ..., min(hₖ(A))]
  
Estimare Jaccard:
  Ĵ(A, B) = (numărul de poziții unde sig(A)[i] == sig(B)[i]) / k
```

### Cerințe de Implementare

```c
typedef struct {
    uint64_t *signature;
    size_t num_hashes;
} MinHashSignature;

// Creare semnătură pentru o mulțime de strings
MinHashSignature *minhash_create_signature(const char **elements, 
                                           size_t num_elements,
                                           size_t num_hashes);

// Estimare similaritate Jaccard între două semnături
double minhash_estimate_jaccard(const MinHashSignature *sig1,
                                 const MinHashSignature *sig2);

// LSH (Locality-Sensitive Hashing) pentru găsire rapidă de similare
typedef struct {
    MinHashSignature **signatures;
    size_t num_signatures;
    size_t bands;         // Număr de benzi
    size_t rows_per_band; // Rânduri per bandă
} LSHIndex;

LSHIndex *lsh_create(size_t bands, size_t rows_per_band);
void lsh_add(LSHIndex *lsh, MinHashSignature *sig, size_t id);
size_t *lsh_find_similar(const LSHIndex *lsh, MinHashSignature *query, 
                         double threshold, size_t *count);
void lsh_free(LSHIndex *lsh);
```

### Aplicație: Detecție Plagiat

```c
// Citește două documente și estimează similaritatea
typedef struct {
    char **shingles;      // k-shingles (k caractere consecutive)
    size_t num_shingles;
} Document;

Document *document_load(const char *filename, size_t k);
double document_similarity(Document *doc1, Document *doc2, size_t num_hashes);
```

### Criterii de Evaluare

| Criteriu | Puncte |
|----------|--------|
| MinHash signature | 4 |
| Jaccard estimation | 3 |
| LSH index | 5 |
| Aplicație plagiat | 3 |

---

## Provocarea 5: Probabilistic Counting cu FM Sketch (15 puncte bonus)

### Context Teoretic

**FM Sketch** (Flajolet-Martin, 1985) este predecesorul HyperLogLog. Folosește trailing zeros în loc de leading zeros:

```
Pentru fiecare element x:
  h = hash(x)
  ρ(h) = poziția primului bit 1 de la dreapta (trailing zeros + 1)
  bitmap[ρ(h)] = 1

Estimare: n̂ = (2^R) / φ
  unde R = poziția celui mai din stânga bit 1 din bitmap
  φ ≈ 0.77351
```

### Variante de Implementat

1. **FM Original:** Un singur bitmap
2. **PCSA (Probabilistic Counting with Stochastic Averaging):** Multiple bitmap-uri
3. **LogLog:** Predecessor direct al HyperLogLog

### Cerințe de Implementare

```c
// FM Sketch simplu
typedef struct {
    uint32_t bitmap;
} FMSketch;

FMSketch *fm_create(void);
void fm_add(FMSketch *fm, const void *data, size_t len);
double fm_estimate(const FMSketch *fm);

// PCSA cu m bitmap-uri
typedef struct {
    uint32_t *bitmaps;
    size_t num_bitmaps;
} PCSASketch;

PCSASketch *pcsa_create(size_t m);
void pcsa_add(PCSASketch *pcsa, const void *data, size_t len);
double pcsa_estimate(const PCSASketch *pcsa);

// LogLog
typedef struct {
    uint8_t *buckets;
    size_t num_buckets;
    uint8_t bits;
} LogLogSketch;

LogLogSketch *loglog_create(uint8_t precision);
void loglog_add(LogLogSketch *ll, const void *data, size_t len);
double loglog_estimate(const LogLogSketch *ll);
```

### Comparație Istorică

| Algoritm | An | Eroare Std. | Memorie pentru eroare ε |
|----------|-----|-------------|-------------------------|
| FM | 1985 | ~0.78/√m | O(ε⁻²) |
| PCSA | 1985 | ~0.78/√m | O(ε⁻²) |
| LogLog | 2003 | ~1.3/√m | O(ε⁻²) |
| SuperLogLog | 2004 | ~1.05/√m | O(ε⁻²) |
| HyperLogLog | 2007 | ~1.04/√m | O(ε⁻² log log n) |

### Criterii de Evaluare

| Criteriu | Puncte |
|----------|--------|
| FM original | 4 |
| PCSA | 4 |
| LogLog | 4 |
| Comparație vs HLL | 3 |

---

## Reguli Generale pentru Provocări

### Submission Format

```
challenge_N/
├── src/
│   └── challenge_N.c
├── include/
│   └── *.h
├── tests/
│   ├── test_challenge_N.c
│   └── data/
├── benchmarks/
│   └── benchmark.c
├── Makefile
└── README.md (cu analiză teoretică)
```

### Cerințe Documentație

Pentru fiecare provocare, README-ul trebuie să conțină:

1. **Explicație teoretică** a structurii de date
2. **Analiză complexitate** (timp și spațiu)
3. **Derivarea formulelor** pentru precizie/eroare
4. **Comparație** cu alternative (Bloom, CMS, HLL)
5. **Benchmark-uri** cu grafice

### Bonus Suplimentar

- **+5 puncte** pentru implementare paralelă (OpenMP/pthreads)
- **+3 puncte** pentru vizualizare grafică a structurii
- **+3 puncte** pentru integrare cu Redis (modul C)

---

## Resurse Academice

1. **Quotient Filter:** Bender et al. "Don't Thrash: How to Cache Your Hash on Flash" (2012)
2. **Cuckoo Filter:** Fan et al. "Cuckoo Filter: Practically Better Than Bloom" (2014)
3. **Spectral Bloom:** Cohen & Matias "Spectral Bloom Filters" (2003)
4. **MinHash:** Broder "On the resemblance and containment of documents" (1997)
5. **FM Sketch:** Flajolet & Martin "Probabilistic Counting Algorithms" (1985)

---

*Aceste provocări sunt opționale dar recomandate pentru cei care vor să devină experți în structuri de date probabilistice!*
