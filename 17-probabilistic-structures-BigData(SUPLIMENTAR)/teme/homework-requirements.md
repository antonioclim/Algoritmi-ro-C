# Teme Săptămâna 17: Structuri de Date Probabilistice pentru Big Data

## Prezentare Generală

Această săptămână explorați implementarea și aplicarea structurilor de date probabilistice în contexte reale de procesare a datelor la scară mare. Temele combină fundamentele teoretice cu aplicații practice din industrie.

---

## Tema 1: Sistem de Caching Inteligent cu Bloom Filter (40 puncte)

### Context Industrial
Sistemele de caching moderne utilizează Bloom Filters pentru a evita accesul inutil la storage-ul secundar. Când un element nu se află în cache, un lookup în baza de date este costisitor (latență, resurse). Un Bloom Filter poate indica rapid dacă un element *nu* există, economisind resurse.

### Cerințe

#### Partea A: Implementare Core (20 puncte)

Implementați un sistem de caching cu două niveluri:

```
┌─────────────────────────────────────────────────────────────┐
│                     Cache System                             │
├─────────────────────────────────────────────────────────────┤
│   Request → [Bloom Filter] → NOT FOUND → Skip DB lookup     │
│                ↓ MAYBE                                       │
│            [L1 Cache] → HIT → Return                        │
│                ↓ MISS                                        │
│            [Database] → Found? → Add to L1                  │
└─────────────────────────────────────────────────────────────┘
```

**Funcții de implementat:**

```c
typedef struct {
    BloomFilter *filter;
    HashTable *l1_cache;
    size_t cache_hits;
    size_t cache_misses;
    size_t bloom_negatives;     // Salvări din BF
    size_t false_positives;     // FP-uri detectate
} CacheSystem;

CacheSystem *cache_create(size_t expected_items, double fpr, 
                          size_t cache_capacity);
                          
// Returnează valoarea sau NULL; actualizează statisticile
char *cache_get(CacheSystem *cs, const char *key, 
                const char *(*db_lookup)(const char *));
                
void cache_put(CacheSystem *cs, const char *key, const char *value);

void cache_print_stats(const CacheSystem *cs);
void cache_free(CacheSystem *cs);
```

#### Partea B: Simulare și Analiză (10 puncte)

1. **Simulare access patterns:** Generați 100,000 de cereri cu distribuție Zipf (80% din cereri vizează 20% din chei)
2. **Măsurați și raportați:**
   - Hit rate pentru L1 cache
   - Economii din Bloom Filter (DB lookups evitate)
   - False positive rate observat vs. teoretic
   - Comparație memorie: BF vs hash table echivalent

#### Partea C: Optimizare (10 puncte)

Implementați **Counting Bloom Filter** pentru suport de ștergere:

```c
typedef struct {
    uint8_t *counters;     // Contoare de 4 biți
    size_t size;
    size_t num_hashes;
} CountingBloomFilter;

void cbf_add(CountingBloomFilter *cbf, const void *data, size_t len);
bool cbf_remove(CountingBloomFilter *cbf, const void *data, size_t len);
bool cbf_check(const CountingBloomFilter *cbf, const void *data, size_t len);
```

### Input/Output Exemplu

**Input:** `data/cache_requests.txt`
```
GET user:1001
PUT user:1002 {"name":"Ion","age":25}
GET user:1003
GET user:1001
DELETE user:1002
GET user:1002
```

**Output:**
```
╔═══════════════════════════════════════════════════════════════╗
║                    Cache System Report                         ║
╚═══════════════════════════════════════════════════════════════╝

  Performance Metrics:
    Total requests:          100,000
    Cache hits (L1):          45,231 (45.23%)
    Cache misses:             54,769 (54.77%)
    
  Bloom Filter Savings:
    DB lookups avoided:       12,456 (guaranteed non-existent)
    False positives:             127 (0.13%)
    Theoretical FPR:           1.00%
    
  Memory Usage:
    Bloom Filter:             117.2 KB
    L1 Cache:               2,048.0 KB
    Total:                  2,165.2 KB
    
  Comparison (without BF):
    Hash table equivalent:  4,096.0 KB
    Memory savings:            47.1%
```

### Criterii de Evaluare

| Criteriu | Puncte |
|----------|--------|
| Bloom Filter corect | 8 |
| Cache integration | 6 |
| Statistici corecte | 6 |
| Simulare Zipf | 5 |
| Counting BF | 8 |
| Raport complet | 5 |
| Cod curat | 2 |

---

## Tema 2: Monitor de Trafic de Rețea (60 puncte)

### Context Industrial
Analizoarele de trafic de rețea procesează milioane de pachete pe secundă. Este imposibil să memorezi fiecare pachet, dar putem estima:
- Numărul de IP-uri unice (HyperLogLog)
- Frecvența conexiunilor per IP (Count-Min Sketch)
- IP-uri suspecte văzute anterior (Bloom Filter)

### Cerințe

#### Partea A: Network Monitor Core (25 puncte)

Implementați un monitor de trafic complet:

```c
typedef struct {
    HyperLogLog *unique_ips;       // Estimare IP-uri unice
    CountMinSketch *ip_frequency;  // Frecvența conexiunilor
    BloomFilter *blacklist;        // IP-uri suspecte cunoscute
    BloomFilter *seen_connections; // Conexiuni văzute (src:dst:port)
} NetworkMonitor;

// Structură pentru un pachet de rețea
typedef struct {
    uint32_t src_ip;
    uint32_t dst_ip;
    uint16_t src_port;
    uint16_t dst_port;
    uint8_t protocol;     // TCP=6, UDP=17
    uint32_t payload_size;
    time_t timestamp;
} NetworkPacket;

typedef struct {
    char ip_str[16];
    uint64_t connection_count;
    bool is_blacklisted;
} SuspiciousIP;

NetworkMonitor *monitor_create(size_t expected_ips, double fpr);

// Procesează un pachet și actualizează toate structurile
void monitor_process_packet(NetworkMonitor *nm, const NetworkPacket *pkt);

// Găsește IP-urile cu cele mai multe conexiuni
void monitor_find_heavy_hitters(const NetworkMonitor *nm, 
                                SuspiciousIP *results, int top_n);

// Verifică dacă un IP este suspect (blacklist + frecvență mare)
bool monitor_is_suspicious(const NetworkMonitor *nm, uint32_t ip, 
                           uint64_t threshold);

void monitor_print_report(const NetworkMonitor *nm);
void monitor_free(NetworkMonitor *nm);
```

#### Partea B: Detecție DDoS (20 puncte)

Implementați detecție de atacuri DDoS folosind sliding window:

```c
typedef struct {
    CountMinSketch *windows[60];   // 60 secunde de ferestre
    int current_window;
    uint64_t window_start;
    uint64_t ddos_threshold;       // Conexiuni/secundă per IP
} DDoSDetector;

DDoSDetector *ddos_create(uint64_t threshold_per_second);

// Procesează și verifică pentru DDoS
// Returnează IP-ul atacator dacă detectat, 0 altfel
uint32_t ddos_process(DDoSDetector *dd, const NetworkPacket *pkt);

// Obține rata curentă pentru un IP (ultimele N secunde)
double ddos_get_rate(const DDoSDetector *dd, uint32_t ip, int seconds);

void ddos_free(DDoSDetector *dd);
```

**Algoritmul de detecție:**
1. Menține 60 de CMS-uri pentru ultimele 60 de secunde
2. La fiecare pachet, verifică rata agregată
3. Dacă un IP depășește pragul (ex: 10,000 conn/sec), alertează

#### Partea C: Raportare și Vizualizare (15 puncte)

1. **Dashboard în terminal:**
```
╔═══════════════════════════════════════════════════════════════╗
║            Network Traffic Monitor - Live Dashboard            ║
╠═══════════════════════════════════════════════════════════════╣
║  Interval: Last 60 seconds                                     ║
║  Packets processed:  1,234,567                                 ║
║  Unique IPs (est.):     45,231                                 ║
║  Bytes transferred: 12.5 GB                                    ║
╠═══════════════════════════════════════════════════════════════╣
║  TOP-5 CONNECTIONS (by frequency)                              ║
║  ┌────────────────┬──────────┬───────────┬──────────────────┐ ║
║  │ Source IP      │ Conn/sec │ Status    │ Action           │ ║
║  ├────────────────┼──────────┼───────────┼──────────────────┤ ║
║  │ 192.168.1.100  │    8,542 │ 🟡 Watch  │ Rate limiting    │ ║
║  │ 10.0.0.55      │    3,421 │ 🟢 Normal │ -                │ ║
║  │ 172.16.0.1     │   12,345 │ 🔴 DDoS!  │ BLOCKED          │ ║
║  │ 192.168.2.50   │    1,234 │ 🟢 Normal │ -                │ ║
║  │ 10.1.1.1       │      567 │ 🟢 Normal │ -                │ ║
║  └────────────────┴──────────┴───────────┴──────────────────┘ ║
╠═══════════════════════════════════════════════════════════════╣
║  BLACKLISTED IPs DETECTED: 23                                  ║
║  POTENTIAL ATTACKS BLOCKED: 5                                  ║
╠═══════════════════════════════════════════════════════════════╣
║  Memory Usage:                                                 ║
║    HyperLogLog:      16.4 KB                                   ║
║    Count-Min Sketch: 272.0 KB × 60 = 15.9 MB (sliding window) ║
║    Bloom Filters:    234.4 KB                                  ║
║    Total:             16.4 MB                                  ║
╚═══════════════════════════════════════════════════════════════╝
```

2. **Export CSV** pentru analiză ulterioară
3. **Simulare încărcare** cu diferite pattern-uri de atac

### Input/Output Exemplu

**Input:** `data/network_packets.csv`
```csv
timestamp,src_ip,dst_ip,src_port,dst_port,protocol,size
1705312800,192.168.1.100,10.0.0.1,45678,80,6,1024
1705312800,192.168.1.101,10.0.0.1,45679,443,6,2048
1705312801,192.168.1.100,10.0.0.1,45680,80,6,512
...
```

**Utilizare:**
```bash
# Procesare normală
./homework2 data/network_packets.csv

# Cu simulare DDoS
./homework2 --simulate-ddos --attack-ip=192.168.100.1 --rate=50000

# Export raport
./homework2 data/network_packets.csv --export=report.csv
```

### Criterii de Evaluare

| Criteriu | Puncte |
|----------|--------|
| NetworkMonitor complet | 15 |
| HyperLogLog integrat | 5 |
| CMS integrat | 5 |
| Detecție DDoS | 15 |
| Sliding window | 5 |
| Dashboard | 8 |
| Export/Simulare | 5 |
| Cod modular | 2 |

---

## Reguli Generale

### Format Submissions

```
homework1/
├── src/
│   └── homework1.c
├── include/
│   └── cache_system.h
├── data/
│   └── test_requests.txt
├── Makefile
└── README.md

homework2/
├── src/
│   └── homework2.c
├── include/
│   ├── network_monitor.h
│   └── ddos_detector.h
├── data/
│   └── sample_packets.csv
├── Makefile
└── README.md
```

### Compilare și Testare

```bash
# Ambele teme
make all
make test
make clean

# Cu debugging
make debug
valgrind ./homework1 data/test_requests.txt
```

### Deadline și Punctaj

- **Deadline:** 2 săptămâni de la publicare
- **Punctaj total:** 100 puncte
- **Bonus:** +10 puncte pentru implementare Skip List în Tema 2 (ordered heavy hitters)

### Resurse Recomandate

1. **Bloom Filter original paper:** Bloom (1970) "Space/Time Trade-offs in Hash Coding with Allowable Errors"
2. **HyperLogLog:** Flajolet et al. (2007) "HyperLogLog: the analysis of a near-optimal cardinality estimation algorithm"
3. **Count-Min Sketch:** Cormode & Muthukrishnan (2005) "An improved data stream summary: the count-min sketch and its applications"
4. **Redis implementation:** https://redis.io/docs/data-types/probabilistic/

### Note Importante

1. **Nu folosiți biblioteci externe** pentru structurile de date probabilistice - scopul este să le implementați de la zero
2. **Documentați trade-off-urile** între precizie și memorie în README
3. **Includeți benchmark-uri** comparative pentru diferite configurații
4. **Tratați erorile** corespunzător (fișiere inexistente, memorie insuficientă)

---

## Întrebări Frecvente

**Q: Pot folosi biblioteca standard de hash-uri?**
A: Nu, implementați funcțiile hash furnizate (FNV-1a, DJB2, double hashing).

**Q: Cât de precise trebuie să fie estimările?**
A: Pentru FPR 1%, eroarea observată ar trebui să fie în intervalul 0.5-1.5%.

**Q: Pot modifica structurile furnizate?**
A: Da, puteți extinde structurile, dar păstrați câmpurile de bază.

**Q: Ce fac dacă memoria este insuficientă?**
A: Returnați cod de eroare și mesaj descriptiv. Nu continuați cu structuri incomplete.

---

*Succes la implementare!*
