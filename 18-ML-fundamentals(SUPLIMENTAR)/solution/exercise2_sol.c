/**
 * ============================================================================
 * SOLUȚIE: Exercițiu 2 - Clasificare Multi-clasă cu Perceptron (One-vs-All)
 * Săptămâna 18: Fundamente Machine Learning în C
 * ============================================================================
 * 
 * Strategia One-vs-All (OvA):
 * - Pentru K clase, antrenăm K perceptroni binari
 * - Perceptronul i clasifică "clasă i" vs "toate celelalte"
 * - La predicție, alegem clasa cu cel mai mare scor (confidence)
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
// CONSTANTE
// ============================================================================

#define MAX_SAMPLES 200
#define MAX_FEATURES 10
#define MAX_CLASSES 10
#define MAX_LINE_LENGTH 256

// ============================================================================
// STRUCTURI DE DATE
// ============================================================================

typedef struct {
    double features[MAX_FEATURES];
    int label;
} IrisSample;

typedef struct {
    IrisSample samples[MAX_SAMPLES];
    int n_samples;
    int n_features;
    int n_classes;
    char class_names[MAX_CLASSES][32];
} IrisDataset;

typedef struct {
    double *weights;        // Vector de weights (include bias ca weights[n])
    int n_features;         // Numărul de caracteristici
    double learning_rate;   // Rata de învățare
    int max_epochs;         // Număr maxim de epoci
    int n_updates;          // Număr de actualizări efectuate
} Perceptron;

typedef struct {
    Perceptron **classifiers;  // Array de K perceptroni
    int n_classes;             // Numărul de clase
    int n_features;            // Numărul de caracteristici
} OneVsAllClassifier;

// ============================================================================
// PARTEA 1: ÎNCĂRCARE DATE
// ============================================================================

/**
 * Mapează numele speciei la index numeric.
 */
int get_class_index(IrisDataset *ds, const char *species) {
    for (int i = 0; i < ds->n_classes; i++) {
        if (strcmp(ds->class_names[i], species) == 0) {
            return i;
        }
    }
    // Clasă nouă
    if (ds->n_classes < MAX_CLASSES) {
        strncpy(ds->class_names[ds->n_classes], species, 31);
        ds->class_names[ds->n_classes][31] = '\0';
        return ds->n_classes++;
    }
    return -1;
}

/**
 * SOLUȚIE: Încarcă datele Iris din CSV.
 */
int load_iris_data(const char *filename, IrisDataset *ds) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("EROARE: Nu s-a putut deschide %s\n", filename);
        return -1;
    }
    
    ds->n_samples = 0;
    ds->n_features = 4;  // Iris are 4 caracteristici
    ds->n_classes = 0;
    
    char line[MAX_LINE_LENGTH];
    int line_num = 0;
    
    while (fgets(line, sizeof(line), file) && ds->n_samples < MAX_SAMPLES) {
        line_num++;
        
        // Ignoră comentarii și linii goale
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') {
            continue;
        }
        
        // Ignoră header
        if (strstr(line, "sepal") != NULL || strstr(line, "species") != NULL) {
            continue;
        }
        
        // Parse CSV: sepal_length,sepal_width,petal_length,petal_width,species
        double f1, f2, f3, f4;
        char species[32];
        
        int parsed = sscanf(line, "%lf,%lf,%lf,%lf,%s", &f1, &f2, &f3, &f4, species);
        
        if (parsed == 5) {
            IrisSample *s = &ds->samples[ds->n_samples];
            s->features[0] = f1;
            s->features[1] = f2;
            s->features[2] = f3;
            s->features[3] = f4;
            s->label = get_class_index(ds, species);
            
            if (s->label >= 0) {
                ds->n_samples++;
            }
        }
    }
    
    fclose(file);
    printf("INFO: Încărcate %d exemple cu %d clase din %s\n", 
           ds->n_samples, ds->n_classes, filename);
    return ds->n_samples;
}

// ============================================================================
// PARTEA 2: NORMALIZARE ȘI PREPROCESARE
// ============================================================================

/**
 * SOLUȚIE: Normalizează caracteristicile (Min-Max pe fiecare coloană).
 */
void normalize_features(IrisDataset *ds) {
    for (int j = 0; j < ds->n_features; j++) {
        double min_val = ds->samples[0].features[j];
        double max_val = ds->samples[0].features[j];
        
        // Găsește min și max
        for (int i = 1; i < ds->n_samples; i++) {
            if (ds->samples[i].features[j] < min_val) {
                min_val = ds->samples[i].features[j];
            }
            if (ds->samples[i].features[j] > max_val) {
                max_val = ds->samples[i].features[j];
            }
        }
        
        // Normalizează
        double range = max_val - min_val;
        if (range > 1e-10) {
            for (int i = 0; i < ds->n_samples; i++) {
                ds->samples[i].features[j] = 
                    (ds->samples[i].features[j] - min_val) / range;
            }
        }
    }
}

/**
 * SOLUȚIE: Shuffle dataset (Fisher-Yates algorithm).
 */
void shuffle_dataset(IrisDataset *ds) {
    for (int i = ds->n_samples - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        // Swap samples[i] și samples[j]
        IrisSample temp = ds->samples[i];
        ds->samples[i] = ds->samples[j];
        ds->samples[j] = temp;
    }
}

/**
 * SOLUȚIE: Split în train și test sets.
 */
void train_test_split(IrisDataset *full, 
                      IrisDataset *train, IrisDataset *test,
                      double test_ratio) {
    int n_test = (int)(full->n_samples * test_ratio);
    int n_train = full->n_samples - n_test;
    
    // Copiază metadata
    train->n_features = test->n_features = full->n_features;
    train->n_classes = test->n_classes = full->n_classes;
    for (int c = 0; c < full->n_classes; c++) {
        strcpy(train->class_names[c], full->class_names[c]);
        strcpy(test->class_names[c], full->class_names[c]);
    }
    
    // Split
    train->n_samples = n_train;
    test->n_samples = n_test;
    
    for (int i = 0; i < n_train; i++) {
        train->samples[i] = full->samples[i];
    }
    for (int i = 0; i < n_test; i++) {
        test->samples[i] = full->samples[n_train + i];
    }
}

// ============================================================================
// PARTEA 3: PERCEPTRON BINAR
// ============================================================================

/**
 * SOLUȚIE: Creează un perceptron.
 */
Perceptron *perceptron_create(int n_features, double lr, int max_epochs) {
    Perceptron *p = (Perceptron *)malloc(sizeof(Perceptron));
    p->n_features = n_features;
    p->learning_rate = lr;
    p->max_epochs = max_epochs;
    p->n_updates = 0;
    
    // weights[0..n_features-1] = weights, weights[n_features] = bias
    p->weights = (double *)calloc(n_features + 1, sizeof(double));
    
    // Inițializare mică random
    for (int i = 0; i <= n_features; i++) {
        p->weights[i] = ((double)rand() / RAND_MAX - 0.5) * 0.1;
    }
    
    return p;
}

/**
 * Eliberează memoria perceptronului.
 */
void perceptron_free(Perceptron *p) {
    if (p) {
        free(p->weights);
        free(p);
    }
}

/**
 * SOLUȚIE: Calculează scorul raw (înainte de activare).
 */
double perceptron_score(Perceptron *p, double *x) {
    double score = p->weights[p->n_features];  // bias
    for (int i = 0; i < p->n_features; i++) {
        score += p->weights[i] * x[i];
    }
    return score;
}

/**
 * SOLUȚIE: Predicție binară (0 sau 1).
 */
int perceptron_predict(Perceptron *p, double *x) {
    return perceptron_score(p, x) >= 0 ? 1 : 0;
}

/**
 * SOLUȚIE: Antrenează perceptronul pentru o epocă.
 * 
 * @param y_binary Array de etichete binare (0 sau 1)
 * @return Numărul de greșeli în această epocă
 */
int perceptron_train_epoch(Perceptron *p, double **X, int *y_binary, int n) {
    int errors = 0;
    
    for (int i = 0; i < n; i++) {
        int prediction = perceptron_predict(p, X[i]);
        int target = y_binary[i];
        
        if (prediction != target) {
            errors++;
            p->n_updates++;
            
            // Update rule: w = w + lr * (target - pred) * x
            // Dacă target=1, pred=0: w = w + lr * x (împingem spre +)
            // Dacă target=0, pred=1: w = w - lr * x (împingem spre -)
            double delta = p->learning_rate * (target - prediction);
            
            for (int j = 0; j < p->n_features; j++) {
                p->weights[j] += delta * X[i][j];
            }
            p->weights[p->n_features] += delta;  // bias update
        }
    }
    
    return errors;
}

/**
 * SOLUȚIE: Antrenare completă cu early stopping.
 */
int perceptron_fit(Perceptron *p, double **X, int *y_binary, int n) {
    for (int epoch = 0; epoch < p->max_epochs; epoch++) {
        int errors = perceptron_train_epoch(p, X, y_binary, n);
        
        if (errors == 0) {
            return epoch + 1;  // Convergență
        }
    }
    return p->max_epochs;  // Nu a convergit complet
}

// ============================================================================
// PARTEA 4: ONE-VS-ALL CLASSIFIER
// ============================================================================

/**
 * SOLUȚIE: Creează clasificatorul OvA.
 */
OneVsAllClassifier *ova_create(int n_classes, int n_features, 
                                double lr, int max_epochs) {
    OneVsAllClassifier *ova = (OneVsAllClassifier *)malloc(sizeof(OneVsAllClassifier));
    ova->n_classes = n_classes;
    ova->n_features = n_features;
    
    ova->classifiers = (Perceptron **)malloc(n_classes * sizeof(Perceptron *));
    for (int c = 0; c < n_classes; c++) {
        ova->classifiers[c] = perceptron_create(n_features, lr, max_epochs);
    }
    
    return ova;
}

/**
 * Eliberează memoria OvA.
 */
void ova_free(OneVsAllClassifier *ova) {
    if (ova) {
        for (int c = 0; c < ova->n_classes; c++) {
            perceptron_free(ova->classifiers[c]);
        }
        free(ova->classifiers);
        free(ova);
    }
}

/**
 * SOLUȚIE: Antrenează clasificatorul OvA.
 */
void ova_fit(OneVsAllClassifier *ova, double **X, int *y, int n) {
    printf("\n  Antrenare One-vs-All (%d clase):\n", ova->n_classes);
    
    // Buffer pentru etichete binare
    int *y_binary = (int *)malloc(n * sizeof(int));
    
    for (int c = 0; c < ova->n_classes; c++) {
        // Creează etichete binare: 1 dacă clasă == c, 0 altfel
        for (int i = 0; i < n; i++) {
            y_binary[i] = (y[i] == c) ? 1 : 0;
        }
        
        // Antrenează perceptronul pentru clasa c
        int epochs = perceptron_fit(ova->classifiers[c], X, y_binary, n);
        
        printf("    Clasă %d: convergență în %d epoci, %d actualizări\n",
               c, epochs, ova->classifiers[c]->n_updates);
    }
    
    free(y_binary);
}

/**
 * SOLUȚIE: Predicție OvA - alege clasa cu scorul maxim.
 */
int ova_predict(OneVsAllClassifier *ova, double *x) {
    double max_score = -DBL_MAX;
    int best_class = 0;
    
    for (int c = 0; c < ova->n_classes; c++) {
        double score = perceptron_score(ova->classifiers[c], x);
        if (score > max_score) {
            max_score = score;
            best_class = c;
        }
    }
    
    return best_class;
}

/**
 * Predicții pentru mai multe exemple.
 */
void ova_predict_batch(OneVsAllClassifier *ova, double **X, int n, int *y_pred) {
    for (int i = 0; i < n; i++) {
        y_pred[i] = ova_predict(ova, X[i]);
    }
}

// ============================================================================
// PARTEA 5: EVALUARE
// ============================================================================

/**
 * SOLUȚIE: Calculează și afișează matricea de confuzie.
 */
void confusion_matrix(int *y_true, int *y_pred, int n, int n_classes,
                      char class_names[MAX_CLASSES][32]) {
    // Alocă matricea
    int **matrix = (int **)calloc(n_classes, sizeof(int *));
    for (int i = 0; i < n_classes; i++) {
        matrix[i] = (int *)calloc(n_classes, sizeof(int));
    }
    
    // Populează
    for (int i = 0; i < n; i++) {
        matrix[y_true[i]][y_pred[i]]++;
    }
    
    // Afișează
    printf("\n  ┌─────────────────────────────────────────────────────────┐\n");
    printf("  │                   MATRICE DE CONFUZIE                   │\n");
    printf("  └─────────────────────────────────────────────────────────┘\n\n");
    
    // Header
    printf("                    │ Predicție                          │\n");
    printf("  Real              │");
    for (int c = 0; c < n_classes; c++) {
        printf(" %-10.10s│", class_names[c]);
    }
    printf("\n  ──────────────────┼");
    for (int c = 0; c < n_classes; c++) {
        printf("───────────┼");
    }
    printf("\n");
    
    // Rânduri
    for (int i = 0; i < n_classes; i++) {
        printf("  %-18.18s│", class_names[i]);
        for (int j = 0; j < n_classes; j++) {
            if (i == j) {
                printf(" \033[32m%4d\033[0m      │", matrix[i][j]);  // Verde pentru diagonală
            } else if (matrix[i][j] > 0) {
                printf(" \033[31m%4d\033[0m      │", matrix[i][j]);  // Roșu pentru greșeli
            } else {
                printf(" %4d      │", matrix[i][j]);
            }
        }
        printf("\n");
    }
    
    // Eliberare
    for (int i = 0; i < n_classes; i++) free(matrix[i]);
    free(matrix);
}

/**
 * SOLUȚIE: Calculează metrici per clasă.
 */
void per_class_metrics(int *y_true, int *y_pred, int n, int n_classes,
                       char class_names[MAX_CLASSES][32]) {
    printf("\n  ┌─────────────────────────────────────────────────────────┐\n");
    printf("  │                 METRICI PER CLASĂ                       │\n");
    printf("  └─────────────────────────────────────────────────────────┘\n\n");
    
    printf("  Clasă              │ Precision │ Recall    │ F1-Score  │ Support │\n");
    printf("  ───────────────────┼───────────┼───────────┼───────────┼─────────┤\n");
    
    double total_f1 = 0;
    int total_support = 0;
    
    for (int c = 0; c < n_classes; c++) {
        int tp = 0, fp = 0, fn = 0;
        int support = 0;
        
        for (int i = 0; i < n; i++) {
            if (y_true[i] == c) {
                support++;
                if (y_pred[i] == c) tp++;
                else fn++;
            } else {
                if (y_pred[i] == c) fp++;
            }
        }
        
        double precision = (tp + fp) > 0 ? (double)tp / (tp + fp) : 0;
        double recall = (tp + fn) > 0 ? (double)tp / (tp + fn) : 0;
        double f1 = (precision + recall) > 0 ? 
                    2 * precision * recall / (precision + recall) : 0;
        
        printf("  %-18.18s │   %5.3f   │   %5.3f   │   %5.3f   │  %4d   │\n",
               class_names[c], precision, recall, f1, support);
        
        total_f1 += f1 * support;
        total_support += support;
    }
    
    printf("  ───────────────────┼───────────┼───────────┼───────────┼─────────┤\n");
    printf("  Weighted Average   │           │           │   %5.3f   │  %4d   │\n",
           total_f1 / total_support, total_support);
}

/**
 * Calculează acuratețea.
 */
double accuracy(int *y_true, int *y_pred, int n) {
    int correct = 0;
    for (int i = 0; i < n; i++) {
        if (y_true[i] == y_pred[i]) correct++;
    }
    return (double)correct / n;
}

// ============================================================================
// FUNCȚIA PRINCIPALĂ
// ============================================================================

int main(int argc, char *argv[]) {
    srand(42);
    
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║  SOLUȚIE: Clasificare Multi-clasă cu Perceptron (One-vs-All)   ║\n");
    printf("║  Săptămâna 18 - Machine Learning Fundamentals                  ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n");
    
    // === 1. Încărcare date ===
    const char *filename = (argc > 1) ? argv[1] : "data/iris.csv";
    IrisDataset full_ds, train_ds, test_ds;
    
    int n = load_iris_data(filename, &full_ds);
    if (n <= 0) {
        printf("EROARE: Nu s-au putut încărca date!\n");
        return 1;
    }
    
    // === 2. Preprocesare ===
    printf("\n  Preprocesare...\n");
    normalize_features(&full_ds);
    printf("  ✓ Date normalizate\n");
    
    shuffle_dataset(&full_ds);
    printf("  ✓ Date amestecate\n");
    
    train_test_split(&full_ds, &train_ds, &test_ds, 0.2);
    printf("  ✓ Split: %d train, %d test\n", train_ds.n_samples, test_ds.n_samples);
    
    // === 3. Conversie la format matriceal ===
    double **X_train = (double **)malloc(train_ds.n_samples * sizeof(double *));
    int *y_train = (int *)malloc(train_ds.n_samples * sizeof(int));
    for (int i = 0; i < train_ds.n_samples; i++) {
        X_train[i] = train_ds.samples[i].features;
        y_train[i] = train_ds.samples[i].label;
    }
    
    double **X_test = (double **)malloc(test_ds.n_samples * sizeof(double *));
    int *y_test = (int *)malloc(test_ds.n_samples * sizeof(int));
    for (int i = 0; i < test_ds.n_samples; i++) {
        X_test[i] = test_ds.samples[i].features;
        y_test[i] = test_ds.samples[i].label;
    }
    
    // === 4. Antrenare OvA ===
    printf("\n═══════════════════════════════════════════════════════\n");
    printf("  Antrenare model One-vs-All Perceptron\n");
    printf("═══════════════════════════════════════════════════════\n");
    
    OneVsAllClassifier *ova = ova_create(
        full_ds.n_classes, 
        full_ds.n_features,
        0.1,    // learning rate
        1000    // max epochs
    );
    
    ova_fit(ova, X_train, y_train, train_ds.n_samples);
    
    // === 5. Evaluare pe test ===
    printf("\n═══════════════════════════════════════════════════════\n");
    printf("  Evaluare pe setul de test\n");
    printf("═══════════════════════════════════════════════════════\n");
    
    int *y_pred = (int *)malloc(test_ds.n_samples * sizeof(int));
    ova_predict_batch(ova, X_test, test_ds.n_samples, y_pred);
    
    double acc = accuracy(y_test, y_pred, test_ds.n_samples);
    printf("\n  Acuratețe: %.4f (%.1f%%)\n", acc, acc * 100);
    
    confusion_matrix(y_test, y_pred, test_ds.n_samples, 
                     full_ds.n_classes, full_ds.class_names);
    
    per_class_metrics(y_test, y_pred, test_ds.n_samples,
                      full_ds.n_classes, full_ds.class_names);
    
    // === 6. Exemple de predicții ===
    printf("\n═══════════════════════════════════════════════════════\n");
    printf("  Exemple de predicții\n");
    printf("═══════════════════════════════════════════════════════\n\n");
    
    for (int i = 0; i < 5 && i < test_ds.n_samples; i++) {
        int pred = ova_predict(ova, X_test[i]);
        int real = y_test[i];
        const char *status = (pred == real) ? "✓" : "✗";
        
        printf("  Exemplu %d: [%.2f, %.2f, %.2f, %.2f]\n", i,
               X_test[i][0], X_test[i][1], X_test[i][2], X_test[i][3]);
        printf("             Real: %-12s, Predicție: %-12s %s\n",
               full_ds.class_names[real], full_ds.class_names[pred], status);
    }
    
    // === 7. Cleanup ===
    ova_free(ova);
    free(X_train);
    free(y_train);
    free(X_test);
    free(y_test);
    free(y_pred);
    
    printf("\n✅ Clasificare completă!\n\n");
    
    return 0;
}
