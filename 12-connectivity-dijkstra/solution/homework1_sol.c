/**
 * =============================================================================
 * SOLUȚIE TEMA 1: NAVIGATOR SOCIAL - ANALIZATOR REȚEA SOCIALĂ
 * =============================================================================
 *
 * Implementare completă pentru analiza unei rețele sociale:
 * - Citirea grafului din fișier
 * - Gradul de separare (BFS)
 * - Componente conexe (comunități)
 * - Utilizatori influenți (grad maxim)
 *
 * CONFIDENȚIAL - NU distribuiți studenților înainte de termenul de predare!
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -o homework1_sol homework1_sol.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* =============================================================================
 * CONSTANTE
 * =============================================================================
 */

#define MAX_USERS 1000
#define INFINITY 999999

/* =============================================================================
 * STRUCTURI DE DATE
 * =============================================================================
 */

/**
 * Structură pentru coadă (BFS)
 */
typedef struct {
    int items[MAX_USERS];
    int front;
    int rear;
} Queue;

/**
 * Structură pentru nod în lista de adiacență
 */
typedef struct Node {
    int vertex;
    struct Node* next;
} Node;

/**
 * Structură pentru graf cu liste de adiacență
 * Folosim liste pentru eficiență în rețele sociale (grafuri sparse)
 */
typedef struct {
    int vertices;
    int edges;
    Node** adj_list;
} Graph;

/**
 * Structură pentru pereche (utilizator, grad) - pentru sortare
 */
typedef struct {
    int user;
    int degree;
} UserDegree;

/* =============================================================================
 * FUNCȚII PENTRU COADĂ
 * =============================================================================
 */

void queue_init(Queue* q) {
    q->front = -1;
    q->rear = -1;
}

bool queue_is_empty(Queue* q) {
    return q->front == -1;
}

void queue_enqueue(Queue* q, int value) {
    if (q->front == -1) q->front = 0;
    q->rear++;
    q->items[q->rear] = value;
}

int queue_dequeue(Queue* q) {
    if (queue_is_empty(q)) return -1;
    int item = q->items[q->front];
    q->front++;
    if (q->front > q->rear) {
        q->front = -1;
        q->rear = -1;
    }
    return item;
}

/* =============================================================================
 * FUNCȚII PENTRU GRAF
 * =============================================================================
 */

/**
 * Creează un nod nou pentru lista de adiacență
 */
Node* create_node(int vertex) {
    Node* node = (Node*)malloc(sizeof(Node));
    if (node == NULL) {
        fprintf(stderr, "Eroare: Alocare memorie eșuată!\n");
        exit(1);
    }
    node->vertex = vertex;
    node->next = NULL;
    return node;
}

/**
 * Creează un graf gol cu n noduri
 */
Graph* create_graph(int n) {
    Graph* g = (Graph*)malloc(sizeof(Graph));
    if (g == NULL) {
        fprintf(stderr, "Eroare: Alocare memorie eșuată!\n");
        exit(1);
    }
    
    g->vertices = n;
    g->edges = 0;
    g->adj_list = (Node**)malloc(n * sizeof(Node*));
    
    if (g->adj_list == NULL) {
        fprintf(stderr, "Eroare: Alocare memorie eșuată!\n");
        free(g);
        exit(1);
    }
    
    for (int i = 0; i < n; i++) {
        g->adj_list[i] = NULL;
    }
    
    return g;
}

/**
 * Adaugă o muchie neorientată
 */
void add_edge(Graph* g, int u, int v) {
    if (u < 0 || u >= g->vertices || v < 0 || v >= g->vertices) {
        return;
    }
    
    /* Adăugăm v în lista lui u */
    Node* node = create_node(v);
    node->next = g->adj_list[u];
    g->adj_list[u] = node;
    
    /* Adăugăm u în lista lui v (graf neorientat) */
    node = create_node(u);
    node->next = g->adj_list[v];
    g->adj_list[v] = node;
    
    g->edges++;
}

/**
 * Eliberează memoria grafului
 */
void free_graph(Graph* g) {
    if (g == NULL) return;
    
    for (int i = 0; i < g->vertices; i++) {
        Node* current = g->adj_list[i];
        while (current != NULL) {
            Node* temp = current;
            current = current->next;
            free(temp);
        }
    }
    
    free(g->adj_list);
    free(g);
}

/**
 * Calculează gradul unui nod
 */
int get_degree(Graph* g, int vertex) {
    if (vertex < 0 || vertex >= g->vertices) return 0;
    
    int degree = 0;
    Node* current = g->adj_list[vertex];
    while (current != NULL) {
        degree++;
        current = current->next;
    }
    return degree;
}

/* =============================================================================
 * CERINȚA 1: CITIREA GRAFULUI DIN FIȘIER (10p)
 * =============================================================================
 */

/**
 * Încarcă rețeaua socială din fișier
 * Format: N M pe prima linie, apoi M perechi u v
 */
Graph* load_social_network(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Eroare: Nu pot deschide fișierul %s\n", filename);
        return NULL;
    }
    
    int n, m;
    if (fscanf(file, "%d %d", &n, &m) != 2) {
        fprintf(stderr, "Eroare: Format invalid al fișierului\n");
        fclose(file);
        return NULL;
    }
    
    Graph* g = create_graph(n);
    
    for (int i = 0; i < m; i++) {
        int u, v;
        if (fscanf(file, "%d %d", &u, &v) != 2) {
            fprintf(stderr, "Eroare: Muchie invalidă la linia %d\n", i + 2);
            continue;
        }
        add_edge(g, u, v);
    }
    
    fclose(file);
    printf("✓ Rețea socială încărcată: %d utilizatori, %d conexiuni\n", n, m);
    return g;
}

/* =============================================================================
 * CERINȚA 2: GRADUL DE SEPARARE - BFS (15p)
 * =============================================================================
 */

/**
 * Calculează gradul de separare (distanța minimă) între doi utilizatori
 * Returnează -1 dacă nu sunt conectați
 */
int degree_of_separation(Graph* g, int user1, int user2) {
    if (g == NULL || user1 < 0 || user1 >= g->vertices || 
        user2 < 0 || user2 >= g->vertices) {
        return -1;
    }
    
    if (user1 == user2) return 0;
    
    /* Algoritm BFS pentru distanța minimă */
    int* distance = (int*)malloc(g->vertices * sizeof(int));
    int* parent = (int*)malloc(g->vertices * sizeof(int));
    bool* visited = (bool*)malloc(g->vertices * sizeof(bool));
    
    for (int i = 0; i < g->vertices; i++) {
        distance[i] = INFINITY;
        parent[i] = -1;
        visited[i] = false;
    }
    
    Queue q;
    queue_init(&q);
    
    visited[user1] = true;
    distance[user1] = 0;
    queue_enqueue(&q, user1);
    
    while (!queue_is_empty(&q)) {
        int current = queue_dequeue(&q);
        
        Node* neighbor = g->adj_list[current];
        while (neighbor != NULL) {
            int v = neighbor->vertex;
            if (!visited[v]) {
                visited[v] = true;
                distance[v] = distance[current] + 1;
                parent[v] = current;
                queue_enqueue(&q, v);
                
                if (v == user2) {
                    /* Am găsit destinația - reconstruim drumul */
                    printf("  Drum găsit: ");
                    
                    int path[MAX_USERS];
                    int path_len = 0;
                    int node = user2;
                    
                    while (node != -1) {
                        path[path_len++] = node;
                        node = parent[node];
                    }
                    
                    /* Afișăm în ordine corectă */
                    for (int i = path_len - 1; i >= 0; i--) {
                        printf("%d", path[i]);
                        if (i > 0) printf(" → ");
                    }
                    printf("\n");
                    
                    int result = distance[user2];
                    free(distance);
                    free(parent);
                    free(visited);
                    return result;
                }
            }
            neighbor = neighbor->next;
        }
    }
    
    free(distance);
    free(parent);
    free(visited);
    return -1;  /* Nu sunt conectați */
}

/* =============================================================================
 * CERINȚA 3: COMPONENTE CONEXE - COMUNITĂȚI (15p)
 * =============================================================================
 */

/**
 * Helper DFS pentru marcarea componentei
 */
void dfs_component(Graph* g, int vertex, bool* visited, int* community, int comp_id) {
    visited[vertex] = true;
    community[vertex] = comp_id;
    
    Node* neighbor = g->adj_list[vertex];
    while (neighbor != NULL) {
        if (!visited[neighbor->vertex]) {
            dfs_component(g, neighbor->vertex, visited, community, comp_id);
        }
        neighbor = neighbor->next;
    }
}

/**
 * Găsește toate comunitățile (componente conexe)
 * Returnează numărul de comunități
 */
int find_communities(Graph* g, int* community) {
    if (g == NULL) return 0;
    
    bool* visited = (bool*)malloc(g->vertices * sizeof(bool));
    for (int i = 0; i < g->vertices; i++) {
        visited[i] = false;
        community[i] = -1;
    }
    
    int num_communities = 0;
    
    for (int i = 0; i < g->vertices; i++) {
        if (!visited[i]) {
            dfs_component(g, i, visited, community, num_communities);
            num_communities++;
        }
    }
    
    free(visited);
    return num_communities;
}

/**
 * Afișează membrii fiecărei comunități
 */
void print_communities(Graph* g, int* community, int num_communities) {
    printf("\n--- Detalii Comunități ---\n");
    
    for (int c = 0; c < num_communities; c++) {
        printf("Comunitatea %d: ", c);
        int count = 0;
        for (int i = 0; i < g->vertices; i++) {
            if (community[i] == c) {
                if (count > 0) printf(", ");
                printf("User%d", i);
                count++;
            }
        }
        printf(" (%d membri)\n", count);
    }
}

/* =============================================================================
 * CERINȚA 4: UTILIZATORI INFLUENȚI (10p)
 * =============================================================================
 */

/**
 * Funcție de comparare pentru qsort (descrescător după grad)
 */
int compare_degree(const void* a, const void* b) {
    UserDegree* ua = (UserDegree*)a;
    UserDegree* ub = (UserDegree*)b;
    return ub->degree - ua->degree;  /* Descrescător */
}

/**
 * Afișează top k utilizatori cu cel mai mare grad
 */
void top_influencers(Graph* g, int k) {
    if (g == NULL || k <= 0) return;
    
    if (k > g->vertices) k = g->vertices;
    
    /* Creăm array cu perechi (user, grad) */
    UserDegree* users = (UserDegree*)malloc(g->vertices * sizeof(UserDegree));
    
    for (int i = 0; i < g->vertices; i++) {
        users[i].user = i;
        users[i].degree = get_degree(g, i);
    }
    
    /* Sortăm descrescător după grad */
    qsort(users, g->vertices, sizeof(UserDegree), compare_degree);
    
    /* Afișăm top k */
    printf("\n--- Top %d Influenceri ---\n", k);
    printf("╔═══════╤════════════════╤═════════╗\n");
    printf("║ Rank  │    Utilizator  │  Prieteni║\n");
    printf("╠═══════╪════════════════╪═════════╣\n");
    
    for (int i = 0; i < k; i++) {
        printf("║  #%d   │    User %-5d  │   %-5d ║\n", 
               i + 1, users[i].user, users[i].degree);
    }
    printf("╚═══════╧════════════════╧═════════╝\n");
    
    free(users);
}

/* =============================================================================
 * FUNCȚIA PRINCIPALĂ - DEMONSTRAȚIE
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     SOLUȚIE TEMA 1: Navigator Social - Rețea Socială          ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Creăm o rețea socială de test
     * 
     *   Comunitatea 0:          Comunitatea 1:
     *   0 --- 1 --- 2           6 --- 7
     *   |     |                       |
     *   3 --- 4 --- 5                 8
     */
    
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("CREARE REȚEA SOCIALĂ DE TEST\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    
    Graph* network = create_graph(9);
    
    /* Comunitatea principală (0-5) */
    add_edge(network, 0, 1);
    add_edge(network, 0, 3);
    add_edge(network, 1, 2);
    add_edge(network, 1, 4);
    add_edge(network, 3, 4);
    add_edge(network, 4, 5);
    add_edge(network, 2, 5);
    
    /* Comunitate separată (6-8) */
    add_edge(network, 6, 7);
    add_edge(network, 7, 8);
    
    printf("✓ Rețea creată: 9 utilizatori, 9 conexiuni\n");
    printf("\nTopologie:\n");
    printf("  Comunitatea A: 0-1-2-3-4-5 (conectați)\n");
    printf("  Comunitatea B: 6-7-8 (izolați)\n\n");
    
    /* Test gradul de separare */
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("TEST 1: GRADUL DE SEPARARE (BFS)\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    
    printf("\nÎntre User0 și User5:\n");
    int sep = degree_of_separation(network, 0, 5);
    printf("  Grad de separare: %d\n", sep);
    
    printf("\nÎntre User0 și User2:\n");
    sep = degree_of_separation(network, 0, 2);
    printf("  Grad de separare: %d\n", sep);
    
    printf("\nÎntre User0 și User7 (comunități diferite):\n");
    sep = degree_of_separation(network, 0, 7);
    if (sep == -1) {
        printf("  ✗ Nu sunt conectați (comunități diferite)\n");
    } else {
        printf("  Grad de separare: %d\n", sep);
    }
    
    /* Test componente conexe */
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("TEST 2: DETECTARE COMUNITĂȚI (COMPONENTE CONEXE)\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    
    int community[MAX_USERS];
    int num_communities = find_communities(network, community);
    printf("\n✓ Număr de comunități detectate: %d\n", num_communities);
    
    print_communities(network, community, num_communities);
    
    /* Test influenceri */
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("TEST 3: TOP INFLUENCERI\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    
    top_influencers(network, 5);
    
    /* Eliberare memorie */
    free_graph(network);
    
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("✓ Toate testele executate cu succes!\n");
    printf("✓ Memoria eliberată corect\n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    return 0;
}
