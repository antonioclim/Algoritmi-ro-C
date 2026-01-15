/**
 * =============================================================================
 * SOLUȚIE EXERCIȚIU 1: Circular Buffer pentru Procesare Comenzi Restaurant
 * =============================================================================
 *
 * Aceasta este soluția completă pentru exercițiul 1.
 * DOAR PENTRU INSTRUCTORI - Nu distribuiți studenților!
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -o exercise1_sol exercise1_sol.c
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

#define MAX_NAME_LENGTH 50
#define MAX_ITEMS_LENGTH 200
#define QUEUE_CAPACITY 10

/* Culori pentru terminal */
#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_RED     "\033[31m"
#define COLOR_CYAN    "\033[36m"

/* =============================================================================
 * TIPURI DE DATE - SOLUȚIE COMPLETĂ
 * =============================================================================
 */

/**
 * Structura Order - SOLUȚIE
 */
typedef struct {
    int id;
    char client_name[MAX_NAME_LENGTH];
    char items[MAX_ITEMS_LENGTH];
    time_t timestamp;
} Order;

/**
 * Structura OrderQueue (Circular Buffer) - SOLUȚIE
 */
typedef struct {
    Order orders[QUEUE_CAPACITY];
    int front;
    int rear;
    int count;
    int total_processed;
    double total_wait_time;
} OrderQueue;

/* Variabilă globală pentru ID-ul comenzilor */
static int next_order_id = 1;

/* =============================================================================
 * FUNCȚII AUXILIARE
 * =============================================================================
 */

const char* get_time_string(time_t t) {
    static char buffer[20];
    struct tm *tm_info = localtime(&t);
    strftime(buffer, sizeof(buffer), "%H:%M:%S", tm_info);
    return buffer;
}

/* =============================================================================
 * FUNCȚII PENTRU ORDER QUEUE - SOLUȚII
 * =============================================================================
 */

/**
 * Inițializare queue - SOLUȚIE
 */
void queue_init(OrderQueue *q) {
    q->front = 0;
    q->rear = 0;
    q->count = 0;
    q->total_processed = 0;
    q->total_wait_time = 0.0;
}

/**
 * isEmpty - SOLUȚIE
 */
bool queue_is_empty(const OrderQueue *q) {
    return q->count == 0;
}

/**
 * isFull - SOLUȚIE
 */
bool queue_is_full(const OrderQueue *q) {
    return q->count >= QUEUE_CAPACITY;
}

/**
 * Adăugare comandă (enqueue) - SOLUȚIE
 */
bool queue_add_order(OrderQueue *q, const char *client_name, const char *items) {
    /* Verificăm dacă queue-ul este plin */
    if (queue_is_full(q)) {
        return false;
    }
    
    /* Creăm comanda nouă la poziția rear */
    Order *order = &q->orders[q->rear];
    order->id = next_order_id++;
    
    /* Copiem datele (cu protecție la buffer overflow) */
    strncpy(order->client_name, client_name, MAX_NAME_LENGTH - 1);
    order->client_name[MAX_NAME_LENGTH - 1] = '\0';
    
    strncpy(order->items, items, MAX_ITEMS_LENGTH - 1);
    order->items[MAX_ITEMS_LENGTH - 1] = '\0';
    
    order->timestamp = time(NULL);
    
    /* Avansăm rear circular */
    q->rear = (q->rear + 1) % QUEUE_CAPACITY;
    q->count++;
    
    return true;
}

/**
 * Procesare comandă (dequeue) - SOLUȚIE
 */
bool queue_process_order(OrderQueue *q, Order *result) {
    /* Verificăm dacă queue-ul este gol */
    if (queue_is_empty(q)) {
        return false;
    }
    
    /* Copiăm comanda */
    *result = q->orders[q->front];
    
    /* Calculăm timpul de așteptare */
    double wait_time = difftime(time(NULL), result->timestamp);
    q->total_wait_time += wait_time;
    q->total_processed++;
    
    /* Avansăm front circular */
    q->front = (q->front + 1) % QUEUE_CAPACITY;
    q->count--;
    
    return true;
}

/**
 * Peek - SOLUȚIE
 */
bool queue_peek_next(const OrderQueue *q, Order *result) {
    if (queue_is_empty(q)) {
        return false;
    }
    *result = q->orders[q->front];
    return true;
}

/**
 * Număr comenzi în așteptare
 */
int queue_orders_waiting(const OrderQueue *q) {
    return q->count;
}

/**
 * Timp mediu de așteptare - SOLUȚIE
 */
double queue_average_wait_time(const OrderQueue *q) {
    if (q->total_processed == 0) {
        return 0.0;
    }
    return q->total_wait_time / q->total_processed;
}

/**
 * Afișare toate comenzile - SOLUȚIE
 */
void queue_print_all(const OrderQueue *q) {
    printf("\n" COLOR_CYAN "═══ Comenzi în așteptare ═══" COLOR_RESET "\n");
    
    if (queue_is_empty(q)) {
        printf("  (nicio comandă în așteptare)\n");
        return;
    }
    
    int idx = q->front;
    for (int i = 0; i < q->count; i++) {
        const Order *order = &q->orders[idx];
        printf("  [%d] #%d: %s - %s (adăugată la %s)\n",
               i + 1, order->id, order->client_name, 
               order->items, get_time_string(order->timestamp));
        idx = (idx + 1) % QUEUE_CAPACITY;
    }
}

/**
 * Afișare statistici
 */
void queue_print_stats(const OrderQueue *q) {
    printf("\n" COLOR_CYAN "═══ Statistici ═══" COLOR_RESET "\n");
    printf("  Comenzi în așteptare: %d\n", queue_orders_waiting(q));
    printf("  Comenzi procesate: %d\n", q->total_processed);
    printf("  Timp mediu așteptare: %.2f secunde\n", queue_average_wait_time(q));
}

/* =============================================================================
 * PROCESARE COMENZI
 * =============================================================================
 */

void process_command(OrderQueue *q, const char *line) {
    char command[20];
    char client_name[MAX_NAME_LENGTH];
    char items[MAX_ITEMS_LENGTH];
    
    if (sscanf(line, "%19s", command) != 1) {
        return;
    }
    
    if (strcmp(command, "ADD") == 0) {
        if (sscanf(line, "%*s %49s %199[^\n]", client_name, items) == 2) {
            if (queue_add_order(q, client_name, items)) {
                printf(COLOR_GREEN "[+] Comanda #%d adăugată pentru %s\n" COLOR_RESET,
                       next_order_id - 1, client_name);
            } else {
                printf(COLOR_RED "[!] Eroare: Queue plin! Comanda respinsă.\n" COLOR_RESET);
            }
        }
    }
    else if (strcmp(command, "PROCESS") == 0) {
        Order order;
        if (queue_process_order(q, &order)) {
            printf(COLOR_GREEN "[✓] Procesăm comanda #%d: %s - %s\n" COLOR_RESET,
                   order.id, order.client_name, order.items);
        } else {
            printf(COLOR_YELLOW "[i] Nicio comandă de procesat.\n" COLOR_RESET);
        }
    }
    else if (strcmp(command, "PEEK") == 0) {
        Order order;
        if (queue_peek_next(q, &order)) {
            printf(COLOR_CYAN "[i] Următoarea comandă: #%d - %s\n" COLOR_RESET,
                   order.id, order.client_name);
        } else {
            printf(COLOR_YELLOW "[i] Nicio comandă în așteptare.\n" COLOR_RESET);
        }
    }
    else if (strcmp(command, "STATUS") == 0) {
        printf(COLOR_CYAN "[i] Comenzi în așteptare: %d\n" COLOR_RESET,
               queue_orders_waiting(q));
    }
    else if (strcmp(command, "LIST") == 0) {
        queue_print_all(q);
    }
    else if (strcmp(command, "STATS") == 0) {
        queue_print_stats(q);
    }
}

/* =============================================================================
 * PROGRAM PRINCIPAL
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║     " COLOR_CYAN "SOLUȚIE EXERCIȚIU 1: Sistem Comenzi Restaurant" COLOR_RESET "             ║\n");
    printf("║     Circular Buffer Implementation                                ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n\n");
    
    OrderQueue queue;
    queue_init(&queue);
    
    printf(COLOR_GREEN "✓ Sistem inițializat (capacitate: %d comenzi)\n" COLOR_RESET, QUEUE_CAPACITY);
    printf("\nComenzi disponibile:\n");
    printf("  ADD <nume> <produse>  - Adaugă o comandă nouă\n");
    printf("  PROCESS               - Procesează următoarea comandă\n");
    printf("  PEEK                  - Vizualizează următoarea comandă\n");
    printf("  STATUS                - Afișează numărul de comenzi\n");
    printf("  LIST                  - Listează toate comenzile\n");
    printf("  STATS                 - Afișează statistici\n");
    printf("  EXIT                  - Ieșire\n\n");
    
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp != NULL) {
            printf(COLOR_YELLOW "→ Citim comenzi din: %s\n\n" COLOR_RESET, argv[1]);
            char line[300];
            while (fgets(line, sizeof(line), fp) != NULL) {
                line[strcspn(line, "\n")] = '\0';
                if (strlen(line) > 0 && line[0] != '#') {
                    printf(COLOR_CYAN "> %s\n" COLOR_RESET, line);
                    process_command(&queue, line);
                }
            }
            fclose(fp);
            printf("\n");
        }
    }
    
    printf(COLOR_YELLOW "→ Mod interactiv (tastați EXIT pentru a ieși):\n" COLOR_RESET);
    char line[300];
    while (1) {
        printf("\n> ");
        if (fgets(line, sizeof(line), stdin) == NULL) {
            break;
        }
        
        line[strcspn(line, "\n")] = '\0';
        
        if (strcmp(line, "EXIT") == 0 || strcmp(line, "exit") == 0) {
            break;
        }
        
        if (strlen(line) > 0) {
            process_command(&queue, line);
        }
    }
    
    queue_print_stats(&queue);
    
    printf("\n" COLOR_GREEN "✓ Program terminat.\n" COLOR_RESET "\n");
    
    return 0;
}
