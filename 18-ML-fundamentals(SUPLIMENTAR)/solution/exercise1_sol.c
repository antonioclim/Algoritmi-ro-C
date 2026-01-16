/**
 * ============================================================================
 * SOLUȚIE: Exercițiu 1 - Segmentarea Clienților cu K-Means
 * Săptămâna 18: Fundamente Machine Learning în C
 * ============================================================================
 * 
 * Acest fișier conține soluția completă pentru exercițiul de clustering.
 * 
 * ATENȚIE STUDENȚI: Nu consultați acest fișier înainte de a încerca singuri!
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <float.h>

// ============================================================================
// CONSTANTE ȘI CONFIGURARE
// ============================================================================

#define MAX_LINE_LENGTH 256
#define MAX_CUSTOMERS 1000
#define MAX_ITERATIONS 100
#define CONVERGENCE_THRESHOLD 1e-6

// ============================================================================
// STRUCTURI DE DATE
// ============================================================================

typedef struct {
    char customer_id[16];
    double annual_income;
    double spending_score;
    double age;
    double visits_per_month;
} CustomerData;

typedef struct {
    double *centroid;       // Coordonatele centroidului
    int *members;           // Indicii membrilor clusterului
    int member_count;       // Numărul de membri
    int capacity;           // Capacitatea alocată pentru membri
} Cluster;

typedef struct {
    int k;                  // Numărul de clustere
    int n_features;         // Numărul de caracteristici
    Cluster *clusters;      // Array de clustere
    double inertia;         // WCSS (Within-Cluster Sum of Squares)
    int n_iterations;       // Iterații până la convergență
} KMeansModel;

// ============================================================================
// PARTEA 1: FUNCȚII UTILITARE
// ============================================================================

/**
 * Calculează distanța Euclidiană între doi vectori.
 */
double euclidean_distance(double *v1, double *v2, int n) {
    double sum = 0.0;
    for (int i = 0; i < n; i++) {
        double diff = v1[i] - v2[i];
        sum += diff * diff;
    }
    return sqrt(sum);
}

/**
 * Generează un număr random între min și max.
 */
double random_double(double min, double max) {
    return min + (double)rand() / RAND_MAX * (max - min);
}

// ============================================================================
// PARTEA 2: ÎNCĂRCARE DATE
// ============================================================================

/**
 * SOLUȚIE: Încarcă datele din fișier CSV.
 */
int load_customer_data(const char *filename, CustomerData *customers, int max_count) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("EROARE: Nu s-a putut deschide %s\n", filename);
        return -1;
    }
    
    char line[MAX_LINE_LENGTH];
    int count = 0;
    int line_num = 0;
    
    while (fgets(line, sizeof(line), file) && count < max_count) {
        line_num++;
        
        // Ignoră comentarii și linii goale
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') {
            continue;
        }
        
        // Ignoră header
        if (line_num == 1 && strstr(line, "customer_id") != NULL) {
            continue;
        }
        
        // Parse CSV
        int parsed = sscanf(line, "%[^,],%lf,%lf,%lf,%lf",
                           customers[count].customer_id,
                           &customers[count].annual_income,
                           &customers[count].spending_score,
                           &customers[count].age,
                           &customers[count].visits_per_month);
        
        if (parsed >= 3) {  // Minimum: id, income, spending
            count++;
        }
    }
    
    fclose(file);
    printf("INFO: Încărcate %d înregistrări din %s\n", count, filename);
    return count;
}

// ============================================================================
// PARTEA 3: NORMALIZARE
// ============================================================================

/**
 * SOLUȚIE: Normalizare Min-Max pe coloane.
 */
void normalize_minmax(double **data, int n, int d, 
                      double *min_vals, double *max_vals) {
    // Calculează min și max pentru fiecare coloană
    for (int j = 0; j < d; j++) {
        min_vals[j] = data[0][j];
        max_vals[j] = data[0][j];
        
        for (int i = 1; i < n; i++) {
            if (data[i][j] < min_vals[j]) min_vals[j] = data[i][j];
            if (data[i][j] > max_vals[j]) max_vals[j] = data[i][j];
        }
    }
    
    // Normalizează
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < d; j++) {
            double range = max_vals[j] - min_vals[j];
            if (range > 1e-10) {
                data[i][j] = (data[i][j] - min_vals[j]) / range;
            } else {
                data[i][j] = 0.5;  // Dacă range=0, setăm la 0.5
            }
        }
    }
}

// ============================================================================
// PARTEA 4: K-MEANS++ INITIALIZATION
// ============================================================================

/**
 * SOLUȚIE: Inițializare K-Means++ pentru alegerea inteligentă a centroizilor.
 * 
 * Algoritm:
 * 1. Alege primul centroid random
 * 2. Pentru fiecare centroid următor:
 *    a) Calculează D(x)² = distanța minimă la cel mai apropiat centroid existent
 *    b) Alege următorul centroid cu probabilitate proporțională cu D(x)²
 * 
 * Avantaj: Evită inițializări proaste, convergență mai rapidă
 */
void kmeans_init_plusplus(double **data, int n, int d, int k, double **centroids) {
    // Pas 1: Primul centroid - aleator uniform
    int first_idx = rand() % n;
    for (int j = 0; j < d; j++) {
        centroids[0][j] = data[first_idx][j];
    }
    
    // Array pentru distanțe minime
    double *min_distances = (double *)malloc(n * sizeof(double));
    
    // Pas 2: Restul centroizilor
    for (int c = 1; c < k; c++) {
        double total_dist_sq = 0.0;
        
        // Calculează distanța minimă la centroizii existenți
        for (int i = 0; i < n; i++) {
            double min_dist = DBL_MAX;
            
            for (int existing = 0; existing < c; existing++) {
                double dist = euclidean_distance(data[i], centroids[existing], d);
                if (dist < min_dist) {
                    min_dist = dist;
                }
            }
            
            min_distances[i] = min_dist * min_dist;  // D(x)²
            total_dist_sq += min_distances[i];
        }
        
        // Selecție probabilistică (roulette wheel)
        double threshold = random_double(0, total_dist_sq);
        double cumsum = 0.0;
        int selected = 0;
        
        for (int i = 0; i < n; i++) {
            cumsum += min_distances[i];
            if (cumsum >= threshold) {
                selected = i;
                break;
            }
        }
        
        // Copiază punctul selectat ca nou centroid
        for (int j = 0; j < d; j++) {
            centroids[c][j] = data[selected][j];
        }
    }
    
    free(min_distances);
}

// ============================================================================
// PARTEA 5: ASIGNARE CLUSTERE
// ============================================================================

/**
 * SOLUȚIE: Asignează fiecare punct la cel mai apropiat centroid.
 * 
 * @return 1 dacă asignările s-au schimbat, 0 altfel
 */
int kmeans_assign(double **data, int n, int d, 
                  double **centroids, int k, int *assignments) {
    int changed = 0;
    
    for (int i = 0; i < n; i++) {
        double min_dist = DBL_MAX;
        int best_cluster = 0;
        
        // Găsește cel mai apropiat centroid
        for (int c = 0; c < k; c++) {
            double dist = euclidean_distance(data[i], centroids[c], d);
            if (dist < min_dist) {
                min_dist = dist;
                best_cluster = c;
            }
        }
        
        // Verifică dacă asignarea s-a schimbat
        if (assignments[i] != best_cluster) {
            changed = 1;
            assignments[i] = best_cluster;
        }
    }
    
    return changed;
}

// ============================================================================
// PARTEA 6: UPDATE CENTROIZI
// ============================================================================

/**
 * SOLUȚIE: Recalculează centroizii ca media membrilor.
 */
void kmeans_update_centroids(double **data, int n, int d,
                             int *assignments, int k, double **centroids) {
    // Resetează centroizii și contoarele
    int *counts = (int *)calloc(k, sizeof(int));
    
    for (int c = 0; c < k; c++) {
        for (int j = 0; j < d; j++) {
            centroids[c][j] = 0.0;
        }
    }
    
    // Sumă
    for (int i = 0; i < n; i++) {
        int c = assignments[i];
        counts[c]++;
        for (int j = 0; j < d; j++) {
            centroids[c][j] += data[i][j];
        }
    }
    
    // Medie
    for (int c = 0; c < k; c++) {
        if (counts[c] > 0) {
            for (int j = 0; j < d; j++) {
                centroids[c][j] /= counts[c];
            }
        }
    }
    
    free(counts);
}

// ============================================================================
// PARTEA 7: ALGORITM COMPLET K-MEANS
// ============================================================================

/**
 * SOLUȚIE: Algoritmul K-Means complet (Lloyd's Algorithm).
 */
double kmeans_fit(double **data, int n, int d, int k, 
                  double **centroids, int *assignments, int *iterations) {
    // Inițializare K-Means++
    kmeans_init_plusplus(data, n, d, k, centroids);
    
    // Inițializare asignări
    for (int i = 0; i < n; i++) {
        assignments[i] = -1;
    }
    
    int iter = 0;
    int changed = 1;
    
    // Bucla principală Lloyd's Algorithm
    while (changed && iter < MAX_ITERATIONS) {
        // Pas 1: Asignare
        changed = kmeans_assign(data, n, d, centroids, k, assignments);
        
        // Pas 2: Update centroizi
        kmeans_update_centroids(data, n, d, assignments, k, centroids);
        
        iter++;
    }
    
    *iterations = iter;
    
    // Calculează inertia (WCSS)
    double inertia = 0.0;
    for (int i = 0; i < n; i++) {
        double dist = euclidean_distance(data[i], centroids[assignments[i]], d);
        inertia += dist * dist;
    }
    
    return inertia;
}

// ============================================================================
// PARTEA 8: ELBOW METHOD
// ============================================================================

/**
 * Rulează Elbow Method pentru a determina k optim.
 */
void elbow_method(double **data, int n, int d, int max_k) {
    printf("\n╔════════════════════════════════════════════════════╗\n");
    printf("║           ELBOW METHOD - Selecție K Optim          ║\n");
    printf("╚════════════════════════════════════════════════════╝\n\n");
    
    double *inertias = (double *)malloc(max_k * sizeof(double));
    
    for (int k = 1; k <= max_k; k++) {
        // Alocă memorie pentru acest k
        double **centroids = (double **)malloc(k * sizeof(double *));
        for (int c = 0; c < k; c++) {
            centroids[c] = (double *)malloc(d * sizeof(double));
        }
        int *assignments = (int *)malloc(n * sizeof(int));
        int iterations;
        
        // Rulează K-Means
        double inertia = kmeans_fit(data, n, d, k, centroids, assignments, &iterations);
        inertias[k-1] = inertia;
        
        // Afișează rezultat
        printf("  k=%d: Inertia=%.2f, Iterații=%d ", k, inertia, iterations);
        
        // Grafic ASCII simplu
        int bar_length = (int)(inertia / 10);
        if (bar_length > 50) bar_length = 50;
        printf("[");
        for (int b = 0; b < bar_length; b++) printf("█");
        for (int b = bar_length; b < 50; b++) printf(" ");
        printf("]\n");
        
        // Eliberează memorie
        for (int c = 0; c < k; c++) free(centroids[c]);
        free(centroids);
        free(assignments);
    }
    
    // Detectează "cotul" (punct de inflexiune)
    printf("\n  Analiză diferențe (|ΔInertia|):\n");
    int best_k = 2;
    double max_decrease_ratio = 0;
    
    for (int k = 2; k < max_k; k++) {
        double decrease_before = inertias[k-2] - inertias[k-1];
        double decrease_after = inertias[k-1] - inertias[k];
        
        if (decrease_before > 0) {
            double ratio = decrease_before / (decrease_after + 1e-10);
            printf("    k=%d→%d: Δ=%.2f, k=%d→%d: Δ=%.2f, Ratio=%.2f\n",
                   k-1, k, decrease_before, k, k+1, decrease_after, ratio);
            
            if (ratio > max_decrease_ratio) {
                max_decrease_ratio = ratio;
                best_k = k;
            }
        }
    }
    
    printf("\n  ➤ K optim sugerat: %d (punct de inflexiune)\n", best_k);
    
    free(inertias);
}

// ============================================================================
// PARTEA 9: PROFILARE CLUSTERE
// ============================================================================

/**
 * Generează profile pentru fiecare cluster.
 */
void profile_clusters(CustomerData *customers, int n, int *assignments, int k,
                      double *income_min, double *income_max,
                      double *spend_min, double *spend_max) {
    printf("\n╔════════════════════════════════════════════════════╗\n");
    printf("║              PROFILARE CLUSTERE                    ║\n");
    printf("╚════════════════════════════════════════════════════╝\n\n");
    
    for (int c = 0; c < k; c++) {
        // Calculează statistici pentru cluster
        double sum_income = 0, sum_spend = 0, sum_age = 0, sum_visits = 0;
        int count = 0;
        
        for (int i = 0; i < n; i++) {
            if (assignments[i] == c) {
                sum_income += customers[i].annual_income;
                sum_spend += customers[i].spending_score;
                sum_age += customers[i].age;
                sum_visits += customers[i].visits_per_month;
                count++;
            }
        }
        
        if (count == 0) continue;
        
        double avg_income = sum_income / count;
        double avg_spend = sum_spend / count;
        double avg_age = sum_age / count;
        double avg_visits = sum_visits / count;
        
        // Denormalizare pentru interpretare
        double real_income = avg_income * (income_max[0] - income_min[0]) + income_min[0];
        double real_spend = avg_spend * (spend_max[0] - spend_min[0]) + spend_min[0];
        
        // Determinare nume descriptiv
        const char *segment_name;
        if (avg_income > 0.7 && avg_spend > 0.7) {
            segment_name = "🌟 Premium VIP";
        } else if (avg_income < 0.4 && avg_spend > 0.6) {
            segment_name = "🛍️  Young Enthusiasts";
        } else if (avg_income > 0.5 && avg_spend < 0.4) {
            segment_name = "💰 Budget Savers";
        } else {
            segment_name = "👤 Standard Customers";
        }
        
        printf("  ┌─────────────────────────────────────────────────┐\n");
        printf("  │ Cluster %d: %-35s │\n", c, segment_name);
        printf("  ├─────────────────────────────────────────────────┤\n");
        printf("  │ Membri:          %-6d (%5.1f%% din total)      │\n", 
               count, 100.0 * count / n);
        printf("  │ Venit mediu:     %-6.1fk RON                    │\n", real_income);
        printf("  │ Spending score:  %-6.1f                         │\n", real_spend);
        printf("  │ Vârstă medie:    %-6.1f ani                     │\n", avg_age);
        printf("  │ Vizite/lună:     %-6.1f                         │\n", avg_visits);
        printf("  └─────────────────────────────────────────────────┘\n\n");
    }
}

// ============================================================================
// FUNCȚIA PRINCIPALĂ
// ============================================================================

int main(int argc, char *argv[]) {
    srand(42);  // Seed fix pentru reproducibilitate
    
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║     SOLUȚIE: Segmentarea Clienților cu K-Means                 ║\n");
    printf("║     Săptămâna 18 - Machine Learning Fundamentals               ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n");
    
    // === 1. Încărcare date ===
    const char *filename = (argc > 1) ? argv[1] : "data/customers.csv";
    CustomerData *customers = (CustomerData *)malloc(MAX_CUSTOMERS * sizeof(CustomerData));
    
    int n = load_customer_data(filename, customers, MAX_CUSTOMERS);
    if (n <= 0) {
        printf("EROARE: Nu s-au putut încărca date!\n");
        free(customers);
        return 1;
    }
    
    // === 2. Pregătire date pentru clustering ===
    int d = 2;  // Folosim doar income și spending score
    
    double **data = (double **)malloc(n * sizeof(double *));
    for (int i = 0; i < n; i++) {
        data[i] = (double *)malloc(d * sizeof(double));
        data[i][0] = customers[i].annual_income;
        data[i][1] = customers[i].spending_score;
    }
    
    // Salvează valorile originale pentru denormalizare
    double income_min, income_max, spend_min, spend_max;
    income_min = income_max = data[0][0];
    spend_min = spend_max = data[0][1];
    for (int i = 1; i < n; i++) {
        if (data[i][0] < income_min) income_min = data[i][0];
        if (data[i][0] > income_max) income_max = data[i][0];
        if (data[i][1] < spend_min) spend_min = data[i][1];
        if (data[i][1] > spend_max) spend_max = data[i][1];
    }
    
    // === 3. Normalizare ===
    double *min_vals = (double *)malloc(d * sizeof(double));
    double *max_vals = (double *)malloc(d * sizeof(double));
    normalize_minmax(data, n, d, min_vals, max_vals);
    
    printf("\n✓ Date normalizate (Min-Max scaling)\n");
    
    // === 4. Elbow Method ===
    elbow_method(data, n, d, 7);
    
    // === 5. K-Means cu k=4 (optim pentru aceste date) ===
    int k = 4;
    printf("\n\n═══════════════════════════════════════════════════════\n");
    printf("  Rulare K-Means cu k=%d\n", k);
    printf("═══════════════════════════════════════════════════════\n");
    
    double **centroids = (double **)malloc(k * sizeof(double *));
    for (int c = 0; c < k; c++) {
        centroids[c] = (double *)malloc(d * sizeof(double));
    }
    int *assignments = (int *)malloc(n * sizeof(int));
    int iterations;
    
    double inertia = kmeans_fit(data, n, d, k, centroids, assignments, &iterations);
    
    printf("\n  Rezultate:\n");
    printf("  • Inertia finală (WCSS): %.4f\n", inertia);
    printf("  • Iterații: %d\n", iterations);
    
    printf("\n  Centroizi (normalizați):\n");
    for (int c = 0; c < k; c++) {
        int count = 0;
        for (int i = 0; i < n; i++) {
            if (assignments[i] == c) count++;
        }
        printf("    Cluster %d: (%.3f, %.3f) - %d membri\n", 
               c, centroids[c][0], centroids[c][1], count);
    }
    
    // === 6. Profilare clustere ===
    profile_clusters(customers, n, assignments, k, 
                    &income_min, &income_max, &spend_min, &spend_max);
    
    // === 7. Eliberare memorie ===
    for (int i = 0; i < n; i++) free(data[i]);
    free(data);
    for (int c = 0; c < k; c++) free(centroids[c]);
    free(centroids);
    free(assignments);
    free(min_vals);
    free(max_vals);
    free(customers);
    
    printf("\n✅ Segmentare completă!\n\n");
    
    return 0;
}
