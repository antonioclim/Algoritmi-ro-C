# 🚀 Teme Extended Săptămâna 18: Provocări Avansate ML

## Informații Generale

| Proprietate | Valoare |
|------------|---------|
| **Tip** | BONUS (opțional) |
| **Punctaj maxim** | +20 puncte la nota finală |
| **Deadline** | Săptămâna 20 |
| **Dificultate** | ⭐⭐⭐ Avansat |

> **Notă**: Aceste provocări sunt pentru studenții care doresc să aprofundeze 
> conceptele ML. Nu sunt obligatorii pentru promovare.

---

## 🏆 Provocare 1: Logistic Regression (4 puncte)

### Descriere

Implementați **Regresia Logistică** pentru clasificare binară, folosind
funcția sigmoid și binary cross-entropy loss.

### Fundamente Teoretice

**Funcția Sigmoid**:
```
σ(z) = 1 / (1 + e^(-z))

Proprietăți:
- σ(0) = 0.5
- σ(z) → 1 când z → +∞
- σ(z) → 0 când z → -∞
- σ'(z) = σ(z) · (1 - σ(z))
```

**Modelul**:
```
P(y=1|x) = σ(w·x + b)
```

**Binary Cross-Entropy Loss**:
```
J(w) = -(1/n) · Σ [y_i·log(ŷ_i) + (1-y_i)·log(1-ŷ_i)]
```

**Gradient**:
```
∂J/∂w = (1/n) · Σ (ŷ_i - y_i) · x_i
∂J/∂b = (1/n) · Σ (ŷ_i - y_i)
```

### Cerințe

```c
typedef struct {
    double *weights;
    double bias;
    int n_features;
    double learning_rate;
    int max_iters;
} LogisticRegression;

// Funcții de implementat
double sigmoid(double z);
LogisticRegression *logistic_create(int n_features, double lr, int max_iters);
void logistic_free(LogisticRegression *model);
double logistic_bce_loss(LogisticRegression *model, double **X, int *y, int n);
int logistic_fit(LogisticRegression *model, double **X, int *y, int n);
double logistic_predict_proba(LogisticRegression *model, double *x);
int logistic_predict(LogisticRegression *model, double *x);
```

### Test

Clasificare Iris: Setosa vs non-Setosa (folosind doar petal features).

---

## 🏆 Provocare 2: Mini-Batch Gradient Descent (4 puncte)

### Descriere

Implementați **Mini-Batch Gradient Descent** pentru Linear Regression,
cu suport pentru batch_size configurabil.

### Context

Există trei variante de Gradient Descent:

| Tip | Batch Size | Avantaje | Dezavantaje |
|-----|------------|----------|-------------|
| Batch GD | n (toate) | Convergență stabilă | Lent pe date mari |
| Stochastic GD | 1 | Rapid, escape local minima | Zgomot, instabil |
| Mini-Batch GD | 16-256 | Balanț viteză/stabilitate | Hyperparametru extra |

### Cerințe

```c
typedef struct {
    double *weights;
    double bias;
    int n_features;
    double learning_rate;
    int max_epochs;
    int batch_size;
    int shuffle;  // 1 = shuffle data fiecare epoch
} MiniBatchGD;

// Funcții de implementat
MiniBatchGD *mbgd_create(int n_features, double lr, int epochs, int batch_size);
void mbgd_free(MiniBatchGD *model);

// Shuffle indices (Fisher-Yates)
void shuffle_indices(int *indices, int n);

// Un pas de gradient pe un mini-batch
void mbgd_update_batch(MiniBatchGD *model, double **X, double *y, 
                       int *indices, int start, int end);

// Antrenare completă
int mbgd_fit(MiniBatchGD *model, double **X, double *y, int n);

// Predicții
void mbgd_predict(MiniBatchGD *model, double **X, int n, double *y_pred);
```

### Test

Comparați convergența pentru batch_size = 1, 8, 32, n pe housing data.
Generați grafic ASCII cu loss vs epoch pentru fiecare variantă.

---

## 🏆 Provocare 3: PCA (Principal Component Analysis) (4 puncte)

### Descriere

Implementați **PCA** pentru reducerea dimensionalității folosind
Power Iteration pentru calculul vectorilor proprii.

### Fundamente

**Pașii PCA**:
1. Centrare date: X_centered = X - mean(X)
2. Calcul matrice de covarianță: C = (1/n) · X^T · X
3. Calcul vectori proprii (eigenvectors) ai lui C
4. Proiecție pe primii k vectori proprii

**Power Iteration** (pentru dominant eigenvector):
```
v₀ = random vector
repeat:
    v_{k+1} = A · v_k
    v_{k+1} = v_{k+1} / ||v_{k+1}||
until convergence
λ = v^T · A · v  (eigenvalue)
```

### Cerințe

```c
typedef struct {
    int n_components;     // Număr de componente de păstrat
    double **components;  // Vectori proprii (k x n_features)
    double *eigenvalues;  // Valori proprii
    double *mean;         // Media pe fiecare coloană
    double explained_var; // Varianța explicată (%)
    int n_features;
} PCA;

// Funcții de implementat
PCA *pca_create(int n_components);
void pca_free(PCA *pca);

// Centrare date
void pca_center(double **X, int n, int d, double *mean);

// Matrice covarianță
void pca_covariance(double **X_centered, int n, int d, double **cov);

// Power iteration pentru un eigenvector
void power_iteration(double **A, int n, double *eigenvector, 
                     double *eigenvalue, int max_iters);

// Fit: calculează componentele principale
void pca_fit(PCA *pca, double **X, int n, int d);

// Transform: proiectează date pe componentele principale
void pca_transform(PCA *pca, double **X, int n, double **X_reduced);

// Inverse transform (reconstrucție)
void pca_inverse_transform(PCA *pca, double **X_reduced, int n, double **X_reconstructed);
```

### Test

Reduceți Iris de la 4D la 2D și vizualizați (ASCII scatter plot).
Calculați reconstruction error pentru k=1, 2, 3, 4.

---

## 🏆 Provocare 4: Decision Stump (4 puncte)

### Descriere

Implementați **Decision Stump** - un arbore de decizie cu o singură
ramificare. Este building block-ul pentru AdaBoost.

### Concept

Un Decision Stump găsește:
- Cea mai bună caracteristică pe care să se separe
- Cel mai bun threshold pentru separare
- Optimizează un criteriu (Gini impurity sau Information Gain)

```
         [feature_j ≤ threshold?]
              /           \
           YES             NO
            |               |
       [clasă_stânga]  [clasă_dreapta]
```

### Gini Impurity

```
Gini(S) = 1 - Σ p_i²

Unde p_i = proporția clasei i în setul S

Gini split = (n_left/n) · Gini(S_left) + (n_right/n) · Gini(S_right)
```

### Cerințe

```c
typedef struct {
    int feature_index;     // Pe ce caracteristică se împarte
    double threshold;      // Pragul de împărțire
    int class_left;        // Clasa pentru <= threshold
    int class_right;       // Clasa pentru > threshold
    double gini;           // Gini impurity al split-ului
} DecisionStump;

// Funcții de implementat
DecisionStump *stump_create();
void stump_free(DecisionStump *stump);

// Calcul Gini impurity
double gini_impurity(int *labels, int n, int n_classes);

// Evaluează un posibil split
double evaluate_split(double **X, int *y, int n, int n_classes,
                      int feature_idx, double threshold);

// Găsește cel mai bun split
void stump_fit(DecisionStump *stump, double **X, int *y, 
               int n, int n_features, int n_classes);

// Predicție
int stump_predict_one(DecisionStump *stump, double *x);
void stump_predict(DecisionStump *stump, double **X, int n, int *y_pred);

// Print stump
void stump_print(DecisionStump *stump);
```

### Test

Antrenați pe Iris (toate cele 3 clase) și afișați regula găsită.
Comparați cu K-NN și Perceptron pe aceleași date.

---

## 🏆 Provocare 5: Naive Bayes (4 puncte)

### Descriere

Implementați **Gaussian Naive Bayes** pentru clasificare, bazat pe
teorema lui Bayes cu asumpția de independență.

### Fundamente

**Teorema lui Bayes**:
```
P(c|x) = P(x|c) · P(c) / P(x)
```

**Naive Bayes asumpție**: caracteristicile sunt independente condiționat
```
P(x|c) = Π P(x_i|c)
```

**Gaussian Naive Bayes**: fiecare P(x_i|c) este o distribuție normală
```
P(x_i|c) = (1/√(2π·σ²_ic)) · exp(-(x_i - μ_ic)² / (2·σ²_ic))
```

### Cerințe

```c
typedef struct {
    int n_classes;
    int n_features;
    double *class_priors;    // P(c) pentru fiecare clasă
    double **means;          // μ_ic: mean[class][feature]
    double **variances;      // σ²_ic: variance[class][feature]
    int *class_counts;       // Număr exemple per clasă
} GaussianNB;

// Funcții de implementat
GaussianNB *gnb_create(int n_classes, int n_features);
void gnb_free(GaussianNB *model);

// Calcul probabilitate Gaussiană
double gaussian_pdf(double x, double mean, double variance);

// Antrenare: calculează P(c), μ_ic, σ²_ic
void gnb_fit(GaussianNB *model, double **X, int *y, int n);

// Log-likelihood pentru stabilitate numerică
double gnb_log_likelihood(GaussianNB *model, double *x, int class_idx);

// Predicție
int gnb_predict_one(GaussianNB *model, double *x);
void gnb_predict(GaussianNB *model, double **X, int n, int *y_pred);

// Probabilități per clasă
void gnb_predict_proba(GaussianNB *model, double *x, double *proba);
```

### Test

Clasificare Iris cu toate 3 clase. Afișați:
- P(c) pentru fiecare clasă
- μ și σ pentru fiecare (clasă, caracteristică)
- Matrice de confuzie finală

---

## 📊 Sistem de Punctare Bonus

| Provocare | Puncte | Total cumulat |
|-----------|--------|---------------|
| 1. Logistic Regression | +4 | +4 |
| 2. Mini-Batch GD | +4 | +8 |
| 3. PCA | +4 | +12 |
| 4. Decision Stump | +4 | +16 |
| 5. Naive Bayes | +4 | +20 |

**Maximum +20 puncte bonus** care se adaugă la nota de laborator.

---

## 📤 Format Predare Extended

```
homework_extended_week18_NUME_PRENUME.zip
├── challenge1_logistic.c
├── challenge2_minibatch.c
├── challenge3_pca.c
├── challenge4_stump.c
├── challenge5_naivebayes.c
├── Makefile
└── results/
    └── [output files pentru fiecare challenge]
```

---

## 💡 Sfaturi

1. **Începeți cu ce vă pare mai ușor** - Naive Bayes și Decision Stump sunt
   relativ directe de implementat.

2. **Folosiți log-probabilități** pentru stabilitate numerică în Naive Bayes
   și Logistic Regression.

3. **Testați pe date mici** înainte de datasetul complet.

4. **Comparați cu sklearn** pentru validare (vezi `python_comparison/`).

5. **Nu uitați de edge cases**: division by zero, variance zero, etc.

---

*Aceste provocări vă vor pregăti pentru cursuri avansate de ML și 
interviuri tehnice. Succes!*
