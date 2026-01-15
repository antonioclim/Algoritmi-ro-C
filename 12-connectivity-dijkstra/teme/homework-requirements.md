# Tema Săptămâna 12: Grafuri - Noțiuni de Bază

## 📋 Informații Generale

- **Termen limită:** Sfârșitul săptămânii 13
- **Punctaj:** 100 puncte (10% din nota finală)
- **Limbaj:** C (standard C11)
- **Compilator:** GCC cu flag-urile `-Wall -Wextra -std=c11`

---

## 📝 Tema 1: Navigator Social (50 puncte)

### Descriere

Implementați un sistem de analiză a unei rețele sociale. Rețeaua este modelată ca un graf neorientat, unde nodurile reprezintă utilizatori, iar muchiile reprezintă relații de prietenie.

Sistemul trebuie să determine:
1. Gradul de separare între doi utilizatori (distanța minimă)
2. Utilizatorii care fac parte din aceeași comunitate (componentă conexă)
3. Cei mai influenți utilizatori (nodurile cu cel mai mare grad)

### Cerințe

1. **Citirea grafului din fișier** (10p)
   - Format: prima linie conține `N M` (noduri și muchii)
   - Următoarele M linii conțin câte o muchie `u v`
   - Implementați funcția `Graph* load_social_network(const char* filename)`

2. **Gradul de separare (BFS)** (15p)
   - Implementați funcția `int degree_of_separation(Graph* g, int user1, int user2)`
   - Returnează -1 dacă utilizatorii nu sunt conectați
   - Afișați și drumul efectiv între utilizatori

3. **Comunități (Componente conexe)** (15p)
   - Implementați funcția `int find_communities(Graph* g, int* community)`
   - Returnează numărul de comunități
   - Array-ul `community[i]` conține ID-ul comunității pentru utilizatorul i

4. **Utilizatori influenți** (10p)
   - Implementați funcția `void top_influencers(Graph* g, int k)`
   - Afișează primii k utilizatori cu cel mai mare grad (număr de prieteni)

### Exemplu de Utilizare

```c
Graph* network = load_social_network("social_network.txt");

printf("Grad de separare între 0 și 5: %d\n", 
       degree_of_separation(network, 0, 5));

int community[MAX_USERS];
int num_communities = find_communities(network, community);
printf("Număr de comunități: %d\n", num_communities);

printf("Top 3 influenceri:\n");
top_influencers(network, 3);

free_graph(network);
```

### Fișier: `homework1_social.c`

---

## 📝 Tema 2: Analizator de Dependențe (50 puncte)

### Descriere

Implementați un analizator de dependențe pentru un sistem de build (similar cu Make sau CMake). Dependențele sunt modelate ca un graf orientat, unde:
- Nodurile reprezintă module/fișiere
- Arcele reprezintă dependențe (A → B înseamnă "A depinde de B")

Sistemul trebuie să:
1. Detecteze dependențe circulare (cicluri)
2. Determine ordinea de compilare (sortare topologică)
3. Afle toate modulele de care depinde un modul dat

### Cerințe

1. **Citirea grafului de dependențe** (10p)
   - Format: prima linie `N M` (module și dependențe)
   - Următoarele M linii: `src dest` (src depinde de dest)
   - Implementați `Graph* load_dependencies(const char* filename)`

2. **Detectarea ciclurilor** (15p)
   - Implementați `bool has_circular_dependency(Graph* g)`
   - Dacă există ciclu, afișați modulele implicate

3. **Sortare topologică (DFS)** (15p)
   - Implementați `int* topological_sort(Graph* g)`
   - Returnează ordinea de compilare (NULL dacă există ciclu)
   - Demonstrează proprietatea: pentru orice arc (u,v), u apare după v

4. **Dependențe tranzitive** (10p)
   - Implementați `void transitive_dependencies(Graph* g, int module)`
   - Afișează TOATE modulele de care `module` depinde (direct și indirect)
   - Folosiți DFS sau BFS

### Exemplu de Utilizare

```c
Graph* deps = load_dependencies("dependencies.txt");

if (has_circular_dependency(deps)) {
    printf("EROARE: Dependență circulară detectată!\n");
} else {
    int* order = topological_sort(deps);
    printf("Ordine de compilare:\n");
    for (int i = 0; i < deps->vertices; i++) {
        printf("  %d. Modul %d\n", i + 1, order[i]);
    }
    free(order);
}

printf("\nModulul 0 depinde de:\n");
transitive_dependencies(deps, 0);

free_graph(deps);
```

### Fișier: `homework2_dependencies.c`

---

## 📊 Criterii de Evaluare

| Criteriu | Puncte |
|----------|--------|
| Corectitudine funcțională | 40 |
| Utilizare corectă a conceptelor de grafuri | 25 |
| Tratarea cazurilor limită | 15 |
| Calitatea codului (stil, comentarii) | 10 |
| Fără warning-uri la compilare | 10 |

### Penalizări

| Problemă | Penalizare |
|----------|------------|
| Warning-uri la compilare | -10p |
| Memory leaks (Valgrind) | -20p |
| Crash pe input valid | -30p |
| Plagiat | -50p + raportare |

---

## 📤 Submisie

1. Încărcați fișierele `homework1_social.c` și `homework2_dependencies.c` pe platforma de cursuri
2. Asigurați-vă că fișierele compilează fără erori:
   ```bash
   gcc -Wall -Wextra -std=c11 -o homework1 homework1_social.c
   gcc -Wall -Wextra -std=c11 -o homework2 homework2_dependencies.c
   ```
3. Verificați cu Valgrind:
   ```bash
   valgrind --leak-check=full ./homework1
   valgrind --leak-check=full ./homework2
   ```

---

## 💡 Sugestii

1. **Începeți simplu**: Implementați mai întâi citirea grafului și afișarea acestuia
2. **Testați incremental**: Testați fiecare funcție separat înainte de integrare
3. **Folosiți exemplele din laborator**: Adaptați codul din `example1.c`
4. **Atenție la memoria**: Eliberați toată memoria alocată
5. **Comentați codul**: Explicați algoritmii folosiți
6. **Gândiți cazurile limită**: Graf gol, un singur nod, graf neconectat

---

## 📚 Resurse

- Capitolul 22 din "Introduction to Algorithms" (Cormen et al.)
- [Visualgo - Graph Traversal](https://visualgo.net/en/dfsbfs)
- Materiale de curs Săptămâna 12
- Prezentarea `presentation-week12.html`
