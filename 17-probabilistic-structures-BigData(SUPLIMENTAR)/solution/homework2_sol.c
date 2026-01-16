/**
 * =============================================================================
 * SOLUȚIE TEMA 2: Monitor de Trafic de Rețea cu Detecție DDoS
 * =============================================================================
 *
 * Această soluție implementează un sistem complet de monitorizare a traficului
 * de rețea folosind structuri de date probabilistice:
 *   - HyperLogLog pentru estimarea IP-urilor unice
 *   - Count-Min Sketch pentru frecvența conexiunilor per IP
 *   - Bloom Filter pentru blacklist și conexiuni văzute
 *   - Sliding window pentru detecție DDoS în timp real
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -O2 -o homework2_sol homework2_sol.c -lm
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
#include <arpa/inet.h>

/* =============================================================================
 * CONSTANTE ȘI CONFIGURARE
 * =============================================================================
 */

#define MAX_IP_LENGTH 16
#define DEFAULT_EXPECTED_IPS 100000
#define DEFAULT_FPR 0.01
#define HLL_PRECISION 14
#define CMS_EPSILON 0.001
#define CMS_DELTA 0.01
#define SLIDING_WINDOW_SIZE 60      /* 60 secunde */
#define DEFAULT_DDOS_THRESHOLD 10000  /* conexiuni/secundă */
#define TOP_N_HEAVY_HITTERS 10

/* =============================================================================
 * FUNCȚII HASH
 * =============================================================================
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

uint64_t hash_djb2(const void *data, size_t len) {
    const uint8_t *bytes = (const uint8_t *)data;
    uint64_t hash = 5381;
    
    for (size_t i = 0; i < len; i++) {
        hash = ((hash << 5) + hash) + bytes[i];
    }
    return hash;
}

uint64_t hash_family(const void *data, size_t len, uint32_t i) {
    uint64_t h1 = hash_fnv1a(data, len);
    uint64_t h2 = hash_djb2(data, len);
    return h1 + i * h2;
}

/* =============================================================================
 * BLOOM FILTER
 * =============================================================================
 */

typedef struct {
    uint8_t *bitmap;
    size_t size_bits;
    size_t size_bytes;
    size_t num_hashes;
    size_t elements_added;
} BloomFilter;

BloomFilter *bloom_create(size_t expected_n, double target_fpr) {
    BloomFilter *bf = (BloomFilter *)malloc(sizeof(BloomFilter));
    if (!bf) return NULL;
    
    double ln2_sq = 0.4804530139182014;
    double m = -((double)expected_n * log(target_fpr)) / ln2_sq;
    double k = (m / (double)expected_n) * 0.693147180559945;
    
    bf->size_bits = (size_t)ceil(m);
    bf->num_hashes = (size_t)ceil(k);
    if (bf->num_hashes < 1) bf->num_hashes = 1;
    bf->size_bytes = (bf->size_bits + 7) / 8;
    
    bf->bitmap = (uint8_t *)calloc(bf->size_bytes, 1);
    if (!bf->bitmap) {
        free(bf);
        return NULL;
    }
    bf->elements_added = 0;
    return bf;
}

void bloom_add(BloomFilter *bf, const void *data, size_t len) {
    for (size_t i = 0; i < bf->num_hashes; i++) {
        uint64_t hash = hash_family(data, len, (uint32_t)i);
        size_t bit_index = hash % bf->size_bits;
        bf->bitmap[bit_index / 8] |= (1 << (bit_index % 8));
    }
    bf->elements_added++;
}

bool bloom_check(const BloomFilter *bf, const void *data, size_t len) {
    for (size_t i = 0; i < bf->num_hashes; i++) {
        uint64_t hash = hash_family(data, len, (uint32_t)i);
        size_t bit_index = hash % bf->size_bits;
        if (!(bf->bitmap[bit_index / 8] & (1 << (bit_index % 8)))) {
            return false;
        }
    }
    return true;
}

void bloom_free(BloomFilter *bf) {
    if (bf) {
        free(bf->bitmap);
        free(bf);
    }
}

/* =============================================================================
 * HYPERLOGLOG
 * =============================================================================
 */

typedef struct {
    uint8_t *registers;
    size_t num_registers;
    uint8_t precision;
    double alpha;
} HyperLogLog;

HyperLogLog *hll_create(uint8_t precision) {
    HyperLogLog *hll = (HyperLogLog *)malloc(sizeof(HyperLogLog));
    if (!hll) return NULL;
    
    hll->precision = precision;
    hll->num_registers = 1ULL << precision;
    
    if (hll->num_registers == 16) hll->alpha = 0.673;
    else if (hll->num_registers == 32) hll->alpha = 0.697;
    else if (hll->num_registers == 64) hll->alpha = 0.709;
    else hll->alpha = 0.7213 / (1.0 + 1.079 / (double)hll->num_registers);
    
    hll->registers = (uint8_t *)calloc(hll->num_registers, sizeof(uint8_t));
    if (!hll->registers) {
        free(hll);
        return NULL;
    }
    return hll;
}

static uint8_t count_leading_zeros_plus_one(uint64_t value, int bits) {
    if (value == 0) return bits + 1;
    uint8_t count = 1;
    while ((value & (1ULL << (bits - 1))) == 0 && count <= bits) {
        count++;
        value <<= 1;
    }
    return count;
}

void hll_add(HyperLogLog *hll, const void *data, size_t len) {
    uint64_t hash = hash_fnv1a(data, len);
    size_t index = hash >> (64 - hll->precision);
    uint64_t remaining = hash << hll->precision;
    int remaining_bits = 64 - hll->precision;
    uint8_t rank = count_leading_zeros_plus_one(remaining, remaining_bits);
    if (rank > hll->registers[index]) {
        hll->registers[index] = rank;
    }
}

double hll_count(const HyperLogLog *hll) {
    double sum = 0.0;
    size_t empty_registers = 0;
    
    for (size_t i = 0; i < hll->num_registers; i++) {
        sum += pow(2.0, -(double)hll->registers[i]);
        if (hll->registers[i] == 0) empty_registers++;
    }
    
    double m = (double)hll->num_registers;
    double estimate = hll->alpha * m * m / sum;
    
    if (estimate <= 2.5 * m && empty_registers > 0) {
        estimate = m * log(m / (double)empty_registers);
    }
    return estimate;
}

void hll_free(HyperLogLog *hll) {
    if (hll) {
        free(hll->registers);
        free(hll);
    }
}

/* =============================================================================
 * COUNT-MIN SKETCH
 * =============================================================================
 */

typedef struct {
    int64_t **counters;
    size_t depth;
    size_t width;
    double epsilon;
    double delta;
    uint64_t total_count;
} CountMinSketch;

CountMinSketch *cms_create(double epsilon, double delta) {
    CountMinSketch *cms = (CountMinSketch *)malloc(sizeof(CountMinSketch));
    if (!cms) return NULL;
    
    cms->width = (size_t)ceil(2.718281828 / epsilon);
    cms->depth = (size_t)ceil(log(1.0 / delta));
    if (cms->width < 100) cms->width = 100;
    if (cms->depth < 3) cms->depth = 3;
    
    cms->epsilon = epsilon;
    cms->delta = delta;
    cms->total_count = 0;
    
    cms->counters = (int64_t **)malloc(cms->depth * sizeof(int64_t *));
    if (!cms->counters) {
        free(cms);
        return NULL;
    }
    
    for (size_t i = 0; i < cms->depth; i++) {
        cms->counters[i] = (int64_t *)calloc(cms->width, sizeof(int64_t));
        if (!cms->counters[i]) {
            for (size_t j = 0; j < i; j++) free(cms->counters[j]);
            free(cms->counters);
            free(cms);
            return NULL;
        }
    }
    return cms;
}

void cms_add(CountMinSketch *cms, const void *data, size_t len, int64_t count) {
    for (size_t i = 0; i < cms->depth; i++) {
        uint64_t hash = hash_family(data, len, (uint32_t)i);
        size_t index = hash % cms->width;
        cms->counters[i][index] += count;
    }
    cms->total_count += count;
}

int64_t cms_estimate(const CountMinSketch *cms, const void *data, size_t len) {
    int64_t min_count = INT64_MAX;
    for (size_t i = 0; i < cms->depth; i++) {
        uint64_t hash = hash_family(data, len, (uint32_t)i);
        size_t index = hash % cms->width;
        if (cms->counters[i][index] < min_count) {
            min_count = cms->counters[i][index];
        }
    }
    return min_count;
}

void cms_reset(CountMinSketch *cms) {
    for (size_t i = 0; i < cms->depth; i++) {
        memset(cms->counters[i], 0, cms->width * sizeof(int64_t));
    }
    cms->total_count = 0;
}

void cms_free(CountMinSketch *cms) {
    if (cms) {
        if (cms->counters) {
            for (size_t i = 0; i < cms->depth; i++) {
                free(cms->counters[i]);
            }
            free(cms->counters);
        }
        free(cms);
    }
}

/* =============================================================================
 * STRUCTURI PENTRU NETWORK MONITORING
 * =============================================================================
 */

/**
 * Structură pentru un pachet de rețea
 */
typedef struct {
    uint32_t src_ip;
    uint32_t dst_ip;
    uint16_t src_port;
    uint16_t dst_port;
    uint8_t protocol;       /* TCP=6, UDP=17 */
    uint32_t payload_size;
    time_t timestamp;
} NetworkPacket;

/**
 * IP suspect
 */
typedef struct {
    char ip_str[MAX_IP_LENGTH];
    uint32_t ip;
    uint64_t connection_count;
    bool is_blacklisted;
    bool is_ddos_suspect;
} SuspiciousIP;

/**
 * Network Monitor principal
 */
typedef struct {
    HyperLogLog *unique_ips;        /* Estimare IP-uri unice */
    CountMinSketch *ip_frequency;   /* Frecvența conexiunilor per IP */
    BloomFilter *blacklist;         /* IP-uri cunoscute ca malițioase */
    BloomFilter *seen_connections;  /* Conexiuni (src:dst:port) văzute */
    
    /* Statistici */
    uint64_t total_packets;
    uint64_t total_bytes;
    uint64_t blacklisted_detected;
    time_t start_time;
    
    /* Pentru heavy hitters tracking */
    uint32_t *candidate_ips;
    size_t num_candidates;
    size_t max_candidates;
} NetworkMonitor;

/* =============================================================================
 * DDOS DETECTOR (Partea B)
 * 
 * Utilizează sliding window cu 60 de Count-Min Sketch-uri pentru 
 * monitorizarea ratei de conexiuni în ultimul minut.
 *
 * Algoritmul:
 *   1. Menținem un CMS pentru fiecare secundă din ultimele 60
 *   2. La fiecare pachet nou:
 *      a. Avansăm fereastra dacă e cazul
 *      b. Adăugăm în CMS-ul curent
 *      c. Verificăm rata agregată
 *   3. Dacă rata depășește pragul → alertă DDoS
 * =============================================================================
 */

typedef struct {
    CountMinSketch *windows[SLIDING_WINDOW_SIZE];  /* 60 CMS-uri */
    int current_window;                             /* Index curent */
    time_t window_start;                            /* Timestamp început fereastră */
    uint64_t ddos_threshold;                        /* Conexiuni/sec threshold */
    uint64_t attacks_detected;                      /* Contor atacuri */
} DDoSDetector;

/**
 * Crează DDoS Detector
 */
DDoSDetector *ddos_create(uint64_t threshold_per_second) {
    DDoSDetector *dd = (DDoSDetector *)malloc(sizeof(DDoSDetector));
    if (!dd) return NULL;
    
    dd->ddos_threshold = threshold_per_second;
    dd->current_window = 0;
    dd->window_start = time(NULL);
    dd->attacks_detected = 0;
    
    /* Creem CMS-uri pentru fiecare fereastră
     * Folosim parametri mai relaxați pentru a economisi memorie
     */
    for (int i = 0; i < SLIDING_WINDOW_SIZE; i++) {
        dd->windows[i] = cms_create(0.01, 0.01);  /* ε=1%, δ=1% */
        if (!dd->windows[i]) {
            for (int j = 0; j < i; j++) {
                cms_free(dd->windows[j]);
            }
            free(dd);
            return NULL;
        }
    }
    
    return dd;
}

/**
 * Avansează fereastra la timestamp-ul curent
 */
static void ddos_advance_window(DDoSDetector *dd, time_t now) {
    time_t elapsed = now - dd->window_start;
    
    while (elapsed > 0 && elapsed < SLIDING_WINDOW_SIZE) {
        /* Avansăm la următoarea fereastră */
        dd->current_window = (dd->current_window + 1) % SLIDING_WINDOW_SIZE;
        
        /* Resetăm fereastra care tocmai a ieșit din interval */
        cms_reset(dd->windows[dd->current_window]);
        
        dd->window_start++;
        elapsed--;
    }
    
    /* Dacă a trecut prea mult timp, resetăm tot */
    if (elapsed >= SLIDING_WINDOW_SIZE) {
        for (int i = 0; i < SLIDING_WINDOW_SIZE; i++) {
            cms_reset(dd->windows[i]);
        }
        dd->window_start = now;
        dd->current_window = 0;
    }
}

/**
 * Obține rata de conexiuni pentru un IP în ultimele N secunde
 */
double ddos_get_rate(const DDoSDetector *dd, uint32_t ip, int seconds) {
    if (seconds <= 0 || seconds > SLIDING_WINDOW_SIZE) {
        seconds = SLIDING_WINDOW_SIZE;
    }
    
    int64_t total = 0;
    
    /* Parcurgem ultimele 'seconds' ferestre */
    for (int i = 0; i < seconds; i++) {
        int window_idx = (dd->current_window - i + SLIDING_WINDOW_SIZE) % SLIDING_WINDOW_SIZE;
        total += cms_estimate(dd->windows[window_idx], &ip, sizeof(ip));
    }
    
    return (double)total / seconds;
}

/**
 * Procesează un pachet și verifică pentru DDoS
 * Returnează IP-ul atacator dacă detectat, 0 altfel
 */
uint32_t ddos_process(DDoSDetector *dd, const NetworkPacket *pkt) {
    /* Avansăm fereastra la timpul curent */
    ddos_advance_window(dd, pkt->timestamp);
    
    /* Adăugăm conexiunea în fereastra curentă */
    cms_add(dd->windows[dd->current_window], &pkt->src_ip, sizeof(pkt->src_ip), 1);
    
    /* Verificăm rata pentru IP-ul sursă (ultimele 10 secunde) */
    double rate = ddos_get_rate(dd, pkt->src_ip, 10);
    
    if (rate > dd->ddos_threshold / 10.0) {  /* Normalizat pe 10 secunde */
        dd->attacks_detected++;
        return pkt->src_ip;
    }
    
    return 0;
}

void ddos_free(DDoSDetector *dd) {
    if (dd) {
        for (int i = 0; i < SLIDING_WINDOW_SIZE; i++) {
            cms_free(dd->windows[i]);
        }
        free(dd);
    }
}

/* =============================================================================
 * NETWORK MONITOR IMPLEMENTATION
 * =============================================================================
 */

/**
 * Crează Network Monitor
 */
NetworkMonitor *monitor_create(size_t expected_ips, double fpr) {
    NetworkMonitor *nm = (NetworkMonitor *)malloc(sizeof(NetworkMonitor));
    if (!nm) return NULL;
    
    nm->unique_ips = hll_create(HLL_PRECISION);
    nm->ip_frequency = cms_create(CMS_EPSILON, CMS_DELTA);
    nm->blacklist = bloom_create(10000, fpr);  /* Presupunem max 10K IP-uri blacklisted */
    nm->seen_connections = bloom_create(expected_ips * 10, fpr);
    
    if (!nm->unique_ips || !nm->ip_frequency || 
        !nm->blacklist || !nm->seen_connections) {
        if (nm->unique_ips) hll_free(nm->unique_ips);
        if (nm->ip_frequency) cms_free(nm->ip_frequency);
        if (nm->blacklist) bloom_free(nm->blacklist);
        if (nm->seen_connections) bloom_free(nm->seen_connections);
        free(nm);
        return NULL;
    }
    
    nm->total_packets = 0;
    nm->total_bytes = 0;
    nm->blacklisted_detected = 0;
    nm->start_time = time(NULL);
    
    /* Heavy hitters tracking */
    nm->max_candidates = 10000;
    nm->candidate_ips = (uint32_t *)calloc(nm->max_candidates, sizeof(uint32_t));
    nm->num_candidates = 0;
    
    return nm;
}

/**
 * Conversie IP în string
 */
static void ip_to_string(uint32_t ip, char *str) {
    struct in_addr addr;
    addr.s_addr = htonl(ip);
    inet_ntop(AF_INET, &addr, str, MAX_IP_LENGTH);
}

/**
 * Conversie string în IP
 */
static uint32_t string_to_ip(const char *str) {
    struct in_addr addr;
    inet_pton(AF_INET, str, &addr);
    return ntohl(addr.s_addr);
}

/**
 * Adaugă IP în blacklist
 */
void monitor_add_to_blacklist(NetworkMonitor *nm, const char *ip_str) {
    uint32_t ip = string_to_ip(ip_str);
    bloom_add(nm->blacklist, &ip, sizeof(ip));
}

/**
 * Verifică dacă un IP este în blacklist
 */
bool monitor_is_blacklisted(const NetworkMonitor *nm, uint32_t ip) {
    return bloom_check(nm->blacklist, &ip, sizeof(ip));
}

/**
 * Procesează un pachet de rețea
 */
void monitor_process_packet(NetworkMonitor *nm, const NetworkPacket *pkt) {
    nm->total_packets++;
    nm->total_bytes += pkt->payload_size;
    
    /* Adăugăm IP-ul sursă în HLL pentru unique counting */
    hll_add(nm->unique_ips, &pkt->src_ip, sizeof(pkt->src_ip));
    
    /* Actualizăm frecvența conexiunilor pentru IP-ul sursă */
    cms_add(nm->ip_frequency, &pkt->src_ip, sizeof(pkt->src_ip), 1);
    
    /* Verificăm dacă IP-ul este în blacklist */
    if (monitor_is_blacklisted(nm, pkt->src_ip)) {
        nm->blacklisted_detected++;
    }
    
    /* Marcăm conexiunea ca văzută (src:dst:port) */
    struct {
        uint32_t src;
        uint32_t dst;
        uint16_t port;
    } conn_key = {pkt->src_ip, pkt->dst_ip, pkt->dst_port};
    bloom_add(nm->seen_connections, &conn_key, sizeof(conn_key));
    
    /* Tracking pentru heavy hitters */
    if (nm->num_candidates < nm->max_candidates) {
        /* Verificăm dacă IP-ul e deja candidat */
        bool found = false;
        for (size_t i = 0; i < nm->num_candidates; i++) {
            if (nm->candidate_ips[i] == pkt->src_ip) {
                found = true;
                break;
            }
        }
        if (!found) {
            nm->candidate_ips[nm->num_candidates++] = pkt->src_ip;
        }
    }
}

/**
 * Comparator pentru sortare heavy hitters
 */
static CountMinSketch *_sort_cms;  /* Temporar pentru qsort */
static int compare_ips_by_freq(const void *a, const void *b) {
    uint32_t ip_a = *(const uint32_t *)a;
    uint32_t ip_b = *(const uint32_t *)b;
    int64_t freq_a = cms_estimate(_sort_cms, &ip_a, sizeof(ip_a));
    int64_t freq_b = cms_estimate(_sort_cms, &ip_b, sizeof(ip_b));
    if (freq_b > freq_a) return 1;
    if (freq_b < freq_a) return -1;
    return 0;
}

/**
 * Găsește IP-urile cu cele mai multe conexiuni (heavy hitters)
 */
void monitor_find_heavy_hitters(const NetworkMonitor *nm, 
                                SuspiciousIP *results, int top_n) {
    if (top_n > (int)nm->num_candidates) {
        top_n = (int)nm->num_candidates;
    }
    
    /* Copiem candidații pentru sortare */
    uint32_t *sorted_ips = (uint32_t *)malloc(nm->num_candidates * sizeof(uint32_t));
    memcpy(sorted_ips, nm->candidate_ips, nm->num_candidates * sizeof(uint32_t));
    
    /* Sortăm după frecvență (folosim global temporar) */
    _sort_cms = nm->ip_frequency;
    qsort(sorted_ips, nm->num_candidates, sizeof(uint32_t), compare_ips_by_freq);
    
    /* Populăm rezultatele */
    for (int i = 0; i < top_n; i++) {
        results[i].ip = sorted_ips[i];
        ip_to_string(sorted_ips[i], results[i].ip_str);
        results[i].connection_count = cms_estimate(nm->ip_frequency, 
                                                    &sorted_ips[i], sizeof(uint32_t));
        results[i].is_blacklisted = monitor_is_blacklisted(nm, sorted_ips[i]);
        results[i].is_ddos_suspect = (results[i].connection_count > 
                                      DEFAULT_DDOS_THRESHOLD / 60);
    }
    
    free(sorted_ips);
}

/**
 * Verifică dacă un IP este suspect
 */
bool monitor_is_suspicious(const NetworkMonitor *nm, uint32_t ip, 
                           uint64_t threshold) {
    /* Verificăm blacklist */
    if (monitor_is_blacklisted(nm, ip)) {
        return true;
    }
    
    /* Verificăm frecvența */
    int64_t freq = cms_estimate(nm->ip_frequency, &ip, sizeof(ip));
    if ((uint64_t)freq > threshold) {
        return true;
    }
    
    return false;
}

/**
 * Afișează raportul network monitor
 */
void monitor_print_report(const NetworkMonitor *nm) {
    time_t now = time(NULL);
    double elapsed = difftime(now, nm->start_time);
    if (elapsed < 1) elapsed = 1;
    
    double estimated_unique = hll_count(nm->unique_ips);
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════════╗\n");
    printf("║            Network Traffic Monitor - Report                            ║\n");
    printf("╠═══════════════════════════════════════════════════════════════════════╣\n");
    printf("║  Monitoring duration:     %.0f seconds                              \n", elapsed);
    printf("║  Packets processed:       %lu                                       \n", nm->total_packets);
    printf("║  Unique IPs (estimated):  %.0f                                      \n", estimated_unique);
    printf("║  Bytes transferred:       %.2f MB                                   \n", nm->total_bytes / (1024.0 * 1024.0));
    printf("╠═══════════════════════════════════════════════════════════════════════╣\n");
    printf("║  Packets/second:          %.0f                                      \n", nm->total_packets / elapsed);
    printf("║  Bytes/second:            %.2f KB                                   \n", nm->total_bytes / elapsed / 1024.0);
    printf("╠═══════════════════════════════════════════════════════════════════════╣\n");
    printf("║  Blacklisted IPs detected: %lu                                      \n", nm->blacklisted_detected);
    printf("╠═══════════════════════════════════════════════════════════════════════╣\n");
    
    /* Heavy Hitters */
    printf("║  TOP-%d CONNECTIONS (by frequency)                                  \n", TOP_N_HEAVY_HITTERS);
    printf("║  ┌────────────────┬──────────────┬───────────────┬─────────────────┐\n");
    printf("║  │ Source IP      │ Connections  │ Status        │ Action          │\n");
    printf("║  ├────────────────┼──────────────┼───────────────┼─────────────────┤\n");
    
    SuspiciousIP heavy_hitters[TOP_N_HEAVY_HITTERS];
    monitor_find_heavy_hitters(nm, heavy_hitters, TOP_N_HEAVY_HITTERS);
    
    for (int i = 0; i < TOP_N_HEAVY_HITTERS && i < (int)nm->num_candidates; i++) {
        const char *status = "🟢 Normal";
        const char *action = "-";
        
        if (heavy_hitters[i].is_blacklisted) {
            status = "🔴 Blacklist";
            action = "BLOCKED";
        } else if (heavy_hitters[i].is_ddos_suspect) {
            status = "🔴 DDoS!";
            action = "BLOCKED";
        } else if (heavy_hitters[i].connection_count > 1000) {
            status = "🟡 Watch";
            action = "Monitoring";
        }
        
        printf("║  │ %-14s │ %12lu │ %-13s │ %-15s │\n",
               heavy_hitters[i].ip_str,
               heavy_hitters[i].connection_count,
               status,
               action);
    }
    
    printf("║  └────────────────┴──────────────┴───────────────┴─────────────────┘\n");
    printf("╠═══════════════════════════════════════════════════════════════════════╣\n");
    
    /* Memory Usage */
    size_t hll_mem = nm->unique_ips->num_registers;
    size_t cms_mem = nm->ip_frequency->depth * nm->ip_frequency->width * sizeof(int64_t);
    size_t bf_mem = nm->blacklist->size_bytes + nm->seen_connections->size_bytes;
    size_t total_mem = hll_mem + cms_mem + bf_mem;
    
    printf("║  Memory Usage:                                                        \n");
    printf("║    HyperLogLog:           %.2f KB                                   \n", hll_mem / 1024.0);
    printf("║    Count-Min Sketch:      %.2f KB                                   \n", cms_mem / 1024.0);
    printf("║    Bloom Filters:         %.2f KB                                   \n", bf_mem / 1024.0);
    printf("║    Total:                 %.2f KB                                   \n", total_mem / 1024.0);
    printf("╚═══════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

void monitor_free(NetworkMonitor *nm) {
    if (nm) {
        hll_free(nm->unique_ips);
        cms_free(nm->ip_frequency);
        bloom_free(nm->blacklist);
        bloom_free(nm->seen_connections);
        free(nm->candidate_ips);
        free(nm);
    }
}

/* =============================================================================
 * SIMULARE TRAFIC DE REȚEA
 * =============================================================================
 */

/**
 * Generează un pachet de rețea random
 */
void generate_random_packet(NetworkPacket *pkt, time_t timestamp) {
    /* IP-uri din rețele private */
    pkt->src_ip = 0xC0A80000 | (rand() % 65536);  /* 192.168.x.x */
    pkt->dst_ip = 0x0A000000 | (rand() % 16777216);  /* 10.x.x.x */
    pkt->src_port = 1024 + (rand() % 64512);
    pkt->dst_port = (rand() % 10 < 7) ? 80 : 443;  /* 70% HTTP, 30% HTTPS */
    pkt->protocol = 6;  /* TCP */
    pkt->payload_size = 64 + (rand() % 1460);  /* MTU tipic */
    pkt->timestamp = timestamp;
}

/**
 * Generează pachete de atac DDoS
 */
void generate_ddos_packet(NetworkPacket *pkt, uint32_t attacker_ip, time_t timestamp) {
    pkt->src_ip = attacker_ip;
    pkt->dst_ip = 0x0A000001;  /* 10.0.0.1 - target */
    pkt->src_port = 1024 + (rand() % 64512);
    pkt->dst_port = 80;
    pkt->protocol = 6;
    pkt->payload_size = 64;  /* Pachete mici pentru flood */
    pkt->timestamp = timestamp;
}

/**
 * Rulează simularea completă
 */
void run_simulation(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════════╗\n");
    printf("║          SIMULARE: Network Traffic Monitor cu Detecție DDoS           ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    srand((unsigned int)time(NULL));
    
    /* Creare monitor și DDoS detector */
    NetworkMonitor *nm = monitor_create(DEFAULT_EXPECTED_IPS, DEFAULT_FPR);
    DDoSDetector *dd = ddos_create(DEFAULT_DDOS_THRESHOLD);
    
    if (!nm || !dd) {
        fprintf(stderr, "Eroare la crearea componentelor!\n");
        return;
    }
    
    /* Adăugăm câteva IP-uri în blacklist */
    printf("  Configurare blacklist...\n");
    monitor_add_to_blacklist(nm, "192.168.100.1");
    monitor_add_to_blacklist(nm, "192.168.100.2");
    monitor_add_to_blacklist(nm, "192.168.100.3");
    printf("    ✓ 3 IP-uri adăugate în blacklist\n");
    
    /* Simulare trafic */
    const int SIMULATION_SECONDS = 60;
    const int PACKETS_PER_SECOND = 10000;
    const int DDOS_START_SECOND = 30;
    const int DDOS_PACKETS_PER_SECOND = 50000;
    uint32_t attacker_ip = string_to_ip("192.168.200.1");
    
    printf("\n  Configurare simulare:\n");
    printf("    Durată:               %d secunde\n", SIMULATION_SECONDS);
    printf("    Trafic normal:        %d pachete/sec\n", PACKETS_PER_SECOND);
    printf("    DDoS starts at:       secunda %d\n", DDOS_START_SECOND);
    printf("    DDoS rate:            %d pachete/sec\n", DDOS_PACKETS_PER_SECOND);
    printf("\n");
    
    printf("  Rulare simulare...\n\n");
    
    time_t start_time = time(NULL);
    int ddos_alerts = 0;
    
    for (int sec = 0; sec < SIMULATION_SECONDS; sec++) {
        time_t current_time = start_time + sec;
        
        /* Trafic normal */
        for (int i = 0; i < PACKETS_PER_SECOND; i++) {
            NetworkPacket pkt;
            generate_random_packet(&pkt, current_time);
            monitor_process_packet(nm, &pkt);
            ddos_process(dd, &pkt);
        }
        
        /* DDoS attack după secunda 30 */
        if (sec >= DDOS_START_SECOND) {
            for (int i = 0; i < DDOS_PACKETS_PER_SECOND; i++) {
                NetworkPacket pkt;
                generate_ddos_packet(&pkt, attacker_ip, current_time);
                monitor_process_packet(nm, &pkt);
                
                uint32_t detected = ddos_process(dd, &pkt);
                if (detected && ddos_alerts == 0) {
                    char ip_str[MAX_IP_LENGTH];
                    ip_to_string(detected, ip_str);
                    printf("  ⚠️  DDOS ALERT at second %d! Attacker: %s\n", sec, ip_str);
                    ddos_alerts++;
                }
            }
        }
        
        /* Progress indicator */
        if ((sec + 1) % 10 == 0) {
            printf("  Progress: %d/%d seconds (%.0f%% complete)\n", 
                   sec + 1, SIMULATION_SECONDS, 
                   100.0 * (sec + 1) / SIMULATION_SECONDS);
        }
    }
    
    printf("\n  Simulare completă!\n");
    printf("  Total DDoS alerts:      %d\n", ddos_alerts);
    printf("  Attacks blocked:        %lu\n", dd->attacks_detected);
    
    /* Afișare raport */
    monitor_print_report(nm);
    
    /* Cleanup */
    monitor_free(nm);
    ddos_free(dd);
}

/* =============================================================================
 * MAIN
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════════╗\n");
    printf("║     SOLUȚIE TEMA 2: Monitor de Trafic de Rețea cu Detecție DDoS       ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════════╝\n");
    
    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        printf("\nUtilizare:\n");
        printf("  %s                    - Rulează simularea standard\n", argv[0]);
        printf("  %s <packets.csv>      - Procesează fișier CSV\n", argv[0]);
        printf("  %s --simulate-ddos    - Simulează atac DDoS\n", argv[0]);
        return 0;
    }
    
    run_simulation();
    
    return 0;
}
