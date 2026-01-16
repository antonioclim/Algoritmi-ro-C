/**
 * =============================================================================
 * SĂPTĂMÂNA 18: FUNDAMENTE MACHINE LEARNING ÎN C
 * Exemplu Complet și Demonstrativ
 * =============================================================================
 *
 * Acest exemplu demonstrează implementarea de la zero a algoritmilor ML:
 *   1. Regresie Liniară cu Gradient Descent
 *   2. K-Means Clustering cu K-Means++ Initialization
 *   3. K-Nearest Neighbors (K-NN) Classification
 *   4. Perceptron cu Perceptron Learning Rule
 *   5. Mini MLP pentru rezolvarea XOR (bonus)
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -g -o example1 example1.c -lm
 * Execuție:  ./example1
 * Valgrind:  valgrind --leak-check=full ./example1
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <float.h>

/* =============================================================================
 * PARTEA 1: INFRASTRUCTURĂ MATRICEALĂ
 * =============================================================================
 */

/**
 * Structură pentru matrice (2D array)
 * Stochează datele în format row-major pentru acces eficient
 */
typedef struct {
    double *data;       /* Date stocate liniar */
    size_t rows;        /* Număr de rânduri */
    size_t cols;        /* Număr de coloane */
} Matrix;

/**
 * Structură pentru vector (1D array)
 */
typedef struct {
    double *data;
    size_t size;
} Vector;

/* Macrou pentru acces la element de matrice */
#define MAT_AT(m, i, j) ((m)->data[(i) * (m)->cols + (j)])

/**
 * Creează o matrice nouă
 */
Matrix* matrix_create(size_t rows, size_t cols) {
    Matrix *m = (Matrix*)malloc(sizeof(Matrix));
    if (!m) return NULL;
    
    m->rows = rows;
    m->cols = cols;
    m->data = (double*)calloc(rows * cols, sizeof(double));
    
    if (!m->data) {
        free(m);
        return NULL;
    }
    
    return m;
}

/**
 * Eliberează memoria unei matrice
 */
void matrix_free(Matrix *m) {
    if (m) {
        free(m->data);
        free(m);
    }
}

/**
 * Creează un vector nou
 */
Vector* vector_create(size_t size) {
    Vector *v = (Vector*)malloc(sizeof(Vector));
    if (!v) return NULL;
    
    v->size = size;
    v->data = (double*)calloc(size, sizeof(double));
    
    if (!v->data) {
        free(v);
        return NULL;
    }
    
    return v;
}

/**
 * Eliberează memoria unui vector
 */
void vector_free(Vector *v) {
    if (v) {
        free(v->data);
        free(v);
    }
}

/**
 * Calculează produsul scalar între doi vectori
 */
double vector_dot(const Vector *a, const Vector *b) {
    if (a->size != b->size) return 0.0;
    
    double sum = 0.0;
    for (size_t i = 0; i < a->size; i++) {
        sum += a->data[i] * b->data[i];
    }
    return sum;
}

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
 * Normalizare Min-Max pentru o matrice
 * Scalează fiecare coloană în intervalul [0, 1]
 */
void normalize_minmax(Matrix *m) {
    for (size_t j = 0; j < m->cols; j++) {
        double min_val = DBL_MAX;
        double max_val = -DBL_MAX;
        
        /* Găsește min și max pentru coloana j */
        for (size_t i = 0; i < m->rows; i++) {
            double val = MAT_AT(m, i, j);
            if (val < min_val) min_val = val;
            if (val > max_val) max_val = val;
        }
        
        /* Normalizează */
        double range = max_val - min_val;
        if (range > 1e-10) {
            for (size_t i = 0; i < m->rows; i++) {
                MAT_AT(m, i, j) = (MAT_AT(m, i, j) - min_val) / range;
            }
        }
    }
}

/* =============================================================================
 * PARTEA 2: REGRESIE LINIARĂ CU GRADIENT DESCENT
 * =============================================================================
 */

/**
 * Model de Regresie Liniară
 * ŷ = w·x + b
 */
typedef struct {
    Vector *weights;        /* Ponderi w */
    double bias;            /* Termen liber b */
    double learning_rate;   /* Rata de învățare α */
    size_t num_features;    /* Număr de features */
} LinearRegression;

/**
 * Creează un model de regresie liniară
 */
LinearRegression* lr_create(size_t num_features, double learning_rate) {
    LinearRegression *model = (LinearRegression*)malloc(sizeof(LinearRegression));
    if (!model) return NULL;
    
    model->weights = vector_create(num_features);
    if (!model->weights) {
        free(model);
        return NULL;
    }
    
    /* Inițializare ponderi cu valori mici random */
    for (size_t i = 0; i < num_features; i++) {
        model->weights->data[i] = ((double)rand() / RAND_MAX) * 0.01;
    }
    
    model->bias = 0.0;
    model->learning_rate = learning_rate;
    model->num_features = num_features;
    
    return model;
}

/**
 * Eliberează modelul
 */
void lr_free(LinearRegression *model) {
    if (model) {
        vector_free(model->weights);
        free(model);
    }
}

/**
 * Predicție pentru un singur sample
 * ŷ = w·x + b
 */
double lr_predict_single(const LinearRegression *model, const double *features) {
    double prediction = model->bias;
    for (size_t i = 0; i < model->num_features; i++) {
        prediction += model->weights->data[i] * features[i];
    }
    return prediction;
}

/**
 * Calculează Mean Squared Error
 * MSE = (1/n) * Σ(ŷᵢ - yᵢ)²
 */
double lr_compute_mse(const LinearRegression *model, const Matrix *X, const double *y) {
    double total_error = 0.0;
    
    for (size_t i = 0; i < X->rows; i++) {
        double prediction = lr_predict_single(model, &MAT_AT(X, i, 0));
        double error = prediction - y[i];
        total_error += error * error;
    }
    
    return total_error / X->rows;
}

/**
 * Antrenare cu Gradient Descent pentru o singură epocă
 * Actualizează ponderile folosind gradientul MSE
 */
void lr_train_epoch(LinearRegression *model, const Matrix *X, const double *y) {
    size_t n = X->rows;
    double *grad_w = (double*)calloc(model->num_features, sizeof(double));
    double grad_b = 0.0;
    
    /* Calculează gradienții */
    for (size_t i = 0; i < n; i++) {
        double prediction = lr_predict_single(model, &MAT_AT(X, i, 0));
        double error = prediction - y[i];
        
        for (size_t j = 0; j < model->num_features; j++) {
            grad_w[j] += error * MAT_AT(X, i, j);
        }
        grad_b += error;
    }
    
    /* Actualizează ponderile */
    for (size_t j = 0; j < model->num_features; j++) {
        model->weights->data[j] -= model->learning_rate * (2.0 / n) * grad_w[j];
    }
    model->bias -= model->learning_rate * (2.0 / n) * grad_b;
    
    free(grad_w);
}

/**
 * Antrenare completă
 */
void lr_fit(LinearRegression *model, const Matrix *X, const double *y, 
            size_t epochs, bool verbose) {
    for (size_t epoch = 0; epoch < epochs; epoch++) {
        lr_train_epoch(model, X, y);
        
        if (verbose && (epoch % 100 == 0 || epoch == epochs - 1)) {
            double mse = lr_compute_mse(model, X, y);
            printf("  Epoca %4zu: MSE = %.6f\n", epoch, mse);
        }
    }
}

/**
 * Calculează R² score (coeficient de determinare)
 */
double lr_r2_score(const LinearRegression *model, const Matrix *X, const double *y) {
    double ss_res = 0.0;  /* Suma pătratelor reziduale */
    double ss_tot = 0.0;  /* Suma pătratelor totale */
    double mean_y = 0.0;
    
    /* Calculează media lui y */
    for (size_t i = 0; i < X->rows; i++) {
        mean_y += y[i];
    }
    mean_y /= X->rows;
    
    /* Calculează SS_res și SS_tot */
    for (size_t i = 0; i < X->rows; i++) {
        double prediction = lr_predict_single(model, &MAT_AT(X, i, 0));
        ss_res += (y[i] - prediction) * (y[i] - prediction);
        ss_tot += (y[i] - mean_y) * (y[i] - mean_y);
    }
    
    return 1.0 - (ss_res / ss_tot);
}

/* =============================================================================
 * PARTEA 3: K-MEANS CLUSTERING
 * =============================================================================
 */

/**
 * Model K-Means
 */
typedef struct {
    Matrix *centroids;      /* Centroizii clusterelor (k × num_features) */
    int *assignments;       /* Asignarea fiecărui punct la un cluster */
    size_t k;               /* Numărul de clustere */
    size_t num_features;    /* Dimensiunea feature-ilor */
    double inertia;         /* Within-cluster sum of squares */
} KMeans;

/**
 * Creează un model K-Means
 */
KMeans* kmeans_create(size_t k, size_t num_features) {
    KMeans *model = (KMeans*)malloc(sizeof(KMeans));
    if (!model) return NULL;
    
    model->centroids = matrix_create(k, num_features);
    model->assignments = NULL;
    model->k = k;
    model->num_features = num_features;
    model->inertia = 0.0;
    
    if (!model->centroids) {
        free(model);
        return NULL;
    }
    
    return model;
}

/**
 * Eliberează modelul
 */
void kmeans_free(KMeans *model) {
    if (model) {
        matrix_free(model->centroids);
        free(model->assignments);
        free(model);
    }
}

/**
 * Inițializare K-Means++ pentru centroizi
 * Alegere probabilistică proporțională cu D(x)²
 */
void kmeans_init_plusplus(KMeans *model, const Matrix *X) {
    size_t n = X->rows;
    double *min_distances = (double*)malloc(n * sizeof(double));
    
    /* Pasul 1: Primul centroid aleator */
    size_t first_idx = rand() % n;
    for (size_t j = 0; j < model->num_features; j++) {
        MAT_AT(model->centroids, 0, j) = MAT_AT(X, first_idx, j);
    }
    
    /* Inițializează distanțele */
    for (size_t i = 0; i < n; i++) {
        min_distances[i] = euclidean_distance(&MAT_AT(X, i, 0), 
                                               &MAT_AT(model->centroids, 0, 0),
                                               model->num_features);
        min_distances[i] *= min_distances[i];  /* D(x)² */
    }
    
    /* Pasul 2-k: Alege centroizii rămași */
    for (size_t c = 1; c < model->k; c++) {
        /* Calculează suma totală */
        double total = 0.0;
        for (size_t i = 0; i < n; i++) {
            total += min_distances[i];
        }
        
        /* Alege proporțional cu D(x)² */
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
        
        /* Setează noul centroid */
        for (size_t j = 0; j < model->num_features; j++) {
            MAT_AT(model->centroids, c, j) = MAT_AT(X, chosen, j);
        }
        
        /* Actualizează distanțele minime */
        for (size_t i = 0; i < n; i++) {
            double dist = euclidean_distance(&MAT_AT(X, i, 0),
                                             &MAT_AT(model->centroids, c, 0),
                                             model->num_features);
            dist *= dist;
            if (dist < min_distances[i]) {
                min_distances[i] = dist;
            }
        }
    }
    
    free(min_distances);
}

/**
 * Asignează fiecare punct la cel mai apropiat centroid
 * Returnează true dacă asignările s-au schimbat
 */
bool kmeans_assign(KMeans *model, const Matrix *X) {
    bool changed = false;
    model->inertia = 0.0;
    
    for (size_t i = 0; i < X->rows; i++) {
        double min_dist = DBL_MAX;
        int best_cluster = 0;
        
        for (size_t c = 0; c < model->k; c++) {
            double dist = euclidean_distance(&MAT_AT(X, i, 0),
                                             &MAT_AT(model->centroids, c, 0),
                                             model->num_features);
            if (dist < min_dist) {
                min_dist = dist;
                best_cluster = (int)c;
            }
        }
        
        if (model->assignments[i] != best_cluster) {
            changed = true;
            model->assignments[i] = best_cluster;
        }
        
        model->inertia += min_dist * min_dist;
    }
    
    return changed;
}

/**
 * Actualizează centroizii ca media punctelor asignate
 */
void kmeans_update_centroids(KMeans *model, const Matrix *X) {
    /* Resetează centroizii */
    for (size_t c = 0; c < model->k; c++) {
        for (size_t j = 0; j < model->num_features; j++) {
            MAT_AT(model->centroids, c, j) = 0.0;
        }
    }
    
    /* Numără punctele per cluster */
    int *counts = (int*)calloc(model->k, sizeof(int));
    
    /* Sumează coordonatele */
    for (size_t i = 0; i < X->rows; i++) {
        int c = model->assignments[i];
        counts[c]++;
        for (size_t j = 0; j < model->num_features; j++) {
            MAT_AT(model->centroids, c, j) += MAT_AT(X, i, j);
        }
    }
    
    /* Calculează media */
    for (size_t c = 0; c < model->k; c++) {
        if (counts[c] > 0) {
            for (size_t j = 0; j < model->num_features; j++) {
                MAT_AT(model->centroids, c, j) /= counts[c];
            }
        }
    }
    
    free(counts);
}

/**
 * Antrenare K-Means
 */
void kmeans_fit(KMeans *model, const Matrix *X, size_t max_iter, bool verbose) {
    /* Alocă asignări */
    model->assignments = (int*)malloc(X->rows * sizeof(int));
    for (size_t i = 0; i < X->rows; i++) {
        model->assignments[i] = -1;
    }
    
    /* Inițializare K-Means++ */
    kmeans_init_plusplus(model, X);
    
    /* Iterații Lloyd */
    for (size_t iter = 0; iter < max_iter; iter++) {
        bool changed = kmeans_assign(model, X);
        
        if (verbose) {
            printf("  Iterația %3zu: Inerție = %.4f\n", iter, model->inertia);
        }
        
        if (!changed) {
            if (verbose) {
                printf("  Convergență atinsă la iterația %zu\n", iter);
            }
            break;
        }
        
        kmeans_update_centroids(model, X);
    }
}

/* =============================================================================
 * PARTEA 4: K-NEAREST NEIGHBORS
 * =============================================================================
 */

/**
 * Clasificator K-NN
 */
typedef struct {
    Matrix *X_train;        /* Date de antrenament */
    int *y_train;           /* Etichete de antrenament */
    size_t n_train;         /* Număr de sample-uri */
    int k;                  /* Număr de vecini */
} KNNClassifier;

/**
 * Creează clasificator K-NN
 */
KNNClassifier* knn_create(int k) {
    KNNClassifier *model = (KNNClassifier*)malloc(sizeof(KNNClassifier));
    if (!model) return NULL;
    
    model->X_train = NULL;
    model->y_train = NULL;
    model->n_train = 0;
    model->k = k;
    
    return model;
}

/**
 * Eliberează clasificatorul
 */
void knn_free(KNNClassifier *model) {
    if (model) {
        matrix_free(model->X_train);
        free(model->y_train);
        free(model);
    }
}

/**
 * "Antrenare" K-NN - doar memorează datele
 */
void knn_fit(KNNClassifier *model, const Matrix *X, const int *y) {
    model->n_train = X->rows;
    
    /* Copiază X */
    model->X_train = matrix_create(X->rows, X->cols);
    memcpy(model->X_train->data, X->data, X->rows * X->cols * sizeof(double));
    
    /* Copiază y */
    model->y_train = (int*)malloc(X->rows * sizeof(int));
    memcpy(model->y_train, y, X->rows * sizeof(int));
}

/**
 * Predicție pentru un singur sample
 */
int knn_predict_single(const KNNClassifier *model, const double *x) {
    size_t num_features = model->X_train->cols;
    
    /* Structură pentru distanță și etichetă */
    typedef struct {
        double distance;
        int label;
    } DistLabel;
    
    DistLabel *distances = (DistLabel*)malloc(model->n_train * sizeof(DistLabel));
    
    /* Calculează distanțele */
    for (size_t i = 0; i < model->n_train; i++) {
        distances[i].distance = euclidean_distance(x, &MAT_AT(model->X_train, i, 0), 
                                                   num_features);
        distances[i].label = model->y_train[i];
    }
    
    /* Sortare după distanță (bubble sort simplificat pentru demo) */
    for (size_t i = 0; i < model->n_train - 1; i++) {
        for (size_t j = 0; j < model->n_train - i - 1; j++) {
            if (distances[j].distance > distances[j + 1].distance) {
                DistLabel temp = distances[j];
                distances[j] = distances[j + 1];
                distances[j + 1] = temp;
            }
        }
    }
    
    /* Votare majoritară pe primii k vecini */
    int max_label = 0;
    for (int i = 0; i < model->k; i++) {
        if (distances[i].label > max_label) max_label = distances[i].label;
    }
    
    int *votes = (int*)calloc(max_label + 1, sizeof(int));
    for (int i = 0; i < model->k; i++) {
        votes[distances[i].label]++;
    }
    
    int best_label = 0;
    int best_count = 0;
    for (int i = 0; i <= max_label; i++) {
        if (votes[i] > best_count) {
            best_count = votes[i];
            best_label = i;
        }
    }
    
    free(distances);
    free(votes);
    
    return best_label;
}

/**
 * Calculează accuracy pe un set de test
 */
double knn_accuracy(const KNNClassifier *model, const Matrix *X_test, const int *y_test) {
    int correct = 0;
    
    for (size_t i = 0; i < X_test->rows; i++) {
        int predicted = knn_predict_single(model, &MAT_AT(X_test, i, 0));
        if (predicted == y_test[i]) {
            correct++;
        }
    }
    
    return (double)correct / X_test->rows;
}

/* =============================================================================
 * PARTEA 5: PERCEPTRON
 * =============================================================================
 */

/**
 * Model Perceptron
 */
typedef struct {
    Vector *weights;        /* Ponderi */
    double bias;            /* Bias */
    double learning_rate;   /* Rata de învățare */
} Perceptron;

/**
 * Creează un Perceptron
 */
Perceptron* perceptron_create(size_t num_features, double learning_rate) {
    Perceptron *model = (Perceptron*)malloc(sizeof(Perceptron));
    if (!model) return NULL;
    
    model->weights = vector_create(num_features);
    if (!model->weights) {
        free(model);
        return NULL;
    }
    
    /* Inițializare cu zerouri */
    model->bias = 0.0;
    model->learning_rate = learning_rate;
    
    return model;
}

/**
 * Eliberează Perceptronul
 */
void perceptron_free(Perceptron *model) {
    if (model) {
        vector_free(model->weights);
        free(model);
    }
}

/**
 * Predicție: sign(w·x + b)
 * Returnează +1 sau -1
 */
int perceptron_predict(const Perceptron *model, const double *x) {
    double sum = model->bias;
    for (size_t i = 0; i < model->weights->size; i++) {
        sum += model->weights->data[i] * x[i];
    }
    return sum >= 0 ? 1 : -1;
}

/**
 * Antrenare pentru o epocă
 * Returnează numărul de erori
 */
int perceptron_train_epoch(Perceptron *model, const Matrix *X, const int *y) {
    int errors = 0;
    
    for (size_t i = 0; i < X->rows; i++) {
        int prediction = perceptron_predict(model, &MAT_AT(X, i, 0));
        
        /* Dacă e greșit, actualizează */
        if (prediction != y[i]) {
            errors++;
            for (size_t j = 0; j < model->weights->size; j++) {
                model->weights->data[j] += model->learning_rate * y[i] * MAT_AT(X, i, j);
            }
            model->bias += model->learning_rate * y[i];
        }
    }
    
    return errors;
}

/**
 * Antrenare completă
 * Returnează true dacă a convergit
 */
bool perceptron_fit(Perceptron *model, const Matrix *X, const int *y,
                    size_t max_epochs, bool verbose) {
    for (size_t epoch = 0; epoch < max_epochs; epoch++) {
        int errors = perceptron_train_epoch(model, X, y);
        
        if (verbose && (epoch % 10 == 0 || errors == 0)) {
            printf("  Epoca %3zu: %d erori\n", epoch, errors);
        }
        
        if (errors == 0) {
            if (verbose) {
                printf("  Convergență la epoca %zu!\n", epoch);
            }
            return true;
        }
    }
    
    return false;
}

/* =============================================================================
 * PARTEA 6: MINI MLP PENTRU XOR (BONUS)
 * =============================================================================
 */

/**
 * MLP cu un layer ascuns
 * Arhitectură: 2 → hidden_size → 1
 */
typedef struct {
    Matrix *W1;             /* Ponderi input → hidden */
    Vector *b1;             /* Bias hidden */
    Matrix *W2;             /* Ponderi hidden → output */
    double b2;              /* Bias output */
    size_t hidden_size;
    double learning_rate;
} MiniMLP;

/**
 * Funcția sigmoid
 */
double sigmoid(double x) {
    return 1.0 / (1.0 + exp(-x));
}

/**
 * Derivata sigmoid
 */
double sigmoid_derivative(double sigmoid_output) {
    return sigmoid_output * (1.0 - sigmoid_output);
}

/**
 * Creează un MLP minimal
 */
MiniMLP* mlp_create(size_t input_size, size_t hidden_size, double learning_rate) {
    MiniMLP *model = (MiniMLP*)malloc(sizeof(MiniMLP));
    if (!model) return NULL;
    
    model->hidden_size = hidden_size;
    model->learning_rate = learning_rate;
    
    model->W1 = matrix_create(hidden_size, input_size);
    model->b1 = vector_create(hidden_size);
    model->W2 = matrix_create(1, hidden_size);
    model->b2 = 0.0;
    
    /* Inițializare random */
    for (size_t i = 0; i < hidden_size * input_size; i++) {
        model->W1->data[i] = ((double)rand() / RAND_MAX - 0.5) * 2.0;
    }
    for (size_t i = 0; i < hidden_size; i++) {
        model->W2->data[i] = ((double)rand() / RAND_MAX - 0.5) * 2.0;
    }
    
    return model;
}

/**
 * Eliberează MLP
 */
void mlp_free(MiniMLP *model) {
    if (model) {
        matrix_free(model->W1);
        vector_free(model->b1);
        matrix_free(model->W2);
        free(model);
    }
}

/**
 * Forward pass
 */
double mlp_forward(const MiniMLP *model, const double *x, double *hidden_output) {
    /* Hidden layer */
    for (size_t h = 0; h < model->hidden_size; h++) {
        double sum = model->b1->data[h];
        for (size_t i = 0; i < model->W1->cols; i++) {
            sum += MAT_AT(model->W1, h, i) * x[i];
        }
        hidden_output[h] = sigmoid(sum);
    }
    
    /* Output layer */
    double output = model->b2;
    for (size_t h = 0; h < model->hidden_size; h++) {
        output += model->W2->data[h] * hidden_output[h];
    }
    
    return sigmoid(output);
}

/**
 * Antrenare MLP pentru o epocă (backpropagation)
 */
double mlp_train_epoch(MiniMLP *model, const Matrix *X, const double *y) {
    double total_loss = 0.0;
    double *hidden = (double*)malloc(model->hidden_size * sizeof(double));
    
    for (size_t i = 0; i < X->rows; i++) {
        /* Forward */
        double output = mlp_forward(model, &MAT_AT(X, i, 0), hidden);
        double error = output - y[i];
        total_loss += error * error;
        
        /* Backward - output layer */
        double d_output = error * sigmoid_derivative(output);
        
        /* Update W2, b2 */
        for (size_t h = 0; h < model->hidden_size; h++) {
            model->W2->data[h] -= model->learning_rate * d_output * hidden[h];
        }
        model->b2 -= model->learning_rate * d_output;
        
        /* Backward - hidden layer */
        for (size_t h = 0; h < model->hidden_size; h++) {
            double d_hidden = d_output * model->W2->data[h] * sigmoid_derivative(hidden[h]);
            
            /* Update W1, b1 */
            for (size_t j = 0; j < model->W1->cols; j++) {
                MAT_AT(model->W1, h, j) -= model->learning_rate * d_hidden * MAT_AT(X, i, j);
            }
            model->b1->data[h] -= model->learning_rate * d_hidden;
        }
    }
    
    free(hidden);
    return total_loss / X->rows;
}

/* =============================================================================
 * UTILITĂȚI DE AFIȘARE
 * =============================================================================
 */

void print_separator(const char *title) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════════════╗\n");
    printf("║  %-73s║\n", title);
    printf("╚═══════════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

void print_subsection(const char *title) {
    printf("\n┌─────────────────────────────────────────────────────────────────────────────┐\n");
    printf("│  %s\n", title);
    printf("└─────────────────────────────────────────────────────────────────────────────┘\n\n");
}

/* =============================================================================
 * MAIN ȘI DEMONSTRAȚII
 * =============================================================================
 */

/**
 * Demo Regresie Liniară
 */
void demo_linear_regression(void) {
    print_separator("DEMO 1: REGRESIE LINIARĂ CU GRADIENT DESCENT");
    
    printf("Problema: Predicția prețului caselor bazat pe suprafață\n");
    printf("Model: preț = w * suprafață + b\n\n");
    
    /* Date de antrenament (suprafață în 100 mp, preț în 100k EUR) */
    double X_data[] = {1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0};
    double y_data[] = {1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.2, 5.5};
    size_t n = 9;
    
    Matrix *X = matrix_create(n, 1);
    for (size_t i = 0; i < n; i++) {
        MAT_AT(X, i, 0) = X_data[i];
    }
    
    /* Creează și antrenează modelul */
    LinearRegression *model = lr_create(1, 0.01);
    
    printf("Antrenare (500 epoci):\n");
    lr_fit(model, X, y_data, 500, true);
    
    printf("\nParametri învățați:\n");
    printf("  w = %.4f (panta)\n", model->weights->data[0]);
    printf("  b = %.4f (intercept)\n", model->bias);
    printf("  R² = %.4f\n", lr_r2_score(model, X, y_data));
    
    printf("\nPredicții:\n");
    for (size_t i = 0; i < n; i += 2) {
        double pred = lr_predict_single(model, &X_data[i]);
        printf("  Suprafață: %.1f × 100mp → Preț: %.2f (actual: %.2f)\n",
               X_data[i], pred, y_data[i]);
    }
    
    lr_free(model);
    matrix_free(X);
}

/**
 * Demo K-Means
 */
void demo_kmeans(void) {
    print_separator("DEMO 2: K-MEANS CLUSTERING CU K-MEANS++ INIT");
    
    printf("Date: 3 clustere 2D generate artificial\n\n");
    
    /* Generează date cu 3 clustere distincte */
    size_t n = 30;  /* 10 puncte per cluster */
    Matrix *X = matrix_create(n, 2);
    
    srand(42);
    
    /* Cluster 1: centrat în (2, 2) */
    for (size_t i = 0; i < 10; i++) {
        MAT_AT(X, i, 0) = 2.0 + ((double)rand() / RAND_MAX - 0.5);
        MAT_AT(X, i, 1) = 2.0 + ((double)rand() / RAND_MAX - 0.5);
    }
    
    /* Cluster 2: centrat în (8, 2) */
    for (size_t i = 10; i < 20; i++) {
        MAT_AT(X, i, 0) = 8.0 + ((double)rand() / RAND_MAX - 0.5);
        MAT_AT(X, i, 1) = 2.0 + ((double)rand() / RAND_MAX - 0.5);
    }
    
    /* Cluster 3: centrat în (5, 8) */
    for (size_t i = 20; i < 30; i++) {
        MAT_AT(X, i, 0) = 5.0 + ((double)rand() / RAND_MAX - 0.5);
        MAT_AT(X, i, 1) = 8.0 + ((double)rand() / RAND_MAX - 0.5);
    }
    
    /* Creează și antrenează K-Means */
    KMeans *model = kmeans_create(3, 2);
    
    printf("Antrenare K-Means (k=3):\n");
    kmeans_fit(model, X, 100, true);
    
    printf("\nCentroizi finali:\n");
    for (size_t c = 0; c < model->k; c++) {
        printf("  Cluster %zu: (%.2f, %.2f)\n", c,
               MAT_AT(model->centroids, c, 0),
               MAT_AT(model->centroids, c, 1));
    }
    
    printf("\nInerție finală: %.4f\n", model->inertia);
    
    /* Contorizează puncte per cluster */
    int counts[3] = {0, 0, 0};
    for (size_t i = 0; i < n; i++) {
        counts[model->assignments[i]]++;
    }
    printf("\nDistribuție puncte: [%d, %d, %d]\n", counts[0], counts[1], counts[2]);
    
    kmeans_free(model);
    matrix_free(X);
}

/**
 * Demo K-NN
 */
void demo_knn(void) {
    print_separator("DEMO 3: K-NEAREST NEIGHBORS CLASSIFICATION");
    
    printf("Date: Clasificare simplă 2D cu 2 clase\n\n");
    
    /* Date de antrenament */
    double X_data[] = {
        1.0, 1.0,   /* Clasa 0 */
        1.5, 2.0,
        2.0, 1.5,
        6.0, 5.0,   /* Clasa 1 */
        7.0, 6.0,
        6.5, 5.5
    };
    int y_data[] = {0, 0, 0, 1, 1, 1};
    
    Matrix *X_train = matrix_create(6, 2);
    memcpy(X_train->data, X_data, 12 * sizeof(double));
    
    /* Creează și antrenează K-NN */
    KNNClassifier *model = knn_create(3);
    knn_fit(model, X_train, y_data);
    
    printf("K-NN cu k=3 antrenat pe %zu puncte\n\n", model->n_train);
    
    /* Teste */
    double test_points[][2] = {
        {1.0, 1.5},   /* Ar trebui să fie 0 */
        {6.0, 6.0},   /* Ar trebui să fie 1 */
        {3.5, 3.5}    /* Nedeterminat */
    };
    
    printf("Predicții pe puncte de test:\n");
    for (int i = 0; i < 3; i++) {
        int pred = knn_predict_single(model, test_points[i]);
        printf("  (%.1f, %.1f) → Clasa %d\n", 
               test_points[i][0], test_points[i][1], pred);
    }
    
    knn_free(model);
    matrix_free(X_train);
}

/**
 * Demo Perceptron
 */
void demo_perceptron(void) {
    print_separator("DEMO 4: PERCEPTRON - AND, OR, XOR");
    
    /* Date pentru porți logice */
    double X_data[] = {0, 0, 0, 1, 1, 0, 1, 1};
    
    Matrix *X = matrix_create(4, 2);
    memcpy(X->data, X_data, 8 * sizeof(double));
    
    /* AND: {-1, -1, -1, +1} */
    print_subsection("Poarta AND");
    int y_and[] = {-1, -1, -1, 1};
    
    Perceptron *p_and = perceptron_create(2, 0.1);
    bool conv_and = perceptron_fit(p_and, X, y_and, 100, true);
    
    printf("\nRezultate AND:\n");
    for (int i = 0; i < 4; i++) {
        int pred = perceptron_predict(p_and, &X_data[i * 2]);
        printf("  (%d, %d) → %d (expected: %d)\n", 
               (int)X_data[i*2], (int)X_data[i*2+1], 
               pred > 0 ? 1 : 0, y_and[i] > 0 ? 1 : 0);
    }
    printf("Converge: %s\n", conv_and ? "DA" : "NU");
    perceptron_free(p_and);
    
    /* OR: {-1, +1, +1, +1} */
    print_subsection("Poarta OR");
    int y_or[] = {-1, 1, 1, 1};
    
    Perceptron *p_or = perceptron_create(2, 0.1);
    bool conv_or = perceptron_fit(p_or, X, y_or, 100, true);
    
    printf("\nRezultate OR:\n");
    for (int i = 0; i < 4; i++) {
        int pred = perceptron_predict(p_or, &X_data[i * 2]);
        printf("  (%d, %d) → %d (expected: %d)\n",
               (int)X_data[i*2], (int)X_data[i*2+1],
               pred > 0 ? 1 : 0, y_or[i] > 0 ? 1 : 0);
    }
    printf("Converge: %s\n", conv_or ? "DA" : "NU");
    perceptron_free(p_or);
    
    /* XOR: {-1, +1, +1, -1} - NU VA CONVERGE! */
    print_subsection("Poarta XOR (NU este liniar separabilă!)");
    int y_xor[] = {-1, 1, 1, -1};
    
    Perceptron *p_xor = perceptron_create(2, 0.1);
    bool conv_xor = perceptron_fit(p_xor, X, y_xor, 100, true);
    
    printf("\nRezultate XOR:\n");
    for (int i = 0; i < 4; i++) {
        int pred = perceptron_predict(p_xor, &X_data[i * 2]);
        printf("  (%d, %d) → %d (expected: %d) %s\n",
               (int)X_data[i*2], (int)X_data[i*2+1],
               pred > 0 ? 1 : 0, y_xor[i] > 0 ? 1 : 0,
               (pred == y_xor[i]) ? "✓" : "✗");
    }
    printf("Converge: %s (EXPECTED: NU - XOR NU este liniar separabil!)\n", 
           conv_xor ? "DA" : "NU");
    perceptron_free(p_xor);
    
    matrix_free(X);
}

/**
 * Demo MLP rezolvă XOR
 */
void demo_mlp_xor(void) {
    print_separator("DEMO 5: MINI MLP REZOLVĂ XOR");
    
    printf("Arhitectură: 2 → 4 (hidden) → 1\n");
    printf("Activare: sigmoid\n\n");
    
    double X_data[] = {0, 0, 0, 1, 1, 0, 1, 1};
    double y_data[] = {0, 1, 1, 0};
    
    Matrix *X = matrix_create(4, 2);
    memcpy(X->data, X_data, 8 * sizeof(double));
    
    MiniMLP *model = mlp_create(2, 4, 1.0);
    
    printf("Antrenare (5000 epoci):\n");
    double *hidden = (double*)malloc(4 * sizeof(double));
    
    for (int epoch = 0; epoch < 5000; epoch++) {
        double loss = mlp_train_epoch(model, X, y_data);
        if (epoch % 1000 == 0 || epoch == 4999) {
            printf("  Epoca %4d: Loss = %.6f\n", epoch, loss);
        }
    }
    
    printf("\nRezultate XOR cu MLP:\n");
    for (int i = 0; i < 4; i++) {
        double pred = mlp_forward(model, &X_data[i * 2], hidden);
        printf("  (%d, %d) → %.3f (rounded: %d, expected: %d) %s\n",
               (int)X_data[i*2], (int)X_data[i*2+1],
               pred, pred > 0.5 ? 1 : 0, (int)y_data[i],
               ((pred > 0.5 ? 1 : 0) == (int)y_data[i]) ? "✓" : "✗");
    }
    
    printf("\n✓ MLP REZOLVĂ XOR! (perceptronul simplu nu poate)\n");
    
    free(hidden);
    mlp_free(model);
    matrix_free(X);
}

/**
 * Comparație cu Python/sklearn
 */
void print_python_comparison(void) {
    print_separator("COMPARAȚIE CU PYTHON/SCIKIT-LEARN");
    
    printf("Echivalentul în Python pentru algoritmii implementați:\n\n");
    
    printf("┌────────────────────────────────────────────────────────────────────────┐\n");
    printf("│  REGRESIE LINIARĂ                                                      │\n");
    printf("├────────────────────────────────────────────────────────────────────────┤\n");
    printf("│  from sklearn.linear_model import LinearRegression                     │\n");
    printf("│  model = LinearRegression()                                            │\n");
    printf("│  model.fit(X, y)                                                       │\n");
    printf("│  predictions = model.predict(X_test)                                   │\n");
    printf("└────────────────────────────────────────────────────────────────────────┘\n\n");
    
    printf("┌────────────────────────────────────────────────────────────────────────┐\n");
    printf("│  K-MEANS CLUSTERING                                                    │\n");
    printf("├────────────────────────────────────────────────────────────────────────┤\n");
    printf("│  from sklearn.cluster import KMeans                                    │\n");
    printf("│  model = KMeans(n_clusters=3, init='k-means++')                        │\n");
    printf("│  model.fit(X)                                                          │\n");
    printf("│  labels = model.labels_                                                │\n");
    printf("└────────────────────────────────────────────────────────────────────────┘\n\n");
    
    printf("┌────────────────────────────────────────────────────────────────────────┐\n");
    printf("│  K-NEAREST NEIGHBORS                                                   │\n");
    printf("├────────────────────────────────────────────────────────────────────────┤\n");
    printf("│  from sklearn.neighbors import KNeighborsClassifier                    │\n");
    printf("│  model = KNeighborsClassifier(n_neighbors=5)                           │\n");
    printf("│  model.fit(X_train, y_train)                                           │\n");
    printf("│  predictions = model.predict(X_test)                                   │\n");
    printf("└────────────────────────────────────────────────────────────────────────┘\n\n");
    
    printf("┌────────────────────────────────────────────────────────────────────────┐\n");
    printf("│  PERCEPTRON                                                            │\n");
    printf("├────────────────────────────────────────────────────────────────────────┤\n");
    printf("│  from sklearn.linear_model import Perceptron                           │\n");
    printf("│  model = Perceptron(max_iter=1000)                                     │\n");
    printf("│  model.fit(X_train, y_train)                                           │\n");
    printf("│  predictions = model.predict(X_test)                                   │\n");
    printf("└────────────────────────────────────────────────────────────────────────┘\n");
}

int main(void) {
    /* Inițializare generator random */
    srand((unsigned int)time(NULL));
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║     SĂPTĂMÂNA 18: FUNDAMENTE MACHINE LEARNING ÎN C                            ║\n");
    printf("║     Algoritmi și Tehnici de Programare (ATP) - ASE CSIE                       ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    /* Rulează toate demo-urile */
    demo_linear_regression();
    demo_kmeans();
    demo_knn();
    demo_perceptron();
    demo_mlp_xor();
    print_python_comparison();
    
    print_separator("TOATE DEMONSTRAȚIILE COMPLETATE CU SUCCES!");
    
    printf("Recapitulare algoritmi implementați:\n");
    printf("  ✓ Regresie Liniară cu Gradient Descent\n");
    printf("  ✓ K-Means Clustering cu K-Means++ Initialization\n");
    printf("  ✓ K-Nearest Neighbors (K-NN) Classification\n");
    printf("  ✓ Perceptron cu Perceptron Learning Rule\n");
    printf("  ✓ Mini MLP pentru rezolvarea XOR\n\n");
    
    printf("Continuați cu exercițiile din src/exercise1.c și src/exercise2.c!\n\n");
    
    return 0;
}
