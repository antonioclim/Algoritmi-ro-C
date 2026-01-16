#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
=============================================================================
SĂPTĂMÂNA 20: Programare Paralelă și Concurentă
Comparație Python - Modul Threading
=============================================================================

Acest fișier demonstrează echivalentele Python pentru conceptele 
de programare concurentă prezentate în C cu POSIX Threads.

ATENȚIE: Python are GIL (Global Interpreter Lock) care limitează 
paralelismul real pentru cod CPU-bound. Pentru operații I/O-bound,
threading funcționează bine. Pentru CPU-bound, folosiți multiprocessing.

Execuție: python3 threading_python.py

Academia de Studii Economice București - CSIE
Algoritmi și Tehnici de Programare (ATP)
Ianuarie 2026
=============================================================================
"""

import threading
import queue
import time
import random
from typing import List, Callable, Any, Optional
from dataclasses import dataclass
from concurrent.futures import ThreadPoolExecutor, Future


# =============================================================================
# PARTEA 1: THREAD-URI DE BAZĂ
# =============================================================================

def basic_thread_example():
    """
    Echivalent Python pentru pthread_create/pthread_join.
    
    În C:
        pthread_t thread;
        pthread_create(&thread, NULL, worker_function, &arg);
        pthread_join(thread, NULL);
    """
    print("\n" + "="*70)
    print("DEMO 1: Thread-uri de Bază")
    print("="*70)
    
    def worker(name: str, iterations: int) -> None:
        """Funcție executată de thread."""
        for i in range(iterations):
            print(f"[Thread {name}] Iterația {i+1}/{iterations}")
            time.sleep(0.1)
        print(f"[Thread {name}] Terminat!")
    
    # Creare și pornire thread-uri
    threads = []
    for i in range(3):
        t = threading.Thread(target=worker, args=(f"Worker-{i}", 3))
        threads.append(t)
        t.start()
    
    # Așteptare terminare (echivalent pthread_join)
    for t in threads:
        t.join()
    
    print("\nToate thread-urile s-au terminat.")


# =============================================================================
# PARTEA 2: MUTEX ȘI RACE CONDITIONS
# =============================================================================

def mutex_example():
    """
    Demonstrație race condition și rezolvare cu Lock (mutex).
    
    În C:
        pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
        pthread_mutex_lock(&mutex);
        // secțiune critică
        pthread_mutex_unlock(&mutex);
    """
    print("\n" + "="*70)
    print("DEMO 2: Mutex și Race Conditions")
    print("="*70)
    
    # Variabilă partajată
    counter_unsafe = 0
    counter_safe = 0
    lock = threading.Lock()
    
    def increment_unsafe(iterations: int) -> None:
        """Incrementare FĂRĂ lock - race condition posibilă."""
        nonlocal counter_unsafe
        for _ in range(iterations):
            temp = counter_unsafe
            time.sleep(0.0000001)  # Mărește șansa de race condition
            counter_unsafe = temp + 1
    
    def increment_safe(iterations: int) -> None:
        """Incrementare CU lock - thread-safe."""
        nonlocal counter_safe
        for _ in range(iterations):
            with lock:  # Echivalent: lock.acquire(); try: ...; finally: lock.release()
                counter_safe += 1
    
    iterations = 10000
    num_threads = 4
    expected = iterations * num_threads
    
    # Test FĂRĂ lock
    threads = [threading.Thread(target=increment_unsafe, args=(iterations,)) 
               for _ in range(num_threads)]
    for t in threads:
        t.start()
    for t in threads:
        t.join()
    
    print(f"Fără lock: {counter_unsafe} (expected: {expected}) - "
          f"{'OK' if counter_unsafe == expected else 'RACE CONDITION!'}")
    
    # Test CU lock
    threads = [threading.Thread(target=increment_safe, args=(iterations,)) 
               for _ in range(num_threads)]
    for t in threads:
        t.start()
    for t in threads:
        t.join()
    
    print(f"Cu lock:   {counter_safe} (expected: {expected}) - "
          f"{'OK' if counter_safe == expected else 'EROARE!'}")


# =============================================================================
# PARTEA 3: SEMAFOARE
# =============================================================================

def semaphore_example():
    """
    Semafor pentru limitarea accesului concurent.
    
    În C:
        sem_t semaphore;
        sem_init(&semaphore, 0, MAX_CONCURRENT);
        sem_wait(&semaphore);
        // acces resursa
        sem_post(&semaphore);
    """
    print("\n" + "="*70)
    print("DEMO 3: Semafoare")
    print("="*70)
    
    MAX_CONCURRENT = 3
    semaphore = threading.Semaphore(MAX_CONCURRENT)
    active_workers = 0
    active_lock = threading.Lock()
    
    def limited_resource_worker(worker_id: int) -> None:
        """Worker cu acces limitat la resursă."""
        nonlocal active_workers
        
        with semaphore:  # Echivalent sem_wait/sem_post
            with active_lock:
                active_workers += 1
                print(f"[Worker {worker_id}] A intrat. Active: {active_workers}/{MAX_CONCURRENT}")
            
            # Simulare lucru
            time.sleep(random.uniform(0.2, 0.5))
            
            with active_lock:
                active_workers -= 1
                print(f"[Worker {worker_id}] A ieșit. Active: {active_workers}/{MAX_CONCURRENT}")
    
    # Lansare 10 workers, dar maxim 3 activi simultan
    threads = [threading.Thread(target=limited_resource_worker, args=(i,)) 
               for i in range(10)]
    for t in threads:
        t.start()
    for t in threads:
        t.join()
    
    print(f"\nMaxim {MAX_CONCURRENT} workers activi în orice moment.")


# =============================================================================
# PARTEA 4: CONDITION VARIABLES
# =============================================================================

def condition_variable_example():
    """
    Condition variables pentru sincronizare complexă.
    
    În C:
        pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
        pthread_cond_wait(&cond, &mutex);
        pthread_cond_signal(&cond);
    """
    print("\n" + "="*70)
    print("DEMO 4: Condition Variables")
    print("="*70)
    
    data_ready = threading.Condition()
    shared_data = []
    finished = False
    
    def producer():
        """Produce date și notifică consumatorii."""
        nonlocal finished
        for i in range(5):
            time.sleep(0.3)
            with data_ready:
                item = f"Item-{i}"
                shared_data.append(item)
                print(f"[Producer] Produs: {item}")
                data_ready.notify()  # Trezește un consumer
        
        with data_ready:
            finished = True
            data_ready.notify_all()  # Trezește toți consumerii
    
    def consumer(consumer_id: int):
        """Consumă date când sunt disponibile."""
        while True:
            with data_ready:
                while not shared_data and not finished:
                    print(f"[Consumer {consumer_id}] Așteaptă...")
                    data_ready.wait()  # Eliberează lock și așteaptă
                
                if shared_data:
                    item = shared_data.pop(0)
                    print(f"[Consumer {consumer_id}] Consumat: {item}")
                elif finished:
                    print(f"[Consumer {consumer_id}] Terminat.")
                    break
    
    # Pornire thread-uri
    threads = [threading.Thread(target=producer)]
    threads.extend([threading.Thread(target=consumer, args=(i,)) for i in range(2)])
    
    for t in threads:
        t.start()
    for t in threads:
        t.join()


# =============================================================================
# PARTEA 5: PRODUCER-CONSUMER CU QUEUE
# =============================================================================

def producer_consumer_example():
    """
    Pattern Producer-Consumer cu queue.Queue (thread-safe).
    
    Echivalent C: bounded buffer cu mutex și condition variables.
    Python oferă queue.Queue care face toate astea automat.
    """
    print("\n" + "="*70)
    print("DEMO 5: Producer-Consumer cu Queue")
    print("="*70)
    
    BUFFER_SIZE = 5
    buffer = queue.Queue(maxsize=BUFFER_SIZE)
    num_items = 10
    
    def producer(producer_id: int):
        """Produce items în buffer."""
        for i in range(num_items):
            item = f"P{producer_id}-Item{i}"
            buffer.put(item)  # Blochează dacă buffer-ul e plin
            print(f"[Producer {producer_id}] Produs: {item} (size: {buffer.qsize()})")
            time.sleep(random.uniform(0.05, 0.15))
    
    def consumer(consumer_id: int):
        """Consumă items din buffer."""
        consumed = 0
        while consumed < num_items:
            try:
                item = buffer.get(timeout=1.0)  # Blochează dacă e gol
                print(f"[Consumer {consumer_id}] Consumat: {item} (size: {buffer.qsize()})")
                buffer.task_done()
                consumed += 1
                time.sleep(random.uniform(0.1, 0.2))
            except queue.Empty:
                break
    
    # 2 producers, 2 consumers
    threads = [
        threading.Thread(target=producer, args=(0,)),
        threading.Thread(target=producer, args=(1,)),
        threading.Thread(target=consumer, args=(0,)),
        threading.Thread(target=consumer, args=(1,)),
    ]
    
    start = time.time()
    for t in threads:
        t.start()
    for t in threads:
        t.join()
    elapsed = time.time() - start
    
    print(f"\nProducție/consum completă în {elapsed:.2f} secunde.")


# =============================================================================
# PARTEA 6: THREAD POOL CU concurrent.futures
# =============================================================================

def thread_pool_example():
    """
    Thread Pool folosind concurrent.futures.ThreadPoolExecutor.
    
    Echivalent la thread pool-ul implementat manual în C.
    Python oferă aceasta funcționalitate built-in.
    """
    print("\n" + "="*70)
    print("DEMO 6: Thread Pool (ThreadPoolExecutor)")
    print("="*70)
    
    def compute_task(n: int) -> int:
        """Task care calculează suma 1..n."""
        result = sum(range(1, n + 1))
        print(f"[Task] Calculat suma 1..{n} = {result}")
        time.sleep(0.1)  # Simulare lucru
        return result
    
    # Creare thread pool cu 4 workers
    with ThreadPoolExecutor(max_workers=4) as executor:
        # Submit task-uri și primire futures
        futures: List[Future] = []
        for n in [1000, 5000, 10000, 50000, 100000]:
            future = executor.submit(compute_task, n)
            futures.append(future)
        
        # Colectare rezultate
        print("\nRezultate:")
        for future in futures:
            result = future.result()  # Blochează până la completare
            print(f"  Rezultat: {result}")


# =============================================================================
# PARTEA 7: FUTURES ȘI ASYNC
# =============================================================================

def futures_example():
    """
    Demonstrație Future API cu timeout și cancel.
    
    Echivalent la sistemul de futures implementat în Tema 1.
    """
    print("\n" + "="*70)
    print("DEMO 7: Futures cu Timeout și Cancel")
    print("="*70)
    
    def slow_task(duration: float) -> str:
        """Task lent pentru demonstrație."""
        time.sleep(duration)
        return f"Completat după {duration}s"
    
    with ThreadPoolExecutor(max_workers=2) as executor:
        # Future cu timeout
        future_fast = executor.submit(slow_task, 0.5)
        future_slow = executor.submit(slow_task, 5.0)
        
        # Încearcă să obții rezultatul cu timeout
        try:
            result = future_fast.result(timeout=1.0)
            print(f"Fast task: {result}")
        except TimeoutError:
            print("Fast task: Timeout!")
        
        try:
            result = future_slow.result(timeout=1.0)
            print(f"Slow task: {result}")
        except TimeoutError:
            print("Slow task: Timeout - încercăm cancel")
            cancelled = future_slow.cancel()
            print(f"Cancel: {'reușit' if cancelled else 'eșuat (deja în execuție)'}")


# =============================================================================
# PARTEA 8: DEADLOCK DEMONSTRATION
# =============================================================================

def deadlock_demonstration():
    """
    Demonstrație potențial deadlock și cum să-l evităm.
    
    Același pattern ca Dining Philosophers din C.
    """
    print("\n" + "="*70)
    print("DEMO 8: Evitarea Deadlock-ului")
    print("="*70)
    
    lock_a = threading.Lock()
    lock_b = threading.Lock()
    
    def thread_1_deadlock():
        """Potențial deadlock - ordine A, B."""
        with lock_a:
            print("[Thread 1] A luat Lock A")
            time.sleep(0.1)  # Mărește șansa de deadlock
            with lock_b:
                print("[Thread 1] A luat Lock B")
    
    def thread_2_deadlock():
        """Potențial deadlock - ordine B, A (inversă!)."""
        with lock_b:
            print("[Thread 2] A luat Lock B")
            time.sleep(0.1)
            with lock_a:
                print("[Thread 2] A luat Lock A")
    
    def thread_safe(thread_id: int):
        """Evită deadlock - ordine consistentă A, B."""
        with lock_a:
            print(f"[Thread {thread_id}] A luat Lock A")
            with lock_b:
                print(f"[Thread {thread_id}] A luat Lock B")
                time.sleep(0.05)
    
    print("\n--- Ordine CONSISTENTĂ (fără deadlock) ---")
    threads = [
        threading.Thread(target=thread_safe, args=(1,)),
        threading.Thread(target=thread_safe, args=(2,)),
    ]
    for t in threads:
        t.start()
    for t in threads:
        t.join()
    print("OK - Niciun deadlock!")
    
    # NOTĂ: Nu rulăm versiunea cu deadlock pentru că ar bloca programul


# =============================================================================
# PARTEA 9: BARRIER
# =============================================================================

def barrier_example():
    """
    Barrier pentru sincronizare în faze.
    
    În C: pthread_barrier_wait()
    """
    print("\n" + "="*70)
    print("DEMO 9: Barrier pentru Sincronizare în Faze")
    print("="*70)
    
    num_workers = 4
    barrier = threading.Barrier(num_workers)
    
    def phased_worker(worker_id: int):
        """Worker cu sincronizare în faze."""
        for phase in range(3):
            # Faza de calcul
            print(f"[Worker {worker_id}] Faza {phase}: calculez...")
            time.sleep(random.uniform(0.1, 0.3))
            
            # Sincronizare - toți așteaptă
            print(f"[Worker {worker_id}] Faza {phase}: așteaptă la barrier")
            barrier.wait()
            
            if worker_id == 0:
                print(f"--- Toți au terminat faza {phase} ---")
    
    threads = [threading.Thread(target=phased_worker, args=(i,)) 
               for i in range(num_workers)]
    for t in threads:
        t.start()
    for t in threads:
        t.join()


# =============================================================================
# PARTEA 10: BENCHMARK GIL
# =============================================================================

def benchmark_gil():
    """
    Demonstrație impactul GIL pentru cod CPU-bound.
    
    ATENȚIE: threading NU oferă speedup pentru operații CPU-bound în Python
    din cauza Global Interpreter Lock (GIL).
    """
    print("\n" + "="*70)
    print("DEMO 10: Impact GIL - Benchmark")
    print("="*70)
    
    def cpu_bound_task(n: int) -> int:
        """Operație CPU-intensive."""
        result = 0
        for i in range(n):
            result += i * i
        return result
    
    N = 5_000_000
    
    # Secvențial
    start = time.time()
    for _ in range(4):
        cpu_bound_task(N)
    sequential_time = time.time() - start
    
    # Cu thread-uri
    start = time.time()
    threads = [threading.Thread(target=cpu_bound_task, args=(N,)) 
               for _ in range(4)]
    for t in threads:
        t.start()
    for t in threads:
        t.join()
    threaded_time = time.time() - start
    
    print(f"Secvențial: {sequential_time:.3f}s")
    print(f"Cu 4 threads: {threaded_time:.3f}s")
    print(f"Speedup: {sequential_time / threaded_time:.2f}x")
    print("\nCONCLUZIE: GIL împiedică paralelismul real pentru CPU-bound!")
    print("Pentru CPU-bound, folosiți multiprocessing (vezi multiprocessing_python.py)")


# =============================================================================
# MAIN
# =============================================================================

def main():
    """Rulează toate demonstrațiile."""
    print("\n")
    print("╔═══════════════════════════════════════════════════════════════════════════════╗")
    print("║     SĂPTĂMÂNA 20: PROGRAMARE PARALELĂ ȘI CONCURENTĂ                          ║")
    print("║     Comparație Python - Modul threading                                       ║")
    print("╚═══════════════════════════════════════════════════════════════════════════════╝")
    
    # Demonstrații
    basic_thread_example()
    mutex_example()
    semaphore_example()
    condition_variable_example()
    producer_consumer_example()
    thread_pool_example()
    futures_example()
    deadlock_demonstration()
    barrier_example()
    benchmark_gil()
    
    print("\n" + "="*70)
    print("TOATE DEMONSTRAȚIILE COMPLETATE!")
    print("="*70)
    print("\nPentru paralelism real CPU-bound, vezi multiprocessing_python.py")


if __name__ == "__main__":
    main()
