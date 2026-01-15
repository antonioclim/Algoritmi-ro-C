/**
 * =============================================================================
 * SOLUȚIE TEMA 2: Simulator de Rețea cu Buffere de Pachete
 * =============================================================================
 *
 * DOAR PENTRU INSTRUCTORI - Nu distribuiți studenților!
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -o homework2_sol homework2_sol.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

/* =============================================================================
 * CONSTANTE
 * =============================================================================
 */

#define BUFFER_SIZE 100

#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_RED     "\033[31m"
#define COLOR_CYAN    "\033[36m"

/* Protocol numbers */
#define PROTO_ICMP 1
#define PROTO_TCP  6
#define PROTO_UDP  17

/* =============================================================================
 * TIPURI DE DATE
 * =============================================================================
 */

/**
 * Structura Packet
 */
typedef struct {
    uint32_t source_ip;
    uint32_t dest_ip;
    uint16_t source_port;
    uint16_t dest_port;
    uint8_t protocol;
    uint16_t size;
    uint8_t ttl;
    uint32_t sequence;
} Packet;

/**
 * Structura RouterBuffer - Circular Buffer pentru pachete
 */
typedef struct {
    Packet buffer[BUFFER_SIZE];
    int front;
    int rear;
    int count;
    
    /* Statistici */
    uint64_t packets_received;
    uint64_t packets_forwarded;
    uint64_t packets_dropped;
    uint64_t bytes_total;
} RouterBuffer;

/* =============================================================================
 * FUNCȚII AUXILIARE
 * =============================================================================
 */

/**
 * Convertește IP din uint32_t în string
 */
void ip_to_string(uint32_t ip, char *buffer) {
    sprintf(buffer, "%u.%u.%u.%u",
            (ip >> 24) & 0xFF,
            (ip >> 16) & 0xFF,
            (ip >> 8) & 0xFF,
            ip & 0xFF);
}

/**
 * Creează un IP din octeti
 */
uint32_t make_ip(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
    return ((uint32_t)a << 24) | ((uint32_t)b << 16) | 
           ((uint32_t)c << 8) | (uint32_t)d;
}

/**
 * Returnează numele protocolului
 */
const char* protocol_name(uint8_t proto) {
    switch (proto) {
        case PROTO_ICMP: return "ICMP";
        case PROTO_TCP: return "TCP";
        case PROTO_UDP: return "UDP";
        default: return "UNKNOWN";
    }
}

/* =============================================================================
 * FUNCȚII PENTRU ROUTER BUFFER
 * =============================================================================
 */

/**
 * Inițializare router buffer
 */
void router_init(RouterBuffer *rb) {
    rb->front = 0;
    rb->rear = 0;
    rb->count = 0;
    rb->packets_received = 0;
    rb->packets_forwarded = 0;
    rb->packets_dropped = 0;
    rb->bytes_total = 0;
}

/**
 * Verifică dacă bufferul este gol
 */
bool router_is_empty(const RouterBuffer *rb) {
    return rb->count == 0;
}

/**
 * Verifică dacă bufferul este plin
 */
bool router_is_full(const RouterBuffer *rb) {
    return rb->count >= BUFFER_SIZE;
}

/**
 * Primește un pachet în buffer
 * Returnează false dacă bufferul e plin (pachet pierdut)
 */
bool router_receive_packet(RouterBuffer *rb, Packet pkt) {
    rb->packets_received++;
    rb->bytes_total += pkt.size;
    
    if (router_is_full(rb)) {
        rb->packets_dropped++;
        return false;  /* Buffer overflow - pachet pierdut */
    }
    
    /* Adăugăm pachetul în buffer */
    rb->buffer[rb->rear] = pkt;
    rb->rear = (rb->rear + 1) % BUFFER_SIZE;
    rb->count++;
    
    return true;
}

/**
 * Transmite (forward) următorul pachet din buffer
 * Returnează false dacă bufferul e gol
 */
bool router_forward_packet(RouterBuffer *rb, Packet *pkt) {
    if (router_is_empty(rb)) {
        return false;
    }
    
    /* Extragem pachetul */
    *pkt = rb->buffer[rb->front];
    rb->front = (rb->front + 1) % BUFFER_SIZE;
    rb->count--;
    
    rb->packets_forwarded++;
    
    return true;
}

/**
 * Returnează procentajul de ocupare al bufferului
 */
int router_buffer_usage(const RouterBuffer *rb) {
    return (rb->count * 100) / BUFFER_SIZE;
}

/**
 * Returnează rata de pierdere (drop rate)
 */
double router_drop_rate(const RouterBuffer *rb) {
    if (rb->packets_received == 0) return 0.0;
    return (double)rb->packets_dropped * 100.0 / rb->packets_received;
}

/**
 * Golește bufferul și resetează statisticile
 */
void router_flush(RouterBuffer *rb) {
    rb->front = 0;
    rb->rear = 0;
    rb->count = 0;
    /* Păstrăm statisticile sau le resetăm - aici resetăm tot */
    rb->packets_received = 0;
    rb->packets_forwarded = 0;
    rb->packets_dropped = 0;
    rb->bytes_total = 0;
}

/**
 * Afișează statisticile routerului
 */
void router_print_stats(const RouterBuffer *rb) {
    printf("\n" COLOR_CYAN "═══ Router Statistics ═══" COLOR_RESET "\n");
    printf("  Packets received:  %lu\n", (unsigned long)rb->packets_received);
    printf("  Packets forwarded: %lu\n", (unsigned long)rb->packets_forwarded);
    printf("  Packets dropped:   %lu\n", (unsigned long)rb->packets_dropped);
    printf("  Drop rate:         %.2f%%\n", router_drop_rate(rb));
    printf("  Buffer usage:      %d%% (%d/%d)\n", 
           router_buffer_usage(rb), rb->count, BUFFER_SIZE);
    
    /* Afișare bytes în format human-readable */
    double bytes = (double)rb->bytes_total;
    const char *unit = "B";
    if (bytes >= 1024*1024) {
        bytes /= (1024*1024);
        unit = "MB";
    } else if (bytes >= 1024) {
        bytes /= 1024;
        unit = "KB";
    }
    printf("  Total bytes:       %.2f %s\n", bytes, unit);
}

/**
 * Afișează un pachet
 */
void print_packet(const Packet *pkt) {
    char src_ip[20], dst_ip[20];
    ip_to_string(pkt->source_ip, src_ip);
    ip_to_string(pkt->dest_ip, dst_ip);
    
    printf("  [SEQ %u] %s:%u -> %s:%u (%s, %u bytes, TTL=%u)\n",
           pkt->sequence,
           src_ip, pkt->source_port,
           dst_ip, pkt->dest_port,
           protocol_name(pkt->protocol),
           pkt->size, pkt->ttl);
}

/* =============================================================================
 * SIMULARE TRAFIC
 * =============================================================================
 */

static uint32_t packet_sequence = 1;

/**
 * Generează un pachet aleatoriu
 */
Packet generate_random_packet(void) {
    Packet pkt;
    
    /* Generăm IP-uri aleatorii în rețeaua 192.168.x.x */
    pkt.source_ip = make_ip(192, 168, rand() % 256, rand() % 254 + 1);
    pkt.dest_ip = make_ip(192, 168, rand() % 256, rand() % 254 + 1);
    
    /* Porturi aleatorii */
    pkt.source_port = 1024 + rand() % 64000;
    pkt.dest_port = rand() % 100 < 70 ? 80 : (rand() % 100 < 50 ? 443 : 22);
    
    /* Protocol aleatoriu */
    int r = rand() % 100;
    if (r < 60) pkt.protocol = PROTO_TCP;
    else if (r < 90) pkt.protocol = PROTO_UDP;
    else pkt.protocol = PROTO_ICMP;
    
    /* Dimensiune aleatorie */
    pkt.size = 64 + rand() % 1400;  /* MTU typical: 1500 */
    
    pkt.ttl = 64;
    pkt.sequence = packet_sequence++;
    
    return pkt;
}

/**
 * Simulează trafic de rețea
 */
void simulate_network_traffic(RouterBuffer *rb, int num_packets, 
                              int receive_rate, int forward_rate) {
    printf("\n" COLOR_YELLOW "═══ Simulare Trafic de Rețea ═══" COLOR_RESET "\n");
    printf("  Pachete de generat: %d\n", num_packets);
    printf("  Rata primire: %d pachete/ciclu\n", receive_rate);
    printf("  Rata transmitere: %d pachete/ciclu\n", forward_rate);
    printf("\n");
    
    int generated = 0;
    int cycle = 0;
    
    while (generated < num_packets || !router_is_empty(rb)) {
        cycle++;
        
        /* Primim pachete (dacă mai avem de generat) */
        int to_receive = (generated + receive_rate <= num_packets) ? 
                         receive_rate : (num_packets - generated);
        
        for (int i = 0; i < to_receive; i++) {
            Packet pkt = generate_random_packet();
            bool success = router_receive_packet(rb, pkt);
            
            if (!success && (generated % 100 == 0)) {
                printf(COLOR_RED "  [!] Pachet #%u pierdut (buffer overflow)\n" COLOR_RESET,
                       pkt.sequence);
            }
            generated++;
        }
        
        /* Transmitem pachete */
        Packet pkt;
        for (int i = 0; i < forward_rate; i++) {
            if (!router_forward_packet(rb, &pkt)) {
                break;  /* Buffer gol */
            }
        }
        
        /* Afișăm progres la fiecare 100 de pachete generate */
        if (generated % 100 == 0 && generated > 0) {
            printf("  Ciclu %d: generat %d/%d, buffer %d%%, pierdute %lu\n",
                   cycle, generated, num_packets,
                   router_buffer_usage(rb),
                   (unsigned long)rb->packets_dropped);
        }
    }
    
    printf("\n" COLOR_GREEN "✓ Simulare completă!\n" COLOR_RESET);
}

/* =============================================================================
 * PROGRAM PRINCIPAL
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║     " COLOR_CYAN "SOLUȚIE TEMA 2: Simulator Router de Rețea" COLOR_RESET "                  ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n\n");
    
    srand((unsigned int)time(NULL));
    
    RouterBuffer rb;
    router_init(&rb);
    
    printf(COLOR_GREEN "✓ Router inițializat (buffer size: %d pachete)\n" COLOR_RESET, BUFFER_SIZE);
    
    /* Test 1: Operații de bază */
    printf("\n" COLOR_YELLOW "═══ Test 1: Operații de bază ═══" COLOR_RESET "\n");
    
    Packet p1 = {
        .source_ip = make_ip(192, 168, 1, 100),
        .dest_ip = make_ip(192, 168, 1, 1),
        .source_port = 12345,
        .dest_port = 80,
        .protocol = PROTO_TCP,
        .size = 1024,
        .ttl = 64,
        .sequence = 1
    };
    
    printf("  Primim pachet:\n");
    print_packet(&p1);
    router_receive_packet(&rb, p1);
    
    Packet p2 = p1;
    p2.sequence = 2;
    p2.protocol = PROTO_UDP;
    p2.dest_port = 53;
    printf("  Primim pachet:\n");
    print_packet(&p2);
    router_receive_packet(&rb, p2);
    
    printf("\n  Buffer usage: %d%%\n", router_buffer_usage(&rb));
    
    Packet forwarded;
    printf("\n  Transmitem pachete:\n");
    while (router_forward_packet(&rb, &forwarded)) {
        printf("  → Forwarded:");
        print_packet(&forwarded);
    }
    
    router_print_stats(&rb);
    
    /* Test 2: Simulare trafic normal */
    printf("\n" COLOR_YELLOW "═══ Test 2: Trafic Normal (fără congestie) ═══" COLOR_RESET "\n");
    router_flush(&rb);
    simulate_network_traffic(&rb, 500, 5, 6);  /* Forward > Receive = no congestion */
    router_print_stats(&rb);
    
    /* Test 3: Simulare congestie */
    printf("\n" COLOR_YELLOW "═══ Test 3: Trafic cu Congestie ═══" COLOR_RESET "\n");
    router_flush(&rb);
    simulate_network_traffic(&rb, 500, 8, 3);  /* Receive > Forward = congestion */
    router_print_stats(&rb);
    
    /* Test 4: Simulare traffic burst */
    printf("\n" COLOR_YELLOW "═══ Test 4: Traffic Burst ═══" COLOR_RESET "\n");
    router_flush(&rb);
    
    printf("  Simulăm un burst de 150 pachete...\n");
    for (int i = 0; i < 150; i++) {
        Packet pkt = generate_random_packet();
        if (!router_receive_packet(&rb, pkt)) {
            printf("  → Overflow la pachetul %d!\n", i + 1);
        }
    }
    printf("  Buffer după burst: %d%%\n", router_buffer_usage(&rb));
    
    printf("  Procesăm bufferul...\n");
    Packet pkt;
    int processed = 0;
    while (router_forward_packet(&rb, &pkt)) {
        processed++;
    }
    printf("  Pachete procesate: %d\n", processed);
    
    router_print_stats(&rb);
    
    printf("\n" COLOR_GREEN "✓ Toate testele complete!\n" COLOR_RESET "\n");
    
    return 0;
}
