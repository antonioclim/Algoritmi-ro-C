#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
================================================================================
HYPERLOGLOG COMPARISON: PYTHON vs C IMPLEMENTATIONS
================================================================================
Săptămâna 17 - Structuri de Date Probabilistice pentru Big Data
Academia de Studii Economice - CSIE
Curs: Algoritmi și Tehnici de Programare

Acest modul oferă:
1. Implementări HyperLogLog în Python (pur și NumPy)
2. Comparații de performanță și acuratețe
3. Demonstrații ale proprietăților teoretice
4. Benchmark-uri pentru diferite cardinalități

FUNDAMENTELE TEORETICE HYPERLOGLOG
================================================================================

HyperLogLog (HLL) este un algoritm probabilistic pentru estimarea cardinalității
(numărului de elemente distincte) într-un multiset, folosind memorie sublineară.

INTUIȚIA ALGORITMULUI:
----------------------
Ideea centrală: dacă hash-urile sunt uniform distribuite, poziția primului bit
de 1 (leading zeros + 1) are distribuția geometrică cu parametrul 1/2.

Pentru n elemente distincte:
- Maximul leading zeros observat ≈ log₂(n)
- Dar acest estimator are varianță mare!

ÎMBUNĂTĂȚIREA PRIN STOCHASTIC AVERAGING:
----------------------------------------
HyperLogLog rezolvă problema varianței prin:
1. Împarte elementele în M = 2^p bucket-uri (folosind primii p biți)
2. Menține maximul leading zeros pentru fiecare bucket
3. Folosește media armonică pentru combinare

FORMULA DE ESTIMARE:
-------------------
            α_m × M²
    E = ─────────────────
         Σ(j=0 to M-1) 2^(-M[j])

Unde:
- α_m = factor de corecție pentru bias (≈ 0.7213 pentru M ≥ 128)
- M[j] = valoarea maximă din bucket j
- Media armonică oferă robustețe la outlieri

CORECȚII:
---------
1. Small range correction (E < 5/2 × M):
   - Dacă există bucket-uri goale (V > 0): E* = M × ln(M/V)
   - Linear counting pentru cardinalități mici

2. Large range correction (E > 2³² / 30):
   - E* = -2³² × ln(1 - E/2³²)
   - Evită overflow pentru cardinalități foarte mari

PRECIZIE:
---------
Eroarea standard: σ ≈ 1.04 / √M
- p=4  (M=16):   σ ≈ 26%
- p=10 (M=1024): σ ≈ 3.25%
- p=14 (M=16384): σ ≈ 0.81%
- p=16 (M=65536): σ ≈ 0.41%

MEMORIE:
--------
Fiecare bucket folosește 5-6 biți (maxim leading zeros ≈ 50 pentru 64-bit hash)
Total: M × 6 biți = 3×2^(p-1) bytes

Exemplu p=14: 16384 × 6 bits = 12KB pentru orice cardinalitate!

================================================================================
"""

import math
import time
import random
import hashlib
import struct
from collections import Counter
from typing import List, Tuple, Optional

# =============================================================================
# CONSTANTE ȘI CONFIGURAȚIE
# =============================================================================

# Factorii de corecție alpha_m pentru diferite valori de m
# Derivați din analiza lui Flajolet et al.
ALPHA_CORRECTIONS = {
    16: 0.673,
    32: 0.697,
    64: 0.709,
}

def get_alpha(m: int) -> float:
    """
    Calculează factorul de corecție alpha pentru m bucket-uri.
    
    Pentru m ≥ 128, folosim formula asimptotică:
        α = 0.7213 / (1 + 1.079/m)
    
    Pentru m < 128, folosim valori tabelate.
    
    Args:
        m: Numărul de bucket-uri (trebuie să fie putere a lui 2)
    
    Returns:
        Factorul de corecție alpha
    """
    if m in ALPHA_CORRECTIONS:
        return ALPHA_CORRECTIONS[m]
    # Formula asimptotică pentru m >= 128
    return 0.7213 / (1 + 1.079 / m)


# =============================================================================
# FUNCȚII HASH
# =============================================================================

def hash_fnv1a_64(data: bytes) -> int:
    """
    FNV-1a hash pe 64 de biți.
    
    FNV (Fowler-Noll-Vo) este un hash non-criptografic rapid,
    potrivit pentru hash tables și fingerprinting.
    
    Proprietăți:
    - Distribuție excelentă
    - Avalanche effect bun
    - Foarte rapid
    """
    FNV_OFFSET_BASIS = 0xcbf29ce484222325
    FNV_PRIME = 0x100000001b3
    
    h = FNV_OFFSET_BASIS
    for byte in data:
        h ^= byte
        h = (h * FNV_PRIME) & 0xFFFFFFFFFFFFFFFF
    return h


def hash_murmur_style(data: bytes, seed: int = 0) -> int:
    """
    Hash inspirat de MurmurHash pentru 64 biți.
    
    Simplifcat pentru claritate pedagogică.
    """
    h = seed
    for i, byte in enumerate(data):
        h ^= byte * (i + 1) * 0x5bd1e995
        h = ((h << 13) | (h >> 51)) & 0xFFFFFFFFFFFFFFFF
        h *= 0x5bd1e995
        h &= 0xFFFFFFFFFFFFFFFF
    
    # Finalizare
    h ^= h >> 33
    h *= 0xff51afd7ed558ccd
    h &= 0xFFFFFFFFFFFFFFFF
    h ^= h >> 33
    h *= 0xc4ceb9fe1a85ec53
    h &= 0xFFFFFFFFFFFFFFFF
    h ^= h >> 33
    
    return h


def leading_zeros_64(value: int) -> int:
    """
    Numără biții de 0 de la stânga (leading zeros) pentru un număr pe 64 biți.
    
    Aceasta este operația fundamentală în HyperLogLog:
    - Dacă hash-ul este uniform distribuit
    - Probabilitatea de a avea k leading zeros este 2^(-(k+1))
    - Deci maximul leading zeros ≈ log₂(n) pentru n elemente
    
    Exemplu:
        0b0001... → 3 leading zeros
        0b1...    → 0 leading zeros
    """
    if value == 0:
        return 64
    
    # Metodă simplă și corectă: bit_length()
    # Pentru un număr pe 64 biți, leading zeros = 64 - bit_length
    return 64 - value.bit_length()


def count_trailing_zeros(value: int, max_bits: int = 64) -> int:
    """
    Numără biții de 0 de la dreapta (trailing zeros).
    
    Alternativă folosită în unele implementări HLL.
    """
    if value == 0:
        return max_bits
    
    count = 0
    while (value & 1) == 0 and count < max_bits:
        value >>= 1
        count += 1
    return count


def rho(value: int, max_bits: int = 64) -> int:
    """
    Calculează ρ(w) = poziția primului bit de 1 de la dreapta.
    
    Aceasta este funcția standard din paper-ul original HyperLogLog.
    ρ(0...01xxx) = numărul de 0-uri + 1
    
    Returnează valoarea între 1 și max_bits+1.
    """
    if value == 0:
        return max_bits + 1
    
    position = 1
    while (value & 1) == 0:
        value >>= 1
        position += 1
    return position


# =============================================================================
# IMPLEMENTARE HYPERLOGLOG - PYTHON PUR
# =============================================================================

class HyperLogLogPure:
    """
    Implementare HyperLogLog în Python pur.
    
    Această implementare este optimizată pentru claritate pedagogică,
    nu pentru performanță. Fiecare metodă include comentarii detaliate
    despre ce face și de ce.
    
    Complexități:
    - add(): O(1) timp, O(1) spațiu
    - count(): O(m) timp, O(1) spațiu
    - merge(): O(m) timp, O(m) spațiu
    - Spațiu total: O(m) = O(2^p)
    
    Attributes:
        p: Precizia (numărul de biți pentru bucket index)
        m: Numărul de bucket-uri (2^p)
        registers: Lista de registre (maximi per bucket)
        alpha: Factorul de corecție pentru estimare
    """
    
    def __init__(self, precision: int = 14):
        """
        Inițializează HyperLogLog cu precizia specificată.
        
        Args:
            precision: Numărul de biți pentru indexarea bucket-urilor.
                      Valori tipice: 10-16
                      Mai mare = mai precis dar mai multă memorie
                      
        Exemplu:
            precision=14 → 16384 bucket-uri → ~12KB memorie
            Eroare standard: 1.04/√16384 ≈ 0.81%
        """
        if not 4 <= precision <= 18:
            raise ValueError("Precision must be between 4 and 18")
        
        self.p = precision
        self.m = 1 << precision  # 2^p bucket-uri
        self.registers = [0] * self.m  # Inițializare cu 0
        self.alpha = get_alpha(self.m)
        
        # Statistici pentru debugging
        self._elements_added = 0
    
    def add(self, item) -> None:
        """
        Adaugă un element în HyperLogLog.
        
        Procesul:
        1. Hash elementul pe 64 biți
        2. Extrage ultimii p biți ca index de bucket
        3. Calculează ρ(w) pentru biții rămași (w = h >> p)
        4. Actualizează registrul dacă valoarea e mai mare
        
        Args:
            item: Orice obiect hashabil (string, int, bytes, etc.)
        
        Notă: Operația este idempotentă - adăugarea aceluiași element
              de mai multe ori nu modifică estimarea.
        """
        # Convertim la bytes pentru hash
        if isinstance(item, str):
            data = item.encode('utf-8')
        elif isinstance(item, int):
            data = str(item).encode('utf-8')
        elif isinstance(item, bytes):
            data = item
        else:
            data = str(item).encode('utf-8')
        
        # Calculăm hash-ul pe 64 biți
        h = hash_fnv1a_64(data)
        
        # Ultimii p biți = index bucket
        # Mască pentru a extrage ultimii p biți
        bucket_index = h & ((1 << self.p) - 1)
        
        # Restul biților (h >> p) pentru funcția ρ
        # ρ = poziția primului bit de 1 (de la dreapta spre stânga)
        w = h >> self.p
        
        # Calculăm ρ(w): poziția primului 1, sau max dacă w=0
        # Folosim 64-p biți rămași
        max_rho = 64 - self.p + 1
        if w == 0:
            r = max_rho
        else:
            r = 1
            while (w & 1) == 0 and r < max_rho:
                w >>= 1
                r += 1
        
        # Actualizăm registrul cu maximul
        if r > self.registers[bucket_index]:
            self.registers[bucket_index] = r
        
        self._elements_added += 1
    
    def count(self) -> float:
        """
        Estimează cardinalitatea (numărul de elemente distincte).
        
        Procesul:
        1. Calculează media armonică a valorilor 2^(-register[j])
        2. Aplică factorul de corecție alpha
        3. Aplică corecții pentru cardinalități mici/mari
        
        Returns:
            Estimarea cardinalității
            
        Nota despre precizie:
        - Eroarea standard este aproximativ 1.04/√m
        - Pentru p=14: eroare ≈ 0.81%
        """
        # Calculăm suma pentru media armonică
        # Σ 2^(-M[j]) pentru j = 0 to m-1
        harmonic_sum = 0.0
        empty_registers = 0
        
        for register_value in self.registers:
            harmonic_sum += 2.0 ** (-register_value)
            if register_value == 0:
                empty_registers += 1
        
        # Estimare raw: E = α × m² / Σ(2^(-M[j]))
        raw_estimate = self.alpha * (self.m ** 2) / harmonic_sum
        
        # Corecție pentru cardinalități mici (Linear Counting)
        # Când E < 5/2 × m și există registre goale
        if raw_estimate <= 2.5 * self.m:
            if empty_registers > 0:
                # Linear counting: m × ln(m/V)
                # V = numărul de registre goale
                corrected = self.m * math.log(self.m / empty_registers)
                return corrected
        
        # Corecție pentru cardinalități foarte mari
        # Evită overflow când E se apropie de 2^32
        TWO_POW_32 = 2 ** 32
        if raw_estimate > TWO_POW_32 / 30:
            corrected = -TWO_POW_32 * math.log(1 - raw_estimate / TWO_POW_32)
            return corrected
        
        return raw_estimate
    
    def merge(self, other: 'HyperLogLogPure') -> 'HyperLogLogPure':
        """
        Fuzionează două structuri HyperLogLog.
        
        Aceasta este o proprietate extraordinară a HLL:
        - Putem combina estimări din surse diferite
        - Rezultatul este echivalent cu procesarea tuturor elementelor
        - Operația este comutativă și asociativă
        
        Aplicații:
        - Agregare distribuită (MapReduce)
        - Combinarea statisticilor din mai multe servere
        - Analiza datelor din diferite perioade de timp
        
        Args:
            other: Alt HyperLogLog cu aceeași precizie
            
        Returns:
            Nou HyperLogLog reprezentând uniunea
        """
        if self.p != other.p:
            raise ValueError("Cannot merge HLLs with different precision")
        
        result = HyperLogLogPure(self.p)
        
        # Fuziunea = maximul element cu element
        for i in range(self.m):
            result.registers[i] = max(self.registers[i], other.registers[i])
        
        result._elements_added = self._elements_added + other._elements_added
        return result
    
    def memory_bytes(self) -> int:
        """
        Estimează memoria utilizată în bytes.
        
        În implementarea Python, fiecare registru este un int Python
        care folosește mult mai multă memorie decât ar fi necesar.
        
        Teoretic: m × 6 biți (suficient pentru valori 0-64)
        Python: m × sizeof(PyObject) ≈ m × 28 bytes per int mic
        """
        # Aproximare pentru Python
        # În practică, lista de int-uri mici e optimizată
        import sys
        return sys.getsizeof(self.registers) + self.m * 4  # Aproximare
    
    def relative_error(self) -> float:
        """
        Returnează eroarea standard relativă teoretică.
        """
        return 1.04 / math.sqrt(self.m)
    
    def __repr__(self) -> str:
        return (f"HyperLogLogPure(p={self.p}, m={self.m}, "
                f"estimate={self.count():.0f}, "
                f"error={self.relative_error()*100:.2f}%)")


# =============================================================================
# IMPLEMENTARE HYPERLOGLOG - NUMPY
# =============================================================================

try:
    import numpy as np
    NUMPY_AVAILABLE = True
except ImportError:
    NUMPY_AVAILABLE = False
    np = None


class HyperLogLogNumPy:
    """
    Implementare HyperLogLog optimizată cu NumPy.
    
    Avantaje față de implementarea pură:
    - Operații vectorizate pentru procesare în bulk
    - Memorie mai eficientă (dtype=np.uint8)
    - Calcule mai rapide pentru count()
    
    Această implementare demonstrează cum bibliotecile
    numerice pot accelera semnificativ algoritmii.
    """
    
    def __init__(self, precision: int = 14):
        """
        Inițializează HyperLogLog cu NumPy.
        
        Args:
            precision: Precizia (4-18)
        """
        if not NUMPY_AVAILABLE:
            raise ImportError("NumPy is required for HyperLogLogNumPy")
        
        if not 4 <= precision <= 18:
            raise ValueError("Precision must be between 4 and 18")
        
        self.p = precision
        self.m = 1 << precision
        # uint8 e suficient: valorile sunt 0-64
        self.registers = np.zeros(self.m, dtype=np.uint8)
        self.alpha = get_alpha(self.m)
        self._elements_added = 0
    
    def add(self, item) -> None:
        """Adaugă un element (algoritm identic cu versiunea pură)."""
        if isinstance(item, str):
            data = item.encode('utf-8')
        elif isinstance(item, int):
            data = str(item).encode('utf-8')
        elif isinstance(item, bytes):
            data = item
        else:
            data = str(item).encode('utf-8')
        
        h = hash_fnv1a_64(data)
        
        # Ultimii p biți = index bucket
        bucket_index = h & ((1 << self.p) - 1)
        
        # Restul biților pentru ρ
        w = h >> self.p
        max_rho = 64 - self.p + 1
        
        if w == 0:
            r = max_rho
        else:
            r = 1
            while (w & 1) == 0 and r < max_rho:
                w >>= 1
                r += 1
        
        # NumPy permite assignment direct cu comparație implicită
        if r > self.registers[bucket_index]:
            self.registers[bucket_index] = r
        
        self._elements_added += 1
    
    def add_batch(self, items: List) -> None:
        """
        Adaugă multiple elemente eficient.
        
        Deși procesarea individuală rămâne necesară pentru hash,
        această metodă reduce overhead-ul Python.
        
        Args:
            items: Lista de elemente de adăugat
        """
        for item in items:
            self.add(item)
    
    def count(self) -> float:
        """
        Estimează cardinalitatea folosind operații NumPy vectorizate.
        
        Calculul 2^(-register) pentru toate registrele simultan
        este semnificativ mai rapid decât iterația Python.
        """
        # Vectorizat: calculăm 2^(-r) pentru toate registrele
        powers = np.power(2.0, -self.registers.astype(np.float64))
        harmonic_sum = np.sum(powers)
        
        raw_estimate = self.alpha * (self.m ** 2) / harmonic_sum
        
        # Corecție small range
        if raw_estimate <= 2.5 * self.m:
            empty_count = np.sum(self.registers == 0)
            if empty_count > 0:
                return self.m * math.log(self.m / empty_count)
        
        # Corecție large range
        TWO_POW_32 = 2 ** 32
        if raw_estimate > TWO_POW_32 / 30:
            return -TWO_POW_32 * math.log(1 - raw_estimate / TWO_POW_32)
        
        return raw_estimate
    
    def merge(self, other: 'HyperLogLogNumPy') -> 'HyperLogLogNumPy':
        """Fuzionează cu alt HLL folosind np.maximum."""
        if self.p != other.p:
            raise ValueError("Cannot merge HLLs with different precision")
        
        result = HyperLogLogNumPy(self.p)
        result.registers = np.maximum(self.registers, other.registers)
        result._elements_added = self._elements_added + other._elements_added
        return result
    
    def memory_bytes(self) -> int:
        """Memorie reală folosită de array-ul NumPy."""
        return self.registers.nbytes
    
    def relative_error(self) -> float:
        return 1.04 / math.sqrt(self.m)
    
    def __repr__(self) -> str:
        return (f"HyperLogLogNumPy(p={self.p}, m={self.m}, "
                f"estimate={self.count():.0f}, "
                f"error={self.relative_error()*100:.2f}%)")


# =============================================================================
# HYPERLOGLOG++ (VARIANTA ÎMBUNĂTĂȚITĂ)
# =============================================================================

class HyperLogLogPlusPlus:
    """
    Implementare HyperLogLog++ cu îmbunătățiri Google.
    
    HyperLogLog++ adaugă:
    1. Bias correction mai precisă pentru cardinalități mici
    2. Sparse representation pentru eficiență la cardinalități foarte mici
    3. Precizie îmbunătățită în general
    
    Această implementare simplificată include doar bias correction.
    
    Referință: Heule, Nunkesser, Hall (2013) - "HyperLogLog in Practice"
    """
    
    # Tabel de corecție empirică pentru p=14 (simplificat)
    # Valorile reale au sute de intrări
    BIAS_DATA_P14 = [
        (0, 0), (10, 11.717), (20, 23.131), (30, 35.133),
        (40, 46.724), (50, 58.216), (100, 118.417),
        (200, 236.731), (500, 584.169), (1000, 1164.834),
        (2000, 2317.963), (5000, 5765.204), (10000, 11447.503)
    ]
    
    def __init__(self, precision: int = 14):
        if not NUMPY_AVAILABLE:
            raise ImportError("NumPy required for HyperLogLog++")
        
        self.p = precision
        self.m = 1 << precision
        self.registers = np.zeros(self.m, dtype=np.uint8)
        self.alpha = get_alpha(self.m)
    
    def add(self, item) -> None:
        """Adaugă element (algoritm identic cu HLL standard)."""
        if isinstance(item, str):
            data = item.encode('utf-8')
        elif isinstance(item, int):
            data = str(item).encode('utf-8')
        else:
            data = str(item).encode('utf-8')
        
        h = hash_fnv1a_64(data)
        
        # Ultimii p biți = index bucket
        bucket_index = h & ((1 << self.p) - 1)
        
        # Restul biților pentru ρ
        w = h >> self.p
        max_rho = 64 - self.p + 1
        
        if w == 0:
            r = max_rho
        else:
            r = 1
            while (w & 1) == 0 and r < max_rho:
                w >>= 1
                r += 1
        
        if r > self.registers[bucket_index]:
            self.registers[bucket_index] = r
    
    def _estimate_bias(self, raw_estimate: float) -> float:
        """
        Estimează bias-ul pentru corecție.
        
        În implementarea completă, aceasta folosește interpolarea
        pe tabele empirice derivate din experimente.
        """
        if self.p != 14:
            return 0  # Bias data doar pentru p=14
        
        # Interpolație liniară simplă
        for i, (est, bias) in enumerate(self.BIAS_DATA_P14[:-1]):
            next_est, next_bias = self.BIAS_DATA_P14[i + 1]
            if est <= raw_estimate < next_est:
                t = (raw_estimate - est) / (next_est - est)
                return bias + t * (next_bias - bias)
        
        return 0
    
    def count(self) -> float:
        """Estimare cu corecție de bias."""
        powers = np.power(2.0, -self.registers.astype(np.float64))
        harmonic_sum = np.sum(powers)
        raw_estimate = self.alpha * (self.m ** 2) / harmonic_sum
        
        # HLL++ folosește corecție empirică în loc de Linear Counting simplu
        if raw_estimate <= 5 * self.m:
            bias = self._estimate_bias(raw_estimate)
            raw_estimate -= bias
        
        # Linear counting pentru cardinalități foarte mici
        empty_count = np.sum(self.registers == 0)
        if empty_count > 0:
            linear_estimate = self.m * math.log(self.m / empty_count)
            # Threshold pentru când să folosim linear counting
            threshold = self._get_threshold()
            if linear_estimate <= threshold:
                return linear_estimate
        
        return raw_estimate
    
    def _get_threshold(self) -> float:
        """Threshold pentru comutarea la linear counting."""
        # Valorile depind de p, aceasta e o aproximare
        thresholds = {
            4: 10, 5: 20, 6: 40, 7: 80, 8: 220, 9: 400,
            10: 900, 11: 1800, 12: 3100, 13: 6500, 14: 11500,
            15: 20000, 16: 50000, 17: 120000, 18: 350000
        }
        return thresholds.get(self.p, 0)
    
    def memory_bytes(self) -> int:
        return self.registers.nbytes


# =============================================================================
# GENERATOARE DE DATE DE TEST
# =============================================================================

def generate_unique_strings(n: int, prefix: str = "item_") -> List[str]:
    """
    Generează n string-uri unice.
    
    Args:
        n: Numărul de string-uri
        prefix: Prefix pentru string-uri
        
    Returns:
        Lista de string-uri unice
    """
    return [f"{prefix}{i}" for i in range(n)]


def generate_with_duplicates(n_unique: int, total: int, 
                            zipf_param: float = 1.0) -> Tuple[List[str], int]:
    """
    Generează date cu distribuție Zipf (mulți duplicați).
    
    Distribuția Zipf modelează fenomene naturale:
    - Frecvența cuvintelor în text
    - Popularitatea site-urilor web
    - Distribuția veniturilor
    
    Args:
        n_unique: Numărul de elemente unice
        total: Numărul total de elemente (cu duplicate)
        zipf_param: Parametrul s al distribuției (mai mare = mai multe duplicate)
        
    Returns:
        (lista_elemente, cardinalitate_reală)
    """
    # Creăm elementele unice
    unique_items = [f"item_{i}" for i in range(n_unique)]
    
    # Generăm ponderile Zipf
    ranks = np.arange(1, n_unique + 1)
    weights = 1.0 / np.power(ranks, zipf_param)
    weights /= weights.sum()
    
    # Selectăm cu repetiție
    indices = np.random.choice(n_unique, size=total, p=weights)
    items = [unique_items[i] for i in indices]
    
    # Calculăm cardinalitatea reală
    actual_cardinality = len(set(items))
    
    return items, actual_cardinality


def generate_ip_addresses(n: int) -> List[str]:
    """
    Generează adrese IP pentru testare.
    
    Simulează trafic de rețea cu unele IP-uri mai frecvente.
    """
    ips = []
    for i in range(n):
        # Unele subnet-uri sunt mai populare (simulare realistă)
        if random.random() < 0.3:
            # Trafic concentrat
            a, b = 192, 168
            c = random.randint(0, 10)
        else:
            a = random.randint(1, 254)
            b = random.randint(0, 255)
            c = random.randint(0, 255)
        d = random.randint(0, 255)
        ips.append(f"{a}.{b}.{c}.{d}")
    return ips


# =============================================================================
# BENCHMARK ȘI COMPARAȚII
# =============================================================================

def benchmark_hll_add(hll, items: List, batch_size: int = 1000) -> dict:
    """
    Măsoară performanța operației add.
    
    Returns:
        Dict cu metrici de performanță
    """
    n = len(items)
    
    # Warmup
    for item in items[:min(100, n)]:
        hll.add(item)
    
    # Recreăm pentru benchmark curat
    hll_class = type(hll)
    hll_fresh = hll_class(hll.p)
    
    start_time = time.perf_counter()
    for item in items:
        hll_fresh.add(item)
    elapsed = time.perf_counter() - start_time
    
    return {
        'total_time_ms': elapsed * 1000,
        'ops_per_second': n / elapsed if elapsed > 0 else float('inf'),
        'time_per_op_us': (elapsed / n) * 1_000_000 if n > 0 else 0,
        'total_items': n
    }


def benchmark_hll_count(hll, iterations: int = 100) -> dict:
    """
    Măsoară performanța operației count.
    """
    # Warmup
    for _ in range(10):
        hll.count()
    
    start_time = time.perf_counter()
    for _ in range(iterations):
        result = hll.count()
    elapsed = time.perf_counter() - start_time
    
    return {
        'total_time_ms': elapsed * 1000,
        'avg_time_us': (elapsed / iterations) * 1_000_000,
        'iterations': iterations,
        'estimate': result
    }


def compare_implementations(n_items: int = 100000, 
                           precision: int = 14) -> dict:
    """
    Compară toate implementările HyperLogLog.
    
    Args:
        n_items: Numărul de elemente de testat
        precision: Precizia HLL
        
    Returns:
        Dict cu rezultatele comparative
    """
    print(f"\n{'='*70}")
    print(f"COMPARAȚIE IMPLEMENTĂRI HYPERLOGLOG")
    print(f"{'='*70}")
    print(f"Elemente: {n_items:,} | Precizie: p={precision} | Bucket-uri: {2**precision:,}")
    print(f"{'='*70}\n")
    
    # Generăm date de test
    print("Generare date de test...")
    items_unique = generate_unique_strings(n_items)
    items_zipf, actual_card = generate_with_duplicates(
        n_items // 10, n_items, zipf_param=1.2
    )
    
    results = {}
    
    # Test 1: Python Pur
    print("\n[1] HyperLogLog Python Pur")
    print("-" * 40)
    
    hll_pure = HyperLogLogPure(precision)
    add_bench_pure = benchmark_hll_add(hll_pure, items_unique[:n_items//10])
    
    # Adăugăm toate elementele
    hll_pure = HyperLogLogPure(precision)
    for item in items_unique:
        hll_pure.add(item)
    
    count_bench_pure = benchmark_hll_count(hll_pure)
    estimate_pure = hll_pure.count()
    error_pure = abs(estimate_pure - n_items) / n_items * 100
    
    results['python_pure'] = {
        'add_ops_per_sec': add_bench_pure['ops_per_second'],
        'count_time_us': count_bench_pure['avg_time_us'],
        'estimate': estimate_pure,
        'error_pct': error_pure,
        'memory_kb': hll_pure.memory_bytes() / 1024
    }
    
    print(f"  Add: {add_bench_pure['ops_per_second']:,.0f} ops/sec")
    print(f"  Count: {count_bench_pure['avg_time_us']:.1f} µs")
    print(f"  Estimare: {estimate_pure:,.0f} (real: {n_items:,})")
    print(f"  Eroare: {error_pure:.2f}%")
    
    # Test 2: NumPy
    if NUMPY_AVAILABLE:
        print("\n[2] HyperLogLog NumPy")
        print("-" * 40)
        
        hll_numpy = HyperLogLogNumPy(precision)
        add_bench_numpy = benchmark_hll_add(hll_numpy, items_unique[:n_items//10])
        
        hll_numpy = HyperLogLogNumPy(precision)
        for item in items_unique:
            hll_numpy.add(item)
        
        count_bench_numpy = benchmark_hll_count(hll_numpy)
        estimate_numpy = hll_numpy.count()
        error_numpy = abs(estimate_numpy - n_items) / n_items * 100
        
        results['numpy'] = {
            'add_ops_per_sec': add_bench_numpy['ops_per_second'],
            'count_time_us': count_bench_numpy['avg_time_us'],
            'estimate': estimate_numpy,
            'error_pct': error_numpy,
            'memory_kb': hll_numpy.memory_bytes() / 1024
        }
        
        print(f"  Add: {add_bench_numpy['ops_per_second']:,.0f} ops/sec")
        print(f"  Count: {count_bench_numpy['avg_time_us']:.1f} µs")
        print(f"  Estimare: {estimate_numpy:,.0f} (real: {n_items:,})")
        print(f"  Eroare: {error_numpy:.2f}%")
        print(f"  Memorie: {hll_numpy.memory_bytes() / 1024:.1f} KB")
    
    # Test 3: HyperLogLog++
    if NUMPY_AVAILABLE:
        print("\n[3] HyperLogLog++ (cu bias correction)")
        print("-" * 40)
        
        hll_pp = HyperLogLogPlusPlus(precision)
        for item in items_unique:
            hll_pp.add(item)
        
        estimate_pp = hll_pp.count()
        error_pp = abs(estimate_pp - n_items) / n_items * 100
        
        results['hll_plus_plus'] = {
            'estimate': estimate_pp,
            'error_pct': error_pp,
            'memory_kb': hll_pp.memory_bytes() / 1024
        }
        
        print(f"  Estimare: {estimate_pp:,.0f} (real: {n_items:,})")
        print(f"  Eroare: {error_pp:.2f}%")
    
    # Test 4: Date cu duplicate (Zipf)
    print("\n[4] Test cu distribuție Zipf (multe duplicate)")
    print("-" * 40)
    
    hll_zipf = HyperLogLogNumPy(precision) if NUMPY_AVAILABLE else HyperLogLogPure(precision)
    for item in items_zipf:
        hll_zipf.add(item)
    
    estimate_zipf = hll_zipf.count()
    error_zipf = abs(estimate_zipf - actual_card) / actual_card * 100
    
    print(f"  Total elemente: {len(items_zipf):,}")
    print(f"  Cardinalitate reală: {actual_card:,}")
    print(f"  Estimare HLL: {estimate_zipf:,.0f}")
    print(f"  Eroare: {error_zipf:.2f}%")
    
    return results


def test_precision_levels():
    """
    Testează diferite niveluri de precizie și demonstrează trade-off-ul.
    """
    print(f"\n{'='*70}")
    print("ANALIZA TRADE-OFF: PRECIZIE vs MEMORIE")
    print(f"{'='*70}\n")
    
    n_items = 50000
    items = generate_unique_strings(n_items)
    
    print(f"{'p':>3} | {'Buckets':>8} | {'Memorie':>10} | {'Eroare Teor.':>12} | "
          f"{'Estimare':>12} | {'Eroare Real':>11}")
    print("-" * 75)
    
    for p in range(4, 17, 2):
        if NUMPY_AVAILABLE:
            hll = HyperLogLogNumPy(p)
        else:
            hll = HyperLogLogPure(p)
        
        for item in items:
            hll.add(item)
        
        estimate = hll.count()
        m = 2 ** p
        theoretical_error = 1.04 / math.sqrt(m) * 100
        actual_error = abs(estimate - n_items) / n_items * 100
        memory_bytes = m  # ~1 byte per register în implementarea optimizată
        
        memory_str = f"{memory_bytes} B" if memory_bytes < 1024 else f"{memory_bytes/1024:.1f} KB"
        
        print(f"{p:>3} | {m:>8,} | {memory_str:>10} | {theoretical_error:>11.2f}% | "
              f"{estimate:>12,.0f} | {actual_error:>10.2f}%")


def test_merge_operation():
    """
    Demonstrează operația de merge pentru procesare distribuită.
    """
    print(f"\n{'='*70}")
    print("DEMONSTRAȚIE: MERGE PENTRU PROCESARE DISTRIBUITĂ")
    print(f"{'='*70}\n")
    
    # Simulăm 4 "servere" care procesează părți diferite ale datelor
    n_per_server = 25000
    precision = 12
    
    servers = []
    all_items = set()
    
    HLLClass = HyperLogLogNumPy if NUMPY_AVAILABLE else HyperLogLogPure
    
    print("Procesare pe 4 servere separate:")
    for i in range(4):
        hll = HLLClass(precision)
        items = generate_unique_strings(n_per_server, f"server{i}_")
        all_items.update(items)
        
        for item in items:
            hll.add(item)
        
        estimate = hll.count()
        print(f"  Server {i+1}: {n_per_server:,} elemente → Estimare: {estimate:,.0f}")
        servers.append(hll)
    
    # Fuzionăm rezultatele
    print("\nFuzionare rezultate:")
    merged = servers[0]
    for i, server in enumerate(servers[1:], 2):
        merged = merged.merge(server)
        print(f"  După merge cu Server {i}: {merged.count():,.0f}")
    
    actual_total = len(all_items)
    final_estimate = merged.count()
    error = abs(final_estimate - actual_total) / actual_total * 100
    
    print(f"\nRezultat final:")
    print(f"  Cardinalitate reală: {actual_total:,}")
    print(f"  Estimare după merge: {final_estimate:,.0f}")
    print(f"  Eroare: {error:.2f}%")


def comparison_table():
    """
    Afișează tabel comparativ final.
    """
    print(f"\n{'='*80}")
    print("TABEL COMPARATIV: PYTHON vs C (ESTIMAT)")
    print(f"{'='*80}\n")
    
    # Estimări bazate pe benchmark-uri tipice
    data = [
        ("Metric", "Python Pur", "NumPy", "C (estimat)"),
        ("─" * 20, "─" * 15, "─" * 15, "─" * 15),
        ("Add ops/sec", "~100K", "~120K", "~3M"),
        ("Count time", "~500 µs", "~50 µs", "~5 µs"),
        ("Memory (p=14)", "~500 KB*", "16 KB", "12 KB"),
        ("Ease of use", "Excelent", "Foarte bun", "Moderat"),
        ("Debugging", "Ușor", "Ușor", "Dificil"),
        ("Integration", "Python native", "Python native", "FFI necesar"),
    ]
    
    print(f"{'Metric':<20} | {'Python Pur':^15} | {'NumPy':^15} | {'C (estimat)':^15}")
    print("-" * 72)
    for row in data[2:]:
        print(f"{row[0]:<20} | {row[1]:^15} | {row[2]:^15} | {row[3]:^15}")
    
    print("\n* Python pur: overhead semnificativ de la PyObject-uri")
    print("\nCONCLUZII:")
    print("─" * 72)
    print("• Python Pur: Ideal pentru prototipare și înțelegere")
    print("• NumPy: Bun compromis performanță/ușurință")  
    print("• C: Necesar pentru producție high-volume (>1M elemente/sec)")
    print("• Redis/biblioteci specializate: Cel mai practic pentru producție")


def demonstrate_hll_properties():
    """
    Demonstrează proprietățile fundamentale ale HyperLogLog.
    """
    print(f"\n{'='*70}")
    print("DEMONSTRAȚIE: PROPRIETĂȚI HYPERLOGLOG")
    print(f"{'='*70}\n")
    
    HLLClass = HyperLogLogNumPy if NUMPY_AVAILABLE else HyperLogLogPure
    
    # 1. Idempotență
    print("[1] IDEMPOTENȚĂ: Adăugarea aceluiași element de mai multe ori")
    print("-" * 60)
    hll = HLLClass(10)
    hll.add("test_element")
    est1 = hll.count()
    
    for _ in range(1000):
        hll.add("test_element")
    est2 = hll.count()
    
    print(f"    După 1 add: {est1:.0f}")
    print(f"    După 1001 add-uri cu același element: {est2:.0f}")
    print(f"    → Estimarea rămâne ~1 (idempotent!)")
    
    # 2. Comutativitate merge
    print("\n[2] COMUTATIVITATE: Ordinea merge nu contează")
    print("-" * 60)
    hll_a = HLLClass(10)
    hll_b = HLLClass(10)
    
    for i in range(1000):
        hll_a.add(f"a_{i}")
    for i in range(1000):
        hll_b.add(f"b_{i}")
    
    merged_ab = hll_a.merge(hll_b)
    merged_ba = hll_b.merge(hll_a)
    
    print(f"    HLL_A merge HLL_B: {merged_ab.count():.0f}")
    print(f"    HLL_B merge HLL_A: {merged_ba.count():.0f}")
    print(f"    → Rezultate identice (comutativ!)")
    
    # 3. Cardinalitate în timp constant
    print("\n[3] COMPLEXITATE: Count este O(m), independent de n")
    print("-" * 60)
    
    for n in [1000, 10000, 100000]:
        hll = HLLClass(14)
        items = generate_unique_strings(n)
        for item in items:
            hll.add(item)
        
        start = time.perf_counter()
        for _ in range(100):
            hll.count()
        elapsed = (time.perf_counter() - start) / 100 * 1_000_000
        
        print(f"    n={n:>7,}: count() = {elapsed:.1f} µs")
    
    print("    → Timp constant indiferent de cardinalitate!")


# =============================================================================
# MAIN
# =============================================================================

def main():
    """Funcția principală - rulează toate demonstrațiile."""
    print("""
╔══════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║            HYPERLOGLOG: COMPARAȚIE PYTHON vs C                               ║
║            Săptămâna 17 - Structuri Probabilistice Big Data                  ║
║                                                                              ║
╚══════════════════════════════════════════════════════════════════════════════╝
    """)
    
    # Informații despre mediu
    print("CONFIGURAȚIE:")
    print(f"  Python: {__import__('sys').version.split()[0]}")
    print(f"  NumPy: {'Disponibil' if NUMPY_AVAILABLE else 'Indisponibil'}")
    if NUMPY_AVAILABLE:
        print(f"  NumPy versiune: {np.__version__}")
    print()
    
    # Demonstrații
    demonstrate_hll_properties()
    
    # Comparație implementări
    compare_implementations(n_items=50000, precision=14)
    
    # Analiza precizie vs memorie
    test_precision_levels()
    
    # Test merge
    test_merge_operation()
    
    # Tabel comparativ
    comparison_table()
    
    print(f"\n{'='*70}")
    print("RECOMANDĂRI FINALE")
    print(f"{'='*70}")
    print("""
    Pentru PROIECTE EDUCAȚIONALE/PROTOTIPARE:
    → Folosiți implementarea Python (pur sau NumPy)
    → Avantaj: cod clar, ușor de debugat și modificat
    
    Pentru PRODUCȚIE CU VOLUM MIC-MEDIU (<100K elemente/sec):
    → NumPy sau biblioteci Python (datasketch, hyperloglog)
    → Avantaj: integrare simplă în ecosistemul Python
    
    Pentru PRODUCȚIE HIGH-VOLUME (>1M elemente/sec):
    → Implementare C/C++ sau Redis PFCOUNT
    → Avantaj: performanță maximă, memorie minimă
    
    Pentru SISTEME DISTRIBUITE:
    → Redis Cluster cu PFMERGE
    → Presto/Spark cu funcții HLL native
    → Avantaj: scalabilitate horizontală built-in
    """)


if __name__ == "__main__":
    main()
