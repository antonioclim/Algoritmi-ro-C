/**
 * =============================================================================
 * EXERCIȚIU 1: Circular Buffer pentru Procesare Comenzi Restaurant
 * =============================================================================
 *
 * OBIECTIV:
 *   Implementarea unui sistem de gestionare a comenzilor pentru un restaurant
 *   folosind un circular buffer. Sistemul trebuie să suporte adăugarea de
 *   comenzi noi și procesarea lor în ordinea sosirii (FIFO).
 *
 * CERINȚE:
 *   1. Completați structura Order cu câmpurile necesare
 *   2. Implementați funcțiile pentru circular buffer
 *   3. Adăugați gestionarea erorilor (buffer plin/gol)
 *   4. Implementați statistici (timp mediu de așteptare)
 *
 * EXEMPLU INPUT (din data/orders.txt):
 *   ADD Ion Popescu Pizza,Suc
 *   ADD Maria Ionescu Paste,Vin
 *   PROCESS
 *   STATUS
 *
 * OUTPUT AȘTEPTAT:
 *   [+] Comanda #1 adăugată pentru Ion Popescu
 *   [+] Comanda #2 adăugată pentru Maria Ionescu
 *   [✓] Procesăm comanda #1: Ion Popescu - Pizza,Suc
 *   [i] Comenzi în așteptare: 1
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -o exercise1 exercise1.c
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
 * TIPURI DE DATE
 * =============================================================================
 */

/**
 * TODO 1: Completați structura Order
 *
 * O comandă trebuie să conțină:
 * - id: identificator unic (int)
 * - client_name: numele clientului (șir de caractere)
 * - items: produsele comandate (șir de caractere)
 * - timestamp: momentul plasării comenzii (time_t)
 *
 * Hint: Folosiți char arrays cu lungimile definite mai sus
 */
typedef struct {
    int id;                              /* identificator comandă */
    char client_name[MAX_NAME_LENGTH];   /* TODO 1a: numele clientului */
    char items[MAX_ITEMS_LENGTH];        /* TODO 1b: produsele comandate */
    time_t timestamp;                    /* TODO 1c: momentul plasării */
} Order;

/**
 * TODO 2: Completați structura OrderQueue (Circular Buffer)
 *
 * Queue-ul trebuie să conțină:
 * - orders: array de comenzi (Order)
 * - front: indexul primului element
 * - rear: indexul următoarei poziții libere
 * - count: numărul de elemente curente
 * - total_processed: total comenzi procesate (pentru statistici)
 * - total_wait_time: timp total de așteptare (pentru statistici)
 *
 * Hint: Folosiți QUEUE_CAPACITY pentru dimensiunea array-ului
 */
typedef struct {
    Order orders[QUEUE_CAPACITY];
    int front;                           /* TODO 2a: index primul element */
    int rear;                            /* TODO 2b: index poziție liberă */
    int count;                           /* TODO 2c: număr elemente curente */
    int total_processed;                 /* TODO 2d: total comenzi procesate */
    double total_wait_time;              /* TODO 2e: timp total de așteptare */
} OrderQueue;

/* Variabilă globală pentru ID-ul comenzilor */
static int next_order_id = 1;

/* =============================================================================
 * FUNCȚII AUXILIARE
 * =============================================================================
 */

/**
 * Returnează timpul curent ca string formatat
 */
const char* get_time_string(time_t t) {
    static char buffer[20];
    struct tm *tm_info = localtime(&t);
    strftime(buffer, sizeof(buffer), "%H:%M:%S", tm_info);
    return buffer;
}

/* =============================================================================
 * FUNCȚII PENTRU ORDER QUEUE
 * =============================================================================
 */

/**
 * TODO 3: Implementați inițializarea queue-ului
 *
 * @param q Pointer la OrderQueue
 *
 * Pași:
 *   1. Setați front = 0
 *   2. Setați rear = 0
 *   3. Setați count = 0
 *   4. Inițializați statisticile (total_processed = 0, total_wait_time = 0)
 */
void queue_init(OrderQueue *q) {
    /* YOUR CODE HERE */
    
}

/**
 * TODO 4: Implementați verificarea dacă queue-ul este gol
 *
 * @param q Pointer la OrderQueue
 * @return true dacă queue-ul este gol, false altfel
 *
 * Hint: Queue-ul este gol când count == 0
 */
bool queue_is_empty(const OrderQueue *q) {
    /* YOUR CODE HERE */
    return true;  /* Replace this */
}

/**
 * TODO 5: Implementați verificarea dacă queue-ul este plin
 *
 * @param q Pointer la OrderQueue
 * @return true dacă queue-ul este plin, false altfel
 *
 * Hint: Queue-ul este plin când count >= QUEUE_CAPACITY
 */
bool queue_is_full(const OrderQueue *q) {
    /* YOUR CODE HERE */
    return false;  /* Replace this */
}

/**
 * TODO 6: Implementați adăugarea unei comenzi (enqueue)
 *
 * @param q Pointer la OrderQueue
 * @param client_name Numele clientului
 * @param items Produsele comandate
 * @return true dacă adăugarea a reușit, false dacă queue-ul este plin
 *
 * Pași:
 *   1. Verificați dacă queue-ul este plin - returnați false dacă da
 *   2. Creați o comandă nouă la poziția q->rear
 *   3. Setați id = next_order_id și incrementați next_order_id
 *   4. Copiați client_name și items folosind strncpy
 *   5. Setați timestamp = time(NULL)
 *   6. Avansați rear circular: rear = (rear + 1) % QUEUE_CAPACITY
 *   7. Incrementați count
 *   8. Returnați true
 *
 * Hint pentru strncpy: strncpy(dest, src, max_len); dest[max_len-1] = '\0';
 */
bool queue_add_order(OrderQueue *q, const char *client_name, const char *items) {
    /* YOUR CODE HERE */
    
    return false;  /* Replace this */
}

/**
 * TODO 7: Implementați procesarea unei comenzi (dequeue)
 *
 * @param q Pointer la OrderQueue
 * @param result Pointer pentru stocarea comenzii procesate
 * @return true dacă procesarea a reușit, false dacă queue-ul este gol
 *
 * Pași:
 *   1. Verificați dacă queue-ul este gol - returnați false dacă da
 *   2. Copiați comanda de la poziția front în result
 *   3. Calculați timpul de așteptare: time(NULL) - result->timestamp
 *   4. Adăugați la total_wait_time
 *   5. Incrementați total_processed
 *   6. Avansați front circular: front = (front + 1) % QUEUE_CAPACITY
 *   7. Decrementați count
 *   8. Returnați true
 */
bool queue_process_order(OrderQueue *q, Order *result) {
    /* YOUR CODE HERE */
    
    return false;  /* Replace this */
}

/**
 * TODO 8: Implementați funcția peek (vizualizare fără eliminare)
 *
 * @param q Pointer la OrderQueue
 * @param result Pointer pentru stocarea comenzii
 * @return true dacă există o comandă, false dacă queue-ul este gol
 */
bool queue_peek_next(const OrderQueue *q, Order *result) {
    /* YOUR CODE HERE */
    
    return false;  /* Replace this */
}

/**
 * Returnează numărul de comenzi în așteptare
 */
int queue_orders_waiting(const OrderQueue *q) {
    return q->count;
}

/**
 * TODO 9: Implementați calculul timpului mediu de așteptare
 *
 * @param q Pointer la OrderQueue
 * @return Timpul mediu de așteptare în secunde, sau 0 dacă nu au fost procesate comenzi
 *
 * Formula: total_wait_time / total_processed
 */
double queue_average_wait_time(const OrderQueue *q) {
    /* YOUR CODE HERE */
    
    return 0.0;  /* Replace this */
}

/**
 * TODO 10: Implementați afișarea tuturor comenzilor din queue
 *
 * @param q Pointer la OrderQueue
 *
 * Parcurgeți circular de la front și afișați fiecare comandă
 * Format: "[i] #id: client_name - items (adăugată la HH:MM:SS)"
 */
void queue_print_all(const OrderQueue *q) {
    printf("\n" COLOR_CYAN "═══ Comenzi în așteptare ═══" COLOR_RESET "\n");
    
    if (queue_is_empty(q)) {
        printf("  (nicio comandă în așteptare)\n");
        return;
    }
    
    /* YOUR CODE HERE */
    /* Hint: int idx = q->front;
     *       for (int i = 0; i < q->count; i++) {
     *           // afișați q->orders[idx]
     *           idx = (idx + 1) % QUEUE_CAPACITY;
     *       }
     */
    
}

/**
 * Afișează statisticile
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

/**
 * Procesează o linie de comandă
 */
void process_command(OrderQueue *q, const char *line) {
    char command[20];
    char client_name[MAX_NAME_LENGTH];
    char items[MAX_ITEMS_LENGTH];
    
    /* Parsăm comanda */
    if (sscanf(line, "%19s", command) != 1) {
        return;
    }
    
    if (strcmp(command, "ADD") == 0) {
        /* Format: ADD client_name items */
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
    printf("║     " COLOR_CYAN "EXERCIȚIU 1: Sistem Comenzi Restaurant" COLOR_RESET "                     ║\n");
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
    
    /* Citim din fișier dacă este specificat */
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp != NULL) {
            printf(COLOR_YELLOW "→ Citim comenzi din: %s\n\n" COLOR_RESET, argv[1]);
            char line[300];
            while (fgets(line, sizeof(line), fp) != NULL) {
                /* Eliminăm newline */
                line[strcspn(line, "\n")] = '\0';
                if (strlen(line) > 0) {
                    printf(COLOR_CYAN "> %s\n" COLOR_RESET, line);
                    process_command(&queue, line);
                }
            }
            fclose(fp);
            printf("\n");
        }
    }
    
    /* Mod interactiv */
    printf(COLOR_YELLOW "→ Mod interactiv (tastați EXIT pentru a ieși):\n" COLOR_RESET);
    char line[300];
    while (1) {
        printf("\n> ");
        if (fgets(line, sizeof(line), stdin) == NULL) {
            break;
        }
        
        /* Eliminăm newline */
        line[strcspn(line, "\n")] = '\0';
        
        if (strcmp(line, "EXIT") == 0 || strcmp(line, "exit") == 0) {
            break;
        }
        
        if (strlen(line) > 0) {
            process_command(&queue, line);
        }
    }
    
    /* Afișăm statisticile finale */
    queue_print_stats(&queue);
    
    printf("\n" COLOR_GREEN "✓ Program terminat.\n" COLOR_RESET "\n");
    
    return 0;
}

/* =============================================================================
 * PROVOCĂRI BONUS (Opțional)
 * =============================================================================
 *
 * 1. Adăugați un câmp "priority" la Order și modificați procesarea pentru
 *    a ține cont de prioritate (comenzi VIP procesate primele).
 *
 * 2. Implementați persistența - salvați queue-ul într-un fișier la EXIT
 *    și încărcați-l la pornire.
 *
 * 3. Adăugați un timer care simulează trecerea timpului și afișează
 *    avertismente pentru comenzile care așteaptă prea mult.
 *
 * 4. Implementați funcția CANCEL <id> care anulează o comandă specifică.
 *
 * 5. Adăugați suport pentru multiple cozi (ex: mâncare, băuturi, deserturi).
 *
 * =============================================================================
 */
