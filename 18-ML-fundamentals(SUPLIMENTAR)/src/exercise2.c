/**
 * =============================================================================
 * EXERCIȚIUL 2: Multi-class Classification cu Perceptron (One-vs-All)
 * =============================================================================
 *
 * OBIECTIV:
 *   Implementați clasificarea multi-clasă pentru setul de date Iris folosind
 *   strategia One-vs-All cu trei perceptroni (unul pentru fiecare clasă).
 *
 * CERINȚE:
 *   1. Citiți datele din fișierul iris.csv
 *   2. Implementați train/test split (80/20)
 *   3. Antrenați 3 perceptroni (setosa vs rest, versicolor vs rest, virginica vs rest)
 *   4. Implementați predicția combinată (clasa cu scorul maxim)
 *   5. Calculați și afișați accuracy și matricea de confuzie
 *
 * EXEMPLU INPUT (iris.csv):
 *   sepal_length,sepal_width,petal_length,petal_width,species
 *   5.1,3.5,1.4,0.2,setosa
 *   7.0,3.2,4.7,1.4,versicolor
 *   6.3,3.3,6.0,2.5,virginica
 *
 * EXPECTED OUTPUT:
 *   Train/Test split: 120/30
 *   
 *   Antrenare Perceptron One-vs-All:
 *     Perceptron 0 (setosa): Converge în 5 epoci
 *     Perceptron 1 (versicolor): Converge în 23 epoci
 *     Perceptron 2 (virginica): Nu converge (100 epoci)
 *   
 *   Accuracy pe test: 0.93 (28/30)
 *   
 *   Matrice de Confuzie:
 *               Pred 0  Pred 1  Pred 2
 *     Actual 0:   10       0       0
 *     Actual 1:    0       9       1
 *     Actual 2:    0       1       9
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -o exercise2 exercise2.c -lm
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

/* =============================================================================
 * CONSTANTE ȘI DEFINIRI
 * =============================================================================
 */

#define MAX_SAMPLES 200
#define N_FEATURES 4
#define N_CLASSES 3
#define MAX_LINE_LENGTH 256

/* Numele claselor */
const char *CLASS_NAMES[N_CLASSES] = {"setosa", "versicolor", "virginica"};

/* =============================================================================
 * DEFINIREA TIPURILOR
 * =============================================================================
 */

/**
 * TODO 1: Definește structura pentru un sample Iris
 */
typedef struct {
    double features[N_FEATURES];    /* sepal_length, sepal_width, petal_length, petal_width */
    int label;                      /* 0=setosa, 1=versicolor, 2=virginica */
} IrisSample;

/**
 * TODO 2: Definește structura pentru dataset-ul Iris
 */
typedef struct {
    IrisSample *samples;
    size_t count;
} IrisDataset;

/**
 * TODO 3: Definește structura Perceptron
 */
typedef struct {
    double weights[N_FEATURES];     /* Ponderi pentru fiecare feature */
    double bias;                    /* Termen liber */
    double learning_rate;           /* Rata de învățare */
    int target_class;               /* Clasa pentru care e antrenat (OvA) */
} Perceptron;

/**
 * Structură pentru matricea de confuzie
 */
typedef struct {
    int matrix[N_CLASSES][N_CLASSES];  /* [actual][predicted] */
} ConfusionMatrix;

/* =============================================================================
 * FUNCȚII DE CITIRE DATE
 * =============================================================================
 */

/**
 * Convertește numele clasei în index
 */
int class_name_to_index(const char *name) {
    for (int i = 0; i < N_CLASSES; i++) {
        if (strstr(name, CLASS_NAMES[i]) != NULL) {
            return i;
        }
    }
    return -1;
}

/**
 * TODO 4: Implementează citirea datelor Iris din CSV
 *
 * @param filename Calea către iris.csv
 * @return Pointer la IrisDataset sau NULL în caz de eroare
 */
IrisDataset* load_iris_data(const char *filename) {
    /* YOUR CODE HERE */
    
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Eroare: Nu pot deschide %s\n", filename);
        return NULL;
    }
    
    IrisDataset *dataset = (IrisDataset*)malloc(sizeof(IrisDataset));
    dataset->samples = (IrisSample*)malloc(MAX_SAMPLES * sizeof(IrisSample));
    dataset->count = 0;
    
    char line[MAX_LINE_LENGTH];
    
    /* Skip header */
    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);
        free(dataset->samples);
        free(dataset);
        return NULL;
    }
    
    /* Citește liniile */
    while (fgets(line, sizeof(line), file) && dataset->count < MAX_SAMPLES) {
        IrisSample *s = &dataset->samples[dataset->count];
        
        char species[64];
        int parsed = sscanf(line, "%lf,%lf,%lf,%lf,%s",
                            &s->features[0], &s->features[1],
                            &s->features[2], &s->features[3],
                            species);
        
        if (parsed == 5) {
            s->label = class_name_to_index(species);
            if (s->label >= 0) {
                dataset->count++;
            }
        }
    }
    
    fclose(file);
    return dataset;
}

/**
 * Eliberează memoria datasetului
 */
void free_iris_dataset(IrisDataset *dataset) {
    if (dataset) {
        free(dataset->samples);
        free(dataset);
    }
}

/* =============================================================================
 * FUNCȚII DE PREPROCESARE
 * =============================================================================
 */

/**
 * TODO 5: Implementează normalizarea Min-Max pentru features
 *
 * Normalizează fiecare feature în intervalul [0, 1]
 */
void normalize_features(IrisDataset *dataset) {
    /* YOUR CODE HERE */
    
    for (int f = 0; f < N_FEATURES; f++) {
        double min_val = dataset->samples[0].features[f];
        double max_val = dataset->samples[0].features[f];
        
        /* Găsește min și max */
        for (size_t i = 1; i < dataset->count; i++) {
            if (dataset->samples[i].features[f] < min_val) {
                min_val = dataset->samples[i].features[f];
            }
            if (dataset->samples[i].features[f] > max_val) {
                max_val = dataset->samples[i].features[f];
            }
        }
        
        /* Normalizează */
        double range = max_val - min_val;
        if (range < 1e-10) range = 1.0;
        
        for (size_t i = 0; i < dataset->count; i++) {
            dataset->samples[i].features[f] = 
                (dataset->samples[i].features[f] - min_val) / range;
        }
    }
}

/**
 * TODO 6: Implementează amestecarea datelor (shuffle)
 *
 * Folosește algoritmul Fisher-Yates
 */
void shuffle_dataset(IrisDataset *dataset) {
    /* YOUR CODE HERE */
    
    for (size_t i = dataset->count - 1; i > 0; i--) {
        size_t j = rand() % (i + 1);
        
        /* Swap samples[i] cu samples[j] */
        IrisSample temp = dataset->samples[i];
        dataset->samples[i] = dataset->samples[j];
        dataset->samples[j] = temp;
    }
}

/**
 * TODO 7: Implementează train/test split
 *
 * @param dataset Dataset-ul sursă
 * @param train_ratio Procentul pentru antrenare (ex: 0.8 pentru 80%)
 * @param train Output: subset de antrenare
 * @param test Output: subset de test
 */
void train_test_split(IrisDataset *dataset, double train_ratio,
                      IrisDataset *train, IrisDataset *test) {
    /* YOUR CODE HERE */
    
    size_t n_train = (size_t)(dataset->count * train_ratio);
    size_t n_test = dataset->count - n_train;
    
    train->samples = (IrisSample*)malloc(n_train * sizeof(IrisSample));
    train->count = n_train;
    
    test->samples = (IrisSample*)malloc(n_test * sizeof(IrisSample));
    test->count = n_test;
    
    /* Copiază primele n_train în train */
    for (size_t i = 0; i < n_train; i++) {
        train->samples[i] = dataset->samples[i];
    }
    
    /* Restul în test */
    for (size_t i = 0; i < n_test; i++) {
        test->samples[i] = dataset->samples[n_train + i];
    }
}

/* =============================================================================
 * FUNCȚII PERCEPTRON
 * =============================================================================
 */

/**
 * TODO 8: Implementează inițializarea perceptronului
 */
void perceptron_init(Perceptron *p, int target_class, double learning_rate) {
    /* YOUR CODE HERE */
    
    for (int i = 0; i < N_FEATURES; i++) {
        p->weights[i] = ((double)rand() / RAND_MAX - 0.5) * 0.1;
    }
    p->bias = 0.0;
    p->learning_rate = learning_rate;
    p->target_class = target_class;
}

/**
 * TODO 9: Implementează calculul scorului raw (înainte de threshold)
 *
 * Score = w·x + b
 */
double perceptron_raw_score(const Perceptron *p, const double *features) {
    /* YOUR CODE HERE */
    
    double score = p->bias;
    for (int i = 0; i < N_FEATURES; i++) {
        score += p->weights[i] * features[i];
    }
    return score;
}

/**
 * TODO 10: Implementează predicția perceptronului
 *
 * @return +1 dacă scorul >= 0, -1 altfel
 */
int perceptron_predict(const Perceptron *p, const double *features) {
    /* YOUR CODE HERE */
    
    return perceptron_raw_score(p, features) >= 0.0 ? 1 : -1;
}

/**
 * Convertește eticheta multi-class în etichetă binară pentru OvA
 * @return +1 dacă label == target_class, -1 altfel
 */
int to_binary_label(int label, int target_class) {
    return (label == target_class) ? 1 : -1;
}

/**
 * TODO 11: Implementează antrenarea pentru o epocă
 *
 * @return Numărul de erori în această epocă
 */
int perceptron_train_epoch(Perceptron *p, const IrisDataset *train) {
    /* YOUR CODE HERE */
    
    int errors = 0;
    
    for (size_t i = 0; i < train->count; i++) {
        int y_true = to_binary_label(train->samples[i].label, p->target_class);
        int y_pred = perceptron_predict(p, train->samples[i].features);
        
        if (y_pred != y_true) {
            errors++;
            
            /* Update weights */
            for (int j = 0; j < N_FEATURES; j++) {
                p->weights[j] += p->learning_rate * y_true * train->samples[i].features[j];
            }
            p->bias += p->learning_rate * y_true;
        }
    }
    
    return errors;
}

/**
 * TODO 12: Implementează antrenarea completă
 *
 * @return Numărul de epoci până la convergență (sau max_epochs)
 */
int perceptron_fit(Perceptron *p, const IrisDataset *train, int max_epochs, bool verbose) {
    /* YOUR CODE HERE */
    
    for (int epoch = 0; epoch < max_epochs; epoch++) {
        int errors = perceptron_train_epoch(p, train);
        
        if (verbose && (epoch % 10 == 0 || errors == 0)) {
            printf("    Epoca %3d: %d erori\n", epoch, errors);
        }
        
        if (errors == 0) {
            return epoch + 1;
        }
    }
    
    return max_epochs;
}

/* =============================================================================
 * FUNCȚII ONE-VS-ALL
 * =============================================================================
 */

/**
 * TODO 13: Implementează predicția multi-class folosind OvA
 *
 * Alege clasa cu scorul raw maxim
 */
int ova_predict(Perceptron *perceptrons, const double *features) {
    /* YOUR CODE HERE */
    
    int best_class = 0;
    double best_score = perceptron_raw_score(&perceptrons[0], features);
    
    for (int c = 1; c < N_CLASSES; c++) {
        double score = perceptron_raw_score(&perceptrons[c], features);
        if (score > best_score) {
            best_score = score;
            best_class = c;
        }
    }
    
    return best_class;
}

/**
 * TODO 14: Implementează calculul accuracy-ului
 */
double calculate_accuracy(Perceptron *perceptrons, const IrisDataset *test) {
    /* YOUR CODE HERE */
    
    int correct = 0;
    
    for (size_t i = 0; i < test->count; i++) {
        int predicted = ova_predict(perceptrons, test->samples[i].features);
        if (predicted == test->samples[i].label) {
            correct++;
        }
    }
    
    return (double)correct / test->count;
}

/**
 * TODO 15: Implementează calculul matricei de confuzie
 */
void calculate_confusion_matrix(Perceptron *perceptrons, const IrisDataset *test,
                                 ConfusionMatrix *cm) {
    /* YOUR CODE HERE */
    
    /* Inițializează cu 0 */
    for (int i = 0; i < N_CLASSES; i++) {
        for (int j = 0; j < N_CLASSES; j++) {
            cm->matrix[i][j] = 0;
        }
    }
    
    /* Completează matricea */
    for (size_t i = 0; i < test->count; i++) {
        int actual = test->samples[i].label;
        int predicted = ova_predict(perceptrons, test->samples[i].features);
        cm->matrix[actual][predicted]++;
    }
}

/**
 * Afișează matricea de confuzie
 */
void print_confusion_matrix(const ConfusionMatrix *cm) {
    printf("\n┌─────────────────────────────────────────────────────────────┐\n");
    printf("│                    MATRICE DE CONFUZIE                      │\n");
    printf("└─────────────────────────────────────────────────────────────┘\n\n");
    
    printf("                 ");
    for (int j = 0; j < N_CLASSES; j++) {
        printf("Pred %-10s ", CLASS_NAMES[j]);
    }
    printf("\n");
    
    printf("           ┌");
    for (int j = 0; j < N_CLASSES; j++) {
        printf("──────────────");
    }
    printf("┐\n");
    
    for (int i = 0; i < N_CLASSES; i++) {
        printf("Actual %-4s│", CLASS_NAMES[i]);
        for (int j = 0; j < N_CLASSES; j++) {
            printf("     %3d      ", cm->matrix[i][j]);
        }
        printf("│\n");
    }
    
    printf("           └");
    for (int j = 0; j < N_CLASSES; j++) {
        printf("──────────────");
    }
    printf("┘\n");
    
    /* Calculează metrici per clasă */
    printf("\nMetrici per clasă:\n");
    for (int c = 0; c < N_CLASSES; c++) {
        int tp = cm->matrix[c][c];
        int fn = 0, fp = 0;
        
        for (int j = 0; j < N_CLASSES; j++) {
            if (j != c) fn += cm->matrix[c][j];  /* False negatives */
        }
        for (int i = 0; i < N_CLASSES; i++) {
            if (i != c) fp += cm->matrix[i][c];  /* False positives */
        }
        
        double precision = (tp + fp > 0) ? (double)tp / (tp + fp) : 0.0;
        double recall = (tp + fn > 0) ? (double)tp / (tp + fn) : 0.0;
        double f1 = (precision + recall > 0) ? 
                    2 * precision * recall / (precision + recall) : 0.0;
        
        printf("  %s: Precision=%.2f, Recall=%.2f, F1=%.2f\n",
               CLASS_NAMES[c], precision, recall, f1);
    }
}

/* =============================================================================
 * MAIN
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    const char *filename = "data/iris.csv";
    
    if (argc > 1) {
        filename = argv[1];
    }
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║     EXERCIȚIUL 2: Multi-class Classification cu Perceptron                    ║\n");
    printf("║     Săptămâna 18 - Machine Learning în C                                      ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    /* Setează seed pentru reproducibilitate */
    srand(42);
    
    /* Încarcă datele */
    printf("Încărcare date din: %s\n", filename);
    IrisDataset *dataset = load_iris_data(filename);
    
    if (!dataset) {
        /* Generează date de test dacă fișierul nu există */
        printf("Fișierul nu există. Generare date de test (Iris simplificat)...\n\n");
        
        dataset = (IrisDataset*)malloc(sizeof(IrisDataset));
        dataset->samples = (IrisSample*)malloc(150 * sizeof(IrisSample));
        dataset->count = 150;
        
        /* Generează date similare cu Iris */
        for (int i = 0; i < 50; i++) {
            /* Setosa: sepal scurt/lat, petal mic */
            dataset->samples[i].features[0] = 4.5 + (rand() % 20) * 0.1;
            dataset->samples[i].features[1] = 3.0 + (rand() % 10) * 0.1;
            dataset->samples[i].features[2] = 1.0 + (rand() % 10) * 0.1;
            dataset->samples[i].features[3] = 0.1 + (rand() % 5) * 0.1;
            dataset->samples[i].label = 0;
        }
        for (int i = 50; i < 100; i++) {
            /* Versicolor: mediu */
            dataset->samples[i].features[0] = 5.5 + (rand() % 20) * 0.1;
            dataset->samples[i].features[1] = 2.5 + (rand() % 10) * 0.1;
            dataset->samples[i].features[2] = 3.5 + (rand() % 15) * 0.1;
            dataset->samples[i].features[3] = 1.0 + (rand() % 8) * 0.1;
            dataset->samples[i].label = 1;
        }
        for (int i = 100; i < 150; i++) {
            /* Virginica: mare */
            dataset->samples[i].features[0] = 6.0 + (rand() % 25) * 0.1;
            dataset->samples[i].features[1] = 2.8 + (rand() % 12) * 0.1;
            dataset->samples[i].features[2] = 5.0 + (rand() % 20) * 0.1;
            dataset->samples[i].features[3] = 1.5 + (rand() % 12) * 0.1;
            dataset->samples[i].label = 2;
        }
    }
    
    printf("Samples încărcate: %zu\n", dataset->count);
    
    /* Afișează distribuția claselor */
    int class_counts[N_CLASSES] = {0, 0, 0};
    for (size_t i = 0; i < dataset->count; i++) {
        class_counts[dataset->samples[i].label]++;
    }
    printf("Distribuție clase: ");
    for (int c = 0; c < N_CLASSES; c++) {
        printf("%s=%d ", CLASS_NAMES[c], class_counts[c]);
    }
    printf("\n\n");
    
    /* Normalizează features */
    printf("Normalizare features (Min-Max)...\n");
    normalize_features(dataset);
    
    /* Shuffle */
    printf("Amestecarea datelor...\n");
    shuffle_dataset(dataset);
    
    /* Train/Test split */
    IrisDataset train, test;
    train_test_split(dataset, 0.8, &train, &test);
    printf("Train/Test split: %zu/%zu\n\n", train.count, test.count);
    
    /* Antrenare perceptroni One-vs-All */
    printf("┌─────────────────────────────────────────────────────────────┐\n");
    printf("│            ANTRENARE PERCEPTRON ONE-VS-ALL                  │\n");
    printf("└─────────────────────────────────────────────────────────────┘\n\n");
    
    Perceptron perceptrons[N_CLASSES];
    
    for (int c = 0; c < N_CLASSES; c++) {
        printf("Perceptron %d (%s vs rest):\n", c, CLASS_NAMES[c]);
        perceptron_init(&perceptrons[c], c, 0.1);
        int epochs = perceptron_fit(&perceptrons[c], &train, 100, false);
        
        if (epochs < 100) {
            printf("  → Converge în %d epoci ✓\n\n", epochs);
        } else {
            printf("  → Nu converge în 100 epoci (date neliniar separabile)\n\n");
        }
    }
    
    /* Evaluare */
    printf("┌─────────────────────────────────────────────────────────────┐\n");
    printf("│                        EVALUARE                             │\n");
    printf("└─────────────────────────────────────────────────────────────┘\n\n");
    
    double accuracy = calculate_accuracy(perceptrons, &test);
    int correct = (int)(accuracy * test.count + 0.5);
    printf("Accuracy pe test: %.2f%% (%d/%zu)\n", accuracy * 100, correct, test.count);
    
    /* Matrice de confuzie */
    ConfusionMatrix cm;
    calculate_confusion_matrix(perceptrons, &test, &cm);
    print_confusion_matrix(&cm);
    
    /* Exemple de predicții */
    printf("\nExemple de predicții:\n");
    for (int i = 0; i < 5 && i < (int)test.count; i++) {
        int predicted = ova_predict(perceptrons, test.samples[i].features);
        printf("  Sample %d: Actual=%s, Predicted=%s %s\n",
               i + 1,
               CLASS_NAMES[test.samples[i].label],
               CLASS_NAMES[predicted],
               (predicted == test.samples[i].label) ? "✓" : "✗");
    }
    
    /* Curățare */
    free(train.samples);
    free(test.samples);
    free_iris_dataset(dataset);
    
    printf("\n✓ Exercițiul 2 completat!\n\n");
    
    return 0;
}
