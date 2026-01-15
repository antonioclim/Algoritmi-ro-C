# Teme Săptămâna 14: Recapitulare Avansată și Optimizare

## 📋 Informații Generale

- **Termen limită:** Sfârșitul săptămânii 15 (ultima zi de curs)
- **Punctaj:** 100 puncte (10% din nota finală)
- **Limbaj:** C (standard C11)
- **Compilator:** GCC cu `-Wall -Wextra -std=c11`

---

## 📝 Tema 1: Sistem de Gestiune Bibliotecă (50 puncte)

### Descriere

Implementați un sistem complet de gestiune a unei biblioteci universitare care integrează toate structurile de date studiate în semestru. Sistemul trebuie să gestioneze cărți, cititori și împrumuturi.

### Cerințe Funcționale

#### Structuri de Date Obligatorii (20p)

1. **Hash Table pentru Cărți** (5p)
   - Cheie: ISBN (string de 13 caractere)
   - Valoare: struct Book (titlu, autor, an, exemplare disponibile)
   - Funcția hash: djb2 sau similar

2. **AVL Tree pentru Cititori** (8p)
   - Ordonare după ID cititor
   - Operații: insert, delete, search, traversare
   - Menținerea echilibrului la fiecare operație

3. **Priority Queue pentru Rezervări** (7p)
   - Min-heap bazat pe data rezervării
   - Când o carte devine disponibilă, primul din coadă primește notificare

#### Operații Obligatorii (20p)

```c
// Gestiune cărți
bool add_book(Library *lib, const char *isbn, const char *title, 
              const char *author, int year, int copies);
Book *find_book(Library *lib, const char *isbn);
bool remove_book(Library *lib, const char *isbn);

// Gestiune cititori
bool register_reader(Library *lib, int reader_id, const char *name,
                     const char *faculty);
Reader *find_reader(Library *lib, int reader_id);
bool unregister_reader(Library *lib, int reader_id);

// Împrumuturi
bool borrow_book(Library *lib, int reader_id, const char *isbn);
bool return_book(Library *lib, int reader_id, const char *isbn);
bool reserve_book(Library *lib, int reader_id, const char *isbn);

// Rapoarte
void list_borrowed_by_reader(Library *lib, int reader_id);
void list_overdue_books(Library *lib, int days);
void list_popular_books(Library *lib, int top_n);
```

#### Persistența Datelor (10p)

- `save_library(Library *lib, const char *filename)` - salvare în fișier binar
- `load_library(Library **lib, const char *filename)` - încărcare din fișier

### Exemplu Utilizare

```c
Library *lib = create_library();

// Adăugare cărți
add_book(lib, "9789731032771", "Ion", "Liviu Rebreanu", 1920, 3);
add_book(lib, "9789731031234", "Enigma Otiliei", "G. Calinescu", 1938, 2);

// Înregistrare cititori
register_reader(lib, 1001, "Ion Popescu", "Informatica");
register_reader(lib, 1002, "Maria Ionescu", "Economie");

// Împrumut
borrow_book(lib, 1001, "9789731032771");

// Raport
list_borrowed_by_reader(lib, 1001);
// Output: Ion Popescu a împrumutat: Ion (Liviu Rebreanu)

free_library(lib);
```

### Fișier: `homework1_biblioteca.c`

---

## 📝 Tema 2: Sistem de Navigație GPS (50 puncte)

### Descriere

Implementați un sistem de navigație care găsește ruta optimă între două puncte pe o hartă reprezentată ca graf ponderat. Sistemul trebuie să ia în considerare distanța, timpul estimat și condițiile de trafic.

### Cerințe Funcționale

#### Structura Grafului (15p)

```c
typedef struct {
    int destination;        // ID-ul nodului destinație
    double distance;        // Distanța în km
    double base_time;       // Timpul de bază în minute
    double traffic_factor;  // Factor de trafic (1.0 = normal, 2.0 = aglomerat)
    struct Edge *next;
} Edge;

typedef struct {
    int id;
    char name[64];          // Numele locației
    double lat, lon;        // Coordonate GPS
    Edge *edges;            // Lista de muchii
} Location;

typedef struct {
    Location *locations;
    int location_count;
    int edge_count;
    HashTable *name_map;    // Căutare rapidă după nume
} Map;
```

#### Algoritmi de Rutare (25p)

1. **Dijkstra pentru Distanță Minimă** (10p)
   ```c
   Path *find_shortest_path(Map *map, int start, int end);
   ```

2. **Dijkstra Modificat pentru Timp Minim** (8p)
   ```c
   Path *find_fastest_path(Map *map, int start, int end);
   // Consideră: timp = distanță / viteză * traffic_factor
   ```

3. **A* pentru Căutare Eficientă** (7p)
   ```c
   Path *find_path_astar(Map *map, int start, int end);
   // Euristică: distanța în linie dreaptă (Haversine)
   ```

#### Funcții Auxiliare (10p)

```c
// Încărcare hartă din fișier
Map *load_map(const char *filename);

// Actualizare trafic în timp real
void update_traffic(Map *map, int from, int to, double factor);

// Afișare rută
void print_path(Map *map, Path *path);
// Output: Start -> Loc1 (2.5km, 5min) -> Loc2 (3.0km, 8min) -> End
//         Total: 5.5km, 13 minute

// Eliberare memorie
void free_path(Path *path);
void free_map(Map *map);
```

### Format Fișier Hartă

```
# Locații: ID NUME LAT LON
L 0 "Piata Unirii" 44.4268 26.1025
L 1 "Piata Victoriei" 44.4530 26.0850
L 2 "Gara de Nord" 44.4457 26.0701

# Muchii: FROM TO DISTANCE BASE_TIME TRAFFIC
E 0 1 3.5 12.0 1.2
E 0 2 4.2 15.0 1.0
E 1 2 2.8 10.0 1.5
```

### Exemplu Utilizare

```c
Map *bucuresti = load_map("data/bucuresti.map");

// Găsire rută cea mai scurtă
Path *shortest = find_shortest_path(bucuresti, 
    find_location_id(bucuresti, "Piata Unirii"),
    find_location_id(bucuresti, "Gara de Nord"));

printf("Ruta cea mai scurtă:\n");
print_path(bucuresti, shortest);

// Actualizare trafic
update_traffic(bucuresti, 0, 1, 2.5);  // Trafic intens

// Recalculare cu trafic
Path *fastest = find_fastest_path(bucuresti,
    find_location_id(bucuresti, "Piata Unirii"),
    find_location_id(bucuresti, "Gara de Nord"));

printf("\nRuta cea mai rapidă (cu trafic):\n");
print_path(bucuresti, fastest);

free_path(shortest);
free_path(fastest);
free_map(bucuresti);
```

### Fișier: `homework2_gps.c`

---

## 📊 Criterii de Evaluare

| Criteriu | Puncte |
|----------|--------|
| Corectitudine funcțională | 40 |
| Utilizarea corectă a structurilor de date | 25 |
| Gestionarea cazurilor limită | 15 |
| Calitatea codului și comentarii | 10 |
| Compilare fără avertismente | 10 |

### Penalizări

| Penalizare | Puncte |
|------------|--------|
| Avertismente la compilare | -10 |
| Memory leaks (Valgrind) | -20 |
| Crash pe input valid | -30 |
| Plagiat | -50 sau anulare |

---

## 📤 Instrucțiuni de Predare

1. **Format**: Un singur fișier `.c` pentru fiecare temă
2. **Nume fișiere**: 
   - `homework1_biblioteca_NumePrenume.c`
   - `homework2_gps_NumePrenume.c`
3. **Compilare**: Codul trebuie să compileze cu:
   ```bash
   gcc -Wall -Wextra -std=c11 -o hw1 homework1_biblioteca.c -lm
   gcc -Wall -Wextra -std=c11 -o hw2 homework2_gps.c -lm
   ```
4. **Testare**: Rulați Valgrind înainte de predare:
   ```bash
   valgrind --leak-check=full ./hw1
   valgrind --leak-check=full ./hw2
   ```
5. **Predare**: Prin platforma de e-learning a universității

---

## 💡 Sfaturi pentru Succes

1. **Începeți devreme** - Temele sunt complexe și necesită timp
2. **Testați incremental** - Nu așteptați să fie totul gata
3. **Folosiți Valgrind** frecvent - Memory leaks costă puncte
4. **Scrieți comentarii** - Ajută la depanare și evaluare
5. **Consultați materialele** - Reluați laboratoarele anterioare
6. **Cereți ajutor** - La laborator sau în forum
7. **Backup** - Salvați versiuni multiple ale codului

---

## 📚 Resurse Utile

- Laboratoarele 1-13 (toate structurile de date)
- Prezentarea Săptămâna 14 (recapitulare)
- Capitolele relevante din "Introduction to Algorithms" (CLRS)
- Documentația funcțiilor standard C: `man qsort`, `man bsearch`

---

*Succes la implementare! Această temă reprezintă o sinteză a întregului semestru.*
