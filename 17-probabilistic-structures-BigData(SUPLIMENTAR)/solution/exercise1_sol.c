/**
 * =============================================================================
 * SOLUȚIE EXERCIȚIUL 1: Sistem de Deduplicare URL-uri
 * =============================================================================
 *
 * Această soluție demonstrează implementarea completă a unui sistem de 
 * deduplicare URL-uri folosind Bloom Filter pentru detecția duplicatelor
 * și HyperLogLog pentru estimarea cardinalității.
 *
 * CONCEPTE CHEIE IMPLEMENTATE:
 *   - Bloom Filter cu parametri optimi calculați automat
 *   - Double hashing pentru familie de funcții hash
 *   - HyperLogLog cu registre pe 8 biți și corecție bias
 *   - Procesare stream cu memorie constantă
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -O2 -o exercise1_sol exercise1_sol.c -lm
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <time.h>

/* =============================================================================
 * DEFINIREA TIPURILOR ȘI CONSTANTELOR
 * =============================================================================
 */

#define MAX_URL_LENGTH 2048
#define DEFAULT_EXPECTED_URLS 100000
#define DEFAULT_FPR 0.01
#define HLL_PRECISION 14

/* =============================================================================
 * FUNCȚII HASH
 * =============================================================================
 */

/**
 * FNV-1a Hash - funcție hash non-criptografică de înaltă calitate
 * 
 * Proprietăți:
 *   - Distribuție uniformă excelentă
 *   - Performanță ridicată (simple XOR și multiplicare)
 *   - Avalanche effect bun (schimbări mici în input → schimbări mari în output)
 */
uint64_t hash_fnv1a(const void *data, size_t len) {
    const uint8_t *bytes = (const uint8_t *)data;
    uint64_t hash = 14695981039346656037ULL;  /* FNV offset basis */
    const uint64_t prime = 1099511628211ULL;  /* FNV prime */
    
    for (size_t i = 0; i < len; i++) {
        hash ^= bytes[i];
        hash *= prime;
    }
    
    return hash;
}

/**
 * DJB2 Hash - funcție hash simplă dar eficientă
 * 
 * Formula: hash(i) = hash(i-1) * 33 + byte[i]
 * Constanta 33 (= 32 + 1) permite optimizare cu shift: (hash << 5) + hash
 */
uint64_t hash_djb2(const void *data, size_t len) {
    const uint8_t *bytes = (const uint8_t *)data;
    uint64_t hash = 5381;  /* Valoare inițială magic number */
    
    for (size_t i = 0; i < len; i++) {
        hash = ((hash << 5) + hash) + bytes[i];
    }
    
    return hash;
}

/**
 * Familia de hash-uri folosind tehnica double hashing
 * 
 * Formula: h_i(x) = h1(x) + i * h2(x)
 * 
 * Această tehnică generează k hash-uri independente din doar 2 evaluări,
 * păstrând proprietățile de independență necesare pentru Bloom Filter.
 * 
 * Referință: Kirsch & Mitzenmacher, "Less Hashing, Same Performance"
 */
uint64_t hash_family(const void *data, size_t len, uint32_t i) {
    uint64_t h1 = hash_fnv1a(data, len);
    uint64_t h2 = hash_djb2(data, len);
    return h1 + i * h2;
}

/* =============================================================================
 * STRUCTURA BLOOM FILTER - SOLUȚIE COMPLETĂ
 * =============================================================================
 */

typedef struct {
    uint8_t *bitmap;           /* Array de biți (packed) */
    size_t size_bits;          /* Dimensiunea în biți (m) */
    size_t size_bytes;         /* Dimensiunea în bytes */
    size_t num_hashes;         /* Numărul de funcții hash (k) */
    size_t elements_added;     /* Contor elemente adăugate */
    double target_fpr;         /* FPR țintă */
} BloomFilter;

/**
 * SOLUȚIE TODO 2: Calculul parametrilor optimi pentru Bloom Filter
 *
 * Derivarea formulelor:
 *   
 * Probabilitatea ca un bit să fie 0 după n inserții cu k hash-uri:
 *   P(bit=0) = (1 - 1/m)^(kn) ≈ e^(-kn/m)
 *
 * Probabilitatea de false positive (toate k bituri setate pentru element nou):
 *   FPR = (1 - e^(-kn/m))^k
 *
 * Pentru a minimiza FPR la m și n fixe, derivăm și obținem:
 *   k_opt = (m/n) * ln(2)
 *
 * Substituind în formula FPR și rezolvând pentru m:
 *   m = -n * ln(p) / (ln(2))²
 *
 * unde p este FPR dorit.
 */
void bloom_calculate_params(size_t n, double fpr, size_t *out_m, size_t *out_k) {
    /* Constante precomputate pentru eficiență */
    const double ln2 = 0.693147180559945;
    const double ln2_sq = 0.4804530139182014;  /* ln(2)² */
    
    /* Calculul numărului de biți (m)
     * m = -n * ln(p) / (ln(2))²
     * 
     * Folosim -log(fpr) pentru că fpr < 1, deci log(fpr) < 0
     */
    double m = -((double)n * log(fpr)) / ln2_sq;
    
    /* Calculul numărului de funcții hash (k)
     * k = (m/n) * ln(2)
     */
    double k = (m / (double)n) * ln2;
    
    /* Rotunjim în sus pentru a garanta parametrii necesari */
    *out_m = (size_t)ceil(m);
    *out_k = (size_t)ceil(k);
    
    /* Asigurăm minimum 1 funcție hash */
    if (*out_k < 1) *out_k = 1;
    
    /* 
     * Notă: Pentru n=100000 și fpr=0.01:
     * m ≈ 958,506 biți ≈ 117 KB
     * k ≈ 7 funcții hash
     */
}

/**
 * SOLUȚIE TODO 3: Crearea Bloom Filter
 *
 * Complexitate spațială: O(m) = O(-n * ln(p) / ln²(2))
 * Pentru p = 0.01 (1% FPR), m ≈ 9.58n biți
 */
BloomFilter *bloom_create(size_t expected_n, double target_fpr) {
    /* Alocarea structurii principale */
    BloomFilter *bf = (BloomFilter *)malloc(sizeof(BloomFilter));
    if (!bf) return NULL;
    
    /* Calculul parametrilor optimi */
    bloom_calculate_params(expected_n, target_fpr, &bf->size_bits, &bf->num_hashes);
    
    /* Calculul dimensiunii în bytes (rotunjire în sus) */
    bf->size_bytes = (bf->size_bits + 7) / 8;
    
    /* Alocarea bitmap-ului inițializat cu zerouri
     * calloc garantează că toți biții sunt 0 inițial
     */
    bf->bitmap = (uint8_t *)calloc(bf->size_bytes, 1);
    if (!bf->bitmap) {
        free(bf);
        return NULL;
    }
    
    /* Inițializarea metadatelor */
    bf->elements_added = 0;
    bf->target_fpr = target_fpr;
    
    return bf;
}

/**
 * SOLUȚIE TODO 4: Adăugarea unui element în Bloom Filter
 *
 * Complexitate temporală: O(k) unde k este numărul de funcții hash
 * 
 * Operații pe biți:
 *   - byte_index = bit_position / 8
 *   - bit_offset = bit_position % 8  (poziția în cadrul byte-ului)
 *   - Setare bit: bitmap[byte] |= (1 << offset)
 */
void bloom_add(BloomFilter *bf, const void *data, size_t len) {
    /* Aplicăm fiecare funcție hash și setăm bitul corespunzător */
    for (size_t i = 0; i < bf->num_hashes; i++) {
        /* Calculăm hash-ul cu funcția i din familie */
        uint64_t hash = hash_family(data, len, (uint32_t)i);
        
        /* Calculăm poziția bitului (modulo dimensiunea în biți) */
        size_t bit_index = hash % bf->size_bits;
        
        /* Decompoziția în byte și offset
         * Exemplu: bit_index = 42
         *   byte_index = 42 / 8 = 5
         *   bit_offset = 42 % 8 = 2
         *   Setăm bitul 2 din byte-ul 5
         */
        size_t byte_index = bit_index / 8;
        size_t bit_offset = bit_index % 8;
        
        /* Setăm bitul folosind operația OR pe bit */
        bf->bitmap[byte_index] |= (1 << bit_offset);
    }
    
    /* Incrementăm contorul de elemente */
    bf->elements_added++;
}

/**
 * SOLUȚIE TODO 5: Verificarea apartenenței (query)
 *
 * Complexitate temporală: O(k)
 * 
 * Semantica rezultatului:
 *   - false: Elementul SIGUR NU este în set (no false negatives)
 *   - true: Elementul PROBABIL este în set (poate fi false positive)
 *
 * Proprietatea crucială: Bloom Filter nu are false negatives!
 */
bool bloom_check(const BloomFilter *bf, const void *data, size_t len) {
    /* Verificăm fiecare bit corespunzător celor k hash-uri */
    for (size_t i = 0; i < bf->num_hashes; i++) {
        /* Calculăm hash-ul cu funcția i */
        uint64_t hash = hash_family(data, len, (uint32_t)i);
        
        /* Calculăm poziția bitului */
        size_t bit_index = hash % bf->size_bits;
        
        /* Decompoziția în byte și offset */
        size_t byte_index = bit_index / 8;
        size_t bit_offset = bit_index % 8;
        
        /* Verificăm dacă bitul este setat
         * Folosim AND pe bit pentru a extrage valoarea bitului
         */
        bool is_set = (bf->bitmap[byte_index] & (1 << bit_offset)) != 0;
        
        /* Dacă oricare bit NU este setat, elementul SIGUR nu există */
        if (!is_set) {
            return false;
        }
    }
    
    /* Toate biturile sunt setate - elementul PROBABIL există */
    return true;
}

/**
 * Eliberare memorie
 */
void bloom_free(BloomFilter *bf) {
    if (bf) {
        free(bf->bitmap);
        free(bf);
    }
}

/**
 * Estimarea FPR actual bazat pe numărul de elemente adăugate
 * 
 * Formula: FPR = (1 - e^(-k*n/m))^k
 */
double bloom_estimate_fpr(const BloomFilter *bf) {
    double exponent = -(double)(bf->num_hashes * bf->elements_added) / 
                       (double)bf->size_bits;
    double prob_bit_set = 1.0 - exp(exponent);
    return pow(prob_bit_set, bf->num_hashes);
}

/* =============================================================================
 * STRUCTURA HYPERLOGLOG - SOLUȚIE COMPLETĂ
 * =============================================================================
 */

typedef struct {
    uint8_t *registers;        /* M registre de 8 biți fiecare */
    size_t num_registers;      /* M = 2^p */
    uint8_t precision;         /* p biți pentru indexare */
    double alpha;              /* Factor de corecție pentru bias */
} HyperLogLog;

/**
 * SOLUȚIE TODO 6: Crearea HyperLogLog
 *
 * Precizia determină:
 *   - M = 2^p registre
 *   - Eroare standard ≈ 1.04 / √M
 *   - Memorie: M bytes (1 byte per registru)
 *
 * Pentru p=14: M=16384 registre, eroare ≈ 0.81%, memorie = 16 KB
 */
HyperLogLog *hll_create(uint8_t precision) {
    HyperLogLog *hll = (HyperLogLog *)malloc(sizeof(HyperLogLog));
    if (!hll) return NULL;
    
    hll->precision = precision;
    hll->num_registers = 1ULL << precision;  /* M = 2^p */
    
    /* Calculul factorului alpha pentru corecția bias
     * 
     * Alpha corectează bias-ul sistematic în estimarea cardinalității.
     * Valorile sunt derivate empiric și depind de M:
     *   M = 16:  alpha = 0.673
     *   M = 32:  alpha = 0.697
     *   M = 64:  alpha = 0.709
     *   M ≥ 128: alpha = 0.7213 / (1 + 1.079/M)
     */
    if (hll->num_registers == 16) {
        hll->alpha = 0.673;
    } else if (hll->num_registers == 32) {
        hll->alpha = 0.697;
    } else if (hll->num_registers == 64) {
        hll->alpha = 0.709;
    } else {
        /* Formula generală pentru M ≥ 128 */
        hll->alpha = 0.7213 / (1.0 + 1.079 / (double)hll->num_registers);
    }
    
    /* Alocarea registrelor inițializate la 0 */
    hll->registers = (uint8_t *)calloc(hll->num_registers, sizeof(uint8_t));
    if (!hll->registers) {
        free(hll);
        return NULL;
    }
    
    return hll;
}

/**
 * Numără leading zeros + 1 în reprezentarea binară
 * 
 * Această funcție returnează "rangul" unui hash, definit ca:
 *   rank = poziția primului bit 1 (de la stânga) + 1
 *
 * Exemplu pentru 8 biți:
 *   00000001 → rank = 8
 *   00000010 → rank = 7
 *   10000000 → rank = 1
 *   00000000 → rank = 9 (caz special)
 */
static uint8_t count_leading_zeros_plus_one(uint64_t value, int bits) {
    if (value == 0) return bits + 1;
    
    uint8_t count = 1;
    while ((value & (1ULL << (bits - 1))) == 0 && count <= bits) {
        count++;
        value <<= 1;
    }
    return count;
}

/**
 * SOLUȚIE TODO 7: Adăugarea unui element în HyperLogLog
 *
 * Algoritmul folosește observația statistică că:
 *   - Într-un set de n hash-uri uniforme, maximul leading zeros ≈ log₂(n)
 *
 * Pași:
 *   1. Hash pe 64 biți
 *   2. Primii p biți → index registru (j)
 *   3. Restul biților → calculăm rank (leading zeros + 1)
 *   4. Actualizăm reg[j] = max(reg[j], rank)
 */
void hll_add(HyperLogLog *hll, const void *data, size_t len) {
    /* Calculăm hash-ul pe 64 biți */
    uint64_t hash = hash_fnv1a(data, len);
    
    /* Extragem primii p biți pentru indexul registrului
     * Shiftăm la dreapta cu (64 - p) biți pentru a obține indicele
     */
    size_t index = hash >> (64 - hll->precision);
    
    /* Extragem restul biților pentru calculul rank-ului
     * Shiftăm la stânga cu p biți pentru a elimina indicele
     */
    uint64_t remaining = hash << hll->precision;
    int remaining_bits = 64 - hll->precision;
    
    /* Calculăm rank-ul (leading zeros + 1) */
    uint8_t rank = count_leading_zeros_plus_one(remaining, remaining_bits);
    
    /* Actualizăm registrul cu valoarea maximă
     * Aceasta este operația cheie: păstrăm cel mai mare rank văzut
     */
    if (rank > hll->registers[index]) {
        hll->registers[index] = rank;
    }
}

/**
 * SOLUȚIE TODO 8: Estimarea cardinalității
 *
 * Formula HyperLogLog:
 *   E = α × M² / Σⱼ(2^(-reg[j]))
 *
 * Intuiția:
 *   - Fiecare registru estimează log₂(cardinalitate_substream)
 *   - Media armonică combină estimările în mod robust
 *   - α corectează bias-ul sistematic
 *
 * Complexitate: O(M) pentru parcurgerea registrelor
 */
double hll_count(const HyperLogLog *hll) {
    /* Calculăm suma armonică Σ(2^(-reg[j])) */
    double sum = 0.0;
    size_t empty_registers = 0;
    
    for (size_t i = 0; i < hll->num_registers; i++) {
        sum += pow(2.0, -(double)hll->registers[i]);
        if (hll->registers[i] == 0) {
            empty_registers++;
        }
    }
    
    /* Estimarea brută */
    double m = (double)hll->num_registers;
    double estimate = hll->alpha * m * m / sum;
    
    /* Corecții pentru cardinalități mici și mari */
    if (estimate <= 2.5 * m && empty_registers > 0) {
        /* Linear counting pentru cardinalități mici
         * Când există multe registre goale, folosim formula:
         * E = M * ln(M / V) unde V = numărul de registre goale
         */
        estimate = m * log(m / (double)empty_registers);
    }
    /* Notă: Corecția pentru cardinalități > 2^32 omisă pentru simplitate */
    
    return estimate;
}

/**
 * Eliberare memorie HyperLogLog
 */
void hll_free(HyperLogLog *hll) {
    if (hll) {
        free(hll->registers);
        free(hll);
    }
}

/* =============================================================================
 * STRUCTURA DE RAPORT
 * =============================================================================
 */

typedef struct {
    size_t total_urls;
    size_t duplicates_detected;
    double estimated_unique;
    double bloom_fpr_target;
    double bloom_fpr_actual;
    size_t bloom_memory_bytes;
    size_t hll_memory_bytes;
} DeduplicationReport;

/* =============================================================================
 * SOLUȚIE TODO 9: FUNCȚIA PRINCIPALĂ DE PROCESARE
 * =============================================================================
 */

/**
 * Procesarea stream-ului de URL-uri
 *
 * Strategia:
 *   1. Verificăm cu Bloom dacă URL-ul a fost văzut
 *   2. Dacă NU (sigur nou): adăugăm în ambele structuri
 *   3. Dacă DA (probabil duplicat): numărăm ca duplicat
 *
 * Complexitate per URL: O(k) pentru Bloom + O(1) pentru HLL
 * Memorie totală: O(m/8) pentru Bloom + O(2^p) pentru HLL
 */
int process_urls(const char *filename, size_t expected_n, double target_fpr,
                 DeduplicationReport *report) {
    /* Deschiderea fișierului */
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Eroare: Nu pot deschide fișierul %s\n", filename);
        return -1;
    }
    
    /* Crearea structurilor de date */
    BloomFilter *bf = bloom_create(expected_n, target_fpr);
    HyperLogLog *hll = hll_create(HLL_PRECISION);
    
    if (!bf || !hll) {
        fprintf(stderr, "Eroare: Nu pot aloca structurile de date\n");
        if (bf) bloom_free(bf);
        if (hll) hll_free(hll);
        fclose(fp);
        return -1;
    }
    
    /* Inițializarea contorilor */
    char url[MAX_URL_LENGTH];
    report->total_urls = 0;
    report->duplicates_detected = 0;
    
    /* Procesarea stream-ului linie cu linie */
    while (fgets(url, sizeof(url), fp)) {
        /* Eliminăm newline */
        size_t len = strlen(url);
        if (len > 0 && url[len-1] == '\n') {
            url[len-1] = '\0';
            len--;
        }
        
        /* Ignorăm linii goale */
        if (len == 0) continue;
        
        report->total_urls++;
        
        /* LOGICA PRINCIPALĂ DE DEDUPLICARE
         * 
         * Bloom Filter oferă:
         *   - check() = false → SIGUR element nou
         *   - check() = true  → PROBABIL duplicat (poate fi false positive)
         */
        if (bloom_check(bf, url, len)) {
            /* Bloom zice că am văzut URL-ul înainte
             * Poate fi: duplicat real SAU false positive
             */
            report->duplicates_detected++;
        } else {
            /* Bloom zice SIGUR că nu am văzut URL-ul
             * Adăugăm în ambele structuri
             */
            bloom_add(bf, url, len);
            hll_add(hll, url, len);
        }
    }
    
    /* Completarea raportului cu statistici */
    report->estimated_unique = hll_count(hll);
    report->bloom_fpr_target = target_fpr;
    report->bloom_fpr_actual = bloom_estimate_fpr(bf);
    report->bloom_memory_bytes = bf->size_bytes;
    report->hll_memory_bytes = hll->num_registers * sizeof(uint8_t);
    
    /* Eliberarea resurselor */
    bloom_free(bf);
    hll_free(hll);
    fclose(fp);
    
    return 0;
}

/* =============================================================================
 * FUNCȚII DE AFIȘARE
 * =============================================================================
 */

void print_report(const DeduplicationReport *report) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║               URL Deduplication Report                        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    printf("  Total URLs processed:     %zu\n", report->total_urls);
    printf("  Unique URLs (estimated):  %.0f\n", report->estimated_unique);
    printf("  Duplicates detected:      %zu\n", report->duplicates_detected);
    printf("\n");
    printf("  Bloom Filter FPR target:  %.2f%%\n", report->bloom_fpr_target * 100);
    printf("  Bloom Filter FPR actual:  %.2f%%\n", report->bloom_fpr_actual * 100);
    printf("\n");
    printf("  Memory used (Bloom):      %.2f KB\n", report->bloom_memory_bytes / 1024.0);
    printf("  Memory used (HLL):        %.2f KB\n", report->hll_memory_bytes / 1024.0);
    printf("  Total memory:             %.2f KB\n", 
           (report->bloom_memory_bytes + report->hll_memory_bytes) / 1024.0);
    printf("\n");
    
    /* Analiză comparativă */
    double exact_memory_estimate = report->estimated_unique * 50;  /* ~50 bytes per URL */
    printf("  Estimated memory for exact set: %.2f MB\n", exact_memory_estimate / (1024*1024));
    printf("  Memory savings factor:          %.0fx\n", 
           exact_memory_estimate / (report->bloom_memory_bytes + report->hll_memory_bytes));
}

/* =============================================================================
 * MAIN
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    printf("\n=== SOLUȚIE EXERCIȚIUL 1: Sistem de Deduplicare URL-uri ===\n");
    
    /* Verificare argumente */
    const char *filename;
    if (argc < 2) {
        printf("Utilizare: %s <fisier_urls> [expected_n] [target_fpr]\n", argv[0]);
        printf("Folosim fișierul implicit pentru demonstrație...\n");
        filename = "../data/urls_100k.txt";
    } else {
        filename = argv[1];
    }
    
    size_t expected_n = DEFAULT_EXPECTED_URLS;
    double target_fpr = DEFAULT_FPR;
    
    if (argc >= 3) expected_n = (size_t)atol(argv[2]);
    if (argc >= 4) target_fpr = atof(argv[3]);
    
    printf("\nParametri:\n");
    printf("  Fișier:           %s\n", filename);
    printf("  Expected URLs:    %zu\n", expected_n);
    printf("  Target FPR:       %.2f%%\n", target_fpr * 100);
    
    /* Procesare */
    DeduplicationReport report = {0};
    clock_t start = clock();
    
    if (process_urls(filename, expected_n, target_fpr, &report) != 0) {
        fprintf(stderr, "Eroare la procesarea URL-urilor!\n");
        return 1;
    }
    
    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
    
    /* Afișare raport */
    print_report(&report);
    
    printf("  Processing time:          %.3f seconds\n", elapsed);
    printf("  Throughput:               %.0f URLs/sec\n\n", report.total_urls / elapsed);
    
    return 0;
}
