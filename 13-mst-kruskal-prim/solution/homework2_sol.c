/**
 * =============================================================================
 * SOLUȚIE TEMĂ 2: Analiză Rețea Financiară (Detectare Arbitraj)
 * =============================================================================
 * 
 * ATENȚIE: Acest fișier este DOAR pentru instructori!
 * Nu distribuiți studenților înainte de termenul limită.
 * 
 * Această soluție demonstrează:
 *   1. Citirea rețelei de tranzacții din fișier
 *   2. Transformarea ratelor de schimb în ponderi (-log)
 *   3. Algoritmul Bellman-Ford pentru detectare arbitraj
 *   4. Identificarea nodurilor afectate de cicluri negative
 *   5. Generarea raportului de analiză
 * 
 * Compilare: gcc -Wall -Wextra -std=c11 -o hw2_sol homework2_sol.c -lm
 * Utilizare: ./hw2_sol < forex.txt
 * 
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <stdbool.h>
#include <float.h>

/* =============================================================================
 * CONSTANTE
 * =============================================================================
 */

#define MAX_NAME 32
#define INF_D DBL_MAX
#define NEG_INF_D (-DBL_MAX)

#define GREEN   "\033[0;32m"
#define CYAN    "\033[0;36m"
#define YELLOW  "\033[0;33m"
#define RED     "\033[0;31m"
#define MAGENTA "\033[0;35m"
#define NC      "\033[0m"

/* =============================================================================
 * STRUCTURI DE DATE
 * =============================================================================
 */

typedef struct {
    int src;
    int dest;
    double weight;          /* -log(rate) pentru Bellman-Ford */
    double original_rate;   /* Rata originală de schimb */
} FinancialEdge;

typedef struct {
    int V;                  /* Număr de companii/valute */
    int E;                  /* Număr de rate de schimb */
    FinancialEdge* edges;   /* Array de muchii */
    char** names;           /* Numele companiilor/valutelor */
} FinancialNetwork;

typedef struct {
    double* dist;           /* Distanțele (în spațiul -log) */
    int* prev;              /* Predecesorii pentru reconstrucție */
    bool* in_neg_cycle;     /* Noduri afectate de arbitraj */
    bool has_arbitrage;     /* Flag global pentru oportunitate de arbitraj */
} AnalysisResult;

/* =============================================================================
 * FUNCȚII REȚEA
 * =============================================================================
 */

FinancialNetwork* network_create(int V, int E) {
    FinancialNetwork* n = malloc(sizeof(FinancialNetwork));
    n->V = V;
    n->E = E;
    n->edges = malloc(E * sizeof(FinancialEdge));
    n->names = malloc(V * sizeof(char*));
    
    for (int i = 0; i < V; i++) {
        n->names[i] = malloc(MAX_NAME * sizeof(char));
        n->names[i][0] = '\0';
    }
    
    return n;
}

void network_destroy(FinancialNetwork* n) {
    for (int i = 0; i < n->V; i++) {
        free(n->names[i]);
    }
    free(n->names);
    free(n->edges);
    free(n);
}

void network_set_name(FinancialNetwork* n, int idx, const char* name) {
    /* Folosim snprintf pentru a evita warning-ul de truncare */
    snprintf(n->names[idx], MAX_NAME, "%s", name);
}

void network_add_rate(FinancialNetwork* n, int idx, int src, int dest, double rate) {
    n->edges[idx].src = src;
    n->edges[idx].dest = dest;
    n->edges[idx].original_rate = rate;
    /* Transformăm: ciclu negativ în graful cu -log(rate) = arbitraj */
    n->edges[idx].weight = -log(rate);
}

/* =============================================================================
 * FUNCȚII REZULTAT
 * =============================================================================
 */

AnalysisResult* result_create(int V, int src) {
    AnalysisResult* r = malloc(sizeof(AnalysisResult));
    r->dist = malloc(V * sizeof(double));
    r->prev = malloc(V * sizeof(int));
    r->in_neg_cycle = calloc(V, sizeof(bool));
    r->has_arbitrage = false;
    
    for (int i = 0; i < V; i++) {
        r->dist[i] = INF_D;
        r->prev[i] = -1;
    }
    r->dist[src] = 0;
    
    return r;
}

void result_destroy(AnalysisResult* r) {
    free(r->dist);
    free(r->prev);
    free(r->in_neg_cycle);
    free(r);
}

/* =============================================================================
 * BELLMAN-FORD PENTRU DETECTARE ARBITRAJ
 * =============================================================================
 */

AnalysisResult* detect_arbitrage(FinancialNetwork* n, int src) {
    AnalysisResult* r = result_create(n->V, src);
    
    /* Faza 1: V-1 relaxări */
    for (int i = 0; i < n->V - 1; i++) {
        bool updated = false;
        
        for (int j = 0; j < n->E; j++) {
            int u = n->edges[j].src;
            int v = n->edges[j].dest;
            double w = n->edges[j].weight;
            
            if (r->dist[u] != INF_D && r->dist[u] + w < r->dist[v]) {
                r->dist[v] = r->dist[u] + w;
                r->prev[v] = u;
                updated = true;
            }
        }
        
        if (!updated) break;
    }
    
    /* Faza 2: Detectare cicluri negative (arbitraj) */
    /* Folosim BFS pentru propagare */
    int* queue = malloc(n->V * sizeof(int));
    bool* in_queue = calloc(n->V, sizeof(bool));
    int front = 0, rear = 0;
    
    /* Găsim nodurile direct în ciclu negativ */
    for (int j = 0; j < n->E; j++) {
        int u = n->edges[j].src;
        int v = n->edges[j].dest;
        double w = n->edges[j].weight;
        
        if (r->dist[u] != INF_D && r->dist[u] != NEG_INF_D &&
            r->dist[u] + w < r->dist[v]) {
            
            r->has_arbitrage = true;
            
            if (!in_queue[v]) {
                queue[rear++] = v;
                in_queue[v] = true;
            }
        }
    }
    
    /* Propagăm la toate nodurile accesibile */
    while (front < rear) {
        int v = queue[front++];
        r->in_neg_cycle[v] = true;
        r->dist[v] = NEG_INF_D;
        
        for (int j = 0; j < n->E; j++) {
            if (n->edges[j].src == v) {
                int dest = n->edges[j].dest;
                if (!r->in_neg_cycle[dest] && !in_queue[dest]) {
                    queue[rear++] = dest;
                    in_queue[dest] = true;
                }
            }
        }
    }
    
    free(queue);
    free(in_queue);
    
    return r;
}

/* =============================================================================
 * GĂSIRE ȘI AFIȘARE CICLU DE ARBITRAJ
 * =============================================================================
 */

void find_arbitrage_cycle(FinancialNetwork* n, AnalysisResult* r) {
    if (!r->has_arbitrage) return;
    
    /* Recreăm Bellman-Ford pentru a găsi ciclul exact */
    double* dist = malloc(n->V * sizeof(double));
    int* prev = malloc(n->V * sizeof(int));
    
    for (int i = 0; i < n->V; i++) {
        dist[i] = 0;  /* Start de la toate nodurile */
        prev[i] = -1;
    }
    
    int cycle_node = -1;
    
    /* V iterații pentru a găsi un nod în ciclu */
    for (int i = 0; i < n->V; i++) {
        cycle_node = -1;
        for (int j = 0; j < n->E; j++) {
            int u = n->edges[j].src;
            int v = n->edges[j].dest;
            double w = n->edges[j].weight;
            
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                prev[v] = u;
                
                if (i == n->V - 1) {
                    cycle_node = v;
                }
            }
        }
    }
    
    if (cycle_node != -1) {
        /* Mergem V pași înapoi pentru a fi sigur că suntem în ciclu */
        for (int i = 0; i < n->V; i++) {
            cycle_node = prev[cycle_node];
        }
        
        printf("\n%s=== Ciclu de Arbitraj Detectat ===%s\n", RED, NC);
        printf("Ciclu: ");
        
        /* Construim ciclul */
        int cycle[100];
        int cycle_len = 0;
        int current = cycle_node;
        
        do {
            cycle[cycle_len++] = current;
            current = prev[current];
        } while (current != cycle_node && cycle_len < 100);
        
        /* Inversăm pentru ordine corectă */
        for (int i = 0; i < cycle_len / 2; i++) {
            int t = cycle[i];
            cycle[i] = cycle[cycle_len - 1 - i];
            cycle[cycle_len - 1 - i] = t;
        }
        
        /* Afișăm ciclul cu rate */
        double total_multiplier = 1.0;
        printf("%s", n->names[cycle[0]]);
        
        for (int i = 0; i < cycle_len; i++) {
            int from = cycle[i];
            int to = cycle[(i + 1) % cycle_len];
            
            /* Găsim rata */
            double rate = 0;
            for (int j = 0; j < n->E; j++) {
                if (n->edges[j].src == from && n->edges[j].dest == to) {
                    rate = n->edges[j].original_rate;
                    break;
                }
            }
            
            total_multiplier *= rate;
            printf(" --(%.4f)--> %s", rate, n->names[to]);
        }
        printf("\n");
        
        /* Calculăm profitul */
        double profit_percent = (total_multiplier - 1.0) * 100.0;
        printf("\nFactor multiplicare: %s%.6f%s\n", 
               profit_percent > 0 ? GREEN : RED, total_multiplier, NC);
        printf("Profit potential: %s%.4f%%%s per ciclu\n",
               profit_percent > 0 ? GREEN : RED, profit_percent, NC);
        
        if (profit_percent > 0) {
            printf("\n%s⚠️  OPORTUNITATE DE ARBITRAJ!%s\n", YELLOW, NC);
            printf("Tranzacționând 1000 unități prin acest ciclu:\n");
            printf("  Start: 1000.00 %s\n", n->names[cycle[0]]);
            printf("  Final: %.2f %s\n", 1000.0 * total_multiplier, n->names[cycle[0]]);
            printf("  Profit: %.2f %s\n", 1000.0 * (total_multiplier - 1.0), n->names[cycle[0]]);
        }
    }
    
    free(dist);
    free(prev);
}

/* =============================================================================
 * FUNCȚII DE AFIȘARE
 * =============================================================================
 */

void print_network_info(FinancialNetwork* n) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║           ANALIZĂ REȚEA FINANCIARĂ                            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    printf("%sCompanii/Valute (%d):%s\n", CYAN, n->V, NC);
    for (int i = 0; i < n->V; i++) {
        printf("  [%d] %s\n", i, n->names[i]);
    }
    
    printf("\n%sRate de schimb (%d):%s\n", CYAN, n->E, NC);
    printf("%-5s %-8s -> %-8s %12s %12s\n", "ID", "De la", "La", "Rata", "-log(Rata)");
    printf("─────────────────────────────────────────────────────────\n");
    
    for (int i = 0; i < n->E; i++) {
        printf("%-5d %-8s -> %-8s %12.4f %12.4f\n",
               i,
               n->names[n->edges[i].src],
               n->names[n->edges[i].dest],
               n->edges[i].original_rate,
               n->edges[i].weight);
    }
}

void print_analysis_results(FinancialNetwork* n, int src, AnalysisResult* r) {
    printf("\n%s=== Rezultate Bellman-Ford (start: %s) ===%s\n", 
           CYAN, n->names[src], NC);
    
    if (r->has_arbitrage) {
        printf("\n%s⚠️  DETECTAT CICLU NEGATIV (ARBITRAJ)!%s\n", RED, NC);
    } else {
        printf("\n%s✓ Nu există oportunități de arbitraj%s\n", GREEN, NC);
    }
    
    printf("\n%sDistanțe (în spațiul -log):%s\n", YELLOW, NC);
    for (int i = 0; i < n->V; i++) {
        printf("  %s: ", n->names[i]);
        if (r->in_neg_cycle[i]) {
            printf("%s-∞ (afectat de arbitraj)%s\n", RED, NC);
        } else if (r->dist[i] >= INF_D / 2) {
            printf("%sINF (inaccesibil)%s\n", MAGENTA, NC);
        } else {
            printf("%s%.4f%s\n", GREEN, r->dist[i], NC);
        }
    }
    
    /* Clasificare noduri */
    printf("\n%sClasificare noduri:%s\n", YELLOW, NC);
    
    printf("  Noduri afectate de instabilitate:\n");
    bool found_affected = false;
    for (int i = 0; i < n->V; i++) {
        if (r->in_neg_cycle[i]) {
            printf("    - %s\n", n->names[i]);
            found_affected = true;
        }
    }
    if (!found_affected) printf("    (niciuna)\n");
    
    printf("  Noduri stabile:\n");
    bool found_stable = false;
    for (int i = 0; i < n->V; i++) {
        if (!r->in_neg_cycle[i] && r->dist[i] < INF_D / 2) {
            printf("    - %s (dist: %.4f)\n", n->names[i], r->dist[i]);
            found_stable = true;
        }
    }
    if (!found_stable) printf("    (niciuna)\n");
}

void print_recommendations(AnalysisResult* r) {
    printf("\n%s=== Recomandări ===%s\n", CYAN, NC);
    
    if (r->has_arbitrage) {
        printf("• %sATENȚIE:%s Există oportunități de arbitraj în rețea\n", RED, NC);
        printf("• Verificați tranzacțiile din ciclurile detectate\n");
        printf("• Monitorizați nodurile afectate pentru volatilitate\n");
        printf("• Considerați ajustarea ratelor de schimb\n");
    } else {
        printf("• Rețeaua este stabilă - nu există oportunități de arbitraj\n");
        printf("• Toate tranzacțiile pot fi efectuate la rate echitabile\n");
        printf("• Monitorizarea periodică este recomandată\n");
    }
}

/* =============================================================================
 * PROGRAMUL PRINCIPAL
 * =============================================================================
 */

int main(void) {
    int N, M;
    
    /* Citim numărul de companii și rate */
    if (scanf("%d %d", &N, &M) != 2) {
        fprintf(stderr, "Eroare la citirea N și M!\n");
        return 1;
    }
    
    FinancialNetwork* net = network_create(N, M);
    
    /* Citim numele companiilor */
    for (int i = 0; i < N; i++) {
        char name[MAX_NAME];
        if (scanf("%31s", name) != 1) {
            fprintf(stderr, "Eroare la citirea numelui companiei %d!\n", i);
            network_destroy(net);
            return 1;
        }
        network_set_name(net, i, name);
    }
    
    /* Citim ratele de schimb */
    for (int i = 0; i < M; i++) {
        int u, v;
        double rate;
        
        if (scanf("%d %d %lf", &u, &v, &rate) != 3) {
            fprintf(stderr, "Eroare la citirea ratei %d!\n", i);
            network_destroy(net);
            return 1;
        }
        
        if (rate <= 0) {
            fprintf(stderr, "Rata de schimb trebuie să fie pozitivă: %f\n", rate);
            network_destroy(net);
            return 1;
        }
        
        network_add_rate(net, i, u, v, rate);
    }
    
    /* Citim nodul de start */
    int src;
    if (scanf("%d", &src) != 1) {
        fprintf(stderr, "Eroare la citirea nodului de start!\n");
        network_destroy(net);
        return 1;
    }
    
    /* Afișăm informații despre rețea */
    print_network_info(net);
    
    /* Rulăm analiza */
    AnalysisResult* result = detect_arbitrage(net, src);
    
    /* Afișăm rezultatele */
    print_analysis_results(net, src, result);
    
    /* Găsim și afișăm ciclul de arbitraj */
    find_arbitrage_cycle(net, result);
    
    /* Afișăm recomandări */
    print_recommendations(result);
    
    /* Cleanup */
    result_destroy(result);
    network_destroy(net);
    
    return 0;
}

/* =============================================================================
 * EXEMPLU DE FIȘIER DE TEST
 * =============================================================================
 * 
 * forex.txt:
 * 4 5
 * EUR
 * USD
 * GBP
 * JPY
 * 0 1 1.10
 * 1 2 0.85
 * 2 0 1.08
 * 0 3 130.0
 * 3 1 0.0077
 * 0
 * 
 * Ciclu arbitraj: EUR -> USD -> GBP -> EUR
 * 1.10 * 0.85 * 1.08 = 1.0098 (profit ~1%)
 * 
 * =============================================================================
 */

/* =============================================================================
 * NOTE PENTRU INSTRUCTORI
 * =============================================================================
 * 
 * Verificați:
 * 
 * 1. Transformarea corectă: weight = -log(rate)
 *    - Ciclu negativ în graful transformat = arbitraj
 * 
 * 2. Bellman-Ford implementat corect:
 *    - V-1 relaxări
 *    - Detectare ciclu prin iterația V
 * 
 * 3. Propagare BFS pentru nodurile afectate
 * 
 * 4. Calculul corect al profitului:
 *    - Produsul ratelor din ciclu
 *    - Profit = (produs - 1) * 100%
 * 
 * 5. Raport clar și informativ
 * 
 * Punctaje sugerate:
 * - Citire corectă: 6p
 * - Transformare -log: 6p
 * - Bellman-Ford: 12p
 * - Detectare ciclu: 10p
 * - Propagare BFS: 8p
 * - Afișare raport: 8p
 * 
 * =============================================================================
 */
