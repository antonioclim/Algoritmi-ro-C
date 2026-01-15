/**
 * =============================================================================
 * SOLUȚIE EXERCIȚIUL 2: Analizator Rețele Sociale
 * Săptămâna 14 - Recapitulare Avansată și Optimizare
 * =============================================================================
 *
 * ATENȚIE: Acest fișier conține soluția completă.
 * Folosiți-l doar pentru verificare după ce ați încercat singuri!
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

/* =============================================================================
 * CONSTANTE
 * =============================================================================
 */

#define MAX_USERS 1000
#define MAX_NAME_LEN 64
#define HASH_SIZE 127
#define MAX_LINE 256
#define INFINITY_DIST INT_MAX

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

typedef struct AdjNode {
    int user_id;
    struct AdjNode *next;
} AdjNode;

typedef struct {
    int id;
    char name[MAX_NAME_LEN];
    int degree;
    int community;
    AdjNode *neighbors;
} User;

typedef struct HashEntry {
    char name[MAX_NAME_LEN];
    int user_id;
    struct HashEntry *next;
} HashEntry;

typedef struct {
    HashEntry *buckets[HASH_SIZE];
} NameHashTable;

typedef struct {
    User users[MAX_USERS];
    int user_count;
    int edge_count;
    NameHashTable *name_map;
} SocialGraph;

/* SOLUȚIE TODO 2: Structura Queue */
typedef struct {
    int *data;
    int front;
    int rear;
    int capacity;
    int size;
} Queue;

typedef struct {
    int communities;
    int max_degree;
    int most_influential;
    double avg_degree;
    double density;
} NetworkStats;

/* =============================================================================
 * IMPLEMENTARE HASH TABLE
 * =============================================================================
 */

/* SOLUȚIE TODO 3: Funcția hash djb2 */
unsigned int hash_string(const char *str) {
    unsigned int hash = 5381;
    int c;
    
    while ((c = *str++)) {
        hash = hash * 33 + c;
    }
    
    return hash % HASH_SIZE;
}

NameHashTable *create_name_table(void) {
    NameHashTable *table = malloc(sizeof(NameHashTable));
    if (!table) return NULL;
    
    for (int i = 0; i < HASH_SIZE; i++) {
        table->buckets[i] = NULL;
    }
    
    return table;
}

/* SOLUȚIE TODO 4: Inserare în hash table */
bool insert_name(NameHashTable *table, const char *name, int user_id) {
    if (!table || !name) return false;
    
    /* Verificăm dacă există deja */
    unsigned int index = hash_string(name);
    HashEntry *entry = table->buckets[index];
    while (entry) {
        if (strcmp(entry->name, name) == 0) {
            return false;  /* Există deja */
        }
        entry = entry->next;
    }
    
    /* Alocăm noua intrare */
    HashEntry *new_entry = malloc(sizeof(HashEntry));
    if (!new_entry) return false;
    
    strncpy(new_entry->name, name, MAX_NAME_LEN - 1);
    new_entry->name[MAX_NAME_LEN - 1] = '\0';
    new_entry->user_id = user_id;
    
    /* Inserăm la începutul bucket-ului */
    new_entry->next = table->buckets[index];
    table->buckets[index] = new_entry;
    
    return true;
}

/* SOLUȚIE TODO 5: Căutare în hash table */
int find_user_id(NameHashTable *table, const char *name) {
    if (!table || !name) return -1;
    
    unsigned int index = hash_string(name);
    HashEntry *entry = table->buckets[index];
    
    while (entry) {
        if (strcmp(entry->name, name) == 0) {
            return entry->user_id;
        }
        entry = entry->next;
    }
    
    return -1;
}

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
 * IMPLEMENTARE COADĂ
 * =============================================================================
 */

/* SOLUȚIE TODO 6: Creare coadă */
Queue *create_queue(int capacity) {
    if (capacity <= 0) return NULL;
    
    Queue *q = malloc(sizeof(Queue));
    if (!q) return NULL;
    
    q->data = malloc(capacity * sizeof(int));
    if (!q->data) {
        free(q);
        return NULL;
    }
    
    q->front = 0;
    q->rear = -1;
    q->capacity = capacity;
    q->size = 0;
    
    return q;
}

/* SOLUȚIE TODO 7: Enqueue */
bool enqueue(Queue *q, int value) {
    if (!q || q->size >= q->capacity) return false;
    
    q->rear = (q->rear + 1) % q->capacity;
    q->data[q->rear] = value;
    q->size++;
    
    return true;
}

/* SOLUȚIE TODO 8: Dequeue */
int dequeue(Queue *q) {
    if (!q || q->size == 0) return -1;
    
    int value = q->data[q->front];
    q->front = (q->front + 1) % q->capacity;
    q->size--;
    
    return value;
}

bool is_queue_empty(Queue *q) {
    return !q || q->size == 0;
}

void free_queue(Queue *q) {
    if (!q) return;
    free(q->data);
    free(q);
}

/* =============================================================================
 * IMPLEMENTARE GRAF
 * =============================================================================
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
    
    for (int i = 0; i < MAX_USERS; i++) {
        graph->users[i].id = -1;
        graph->users[i].name[0] = '\0';
        graph->users[i].degree = 0;
        graph->users[i].community = -1;
        graph->users[i].neighbors = NULL;
    }
    
    return graph;
}

/* SOLUȚIE TODO 9: Adăugare utilizator */
int add_user(SocialGraph *graph, const char *name) {
    if (!graph || !name) return -1;
    if (graph->user_count >= MAX_USERS) return -1;
    
    /* Verificăm dacă există deja */
    int existing = find_user_id(graph->name_map, name);
    if (existing >= 0) return existing;
    
    /* Adăugăm noul utilizator */
    int id = graph->user_count;
    graph->users[id].id = id;
    strncpy(graph->users[id].name, name, MAX_NAME_LEN - 1);
    graph->users[id].name[MAX_NAME_LEN - 1] = '\0';
    graph->users[id].degree = 0;
    graph->users[id].community = -1;
    graph->users[id].neighbors = NULL;
    
    /* Adăugăm în hash table */
    insert_name(graph->name_map, name, id);
    
    graph->user_count++;
    
    return id;
}

int get_or_create_user(SocialGraph *graph, const char *name) {
    int id = find_user_id(graph->name_map, name);
    if (id >= 0) return id;
    return add_user(graph, name);
}

/* Helper: verifică dacă există conexiune */
static bool has_connection(SocialGraph *graph, int user1, int user2) {
    AdjNode *node = graph->users[user1].neighbors;
    while (node) {
        if (node->user_id == user2) return true;
        node = node->next;
    }
    return false;
}

/* SOLUȚIE TODO 10: Adăugare conexiune */
bool add_connection(SocialGraph *graph, int user1_id, int user2_id) {
    if (!graph) return false;
    if (user1_id < 0 || user1_id >= graph->user_count) return false;
    if (user2_id < 0 || user2_id >= graph->user_count) return false;
    if (user1_id == user2_id) return false;
    
    /* Verificăm dacă conexiunea există deja */
    if (has_connection(graph, user1_id, user2_id)) {
        return false;
    }
    
    /* Adăugăm muchie user1 -> user2 */
    AdjNode *node1 = malloc(sizeof(AdjNode));
    if (!node1) return false;
    node1->user_id = user2_id;
    node1->next = graph->users[user1_id].neighbors;
    graph->users[user1_id].neighbors = node1;
    
    /* Adăugăm muchie user2 -> user1 */
    AdjNode *node2 = malloc(sizeof(AdjNode));
    if (!node2) {
        /* Rollback */
        graph->users[user1_id].neighbors = node1->next;
        free(node1);
        return false;
    }
    node2->user_id = user1_id;
    node2->next = graph->users[user2_id].neighbors;
    graph->users[user2_id].neighbors = node2;
    
    /* Actualizăm gradele și contorul */
    graph->users[user1_id].degree++;
    graph->users[user2_id].degree++;
    graph->edge_count++;
    
    return true;
}

bool add_connection_by_name(SocialGraph *graph, const char *name1, const char *name2) {
    if (!graph || !name1 || !name2) return false;
    
    int id1 = get_or_create_user(graph, name1);
    int id2 = get_or_create_user(graph, name2);
    
    return add_connection(graph, id1, id2);
}

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

void free_graph(SocialGraph *graph) {
    if (!graph) return;
    
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
 * IMPLEMENTARE BFS
 * =============================================================================
 */

/* SOLUȚIE TODO 11: BFS pentru distanțe */
int *bfs_distances(SocialGraph *graph, int start_id) {
    if (!graph || start_id < 0 || start_id >= graph->user_count) return NULL;
    
    int *distances = malloc(graph->user_count * sizeof(int));
    if (!distances) return NULL;
    
    /* Inițializare distanțe */
    for (int i = 0; i < graph->user_count; i++) {
        distances[i] = INFINITY_DIST;
    }
    
    /* BFS */
    Queue *queue = create_queue(graph->user_count);
    if (!queue) {
        free(distances);
        return NULL;
    }
    
    distances[start_id] = 0;
    enqueue(queue, start_id);
    
    while (!is_queue_empty(queue)) {
        int current = dequeue(queue);
        
        /* Parcurgem vecinii */
        AdjNode *neighbor = graph->users[current].neighbors;
        while (neighbor) {
            int next = neighbor->user_id;
            
            if (distances[next] == INFINITY_DIST) {
                distances[next] = distances[current] + 1;
                enqueue(queue, next);
            }
            
            neighbor = neighbor->next;
        }
    }
    
    free_queue(queue);
    return distances;
}

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
 * IMPLEMENTARE DFS - COMUNITĂȚI
 * =============================================================================
 */

/* SOLUȚIE TODO 12: DFS pentru comunitate */
void dfs_community(SocialGraph *graph, int user_id, bool *visited, int community_id) {
    if (!graph || !visited) return;
    if (user_id < 0 || user_id >= graph->user_count) return;
    if (visited[user_id]) return;
    
    /* Marcăm ca vizitat și setăm comunitatea */
    visited[user_id] = true;
    graph->users[user_id].community = community_id;
    
    /* Parcurgem vecinii recursiv */
    AdjNode *neighbor = graph->users[user_id].neighbors;
    while (neighbor) {
        if (!visited[neighbor->user_id]) {
            dfs_community(graph, neighbor->user_id, visited, community_id);
        }
        neighbor = neighbor->next;
    }
}

/* SOLUȚIE TODO 13: Găsire comunități */
int find_communities(SocialGraph *graph) {
    if (!graph || graph->user_count == 0) return 0;
    
    bool *visited = calloc(graph->user_count, sizeof(bool));
    if (!visited) return 0;
    
    int community_count = 0;
    
    for (int i = 0; i < graph->user_count; i++) {
        if (!visited[i]) {
            dfs_community(graph, i, visited, community_count);
            community_count++;
        }
    }
    
    free(visited);
    return community_count;
}

/* =============================================================================
 * IMPLEMENTARE ANALIZĂ
 * =============================================================================
 */

/* SOLUȚIE TODO 14: Analiză completă */
NetworkStats analyze_network(SocialGraph *graph) {
    NetworkStats stats = {0, 0, -1, 0.0, 0.0};
    
    if (!graph || graph->user_count == 0) return stats;
    
    /* Număr comunități */
    stats.communities = find_communities(graph);
    
    /* Gradul maxim și utilizatorul cel mai influent */
    for (int i = 0; i < graph->user_count; i++) {
        if (graph->users[i].degree > stats.max_degree) {
            stats.max_degree = graph->users[i].degree;
            stats.most_influential = i;
        }
    }
    
    /* Gradul mediu */
    stats.avg_degree = (2.0 * graph->edge_count) / graph->user_count;
    
    /* Densitatea */
    if (graph->user_count > 1) {
        int max_edges = graph->user_count * (graph->user_count - 1) / 2;
        stats.density = (double)graph->edge_count / max_edges;
    }
    
    return stats;
}

/* Helper: comparator pentru qsort */
static int compare_by_degree(const void *a, const void *b) {
    const User *ua = (const User *)a;
    const User *ub = (const User *)b;
    return ub->degree - ua->degree;  /* Descrescător */
}

/* SOLUȚIE TODO 15: Găsire influenceri */
void find_influencers(SocialGraph *graph, int top_n) {
    if (!graph || graph->user_count == 0 || top_n <= 0) return;
    
    printf(COLOR_MAGENTA "\n🌟 TOP %d INFLUENCERI:\n" COLOR_RESET, top_n);
    
    /* Copiem utilizatorii pentru sortare */
    User *sorted = malloc(graph->user_count * sizeof(User));
    if (!sorted) return;
    
    memcpy(sorted, graph->users, graph->user_count * sizeof(User));
    
    /* Sortăm după grad */
    qsort(sorted, graph->user_count, sizeof(User), compare_by_degree);
    
    /* Afișăm top N */
    int count = (top_n < graph->user_count) ? top_n : graph->user_count;
    for (int i = 0; i < count; i++) {
        printf("  %d. " COLOR_CYAN "%s" COLOR_RESET " - %d conexiuni\n",
               i + 1, sorted[i].name, sorted[i].degree);
    }
    
    free(sorted);
}

/* =============================================================================
 * FUNCȚII UTILITARE
 * =============================================================================
 */

int load_network_from_file(SocialGraph *graph, const char *filename) {
    if (!graph || !filename) return -1;
    
    FILE *file = fopen(filename, "r");
    if (!file) return -1;
    
    char line[MAX_LINE];
    int connections = 0;
    
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = 0;
        
        char *comma = strchr(line, ',');
        if (!comma) continue;
        
        *comma = '\0';
        char *name1 = line;
        char *name2 = comma + 1;
        
        while (*name1 == ' ') name1++;
        while (*name2 == ' ') name2++;
        
        if (add_connection_by_name(graph, name1, name2)) {
            connections++;
        }
    }
    
    fclose(file);
    return connections;
}

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
    printf("║     SOLUȚIE COMPLETĂ                                          ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n" COLOR_RESET);
    
    /* Partea 1: Creare Graf */
    print_separator("PARTEA 1: CREARE REȚEA SOCIALĂ");
    
    SocialGraph *network = create_graph();
    if (!network) {
        fprintf(stderr, "Eroare la crearea grafului!\n");
        return 1;
    }
    
    int loaded = load_network_from_file(network, "data/social_network.csv");
    
    if (loaded <= 0) {
        printf("Creăm rețea de test...\n");
        
        add_connection_by_name(network, "Alice", "Bob");
        add_connection_by_name(network, "Alice", "Charlie");
        add_connection_by_name(network, "Bob", "Diana");
        add_connection_by_name(network, "Bob", "Charlie");
        add_connection_by_name(network, "Diana", "Eve");
        add_connection_by_name(network, "Diana", "Frank");
        add_connection_by_name(network, "Charlie", "Frank");
        add_connection_by_name(network, "Charlie", "George");
        add_connection_by_name(network, "George", "Helen");
        
        add_connection_by_name(network, "Ivan", "Julia");
        add_connection_by_name(network, "Ivan", "Kevin");
        add_connection_by_name(network, "Julia", "Kevin");
        
        printf(COLOR_GREEN "✓ Creată rețea cu %d utilizatori și %d conexiuni\n" COLOR_RESET,
               network->user_count, network->edge_count);
    } else {
        printf(COLOR_GREEN "✓ Încărcate %d conexiuni din fișier\n" COLOR_RESET, loaded);
    }
    
    /* Partea 2: Afișare Structură */
    print_separator("PARTEA 2: STRUCTURA GRAFULUI");
    print_graph(network);
    
    /* Partea 3: Detectare Comunități */
    print_separator("PARTEA 3: DETECTARE COMUNITĂȚI (DFS)");
    
    int communities = find_communities(network);
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
    
    /* Partea 4: Grade de Separare */
    print_separator("PARTEA 4: GRADE DE SEPARARE (BFS)");
    
    int alice_id = find_user_id(network->name_map, "Alice");
    int diana_id = find_user_id(network->name_map, "Diana");
    int ivan_id = find_user_id(network->name_map, "Ivan");
    
    if (alice_id >= 0 && diana_id >= 0) {
        int sep = degrees_of_separation(network, alice_id, diana_id);
        printf(COLOR_GREEN "✓ " COLOR_RESET);
        printf("Grade de separare Alice -> Diana: %d\n", sep);
    }
    
    if (alice_id >= 0 && ivan_id >= 0) {
        int sep = degrees_of_separation(network, alice_id, ivan_id);
        if (sep < 0) {
            printf(COLOR_RED "✗ " COLOR_RESET);
            printf("Alice și Ivan nu sunt conectați (comunități diferite)\n");
        }
    }
    
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
    
    /* Partea 5: Statistici */
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
    
    /* Partea 6: Influenceri */
    print_separator("PARTEA 6: TOP INFLUENCERI");
    find_influencers(network, 5);
    
    /* Curățare */
    print_separator("CURĂȚARE MEMORIE");
    
    free_graph(network);
    printf(COLOR_GREEN "✓ Memorie eliberată corect.\n" COLOR_RESET);
    
    return 0;
}
