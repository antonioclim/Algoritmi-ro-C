# Săptămâna 18: Fundamente Machine Learning în C

## 🎯 Obiective de Învățare

La finalul acestei săptămâni, veți fi capabili să:

1. **Rețineți** (Remember): Formulele fundamentale pentru MSE și gradient în regresie liniară; regula de actualizare a centroizilor în K-Means; distanța Euclidiană pentru K-NN; regula de actualizare a ponderilor pentru Perceptron
2. **Înțelegeți** (Understand): Mecanismul convergenței în gradient descent și factorii care o influențează; efectul parametrului k asupra calității clustering-ului; limitările perceptronului pentru probleme neliniar separabile (XOR)
3. **Aplicați** (Apply): Implementarea completă în C a: regresiei liniare cu gradient descent, K-Means cu inițializare k-means++, clasificatorului K-NN, și perceptronului cu regulă de învățare
4. **Analizați** (Analyse): Interpretarea curbelor de convergență (loss curve); evaluarea calității clustering-ului prin inerție și silhouette simplificat; analiza matricei de confuzie
5. **Evaluați** (Evaluate): Selectarea hiperparametrilor optimi (learning rate, k, număr de epoci) și evaluarea trade-off-urilor asociate
6. **Creați** (Create): Sistem complet de clasificare multi-clasă pentru setul de date Iris cu train/test split și evaluare riguroasă

---

## 📜 Context Istoric

### Originile Învățării Automate

Învățarea automată (Machine Learning) își are rădăcinile în visul uman de a crea mașini capabile să învețe din experiență. Această disciplină a evoluat la intersecția statisticii, informaticii și neuroștiințelor, transformându-se dintr-o curiozitate academică într-o tehnologie care redefinește societatea modernă.

Conceptul de "mașină care învață" a fost articulat pentru prima dată de **Alan Turing** în articolul său revoluționar din 1950, *"Computing Machinery and Intelligence"*, unde a propus celebrul test care îi poartă numele. Turing a imaginat că, în loc să programăm explicit fiecare comportament, am putea construi sisteme care să învețe din date, similar modului în care copiii învață din experiență.

### Figuri Cheie

```
    ┌─────────────────────────────────────────────────────────────────┐
    │  Frank Rosenblatt (1928-1971)                                   │
    │  Psiholog și informatician american                             │
    │                                                                 │
    │  • Inventatorul Perceptronului (1958) la Cornell                │
    │  • A construit Mark I Perceptron - primul hardware neuronal     │
    │  • Pionierul rețelelor neuronale artificiale                    │
    │  • "Father of Deep Learning" - recunoscut postum                │
    └─────────────────────────────────────────────────────────────────┘
```

> *"The embryo of an electronic computer that [the Navy] expects will be able to walk, talk, see, write, reproduce itself and be conscious of its existence."*
> — The New York Times, despre Perceptron, 1958

```
    ┌─────────────────────────────────────────────────────────────────┐
    │  Stuart Lloyd (1923-2010)                                       │
    │  Matematician la Bell Labs                                      │
    │                                                                 │
    │  • A dezvoltat algoritmul K-Means în 1957                       │
    │  • Publicat oficial abia în 1982 în IEEE                        │
    │  • Algoritmul e cunoscut și ca "Lloyd's algorithm"              │
    │  • Aplicații în compresia datelor și cuantizare vectorială      │
    └─────────────────────────────────────────────────────────────────┘
```

```
    ┌─────────────────────────────────────────────────────────────────┐
    │  Adrien-Marie Legendre (1752-1833) & Carl Friedrich Gauss       │
    │  (1777-1855)                                                    │
    │  Matematicieni francez și german                                │
    │                                                                 │
    │  • Legendre: Prima publicare a metodei celor mai mici pătrate   │
    │    în 1805 ("Nouvelles méthodes pour la détermination des       │
    │    orbites des comètes")                                        │
    │  • Gauss: A revendicat utilizarea metodei încă din 1795         │
    │  • Fundamentul matematic al regresiei liniare moderne           │
    └─────────────────────────────────────────────────────────────────┘
```

### Moment Crucial: Perceptrons (1969)

Cartea *"Perceptrons: An Introduction to Computational Geometry"* de **Marvin Minsky** și **Seymour Papert** a demonstrat matematic că perceptronul simplu nu poate învăța funcția XOR. Această demonstrație a declanșat primul "AI Winter" - o perioadă de reducere drastică a finanțării cercetării în rețele neuronale. Ironic, soluția (rețelele multi-strat cu backpropagation) exista deja în formă incipientă, dar a fost nevoie de două decenii pentru redescoperire.

---

## 📚 Fundamente Teoretice

### 1. Infrastructură Matriceală pentru ML

Algoritmii ML operează fundamental pe vectori și matrice. În C, trebuie să implementăm aceste abstracții de la zero:

```
    ┌─────────────────────────────────────────────────────────────────┐
    │                    REPREZENTAREA DATELOR                        │
    ├─────────────────────────────────────────────────────────────────┤
    │                                                                 │
    │   Dataset Iris (exemplu):                                       │
    │   ┌─────────────────────────────────────────────────────────┐   │
    │   │ Feature 1 │ Feature 2 │ Feature 3 │ Feature 4 │ Label   │   │
    │   │ (sepal L) │ (sepal W) │ (petal L) │ (petal W) │         │   │
    │   ├───────────┼───────────┼───────────┼───────────┼─────────┤   │
    │   │   5.1     │   3.5     │   1.4     │   0.2     │    0    │   │
    │   │   7.0     │   3.2     │   4.7     │   1.4     │    1    │   │
    │   │   6.3     │   3.3     │   6.0     │   2.5     │    2    │   │
    │   └───────────┴───────────┴───────────┴───────────┴─────────┘   │
    │                                                                 │
    │   În C: double **X  (n_samples × n_features)                    │
    │         int *y      (n_samples)                                 │
    │                                                                 │
    └─────────────────────────────────────────────────────────────────┘
```

**Structurile fundamentale în C:**

```c
typedef struct {
    double *data;       /* Date stocate liniar (row-major) */
    size_t rows;        /* Număr de linii */
    size_t cols;        /* Număr de coloane */
} Matrix;

typedef struct {
    double *data;       /* Elementele vectorului */
    size_t size;        /* Dimensiunea vectorului */
} Vector;
```

### 2. Regresie Liniară cu Gradient Descent

Regresia liniară modelează relația liniară dintre features (X) și target (y):

```
    ŷ = w₁x₁ + w₂x₂ + ... + wₙxₙ + b = w·x + b
    
    unde:
    • w = vector de ponderi (weights)
    • b = bias (intercept)
    • ŷ = predicția modelului
```

**Funcția de cost (Mean Squared Error):**

```
                    1   n
    MSE(w, b) = ─────  Σ  (ŷᵢ - yᵢ)²
                  n   i=1
```

**Gradient Descent:**

```
    ┌─────────────────────────────────────────────────────────────────┐
    │                    GRADIENT DESCENT                             │
    ├─────────────────────────────────────────────────────────────────┤
    │                                                                 │
    │   Intuiție: Coborâm pe "suprafața" funcției de cost             │
    │                                                                 │
    │   Loss                                                          │
    │     │     ╭───╮                                                 │
    │     │    ╱     ╲                                                │
    │     │   ╱   ●   ╲       ● = poziția curentă                     │
    │     │  ╱     ↓   ╲      ↓ = direcția gradientului               │
    │     │ ╱       ●   ╲                                             │
    │     │╱         ↓   ╲                                            │
    │     ├───────────●───────────────                                │
    │               (minim)      w                                    │
    │                                                                 │
    │   Regula de actualizare:                                        │
    │   • w = w - α · ∂MSE/∂w                                         │
    │   • b = b - α · ∂MSE/∂b                                         │
    │                                                                 │
    │   unde α = learning rate (rată de învățare)                     │
    │                                                                 │
    └─────────────────────────────────────────────────────────────────┘
```

**Derivatele:**

```
    ∂MSE          2   n
    ───── = ─────  Σ  (ŷᵢ - yᵢ) · xᵢⱼ
    ∂wⱼ       n   i=1

    ∂MSE          2   n
    ───── = ─────  Σ  (ŷᵢ - yᵢ)
    ∂b        n   i=1
```

### 3. K-Means Clustering

K-Means partiționează datele în k clustere minimizând inerția (within-cluster sum of squares):

```
    ┌─────────────────────────────────────────────────────────────────┐
    │                    ALGORITMUL K-MEANS                           │
    ├─────────────────────────────────────────────────────────────────┤
    │                                                                 │
    │   1. Inițializare: Alege k centroizi                            │
    │                                                                 │
    │   2. Repetă până la convergență:                                │
    │      a) Asignare: Fiecare punct → cel mai apropiat centroid     │
    │      b) Actualizare: Centroid = media punctelor asignate        │
    │                                                                 │
    │   Vizualizare (k=3):                                            │
    │                                                                 │
    │       Iterația 1:        Iterația 2:        Convergență:        │
    │       ○ ○                ○ ○                ● ●                 │
    │      ○   ×              ●   ×              ●   ×                │
    │        ○                  ●                  ●                  │
    │                                                                 │
    │      □ □ □              □ □ □              ■ ■ ■                │
    │         ×                  ×                  ×                 │
    │      □ □                ■ ■                ■ ■                  │
    │                                                                 │
    │      △   △              △   △              ▲   ▲                │
    │        × △                × ▲                × ▲                │
    │      △                  ▲                  ▲                    │
    │                                                                 │
    │   × = centroid, forme goale = neasignat, pline = asignat        │
    │                                                                 │
    └─────────────────────────────────────────────────────────────────┘
```

**K-Means++ Initialization:**

Inițializarea aleatoare poate duce la rezultate suboptimale. K-means++ îmbunătățește dramatic convergența:

```
    1. Alege primul centroid uniform aleator din date
    2. Pentru fiecare centroid următor:
       a) Calculează D(x)² = distanța minimă de la x la centroizii existenți
       b) Alege noul centroid cu probabilitate ∝ D(x)²
    3. Repetă până avem k centroizi
```

### 4. K-Nearest Neighbors (K-NN)

K-NN este un algoritm "lazy" - nu construiește un model explicit, ci memorează toate datele de antrenament:

```
    ┌─────────────────────────────────────────────────────────────────┐
    │                    CLASIFICARE K-NN                             │
    ├─────────────────────────────────────────────────────────────────┤
    │                                                                 │
    │   Pentru un punct nou ?:                                        │
    │                                                                 │
    │   k=3:                        k=5:                              │
    │   ○ ○                         ○ ○                               │
    │     ○                           ○    ○                          │
    │       ?  ○                        ?  ○                          │
    │     □                           □                               │
    │   □   □                       □   □                             │
    │                                                                 │
    │   Vecini: ○○○ → Clasa: ○      Vecini: ○○○□□ → Clasa: ○          │
    │   (2○, 1□) → majoritate ○     (3○, 2□) → majoritate ○           │
    │                                                                 │
    │   Distanța Euclidiană:                                          │
    │                   ___________________________                   │
    │   d(a,b) = √ Σᵢ (aᵢ - bᵢ)²                                      │
    │                                                                 │
    └─────────────────────────────────────────────────────────────────┘
```

### 5. Perceptronul

Perceptronul este cel mai simplu model de neuron artificial, capabil să învețe granițe de decizie liniare:

```
    ┌─────────────────────────────────────────────────────────────────┐
    │                    STRUCTURA PERCEPTRONULUI                     │
    ├─────────────────────────────────────────────────────────────────┤
    │                                                                 │
    │                    x₁ ─────┐                                    │
    │                            │w₁                                  │
    │                    x₂ ─────┼──→ Σ ──→ f(·) ──→ ŷ               │
    │                            │w₂       ↑                          │
    │                    x₃ ─────┘         │                          │
    │                        w₃    b ──────┘                          │
    │                                                                 │
    │   Suma ponderată: z = w·x + b = Σᵢ wᵢxᵢ + b                     │
    │   Funcția de activare: f(z) = sign(z) = { +1 dacă z ≥ 0        │
    │                                         { -1 dacă z < 0         │
    │                                                                 │
    │   Regula de învățare (Perceptron Learning Rule):                │
    │   Dacă yᵢ · (w·xᵢ + b) ≤ 0 (clasificare greșită):              │
    │       w = w + α · yᵢ · xᵢ                                       │
    │       b = b + α · yᵢ                                            │
    │                                                                 │
    └─────────────────────────────────────────────────────────────────┘
```

**Limitarea XOR:**

```
    ┌─────────────────────────────────────────────────────────────────┐
    │                    PROBLEMA XOR                                 │
    ├─────────────────────────────────────────────────────────────────┤
    │                                                                 │
    │   AND (separabil liniar):    XOR (NU e separabil liniar):       │
    │                                                                 │
    │   y│                         y│                                 │
    │   1│    ○ (1,1)=1            1│    ● (0,1)=1    ○ (1,1)=0       │
    │    │      ╲                   │                                 │
    │    │       ╲                  │         ???                     │
    │   0│○ ○     ╲                0│    ○ (0,0)=0    ● (1,0)=1       │
    │    └────────────x             └────────────────────x            │
    │      0    1                     0         1                     │
    │                                                                 │
    │   O singură linie poate       NU există o linie care să         │
    │   separa clasa 1 de clasa 0   separe clasele 0 și 1!            │
    │                                                                 │
    │   Soluția: Multi-Layer Perceptron (MLP) cu layer ascuns         │
    │                                                                 │
    └─────────────────────────────────────────────────────────────────┘
```

---

## 🏭 Aplicații Industriale

### 1. Regresie Liniară în Practică

- **Predicția prețurilor imobiliare**: Estimarea valorii proprietăților bazată pe features (suprafață, locație, număr camere)
- **Forecasting vânzări**: Proiecții bazate pe date istorice și factori sezonieri
- **Analiza riscului de credit**: Scoring bazat pe caracteristici financiare

### 2. K-Means în Industrie

- **Segmentarea clienților**: Gruparea bazată pe comportament de cumpărare (RFM: Recency, Frequency, Monetary)
- **Compresia imaginilor**: Reducerea numărului de culori prin cuantizare
- **Detectarea anomaliilor**: Identificarea outlier-ilor ca puncte departe de orice centroid

### 3. K-NN în Aplicații Reale

- **Sisteme de recomandare**: "Users like you also purchased..."
- **Recunoașterea scrisului de mână**: MNIST digit classification
- **Diagnosticare medicală**: Clasificare bazată pe cazuri similare anterioare

### 4. Perceptron și Descendenții Săi

- **Spam filtering**: Clasificarea email-urilor (origini istorice)
- **Clasificare de sentiment**: Pozitiv/Negativ în recenzii
- **Deep Learning**: Rețelele neuronale moderne sunt extensii ale perceptronului

---

## 💻 Exerciții de Laborator

### Exercițiul 1: Customer Segmentation cu K-Means

**Obiectiv**: Implementați segmentarea clienților folosind K-Means cu k-means++ initialization.

**Cerințe**:
1. Citiți datele clienților din `data/customers.csv` (venit anual, frecvență achiziții, valoare totală)
2. Normalizați datele folosind Min-Max scaling
3. Implementați K-Means cu inițializare k-means++
4. Aplicați metoda Elbow pentru a determina k optim
5. Afișați profilul fiecărui cluster identificat

**Fișiere**: `src/exercise1.c`, `data/customers.csv`

### Exercițiul 2: Multi-class Classification cu Perceptron

**Obiectiv**: Implementați clasificarea multi-clasă pentru setul Iris folosind strategia One-vs-All.

**Cerințe**:
1. Citiți datele din `data/iris.csv`
2. Implementați train/test split (80/20)
3. Antrenați 3 perceptroni (unul pentru fiecare clasă)
4. Implementați predicția combinată (max score)
5. Calculați și afișați accuracy și matricea de confuzie

**Fișiere**: `src/exercise2.c`, `data/iris.csv`

---

## 🔧 Compilare și Execuție

```bash
# Construiește toate targeturile
make all

# Rulează exemplul demonstrativ
make run

# Compilează și rulează exercițiul 1
make exercise1
./exercise1

# Compilează și rulează exercițiul 2
make exercise2
./exercise2

# Rulează testele automate
make test

# Verificare memory leaks
make valgrind

# Curățare
make clean

# Ajutor
make help
```

---

## 🐳 Rulare în Docker

```bash
# Construiește imaginea
docker build -t atp-week18 .

# Rulează containerul
docker run -it --rm -v $(pwd):/workspace atp-week18

# În container:
make all && make run
```

---

## 📁 Structura Directorului

```
18-ml-fundamentals-c/
├── README.md                           # Documentația completă (acest fișier)
├── Makefile                            # Build system cu output colorat
├── Dockerfile                          # Containerizare pentru portabilitate
├── docker-compose.yml                  # Setup complet cu toate dependențele
│
├── slides/
│   ├── presentation-week18.html        # Prezentare interactivă principală
│   └── presentation-comparativ.html    # Comparație Pseudocod/C/Python
│
├── src/
│   ├── example1.c                      # Demo complet: LR, K-Means, K-NN, Perceptron
│   ├── exercise1.c                     # Customer Segmentation (cu TODO-uri)
│   └── exercise2.c                     # Multi-class Classification (cu TODO-uri)
│
├── data/
│   ├── iris.csv                        # Dataset Iris clasic (150 samples)
│   ├── housing_simple.csv              # Date pentru regresie
│   ├── customers.csv                   # Date pentru clustering
│   └── xor_data.csv                    # Date XOR pentru demo limitări
│
├── tests/
│   ├── test1_input.txt                 # Input pentru test clustering
│   ├── test1_expected.txt              # Output așteptat clustering
│   ├── test2_input.txt                 # Input pentru test clasificare
│   └── test2_expected.txt              # Output așteptat clasificare
│
├── python_comparison/
│   ├── sklearn_comparison.py           # Echivalent scikit-learn
│   └── numpy_operations.py             # Operații matriceale în NumPy
│
├── teme/
│   ├── homework-requirements.md        # Cerințe temă (Ridge Regression, Weighted K-NN)
│   └── homework-extended.md            # Provocări avansate (5)
│
└── solution/
    ├── exercise1_sol.c                 # Soluție completă Ex. 1
    ├── exercise2_sol.c                 # Soluție completă Ex. 2
    ├── homework1_sol.c                 # Soluție Tema 1
    └── homework2_sol.c                 # Soluție Tema 2
```

---

## 📖 Bibliografie Recomandată

### Esențiale

- **Tom M. Mitchell** - *Machine Learning* (1997), Capitolele 1-4
- **Christopher M. Bishop** - *Pattern Recognition and Machine Learning* (2006), Capitolul 1
- **Gareth James et al.** - *An Introduction to Statistical Learning* (2013), Capitolele 2-3

### Avansate

- **Marvin Minsky & Seymour Papert** - *Perceptrons: An Introduction to Computational Geometry* (1969) - Document istoric fundamental
- **Stuart Lloyd** - *Least Squares Quantization in PCM* (1982), IEEE Transactions on Information Theory

### Resurse Online

- [Scikit-learn Documentation](https://scikit-learn.org/stable/documentation.html) - Referință pentru implementări profesionale
- [Stanford CS229: Machine Learning](https://cs229.stanford.edu/) - Curs universitar comprehensiv
- [3Blue1Brown: Neural Networks](https://www.3blue1brown.com/topics/neural-networks) - Vizualizări excelente

---

## ✅ Lista de Auto-Verificare

### Înțelegere Teoretică
- [ ] Pot explica diferența dintre supervised și unsupervised learning
- [ ] Înțeleg de ce normalizarea datelor este importantă
- [ ] Pot deriva manual gradientul pentru MSE
- [ ] Înțeleg de ce perceptronul nu poate rezolva XOR

### Implementare Practică
- [ ] Am implementat regresie liniară cu gradient descent
- [ ] Modelul meu converge (loss scade în timp)
- [ ] Am implementat K-Means cu k-means++ initialization
- [ ] K-NN clasifică corect pe date de test
- [ ] Perceptronul converge pe date liniar separabile

### Evaluare și Debugging
- [ ] Știu să interpretez curba de loss
- [ ] Pot identifica overfitting vs. underfitting
- [ ] Știu să aleg k optim pentru K-Means (metoda Elbow)
- [ ] Înțeleg matricea de confuzie

---

## 💼 Pregătire pentru Interviu

### Întrebări Frecvente

1. **Care este diferența dintre regresie și clasificare?**
   - Regresia prezice valori continue (ex: preț), clasificarea prezice categorii discrete (ex: spam/not spam)

2. **De ce normalizăm datele înainte de K-Means?**
   - Features cu scale diferite domină distanța Euclidiană; normalizarea asigură contribuție egală

3. **Care e complexitatea K-NN la inference?**
   - O(n·d) pentru fiecare predicție, unde n = număr samples, d = dimensiuni; de aceea e "lazy" și ineficient pentru date mari

4. **De ce perceptronul nu poate învăța XOR?**
   - XOR nu e liniar separabil; o singură hiperplan nu poate separa clasele

5. **Când folosești K-Means vs. Hierarchical Clustering?**
   - K-Means: k cunoscut, date mari, clustere sferice; Hierarchical: k necunoscut, dendrogramă utilă, date mici-medii

---

## 🔗 Preview Săptămâna Următoare

**Săptămâna 19: Algoritmi pentru IoT și Stream Processing**

Vom explora algoritmi optimizați pentru dispozitive cu resurse limitate și procesarea fluxurilor de date în timp real:

- Filtre digitale: Moving Average, Exponential Moving Average, Kalman 1D
- Ferestre temporale: Tumbling, Sliding, Session
- Detectarea anomaliilor în stream-uri de date
- Simulare senzori IoT în Docker

Legătura cu ML: Algoritmii de streaming pot incorpora modele ML pentru predicții în timp real (online learning).

---

## 🔧 Hardware Real (Opțional)

Kit-uri Arduino 32-bit disponibile la Biblioteca Facultății pentru studenții interesați de ML pe edge devices:

- **ESP32 DevKit** - Dual-core, WiFi/BLE, suficient pentru inference pe modele mici
- **Arduino Due** - ARM Cortex-M3, mai multă memorie pentru modele complexe

**Avantaje hardware real:**
- Înțelegerea constrângerilor de memorie și compute
- Experiența cu fixed-point arithmetic (în loc de float)
- Aplicații IoT practice cu ML la edge

**Notă**: Toate exercițiile sunt 100% realizabile în Docker pe laptop standard. Hardware-ul este doar pentru explorare suplimentară.

---

*Acest material face parte din cursul "Algoritmi și Tehnici de Programare" (ATP)*
*Academia de Studii Economice din București - CSIE*
*Actualizat: Ianuarie 2026*
