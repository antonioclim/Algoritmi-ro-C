#!/usr/bin/env python3
# ==============================================================================
# bloom_comparison.py - Comparație implementare Bloom Filter
# ==============================================================================
# Săptămâna 17: Structuri de Date Probabilistice pentru Big Data
# Algoritmi și Tehnici de Programare - ASE București CSIE
# ==============================================================================
# Acest script compară:
# - Implementarea noastră în C (prin output-uri)
# - Implementarea Python nativă
# - Biblioteca pybloom_live (dacă este disponibilă)
# ==============================================================================

import sys
import time
import random
import string
import hashlib
from typing import List, Optional
import math

# ==============================================================================
# IMPLEMENTARE BLOOM FILTER ÎN PYTHON PUR
# ==============================================================================

class BloomFilter:
    """
    Implementare educațională Bloom Filter în Python.
    
    Burton Howard Bloom (1970): "Space/Time Trade-offs in Hash Coding
    with Allowable Errors"
    
    Proprietăți:
    - Fără fals negative (dacă spune "nu există", sigur nu există)
    - Posibile fals pozitive (poate spune "există" când nu există)
    - Nu suportă ștergeri (ar crea fals negative)
    """
    
    def __init__(self, expected_elements: int, false_positive_rate: float = 0.01):
        """
        Inițializare cu parametri calculați optimal.
        
        Args:
            expected_elements (n): Numărul așteptat de elemente
            false_positive_rate (p): Rata de fals pozitive țintă
        
        Formule:
            m = -n * ln(p) / (ln(2))²  (număr de biți)
            k = (m/n) * ln(2)           (număr de funcții hash)
        """
        self.n = expected_elements
        self.p = false_positive_rate
        
        # Calculare dimensiune optimă
        self.m = int(math.ceil(-expected_elements * math.log(false_positive_rate) 
                               / (math.log(2) ** 2)))
        
        # Calculare număr optim de funcții hash
        self.k = int(math.ceil((self.m / expected_elements) * math.log(2)))
        
        # Inițializare bitmap (folosim bytearray pentru eficiență)
        self.bitmap = bytearray((self.m + 7) // 8)
        
        # Statistici
        self.elements_added = 0
        self.queries = 0
        self.positives = 0
    
    def _hash(self, item: str, seed: int) -> int:
        """
        Funcție hash cu seed pentru generarea familiei de hash-uri.
        Folosim double hashing: h(i) = h1 + i * h2
        """
        # Hash 1: MD5 bazat
        h1 = int(hashlib.md5(item.encode()).hexdigest(), 16)
        # Hash 2: SHA1 bazat
        h2 = int(hashlib.sha1(item.encode()).hexdigest(), 16)
        
        return (h1 + seed * h2) % self.m
    
    def _set_bit(self, index: int) -> None:
        """Setează bitul la poziția index."""
        byte_index = index // 8
        bit_index = index % 8
        self.bitmap[byte_index] |= (1 << bit_index)
    
    def _get_bit(self, index: int) -> bool:
        """Verifică bitul la poziția index."""
        byte_index = index // 8
        bit_index = index % 8
        return bool(self.bitmap[byte_index] & (1 << bit_index))
    
    def add(self, item: str) -> None:
        """Adaugă un element în Bloom Filter."""
        for i in range(self.k):
            index = self._hash(item, i)
            self._set_bit(index)
        self.elements_added += 1
    
    def contains(self, item: str) -> bool:
        """
        Verifică dacă un element ar putea fi în set.
        
        Returns:
            True: Element posibil prezent (poate fi fals pozitiv)
            False: Element sigur absent (niciodată fals negativ)
        """
        self.queries += 1
        for i in range(self.k):
            index = self._hash(item, i)
            if not self._get_bit(index):
                return False  # Sigur nu există
        self.positives += 1
        return True  # Probabil există (posibil fals pozitiv)
    
    def estimated_fpr(self) -> float:
        """
        Estimează rata actuală de fals pozitive.
        
        Formula: (1 - e^(-kn/m))^k
        """
        if self.m == 0:
            return 1.0
        exponent = -self.k * self.elements_added / self.m
        return math.pow(1 - math.exp(exponent), self.k)
    
    def fill_ratio(self) -> float:
        """Calculează raportul de biți setați."""
        set_bits = sum(bin(byte).count('1') for byte in self.bitmap)
        return set_bits / self.m
    
    def memory_usage(self) -> int:
        """Returnează memoria folosită în bytes."""
        return len(self.bitmap)
    
    def __repr__(self) -> str:
        return (f"BloomFilter(n={self.n}, m={self.m}, k={self.k}, "
                f"elements={self.elements_added}, "
                f"memory={self.memory_usage()} bytes)")


# ==============================================================================
# FUNCȚII DE TESTARE ȘI COMPARAȚIE
# ==============================================================================

def generate_random_strings(count: int, length: int = 10) -> List[str]:
    """Generează stringuri aleatorii pentru testare."""
    chars = string.ascii_letters + string.digits
    return [''.join(random.choice(chars) for _ in range(length)) 
            for _ in range(count)]


def test_bloom_filter_accuracy(bf: BloomFilter, 
                                present: List[str], 
                                absent: List[str]) -> dict:
    """
    Testează acuratețea Bloom Filter-ului.
    
    Args:
        bf: Bloom Filter-ul de testat
        present: Elemente care TREBUIE să fie găsite
        absent: Elemente care NU au fost adăugate
    
    Returns:
        Dicționar cu statistici de performanță
    """
    # Test elemente prezente
    true_positives = sum(1 for item in present if bf.contains(item))
    false_negatives = len(present) - true_positives
    
    # Test elemente absente
    false_positives = sum(1 for item in absent if bf.contains(item))
    true_negatives = len(absent) - false_positives
    
    return {
        'true_positives': true_positives,
        'false_negatives': false_negatives,
        'true_negatives': true_negatives,
        'false_positives': false_positives,
        'fpr_actual': false_positives / len(absent) if absent else 0,
        'fnr_actual': false_negatives / len(present) if present else 0,
        'accuracy': (true_positives + true_negatives) / 
                   (len(present) + len(absent)) if (present or absent) else 0
    }


def benchmark_bloom_filter(sizes: List[int], fpr: float = 0.01) -> None:
    """
    Benchmark pentru diferite dimensiuni.
    
    Măsoară:
    - Timp de inserare
    - Timp de căutare
    - Memorie utilizată
    - FPR actual vs țintă
    """
    print("\n" + "=" * 70)
    print("BENCHMARK BLOOM FILTER - PYTHON")
    print("=" * 70)
    print(f"{'Elemente':>12} | {'Memorie':>10} | {'Insert/s':>12} | "
          f"{'Query/s':>12} | {'FPR țintă':>10} | {'FPR actual':>10}")
    print("-" * 70)
    
    for n in sizes:
        bf = BloomFilter(n, fpr)
        
        # Generare date
        items = generate_random_strings(n)
        test_items = generate_random_strings(n // 10)
        
        # Benchmark inserare
        start = time.perf_counter()
        for item in items:
            bf.add(item)
        insert_time = time.perf_counter() - start
        
        # Benchmark căutare
        start = time.perf_counter()
        for item in test_items:
            bf.contains(item)
        query_time = time.perf_counter() - start
        
        # Statistici
        memory_kb = bf.memory_usage() / 1024
        insert_rate = n / insert_time if insert_time > 0 else 0
        query_rate = len(test_items) / query_time if query_time > 0 else 0
        fpr_actual = bf.estimated_fpr()
        
        print(f"{n:>12,} | {memory_kb:>8.1f}KB | {insert_rate:>12,.0f} | "
              f"{query_rate:>12,.0f} | {fpr:>10.2%} | {fpr_actual:>10.4%}")


def compare_with_set(n: int = 10000) -> None:
    """
    Compară Bloom Filter cu set() Python nativ.
    
    Evidențiază trade-off-ul: memorie vs acuratețe.
    """
    print("\n" + "=" * 70)
    print("COMPARAȚIE: BLOOM FILTER vs SET PYTHON")
    print("=" * 70)
    
    items = generate_random_strings(n)
    test_present = random.sample(items, n // 10)
    test_absent = generate_random_strings(n // 10)
    
    # Python set
    start = time.perf_counter()
    py_set = set(items)
    set_insert_time = time.perf_counter() - start
    
    set_memory = sys.getsizeof(py_set)
    for item in py_set:
        set_memory += sys.getsizeof(item)
    
    start = time.perf_counter()
    set_results = [item in py_set for item in test_present + test_absent]
    set_query_time = time.perf_counter() - start
    
    # Bloom Filter
    bf = BloomFilter(n, 0.01)
    
    start = time.perf_counter()
    for item in items:
        bf.add(item)
    bf_insert_time = time.perf_counter() - start
    
    start = time.perf_counter()
    bf_results = [bf.contains(item) for item in test_present + test_absent]
    bf_query_time = time.perf_counter() - start
    
    # Acuratețe Bloom Filter
    stats = test_bloom_filter_accuracy(bf, test_present, test_absent)
    
    # Afișare rezultate
    print(f"\nNumăr elemente: {n:,}")
    print(f"\n{'Metric':<25} | {'Python set':>15} | {'Bloom Filter':>15} | {'Raport':>10}")
    print("-" * 70)
    print(f"{'Memorie utilizată':<25} | {set_memory:>12,} B | "
          f"{bf.memory_usage():>12,} B | {set_memory / bf.memory_usage():>9.1f}x")
    print(f"{'Timp inserare (s)':<25} | {set_insert_time:>15.4f} | "
          f"{bf_insert_time:>15.4f} | {set_insert_time / bf_insert_time:>9.2f}x")
    print(f"{'Timp query (s)':<25} | {set_query_time:>15.4f} | "
          f"{bf_query_time:>15.4f} | {set_query_time / bf_query_time:>9.2f}x")
    print(f"{'Fals negative':<25} | {'0':>15} | {stats['false_negatives']:>15} | {'N/A':>10}")
    print(f"{'Fals pozitive':<25} | {'0':>15} | {stats['false_positives']:>15} | {'N/A':>10}")
    print(f"{'FPR actual':<25} | {'0%':>15} | {stats['fpr_actual']:>14.2%} | {'N/A':>10}")
    
    print(f"\n{'':=<70}")
    print(f"CONCLUZIE: Bloom Filter folosește {set_memory / bf.memory_usage():.1f}x "
          f"mai puțină memorie")
    print(f"           cu prețul a {stats['fpr_actual']:.2%} fals pozitive.")
    print("=" * 70)


def demo_use_cases() -> None:
    """
    Demonstrație cazuri reale de utilizare.
    """
    print("\n" + "=" * 70)
    print("DEMONSTRAȚIE CAZURI DE UTILIZARE")
    print("=" * 70)
    
    # Caz 1: Deduplicare URL-uri
    print("\n[1] DEDUPLICARE URL-URI (Web Crawler)")
    print("-" * 50)
    
    # Simulăm un crawler cu 100K URL-uri văzute
    seen_urls = BloomFilter(100000, 0.001)  # FPR 0.1%
    
    sample_urls = [
        "https://example.com/page1",
        "https://example.com/page2",
        "https://test.org/article",
        "https://demo.io/data",
    ]
    
    for url in sample_urls:
        if not seen_urls.contains(url):
            print(f"  Nou: {url}")
            seen_urls.add(url)
        else:
            print(f"  Duplicat: {url}")
    
    # Verificăm din nou (ar trebui să fie toate "văzute")
    print("\n  Verificare duplicări:")
    for url in sample_urls:
        status = "SKIP" if seen_urls.contains(url) else "CRAWL"
        print(f"    {url}: {status}")
    
    print(f"\n  Memorie folosită: {seen_urls.memory_usage():,} bytes")
    print(f"  (Pentru 100K URL-uri cu FPR 0.1%)")
    
    # Caz 2: Cache check înainte de query DB
    print("\n[2] CACHE CHECK ÎNAINTE DE QUERY DATABASE")
    print("-" * 50)
    
    # Bloom filter pentru chei inexistente
    negative_cache = BloomFilter(50000, 0.01)
    
    # Simulăm că am învățat că anumite chei nu există
    nonexistent_keys = ["user_999999", "product_ABC", "order_XYZ123"]
    for key in nonexistent_keys:
        negative_cache.add(key)
    
    def check_database(key: str) -> str:
        if negative_cache.contains(key):
            return "SKIP DB (known missing)"
        return "QUERY DB"
    
    test_keys = ["user_123", "user_999999", "product_ABC", "order_100"]
    for key in test_keys:
        result = check_database(key)
        print(f"  {key}: {result}")
    
    # Caz 3: Spell checker
    print("\n[3] SPELL CHECKER (Dicționar rapid)")
    print("-" * 50)
    
    dictionary = BloomFilter(50000, 0.0001)  # FPR foarte mic
    words = ["algorithm", "programming", "computer", "science", "data", 
             "structure", "hash", "function", "bloom", "filter"]
    
    for word in words:
        dictionary.add(word)
    
    test_words = ["algorithm", "algoritm", "data", "dta", "bloom", "blom"]
    for word in test_words:
        if dictionary.contains(word):
            print(f"  '{word}': probabil corect ✓")
        else:
            print(f"  '{word}': sigur greșit ✗")


# ==============================================================================
# MAIN
# ==============================================================================

def main():
    """Funcție principală."""
    print("╔" + "═" * 68 + "╗")
    print("║  BLOOM FILTER - COMPARAȚIE PYTHON                                  ║")
    print("║  Săptămâna 17: Structuri de Date Probabilistice                    ║")
    print("║  ATP - ASE București CSIE                                          ║")
    print("╚" + "═" * 68 + "╝")
    
    # Test de bază
    print("\n[TEST BASIC]")
    bf = BloomFilter(1000, 0.01)
    print(f"Inițializat: {bf}")
    
    test_items = ["hello", "world", "algorithm", "data", "structure"]
    for item in test_items:
        bf.add(item)
    print(f"După adăugare: {bf}")
    
    # Verificare
    print("\nVerificare apartenență:")
    for item in test_items:
        result = "FOUND" if bf.contains(item) else "NOT FOUND"
        print(f"  '{item}': {result}")
    
    missing = ["missing", "absent", "none"]
    for item in missing:
        result = "FALSE POSITIVE!" if bf.contains(item) else "CORRECT: NOT FOUND"
        print(f"  '{item}': {result}")
    
    # Benchmark
    benchmark_bloom_filter([1000, 10000, 100000])
    
    # Comparație cu set
    compare_with_set(50000)
    
    # Demo cazuri de utilizare
    demo_use_cases()
    
    print("\n[DONE] Comparație completă!")
    return 0


if __name__ == "__main__":
    sys.exit(main())
