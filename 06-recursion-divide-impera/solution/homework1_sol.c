/**
 * =============================================================================
 * SOLUȚIE TEMA 1: Sistem de Gestionare a Clienților Bancari
 * =============================================================================
 *
 * DOAR PENTRU INSTRUCTORI - Nu distribuiți studenților!
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -o homework1_sol homework1_sol.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

/* =============================================================================
 * CONSTANTE
 * =============================================================================
 */

#define QUEUE_CAPACITY 50
#define MAX_NAME_LENGTH 50
#define NUM_PRIORITIES 3

#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_RED     "\033[31m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_MAGENTA "\033[35m"

/* =============================================================================
 * TIPURI DE DATE
 * =============================================================================
 */

typedef enum {
    CLIENT_REGULAR = 0,
    CLIENT_ELDERLY = 1,
    CLIENT_VIP = 2
} ClientType;

typedef enum {
    SERVICE_DEPOSIT,
    SERVICE_WITHDRAWAL,
    SERVICE_TRANSFER,
    SERVICE_OTHER
} ServiceType;

const char *client_type_names[] = {"REGULAR", "ELDERLY", "VIP"};
const char *service_names[] = {"DEPOSIT", "WITHDRAWAL", "TRANSFER", "OTHER"};

typedef struct {
    int ticket_number;
    char name[MAX_NAME_LENGTH];
    ClientType type;
    ServiceType service;
    time_t arrival_time;
} Client;

/* Circular buffer pentru fiecare tip de client */
typedef struct {
    Client clients[QUEUE_CAPACITY];
    int front;
    int rear;
    int count;
} ClientQueue;

/* Sistem complet cu 3 cozi (VIP, ELDERLY, REGULAR) */
typedef struct {
    ClientQueue queues[NUM_PRIORITIES];
    int total_clients;
    int total_served;
    double total_wait_time;
    int next_ticket;
} BankQueue;

/* =============================================================================
 * FUNCȚII PENTRU CLIENT QUEUE
 * =============================================================================
 */

void cqueue_init(ClientQueue *q) {
    q->front = 0;
    q->rear = 0;
    q->count = 0;
}

bool cqueue_is_empty(const ClientQueue *q) {
    return q->count == 0;
}

bool cqueue_is_full(const ClientQueue *q) {
    return q->count >= QUEUE_CAPACITY;
}

bool cqueue_enqueue(ClientQueue *q, Client client) {
    if (cqueue_is_full(q)) return false;
    
    q->clients[q->rear] = client;
    q->rear = (q->rear + 1) % QUEUE_CAPACITY;
    q->count++;
    return true;
}

bool cqueue_dequeue(ClientQueue *q, Client *result) {
    if (cqueue_is_empty(q)) return false;
    
    *result = q->clients[q->front];
    q->front = (q->front + 1) % QUEUE_CAPACITY;
    q->count--;
    return true;
}

/* =============================================================================
 * FUNCȚII PENTRU BANK QUEUE
 * =============================================================================
 */

void bank_queue_init(BankQueue *bq) {
    for (int i = 0; i < NUM_PRIORITIES; i++) {
        cqueue_init(&bq->queues[i]);
    }
    bq->total_clients = 0;
    bq->total_served = 0;
    bq->total_wait_time = 0.0;
    bq->next_ticket = 1;
}

int bank_add_client(BankQueue *bq, const char *name, ClientType type, ServiceType service) {
    Client client;
    client.ticket_number = bq->next_ticket++;
    strncpy(client.name, name, MAX_NAME_LENGTH - 1);
    client.name[MAX_NAME_LENGTH - 1] = '\0';
    client.type = type;
    client.service = service;
    client.arrival_time = time(NULL);
    
    /* Alegem coada în funcție de tip
     * Index: 0=REGULAR, 1=ELDERLY, 2=VIP
     * Procesare: de la 2 (VIP) la 0 (REGULAR)
     */
    int queue_idx = (int)type;
    
    if (!cqueue_enqueue(&bq->queues[queue_idx], client)) {
        return -1;  /* Queue plină */
    }
    
    bq->total_clients++;
    return client.ticket_number;
}

bool bank_call_next(BankQueue *bq, int window_id, Client *result) {
    /* Parcurgem de la prioritatea cea mai mare (VIP=2) la cea mai mică */
    for (int i = NUM_PRIORITIES - 1; i >= 0; i--) {
        if (!cqueue_is_empty(&bq->queues[i])) {
            if (cqueue_dequeue(&bq->queues[i], result)) {
                double wait_time = difftime(time(NULL), result->arrival_time);
                bq->total_wait_time += wait_time;
                bq->total_served++;
                
                printf(COLOR_GREEN "Ghișeul %d: Bon #%d - %s (%s, %s) - așteptat %.0f sec\n" COLOR_RESET,
                       window_id, result->ticket_number, result->name,
                       client_type_names[result->type], service_names[result->service],
                       wait_time);
                return true;
            }
        }
    }
    return false;
}

int bank_clients_waiting(const BankQueue *bq) {
    int total = 0;
    for (int i = 0; i < NUM_PRIORITIES; i++) {
        total += bq->queues[i].count;
    }
    return total;
}

double bank_average_wait_time(const BankQueue *bq) {
    if (bq->total_served == 0) return 0.0;
    return bq->total_wait_time / bq->total_served;
}

void bank_print_status(const BankQueue *bq) {
    printf("\n" COLOR_CYAN "═══ Status Bancă ═══" COLOR_RESET "\n");
    printf("  Clienți în așteptare: %d\n", bank_clients_waiting(bq));
    printf("    - VIP: %d\n", bq->queues[CLIENT_VIP].count);
    printf("    - ELDERLY: %d\n", bq->queues[CLIENT_ELDERLY].count);
    printf("    - REGULAR: %d\n", bq->queues[CLIENT_REGULAR].count);
    printf("  Clienți serviți: %d\n", bq->total_served);
    printf("  Timp mediu așteptare: %.1f secunde\n", bank_average_wait_time(bq));
}

/* =============================================================================
 * DEMONSTRAȚIE
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║     " COLOR_CYAN "SOLUȚIE TEMA 1: Sistem Bancar" COLOR_RESET "                               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n\n");
    
    BankQueue bq;
    bank_queue_init(&bq);
    
    printf(COLOR_YELLOW "→ Adăugăm clienți:\n" COLOR_RESET);
    
    int t1 = bank_add_client(&bq, "Ion Popescu", CLIENT_REGULAR, SERVICE_DEPOSIT);
    printf("  Bon #%d: Ion Popescu (REGULAR, DEPOSIT)\n", t1);
    
    int t2 = bank_add_client(&bq, "Maria VIP", CLIENT_VIP, SERVICE_TRANSFER);
    printf("  Bon #%d: Maria VIP (VIP, TRANSFER)\n", t2);
    
    int t3 = bank_add_client(&bq, "Bunica Elena", CLIENT_ELDERLY, SERVICE_WITHDRAWAL);
    printf("  Bon #%d: Bunica Elena (ELDERLY, WITHDRAWAL)\n", t3);
    
    int t4 = bank_add_client(&bq, "Andrei", CLIENT_REGULAR, SERVICE_OTHER);
    printf("  Bon #%d: Andrei (REGULAR, OTHER)\n", t4);
    
    int t5 = bank_add_client(&bq, "CEO Important", CLIENT_VIP, SERVICE_DEPOSIT);
    printf("  Bon #%d: CEO Important (VIP, DEPOSIT)\n", t5);
    
    bank_print_status(&bq);
    
    printf("\n" COLOR_YELLOW "→ Chemăm clienți la ghișee (ordinea ar trebui: VIP → ELDERLY → REGULAR):\n" COLOR_RESET);
    
    Client c;
    bank_call_next(&bq, 1, &c);  /* VIP - Maria */
    bank_call_next(&bq, 2, &c);  /* VIP - CEO */
    bank_call_next(&bq, 1, &c);  /* ELDERLY - Bunica */
    bank_call_next(&bq, 2, &c);  /* REGULAR - Ion */
    bank_call_next(&bq, 1, &c);  /* REGULAR - Andrei */
    
    bank_print_status(&bq);
    
    printf("\n" COLOR_GREEN "✓ Demonstrație completă!\n" COLOR_RESET "\n");
    
    return 0;
}
