/**
 * ============================================================================
 * SOLUȚIE: Tema 2 - Weighted K-Nearest Neighbors
 * Săptămâna 18: Fundamente Machine Learning în C
 * ============================================================================
 * 
 * Weighted K-NN îmbunătățește K-NN clasic prin acordarea de ponderi
 * vecinilor în funcție de distanța lor față de punctul de prezis.
 * 
 * Scheme de ponderare implementate:
 * 1. Uniform: toate ponderile = 1 (KNN clasic)
 * 2. Inverse Distance: w = 1/d
 * 3. Gaussian: w = exp(-d²/2σ²)
 * 4. Linear Decay: w = (k - rank + 1) / k
 * 
 * ATENȚIE: Soluție pentru profesori - nu distribuiți studenților!
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>

#define MAX_SAMPLES 200
#define MAX_FEATURES 10
#define MAX_CLASSES 10
#define MAX_LINE_LENGTH 256

// ============================================================================
// ENUMERĂRI ȘI STRUCTURI
// ============================================================================

typedef enum {
    WEIGHT_UNIFORM,        // KNN clasic (toate ponderile = 1)
    WEIGHT_INVERSE_DIST,   // w = 1/d
    WEIGHT_GAUSSIAN,       // w = exp(-d²/2σ²)
    WEIGHT_LINEAR_DECAY    // w = (k - rank + 1) / k
} WeightScheme;

const char *scheme_names[] = {
    "Uniform (KNN clasic)",
    "Inverse Distance",
    "Gaussian Kernel",
    "Linear Decay"
};

typedef struct {
    int k;                 // Număr de vecini
    WeightScheme scheme;   // Schema de ponderare
    double sigma;          // Parametru pentru Gaussian (dacă e cazul)
    
    // Date de antrenare (stocare lazy)
    double **X_train;
    int *y_train;
    int n_train;
    int n_features;
    int n_classes;
} WeightedKNN;

typedef struct {
    int index;
    double distance;
} Neighbor;

// ============================================================================
// FUNCȚII UTILITARE
// ============================================================================

/**
 * Calculează distanța Euclidiană.
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
 * Comparator pentru sortare vecini după distanță.
 */
int compare_neighbors(const void *a, const void *b) {
    const Neighbor *na = (const Neighbor *)a;
    const Neighbor *nb = (const Neighbor *)b;
    if (na->distance < nb->distance) return -1;
    if (na->distance > nb->distance) return 1;
    return 0;
}

// ============================================================================
// WEIGHTED K-NN
// ============================================================================

/**
 * Creează clasificator Weighted K-NN.
 */
WeightedKNN *wknn_create(int k, WeightScheme scheme, double sigma) {
    WeightedKNN *model = (WeightedKNN *)malloc(sizeof(WeightedKNN));
    
    model->k = k;
    model->scheme = scheme;
    model->sigma = sigma;
    
    model->X_train = NULL;
    model->y_train = NULL;
    model->n_train = 0;
    model->n_features = 0;
    model->n_classes = 0;
    
    return model;
}

/**
 * Eliberează memoria.
 */
void wknn_free(WeightedKNN *model) {
    if (model) {
        // Nu eliberăm X_train și y_train - sunt referințe externe
        free(model);
    }
}

/**
 * "Antrenează" modelul (stocare date pentru lazy learning).
 */
void wknn_fit(WeightedKNN *model, double **X, int *y, 
              int n, int n_feat, int n_class) {
    model->X_train = X;
    model->y_train = y;
    model->n_train = n;
    model->n_features = n_feat;
    model->n_classes = n_class;
}

/**
 * Calculează ponderea pentru un vecin.
 * 
 * @param distance  Distanța până la vecin
 * @param rank      Poziția în ordinea distanțelor (1 = cel mai apropiat)
 * @return          Ponderea calculată
 */
double wknn_compute_weight(WeightedKNN *model, double distance, int rank) {
    switch (model->scheme) {
        case WEIGHT_UNIFORM:
            return 1.0;
            
        case WEIGHT_INVERSE_DIST:
            // Evită division by zero
            if (distance < 1e-10) return 1e10;
            return 1.0 / distance;
            
        case WEIGHT_GAUSSIAN:
            // w = exp(-d²/2σ²)
            return exp(-(distance * distance) / (2.0 * model->sigma * model->sigma));
            
        case WEIGHT_LINEAR_DECAY:
            // w = (k - rank + 1) / k
            return (double)(model->k - rank + 1) / model->k;
            
        default:
            return 1.0;
    }
}

/**
 * Prezice clasa pentru un singur exemplu.
 */
int wknn_predict_one(WeightedKNN *model, double *x) {
    // Calculează distanțele la toate punctele de antrenare
    Neighbor *neighbors = (Neighbor *)malloc(model->n_train * sizeof(Neighbor));
    
    for (int i = 0; i < model->n_train; i++) {
        neighbors[i].index = i;
        neighbors[i].distance = euclidean_distance(x, model->X_train[i], model->n_features);
    }
    
    // Sortează după distanță
    qsort(neighbors, model->n_train, sizeof(Neighbor), compare_neighbors);
    
    // Calculează scoruri ponderate pentru fiecare clasă
    double *class_scores = (double *)calloc(model->n_classes, sizeof(double));
    
    for (int i = 0; i < model->k && i < model->n_train; i++) {
        int neighbor_idx = neighbors[i].index;
        int neighbor_class = model->y_train[neighbor_idx];
        double weight = wknn_compute_weight(model, neighbors[i].distance, i + 1);
        
        class_scores[neighbor_class] += weight;
    }
    
    // Găsește clasa cu scorul maxim
    int best_class = 0;
    double max_score = class_scores[0];
    
    for (int c = 1; c < model->n_classes; c++) {
        if (class_scores[c] > max_score) {
            max_score = class_scores[c];
            best_class = c;
        }
    }
    
    free(neighbors);
    free(class_scores);
    
    return best_class;
}

/**
 * Prezice clasele pentru mai multe exemple.
 */
void wknn_predict(WeightedKNN *model, double **X, int n, int *y_pred) {
    for (int i = 0; i < n; i++) {
        y_pred[i] = wknn_predict_one(model, X[i]);
    }
}

/**
 * Calculează acuratețea.
 */
double wknn_accuracy(int *y_true, int *y_pred, int n) {
    int correct = 0;
    for (int i = 0; i < n; i++) {
        if (y_true[i] == y_pred[i]) correct++;
    }
    return (double)correct / n;
}

// ============================================================================
// ÎNCĂRCARE DATE
// ============================================================================

int load_iris_data(const char *filename, double **X, int *y, 
                   char class_names[MAX_CLASSES][32], int *n_classes, int max_n) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("EROARE: Nu s-a putut deschide %s\n", filename);
        return -1;
    }
    
    char line[MAX_LINE_LENGTH];
    int n = 0;
    *n_classes = 0;
    
    while (fgets(line, sizeof(line), file) && n < max_n) {
        if (line[0] == '#' || line[0] == '\n') continue;
        if (strstr(line, "sepal") != NULL) continue;
        
        double f1, f2, f3, f4;
        char species[32];
        
        if (sscanf(line, "%lf,%lf,%lf,%lf,%s", &f1, &f2, &f3, &f4, species) == 5) {
            X[n][0] = f1;
            X[n][1] = f2;
            X[n][2] = f3;
            X[n][3] = f4;
            
            // Mapează specia la index
            int class_idx = -1;
            for (int c = 0; c < *n_classes; c++) {
                if (strcmp(class_names[c], species) == 0) {
                    class_idx = c;
                    break;
                }
            }
            if (class_idx < 0 && *n_classes < MAX_CLASSES) {
                class_idx = *n_classes;
                strncpy(class_names[*n_classes], species, 31);
                (*n_classes)++;
            }
            
            y[n] = class_idx;
            n++;
        }
    }
    
    fclose(file);
    return n;
}

void normalize_data(double **X, int n, int d) {
    for (int j = 0; j < d; j++) {
        double min_val = X[0][j], max_val = X[0][j];
        for (int i = 1; i < n; i++) {
            if (X[i][j] < min_val) min_val = X[i][j];
            if (X[i][j] > max_val) max_val = X[i][j];
        }
        double range = max_val - min_val;
        if (range > 1e-10) {
            for (int i = 0; i < n; i++) {
                X[i][j] = (X[i][j] - min_val) / range;
            }
        }
    }
}

void shuffle_data(double **X, int *y, int n, int d) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        // Swap X
        double *tmp_x = X[i];
        X[i] = X[j];
        X[j] = tmp_x;
        // Swap y
        int tmp_y = y[i];
        y[i] = y[j];
        y[j] = tmp_y;
    }
}

// ============================================================================
// COMPARAȚIE SCHEME
// ============================================================================

/**
 * Compară diferite scheme de ponderare.
 */
void wknn_compare_schemes(double **X_train, int *y_train, int n_train,
                          double **X_test, int *y_test, int n_test,
                          int n_features, int n_classes,
                          int k_values[], int n_k_values) {
    
    printf("\n╔════════════════════════════════════════════════════════════════════╗\n");
    printf("║           COMPARAȚIE SCHEME DE PONDERARE K-NN                      ║\n");
    printf("╚════════════════════════════════════════════════════════════════════╝\n\n");
    
    int *y_pred = (int *)malloc(n_test * sizeof(int));
    
    // Pentru fiecare valoare de k
    for (int ki = 0; ki < n_k_values; ki++) {
        int k = k_values[ki];
        
        printf("┌──────────────────────────────────────────────────────────────────┐\n");
        printf("│  k = %-4d                                                        │\n", k);
        printf("├──────────────────────────────────────────────────────────────────┤\n");
        printf("│  Schema                    │ Acuratețe │ Greșeli │ Observații    │\n");
        printf("├────────────────────────────┼───────────┼─────────┼───────────────┤\n");
        
        // Test fiecare schemă
        WeightScheme schemes[] = {WEIGHT_UNIFORM, WEIGHT_INVERSE_DIST, 
                                  WEIGHT_GAUSSIAN, WEIGHT_LINEAR_DECAY};
        double sigmas[] = {0.0, 0.0, 0.5, 0.0};  // sigma pentru Gaussian
        
        double best_acc = 0;
        WeightScheme best_scheme = WEIGHT_UNIFORM;
        
        for (int s = 0; s < 4; s++) {
            WeightedKNN *model = wknn_create(k, schemes[s], sigmas[s]);
            wknn_fit(model, X_train, y_train, n_train, n_features, n_classes);
            wknn_predict(model, X_test, n_test, y_pred);
            
            double acc = wknn_accuracy(y_test, y_pred, n_test);
            int errors = n_test - (int)(acc * n_test);
            
            const char *obs = "";
            if (acc > best_acc) {
                best_acc = acc;
                best_scheme = schemes[s];
                obs = "← Best";
            }
            
            printf("│  %-24s │   %5.3f   │   %3d   │ %-13s │\n",
                   scheme_names[s], acc, errors, obs);
            
            wknn_free(model);
        }
        
        printf("└──────────────────────────────────────────────────────────────────┘\n\n");
    }
    
    free(y_pred);
}

// ============================================================================
// FUNCȚIA PRINCIPALĂ
// ============================================================================

int main(int argc, char *argv[]) {
    srand(42);
    
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║          SOLUȚIE: Weighted K-Nearest Neighbors                 ║\n");
    printf("║          Săptămâna 18 - Machine Learning Fundamentals          ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n");
    
    // === 1. Încărcare date ===
    const char *filename = (argc > 1) ? argv[1] : "data/iris.csv";
    int max_n = MAX_SAMPLES;
    int n_features = 4;
    
    double **X = (double **)malloc(max_n * sizeof(double *));
    for (int i = 0; i < max_n; i++) {
        X[i] = (double *)malloc(n_features * sizeof(double));
    }
    int *y = (int *)malloc(max_n * sizeof(int));
    char class_names[MAX_CLASSES][32];
    int n_classes;
    
    int n = load_iris_data(filename, X, y, class_names, &n_classes, max_n);
    if (n <= 0) {
        printf("EROARE: Nu s-au putut încărca date!\n");
        return 1;
    }
    printf("\n  Date încărcate: %d exemple, %d clase\n", n, n_classes);
    printf("  Clase: ");
    for (int c = 0; c < n_classes; c++) {
        printf("%s%s", class_names[c], c < n_classes-1 ? ", " : "\n");
    }
    
    // === 2. Preprocesare ===
    normalize_data(X, n, n_features);
    printf("  Date normalizate (Min-Max)\n");
    
    shuffle_data(X, y, n, n_features);
    printf("  Date amestecate\n");
    
    // === 3. Train/Test split ===
    int n_test = n / 5;  // 20% test
    int n_train = n - n_test;
    
    double **X_train = X;
    int *y_train = y;
    double **X_test = X + n_train;
    int *y_test = y + n_train;
    
    printf("  Split: %d train, %d test\n", n_train, n_test);
    
    // === 4. Comparație scheme pentru diferite valori de k ===
    int k_values[] = {3, 5, 7, 11};
    int n_k_values = sizeof(k_values) / sizeof(k_values[0]);
    
    wknn_compare_schemes(X_train, y_train, n_train,
                         X_test, y_test, n_test,
                         n_features, n_classes,
                         k_values, n_k_values);
    
    // === 5. Demonstrație detaliată pentru k=5 ===
    printf("\n═══════════════════════════════════════════════════════\n");
    printf("  Demonstrație detaliată: k=5, schema Inverse Distance\n");
    printf("═══════════════════════════════════════════════════════\n\n");
    
    WeightedKNN *demo_model = wknn_create(5, WEIGHT_INVERSE_DIST, 0.0);
    wknn_fit(demo_model, X_train, y_train, n_train, n_features, n_classes);
    
    // Afișează detalii pentru primele 3 exemple de test
    printf("  Exemple de clasificare cu voturi ponderate:\n\n");
    
    for (int t = 0; t < 3 && t < n_test; t++) {
        printf("  Exemplu %d: [%.2f, %.2f, %.2f, %.2f]\n", t,
               X_test[t][0], X_test[t][1], X_test[t][2], X_test[t][3]);
        printf("  Real: %s\n", class_names[y_test[t]]);
        
        // Calculează și afișează vecinii
        Neighbor *neighbors = (Neighbor *)malloc(n_train * sizeof(Neighbor));
        for (int i = 0; i < n_train; i++) {
            neighbors[i].index = i;
            neighbors[i].distance = euclidean_distance(X_test[t], X_train[i], n_features);
        }
        qsort(neighbors, n_train, sizeof(Neighbor), compare_neighbors);
        
        printf("  Top %d vecini:\n", demo_model->k);
        double class_scores[MAX_CLASSES] = {0};
        
        for (int i = 0; i < demo_model->k; i++) {
            int idx = neighbors[i].index;
            double d = neighbors[i].distance;
            double w = wknn_compute_weight(demo_model, d, i + 1);
            int c = y_train[idx];
            class_scores[c] += w;
            
            printf("    %d. clasă=%s, dist=%.4f, weight=%.4f\n",
                   i + 1, class_names[c], d, w);
        }
        
        printf("  Scoruri per clasă:\n");
        int pred = 0;
        double max_score = class_scores[0];
        for (int c = 0; c < n_classes; c++) {
            printf("    %s: %.4f%s\n", class_names[c], class_scores[c],
                   class_scores[c] == max_score ? " ← max" : "");
            if (class_scores[c] > max_score) {
                max_score = class_scores[c];
                pred = c;
            }
        }
        printf("  Predicție: %s %s\n\n", class_names[pred],
               pred == y_test[t] ? "✓" : "✗");
        
        free(neighbors);
    }
    
    wknn_free(demo_model);
    
    // === 6. Testare influența parametrului sigma pentru Gaussian ===
    printf("\n═══════════════════════════════════════════════════════\n");
    printf("  Influența parametrului σ pentru Gaussian Kernel\n");
    printf("═══════════════════════════════════════════════════════\n\n");
    
    double sigmas[] = {0.1, 0.3, 0.5, 1.0, 2.0};
    int n_sigmas = sizeof(sigmas) / sizeof(sigmas[0]);
    int *y_pred = (int *)malloc(n_test * sizeof(int));
    
    printf("  k=5, schema Gaussian:\n");
    printf("  ┌─────────┬───────────┬─────────┐\n");
    printf("  │    σ    │ Acuratețe │ Greșeli │\n");
    printf("  ├─────────┼───────────┼─────────┤\n");
    
    for (int si = 0; si < n_sigmas; si++) {
        WeightedKNN *model = wknn_create(5, WEIGHT_GAUSSIAN, sigmas[si]);
        wknn_fit(model, X_train, y_train, n_train, n_features, n_classes);
        wknn_predict(model, X_test, n_test, y_pred);
        
        double acc = wknn_accuracy(y_test, y_pred, n_test);
        int errors = n_test - (int)(acc * n_test);
        
        printf("  │  %5.2f  │   %5.3f   │   %3d   │\n", sigmas[si], acc, errors);
        
        wknn_free(model);
    }
    
    printf("  └─────────┴───────────┴─────────┘\n");
    
    printf("\n  Observații:\n");
    printf("  • σ mic: vecinii foarte apropiați domină\n");
    printf("  • σ mare: toți vecinii au ponderi similare (→ uniform)\n");
    printf("  • Optim: σ care balansează influența vecinilor apropiați\n");
    
    // === Cleanup ===
    for (int i = 0; i < max_n; i++) free(X[i]);
    free(X);
    free(y);
    free(y_pred);
    
    printf("\n✅ Comparație completă!\n\n");
    
    return 0;
}
