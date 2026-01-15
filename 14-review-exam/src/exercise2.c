/**
 * =============================================================================
 * EXERCIȚIUL 2: Analizator Rețele Sociale
 * Săptămâna 14 - Recapitulare Avansată și Optimizare
 * =============================================================================
 *
 * OBIECTIV:
 *   Implementarea unui sistem de analiză a rețelelor sociale care folosește
 *   concepte avansate de grafuri și algoritmi:
 *   - Graf reprezentat ca listă de adiacență
 *   - BFS pentru calculul distanțelor (grade de separare)
 *   - DFS pentru detectarea comunităților conectate
 *   - Hash table pentru maparea nume -> ID utilizator
 *   - Heap pentru identificarea utilizatorilor influenți
 *   - Function pointers pentru diverse criterii de sortare
 *
 * CERINȚE:
 *   1. Implementați graful ca listă de adiacență
 *   2. Implementați BFS pentru "grade de separare"
 *   3. Implementați DFS pentru componente conexe
 *   4. Găsiți utilizatorii cei mai influenți (după grad)
 *   5. Analizați structura rețelei
 *
 * EXEMPLU INPUT (social_network.csv):
 *   Alice,Bob
 *   Alice,Charlie
 *   Bob,Diana
 *   Charlie,Diana
 *   Eve,Frank
 *
 * EXEMPLU OUTPUT:
 *   === ANALIZĂ REȚEA SOCIALĂ ===
 *   Utilizatori: 6
 *   Conexiuni: 5
 *   Comunități detectate: 2
 *   Grade de separare Alice -> Diana: 2
 *   Top influenceri: Diana (3 conexiuni)
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -o exercise2 exercise2.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

/* =============================================================================
 * CONSTANTE ȘI MACRO-URI
 * =============================================================================
 */

#define MAX_USERS 1000
#define MAX_NAME_LEN 64
#define HASH_SIZE 127           /* Număr prim pentru hash table */
#define MAX_LINE 256
#define INFINITY_DIST INT_MAX

/* Coduri culori ANSI */
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_RESET   "\x1b[0m"

/* =============================================================================
 * STRUCTURI DE DATE
 * =============================================================================
 */

/**
 * Nod în lista de adiacență (reprezintă o muchie/conexiune)
 */
typedef struct AdjNode {
    int user_id;                /* ID-ul vecinului */
    struct AdjNode *next;       /* Următoarea conexiune */
} AdjNode;

/**
 * Informații despre un utilizator
 */
typedef struct {
    int id;                     /* ID numeric (index în graf) */
    char name[MAX_NAME_LEN];    /* Nume utilizator */
    int degree;                 /* Număr de conexiuni */
    int community;              /* ID comunitate (după DFS) */
    AdjNode *neighbors;         /* Lista de adiacență */
} User;

/**
 * TODO 1: Completați structura pentru hash table (mapare nume -> ID)
 *
 * Hash table-ul permite găsirea rapidă a ID-ului unui utilizator
 * după nume. Structura trebuie să conțină:
 * - Array de pointeri la utilizatori (pentru chaining)
 * - Dimensiunea (HASH_SIZE)
 *
 * Hint: Folosiți User* pentru elemente, cu un câmp suplimentar
 *       pentru next în lanț (puteți crea o structură auxiliară)
 */
typedef struct HashEntry {
    char name[MAX_NAME_LEN];
    int user_id;
    struct HashEntry *next;
} HashEntry;

typedef struct {
    /* YOUR CODE HERE - declarați array-ul de bucket-uri */
    HashEntry *buckets[HASH_SIZE];
} NameHashTable;

/**
 * Structura principală pentru graf
 */
typedef struct {
    User users[MAX_USERS];      /* Array de utilizatori */
    int user_count;             /* Număr curent de utilizatori */
    int edge_count;             /* Număr de muchii (conexiuni) */
    NameHashTable *name_map;    /* Hash table pentru căutare după nume */
} SocialGraph;

/**
 * TODO 2: Completați structura pentru coadă (folosită în BFS)
 *
 * Coada trebuie să suporte operații circulare:
 * - Array de ID-uri de utilizatori
 * - Indici front și rear
 * - Capacitate maximă
 * - Dimensiune curentă
 */
typedef struct {
    /* YOUR CODE HERE */
    int *data;
    int front;
    int rear;
    int capacity;
    int size;
} Queue;

/**
 * Structura pentru rezultatele analizei
 */
typedef struct {
    int communities;            /* Număr de comunități */
    int max_degree;             /* Gradul maxim */
    int most_influential;       /* ID-ul celui mai influent */
    double avg_degree;          /* Gradul mediu */
    double density;             /* Densitatea grafului */
} NetworkStats;

/* =============================================================================
 * DECLARAȚII FUNCȚII (PROTOTIPURI)
 * =============================================================================
 */

/* Hash Table Functions */
unsigned int hash_string(const char *str);
NameHashTable *create_name_table(void);
bool insert_name(NameHashTable *table, const char *name, int user_id);
int find_user_id(NameHashTable *table, const char *name);
void free_name_table(NameHashTable *table);

/* Graph Functions */
SocialGraph *create_graph(void);
int add_user(SocialGraph *graph, const char *name);
bool add_connection(SocialGraph *graph, int user1_id, int user2_id);
bool add_connection_by_name(SocialGraph *graph, const char *name1, const char *name2);
int get_or_create_user(SocialGraph *graph, const char *name);
void print_user(const SocialGraph *graph, int user_id);
void print_graph(const SocialGraph *graph);
void free_graph(SocialGraph *graph);

/* Queue Functions */
Queue *create_queue(int capacity);
bool enqueue(Queue *q, int value);
int dequeue(Queue *q);
bool is_queue_empty(Queue *q);
void free_queue(Queue *q);

/* BFS & DFS Functions */
int *bfs_distances(SocialGraph *graph, int start_id);
int degrees_of_separation(SocialGraph *graph, int user1, int user2);
void dfs_community(SocialGraph *graph, int user_id, bool *visited, int community_id);
int find_communities(SocialGraph *graph);

/* Analysis Functions */
NetworkStats analyze_network(SocialGraph *graph);
void find_influencers(SocialGraph *graph, int top_n);
int *find_shortest_path(SocialGraph *graph, int start, int end, int *path_length);

/* Utility Functions */
int load_network_from_file(SocialGraph *graph, const char *filename);
void print_separator(const char *title);

/* =============================================================================
 * IMPLEMENTARE: HASH TABLE PENTRU NUME
 * =============================================================================
 */

/**
 * TODO 3: Implementați funcția hash pentru string-uri
 *
 * Folosiți algoritmul djb2:
 *   hash = 5381
 *   for each char c in string:
 *       hash = hash * 33 + c
 *   return hash % HASH_SIZE
 *
 * @param str String-ul de hashuit
 * @return Valoarea hash (0 la HASH_SIZE-1)
 */
unsigned int hash_string(const char *str) {
    /* YOUR CODE HERE */
    
    return 0;
}

/**
 * Creează un hash table pentru maparea nume -> ID
 */
NameHashTable *create_name_table(void) {
    NameHashTable *table = malloc(sizeof(NameHashTable));
    if (!table) return NULL;
    
    for (int i = 0; i < HASH_SIZE; i++) {
        table->buckets[i] = NULL;
    }
    
    return table;
}

/**
 * TODO 4: Implementați inserarea în hash table
 *
 * Pași:
 *   1. Calculați hash-ul numelui
 *   2. Verificați dacă numele există deja
 *   3. Alocați o nouă intrare HashEntry
 *   4. Copiați numele și ID-ul
 *   5. Inserați la începutul bucket-ului
 *
 * @param table Hash table-ul
 * @param name Numele utilizatorului
 * @param user_id ID-ul utilizatorului
 * @return true dacă inserarea a reușit
 */
bool insert_name(NameHashTable *table, const char *name, int user_id) {
    if (!table || !name) return false;
    
    /* YOUR CODE HERE */
    
    return false;
}

/**
 * TODO 5: Implementați căutarea în hash table
 *
 * @param table Hash table-ul
 * @param name Numele de căutat
 * @return ID-ul utilizatorului sau -1 dacă nu există
 */
int find_user_id(NameHashTable *table, const char *name) {
    if (!table || !name) return -1;
    
    /* YOUR CODE HERE */
    
    return -1;
}

/**
 * Eliberează memoria pentru hash table
 */
void free_name_table(NameHashTable *table) {
    if (!table) return;
    
    for (int i = 0; i < HASH_SIZE; i++) {
        HashEntry *entry = table->buckets[i];
        while (entry) {
            HashEntry *temp = entry;
            entry = entry->next;
            free(temp);
        }
    }
    free(table);
}

/* =============================================================================
 * IMPLEMENTARE: COADĂ PENTRU BFS
 * =============================================================================
 */

/**
 * TODO 6: Implementați crearea cozii
 *
 * @param capacity Capacitatea maximă
 * @return Pointer la coadă sau NULL
 */
Queue *create_queue(int capacity) {
    if (capacity <= 0) return NULL;
    
    /* YOUR CODE HERE */
    
    return NULL;
}

/**
 * TODO 7: Implementați operația enqueue (adăugare la coadă)
 *
 * @param q Coada
 * @param value Valoarea de adăugat
 * @return true dacă operația a reușit
 */
bool enqueue(Queue *q, int value) {
    if (!q) return false;
    
    /* YOUR CODE HERE */
    
    return false;
}

/**
 * TODO 8: Implementați operația dequeue (extragere din coadă)
 *
 * @param q Coada
 * @return Valoarea extrasă sau -1 dacă coada e goală
 */
int dequeue(Queue *q) {
    if (!q) return -1;
    
    /* YOUR CODE HERE */
    
    return -1;
}

/**
 * Verifică dacă coada este goală
 */
bool is_queue_empty(Queue *q) {
    return !q || q->size == 0;
}

/**
 * Eliberează memoria pentru coadă
 */
void free_queue(Queue *q) {
    if (!q) return;
    free(q->data);
    free(q);
}

/* =============================================================================
 * IMPLEMENTARE: GRAF - FUNCȚII DE BAZĂ
 * =============================================================================
 */

/**
 * Creează un graf gol
 */
SocialGraph *create_graph(void) {
    SocialGraph *graph = malloc(sizeof(SocialGraph));
    if (!graph) return NULL;
    
    graph->user_count = 0;
    graph->edge_count = 0;
    graph->name_map = create_name_table();
    
    if (!graph->name_map) {
        free(graph);
        return NULL;
    }
    
    /* Inițializare utilizatori */
    for (int i = 0; i < MAX_USERS; i++) {
        graph->users[i].id = -1;
        graph->users[i].name[0] = '\0';
        graph->users[i].degree = 0;
        graph->users[i].community = -1;
        graph->users[i].neighbors = NULL;
    }
    
    return graph;
}

/**
 * TODO 9: Implementați adăugarea unui utilizator nou
 *
 * Pași:
 *   1. Verificați dacă nu s-a atins limita MAX_USERS
 *   2. Verificați dacă utilizatorul nu există deja
 *   3. Completați datele în array-ul users
 *   4. Adăugați în hash table
 *   5. Incrementați user_count
 *   6. Returnați ID-ul noului utilizator
 *
 * @param graph Graful
 * @param name Numele utilizatorului
 * @return ID-ul noului utilizator sau -1 la eroare
 */
int add_user(SocialGraph *graph, const char *name) {
    if (!graph || !name) return -1;
    
    /* YOUR CODE HERE */
    
    return -1;
}

/**
 * Obține ID-ul unui utilizator sau îl creează dacă nu există
 */
int get_or_create_user(SocialGraph *graph, const char *name) {
    int id = find_user_id(graph->name_map, name);
    if (id >= 0) return id;
    return add_user(graph, name);
}

/**
 * TODO 10: Implementați adăugarea unei conexiuni (muchie)
 *
 * Graful este neorientat, deci adăugăm în ambele liste de adiacență.
 *
 * Pași:
 *   1. Verificați că ambii utilizatori există (ID valid)
 *   2. Verificați că nu există deja conexiunea
 *   3. Alocați un AdjNode pentru user1 -> user2
 *   4. Adăugați la începutul listei lui user1
 *   5. Alocați un AdjNode pentru user2 -> user1
 *   6. Adăugați la începutul listei lui user2
 *   7. Incrementați degree pentru ambii și edge_count
 *
 * @param graph Graful
 * @param user1_id ID-ul primului utilizator
 * @param user2_id ID-ul celui de-al doilea utilizator
 * @return true dacă conexiunea a fost adăugată
 */
bool add_connection(SocialGraph *graph, int user1_id, int user2_id) {
    if (!graph) return false;
    if (user1_id < 0 || user1_id >= graph->user_count) return false;
    if (user2_id < 0 || user2_id >= graph->user_count) return false;
    if (user1_id == user2_id) return false;  /* Nu permitem self-loops */
    
    /* YOUR CODE HERE */
    
    return false;
}

/**
 * Adaugă conexiune folosind numele utilizatorilor
 */
bool add_connection_by_name(SocialGraph *graph, const char *name1, const char *name2) {
    if (!graph || !name1 || !name2) return false;
    
    int id1 = get_or_create_user(graph, name1);
    int id2 = get_or_create_user(graph, name2);
    
    return add_connection(graph, id1, id2);
}

/**
 * Afișează informații despre un utilizator
 */
void print_user(const SocialGraph *graph, int user_id) {
    if (!graph || user_id < 0 || user_id >= graph->user_count) return;
    
    const User *user = &graph->users[user_id];
    
    printf(COLOR_CYAN "  [%d] %s" COLOR_RESET, user->id, user->name);
    printf(" (grad: %d, comunitate: %d)\n", user->degree, user->community);
    printf("      Conexiuni: ");
    
    AdjNode *neighbor = user->neighbors;
    if (!neighbor) {
        printf("(niciuna)");
    }
    while (neighbor) {
        printf("%s", graph->users[neighbor->user_id].name);
        neighbor = neighbor->next;
        if (neighbor) printf(", ");
    }
    printf("\n");
}

/**
 * Afișează întregul graf
 */
void print_graph(const SocialGraph *graph) {
    if (!graph) return;
    
    printf(COLOR_BLUE "╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║  STRUCTURA GRAFULUI                                           ║\n");
    printf("╠═══════════════════════════════════════════════════════════════╣\n" COLOR_RESET);
    printf("  Utilizatori: %d\n", graph->user_count);
    printf("  Conexiuni: %d\n", graph->edge_count);
    printf(COLOR_BLUE "╠═══════════════════════════════════════════════════════════════╣\n" COLOR_RESET);
    
    for (int i = 0; i < graph->user_count; i++) {
        print_user(graph, i);
    }
    
    printf(COLOR_BLUE "╚═══════════════════════════════════════════════════════════════╝\n" COLOR_RESET);
}

/**
 * Eliberează memoria pentru graf
 */
void free_graph(SocialGraph *graph) {
    if (!graph) return;
    
    /* Eliberare liste de adiacență */
    for (int i = 0; i < graph->user_count; i++) {
        AdjNode *node = graph->users[i].neighbors;
        while (node) {
            AdjNode *temp = node;
            node = node->next;
            free(temp);
        }
    }
    
    free_name_table(graph->name_map);
    free(graph);
}

/* =============================================================================
 * IMPLEMENTARE: BFS - GRADE DE SEPARARE
 * =============================================================================
 */

/**
 * TODO 11: Implementați BFS pentru calculul distanțelor
 *
 * BFS calculează distanța minimă de la un nod start la toate celelalte.
 *
 * Algoritm:
 *   1. Alocați și inițializați array-ul de distanțe cu INFINITY_DIST
 *   2. Creați o coadă și adăugați nodul start
 *   3. Setați distanța[start] = 0
 *   4. Cât timp coada nu este goală:
 *      a. Extrageți nodul curent
 *      b. Pentru fiecare vecin nevizitat:
 *         - Setați distanța[vecin] = distanța[curent] + 1
 *         - Adăugați vecinul în coadă
 *   5. Eliberați coada și returnați array-ul de distanțe
 *
 * @param graph Graful
 * @param start_id ID-ul nodului de start
 * @return Array cu distanțele (apelantul trebuie să elibereze!)
 */
int *bfs_distances(SocialGraph *graph, int start_id) {
    if (!graph || start_id < 0 || start_id >= graph->user_count) return NULL;
    
    /* Alocăm array-ul de distanțe */
    int *distances = malloc(graph->user_count * sizeof(int));
    if (!distances) return NULL;
    
    /* Inițializăm toate distanțele la infinit */
    for (int i = 0; i < graph->user_count; i++) {
        distances[i] = INFINITY_DIST;
    }
    
    /* YOUR CODE HERE - implementați BFS */
    
    return distances;
}

/**
 * Calculează gradele de separare între doi utilizatori
 */
int degrees_of_separation(SocialGraph *graph, int user1, int user2) {
    if (!graph) return -1;
    if (user1 < 0 || user1 >= graph->user_count) return -1;
    if (user2 < 0 || user2 >= graph->user_count) return -1;
    
    int *distances = bfs_distances(graph, user1);
    if (!distances) return -1;
    
    int result = distances[user2];
    free(distances);
    
    return (result == INFINITY_DIST) ? -1 : result;
}

/* =============================================================================
 * IMPLEMENTARE: DFS - DETECTARE COMUNITĂȚI
 * =============================================================================
 */

/**
 * TODO 12: Implementați DFS pentru marcarea unei comunități
 *
 * DFS recursiv care marchează toți utilizatorii dintr-o componentă conexă.
 *
 * Algoritm:
 *   1. Marcați nodul curent ca vizitat
 *   2. Setați community = community_id pentru utilizator
 *   3. Pentru fiecare vecin nevizitat:
 *      - Apelați recursiv dfs_community
 *
 * @param graph Graful
 * @param user_id ID-ul utilizatorului curent
 * @param visited Array de booleeni pentru noduri vizitate
 * @param community_id ID-ul comunității curente
 */
void dfs_community(SocialGraph *graph, int user_id, bool *visited, int community_id) {
    if (!graph || !visited) return;
    if (user_id < 0 || user_id >= graph->user_count) return;
    if (visited[user_id]) return;
    
    /* YOUR CODE HERE */
    
}

/**
 * TODO 13: Implementați găsirea tuturor comunităților
 *
 * Parcurgeți toți utilizatorii și pentru fiecare nod nevizitat,
 * lansați un DFS care marchează o nouă comunitate.
 *
 * @param graph Graful
 * @return Numărul de comunități găsite
 */
int find_communities(SocialGraph *graph) {
    if (!graph || graph->user_count == 0) return 0;
    
    /* YOUR CODE HERE */
    
    return 0;
}

/* =============================================================================
 * IMPLEMENTARE: ANALIZĂ REȚEA
 * =============================================================================
 */

/**
 * TODO 14: Implementați analiza completă a rețelei
 *
 * Calculați:
 *   - Numărul de comunități (apelați find_communities)
 *   - Gradul maxim (parcurgeți utilizatorii)
 *   - Utilizatorul cel mai influent (gradul maxim)
 *   - Gradul mediu = 2 * edge_count / user_count
 *   - Densitatea = 2 * edge_count / (user_count * (user_count - 1))
 *
 * @param graph Graful
 * @return Structura cu statisticile
 */
NetworkStats analyze_network(SocialGraph *graph) {
    NetworkStats stats = {0, 0, -1, 0.0, 0.0};
    
    if (!graph || graph->user_count == 0) return stats;
    
    /* YOUR CODE HERE */
    
    return stats;
}

/**
 * TODO 15: Implementați găsirea celor mai influenți utilizatori
 *
 * Sortați utilizatorii după grad (descrescător) și afișați top N.
 *
 * Hint: Puteți folosi qsort cu un comparator sau un heap.
 *
 * @param graph Graful
 * @param top_n Câți utilizatori să afișeze
 */
void find_influencers(SocialGraph *graph, int top_n) {
    if (!graph || graph->user_count == 0 || top_n <= 0) return;
    
    printf(COLOR_MAGENTA "\n🌟 TOP %d INFLUENCERI:\n" COLOR_RESET, top_n);
    
    /* YOUR CODE HERE - sortare și afișare */
    
    printf(COLOR_YELLOW "(Implementați TODO 15 pentru afișare completă)\n" COLOR_RESET);
}

/* =============================================================================
 * FUNCȚII UTILITARE
 * =============================================================================
 */

/**
 * Încarcă rețeaua dintr-un fișier CSV
 * Format: name1,name2 (o conexiune pe linie)
 */
int load_network_from_file(SocialGraph *graph, const char *filename) {
    if (!graph || !filename) return -1;
    
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, COLOR_RED "Eroare: Nu pot deschide %s\n" COLOR_RESET, filename);
        return -1;
    }
    
    char line[MAX_LINE];
    int connections = 0;
    
    while (fgets(line, sizeof(line), file)) {
        /* Eliminăm newline */
        line[strcspn(line, "\r\n")] = 0;
        
        /* Găsim separatorul */
        char *comma = strchr(line, ',');
        if (!comma) continue;
        
        *comma = '\0';
        char *name1 = line;
        char *name2 = comma + 1;
        
        /* Eliminăm spațiile */
        while (*name1 == ' ') name1++;
        while (*name2 == ' ') name2++;
        
        if (add_connection_by_name(graph, name1, name2)) {
            connections++;
        }
    }
    
    fclose(file);
    return connections;
}

/**
 * Afișează separator decorativ
 */
void print_separator(const char *title) {
    printf("\n");
    printf(COLOR_YELLOW "╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║  %-60s ║\n", title ? title : "");
    printf("╚═══════════════════════════════════════════════════════════════╝\n" COLOR_RESET);
    printf("\n");
}

/* =============================================================================
 * PROGRAM PRINCIPAL
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf(COLOR_GREEN "╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     SĂPTĂMÂNA 14: ANALIZATOR REȚELE SOCIALE                   ║\n");
    printf("║     Exercițiul 2 - Grafuri și Algoritmi                       ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n" COLOR_RESET);
    
    /* ================================================================
     * PARTEA 1: Creare și Populare Graf
     * ================================================================ */
    print_separator("PARTEA 1: CREARE REȚEA SOCIALĂ");
    
    SocialGraph *network = create_graph();
    if (!network) {
        fprintf(stderr, "Eroare la crearea grafului!\n");
        return 1;
    }
    
    /* Încercăm să încărcăm din fișier */
    int loaded = load_network_from_file(network, "data/social_network.csv");
    
    if (loaded <= 0) {
        printf("Fișierul nu există, creăm rețea de test...\n\n");
        
        /* Creare rețea de test:
         * 
         *    Alice --- Bob --- Diana --- Eve
         *      |       |         |
         *    Charlie --+------Frank
         *      |
         *    George --- Helen
         *      |
         *    Ivan (izolat în altă componentă)
         *      |
         *    Julia
         */
        
        /* Comunitatea 1: Grupul principal */
        add_connection_by_name(network, "Alice", "Bob");
        add_connection_by_name(network, "Alice", "Charlie");
        add_connection_by_name(network, "Bob", "Diana");
        add_connection_by_name(network, "Bob", "Charlie");
        add_connection_by_name(network, "Diana", "Eve");
        add_connection_by_name(network, "Diana", "Frank");
        add_connection_by_name(network, "Charlie", "Frank");
        add_connection_by_name(network, "Charlie", "George");
        add_connection_by_name(network, "George", "Helen");
        
        /* Comunitatea 2: Grup izolat */
        add_connection_by_name(network, "Ivan", "Julia");
        add_connection_by_name(network, "Ivan", "Kevin");
        add_connection_by_name(network, "Julia", "Kevin");
        
        printf(COLOR_GREEN "✓ Creată rețea de test cu %d utilizatori și %d conexiuni\n" COLOR_RESET,
               network->user_count, network->edge_count);
    } else {
        printf(COLOR_GREEN "✓ Încărcate %d conexiuni din fișier\n" COLOR_RESET, loaded);
    }
    
    /* ================================================================
     * PARTEA 2: Afișare Structură Graf
     * ================================================================ */
    print_separator("PARTEA 2: STRUCTURA GRAFULUI");
    
    print_graph(network);
    
    /* ================================================================
     * PARTEA 3: Detectare Comunități (DFS)
     * ================================================================ */
    print_separator("PARTEA 3: DETECTARE COMUNITĂȚI (DFS)");
    
    int communities = find_communities(network);
    
    if (communities > 0) {
        printf(COLOR_GREEN "✓ Detectate %d comunități:\n\n" COLOR_RESET, communities);
        
        for (int c = 0; c < communities; c++) {
            printf(COLOR_CYAN "  Comunitatea %d: " COLOR_RESET, c);
            for (int i = 0; i < network->user_count; i++) {
                if (network->users[i].community == c) {
                    printf("%s ", network->users[i].name);
                }
            }
            printf("\n");
        }
    } else {
        printf(COLOR_YELLOW "Comunități nedetectate. Implementați TODO 12-13.\n" COLOR_RESET);
    }
    
    /* ================================================================
     * PARTEA 4: Grade de Separare (BFS)
     * ================================================================ */
    print_separator("PARTEA 4: GRADE DE SEPARARE (BFS)");
    
    /* Calculăm distanțele de la Alice (index 0) */
    int alice_id = find_user_id(network->name_map, "Alice");
    int diana_id = find_user_id(network->name_map, "Diana");
    int ivan_id = find_user_id(network->name_map, "Ivan");
    
    if (alice_id >= 0 && diana_id >= 0) {
        int sep = degrees_of_separation(network, alice_id, diana_id);
        if (sep >= 0) {
            printf(COLOR_GREEN "✓ " COLOR_RESET);
            printf("Grade de separare Alice -> Diana: %d\n", sep);
        } else {
            printf(COLOR_YELLOW "BFS neimplementat. Verificați TODO 11.\n" COLOR_RESET);
        }
    }
    
    if (alice_id >= 0 && ivan_id >= 0) {
        int sep = degrees_of_separation(network, alice_id, ivan_id);
        if (sep < 0) {
            printf(COLOR_RED "✗ " COLOR_RESET);
            printf("Alice și Ivan nu sunt conectați (comunități diferite)\n");
        } else {
            printf("Grade de separare Alice -> Ivan: %d\n", sep);
        }
    }
    
    /* Afișăm toate distanțele de la Alice */
    printf("\nDistanțe de la Alice:\n");
    int *distances = bfs_distances(network, alice_id);
    if (distances) {
        for (int i = 0; i < network->user_count; i++) {
            if (distances[i] == INFINITY_DIST) {
                printf("  %s: ∞ (neconectat)\n", network->users[i].name);
            } else {
                printf("  %s: %d\n", network->users[i].name, distances[i]);
            }
        }
        free(distances);
    }
    
    /* ================================================================
     * PARTEA 5: Analiza Rețelei
     * ================================================================ */
    print_separator("PARTEA 5: STATISTICI REȚEA");
    
    NetworkStats stats = analyze_network(network);
    
    printf("Analiză rețea socială:\n\n");
    printf("  📊 Utilizatori totali:     %d\n", network->user_count);
    printf("  🔗 Conexiuni totale:       %d\n", network->edge_count);
    printf("  🏘️  Comunități detectate:   %d\n", stats.communities);
    printf("  ⭐ Grad maxim:             %d\n", stats.max_degree);
    
    if (stats.most_influential >= 0) {
        printf("  👑 Cel mai influent:       %s\n", 
               network->users[stats.most_influential].name);
    }
    
    printf("  📈 Grad mediu:             %.2f\n", stats.avg_degree);
    printf("  🎯 Densitate rețea:        %.4f (%.2f%%)\n", 
           stats.density, stats.density * 100);
    
    /* ================================================================
     * PARTEA 6: Top Influenceri
     * ================================================================ */
    print_separator("PARTEA 6: ANALIZA INFLUENCERILOR");
    
    find_influencers(network, 5);
    
    /* ================================================================
     * CURĂȚARE MEMORIE
     * ================================================================ */
    print_separator("CURĂȚARE MEMORIE");
    
    printf("Eliberare memorie...\n");
    free_graph(network);
    
    printf(COLOR_GREEN "✓ Toată memoria a fost eliberată.\n" COLOR_RESET);
    printf(COLOR_GREEN "✓ Rulați cu Valgrind pentru verificare!\n" COLOR_RESET);
    
    return 0;
}

/* =============================================================================
 * PROVOCĂRI BONUS (Opțional)
 * =============================================================================
 *
 * 1. Implementați funcția find_shortest_path care returnează calea
 *    efectivă (nu doar lungimea) între doi utilizatori.
 *
 * 2. Implementați detectarea "bridges" - conexiuni a căror ștergere
 *    ar deconecta rețeaua.
 *
 * 3. Calculați "betweenness centrality" - cât de des apare un nod
 *    pe drumurile cele mai scurte dintre altele.
 *
 * 4. Implementați algoritmul de clustering coefficient pentru
 *    a măsura cât de conectați sunt vecinii unui nod între ei.
 *
 * 5. Adăugați suport pentru conexiuni ponderate (prieteni apropiați
 *    vs. cunoștințe) și adaptați BFS la Dijkstra.
 *
 * =============================================================================
 */
