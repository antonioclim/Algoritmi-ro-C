# Săptămâna 13 - Teme: Algoritmi pe Grafuri

## 📋 Informații Generale

- **Termen limită:** Sfârșitul săptămânii 14
- **Punctaj:** 100 puncte (10% din nota finală)
- **Limbaj:** C (standard C11)
- **Compilator:** GCC cu flagurile `-Wall -Wextra -std=c11`

---

## 📝 Tema 1: Sistem de Navigație GPS (50 puncte)

### Descriere

Implementați un sistem de navigație care găsește drumul optim între două intersecții într-un oraș. Sistemul trebuie să citească harta orașului dintr-un fișier și să calculeze cel mai scurt drum folosind algoritmul Dijkstra.

### Context

Sunteți angajat la o companie de navigație GPS. Trebuie să implementați modulul de calcul al rutelor care:
1. Încarcă harta orașului (graf ponderat)
2. Găsește drumul cel mai scurt între două puncte
3. Oferă rute alternative când drumul principal este blocat

### Cerințe Funcționale

| Cerință | Puncte |
|---------|--------|
| Citirea grafului din fișier text | 8p |
| Implementarea listelor de adiacență | 8p |
| Implementarea MinHeap cu decrease-key | 12p |
| Algoritmul Dijkstra corect | 10p |
| Reconstrucția și afișarea drumului | 6p |
| Calcul rute alternative (exclude o muchie) | 6p |

### Format Fișier de Intrare

```
N M
u1 v1 w1 strada1
u2 v2 w2 strada2
...
Q
src1 dest1
src2 dest2
...
```

Unde:
- `N` = număr de intersecții (noduri)
- `M` = număr de străzi (muchii)
- `ui vi wi stradai` = strada bidirecțională între intersecțiile ui și vi cu distanța wi (în metri) și numele străzii
- `Q` = număr de interogări
- `srci desti` = găsește drumul de la intersecția srci la desti

### Exemplu de Utilizare

**Input (bucuresti.txt):**
```
6 8
0 1 500 Calea_Victoriei
0 2 800 Bd_Magheru
1 2 300 Str_Academiei
1 3 600 Bd_Carol
2 4 400 Bd_Bratianu
3 4 200 Str_Lipscani
3 5 500 Bd_Unirii
4 5 300 Splaiul_Independentei
3
0 5
0 4
2 3
```

**Output:**
```
=== Interogare 1: 0 -> 5 ===
Distanta minima: 1400 metri
Drum: 0 (Calea_Victoriei) -> 1 (Bd_Carol) -> 3 (Str_Lipscani) -> 4 (Splaiul_Independentei) -> 5
Ruta alternativa (fara Str_Lipscani): 1600 metri
Drum: 0 -> 1 -> 2 -> 4 -> 5

=== Interogare 2: 0 -> 4 ===
Distanta minima: 1100 metri
Drum: 0 (Bd_Magheru) -> 2 (Bd_Bratianu) -> 4
...
```

### Fișier: `homework1_gps.c`

### Structuri de Date Recomandate

```c
typedef struct Edge {
    int dest;
    int weight;
    char street_name[64];
    struct Edge* next;
} Edge;

typedef struct {
    int V;
    Edge** adj;
    char** node_names;  // Optional: nume intersecții
} CityMap;

typedef struct {
    int* data;
    int* dist;
    int* pos;
    int size;
    int capacity;
} MinHeap;
```

---

## 📝 Tema 2: Analiză Rețea Financiară (50 puncte)

### Descriere

Implementați un sistem de analiză pentru o rețea de tranzacții financiare între companii. Sistemul trebuie să detecteze oportunități de arbitraj (cicluri negative) folosind algoritmul Bellman-Ford.

### Context

Într-o piață de schimb valutar, un ciclu negativ reprezintă o oportunitate de arbitraj - o secvență de tranzacții care generează profit fără risc. Trebuie să:
1. Modelați ratele de schimb ca muchii într-un graf
2. Detectați cicluri negative (arbitraj)
3. Identificați toate companiile afectate de instabilitate

### Transformarea Ratelor

Pentru rata de schimb `R` de la valuta A la B:
- Ponderea muchiei = `-log(R)`
- Ciclu negativ în graf = arbitraj în piață

### Cerințe Funcționale

| Cerință | Puncte |
|---------|--------|
| Citirea rețelei din fișier | 6p |
| Reprezentarea cu listă de muchii | 6p |
| Algoritmul Bellman-Ford corect | 12p |
| Detectarea ciclurilor negative | 10p |
| Propagarea efectelor (BFS) | 8p |
| Afișarea raportului de analiză | 8p |

### Format Fișier de Intrare

```
N M
company1
company2
...
companyN
u1 v1 rate1
u2 v2 rate2
...
S
```

Unde:
- `N` = număr de companii/valute
- `M` = număr de rate de schimb
- `companyi` = numele companiei/valutei i
- `ui vi ratei` = rata de schimb de la ui la vi
- `S` = nodul de start pentru analiză

### Exemplu de Utilizare

**Input (forex.txt):**
```
4 5
EUR
USD
GBP
JPY
0 1 1.10
1 2 0.85
2 0 1.08
0 3 130.0
3 1 0.0077
0
```

**Output:**
```
=== Analiza Retea Financiara ===
Noduri: 4, Muchii: 5

Transformare -log(rate):
  EUR -> USD: -0.0953
  USD -> GBP: 0.1625
  GBP -> EUR: -0.0770
  EUR -> JPY: -4.8675
  JPY -> USD: 4.8675

=== Rezultate Bellman-Ford (start: EUR) ===

⚠️  DETECTAT CICLU NEGATIV!
Ciclu: EUR -> USD -> GBP -> EUR
Profit potential: 0.95%

Noduri afectate de instabilitate:
  - EUR (afectat direct)
  - USD (propagat)
  - GBP (propagat)

Noduri stabile:
  - JPY (distanta: -4.8675)

=== Recomandari ===
- Verificati tranzactiile in ciclul detectat
- Monitorizati nodurile afectate
```

### Fișier: `homework2_arbitrage.c`

### Structuri de Date Recomandate

```c
typedef struct {
    int src;
    int dest;
    double weight;  // -log(rate)
    double original_rate;
} FinancialEdge;

typedef struct {
    int V;
    int E;
    FinancialEdge* edges;
    char** company_names;
} FinancialNetwork;

typedef struct {
    double* dist;
    int* prev;
    bool* in_negative_cycle;
    bool has_negative_cycle;
} AnalysisResult;
```

---

## 📊 Criterii de Evaluare

| Criteriu | Puncte |
|----------|--------|
| Corectitudine funcțională | 40p |
| Utilizarea corectă a Dijkstra/Bellman-Ford | 25p |
| Tratarea cazurilor limită | 15p |
| Calitatea codului (claritate, comentarii) | 10p |
| Fără warning-uri la compilare | 10p |

### Penalizări

| Problemă | Penalizare |
|----------|------------|
| Warning-uri la compilare | -10p |
| Memory leaks (Valgrind) | -20p |
| Crash pe input valid | -30p |
| Plagiat | -50p (minim) |

---

## 📤 Submisie

### Structura Arhivei

```
Nume_Prenume_Grupa_ATP_S13.zip
├── homework1_gps.c
├── homework2_arbitrage.c
├── bucuresti.txt        (fișier test propriu pentru Tema 1)
├── forex.txt            (fișier test propriu pentru Tema 2)
└── README.txt           (explicații, decizii de design)
```

### Unde se trimite

- Platforma Moodle a cursului
- Deadline: Duminică, ora 23:59, săptămâna 14

---

## 💡 Sfaturi

1. **Testați incremental** - verificați fiecare componentă separat (heap, Dijkstra, citire fișier)

2. **Folosiți Valgrind** - `valgrind --leak-check=full ./homework1` pentru a detecta memory leaks

3. **Desenați grafurile** - pe hârtie, pentru a verifica manual rezultatele

4. **Tratați cazuri speciale:**
   - Graf deconectat (noduri inaccesibile)
   - Graf cu un singur nod
   - Rate de schimb = 1.0 (log = 0)

5. **Documentați deciziile** - explicați în README de ce ați ales anumite structuri de date

6. **Folosiți constante** - `#define INF INT_MAX` în loc de valori hardcodate

---

## 🔍 Verificare Automată

Temele vor fi testate automat cu:

```bash
# Compilare
gcc -Wall -Wextra -std=c11 -o hw1 homework1_gps.c -lm
gcc -Wall -Wextra -std=c11 -o hw2 homework2_arbitrage.c -lm

# Rulare teste
./hw1 < test1.txt > output1.txt
diff output1.txt expected1.txt

# Verificare memorie
valgrind --error-exitcode=1 --leak-check=full ./hw1 < test1.txt
```

**Succes!** 🎯
