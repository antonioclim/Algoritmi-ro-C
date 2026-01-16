/**
 * ============================================================================
 * SOLUȚIE: Tema 1 - Ridge Regression (Regularizare L2)
 * Săptămâna 18: Fundamente Machine Learning în C
 * ============================================================================
 * 
 * Ridge Regression adaugă un termen de penalizare L2 pentru a preveni
 * overfitting-ul și a îmbunătăți generalizarea.
 * 
 * Cost function: J(w) = MSE(w) + λ·||w||²
 * Gradient: ∂J/∂w = (2/n)·X^T·(Xw - y) + 2λ·w
 * 
 * ATENȚIE: Soluție pentru profesori - nu distribuiți studenților!
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAX_LINE 256
#define MAX_SAMPLES 1000

// ============================================================================
// STRUCTURI
// ============================================================================

typedef struct {
    double *weights;      // Vector de weight-uri
    double bias;          // Bias term
    int n_features;       // Număr de caracteristici
    double lambda;        // Parametru de regularizare
    double learning_rate; // Rata de învățare
    int max_iters;        // Număr maxim de iterații
    double tolerance;     // Toleranță pentru convergență
} RidgeRegression;

// ============================================================================
// FUNCȚII UTILITARE
// ============================================================================

/**
 * Creează și inițializează un model Ridge Regression.
 */
RidgeRegression *ridge_create(int n_features, double lambda, 
                              double lr, int max_iters, double tol) {
    RidgeRegression *model = (RidgeRegression *)malloc(sizeof(RidgeRegression));
    
    model->n_features = n_features;
    model->lambda = lambda;
    model->learning_rate = lr;
    model->max_iters = max_iters;
    model->tolerance = tol;
    
    // Inițializare weights cu valori mici
    model->weights = (double *)calloc(n_features, sizeof(double));
    model->bias = 0.0;
    
    // Inițializare random mică
    for (int i = 0; i < n_features; i++) {
        model->weights[i] = ((double)rand() / RAND_MAX - 0.5) * 0.01;
    }
    
    return model;
}

/**
 * Eliberează memoria pentru model.
 */
void ridge_free(RidgeRegression *model) {
    if (model) {
        free(model->weights);
        free(model);
    }
}

/**
 * Predicție pentru un singur exemplu.
 */
double ridge_predict_one(RidgeRegression *model, double *x) {
    double pred = model->bias;
    for (int j = 0; j < model->n_features; j++) {
        pred += model->weights[j] * x[j];
    }
    return pred;
}

/**
 * Predicții pentru mai multe exemple.
 */
void ridge_predict(RidgeRegression *model, double **X, int n, double *y_pred) {
    for (int i = 0; i < n; i++) {
        y_pred[i] = ridge_predict_one(model, X[i]);
    }
}

/**
 * Calculează MSE (Mean Squared Error).
 */
double compute_mse(double *y_true, double *y_pred, int n) {
    double sum = 0.0;
    for (int i = 0; i < n; i++) {
        double error = y_true[i] - y_pred[i];
        sum += error * error;
    }
    return sum / n;
}

/**
 * Calculează funcția de cost cu regularizare L2.
 * 
 * J(w) = (1/n)·Σ(y - ŷ)² + λ·Σw²
 */
double ridge_cost(RidgeRegression *model, double **X, double *y, int n) {
    // MSE term
    double *y_pred = (double *)malloc(n * sizeof(double));
    ridge_predict(model, X, n, y_pred);
    double mse = compute_mse(y, y_pred, n);
    free(y_pred);
    
    // Regularization term (L2 penalty)
    double l2_penalty = 0.0;
    for (int j = 0; j < model->n_features; j++) {
        l2_penalty += model->weights[j] * model->weights[j];
    }
    
    return mse + model->lambda * l2_penalty;
}

/**
 * Antrenează modelul folosind Gradient Descent cu regularizare L2.
 * 
 * Gradient pentru weight j:
 *   ∂J/∂w_j = (2/n)·Σ(ŷ_i - y_i)·x_ij + 2·λ·w_j
 * 
 * Update rule:
 *   w_j = w_j - α·∂J/∂w_j
 *       = w_j·(1 - 2·α·λ) - α·(2/n)·Σ(error·x_ij)
 */
int ridge_fit(RidgeRegression *model, double **X, double *y, int n) {
    double *y_pred = (double *)malloc(n * sizeof(double));
    double *gradients = (double *)malloc(model->n_features * sizeof(double));
    
    double prev_cost = 1e10;
    int iter;
    
    for (iter = 0; iter < model->max_iters; iter++) {
        // 1. Calculează predicții
        ridge_predict(model, X, n, y_pred);
        
        // 2. Calculează gradienți
        // Resetează
        for (int j = 0; j < model->n_features; j++) {
            gradients[j] = 0.0;
        }
        double bias_gradient = 0.0;
        
        // Sumă gradienți din MSE
        for (int i = 0; i < n; i++) {
            double error = y_pred[i] - y[i];
            
            for (int j = 0; j < model->n_features; j++) {
                gradients[j] += error * X[i][j];
            }
            bias_gradient += error;
        }
        
        // Normalizare și adăugare termen de regularizare
        for (int j = 0; j < model->n_features; j++) {
            gradients[j] = (2.0 / n) * gradients[j] + 2.0 * model->lambda * model->weights[j];
        }
        bias_gradient = (2.0 / n) * bias_gradient;  // bias nu se regularizează
        
        // 3. Update weights
        for (int j = 0; j < model->n_features; j++) {
            model->weights[j] -= model->learning_rate * gradients[j];
        }
        model->bias -= model->learning_rate * bias_gradient;
        
        // 4. Verifică convergență
        double current_cost = ridge_cost(model, X, y, n);
        
        if (fabs(prev_cost - current_cost) < model->tolerance) {
            iter++;
            break;
        }
        prev_cost = current_cost;
    }
    
    free(y_pred);
    free(gradients);
    
    return iter;
}

/**
 * Calculează R² score.
 * 
 * R² = 1 - SS_res / SS_tot
 *    = 1 - Σ(y - ŷ)² / Σ(y - ȳ)²
 */
double ridge_r2_score(double *y_true, double *y_pred, int n) {
    // Calculează media
    double y_mean = 0.0;
    for (int i = 0; i < n; i++) {
        y_mean += y_true[i];
    }
    y_mean /= n;
    
    // SS_res și SS_tot
    double ss_res = 0.0, ss_tot = 0.0;
    for (int i = 0; i < n; i++) {
        ss_res += (y_true[i] - y_pred[i]) * (y_true[i] - y_pred[i]);
        ss_tot += (y_true[i] - y_mean) * (y_true[i] - y_mean);
    }
    
    if (ss_tot < 1e-10) return 0.0;  // Evită division by zero
    
    return 1.0 - ss_res / ss_tot;
}

// ============================================================================
// ÎNCĂRCARE DATE
// ============================================================================

int load_housing_data(const char *filename, double **X, double *y, int max_n) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("EROARE: Nu s-a putut deschide %s\n", filename);
        return -1;
    }
    
    char line[MAX_LINE];
    int n = 0;
    
    while (fgets(line, sizeof(line), file) && n < max_n) {
        if (line[0] == '#' || line[0] == '\n') continue;
        if (strstr(line, "sqft") != NULL) continue;  // header
        
        double sqft, beds, baths, age, price;
        if (sscanf(line, "%lf,%lf,%lf,%lf,%lf", 
                   &sqft, &beds, &baths, &age, &price) == 5) {
            X[n][0] = sqft;
            X[n][1] = beds;
            X[n][2] = baths;
            y[n] = price;
            n++;
        }
    }
    
    fclose(file);
    return n;
}

void normalize_data(double **X, int n, int d, double *means, double *stds) {
    // Calculează mean și std pentru fiecare caracteristică
    for (int j = 0; j < d; j++) {
        means[j] = 0.0;
        for (int i = 0; i < n; i++) {
            means[j] += X[i][j];
        }
        means[j] /= n;
        
        stds[j] = 0.0;
        for (int i = 0; i < n; i++) {
            stds[j] += (X[i][j] - means[j]) * (X[i][j] - means[j]);
        }
        stds[j] = sqrt(stds[j] / n);
        if (stds[j] < 1e-10) stds[j] = 1.0;
    }
    
    // Normalizează
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < d; j++) {
            X[i][j] = (X[i][j] - means[j]) / stds[j];
        }
    }
}

// ============================================================================
// FUNCȚIA PRINCIPALĂ
// ============================================================================

int main(int argc, char *argv[]) {
    srand(42);
    
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║          SOLUȚIE: Ridge Regression (Regularizare L2)           ║\n");
    printf("║          Săptămâna 18 - Machine Learning Fundamentals          ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n");
    
    // === 1. Încărcare date ===
    const char *filename = (argc > 1) ? argv[1] : "data/housing_simple.csv";
    int max_n = MAX_SAMPLES;
    int n_features = 3;
    
    double **X = (double **)malloc(max_n * sizeof(double *));
    for (int i = 0; i < max_n; i++) {
        X[i] = (double *)malloc(n_features * sizeof(double));
    }
    double *y = (double *)malloc(max_n * sizeof(double));
    
    int n = load_housing_data(filename, X, y, max_n);
    if (n <= 0) {
        printf("EROARE: Nu s-au putut încărca date!\n");
        return 1;
    }
    printf("\n  Date încărcate: %d exemple, %d caracteristici\n", n, n_features);
    
    // === 2. Normalizare ===
    double *means = (double *)malloc(n_features * sizeof(double));
    double *stds = (double *)malloc(n_features * sizeof(double));
    normalize_data(X, n, n_features, means, stds);
    printf("  Date normalizate (Z-score)\n");
    
    // === 3. Comparație diferite valori de λ ===
    printf("\n═══════════════════════════════════════════════════════\n");
    printf("  Comparație Ridge Regression pentru diferite λ\n");
    printf("═══════════════════════════════════════════════════════\n\n");
    
    double lambdas[] = {0.0, 0.001, 0.01, 0.1, 1.0, 10.0};
    int n_lambdas = sizeof(lambdas) / sizeof(lambdas[0]);
    
    printf("  ┌────────┬──────────┬──────────┬────────────────────────────────┐\n");
    printf("  │   λ    │    R²    │  Iters   │  Weights (w1, w2, w3, bias)    │\n");
    printf("  ├────────┼──────────┼──────────┼────────────────────────────────┤\n");
    
    double *y_pred = (double *)malloc(n * sizeof(double));
    
    for (int l = 0; l < n_lambdas; l++) {
        RidgeRegression *model = ridge_create(
            n_features,
            lambdas[l],     // lambda
            0.01,           // learning rate
            10000,          // max iterations
            1e-8            // tolerance
        );
        
        int iters = ridge_fit(model, X, y, n);
        ridge_predict(model, X, n, y_pred);
        double r2 = ridge_r2_score(y, y_pred, n);
        
        printf("  │ %6.3f │  %6.4f  │  %6d  │ (%6.2f, %6.2f, %6.2f, %6.2f) │\n",
               lambdas[l], r2, iters,
               model->weights[0], model->weights[1], model->weights[2], model->bias);
        
        ridge_free(model);
    }
    
    printf("  └────────┴──────────┴──────────┴────────────────────────────────┘\n");
    
    printf("\n  Observații:\n");
    printf("  • λ=0: Linear Regression standard (fără regularizare)\n");
    printf("  • λ↑: Weight-urile devin mai mici (shrinkage)\n");
    printf("  • λ prea mare: Underfitting (R² scade)\n");
    printf("  • Optimal: λ care maximizează R² pe date de test (cross-validation)\n");
    
    // === Cleanup ===
    for (int i = 0; i < max_n; i++) free(X[i]);
    free(X);
    free(y);
    free(y_pred);
    free(means);
    free(stds);
    
    printf("\n✅ Comparație completă!\n\n");
    
    return 0;
}
