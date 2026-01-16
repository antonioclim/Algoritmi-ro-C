/**
 * =============================================================================
 * EXERCIȚIUL 1: Sistem de Deduplicare URL-uri
 * =============================================================================
 *
 * OBIECTIV:
 *   Implementarea unui sistem care procesează un stream de URL-uri și 
 *   identifică duplicatele folosind Bloom Filter, raportând statistici
 *   de cardinalitate cu HyperLogLog.
 *
 * CERINȚE:
 *   1. Implementați un Bloom Filter cu parametri configurabili
 *   2. Calculați automat dimensiunea optimă și numărul de hash-uri
 *   3. Procesați un fișier cu URL-uri linie cu linie
 *   4. Folosiți HyperLogLog pentru estimarea cardinalității
 *   5. Raportați statistici detaliate
 *
 * EXEMPLU UTILIZARE:
 *   ./exercise1 data/urls_100k.txt
 *
 * EXPECTED OUTPUT:
 *   === URL Deduplication Report ===
 *   Total URLs processed:     100,000
 *   Unique URLs (estimated):   78,432
 *   Duplicates detected:       21,568
 *   Bloom Filter FPR target:     1.00%
 *   Bloom Filter FPR actual:     0.97%
 *   Memory used (Bloom):      117.2 KB
 *   Memory used (HLL):          1.5 KB
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -o exercise1 exercise1.c -lm
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
 * FUNCȚII HASH (FURNIZATE - NU MODIFICAȚI)
 * =============================================================================
 */

/**
 * FNV-1a Hash
 */
uint64_t hash_fnv1a(const void *data, size_t len) {
    const uint8_t *bytes = (const uint8_t *)data;
    uint64_t hash = 14695981039346656037ULL;
    const uint64_t prime = 1099511628211ULL;
    
    for (size_t i = 0; i < len; i++) {
        hash ^= bytes[i];
        hash *= prime;
    }
    
    return hash;
}

/**
 * DJB2 Hash
 */
uint64_t hash_djb2(const void *data, size_t len) {
    const uint8_t *bytes = (const uint8_t *)data;
    uint64_t hash = 5381;
    
    for (size_t i = 0; i < len; i++) {
        hash = ((hash << 5) + hash) + bytes[i];
    }
    
    return hash;
}

/**
 * Familia de hash-uri (double hashing)
 */
uint64_t hash_family(const void *data, size_t len, uint32_t i) {
    uint64_t h1 = hash_fnv1a(data, len);
    uint64_t h2 = hash_djb2(data, len);
    return h1 + i * h2;
}

/* =============================================================================
 * STRUCTURA BLOOM FILTER
 * =============================================================================
 */

/**
 * TODO 1: Completați structura BloomFilter
 *
 * Hint: Aveți nevoie de:
 *   - Un array de biți (bitmap)
 *   - Dimensiunea în biți și bytes
 *   - Numărul de funcții hash
 *   - Contor de elemente adăugate
 *   - Rata de false positives țintă
 */
typedef struct {
    /* YOUR CODE HERE */
    uint8_t *bitmap;           /* Array de biți */
    size_t size_bits;          /* Dimensiunea în biți (m) */
    size_t size_bytes;         /* Dimensiunea în bytes */
    size_t num_hashes;         /* Numărul de funcții hash (k) */
    size_t elements_added;     /* Contor elemente */
    double target_fpr;         /* FPR țintă */
} BloomFilter;

/**
 * TODO 2: Implementați funcția de calcul parametri optimi
 *
 * Formule:
 *   m = -n * ln(p) / (ln(2))²
 *   k = (m/n) * ln(2)
 *
 * @param n       Numărul așteptat de elemente
 * @param fpr     Rata de false positives dorită (ex: 0.01)
 * @param out_m   Output: numărul de biți
 * @param out_k   Output: numărul de funcții hash
 */
void bloom_calculate_params(size_t n, double fpr, size_t *out_m, size_t *out_k) {
    /* YOUR CODE HERE */
    
    /* Hint: 
     * double ln2_sq = 0.4804530139182014;  // ln(2)²
     * Folosiți log() pentru logaritm natural
     * Folosiți ceil() pentru rotunjire în sus
     */
    
    /* Implementare placeholder - înlocuiți! */
    *out_m = 10000;  /* Placeholder */
    *out_k = 7;      /* Placeholder */
}

/**
 * TODO 3: Implementați funcția de creare Bloom Filter
 *
 * Pași:
 *   1. Alocați structura BloomFilter
 *   2. Calculați parametrii optimi cu bloom_calculate_params
 *   3. Alocați bitmap-ul (folosiți calloc pentru inițializare la 0)
 *   4. Inițializați celelalte câmpuri
 *
 * @param expected_n  Numărul așteptat de elemente
 * @param target_fpr  Rata de false positives țintă
 * @return Pointer la BloomFilter sau NULL la eroare
 */
BloomFilter *bloom_create(size_t expected_n, double target_fpr) {
    /* YOUR CODE HERE */
    
    BloomFilter *bf = (BloomFilter *)malloc(sizeof(BloomFilter));
    if (!bf) return NULL;
    
    /* Calculați parametrii optimi */
    /* Hint: bloom_calculate_params(expected_n, target_fpr, &bf->size_bits, &bf->num_hashes); */
    
    /* Alocați bitmap-ul */
    /* Hint: bf->size_bytes = (bf->size_bits + 7) / 8; */
    /* Hint: bf->bitmap = (uint8_t *)calloc(bf->size_bytes, 1); */
    
    /* Inițializați restul câmpurilor */
    
    return bf;  /* Înlocuiți cu implementarea completă */
}

/**
 * TODO 4: Implementați funcția de adăugare element
 *
 * Algoritm:
 *   Pentru i de la 0 la num_hashes-1:
 *     1. Calculați hash_i = hash_family(data, len, i)
 *     2. Calculați index = hash_i % size_bits
 *     3. Setați bitul la poziția index în bitmap
 *   Incrementați elements_added
 *
 * @param bf    Bloom Filter
 * @param data  Pointer la date
 * @param len   Lungimea datelor în bytes
 */
void bloom_add(BloomFilter *bf, const void *data, size_t len) {
    /* YOUR CODE HERE */
    
    /* Hint pentru setarea unui bit:
     * size_t byte_index = index / 8;
     * size_t bit_offset = index % 8;
     * bf->bitmap[byte_index] |= (1 << bit_offset);
     */
}

/**
 * TODO 5: Implementați funcția de verificare apartenență
 *
 * Algoritm:
 *   Pentru i de la 0 la num_hashes-1:
 *     1. Calculați hash_i = hash_family(data, len, i)
 *     2. Calculați index = hash_i % size_bits
 *     3. Dacă bitul la index NU este setat, returnați false
 *   Returnați true (toate biturile sunt setate)
 *
 * @param bf    Bloom Filter
 * @param data  Pointer la date
 * @param len   Lungimea datelor în bytes
 * @return true dacă elementul PROBABIL este în set, false dacă SIGUR NU este
 */
bool bloom_check(const BloomFilter *bf, const void *data, size_t len) {
    /* YOUR CODE HERE */
    
    /* Hint pentru verificarea unui bit:
     * size_t byte_index = index / 8;
     * size_t bit_offset = index % 8;
     * bool is_set = (bf->bitmap[byte_index] & (1 << bit_offset)) != 0;
     */
    
    return true;  /* Placeholder - înlocuiți! */
}

/**
 * Eliberare memorie (furnizată)
 */
void bloom_free(BloomFilter *bf) {
    if (bf) {
        free(bf->bitmap);
        free(bf);
    }
}

/**
 * Estimare FPR actual (furnizată)
 */
double bloom_estimate_fpr(const BloomFilter *bf) {
    double exponent = -(double)(bf->num_hashes * bf->elements_added) / 
                       (double)bf->size_bits;
    double base = 1.0 - exp(exponent);
    return pow(base, bf->num_hashes);
}

/* =============================================================================
 * STRUCTURA HYPERLOGLOG
 * =============================================================================
 */

/**
 * Structura HyperLogLog (furnizată)
 */
typedef struct {
    uint8_t *registers;
    size_t num_registers;
    uint8_t precision;
    double alpha;
} HyperLogLog;

/**
 * TODO 6: Implementați funcția de creare HyperLogLog
 *
 * @param precision Precizia p (tipic 14)
 * @return Pointer la HyperLogLog sau NULL la eroare
 */
HyperLogLog *hll_create(uint8_t precision) {
    /* YOUR CODE HERE */
    
    HyperLogLog *hll = (HyperLogLog *)malloc(sizeof(HyperLogLog));
    if (!hll) return NULL;
    
    hll->precision = precision;
    hll->num_registers = 1ULL << precision;  /* M = 2^p */
    
    /* Calculați alpha */
    /* Hint: 
     * if (hll->num_registers >= 128)
     *     hll->alpha = 0.7213 / (1.0 + 1.079 / hll->num_registers);
     */
    
    /* Alocați registrele */
    /* Hint: hll->registers = (uint8_t *)calloc(hll->num_registers, sizeof(uint8_t)); */
    
    return hll;  /* Înlocuiți cu implementarea completă */
}

/**
 * Numără leading zeros + 1 (furnizată)
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
 * TODO 7: Implementați funcția de adăugare element în HyperLogLog
 *
 * Algoritm:
 *   1. Calculați hash pe 64 biți
 *   2. Primii p biți -> index registru (hash >> (64 - p))
 *   3. Restul biților -> numărați leading zeros + 1
 *   4. Actualizați registrul cu maximul
 *
 * @param hll   HyperLogLog
 * @param data  Pointer la date
 * @param len   Lungimea datelor
 */
void hll_add(HyperLogLog *hll, const void *data, size_t len) {
    /* YOUR CODE HERE */
    
    /* Hint:
     * uint64_t hash = hash_fnv1a(data, len);
     * size_t index = hash >> (64 - hll->precision);
     * uint64_t remaining = hash << hll->precision;
     * int remaining_bits = 64 - hll->precision;
     * uint8_t rank = count_leading_zeros_plus_one(remaining, remaining_bits);
     * if (rank > hll->registers[index]) {
     *     hll->registers[index] = rank;
     * }
     */
}

/**
 * TODO 8: Implementați funcția de estimare cardinalitate
 *
 * Formula: E = α × M² / Σ(2^(-reg[j]))
 *
 * @param hll HyperLogLog
 * @return Cardinalitatea estimată
 */
double hll_count(const HyperLogLog *hll) {
    /* YOUR CODE HERE */
    
    /* Hint:
     * double sum = 0.0;
     * for (size_t i = 0; i < hll->num_registers; i++) {
     *     sum += pow(2.0, -(double)hll->registers[i]);
     * }
     * double m = (double)hll->num_registers;
     * return hll->alpha * m * m / sum;
     */
    
    return 0.0;  /* Placeholder - înlocuiți! */
}

/**
 * Eliberare memorie (furnizată)
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
 * FUNCȚIA PRINCIPALĂ DE PROCESARE
 * =============================================================================
 */

/**
 * TODO 9: Implementați funcția de procesare URL-uri
 *
 * Algoritm:
 *   1. Creați BloomFilter și HyperLogLog
 *   2. Pentru fiecare linie din fișier:
 *      a. Verificați cu Bloom dacă URL-ul a fost văzut
 *      b. Dacă NU -> adăugați în Bloom și HLL, este URL nou
 *      c. Dacă DA -> incrementați duplicates (poate fi false positive)
 *   3. Completați raportul
 *
 * @param filename    Calea către fișierul cu URL-uri
 * @param expected_n  Numărul așteptat de URL-uri unice
 * @param target_fpr  Rata de false positives țintă
 * @param report      Pointer la structura de raport (output)
 * @return 0 la succes, -1 la eroare
 */
int process_urls(const char *filename, size_t expected_n, double target_fpr,
                 DeduplicationReport *report) {
    /* YOUR CODE HERE */
    
    /* Pași:
     * 1. Deschideți fișierul
     * 2. Creați Bloom Filter și HyperLogLog
     * 3. Citiți fiecare linie și procesați
     * 4. Completați raportul
     * 5. Eliberați resursele
     */
    
    /* Placeholder implementation - înlocuiți! */
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Eroare: Nu pot deschide fișierul %s\n", filename);
        return -1;
    }
    
    BloomFilter *bf = bloom_create(expected_n, target_fpr);
    HyperLogLog *hll = hll_create(HLL_PRECISION);
    
    if (!bf || !hll) {
        fprintf(stderr, "Eroare: Nu pot aloca structurile de date\n");
        if (bf) bloom_free(bf);
        if (hll) hll_free(hll);
        fclose(fp);
        return -1;
    }
    
    char url[MAX_URL_LENGTH];
    report->total_urls = 0;
    report->duplicates_detected = 0;
    
    while (fgets(url, sizeof(url), fp)) {
        /* Elimină newline */
        size_t len = strlen(url);
        if (len > 0 && url[len-1] == '\n') {
            url[len-1] = '\0';
            len--;
        }
        
        if (len == 0) continue;
        
        report->total_urls++;
        
        /* TODO: Verificați și adăugați URL-ul */
        /* Hint:
         * if (bloom_check(bf, url, len)) {
         *     report->duplicates_detected++;
         * } else {
         *     bloom_add(bf, url, len);
         *     hll_add(hll, url, len);
         * }
         */
    }
    
    /* Completați raportul */
    report->estimated_unique = hll_count(hll);
    report->bloom_fpr_target = target_fpr;
    report->bloom_fpr_actual = bloom_estimate_fpr(bf);
    report->bloom_memory_bytes = bf->size_bytes;
    report->hll_memory_bytes = hll->num_registers * sizeof(uint8_t);
    
    bloom_free(bf);
    hll_free(hll);
    fclose(fp);
    
    return 0;
}

/* =============================================================================
 * FUNCȚII DE AFIȘARE (FURNIZATE)
 * =============================================================================
 */

void print_report(const DeduplicationReport *report) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║               URL Deduplication Report                        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    printf("  Total URLs processed:     %'zu\n", report->total_urls);
    printf("  Unique URLs (estimated):  %'.0f\n", report->estimated_unique);
    printf("  Duplicates detected:      %'zu\n", report->duplicates_detected);
    printf("\n");
    printf("  Bloom Filter FPR target:  %.2f%%\n", report->bloom_fpr_target * 100);
    printf("  Bloom Filter FPR actual:  %.2f%%\n", report->bloom_fpr_actual * 100);
    printf("\n");
    printf("  Memory used (Bloom):      %.2f KB\n", report->bloom_memory_bytes / 1024.0);
    printf("  Memory used (HLL):        %.2f KB\n", report->hll_memory_bytes / 1024.0);
    printf("  Total memory:             %.2f KB\n", 
           (report->bloom_memory_bytes + report->hll_memory_bytes) / 1024.0);
    printf("\n");
}

/* =============================================================================
 * MAIN
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    printf("\n=== EXERCIȚIUL 1: Sistem de Deduplicare URL-uri ===\n");
    
    /* Verificare argumente */
    const char *filename;
    if (argc < 2) {
        printf("Utilizare: %s <fisier_urls> [expected_n] [target_fpr]\n", argv[0]);
        printf("Folosim fișierul implicit pentru demonstrație...\n");
        filename = "data/urls_100k.txt";
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
    
    if (process_urls(filename, expected_n, target_fpr, &report) != 0) {
        fprintf(stderr, "Eroare la procesarea URL-urilor!\n");
        return 1;
    }
    
    /* Afișare raport */
    print_report(&report);
    
    return 0;
}
