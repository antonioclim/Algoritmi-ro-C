#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
=============================================================================
SĂPTĂMÂNA 20: Programare Paralelă și Concurentă
Script pentru vizualizarea rezultatelor benchmark
=============================================================================

Acest script generează grafice pentru:
1. Speedup vs. Număr de threads
2. Eficiență vs. Număr de threads
3. Comparație cu Legea lui Amdahl
4. Timp de execuție vs. Dimensiune array

Utilizare:
    python3 plot_results.py [results_file.csv]
    
Dacă nu se specifică fișier, se folosește benchmark_results.csv

Dependențe:
    pip install matplotlib numpy

Autor: Material educațional ATP - CSIE ASE București
Data: Ianuarie 2026
=============================================================================
"""

import sys
import os
import csv
from datetime import datetime

# Verificare și instalare opțională matplotlib
try:
    import matplotlib.pyplot as plt
    import matplotlib.patches as mpatches
    HAS_MATPLOTLIB = True
except ImportError:
    HAS_MATPLOTLIB = False
    print("⚠️  matplotlib nu este instalat. Graficele nu vor fi generate.")
    print("   Instalare: pip install matplotlib")

try:
    import numpy as np
    HAS_NUMPY = True
except ImportError:
    HAS_NUMPY = False
    print("⚠️  numpy nu este instalat. Analiza avansată limitată.")
    print("   Instalare: pip install numpy")


# =============================================================================
# CULORI ȘI STILURI
# =============================================================================
COLORS = {
    'primary': '#00d9ff',      # Cyan
    'secondary': '#ff006e',    # Magenta
    'success': '#00f5d4',      # Teal
    'warning': '#ffbe0b',      # Yellow
    'danger': '#ff4444',       # Red
    'background': '#1a1a2e',   # Dark blue
    'text': '#e8e8e8',         # Light gray
    'grid': '#30363d',         # Medium gray
    'amdahl': '#a371f7',       # Purple
}

# Stil pentru grafice
plt.style.use('dark_background') if HAS_MATPLOTLIB else None


def setup_plot_style():
    """Configurează stilul global pentru grafice."""
    if not HAS_MATPLOTLIB:
        return
    
    plt.rcParams.update({
        'figure.facecolor': COLORS['background'],
        'axes.facecolor': COLORS['background'],
        'axes.edgecolor': COLORS['text'],
        'axes.labelcolor': COLORS['text'],
        'text.color': COLORS['text'],
        'xtick.color': COLORS['text'],
        'ytick.color': COLORS['text'],
        'grid.color': COLORS['grid'],
        'grid.alpha': 0.3,
        'font.size': 10,
        'axes.titlesize': 14,
        'axes.labelsize': 12,
        'legend.fontsize': 10,
        'figure.titlesize': 16,
    })


def load_results(filename):
    """
    Încarcă rezultatele benchmark din fișier CSV.
    
    Format așteptat:
    threads,time_seconds,speedup,efficiency
    1,1.234,1.00,100.0
    2,0.678,1.82,91.0
    ...
    
    Args:
        filename: Calea către fișierul CSV
        
    Returns:
        dict cu liste: threads, times, speedups, efficiencies
    """
    results = {
        'threads': [],
        'times': [],
        'speedups': [],
        'efficiencies': []
    }
    
    try:
        with open(filename, 'r') as f:
            reader = csv.DictReader(f)
            for row in reader:
                results['threads'].append(int(row.get('threads', row.get('Threads', 0))))
                results['times'].append(float(row.get('time_seconds', row.get('Time', 0))))
                results['speedups'].append(float(row.get('speedup', row.get('Speedup', 1))))
                results['efficiencies'].append(float(row.get('efficiency', row.get('Efficiency', 100))))
    except FileNotFoundError:
        print(f"❌ Fișierul {filename} nu a fost găsit.")
        return None
    except Exception as e:
        print(f"❌ Eroare la citirea fișierului: {e}")
        return None
    
    return results


def amdahl_speedup(n, p):
    """
    Calculează speedup-ul teoretic conform Legii lui Amdahl.
    
    Args:
        n: Numărul de procesoare
        p: Fracțiunea paralelizabilă (0 <= p <= 1)
        
    Returns:
        Speedup-ul teoretic
    """
    if p < 0 or p > 1:
        raise ValueError("p trebuie să fie între 0 și 1")
    return 1.0 / ((1.0 - p) + p / n)


def estimate_parallel_fraction(threads, speedups):
    """
    Estimează fracțiunea paralelizabilă din datele empirice.
    
    Folosește metoda celor mai mici pătrate pentru a găsi P
    care minimizează eroarea față de speedup-urile observate.
    
    Args:
        threads: Lista cu numărul de thread-uri
        speedups: Lista cu speedup-urile măsurate
        
    Returns:
        Estimarea lui P (fracțiunea paralelizabilă)
    """
    if not HAS_NUMPY:
        # Estimare simplă folosind ultimul punct
        n = threads[-1]
        s = speedups[-1]
        # S = 1 / ((1-P) + P/n)
        # Rezolvăm pentru P:
        # S * (1-P) + S*P/n = 1
        # S - S*P + S*P/n = 1
        # S - S*P * (1 - 1/n) = 1
        # S*P * (1 - 1/n) = S - 1
        # P = (S - 1) / (S * (1 - 1/n))
        if s > 1 and n > 1:
            p = (s - 1) / (s * (1 - 1/n))
            return min(max(p, 0), 1)
        return 0.9  # Default
    
    # Optimizare cu numpy
    def error(p):
        theoretical = [amdahl_speedup(n, p) for n in threads]
        return sum((t - s)**2 for t, s in zip(theoretical, speedups))
    
    # Grid search simplu
    best_p = 0.5
    best_error = float('inf')
    for p in np.linspace(0.5, 0.99, 100):
        e = error(p)
        if e < best_error:
            best_error = e
            best_p = p
    
    return best_p


def plot_speedup(results, p_estimated, output_dir='.'):
    """
    Generează graficul Speedup vs. Threads.
    
    Args:
        results: Dicționar cu rezultatele benchmark
        p_estimated: Fracțiunea paralelizabilă estimată
        output_dir: Directorul pentru salvarea graficului
    """
    if not HAS_MATPLOTLIB:
        print("⚠️  Graficul speedup nu poate fi generat (lipsește matplotlib)")
        return
    
    setup_plot_style()
    
    fig, ax = plt.subplots(figsize=(10, 6))
    
    threads = results['threads']
    speedups = results['speedups']
    max_threads = max(threads) * 1.2
    
    # Speedup ideal (linear)
    ideal_threads = list(range(1, int(max_threads) + 1))
    ideal_speedups = ideal_threads
    
    # Speedup Amdahl
    amdahl_threads = list(range(1, int(max_threads) + 1))
    amdahl_speedups = [amdahl_speedup(n, p_estimated) for n in amdahl_threads]
    
    # Plot
    ax.plot(ideal_threads, ideal_speedups, '--', color=COLORS['text'], 
            alpha=0.5, label='Ideal (linear)', linewidth=1.5)
    ax.plot(amdahl_threads, amdahl_speedups, '-', color=COLORS['amdahl'],
            label=f'Amdahl (P={p_estimated:.2%})', linewidth=2)
    ax.plot(threads, speedups, 'o-', color=COLORS['primary'],
            label='Măsurat', linewidth=2.5, markersize=10)
    
    # Limită Amdahl
    max_speedup = amdahl_speedup(float('inf'), p_estimated)
    ax.axhline(y=max_speedup, color=COLORS['danger'], linestyle=':',
               alpha=0.7, label=f'Limită Amdahl: {max_speedup:.2f}×')
    
    ax.set_xlabel('Număr de Threads')
    ax.set_ylabel('Speedup (×)')
    ax.set_title('Speedup: Măsurat vs. Teoretic (Amdahl)')
    ax.legend(loc='upper left', framealpha=0.8)
    ax.grid(True, alpha=0.3)
    ax.set_xlim(0, max_threads)
    ax.set_ylim(0, max(max(speedups) * 1.2, max_speedup * 1.1))
    
    # Salvare
    output_path = os.path.join(output_dir, 'speedup_plot.png')
    plt.tight_layout()
    plt.savefig(output_path, dpi=150, facecolor=COLORS['background'])
    plt.close()
    
    print(f"✓ Grafic salvat: {output_path}")


def plot_efficiency(results, output_dir='.'):
    """
    Generează graficul Eficiență vs. Threads.
    
    Args:
        results: Dicționar cu rezultatele benchmark
        output_dir: Directorul pentru salvarea graficului
    """
    if not HAS_MATPLOTLIB:
        print("⚠️  Graficul eficiență nu poate fi generat (lipsește matplotlib)")
        return
    
    setup_plot_style()
    
    fig, ax = plt.subplots(figsize=(10, 6))
    
    threads = results['threads']
    efficiencies = results['efficiencies']
    
    # Bar chart
    bars = ax.bar(threads, efficiencies, color=COLORS['success'], 
                  edgecolor=COLORS['text'], alpha=0.8)
    
    # Colorare în funcție de eficiență
    for bar, eff in zip(bars, efficiencies):
        if eff >= 90:
            bar.set_color(COLORS['success'])
        elif eff >= 70:
            bar.set_color(COLORS['warning'])
        else:
            bar.set_color(COLORS['danger'])
    
    # Linie de referință la 100%
    ax.axhline(y=100, color=COLORS['primary'], linestyle='--', 
               alpha=0.7, label='Ideal (100%)')
    
    # Etichete pe bare
    for bar, eff in zip(bars, efficiencies):
        height = bar.get_height()
        ax.annotate(f'{eff:.1f}%',
                    xy=(bar.get_x() + bar.get_width() / 2, height),
                    xytext=(0, 3),
                    textcoords="offset points",
                    ha='center', va='bottom',
                    fontsize=9, color=COLORS['text'])
    
    ax.set_xlabel('Număr de Threads')
    ax.set_ylabel('Eficiență (%)')
    ax.set_title('Eficiență Paralelă = Speedup / Threads × 100%')
    ax.set_xticks(threads)
    ax.set_ylim(0, 120)
    ax.legend(loc='upper right')
    ax.grid(True, alpha=0.3, axis='y')
    
    # Salvare
    output_path = os.path.join(output_dir, 'efficiency_plot.png')
    plt.tight_layout()
    plt.savefig(output_path, dpi=150, facecolor=COLORS['background'])
    plt.close()
    
    print(f"✓ Grafic salvat: {output_path}")


def plot_execution_time(results, output_dir='.'):
    """
    Generează graficul Timp de Execuție vs. Threads.
    
    Args:
        results: Dicționar cu rezultatele benchmark
        output_dir: Directorul pentru salvarea graficului
    """
    if not HAS_MATPLOTLIB:
        print("⚠️  Graficul timp nu poate fi generat (lipsește matplotlib)")
        return
    
    setup_plot_style()
    
    fig, ax = plt.subplots(figsize=(10, 6))
    
    threads = results['threads']
    times = results['times']
    
    # Plot
    ax.plot(threads, times, 'o-', color=COLORS['secondary'],
            linewidth=2.5, markersize=10, label='Timp măsurat')
    
    # Fill area
    ax.fill_between(threads, times, alpha=0.3, color=COLORS['secondary'])
    
    ax.set_xlabel('Număr de Threads')
    ax.set_ylabel('Timp de Execuție (secunde)')
    ax.set_title('Timp de Execuție vs. Număr de Threads')
    ax.set_xticks(threads)
    ax.legend(loc='upper right')
    ax.grid(True, alpha=0.3)
    
    # Salvare
    output_path = os.path.join(output_dir, 'execution_time_plot.png')
    plt.tight_layout()
    plt.savefig(output_path, dpi=150, facecolor=COLORS['background'])
    plt.close()
    
    print(f"✓ Grafic salvat: {output_path}")


def plot_combined(results, p_estimated, output_dir='.'):
    """
    Generează un grafic combinat cu toate metricile.
    
    Args:
        results: Dicționar cu rezultatele benchmark
        p_estimated: Fracțiunea paralelizabilă estimată
        output_dir: Directorul pentru salvarea graficului
    """
    if not HAS_MATPLOTLIB:
        print("⚠️  Graficul combinat nu poate fi generat (lipsește matplotlib)")
        return
    
    setup_plot_style()
    
    fig, axes = plt.subplots(2, 2, figsize=(14, 10))
    fig.suptitle('Analiza Performanței Paralele - Săptămâna 20', 
                 fontsize=16, color=COLORS['primary'])
    
    threads = results['threads']
    times = results['times']
    speedups = results['speedups']
    efficiencies = results['efficiencies']
    
    # 1. Speedup
    ax1 = axes[0, 0]
    ideal = list(range(1, max(threads) + 1))
    amdahl = [amdahl_speedup(n, p_estimated) for n in ideal]
    ax1.plot(ideal, ideal, '--', color=COLORS['text'], alpha=0.5, label='Ideal')
    ax1.plot(ideal, amdahl, '-', color=COLORS['amdahl'], label=f'Amdahl (P={p_estimated:.2%})')
    ax1.plot(threads, speedups, 'o-', color=COLORS['primary'], label='Măsurat')
    ax1.set_xlabel('Threads')
    ax1.set_ylabel('Speedup')
    ax1.set_title('Speedup')
    ax1.legend(fontsize=8)
    ax1.grid(True, alpha=0.3)
    
    # 2. Eficiență
    ax2 = axes[0, 1]
    bars = ax2.bar(threads, efficiencies, color=COLORS['success'], alpha=0.8)
    for bar, eff in zip(bars, efficiencies):
        if eff >= 90:
            bar.set_color(COLORS['success'])
        elif eff >= 70:
            bar.set_color(COLORS['warning'])
        else:
            bar.set_color(COLORS['danger'])
    ax2.axhline(y=100, color=COLORS['primary'], linestyle='--', alpha=0.7)
    ax2.set_xlabel('Threads')
    ax2.set_ylabel('Eficiență (%)')
    ax2.set_title('Eficiență')
    ax2.set_xticks(threads)
    ax2.set_ylim(0, 120)
    ax2.grid(True, alpha=0.3, axis='y')
    
    # 3. Timp
    ax3 = axes[1, 0]
    ax3.plot(threads, times, 'o-', color=COLORS['secondary'], linewidth=2, markersize=8)
    ax3.fill_between(threads, times, alpha=0.3, color=COLORS['secondary'])
    ax3.set_xlabel('Threads')
    ax3.set_ylabel('Timp (s)')
    ax3.set_title('Timp de Execuție')
    ax3.set_xticks(threads)
    ax3.grid(True, alpha=0.3)
    
    # 4. Tabel rezumat
    ax4 = axes[1, 1]
    ax4.axis('off')
    
    # Creăm un tabel
    table_data = [
        ['Threads', 'Timp (s)', 'Speedup', 'Eficiență (%)'],
    ]
    for t, tm, s, e in zip(threads, times, speedups, efficiencies):
        table_data.append([str(t), f'{tm:.4f}', f'{s:.2f}×', f'{e:.1f}%'])
    
    table = ax4.table(cellText=table_data, loc='center', cellLoc='center')
    table.auto_set_font_size(False)
    table.set_fontsize(10)
    table.scale(1.2, 1.5)
    
    # Stilizare tabel
    for i, key in enumerate(table_data[0]):
        table[(0, i)].set_facecolor(COLORS['primary'])
        table[(0, i)].set_text_props(weight='bold', color=COLORS['background'])
    
    ax4.set_title(f'Rezumat (P estimat = {p_estimated:.2%})')
    
    # Salvare
    output_path = os.path.join(output_dir, 'combined_analysis.png')
    plt.tight_layout()
    plt.savefig(output_path, dpi=150, facecolor=COLORS['background'])
    plt.close()
    
    print(f"✓ Grafic combinat salvat: {output_path}")


def print_ascii_chart(results, p_estimated):
    """
    Afișează un grafic ASCII în terminal (fallback când matplotlib lipsește).
    
    Args:
        results: Dicționar cu rezultatele benchmark
        p_estimated: Fracțiunea paralelizabilă estimată
    """
    threads = results['threads']
    speedups = results['speedups']
    
    print("\n" + "=" * 60)
    print(" GRAFIC SPEEDUP (ASCII)")
    print("=" * 60)
    
    max_speedup = max(speedups)
    max_theoretical = amdahl_speedup(max(threads), p_estimated)
    scale = max(max_speedup, max_theoretical, max(threads))
    width = 50
    
    print(f"\n  Ideal (linear)    ----")
    print(f"  Amdahl (P={p_estimated:.2%}) ████")
    print(f"  Măsurat           ████\n")
    
    for t, s in zip(threads, speedups):
        # Bara pentru speedup măsurat
        bar_len = int(s / scale * width)
        ideal_len = int(t / scale * width)
        amdahl_s = amdahl_speedup(t, p_estimated)
        amdahl_len = int(amdahl_s / scale * width)
        
        print(f"  {t:2d} threads: ", end="")
        
        # Afișare bară
        bar = ""
        for i in range(width):
            if i < bar_len:
                bar += "█"
            elif i < amdahl_len:
                bar += "▒"
            elif i < ideal_len:
                bar += "░"
            else:
                bar += " "
        
        print(f"[{bar}] {s:.2f}×")
    
    print(f"\n  Limită Amdahl: {amdahl_speedup(float('inf'), p_estimated):.2f}×")
    print("=" * 60)


def generate_sample_csv(filename):
    """
    Generează un fișier CSV de exemplu cu date simulate.
    
    Args:
        filename: Numele fișierului de generat
    """
    import random
    
    print(f"📝 Generare fișier de exemplu: {filename}")
    
    # Simulare date realiste
    base_time = 2.5  # Timp secvențial
    p = 0.92  # Fracțiune paralelizabilă
    
    with open(filename, 'w') as f:
        f.write("threads,time_seconds,speedup,efficiency\n")
        
        for n in [1, 2, 4, 8]:
            # Timp teoretic cu variație
            theoretical_speedup = amdahl_speedup(n, p)
            noise = 1 + random.uniform(-0.05, 0.05)  # ±5% variație
            actual_speedup = theoretical_speedup * noise
            
            time_n = base_time / actual_speedup
            efficiency = actual_speedup / n * 100
            
            f.write(f"{n},{time_n:.6f},{actual_speedup:.4f},{efficiency:.2f}\n")
    
    print(f"✓ Fișier generat cu succes")


def main():
    """Funcția principală."""
    print("\n" + "=" * 60)
    print(" VIZUALIZARE REZULTATE BENCHMARK - SĂPTĂMÂNA 20")
    print(" Programare Paralelă și Concurentă")
    print("=" * 60 + "\n")
    
    # Determinare fișier input
    if len(sys.argv) > 1:
        results_file = sys.argv[1]
    else:
        results_file = 'benchmark_results.csv'
    
    # Verificare existență fișier
    if not os.path.exists(results_file):
        print(f"⚠️  Fișierul {results_file} nu există.")
        print("   Generez fișier de exemplu...")
        generate_sample_csv(results_file)
    
    # Încărcare rezultate
    print(f"📊 Încărcare rezultate din: {results_file}")
    results = load_results(results_file)
    
    if results is None:
        print("❌ Eroare la încărcarea datelor.")
        return 1
    
    print(f"✓ Date încărcate: {len(results['threads'])} puncte de măsurare")
    
    # Estimare P
    p_estimated = estimate_parallel_fraction(results['threads'], results['speedups'])
    print(f"✓ Fracțiune paralelizabilă estimată: P = {p_estimated:.2%}")
    print(f"✓ Limită teoretică Amdahl: {amdahl_speedup(float('inf'), p_estimated):.2f}×")
    
    # Afișare ASCII (funcționează întotdeauna)
    print_ascii_chart(results, p_estimated)
    
    # Generare grafice (dacă matplotlib e disponibil)
    if HAS_MATPLOTLIB:
        print("\n📈 Generare grafice...")
        output_dir = os.path.dirname(results_file) or '.'
        
        plot_speedup(results, p_estimated, output_dir)
        plot_efficiency(results, output_dir)
        plot_execution_time(results, output_dir)
        plot_combined(results, p_estimated, output_dir)
        
        print("\n✅ Toate graficele au fost generate cu succes!")
    else:
        print("\n⚠️  Pentru grafice avansate, instalați matplotlib:")
        print("   pip install matplotlib")
    
    return 0


if __name__ == '__main__':
    sys.exit(main())
