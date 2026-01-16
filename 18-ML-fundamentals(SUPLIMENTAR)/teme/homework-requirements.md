# 📝 Teme Săptămâna 18: Fundamente Machine Learning în C

## Informații Generale

| Proprietate | Valoare |
|------------|---------|
| **Deadline** | Săptămâna 19, ora laboratorului |
| **Punctaj maxim** | 100 puncte |
| **Bonus posibil** | +20 puncte (pentru Extended) |
| **Limbaj** | C11 standard |
| **Restricții** | Fără biblioteci externe ML |

---

## 📋 Tema 1: Ridge Regression (Regularizare L2) - 50 puncte

### Context Teoretic

**Regresia Ridge** (Tikhonov regularization) adaugă un termen de penalizare
la funcția de cost pentru a preveni overfitting-ul:

```
J(w) = MSE(w) + λ·||w||²
     = (1/n)·Σ(y_i - ŷ_i)² + λ·Σw_j²
```

Unde:
- `MSE(w)` = eroarea medie pătratică (ca la regresie obișnuită)
- `λ` = parametrul de regularizare (hyperparametru)
- `||w||²` = suma pătratelor weight-urilor

**Intuiție**: Penalizarea forțează weight-urile să rămână mici, prevenind
modelul să se bazeze prea mult pe o singură caracteristică.

### Gradientul cu Regularizare

Gradientul devine:

```
∂J/∂w_j = (2/n)·Σ(ŷ_i - y_i)·x_ij + 2·λ·w_j
```

**Update rule** (Gradient Descent):
```
w_j = w_j - α·[(2/n)·Σ(error·x_ij) + 2·λ·w_j]
    = w_j·(1 - 2·α·λ) - α·(2/n)·Σ(error·x_ij)
```

### Cerințe de Implementare

Creați fișierul `homework1.c` cu următoarele:

```c
// ============================================================
// STRUCTURA RIDGE REGRESSION
// ============================================================
typedef struct {
    double *weights;      // Vector de weight-uri
    double bias;          // Bias term
    int n_features;       // Număr de caracteristici
    double lambda;        // Parametru de regularizare
    double learning_rate; // Rata de învățare
    int max_iters;        // Număr maxim de iterații
    double tolerance;     // Toleranță pentru convergență
} RidgeRegression;

// ============================================================
// FUNCȚII DE IMPLEMENTAT
// ============================================================

/**
 * Creează și inițializează un model Ridge Regression.
 * 
 * @param n_features  Numărul de caracteristici din date
 * @param lambda      Parametrul de regularizare (0.01 - 10.0 tipic)
 * @param lr          Learning rate (0.001 - 0.1 tipic)
 * @param max_iters   Număr maxim de iterații (1000 - 10000)
 * @param tol         Toleranță pentru early stopping (1e-6)
 * @return            Pointer la structură alocată
 */
RidgeRegression *ridge_create(int n_features, double lambda, 
                              double lr, int max_iters, double tol);

/**
 * Eliberează memoria pentru model.
 */
void ridge_free(RidgeRegression *model);

/**
 * Calculează funcția de cost cu regularizare.
 * 
 * J(w) = (1/n)·Σ(y - ŷ)² + λ·Σw²
 * 
 * @return Valoarea funcției de cost
 */
double ridge_cost(RidgeRegression *model, double **X, double *y, int n);

/**
 * Antrenează modelul folosind Gradient Descent cu regularizare L2.
 * 
 * @param X      Matricea de caracteristici [n_samples][n_features]
 * @param y      Vectorul de target-uri
 * @param n      Numărul de exemple
 * @return       Numărul de iterații până la convergență
 */
int ridge_fit(RidgeRegression *model, double **X, double *y, int n);

/**
 * Prezice valorile pentru date noi.
 * 
 * @param X      Matricea de caracteristici
 * @param n      Numărul de exemple
 * @param y_pred Buffer pentru predicții (pre-alocat)
 */
void ridge_predict(RidgeRegression *model, double **X, int n, double *y_pred);

/**
 * Calculează R² score.
 */
double ridge_r2_score(double *y_true, double *y_pred, int n);
```

### Testare

Folosiți `data/housing_simple.csv` și comparați:
1. Ridge cu λ=0 (echivalent cu Linear Regression obișnuită)
2. Ridge cu λ=0.1, λ=1.0, λ=10.0
3. Observați cum weight-urile devin mai mici cu λ mai mare

### Criterii de Evaluare (50p)

| Criteriu | Puncte |
|----------|--------|
| Implementare corectă `ridge_cost` | 10p |
| Implementare corectă `ridge_fit` cu gradient | 20p |
| Convergență și early stopping | 5p |
| Predicții corecte | 10p |
| Comparație diferite λ în main() | 5p |

---

## 📋 Tema 2: Weighted K-NN - 50 puncte

### Context Teoretic

**Weighted K-NN** îmbunătățește K-NN clasic prin acordarea de ponderi
vecinilor în funcție de distanța lor:

```
Vot ponderat: P(clasă c) = Σ w_i · I(y_i = c) / Σ w_i
```

Unde `w_i` este ponderea vecinului `i`, calculată ca:

1. **Inversul distanței**: `w_i = 1 / d_i`
2. **Gaussian kernel**: `w_i = exp(-d_i² / (2σ²))`
3. **Linear decay**: `w_i = (k - rank_i + 1) / k`

**Avantaj**: Vecinii mai apropiați au influență mai mare în decizie.

### Cerințe de Implementare

Creați fișierul `homework2.c` cu următoarele:

```c
// ============================================================
// ENUMĂRI ȘI STRUCTURI
// ============================================================
typedef enum {
    WEIGHT_UNIFORM,        // KNN clasic (toate ponderile = 1)
    WEIGHT_INVERSE_DIST,   // w = 1/d
    WEIGHT_GAUSSIAN,       // w = exp(-d²/2σ²)
    WEIGHT_LINEAR_DECAY    // w = (k - rank + 1) / k
} WeightScheme;

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

// ============================================================
// FUNCȚII DE IMPLEMENTAT
// ============================================================

/**
 * Creează clasificator Weighted K-NN.
 */
WeightedKNN *wknn_create(int k, WeightScheme scheme, double sigma);

/**
 * Eliberează memoria.
 */
void wknn_free(WeightedKNN *model);

/**
 * "Antrenează" modelul (stocare date pentru lazy learning).
 * 
 * @param X        Matricea de caracteristici
 * @param y        Etichetele (0, 1, 2, ...)
 * @param n        Numărul de exemple
 * @param n_feat   Numărul de caracteristici
 * @param n_class  Numărul de clase
 */
void wknn_fit(WeightedKNN *model, double **X, int *y, 
              int n, int n_feat, int n_class);

/**
 * Calculează ponderea pentru un vecin.
 * 
 * @param distance  Distanța până la vecin
 * @param rank      Poziția în ordinea distanțelor (1 = cel mai apropiat)
 * @return          Ponderea calculată
 */
double wknn_compute_weight(WeightedKNN *model, double distance, int rank);

/**
 * Prezice clasa pentru un singur exemplu.
 * 
 * @param x        Vectorul de caracteristici
 * @return         Clasa prezisă
 */
int wknn_predict_one(WeightedKNN *model, double *x);

/**
 * Prezice clasele pentru mai multe exemple.
 */
void wknn_predict(WeightedKNN *model, double **X, int n, int *y_pred);

/**
 * Calculează acuratețea.
 */
double wknn_accuracy(int *y_true, int *y_pred, int n);

/**
 * Compară diferite scheme de ponderare.
 */
void wknn_compare_schemes(double **X_train, int *y_train, int n_train,
                          double **X_test, int *y_test, int n_test,
                          int n_features, int n_classes);
```

### Exemplu de Vot Ponderat

```
Exemplu cu k=5, clasă = {0, 1}:

Vecin 1: clasă=1, distanță=0.5 → w=1/0.5=2.0
Vecin 2: clasă=0, distanță=1.0 → w=1/1.0=1.0
Vecin 3: clasă=1, distanță=1.2 → w=1/1.2=0.83
Vecin 4: clasă=1, distanță=2.0 → w=1/2.0=0.5
Vecin 5: clasă=0, distanță=3.0 → w=1/3.0=0.33

Scor clasă 0: 1.0 + 0.33 = 1.33
Scor clasă 1: 2.0 + 0.83 + 0.5 = 3.33

→ Predicție: clasă 1 (scor mai mare)

Cu KNN clasic (uniform): 3 voturi pentru 1, 2 pentru 0 → tot clasă 1
Dar ponderile dau mai multă încredere!
```

### Testare

Folosiți `data/iris.csv` și comparați:
1. KNN uniform (k=5)
2. Weighted KNN inverse distance (k=5)
3. Weighted KNN Gaussian (k=5, σ=0.5)
4. Testați cu k=3, k=7, k=11

### Criterii de Evaluare (50p)

| Criteriu | Puncte |
|----------|--------|
| Implementare `wknn_compute_weight` pentru toate schemele | 15p |
| Implementare corectă `wknn_predict_one` | 15p |
| Testare pe Iris dataset | 10p |
| Comparație scheme + analiză rezultate | 10p |

---

## 📤 Instrucțiuni de Predare

### Format Arhivă

```
homework_week18_NUME_PRENUME.zip
├── homework1.c          # Ridge Regression
├── homework2.c          # Weighted K-NN
├── Makefile             # Pentru compilare
├── README.md            # Documentație (opțional dar recomandat)
└── results/
    ├── ridge_comparison.txt    # Output comparație λ
    └── wknn_comparison.txt     # Output comparație scheme
```

### Makefile Minimal

```makefile
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2
LDFLAGS = -lm

all: homework1 homework2

homework1: homework1.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

homework2: homework2.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f homework1 homework2

.PHONY: all clean
```

### Criterii de Admitere

- [ ] Codul compilează fără erori cu `gcc -Wall -Wextra`
- [ ] Nu există memory leaks (verificat cu Valgrind)
- [ ] Funcționează pe datele furnizate
- [ ] Rezultatele sunt rezonabile (R² > 0.8 pentru Ridge, Acuratețe > 90% pentru KNN)

---

## 🔗 Resurse Utile

1. **Ridge Regression**: [Elements of Statistical Learning, Cap. 3.4](https://hastie.su.domains/ElemStatLearn/)
2. **Weighted K-NN**: [Dudani (1976) - The Distance-Weighted k-Nearest-Neighbor Rule](https://ieeexplore.ieee.org/document/4309452)
3. **Gradient Descent**: [Stanford CS229 Notes](https://cs229.stanford.edu/notes2022fall/main_notes.pdf)

---

*Succes la implementare! Întrebările se adresează în cadrul laboratorului sau pe forum.*
