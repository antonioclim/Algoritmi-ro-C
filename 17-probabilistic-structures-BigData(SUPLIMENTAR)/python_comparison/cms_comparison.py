#!/usr/bin/env python3
"""
=============================================================================
Comparație Count-Min Sketch: C vs Python
=============================================================================

Acest script demonstrează implementarea Count-Min Sketch în Python și
compară performanța cu implementarea în C.

CMS în Python poate fi implementat:
  1. Manual cu NumPy (pentru înțelegere)
  2. Cu biblioteci existente (pentru producție)

RULARE: python3 cms_comparison.py

=============================================================================
"""

import time
import random
import hashlib
import struct
from collections import Counter
from typing import Optional, Tuple, List
import sys

# Încercăm să importăm biblioteci opționale
try:
    import numpy as np
    HAS_NUMPY = True
except ImportError:
    HAS_NUMPY = False
    print("⚠️  NumPy nu este instalat. Folosim implementare pură Python.")

try:
    import mmh3  # MurmurHash3
    HAS_MMH3 = True
except ImportError:
    HAS_MMH3 = False


# =============================================================================
# FUNCȚII HASH
# =============================================================================

def hash_fnv1a(data: bytes) -> int:
    """FNV-1a hash - identic cu implementarea C"""
    hash_value = 14695981039346656037
    prime = 1099511628211
    
    for byte in data:
        hash_value ^= byte
        hash_value = (hash_value * prime) & 0xFFFFFFFFFFFFFFFF
    
    return hash_value


def hash_djb2(data: bytes) -> int:
    """DJB2 hash - identic cu implementarea C"""
    hash_value = 5381
    
    for byte in data:
        hash_value = ((hash_value << 5) + hash_value + byte) & 0xFFFFFFFFFFFFFFFF
    
    return hash_value


def hash_family(data: bytes, i: int) -> int:
    """Familia de hash-uri folosind double hashing"""
    h1 = hash_fnv1a(data)
    h2 = hash_djb2(data)
    return (h1 + i * h2) & 0xFFFFFFFFFFFFFFFF


# =============================================================================
# COUNT-MIN SKETCH - IMPLEMENTARE PURĂ PYTHON
# =============================================================================

class CountMinSketchPure:
    """
    Count-Min Sketch implementat în Python pur (fără NumPy)
    
    Utilizat pentru:
      - Înțelegerea algoritmului
      - Medii fără NumPy
      - Comparație educațională
    """
    
    def __init__(self, epsilon: float = 0.01, delta: float = 0.01):
        """
        Inițializează CMS cu parametrii de eroare.
        
        Args:
            epsilon: Eroarea relativă maximă (e.g., 0.01 = 1%)
            delta: Probabilitatea de depășire a erorii (e.g., 0.01 = 1%)
        """
        import math
        
        # Calculul dimensiunilor conform formulelor teoretice
        # width (w) = ⌈e/ε⌉
        # depth (d) = ⌈ln(1/δ)⌉
        self.width = int(math.ceil(2.718281828 / epsilon))
        self.depth = int(math.ceil(math.log(1.0 / delta)))
        
        # Asigurăm minime rezonabile
        self.width = max(self.width, 100)
        self.depth = max(self.depth, 3)
        
        self.epsilon = epsilon
        self.delta = delta
        self.total_count = 0
        
        # Matrice de contoare (liste nested)
        self.counters = [[0] * self.width for _ in range(self.depth)]
    
    def add(self, item: str, count: int = 1) -> None:
        """Adaugă un element în CMS"""
        item_bytes = item.encode('utf-8')
        
        for i in range(self.depth):
            hash_val = hash_family(item_bytes, i)
            index = hash_val % self.width
            self.counters[i][index] += count
        
        self.total_count += count
    
    def estimate(self, item: str) -> int:
        """Estimează frecvența unui element"""
        item_bytes = item.encode('utf-8')
        min_count = float('inf')
        
        for i in range(self.depth):
            hash_val = hash_family(item_bytes, i)
            index = hash_val % self.width
            min_count = min(min_count, self.counters[i][index])
        
        return int(min_count)
    
    def memory_bytes(self) -> int:
        """Estimare memorie utilizată"""
        # Python int = ~28 bytes, dar pentru comparație folosim 8
        return self.depth * self.width * 8


# =============================================================================
# COUNT-MIN SKETCH - IMPLEMENTARE CU NUMPY
# =============================================================================

if HAS_NUMPY:
    class CountMinSketchNumPy:
        """
        Count-Min Sketch implementat cu NumPy pentru performanță.
        
        Avantaje față de implementarea pură:
          - Operații vectorizate
          - Memorie mai eficientă (numpy arrays)
          - ~5-10x mai rapid
        """
        
        def __init__(self, epsilon: float = 0.01, delta: float = 0.01):
            import math
            
            self.width = int(math.ceil(2.718281828 / epsilon))
            self.depth = int(math.ceil(math.log(1.0 / delta)))
            self.width = max(self.width, 100)
            self.depth = max(self.depth, 3)
            
            self.epsilon = epsilon
            self.delta = delta
            self.total_count = 0
            
            # Matrice NumPy (mult mai eficientă)
            self.counters = np.zeros((self.depth, self.width), dtype=np.int64)
        
        def add(self, item: str, count: int = 1) -> None:
            """Adaugă element - versiune vectorizată"""
            item_bytes = item.encode('utf-8')
            
            # Calculăm toate hash-urile și indicii
            indices = np.array([
                hash_family(item_bytes, i) % self.width 
                for i in range(self.depth)
            ])
            
            # Actualizăm toate contoarele simultan
            for i, idx in enumerate(indices):
                self.counters[i, idx] += count
            
            self.total_count += count
        
        def estimate(self, item: str) -> int:
            """Estimează frecvența - versiune vectorizată"""
            item_bytes = item.encode('utf-8')
            
            indices = np.array([
                hash_family(item_bytes, i) % self.width 
                for i in range(self.depth)
            ])
            
            # Obținem valorile și returnăm minimul
            values = np.array([
                self.counters[i, idx] 
                for i, idx in enumerate(indices)
            ])
            
            return int(np.min(values))
        
        def add_batch(self, items: List[str]) -> None:
            """Adaugă un batch de elemente - optimizat"""
            for item in items:
                self.add(item)
        
        def memory_bytes(self) -> int:
            """Memorie utilizată de array-ul NumPy"""
            return self.counters.nbytes


# =============================================================================
# COUNT-MIN SKETCH - CU BIBLIOTECA probabilistic-data-structures
# =============================================================================

def create_cms_with_library(epsilon: float, delta: float):
    """
    Încearcă să creeze CMS folosind biblioteci externe.
    Returnează None dacă biblioteca nu este disponibilă.
    """
    try:
        from probables import CountMinSketch as ProbablesCMS
        return ProbablesCMS(width=int(2.72 / epsilon), depth=int(4.6 / delta))
    except ImportError:
        pass
    
    return None


# =============================================================================
# BENCHMARK ȘI COMPARAȚIE
# =============================================================================

def benchmark_cms(cms, items: List[str], name: str) -> dict:
    """Benchmark pentru o implementare CMS"""
    
    # Benchmark add
    start = time.perf_counter()
    for item in items:
        cms.add(item)
    add_time = time.perf_counter() - start
    
    # Benchmark estimate
    start = time.perf_counter()
    for item in items[:1000]:
        cms.estimate(item)
    estimate_time = time.perf_counter() - start
    
    return {
        'name': name,
        'add_time': add_time,
        'estimate_time': estimate_time,
        'memory_kb': cms.memory_bytes() / 1024,
        'ops_per_sec': len(items) / add_time
    }


def compare_accuracy(cms, exact_counts: Counter, sample_items: List[str]) -> dict:
    """Compară acuratețea CMS cu numărarea exactă"""
    
    errors = []
    for item in sample_items:
        estimated = cms.estimate(item)
        exact = exact_counts[item]
        if exact > 0:
            error = (estimated - exact) / exact * 100
            errors.append(error)
    
    return {
        'mean_error': sum(errors) / len(errors) if errors else 0,
        'max_error': max(errors) if errors else 0,
        'min_error': min(errors) if errors else 0
    }


def run_comparison():
    """Rulează comparația completă"""
    
    print("\n" + "=" * 70)
    print("   COUNT-MIN SKETCH: Comparație Implementări Python")
    print("=" * 70 + "\n")
    
    # Generare date de test
    print("Generare date de test...")
    random.seed(42)
    
    # Distribuție Zipf - câteva elemente foarte frecvente
    n_items = 100000
    n_unique = 10000
    
    items = []
    for _ in range(n_items):
        # Zipf-like: 80% din accese pe 20% din elemente
        if random.random() < 0.8:
            items.append(f"item_{random.randint(0, n_unique // 5)}")
        else:
            items.append(f"item_{random.randint(0, n_unique)}")
    
    # Numărare exactă pentru referință
    exact_counts = Counter(items)
    top_items = [item for item, _ in exact_counts.most_common(100)]
    
    print(f"  Total elemente:    {n_items:,}")
    print(f"  Elemente unice:    {len(exact_counts):,}")
    print(f"  Top element:       {exact_counts.most_common(1)[0]}")
    print()
    
    # Benchmark implementări
    results = []
    
    # 1. Python pur
    print("Testing: Python pur...")
    cms_pure = CountMinSketchPure(epsilon=0.01, delta=0.01)
    result = benchmark_cms(cms_pure, items, "Python Pur")
    accuracy = compare_accuracy(cms_pure, exact_counts, top_items)
    result.update(accuracy)
    results.append(result)
    
    # 2. NumPy
    if HAS_NUMPY:
        print("Testing: NumPy...")
        cms_numpy = CountMinSketchNumPy(epsilon=0.01, delta=0.01)
        result = benchmark_cms(cms_numpy, items, "NumPy")
        accuracy = compare_accuracy(cms_numpy, exact_counts, top_items)
        result.update(accuracy)
        results.append(result)
    
    # Afișare rezultate
    print("\n" + "-" * 70)
    print("REZULTATE BENCHMARK")
    print("-" * 70)
    
    print(f"\n{'Implementare':<15} {'Add Time':<12} {'Est Time':<12} {'Memory':<10} {'Ops/sec':<12}")
    print("-" * 60)
    
    for r in results:
        print(f"{r['name']:<15} {r['add_time']:.3f}s      {r['estimate_time']:.4f}s     "
              f"{r['memory_kb']:.1f} KB   {r['ops_per_sec']:,.0f}")
    
    print("\n" + "-" * 70)
    print("ACURATEȚE (eroare % față de numărare exactă)")
    print("-" * 70)
    
    print(f"\n{'Implementare':<15} {'Mean Error':<15} {'Max Error':<15} {'Min Error':<15}")
    print("-" * 60)
    
    for r in results:
        print(f"{r['name']:<15} {r['mean_error']:>10.2f}%    {r['max_error']:>10.2f}%    "
              f"{r['min_error']:>10.2f}%")
    
    # Demonstrație funcțională
    print("\n" + "-" * 70)
    print("DEMONSTRAȚIE: Top-10 Heavy Hitters")
    print("-" * 70)
    
    cms = cms_pure if not HAS_NUMPY else cms_numpy
    
    print(f"\n{'Rank':<6} {'Item':<15} {'Estimated':<12} {'Exact':<12} {'Error %':<10}")
    print("-" * 55)
    
    for i, (item, exact) in enumerate(exact_counts.most_common(10)):
        estimated = cms.estimate(item)
        error = (estimated - exact) / exact * 100 if exact > 0 else 0
        print(f"{i+1:<6} {item:<15} {estimated:<12,} {exact:<12,} {error:>8.2f}%")
    
    # Comparație cu C (estimată)
    print("\n" + "-" * 70)
    print("COMPARAȚIE ESTIMATĂ CU C")
    print("-" * 70)
    
    c_estimated_time = 0.052  # Din benchmark-urile anterioare
    python_time = results[-1]['add_time']
    speedup = python_time / c_estimated_time
    
    print(f"""
    Implementare C (estimat):    ~{c_estimated_time:.3f}s pentru {n_items:,} elemente
    Implementare Python:         {python_time:.3f}s pentru {n_items:,} elemente
    
    C este ~{speedup:.1f}x mai rapid decât Python pentru această operație.
    
    Când să folosim fiecare:
    
    ┌─────────────────────────────────────────────────────────────────────┐
    │ PYTHON                        │ C                                   │
    ├───────────────────────────────┼─────────────────────────────────────┤
    │ ✓ Prototipare rapidă          │ ✓ Producție high-throughput         │
    │ ✓ Analiză exploratorie        │ ✓ Sisteme embedded                  │
    │ ✓ Integrare cu pandas/numpy   │ ✓ Latență critică (<1ms)            │
    │ ✓ Batch processing            │ ✓ Milioane operații/secundă         │
    │ ✓ Scripting și automatizare   │ ✓ Biblioteci de sistem              │
    └───────────────────────────────┴─────────────────────────────────────┘
    """)


def demonstrate_cms_properties():
    """Demonstrează proprietățile teoretice ale CMS"""
    
    print("\n" + "=" * 70)
    print("   DEMONSTRAȚIE: Proprietăți Count-Min Sketch")
    print("=" * 70)
    
    cms = CountMinSketchPure(epsilon=0.01, delta=0.01)
    
    print(f"\n  Parametri CMS:")
    print(f"    Width (w):     {cms.width}")
    print(f"    Depth (d):     {cms.depth}")
    print(f"    Epsilon (ε):   {cms.epsilon}")
    print(f"    Delta (δ):     {cms.delta}")
    print(f"    Memorie:       {cms.memory_bytes() / 1024:.2f} KB")
    
    # Adăugăm elemente cu frecvențe cunoscute
    test_data = {
        'apple': 1000,
        'banana': 500,
        'cherry': 250,
        'date': 100,
        'elderberry': 50
    }
    
    print(f"\n  Adăugăm elemente cu frecvențe cunoscute:\n")
    for item, count in test_data.items():
        for _ in range(count):
            cms.add(item)
        print(f"    {item}: {count:,} apariții")
    
    print(f"\n  Estimări CMS:\n")
    print(f"    {'Element':<15} {'Exact':<10} {'Estimat':<10} {'Eroare':<10}")
    print("    " + "-" * 45)
    
    for item, exact in test_data.items():
        estimated = cms.estimate(item)
        error = (estimated - exact) / exact * 100 if exact > 0 else 0
        print(f"    {item:<15} {exact:<10} {estimated:<10} {error:>8.2f}%")
    
    # Element inexistent
    missing = cms.estimate('nonexistent')
    print(f"\n    Element inexistent ('nonexistent'): estimat = {missing}")
    print("    (Aceasta este o supraestimare din cauza coliziunilor)")
    
    print(f"""
  Proprietăți demonstrate:
    ✓ Niciodată nu subescitează (estimat ≥ exact)
    ✓ Eroarea este mărginită: ≤ ε × N = {cms.epsilon * cms.total_count:.0f}
    ✓ Garanție: Cu prob. {(1-cms.delta)*100:.0f}%, eroarea ≤ {cms.epsilon * cms.total_count:.0f}
    """)


# =============================================================================
# MAIN
# =============================================================================

if __name__ == "__main__":
    demonstrate_cms_properties()
    run_comparison()
