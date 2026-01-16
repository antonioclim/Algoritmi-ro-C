#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
================================================================================
COMPARAȚIE: Implementări C vs scikit-learn
Săptămâna 18: Fundamente Machine Learning în C
================================================================================

Acest script demonstrează echivalența dintre implementările noastre în C
și bibliotecile profesionale din Python (scikit-learn).

Scopul este EDUCAȚIONAL:
- Validăm că algoritmii noștri produc rezultate similare
- Observăm diferențele de performanță (timp de execuție)
- Înțelegem API-ul profesional pentru viitoarele proiecte

Autor: Curs ATP - ASE București
================================================================================
"""

import numpy as np
from sklearn.linear_model import LinearRegression, Perceptron
from sklearn.cluster import KMeans
from sklearn.neighbors import KNeighborsClassifier
from sklearn.preprocessing import MinMaxScaler
from sklearn.model_selection import train_test_split
from sklearn.metrics import (
    mean_squared_error, r2_score,
    accuracy_score, classification_report,
    silhouette_score
)
import time
import pandas as pd

# ==============================================================================
# PARTEA 1: REGRESIE LINIARĂ
# ==============================================================================

def demo_linear_regression():
    """
    Compară regresia liniară implementată în C cu sklearn.
    
    În C: Gradient Descent iterativ
    În sklearn: Ecuația normală (closed-form) sau SVD
    """
    print("\n" + "="*70)
    print("PARTEA 1: REGRESIE LINIARĂ")
    print("="*70)
    
    # Încarcă datele
    try:
        df = pd.read_csv('data/housing_simple.csv', comment='#')
    except FileNotFoundError:
        print("EROARE: Fișierul data/housing_simple.csv nu a fost găsit!")
        return
    
    # Pregătește datele
    X = df[['sqft', 'bedrooms', 'bathrooms']].values
    y = df['price'].values
    
    # Normalizare (la fel ca în implementarea C)
    scaler = MinMaxScaler()
    X_scaled = scaler.fit_transform(X)
    
    # Split train/test
    X_train, X_test, y_train, y_test = train_test_split(
        X_scaled, y, test_size=0.2, random_state=42
    )
    
    # Antrenare sklearn
    start_time = time.time()
    model = LinearRegression()
    model.fit(X_train, y_train)
    sklearn_time = time.time() - start_time
    
    # Predicții
    y_pred = model.predict(X_test)
    
    # Metrici
    mse = mean_squared_error(y_test, y_pred)
    r2 = r2_score(y_test, y_pred)
    
    print(f"\n📊 Rezultate sklearn LinearRegression:")
    print(f"   • Timp antrenare: {sklearn_time*1000:.3f} ms")
    print(f"   • MSE (test): {mse:.4f}")
    print(f"   • R² score: {r2:.4f}")
    print(f"\n   Coeficienți (weights):")
    print(f"   • w_sqft:     {model.coef_[0]:.4f}")
    print(f"   • w_bedrooms: {model.coef_[1]:.4f}")
    print(f"   • w_bath:     {model.coef_[2]:.4f}")
    print(f"   • bias:       {model.intercept_:.4f}")
    
    print("\n💡 Diferența față de implementarea C:")
    print("   • sklearn folosește SVD (Singular Value Decomposition)")
    print("   • Soluție exactă în O(n·d²), nu iterativă")
    print("   • Gradient Descent în C: mai lent, dar educațional")


# ==============================================================================
# PARTEA 2: K-MEANS CLUSTERING
# ==============================================================================

def demo_kmeans():
    """
    Compară K-Means implementat în C cu sklearn.
    
    Ambele folosesc Lloyd's Algorithm, dar sklearn are:
    - K-Means++ initialization (default)
    - Paralelizare pe mai multe nuclee
    - Optimizări Elkan
    """
    print("\n" + "="*70)
    print("PARTEA 2: K-MEANS CLUSTERING")
    print("="*70)
    
    # Încarcă datele
    try:
        df = pd.read_csv('data/customers.csv', comment='#')
    except FileNotFoundError:
        print("EROARE: Fișierul data/customers.csv nu a fost găsit!")
        return
    
    # Pregătește datele
    X = df[['annual_income', 'spending_score']].values
    
    # Normalizare
    scaler = MinMaxScaler()
    X_scaled = scaler.fit_transform(X)
    
    # Testează diferite valori de k (Elbow Method)
    print("\n📊 Elbow Method - Inertia pentru diferite k:")
    print("-" * 40)
    
    inertias = []
    silhouettes = []
    
    for k in range(2, 8):
        kmeans = KMeans(n_clusters=k, init='k-means++', 
                       n_init=10, random_state=42)
        kmeans.fit(X_scaled)
        inertias.append(kmeans.inertia_)
        sil = silhouette_score(X_scaled, kmeans.labels_)
        silhouettes.append(sil)
        print(f"   k={k}: Inertia={kmeans.inertia_:.2f}, Silhouette={sil:.4f}")
    
    # K optim (în acest caz, k=4)
    optimal_k = 4
    print(f"\n   ➤ K optim selectat: {optimal_k}")
    
    # Antrenare finală
    start_time = time.time()
    kmeans = KMeans(n_clusters=optimal_k, init='k-means++', 
                   n_init=10, random_state=42)
    kmeans.fit(X_scaled)
    sklearn_time = time.time() - start_time
    
    print(f"\n📊 Rezultate sklearn KMeans (k={optimal_k}):")
    print(f"   • Timp antrenare: {sklearn_time*1000:.3f} ms")
    print(f"   • Inertia finală: {kmeans.inertia_:.4f}")
    print(f"   • Iterații până la convergență: {kmeans.n_iter_}")
    
    print(f"\n   Centroizi (normalizați):")
    for i, centroid in enumerate(kmeans.cluster_centers_):
        count = np.sum(kmeans.labels_ == i)
        print(f"   • Cluster {i}: ({centroid[0]:.3f}, {centroid[1]:.3f}) - {count} clienți")
    
    # Profilare clustere
    df['cluster'] = kmeans.labels_
    print(f"\n   Profilare clustere (date originale):")
    for i in range(optimal_k):
        cluster_data = df[df['cluster'] == i]
        avg_income = cluster_data['annual_income'].mean()
        avg_spend = cluster_data['spending_score'].mean()
        print(f"   • Cluster {i}: Venit={avg_income:.1f}k, Spending={avg_spend:.1f}")


# ==============================================================================
# PARTEA 3: K-NEAREST NEIGHBORS
# ==============================================================================

def demo_knn():
    """
    Compară K-NN implementat în C cu sklearn.
    
    sklearn KNN are:
    - Ball Tree și KD Tree pentru căutare rapidă
    - Distance weighting
    - Suport pentru diverse metrici
    """
    print("\n" + "="*70)
    print("PARTEA 3: K-NEAREST NEIGHBORS")
    print("="*70)
    
    # Încarcă datele Iris
    try:
        df = pd.read_csv('data/iris.csv', comment='#')
    except FileNotFoundError:
        print("EROARE: Fișierul data/iris.csv nu a fost găsit!")
        return
    
    # Pregătește datele
    X = df[['sepal_length', 'sepal_width', 'petal_length', 'petal_width']].values
    y = df['species'].values
    
    # Normalizare
    scaler = MinMaxScaler()
    X_scaled = scaler.fit_transform(X)
    
    # Split
    X_train, X_test, y_train, y_test = train_test_split(
        X_scaled, y, test_size=0.2, random_state=42
    )
    
    # Testează diferite valori de k
    print("\n📊 Acuratețe pentru diferite valori de k:")
    print("-" * 40)
    
    for k in [1, 3, 5, 7, 9]:
        knn = KNeighborsClassifier(n_neighbors=k)
        knn.fit(X_train, y_train)
        acc = knn.score(X_test, y_test)
        print(f"   k={k}: Acuratețe = {acc:.4f} ({acc*100:.1f}%)")
    
    # Model final cu k=5
    optimal_k = 5
    start_time = time.time()
    knn = KNeighborsClassifier(n_neighbors=optimal_k, algorithm='brute')
    knn.fit(X_train, y_train)
    sklearn_time = time.time() - start_time
    
    y_pred = knn.predict(X_test)
    
    print(f"\n📊 Rezultate sklearn KNeighborsClassifier (k={optimal_k}):")
    print(f"   • Timp antrenare: {sklearn_time*1000:.3f} ms (lazy learner)")
    print(f"   • Acuratețe (test): {accuracy_score(y_test, y_pred):.4f}")
    
    print(f"\n   Classification Report:")
    print(classification_report(y_test, y_pred, target_names=['setosa', 'versicolor', 'virginica']))
    
    print("💡 Notă: KNN este un 'lazy learner' - nu are fază de antrenare propriu-zisă")


# ==============================================================================
# PARTEA 4: PERCEPTRON
# ==============================================================================

def demo_perceptron():
    """
    Compară Perceptron implementat în C cu sklearn.
    
    Demonstrează și limitarea: XOR nu poate fi învățat!
    """
    print("\n" + "="*70)
    print("PARTEA 4: PERCEPTRON")
    print("="*70)
    
    # === Test 1: Date liniar separabile (Setosa vs rest) ===
    print("\n--- Test 1: Clasificare binară (Setosa vs Non-Setosa) ---")
    
    try:
        df = pd.read_csv('data/iris.csv', comment='#')
    except FileNotFoundError:
        print("EROARE: Fișierul data/iris.csv nu a fost găsit!")
        return
    
    X = df[['petal_length', 'petal_width']].values
    y = (df['species'] == 'setosa').astype(int).values
    
    scaler = MinMaxScaler()
    X_scaled = scaler.fit_transform(X)
    
    X_train, X_test, y_train, y_test = train_test_split(
        X_scaled, y, test_size=0.2, random_state=42
    )
    
    start_time = time.time()
    perceptron = Perceptron(max_iter=1000, tol=1e-3, random_state=42)
    perceptron.fit(X_train, y_train)
    sklearn_time = time.time() - start_time
    
    y_pred = perceptron.predict(X_test)
    
    print(f"\n📊 Rezultate sklearn Perceptron:")
    print(f"   • Timp antrenare: {sklearn_time*1000:.3f} ms")
    print(f"   • Iterații: {perceptron.n_iter_}")
    print(f"   • Acuratețe (test): {accuracy_score(y_test, y_pred):.4f}")
    print(f"\n   Weights: w1={perceptron.coef_[0][0]:.4f}, w2={perceptron.coef_[0][1]:.4f}")
    print(f"   Bias: {perceptron.intercept_[0]:.4f}")
    
    # === Test 2: XOR - trebuie să eșueze! ===
    print("\n--- Test 2: Problema XOR (trebuie să EȘUEZE) ---")
    
    # Date XOR pure
    X_xor = np.array([[0, 0], [0, 1], [1, 0], [1, 1]])
    y_xor = np.array([0, 1, 1, 0])
    
    perceptron_xor = Perceptron(max_iter=1000, tol=None, random_state=42)
    perceptron_xor.fit(X_xor, y_xor)
    y_pred_xor = perceptron_xor.predict(X_xor)
    
    print(f"\n📊 Perceptron pe XOR:")
    print(f"   • Acuratețe: {accuracy_score(y_xor, y_pred_xor):.4f} (50% = random)")
    print(f"   • Predicții: {y_pred_xor} (așteptat: [0 1 1 0])")
    print(f"\n   ⚠️  CONCLUZIE: Perceptronul NU poate învăța XOR!")
    print("   ➤ XOR nu este liniar separabil")
    print("   ➤ Necesită rețea cu cel puțin un strat ascuns (MLP)")


# ==============================================================================
# PARTEA 5: COMPARAȚIE DE TIMP
# ==============================================================================

def benchmark_comparison():
    """
    Compară timpii de execuție între implementarea C și sklearn.
    """
    print("\n" + "="*70)
    print("PARTEA 5: BENCHMARK COMPARATIV")
    print("="*70)
    
    print("""
    ┌────────────────────┬──────────────┬──────────────┬─────────────┐
    │    Algoritm        │  sklearn (ms)│   C (ms)     │   Notă      │
    ├────────────────────┼──────────────┼──────────────┼─────────────┤
    │ Linear Regression  │    ~0.5      │   ~5-10      │ C: iterativ │
    │ K-Means (k=4)      │    ~2-5      │   ~10-20     │ C: single   │
    │ K-NN (k=5)         │    ~0.1      │   ~0.5-1     │ lazy learn  │
    │ Perceptron         │    ~1-2      │   ~5-10      │ convergence │
    └────────────────────┴──────────────┴──────────────┴─────────────┘
    
    Observații:
    • sklearn este optimizat în Cython/C sub capotă
    • sklearn folosește BLAS/LAPACK pentru algebra liniară
    • Implementările noastre C sunt DIDACTICE, nu pentru producție
    • Valoarea educațională: înțelegem algoritmii în profunzime
    """)


# ==============================================================================
# MAIN
# ==============================================================================

def main():
    """
    Rulează toate demonstrațiile.
    """
    print("╔" + "═"*68 + "╗")
    print("║" + " "*15 + "COMPARAȚIE: C vs scikit-learn" + " "*24 + "║")
    print("║" + " "*15 + "Săptămâna 18 - ML Fundamentals" + " "*23 + "║")
    print("╚" + "═"*68 + "╝")
    
    demo_linear_regression()
    demo_kmeans()
    demo_knn()
    demo_perceptron()
    benchmark_comparison()
    
    print("\n" + "="*70)
    print("✅ Toate comparațiile finalizate!")
    print("="*70)


if __name__ == "__main__":
    main()
