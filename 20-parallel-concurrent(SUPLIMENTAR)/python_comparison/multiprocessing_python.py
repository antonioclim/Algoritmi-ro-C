#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
=============================================================================
SĂPTĂMÂNA 20: Programare Paralelă și Concurentă
Comparație Python - Modul Multiprocessing
=============================================================================

Acest fișier demonstrează paralelismul REAL în Python folosind 
multiprocessing, care ocolește GIL prin crearea de procese separate.

DIFERENȚA CHEIE:
- threading: Fire de execuție în ACELAȘI proces, partajează memorie
             DAR: GIL blochează execuția simultană a codului Python
- multiprocessing: Procese SEPARATE, fiecare cu propriul GIL
                   PARALELISM REAL pentru operații CPU-bound

Execuție: python3 multiprocessing_python.py

Academia de Studii Economice București - CSIE
Algoritmi și Tehnici de Programare (ATP)
Ianuarie 2026
=============================================================================
"""

import multiprocessing as mp
from multiprocessing import Pool, Process, Queue, Pipe, Manager, Value, Array
import time
import random
import os
from typing import List, Tuple, Any
from functools import reduce
import math


# =============================================================================
# PARTEA 1: PROCESE DE BAZĂ
# =============================================================================

def basic_process_example():
    """
    Crearea de procese în Python.
    
    Similar cu fork() în C, dar mai simplu de folosit.
    Fiecare proces are propriul spațiu de memorie!
    """
    print("\n" + "="*70)
    print("DEMO 1: Procese de Bază")
    print("="*70)
    
    def worker(name: str, iterations: int) -> None:
        """Funcție executată într-un proces separat."""
        pid = os.getpid()
        for i in range(iterations):
            print(f"[{name}][PID {pid}] Iterația {i+1}/{iterations}")
            time.sleep(0.2)
        print(f"[{name}][PID {pid}] Terminat!")
    
    # Creare procese
    processes = []
    for i in range(3):
        p = mp.Process(target=worker, args=(f"Worker-{i}", 3))
        processes.append(p)
        p.start()
    
    # Așteptare terminare
    for p in processes:
        p.join()
    
    print("\nToate procesele s-au terminat.")


# =============================================================================
# PARTEA 2: BENCHMARK THREADING VS MULTIPROCESSING
# =============================================================================

def cpu_bound_task(n: int) -> int:
    """Operație CPU-intensive: calculează suma pătratelor."""
    return sum(i * i for i in range(n))


def benchmark_comparison():
    """
    Comparație directă: threading vs multiprocessing pentru CPU-bound.
    
    Demonstrează avantajul multiprocessing pentru operații CPU-intensive.
    """
    print("\n" + "="*70)
    print("DEMO 2: Benchmark Threading vs Multiprocessing")
    print("="*70)
    
    import threading
    
    N = 5_000_000
    num_workers = 4
    
    # 1. Secvențial
    print("\n[Secvențial]")
    start = time.time()
    results = [cpu_bound_task(N) for _ in range(num_workers)]
    sequential_time = time.time() - start
    print(f"  Timp: {sequential_time:.3f}s")
    
    # 2. Threading (limitat de GIL)
    print("\n[Threading - limitat de GIL]")
    results = []
    lock = threading.Lock()
    
    def threaded_worker():
        result = cpu_bound_task(N)
        with lock:
            results.append(result)
    
    start = time.time()
    threads = [threading.Thread(target=threaded_worker) for _ in range(num_workers)]
    for t in threads:
        t.start()
    for t in threads:
        t.join()
    threading_time = time.time() - start
    print(f"  Timp: {threading_time:.3f}s")
    print(f"  Speedup: {sequential_time / threading_time:.2f}x")
    
    # 3. Multiprocessing (paralelism real)
    print("\n[Multiprocessing - paralelism real]")
    start = time.time()
    with Pool(processes=num_workers) as pool:
        results = pool.map(cpu_bound_task, [N] * num_workers)
    multiprocessing_time = time.time() - start
    print(f"  Timp: {multiprocessing_time:.3f}s")
    print(f"  Speedup: {sequential_time / multiprocessing_time:.2f}x")
    
    # Sumar
    print("\n" + "-"*50)
    print("SUMAR:")
    print(f"  Secvențial:      {sequential_time:.3f}s (baseline)")
    print(f"  Threading:       {threading_time:.3f}s ({sequential_time/threading_time:.2f}x)")
    print(f"  Multiprocessing: {multiprocessing_time:.3f}s ({sequential_time/multiprocessing_time:.2f}x)")
    print("\nConcluzie: Multiprocessing oferă paralelism real pentru CPU-bound!")


# =============================================================================
# PARTEA 3: POOL DE PROCESE
# =============================================================================

def pool_example():
    """
    Pool de procese - echivalent thread pool din C.
    
    Pool-ul gestionează automat crearea/refolosirea proceselor.
    """
    print("\n" + "="*70)
    print("DEMO 3: Pool de Procese")
    print("="*70)
    
    def compute_factorial(n: int) -> Tuple[int, int]:
        """Calculează factorial."""
        result = math.factorial(n)
        return n, result
    
    numbers = [10, 15, 20, 25, 30, 35]
    
    # Folosind pool.map
    print("\n[pool.map - procesare sincronă]")
    with Pool(processes=4) as pool:
        results = pool.map(compute_factorial, numbers)
    
    for n, fact in results:
        print(f"  {n}! = {fact}")
    
    # Folosind pool.apply_async (asincron)
    print("\n[pool.apply_async - procesare asincronă]")
    with Pool(processes=4) as pool:
        async_results = [pool.apply_async(compute_factorial, (n,)) for n in numbers]
        
        for ar in async_results:
            n, fact = ar.get(timeout=5)  # Așteaptă rezultatul
            print(f"  {n}! = {fact}")
    
    # Folosind pool.imap (iterator lazy)
    print("\n[pool.imap - procesare lazy]")
    with Pool(processes=4) as pool:
        for n, fact in pool.imap(compute_factorial, numbers):
            print(f"  {n}! = {fact}")


# =============================================================================
# PARTEA 4: COMUNICARE ÎNTRE PROCESE - QUEUE
# =============================================================================

def queue_communication():
    """
    Comunicare între procese folosind Queue.
    
    Echivalent cu bounded buffer din C, dar inter-proces.
    """
    print("\n" + "="*70)
    print("DEMO 4: Comunicare cu Queue")
    print("="*70)
    
    def producer(queue: Queue, producer_id: int, num_items: int):
        """Produce items în queue."""
        pid = os.getpid()
        for i in range(num_items):
            item = f"P{producer_id}-Item{i}"
            queue.put(item)
            print(f"[Producer {producer_id}][PID {pid}] Produs: {item}")
            time.sleep(random.uniform(0.05, 0.1))
        queue.put(None)  # Semnal de terminare
    
    def consumer(queue: Queue, consumer_id: int, num_producers: int):
        """Consumă items din queue."""
        pid = os.getpid()
        finished_producers = 0
        while finished_producers < num_producers:
            item = queue.get()
            if item is None:
                finished_producers += 1
                print(f"[Consumer {consumer_id}][PID {pid}] Producer terminat ({finished_producers}/{num_producers})")
            else:
                print(f"[Consumer {consumer_id}][PID {pid}] Consumat: {item}")
                time.sleep(random.uniform(0.05, 0.15))
    
    # Setup
    queue = Queue()
    num_producers = 2
    items_per_producer = 5
    
    # Pornire procese
    producers = [Process(target=producer, args=(queue, i, items_per_producer)) 
                 for i in range(num_producers)]
    consumer_proc = Process(target=consumer, args=(queue, 0, num_producers))
    
    for p in producers:
        p.start()
    consumer_proc.start()
    
    for p in producers:
        p.join()
    consumer_proc.join()
    
    print("\nComunicare completă!")


# =============================================================================
# PARTEA 5: COMUNICARE CU PIPE
# =============================================================================

def pipe_communication():
    """
    Comunicare bidirecțională folosind Pipe.
    
    Mai eficient decât Queue pentru comunicare 1-la-1.
    """
    print("\n" + "="*70)
    print("DEMO 5: Comunicare cu Pipe")
    print("="*70)
    
    def sender(conn, messages: List[str]):
        """Trimite mesaje prin pipe."""
        pid = os.getpid()
        for msg in messages:
            print(f"[Sender][PID {pid}] Trimite: {msg}")
            conn.send(msg)
            time.sleep(0.2)
        conn.send(None)  # Semnal de terminare
        conn.close()
    
    def receiver(conn):
        """Primește mesaje prin pipe."""
        pid = os.getpid()
        while True:
            msg = conn.recv()
            if msg is None:
                break
            print(f"[Receiver][PID {pid}] Primit: {msg}")
        conn.close()
    
    # Creare pipe (două capete)
    parent_conn, child_conn = Pipe()
    
    messages = ["Salut!", "Cum ești?", "Bine, mulțumesc!", "La revedere!"]
    
    # Pornire procese
    p_sender = Process(target=sender, args=(parent_conn, messages))
    p_receiver = Process(target=receiver, args=(child_conn,))
    
    p_sender.start()
    p_receiver.start()
    
    p_sender.join()
    p_receiver.join()


# =============================================================================
# PARTEA 6: MEMORIE PARTAJATĂ - VALUE ȘI ARRAY
# =============================================================================

def shared_memory_example():
    """
    Partajare memorie între procese cu Value și Array.
    
    ATENȚIE: Necesită sincronizare explicită!
    """
    print("\n" + "="*70)
    print("DEMO 6: Memorie Partajată (Value & Array)")
    print("="*70)
    
    def increment_counter(counter, lock, iterations: int):
        """Incrementează counter partajat."""
        pid = os.getpid()
        for _ in range(iterations):
            with lock:
                counter.value += 1
        print(f"[PID {pid}] Terminat {iterations} incrementări")
    
    def update_array(arr, lock, worker_id: int):
        """Actualizează array partajat."""
        pid = os.getpid()
        with lock:
            for i in range(len(arr)):
                arr[i] += worker_id
        print(f"[Worker {worker_id}][PID {pid}] A actualizat array")
    
    # Shared counter
    print("\n[Counter partajat]")
    counter = Value('i', 0)  # 'i' = integer
    lock = mp.Lock()
    iterations = 10000
    
    processes = [Process(target=increment_counter, args=(counter, lock, iterations)) 
                 for _ in range(4)]
    for p in processes:
        p.start()
    for p in processes:
        p.join()
    
    print(f"Counter final: {counter.value} (expected: {iterations * 4})")
    
    # Shared array
    print("\n[Array partajat]")
    arr = Array('i', [0, 0, 0, 0, 0])  # Array de 5 integers
    
    processes = [Process(target=update_array, args=(arr, lock, i)) 
                 for i in range(1, 4)]
    for p in processes:
        p.start()
    for p in processes:
        p.join()
    
    print(f"Array final: {list(arr)}")


# =============================================================================
# PARTEA 7: MANAGER PENTRU STRUCTURI COMPLEXE
# =============================================================================

def manager_example():
    """
    Manager pentru partajarea structurilor complexe între procese.
    
    Permite list, dict, etc. partajate (cu overhead de serializare).
    """
    print("\n" + "="*70)
    print("DEMO 7: Manager pentru Structuri Complexe")
    print("="*70)
    
    def worker(shared_list, shared_dict, worker_id: int):
        """Worker care modifică structuri partajate."""
        pid = os.getpid()
        
        # Adaugă la listă
        shared_list.append(f"Item de la Worker {worker_id}")
        
        # Adaugă în dict
        shared_dict[f"worker_{worker_id}"] = {
            'pid': pid,
            'timestamp': time.time()
        }
        
        print(f"[Worker {worker_id}][PID {pid}] A contribuit la structurile partajate")
    
    with Manager() as manager:
        # Structuri partajate gestionate de manager
        shared_list = manager.list()
        shared_dict = manager.dict()
        
        # Pornire workers
        processes = [Process(target=worker, args=(shared_list, shared_dict, i)) 
                     for i in range(4)]
        for p in processes:
            p.start()
        for p in processes:
            p.join()
        
        # Rezultate
        print(f"\nLista partajată: {list(shared_list)}")
        print(f"Dict partajat: {dict(shared_dict)}")


# =============================================================================
# PARTEA 8: MAP-REDUCE CU MULTIPROCESSING
# =============================================================================

def map_reduce_example():
    """
    Implementare simplă Map-Reduce folosind multiprocessing.
    
    Echivalent la Tema 2 din C.
    """
    print("\n" + "="*70)
    print("DEMO 8: Map-Reduce (Word Count)")
    print("="*70)
    
    # Date de test
    documents = [
        "ana are mere si pere ana are si caise",
        "ion are mere ion are si prune",
        "maria are pere si caise maria merge la piata",
        "ana si maria sunt prietene ele au mere"
    ]
    
    def mapper(doc: str) -> List[Tuple[str, int]]:
        """Map: document -> lista de (cuvânt, 1)."""
        words = doc.lower().split()
        return [(word, 1) for word in words]
    
    def reducer(word_counts: List[Tuple[str, int]]) -> dict:
        """Reduce: agregare counts per cuvânt."""
        result = {}
        for word, count in word_counts:
            result[word] = result.get(word, 0) + count
        return result
    
    # MAP PHASE (paralel)
    print("\n[Faza MAP - paralelă]")
    with Pool(processes=4) as pool:
        mapped_results = pool.map(mapper, documents)
    
    # Flatten rezultate
    all_pairs = []
    for pairs in mapped_results:
        all_pairs.extend(pairs)
    print(f"  Total perechi emise: {len(all_pairs)}")
    
    # SHUFFLE PHASE (grupare după cheie)
    print("\n[Faza SHUFFLE]")
    # În producție, am partiționa pe mai mulți reducers
    
    # REDUCE PHASE
    print("\n[Faza REDUCE]")
    final_counts = reducer(all_pairs)
    
    # Sortare și afișare top 10
    print("\nTop 10 cuvinte:")
    sorted_counts = sorted(final_counts.items(), key=lambda x: x[1], reverse=True)
    for word, count in sorted_counts[:10]:
        print(f"  {word}: {count}")


# =============================================================================
# PARTEA 9: PARALLEL SORT
# =============================================================================

def parallel_sort_example():
    """
    Sortare paralelă folosind multiprocessing.
    
    Echivalent la exercițiul de Parallel Quick Sort din C.
    """
    print("\n" + "="*70)
    print("DEMO 9: Sortare Paralelă")
    print("="*70)
    
    def merge(left: List[int], right: List[int]) -> List[int]:
        """Interclasare două liste sortate."""
        result = []
        i = j = 0
        while i < len(left) and j < len(right):
            if left[i] <= right[j]:
                result.append(left[i])
                i += 1
            else:
                result.append(right[j])
                j += 1
        result.extend(left[i:])
        result.extend(right[j:])
        return result
    
    def parallel_merge_sort(arr: List[int], pool: Pool, depth: int = 0) -> List[int]:
        """Merge sort paralel cu limită de adâncime."""
        if len(arr) <= 1:
            return arr
        
        mid = len(arr) // 2
        left_half = arr[:mid]
        right_half = arr[mid:]
        
        if depth < 2:  # Paralelizare doar pentru primele nivele
            # Sortare paralelă
            left_result = pool.apply_async(sorted, (left_half,))
            right_result = pool.apply_async(sorted, (right_half,))
            left_sorted = left_result.get()
            right_sorted = right_result.get()
        else:
            # Sortare secvențială
            left_sorted = sorted(left_half)
            right_sorted = sorted(right_half)
        
        return merge(left_sorted, right_sorted)
    
    # Generare date
    n = 100000
    arr = [random.randint(0, 1000000) for _ in range(n)]
    
    # Sortare secvențială
    print(f"\n[Sortare secvențială - {n} elemente]")
    arr_copy = arr.copy()
    start = time.time()
    sorted_arr = sorted(arr_copy)
    sequential_time = time.time() - start
    print(f"  Timp: {sequential_time:.3f}s")
    
    # Sortare paralelă (simplificată - folosim sorted() ca sub-task)
    print(f"\n[Sortare paralelă]")
    arr_copy = arr.copy()
    start = time.time()
    
    # Împărțire în chunk-uri, sortare paralelă, merge
    num_chunks = 4
    chunk_size = len(arr_copy) // num_chunks
    chunks = [arr_copy[i*chunk_size:(i+1)*chunk_size] for i in range(num_chunks)]
    # Ultimul chunk ia și restul
    chunks[-1].extend(arr_copy[num_chunks*chunk_size:])
    
    with Pool(processes=4) as pool:
        sorted_chunks = pool.map(sorted, chunks)
    
    # Merge secvențial (poate fi și paralel)
    result = sorted_chunks[0]
    for chunk in sorted_chunks[1:]:
        result = merge(result, chunk)
    
    parallel_time = time.time() - start
    print(f"  Timp: {parallel_time:.3f}s")
    print(f"  Speedup: {sequential_time / parallel_time:.2f}x")
    print(f"  Corect: {result == sorted_arr}")


# =============================================================================
# PARTEA 10: AMDAHL'S LAW DEMONSTRATION
# =============================================================================

def amdahl_demonstration():
    """
    Demonstrație practică a Legii lui Amdahl.
    
    Speedup = 1 / ((1-P) + P/N)
    Unde: P = fracția paralelizabilă, N = număr procesoare
    """
    print("\n" + "="*70)
    print("DEMO 10: Legea lui Amdahl")
    print("="*70)
    
    def mixed_workload(serial_work: int, parallel_work: int) -> None:
        """Workload cu parte serială și parte paralelizabilă."""
        # Parte serială (nu poate fi paralelizată)
        for _ in range(serial_work):
            _ = sum(i*i for i in range(1000))
        
        # Parte paralelizabilă
        for _ in range(parallel_work):
            _ = sum(i*i for i in range(1000))
    
    serial_ratio = 0.2  # 20% serial
    total_iterations = 100
    serial_work = int(total_iterations * serial_ratio)
    parallel_work = total_iterations - serial_work
    
    print(f"\nWorkload: {serial_ratio*100:.0f}% serial, {(1-serial_ratio)*100:.0f}% paralelizabil")
    print(f"Serial work: {serial_work}, Parallel work: {parallel_work}")
    
    # Secvențial (baseline)
    start = time.time()
    mixed_workload(serial_work, parallel_work)
    baseline_time = time.time() - start
    print(f"\n[Secvențial] Timp: {baseline_time:.3f}s")
    
    # Cu diferite numere de procese
    print("\n" + "-"*50)
    print(f"{'Procese':>8} | {'Timp (s)':>10} | {'Speedup':>10} | {'Amdahl Teoretic':>15}")
    print("-"*50)
    
    for num_procs in [1, 2, 4, 8]:
        # Calcul teoretic Amdahl
        P = 1 - serial_ratio
        theoretical_speedup = 1 / ((1-P) + P/num_procs)
        
        # Măsurare reală
        start = time.time()
        
        # Parte serială (nu poate fi paralelizată)
        for _ in range(serial_work):
            _ = sum(i*i for i in range(1000))
        
        # Parte paralelizabilă
        with Pool(processes=num_procs) as pool:
            parallel_per_proc = parallel_work // num_procs
            pool.map(lambda x: sum(i*i for i in range(1000)), 
                     range(parallel_work))
        
        actual_time = time.time() - start
        actual_speedup = baseline_time / actual_time
        
        print(f"{num_procs:>8} | {actual_time:>10.3f} | {actual_speedup:>10.2f}x | {theoretical_speedup:>15.2f}x")
    
    print("\nConcluzie: Partea serială limitează speedup-ul maxim!")
    print(f"Speedup maxim teoretic (∞ procesoare): {1/serial_ratio:.2f}x")


# =============================================================================
# MAIN
# =============================================================================

def main():
    """Rulează toate demonstrațiile."""
    print("\n")
    print("╔═══════════════════════════════════════════════════════════════════════════════╗")
    print("║     SĂPTĂMÂNA 20: PROGRAMARE PARALELĂ ȘI CONCURENTĂ                          ║")
    print("║     Comparație Python - Modul multiprocessing                                 ║")
    print("╚═══════════════════════════════════════════════════════════════════════════════╝")
    
    # Notă despre platformă
    print(f"\nPlatformă: {os.name}")
    print(f"Număr CPU-uri: {mp.cpu_count()}")
    
    # Demonstrații
    basic_process_example()
    benchmark_comparison()
    pool_example()
    queue_communication()
    pipe_communication()
    shared_memory_example()
    manager_example()
    map_reduce_example()
    parallel_sort_example()
    amdahl_demonstration()
    
    print("\n" + "="*70)
    print("TOATE DEMONSTRAȚIILE COMPLETATE!")
    print("="*70)
    print("\nSumar:")
    print("  - threading: Bun pentru I/O-bound, limitat de GIL pentru CPU-bound")
    print("  - multiprocessing: Paralelism real pentru CPU-bound, overhead mai mare")
    print("  - Alegere: Depinde de tipul workload-ului!")


if __name__ == "__main__":
    # Necesar pentru Windows
    mp.freeze_support()
    main()
