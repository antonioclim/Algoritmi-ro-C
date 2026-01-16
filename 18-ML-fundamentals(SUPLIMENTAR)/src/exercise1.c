/**
 * =============================================================================
 * EXERCIȚIUL 1: Customer Segmentation cu K-Means
 * =============================================================================
 *
 * OBIECTIV:
 *   Implementați segmentarea clienților folosind algoritmul K-Means cu 
 *   inițializare K-Means++. Veți grupa clienții în segmente bazate pe 
 *   comportamentul lor de cumpărare.
 *
 * CERINȚE:
 *   1. Citiți datele clienților din fișierul CSV furnizat
 *   2. Implementați normalizarea Min-Max pentru features
 *   3. Implementați algoritmul K-Means cu K-Means++ initialization
 *   4. Folosiți metoda Elbow pentru a determina k optim
 *   5. Afișați profilul fiecărui cluster identificat
 *
 * EXEMPLU INPUT (customers.csv):
 *   customer_id,annual_income,purchase_frequency,total_value
 *   1,45000,12,2500
 *   2,85000,24,8000
 *   ...
 *
 * EXPECTED OUTPUT:
 *   Metoda Elbow:
 *     k=2: Inerție = ...
 *     k=3: Inerție = ...
 *     ...
 *   k optim sugerat: 3
 *   
 *   Cluster 0 (n=15): "Cumpărători ocazionali"
 *     - Venit mediu: 35000
 *     - Frecvență medie: 8
 *     - Valoare medie: 1200
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -o exercise1 exercise1.c -lm
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <float.h>

/* =============================================================================
 * DEFINIREA TIPURILOR
 * =============================================================================
 */

/**
 * TODO 1: Definește structura pentru stocarea datelor clienților
 *
 * Hint: Ai nevoie de o matrice pentru features și un array pentru ID-uri
 */
typedef struct {
    /* YOUR CODE HERE */
    int *customer_ids;      /* ID-urile clienților */
    double *annual_income;  /* Venit anual */
    double *frequency;      /* Frecvența achizițiilor */
    double *total_value;    /* Valoare totală achiziții */
    size_t count;           /* Numărul de clienți */
} CustomerData;

/**
 * TODO 2: Definește structura pentru modelul K-Means
 *
 * Hint: Ai nevoie de centroizi, asignări, parametri
 */
typedef struct {
    /* YOUR CODE HERE */
    double **centroids;     /* Centroizii (k × 3 features) */
    int *assignments;       /* Clusterul fiecărui punct */
    size_t k;               /* Număr de clustere */
    size_t n_features;      /* Număr de features (3) */
    double inertia;         /* Within-cluster sum of squares */
} KMeansModel;

/**
 * Structură pentru statistici cluster
 */
typedef struct {
    int cluster_id;
    int count;
    double mean_income;
    double mean_frequency;
    double mean_value;
    char profile_name[64];
} ClusterProfile;

/* =============================================================================
 * FUNCȚII DE CITIRE DATE
 * =============================================================================
 */

/**
 * TODO 3: Implementează citirea datelor din CSV
 *
 * @param filename Calea către fișierul CSV
 * @return Pointer la structura CustomerData populată sau NULL în caz de eroare
 *
 * Pași:
 *   1. Deschide fișierul
 *   2. Numără liniile (excluzând header-ul)
 *   3. Alocă memorie
 *   4. Parsează fiecare linie
 *   5. Închide fișierul
 */
CustomerData* load_customer_data(const char *filename) {
    /* YOUR CODE HERE */
    
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Eroare: Nu pot deschide %s\n", filename);
        return NULL;
    }
    
    /* Numără liniile */
    char line[256];
    int count = -1;  /* -1 pentru header */
    while (fgets(line, sizeof(line), file)) {
        count++;
    }
    
    if (count <= 0) {
        fclose(file);
        return NULL;
    }
    
    /* Alocă structura */
    CustomerData *data = (CustomerData*)malloc(sizeof(CustomerData));
    data->count = count;
    data->customer_ids = (int*)malloc(count * sizeof(int));
    data->annual_income = (double*)malloc(count * sizeof(double));
    data->frequency = (double*)malloc(count * sizeof(double));
    data->total_value = (double*)malloc(count * sizeof(double));
    
    /* Revino la început și citește datele */
    rewind(file);
    fgets(line, sizeof(line), file);  /* Skip header */
    
    for (int i = 0; i < count; i++) {
        if (fgets(line, sizeof(line), file)) {
            sscanf(line, "%d,%lf,%lf,%lf",
                   &data->customer_ids[i],
                   &data->annual_income[i],
                   &data->frequency[i],
                   &data->total_value[i]);
        }
    }
    
    fclose(file);
    return data;
}

/**
 * Eliberează memoria datelor clienților
 */
void free_customer_data(CustomerData *data) {
    if (data) {
        free(data->customer_ids);
        free(data->annual_income);
        free(data->frequency);
        free(data->total_value);
        free(data);
    }
}

/* =============================================================================
 * FUNCȚII DE NORMALIZARE
 * =============================================================================
 */

/**
 * TODO 4: Implementează normalizarea Min-Max
 *
 * Formula: x_norm = (x - min) / (max - min)
 *
 * @param data Array-ul de date
 * @param n Numărul de elemente
 * @param normalized Array-ul unde se stochează valorile normalizate (pre-alocat)
 */
void normalize_minmax(const double *data, size_t n, double *normalized) {
    /* YOUR CODE HERE */
    
    double min_val = DBL_MAX;
    double max_val = -DBL_MAX;
    
    /* Găsește min și max */
    for (size_t i = 0; i < n; i++) {
        if (data[i] < min_val) min_val = data[i];
        if (data[i] > max_val) max_val = data[i];
    }
    
    /* Normalizează */
    double range = max_val - min_val;
    if (range < 1e-10) range = 1.0;  /* Evită diviziunea cu 0 */
    
    for (size_t i = 0; i < n; i++) {
        normalized[i] = (data[i] - min_val) / range;
    }
}

/* =============================================================================
 * FUNCȚII K-MEANS
 * =============================================================================
 */

/**
 * Calculează distanța Euclidiană între două puncte
 */
double euclidean_distance(const double *a, const double *b, size_t dim) {
    double sum = 0.0;
    for (size_t i = 0; i < dim; i++) {
        double diff = a[i] - b[i];
        sum += diff * diff;
    }
    return sqrt(sum);
}

/**
 * TODO 5: Implementează crearea modelului K-Means
 */
KMeansModel* kmeans_create(size_t k, size_t n_features) {
    /* YOUR CODE HERE */
    
    KMeansModel *model = (KMeansModel*)malloc(sizeof(KMeansModel));
    if (!model) return NULL;
    
    model->k = k;
    model->n_features = n_features;
    model->inertia = 0.0;
    model->assignments = NULL;
    
    /* Alocă centroizii */
    model->centroids = (double**)malloc(k * sizeof(double*));
    for (size_t i = 0; i < k; i++) {
        model->centroids[i] = (double*)calloc(n_features, sizeof(double));
    }
    
    return model;
}

/**
 * Eliberează modelul K-Means
 */
void kmeans_free(KMeansModel *model) {
    if (model) {
        for (size_t i = 0; i < model->k; i++) {
            free(model->centroids[i]);
        }
        free(model->centroids);
        free(model->assignments);
        free(model);
    }
}

/**
 * TODO 6: Implementează inițializarea K-Means++
 *
 * Pași:
 *   1. Alege primul centroid aleator
 *   2. Pentru fiecare centroid următor:
 *      a. Calculează D(x)² - distanța minimă la centroizii existenți
 *      b. Alege noul centroid proporțional cu D(x)²
 *
 * @param model Modelul K-Means
 * @param X Matricea de features (n × 3), reprezentată ca n puncte
 * @param n Numărul de puncte
 */
void kmeans_init_plusplus(KMeansModel *model, double **X, size_t n) {
    /* YOUR CODE HERE */
    
    double *min_distances = (double*)malloc(n * sizeof(double));
    
    /* Primul centroid aleator */
    size_t first = rand() % n;
    for (size_t j = 0; j < model->n_features; j++) {
        model->centroids[0][j] = X[first][j];
    }
    
    /* Inițializează distanțele */
    for (size_t i = 0; i < n; i++) {
        min_distances[i] = euclidean_distance(X[i], model->centroids[0], model->n_features);
        min_distances[i] *= min_distances[i];
    }
    
    /* Alege restul centroizilor */
    for (size_t c = 1; c < model->k; c++) {
        /* Calculează suma totală */
        double total = 0.0;
        for (size_t i = 0; i < n; i++) {
            total += min_distances[i];
        }
        
        /* Alege proporțional */
        double r = ((double)rand() / RAND_MAX) * total;
        double cumsum = 0.0;
        size_t chosen = 0;
        
        for (size_t i = 0; i < n; i++) {
            cumsum += min_distances[i];
            if (cumsum >= r) {
                chosen = i;
                break;
            }
        }
        
        /* Setează centroidul */
        for (size_t j = 0; j < model->n_features; j++) {
            model->centroids[c][j] = X[chosen][j];
        }
        
        /* Actualizează distanțele */
        for (size_t i = 0; i < n; i++) {
            double dist = euclidean_distance(X[i], model->centroids[c], model->n_features);
            dist *= dist;
            if (dist < min_distances[i]) {
                min_distances[i] = dist;
            }
        }
    }
    
    free(min_distances);
}

/**
 * TODO 7: Implementează pasul de asignare
 *
 * Asignează fiecare punct la cel mai apropiat centroid
 *
 * @return true dacă asignările s-au schimbat
 */
bool kmeans_assign(KMeansModel *model, double **X, size_t n) {
    /* YOUR CODE HERE */
    
    bool changed = false;
    model->inertia = 0.0;
    
    for (size_t i = 0; i < n; i++) {
        double min_dist = DBL_MAX;
        int best = 0;
        
        for (size_t c = 0; c < model->k; c++) {
            double dist = euclidean_distance(X[i], model->centroids[c], model->n_features);
            if (dist < min_dist) {
                min_dist = dist;
                best = (int)c;
            }
        }
        
        if (model->assignments[i] != best) {
            changed = true;
            model->assignments[i] = best;
        }
        
        model->inertia += min_dist * min_dist;
    }
    
    return changed;
}

/**
 * TODO 8: Implementează pasul de actualizare a centroizilor
 */
void kmeans_update_centroids(KMeansModel *model, double **X, size_t n) {
    /* YOUR CODE HERE */
    
    /* Resetează centroizii */
    for (size_t c = 0; c < model->k; c++) {
        for (size_t j = 0; j < model->n_features; j++) {
            model->centroids[c][j] = 0.0;
        }
    }
    
    /* Contorizează și sumează */
    int *counts = (int*)calloc(model->k, sizeof(int));
    
    for (size_t i = 0; i < n; i++) {
        int c = model->assignments[i];
        counts[c]++;
        for (size_t j = 0; j < model->n_features; j++) {
            model->centroids[c][j] += X[i][j];
        }
    }
    
    /* Media */
    for (size_t c = 0; c < model->k; c++) {
        if (counts[c] > 0) {
            for (size_t j = 0; j < model->n_features; j++) {
                model->centroids[c][j] /= counts[c];
            }
        }
    }
    
    free(counts);
}

/**
 * TODO 9: Implementează antrenarea completă K-Means
 */
void kmeans_fit(KMeansModel *model, double **X, size_t n, size_t max_iter) {
    /* YOUR CODE HERE */
    
    /* Alocă asignări */
    model->assignments = (int*)malloc(n * sizeof(int));
    for (size_t i = 0; i < n; i++) {
        model->assignments[i] = -1;
    }
    
    /* Inițializare K-Means++ */
    kmeans_init_plusplus(model, X, n);
    
    /* Iterații Lloyd */
    for (size_t iter = 0; iter < max_iter; iter++) {
        bool changed = kmeans_assign(model, X, n);
        
        if (!changed) {
            break;
        }
        
        kmeans_update_centroids(model, X, n);
    }
}

/* =============================================================================
 * FUNCȚII DE ANALIZĂ
 * =============================================================================
 */

/**
 * TODO 10: Implementează metoda Elbow
 *
 * Rulează K-Means pentru k de la 1 la max_k și afișează inerția
 */
void elbow_method(double **X, size_t n, size_t max_k) {
    /* YOUR CODE HERE */
    
    printf("\nMetoda Elbow pentru selectarea k optim:\n");
    printf("────────────────────────────────────────\n");
    
    double prev_inertia = 0.0;
    double max_drop = 0.0;
    size_t best_k = 2;
    
    for (size_t k = 1; k <= max_k; k++) {
        KMeansModel *model = kmeans_create(k, 3);
        kmeans_fit(model, X, n, 100);
        
        printf("  k=%zu: Inerție = %.2f", k, model->inertia);
        
        if (k > 1) {
            double drop = prev_inertia - model->inertia;
            double drop_pct = (drop / prev_inertia) * 100;
            printf(" (scădere: %.1f%%)", drop_pct);
            
            if (k == 2 || drop > max_drop * 0.5) {
                max_drop = drop;
                best_k = k;
            }
        }
        printf("\n");
        
        prev_inertia = model->inertia;
        kmeans_free(model);
    }
    
    printf("\n→ k optim sugerat: %zu\n", best_k);
}

/**
 * TODO 11: Implementează calcularea profilului clusterelor
 */
void calculate_cluster_profiles(KMeansModel *model, CustomerData *data, 
                                 double **X_original, ClusterProfile *profiles) {
    /* YOUR CODE HERE */
    
    /* Inițializează profilele */
    for (size_t c = 0; c < model->k; c++) {
        profiles[c].cluster_id = (int)c;
        profiles[c].count = 0;
        profiles[c].mean_income = 0.0;
        profiles[c].mean_frequency = 0.0;
        profiles[c].mean_value = 0.0;
    }
    
    /* Sumează valorile */
    for (size_t i = 0; i < data->count; i++) {
        int c = model->assignments[i];
        profiles[c].count++;
        profiles[c].mean_income += data->annual_income[i];
        profiles[c].mean_frequency += data->frequency[i];
        profiles[c].mean_value += data->total_value[i];
    }
    
    /* Calculează medii și atribuie nume */
    for (size_t c = 0; c < model->k; c++) {
        if (profiles[c].count > 0) {
            profiles[c].mean_income /= profiles[c].count;
            profiles[c].mean_frequency /= profiles[c].count;
            profiles[c].mean_value /= profiles[c].count;
            
            /* Atribuie nume bazat pe caracteristici */
            if (profiles[c].mean_income > 70000 && profiles[c].mean_value > 5000) {
                strcpy(profiles[c].profile_name, "Premium - Valoare Mare");
            } else if (profiles[c].mean_frequency > 15) {
                strcpy(profiles[c].profile_name, "Loiali - Frecvență Mare");
            } else if (profiles[c].mean_income < 40000) {
                strcpy(profiles[c].profile_name, "Economici - Buget Redus");
            } else {
                strcpy(profiles[c].profile_name, "Standard - Mediu");
            }
        }
    }
}

/**
 * Afișează profilele clusterelor
 */
void print_cluster_profiles(ClusterProfile *profiles, size_t k) {
    printf("\n╔═════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                        PROFILURI CLUSTERE                               ║\n");
    printf("╠═════════════════════════════════════════════════════════════════════════╣\n");
    
    for (size_t c = 0; c < k; c++) {
        printf("║                                                                         ║\n");
        printf("║  Cluster %d: \"%s\"                                 \n", 
               profiles[c].cluster_id, profiles[c].profile_name);
        printf("║  ───────────────────────────────────────────────────────────────────── ║\n");
        printf("║    • Număr clienți: %d                                                  \n", 
               profiles[c].count);
        printf("║    • Venit mediu: %.0f EUR                                              \n", 
               profiles[c].mean_income);
        printf("║    • Frecvență medie: %.1f achiziții/an                                 \n", 
               profiles[c].mean_frequency);
        printf("║    • Valoare medie: %.0f EUR                                            \n", 
               profiles[c].mean_value);
    }
    
    printf("╚═════════════════════════════════════════════════════════════════════════╝\n");
}

/* =============================================================================
 * MAIN
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    const char *filename = "data/customers.csv";
    
    if (argc > 1) {
        filename = argv[1];
    }
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║     EXERCIȚIUL 1: Customer Segmentation cu K-Means                            ║\n");
    printf("║     Săptămâna 18 - Machine Learning în C                                      ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    /* Setează seed pentru reproducibilitate */
    srand(42);
    
    /* Încarcă datele */
    printf("Încărcare date din: %s\n", filename);
    CustomerData *data = load_customer_data(filename);
    
    if (!data) {
        /* Generează date de test dacă fișierul nu există */
        printf("Fișierul nu există. Generare date de test...\n\n");
        
        data = (CustomerData*)malloc(sizeof(CustomerData));
        data->count = 50;
        data->customer_ids = (int*)malloc(50 * sizeof(int));
        data->annual_income = (double*)malloc(50 * sizeof(double));
        data->frequency = (double*)malloc(50 * sizeof(double));
        data->total_value = (double*)malloc(50 * sizeof(double));
        
        /* Generează 3 clustere distincte */
        for (int i = 0; i < 15; i++) {
            data->customer_ids[i] = i + 1;
            data->annual_income[i] = 30000 + rand() % 15000;
            data->frequency[i] = 5 + rand() % 8;
            data->total_value[i] = 800 + rand() % 1500;
        }
        for (int i = 15; i < 35; i++) {
            data->customer_ids[i] = i + 1;
            data->annual_income[i] = 50000 + rand() % 20000;
            data->frequency[i] = 12 + rand() % 10;
            data->total_value[i] = 3000 + rand() % 3000;
        }
        for (int i = 35; i < 50; i++) {
            data->customer_ids[i] = i + 1;
            data->annual_income[i] = 85000 + rand() % 30000;
            data->frequency[i] = 20 + rand() % 15;
            data->total_value[i] = 8000 + rand() % 7000;
        }
    }
    
    printf("Clienți încărcați: %zu\n", data->count);
    
    /* Normalizează datele */
    double *norm_income = (double*)malloc(data->count * sizeof(double));
    double *norm_freq = (double*)malloc(data->count * sizeof(double));
    double *norm_value = (double*)malloc(data->count * sizeof(double));
    
    normalize_minmax(data->annual_income, data->count, norm_income);
    normalize_minmax(data->frequency, data->count, norm_freq);
    normalize_minmax(data->total_value, data->count, norm_value);
    
    /* Construiește matricea de features */
    double **X = (double**)malloc(data->count * sizeof(double*));
    for (size_t i = 0; i < data->count; i++) {
        X[i] = (double*)malloc(3 * sizeof(double));
        X[i][0] = norm_income[i];
        X[i][1] = norm_freq[i];
        X[i][2] = norm_value[i];
    }
    
    /* Metoda Elbow */
    elbow_method(X, data->count, 6);
    
    /* Antrenare finală cu k=3 */
    printf("\nAntrenare K-Means cu k=3...\n");
    KMeansModel *model = kmeans_create(3, 3);
    kmeans_fit(model, X, data->count, 100);
    printf("Inerție finală: %.4f\n", model->inertia);
    
    /* Calculează și afișează profilele */
    ClusterProfile *profiles = (ClusterProfile*)malloc(3 * sizeof(ClusterProfile));
    calculate_cluster_profiles(model, data, X, profiles);
    print_cluster_profiles(profiles, 3);
    
    /* Curățare */
    free(profiles);
    kmeans_free(model);
    for (size_t i = 0; i < data->count; i++) {
        free(X[i]);
    }
    free(X);
    free(norm_income);
    free(norm_freq);
    free(norm_value);
    free_customer_data(data);
    
    printf("\n✓ Exercițiul 1 completat!\n\n");
    
    return 0;
}
