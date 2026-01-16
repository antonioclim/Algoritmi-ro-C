#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
================================================================================
OPERAȚII MATRICEALE: NumPy vs Implementare Manuală în C
Săptămâna 18: Fundamente Machine Learning în C
================================================================================

Acest script demonstrează cum operațiile pe care le-am implementat manual
în C sunt disponibile ca funcții optimizate în NumPy.

Scopul este să înțelegem:
1. Ce face NumPy "sub capotă" (ceea ce am implementat noi)
2. De ce NumPy este mult mai rapid (BLAS, SIMD, cache optimization)
3. Cum să traducem pseudocod → NumPy pentru prototipare rapidă

================================================================================
"""

import numpy as np
import time

# ==============================================================================
# PARTEA 1: OPERAȚII VECTORIALE
# ==============================================================================

def vector_operations():
    """
    Demonstrează operații pe vectori în NumPy.
    
    În C am implementat:
    - dot_product(v1, v2)
    - euclidean_distance(v1, v2)
    - normalize_minmax(v)
    """
    print("\n" + "="*70)
    print("PARTEA 1: OPERAȚII VECTORIALE")
    print("="*70)
    
    # Vectori de test
    v1 = np.array([1.0, 2.0, 3.0, 4.0, 5.0])
    v2 = np.array([2.0, 3.0, 4.0, 5.0, 6.0])
    
    print("\n📊 Vectori de test:")
    print(f"   v1 = {v1}")
    print(f"   v2 = {v2}")
    
    # --- Produs scalar (dot product) ---
    print("\n--- Produs Scalar (Dot Product) ---")
    
    # Metoda 1: np.dot()
    dot1 = np.dot(v1, v2)
    
    # Metoda 2: operatorul @
    dot2 = v1 @ v2
    
    # Metoda 3: sum de produs element-wise
    dot3 = np.sum(v1 * v2)
    
    print(f"   np.dot(v1, v2)     = {dot1}")
    print(f"   v1 @ v2            = {dot2}")
    print(f"   np.sum(v1 * v2)    = {dot3}")
    
    # Echivalent în C:
    print("""
   /* Implementare C (din example1.c): */
   double dot_product(double *v1, double *v2, int n) {
       double sum = 0.0;
       for (int i = 0; i < n; i++) {
           sum += v1[i] * v2[i];
       }
       return sum;
   }
    """)
    
    # --- Distanță Euclidiană ---
    print("--- Distanță Euclidiană ---")
    
    # Metoda 1: np.linalg.norm
    dist1 = np.linalg.norm(v1 - v2)
    
    # Metoda 2: manual cu sqrt și sum
    dist2 = np.sqrt(np.sum((v1 - v2) ** 2))
    
    print(f"   np.linalg.norm(v1 - v2)           = {dist1:.6f}")
    print(f"   np.sqrt(np.sum((v1 - v2) ** 2))   = {dist2:.6f}")
    
    # Echivalent în C:
    print("""
   /* Implementare C: */
   double euclidean_distance(double *v1, double *v2, int n) {
       double sum = 0.0;
       for (int i = 0; i < n; i++) {
           double diff = v1[i] - v2[i];
           sum += diff * diff;
       }
       return sqrt(sum);
   }
    """)
    
    # --- Normalizare Min-Max ---
    print("--- Normalizare Min-Max ---")
    
    # Date pentru normalizare
    data = np.array([10.0, 20.0, 30.0, 40.0, 50.0])
    
    # Normalizare: (x - min) / (max - min)
    normalized = (data - data.min()) / (data.max() - data.min())
    
    print(f"   Date originale: {data}")
    print(f"   Normalizate:    {normalized}")
    
    print("""
   /* Implementare C: */
   void normalize_minmax(double *data, int n, double *out) {
       double min_val = data[0], max_val = data[0];
       for (int i = 1; i < n; i++) {
           if (data[i] < min_val) min_val = data[i];
           if (data[i] > max_val) max_val = data[i];
       }
       double range = max_val - min_val;
       for (int i = 0; i < n; i++) {
           out[i] = (data[i] - min_val) / range;
       }
   }
    """)


# ==============================================================================
# PARTEA 2: OPERAȚII MATRICEALE
# ==============================================================================

def matrix_operations():
    """
    Demonstrează operații pe matrice în NumPy.
    
    În C am implementat:
    - matrix_create(rows, cols)
    - matrix_multiply(A, B)
    - matrix_transpose(M)
    """
    print("\n" + "="*70)
    print("PARTEA 2: OPERAȚII MATRICEALE")
    print("="*70)
    
    # Matrice de test
    A = np.array([
        [1, 2, 3],
        [4, 5, 6]
    ], dtype=float)
    
    B = np.array([
        [7, 8],
        [9, 10],
        [11, 12]
    ], dtype=float)
    
    print("\n📊 Matrice de test:")
    print(f"   A (2x3):\n{A}")
    print(f"\n   B (3x2):\n{B}")
    
    # --- Înmulțire Matriceală ---
    print("\n--- Înmulțire Matriceală (A @ B) ---")
    
    C = A @ B  # sau np.matmul(A, B) sau np.dot(A, B)
    
    print(f"   A @ B (2x2):\n{C}")
    
    print("""
   /* Implementare C - O(n³): */
   void matrix_multiply(Matrix *A, Matrix *B, Matrix *C) {
       for (int i = 0; i < A->rows; i++) {
           for (int j = 0; j < B->cols; j++) {
               double sum = 0.0;
               for (int k = 0; k < A->cols; k++) {
                   sum += A->data[i * A->cols + k] * 
                          B->data[k * B->cols + j];
               }
               C->data[i * C->cols + j] = sum;
           }
       }
   }
    """)
    
    # --- Transpunere ---
    print("--- Transpunere ---")
    
    A_T = A.T  # sau np.transpose(A)
    
    print(f"   A.T (3x2):\n{A_T}")
    
    print("""
   /* Implementare C: */
   void matrix_transpose(Matrix *M, Matrix *M_T) {
       for (int i = 0; i < M->rows; i++) {
           for (int j = 0; j < M->cols; j++) {
               M_T->data[j * M_T->cols + i] = 
                   M->data[i * M->cols + j];
           }
       }
   }
    """)
    
    # --- Broadcasting (nu există direct în C) ---
    print("--- Broadcasting (caracteristică NumPy) ---")
    
    # Adunare scalar la matrice
    A_plus_10 = A + 10
    
    # Adunare vector la fiecare rând
    row_vector = np.array([100, 200, 300])
    A_plus_row = A + row_vector
    
    print(f"   A + 10:\n{A_plus_10}")
    print(f"\n   A + [100, 200, 300]:\n{A_plus_row}")
    
    print("""
   /* În C trebuie să iterăm explicit: */
   for (int i = 0; i < rows; i++) {
       for (int j = 0; j < cols; j++) {
           A[i][j] += 10;  // sau row_vector[j]
       }
   }
    """)


# ==============================================================================
# PARTEA 3: OPERAȚII SPECIFICE ML
# ==============================================================================

def ml_operations():
    """
    Operații comune în Machine Learning.
    """
    print("\n" + "="*70)
    print("PARTEA 3: OPERAȚII SPECIFICE ML")
    print("="*70)
    
    # Date pentru exemple
    X = np.array([
        [1.0, 2.0],
        [3.0, 4.0],
        [5.0, 6.0],
        [7.0, 8.0]
    ])
    y = np.array([3.0, 7.0, 11.0, 15.0])  # y = x1 + x2
    
    print("\n📊 Date de antrenare:")
    print(f"   X (4x2):\n{X}")
    print(f"   y: {y}")
    
    # --- Mean Squared Error ---
    print("\n--- Mean Squared Error (MSE) ---")
    
    # Simulăm predicții
    y_pred = np.array([3.5, 6.5, 11.5, 14.5])
    
    # MSE = (1/n) * Σ(y - y_pred)²
    mse = np.mean((y - y_pred) ** 2)
    
    print(f"   y_true: {y}")
    print(f"   y_pred: {y_pred}")
    print(f"   MSE:    {mse}")
    
    print("""
   /* Implementare C: */
   double compute_mse(double *y_true, double *y_pred, int n) {
       double sum = 0.0;
       for (int i = 0; i < n; i++) {
           double error = y_true[i] - y_pred[i];
           sum += error * error;
       }
       return sum / n;
   }
    """)
    
    # --- Gradient pentru Linear Regression ---
    print("--- Gradient (pentru Gradient Descent) ---")
    
    # Pentru regresie liniară: grad = (2/n) * X.T @ (X @ w - y)
    # Simplificat: grad_i = mean((pred - y) * X[:, i])
    
    w = np.array([0.5, 0.5])  # weights inițiale
    pred = X @ w
    error = pred - y
    
    gradient = (2.0 / len(y)) * X.T @ error
    
    print(f"   Weights inițiale: {w}")
    print(f"   Predicții: {pred}")
    print(f"   Erori: {error}")
    print(f"   Gradient: {gradient}")
    
    print("""
   /* Implementare C (pentru o singură caracteristică): */
   double compute_gradient(double *X, double *y, double *pred, 
                          int n, int feature_idx, int n_features) {
       double sum = 0.0;
       for (int i = 0; i < n; i++) {
           double error = pred[i] - y[i];
           sum += error * X[i * n_features + feature_idx];
       }
       return (2.0 / n) * sum;
   }
    """)
    
    # --- Softmax (pentru clasificare multi-clasă) ---
    print("--- Softmax (pentru probabilități) ---")
    
    logits = np.array([2.0, 1.0, 0.1])
    
    # Softmax: exp(x_i) / Σ exp(x_j)
    # Cu trick numeric pentru stabilitate: exp(x_i - max(x))
    exp_shifted = np.exp(logits - np.max(logits))
    softmax = exp_shifted / np.sum(exp_shifted)
    
    print(f"   Logits:  {logits}")
    print(f"   Softmax: {softmax}")
    print(f"   Sum:     {np.sum(softmax)} (trebuie să fie 1.0)")
    
    print("""
   /* Implementare C (cu stabilitate numerică): */
   void softmax(double *logits, int n, double *output) {
       double max_val = logits[0];
       for (int i = 1; i < n; i++) {
           if (logits[i] > max_val) max_val = logits[i];
       }
       double sum = 0.0;
       for (int i = 0; i < n; i++) {
           output[i] = exp(logits[i] - max_val);
           sum += output[i];
       }
       for (int i = 0; i < n; i++) {
           output[i] /= sum;
       }
   }
    """)


# ==============================================================================
# PARTEA 4: BENCHMARK COMPARATIV
# ==============================================================================

def benchmark():
    """
    Compară timpul de execuție: NumPy vs Python pur.
    """
    print("\n" + "="*70)
    print("PARTEA 4: BENCHMARK (NumPy vs Python pur)")
    print("="*70)
    
    n = 10000
    
    # Vectori mari
    v1 = np.random.randn(n)
    v2 = np.random.randn(n)
    v1_list = v1.tolist()
    v2_list = v2.tolist()
    
    print(f"\n📊 Benchmark pentru vectori de dimensiune {n}:")
    print("-" * 50)
    
    # --- Dot product: NumPy ---
    start = time.time()
    for _ in range(1000):
        result_np = np.dot(v1, v2)
    numpy_time = time.time() - start
    
    # --- Dot product: Python pur ---
    start = time.time()
    for _ in range(100):  # doar 100 iterații (mult mai lent)
        result_py = sum(a * b for a, b in zip(v1_list, v2_list))
    python_time = (time.time() - start) * 10  # scalăm la 1000 iterații
    
    print(f"   Dot Product (1000 iterații):")
    print(f"   • NumPy:      {numpy_time*1000:.2f} ms")
    print(f"   • Python pur: {python_time*1000:.2f} ms (estimat)")
    print(f"   • Speedup:    {python_time/numpy_time:.1f}x")
    
    # --- Matrice multiplication ---
    m = 500
    A = np.random.randn(m, m)
    B = np.random.randn(m, m)
    
    start = time.time()
    C = A @ B
    numpy_matmul_time = time.time() - start
    
    print(f"\n   Înmulțire matrice {m}x{m}:")
    print(f"   • NumPy: {numpy_matmul_time*1000:.2f} ms")
    print(f"   • C naiv ar dura: ~{m**3 / 1e9 * 1000:.0f} ms (O(n³))")
    print(f"   • NumPy folosește BLAS optimizat cu SIMD")


# ==============================================================================
# MAIN
# ==============================================================================

def main():
    print("╔" + "═"*68 + "╗")
    print("║" + " "*12 + "OPERAȚII MATRICEALE: NumPy vs C Manual" + " "*18 + "║")
    print("║" + " "*15 + "Săptămâna 18 - ML Fundamentals" + " "*23 + "║")
    print("╚" + "═"*68 + "╝")
    
    vector_operations()
    matrix_operations()
    ml_operations()
    benchmark()
    
    print("\n" + "="*70)
    print("💡 CONCLUZIE:")
    print("   • NumPy este 50-100x mai rapid decât Python pur")
    print("   • Implementările noastre C sunt DIDACTICE")
    print("   • Pentru producție: folosiți biblioteci optimizate!")
    print("="*70)


if __name__ == "__main__":
    main()
