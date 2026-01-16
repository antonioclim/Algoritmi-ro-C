# =============================================================================
# TEME SĂPTĂMÂNA 20: Programare Paralelă și Concurentă în C
# =============================================================================
# Academia de Studii Economice București - CSIE
# Algoritmi și Tehnici de Programare (ATP)
# Deadline: 2 săptămâni de la data laboratorului
# =============================================================================

## 📋 Informații Generale

| Aspect | Detalii |
|--------|---------|
| **Punctaj Total** | 100 puncte (50p × 2 teme) |
| **Limbaj** | C11 cu POSIX Threads |
| **Compilare** | `gcc -Wall -Wextra -std=c11 -pthread` |
| **Livrabile** | Cod sursă + Raport PDF |
| **Plagiat** | Toleranță zero - verificare automată |

---

## 🎯 TEMA 1: Thread Pool cu Futures (50 puncte)

### Descriere

Implementați un **thread pool** complet care suportă mecanismul de **futures** - 
o abstracție care permite trimiterea de task-uri asincrone și obținerea 
rezultatelor în mod sincron ulterior.

Un **future** reprezintă un rezultat care nu este încă disponibil, dar va fi 
calculat la un moment dat în viitor de către un thread din pool.

### Cerințe Funcționale

#### 1. Structura Thread Pool (15p)

```c
typedef struct {
    pthread_t *workers;           /* Array de thread-uri worker */
    size_t num_workers;           /* Numărul de workers */
    TaskQueue *queue;             /* Coada de task-uri */
    volatile bool shutdown;       /* Flag de închidere */
    pthread_mutex_t mutex;        /* Mutex pentru sincronizare */
    pthread_cond_t task_available; /* Condiție: task disponibil */
} ThreadPool;
```

Implementați:
- `threadpool_create(size_t num_workers)` - creează pool-ul
- `threadpool_shutdown(ThreadPool *pool)` - oprește grațios toate thread-urile
- `threadpool_destroy(ThreadPool *pool)` - eliberează resursele

#### 2. Mecanismul Future (20p)

```c
typedef void* (*TaskFunction)(void *arg);

typedef struct {
    TaskFunction func;            /* Funcția de executat */
    void *arg;                    /* Argumentul funcției */
    void *result;                 /* Rezultatul (după execuție) */
    bool completed;               /* Task-ul s-a terminat? */
    bool cancelled;               /* Task-ul a fost anulat? */
    pthread_mutex_t mutex;        /* Mutex pentru acest future */
    pthread_cond_t done;          /* Condiție: execuția completă */
} Future;
```

Implementați:
- `threadpool_submit(ThreadPool *pool, TaskFunction func, void *arg)` → `Future*`
- `future_get(Future *f)` → `void*` (blochează până la completare)
- `future_get_timeout(Future *f, unsigned int ms)` → `void*` sau `NULL`
- `future_cancel(Future *f)` → `bool` (încearcă anularea)
- `future_is_done(Future *f)` → `bool`
- `future_destroy(Future *f)`

#### 3. Cazuri Speciale (10p)

- **Shutdown grațios**: `threadpool_shutdown()` trebuie să:
  - Oprească acceptarea de noi task-uri
  - Permită finalizarea task-urilor în execuție
  - Wake-up pe toate thread-urile care așteaptă

- **Future cancel**: Dacă task-ul nu a început încă:
  - Marchează-l ca anulat
  - `future_get()` returnează `NULL`
  - Dacă task-ul a început deja, nu poate fi anulat

- **Memory safety**: Fără memory leaks (verificabil cu Valgrind)

#### 4. Program Demonstrativ (5p)

Creați un program `homework1.c` care:
1. Creează un pool cu 4 workers
2. Trimite 20 de task-uri (calcule CPU-intensive: ex. numere prime)
3. Demonstrează `future_get()` și `future_get_timeout()`
4. Demonstrează `future_cancel()` pe task-uri pending
5. Face shutdown grațios și afișează statistici

### Exemplu de Utilizare

```c
/* Task care calculează suma 1..n */
void* compute_sum(void *arg) {
    long n = *(long*)arg;
    long sum = 0;
    for (long i = 1; i <= n; i++) {
        sum += i;
    }
    long *result = malloc(sizeof(long));
    *result = sum;
    return result;
}

int main(void) {
    ThreadPool *pool = threadpool_create(4);
    
    long n = 1000000;
    Future *f = threadpool_submit(pool, compute_sum, &n);
    
    printf("Așteptăm rezultatul...\n");
    long *result = (long*)future_get(f);
    printf("Suma 1..%ld = %ld\n", n, *result);
    
    free(result);
    future_destroy(f);
    threadpool_shutdown(pool);
    threadpool_destroy(pool);
    
    return 0;
}
```

### Barem Detaliat

| Cerință | Puncte | Criterii |
|---------|--------|----------|
| Thread pool funcțional | 15p | Creare/shutdown/destroy corecte |
| Future submit/get | 10p | Funcționează corect |
| Future timeout | 5p | Returnează după timeout |
| Future cancel | 5p | Anulează task-uri pending |
| Thread safety | 5p | Fără race conditions |
| Memory safety | 5p | Fără leaks (Valgrind clean) |
| Program demonstrativ | 5p | Demonstrează toate funcțiile |

### Restricții

- NU folosiți biblioteci externe (doar pthread, standard C11)
- Coada de task-uri trebuie implementată de voi (nu `std::queue`)
- Futures trebuie să fie alocate dinamic

---

## 🎯 TEMA 2: Map-Reduce în C (50 puncte)

### Descriere

Implementați un framework **Map-Reduce** simplificat în C pentru procesarea 
paralelă a fișierelor text mari. Aplicația va număra frecvența cuvintelor 
(word count) într-un set de fișiere.

### Arhitectura Map-Reduce

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           MAP-REDUCE PIPELINE                               │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   INPUT FILES          MAP PHASE              SHUFFLE           REDUCE      │
│   ┌─────────┐                                                               │
│   │ file1   │──┐      ┌─────────┐                              ┌─────────┐  │
│   └─────────┘  ├──────│ Mapper 1│────┐    Partition by         │Reducer 1│  │
│   ┌─────────┐  │      └─────────┘    │    hash(key) % R        └────┬────┘  │
│   │ file2   │──┤      ┌─────────┐    │    ┌─────────────┐           │       │
│   └─────────┘  ├──────│ Mapper 2│────┼───▶│  Bucket 0   │───────────┤       │
│   ┌─────────┐  │      └─────────┘    │    ├─────────────┤           │       │
│   │ file3   │──┤      ┌─────────┐    │    │  Bucket 1   │───────────┤       │
│   └─────────┘  │      │ Mapper 3│────┤    ├─────────────┤      ┌────┴────┐  │
│       ...     ─┤      └─────────┘    │    │     ...     │      │Reducer 2│  │
│   ┌─────────┐  │      ┌─────────┐    │    ├─────────────┤      └────┬────┘  │
│   │ fileN   │──┘      │ Mapper M│────┘    │  Bucket R-1 │───────────┤       │
│   └─────────┘         └─────────┘         └─────────────┘           │       │
│                                                                     ▼       │
│                                                              MERGED OUTPUT  │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### Cerințe Funcționale

#### 1. Structuri de Date (10p)

```c
/* Pereche cheie-valoare */
typedef struct {
    char *key;
    int value;
} KeyValue;

/* Lista de perechi emise de un mapper */
typedef struct {
    KeyValue *pairs;
    size_t count;
    size_t capacity;
    pthread_mutex_t mutex;
} KeyValueList;

/* Configurația Map-Reduce */
typedef struct {
    char **input_files;
    size_t num_files;
    size_t num_mappers;
    size_t num_reducers;
    KeyValueList **intermediate;  /* [num_reducers] liste intermediare */
    KeyValueList *output;
} MapReduceConfig;
```

#### 2. Faza Map (15p)

Fiecare **mapper** procesează unul sau mai multe fișiere:
1. Citește fișierul linie cu linie
2. Extrage cuvintele (separate prin spații/punctuație)
3. Normalizează (lowercase, elimină caractere speciale)
4. Emite perechi `(cuvânt, 1)` în bucket-ul corespunzător

```c
void map_function(const char *line, KeyValueList *output);
void* mapper_thread(void *arg);
```

**Partition function** pentru shuffle:
```c
int partition(const char *key, int num_reducers) {
    unsigned int hash = 0;
    while (*key) {
        hash = hash * 31 + *key++;
    }
    return hash % num_reducers;
}
```

#### 3. Faza Reduce (15p)

Fiecare **reducer** procesează un bucket:
1. Primește toate perechile din bucket-ul său
2. Grupează după cheie
3. Sumează valorile pentru fiecare cheie
4. Scrie rezultatul în output

```c
void reduce_function(const char *key, int *values, size_t count, KeyValueList *output);
void* reducer_thread(void *arg);
```

#### 4. Orchestrare și Sincronizare (10p)

```c
/* Funcția principală */
void map_reduce(MapReduceConfig *config) {
    /* 1. Creează și pornește mapperii */
    /* 2. Așteaptă terminarea mapperilor (barrier) */
    /* 3. Creează și pornește reducerii */
    /* 4. Așteaptă terminarea reducerilor */
    /* 5. Combină output-urile */
}
```

**Cerințe sincronizare:**
- Mapperii pot rula în paralel
- Reducerii încep DOAR după terminarea TUTUROR mapperilor
- Accesul la bucket-urile intermediare trebuie sincronizat
- Folosiți bariere sau join-uri pentru coordonare

### Input/Output

**Input**: Director cu fișiere `.txt`

```
data/
├── file1.txt    "Ana are mere și pere"
├── file2.txt    "Ion are mere"
└── file3.txt    "Maria are pere și caise"
```

**Output**: Fișier cu frecvențe sortate descrescător

```
are: 3
mere: 2
pere: 2
și: 2
ana: 1
caise: 1
ion: 1
maria: 1
```

### Program Demonstrativ

```c
int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <input_dir> <num_mappers> <num_reducers>\n", argv[0]);
        return 1;
    }
    
    MapReduceConfig config;
    config.input_files = list_files(argv[1], &config.num_files);
    config.num_mappers = atoi(argv[2]);
    config.num_reducers = atoi(argv[3]);
    
    printf("Map-Reduce: %zu fișiere, %zu mapperi, %zu reduceri\n",
           config.num_files, config.num_mappers, config.num_reducers);
    
    double start = get_time();
    map_reduce(&config);
    double elapsed = get_time() - start;
    
    printf("Procesare completă în %.3f secunde\n", elapsed);
    print_top_words(&config.output, 20);
    
    cleanup(&config);
    return 0;
}
```

### Barem Detaliat

| Cerință | Puncte | Criterii |
|---------|--------|----------|
| Structuri de date | 5p | Corect definite și inițializate |
| Map function | 10p | Extrage și emite corect |
| Partition/Shuffle | 5p | Distribuție uniformă |
| Reduce function | 10p | Agregare corectă |
| Sincronizare | 10p | Barrier între faze, mutex pe buckets |
| Benchmark | 5p | Comparație threads: 1, 2, 4, 8 |
| Memory safety | 5p | Fără leaks |

### Testare

Generați fișiere de test cu:
```bash
# Generează 10 fișiere cu ~1000 cuvinte fiecare
for i in $(seq 1 10); do
    fortune | fmt -w 80 > "data/test_$i.txt"
done
```

Sau folosiți fișierele din `/usr/share/dict/` sau texte din Project Gutenberg.

### Restricții

- NU folosiți biblioteci externe de Map-Reduce
- Fișierele trebuie procesate în paralel (nu secvențial)
- Minim 4 mapperi și 2 reduceri în demo

---

## 📝 Format Livrare

### Structura Arhivei

```
Tema_Sapt20_NumePrenume.zip
├── tema1/
│   ├── threadpool.h
│   ├── threadpool.c
│   ├── future.h
│   ├── future.c
│   ├── homework1.c
│   └── Makefile
├── tema2/
│   ├── mapreduce.h
│   ├── mapreduce.c
│   ├── homework2.c
│   ├── data/           (fișiere de test)
│   └── Makefile
└── Raport_Sapt20.pdf
```

### Cerințe Raport (5-7 pagini)

1. **Descriere implementare** - arhitectura aleasă, structuri de date
2. **Diagrame** - flux de date, sincronizare între thread-uri
3. **Probleme întâmpinate** - race conditions, deadlocks rezolvate
4. **Benchmark-uri** - grafice speedup vs. număr thread-uri
5. **Concluzii** - ce ați învățat, posibile îmbunătățiri

### Criterii de Evaluare

| Criteriu | Pondere |
|----------|---------|
| Corectitudine funcțională | 50% |
| Calitatea codului | 20% |
| Eficiența paralelizării | 15% |
| Raport și documentare | 15% |

---

## 🔧 Resurse Utile

### Documentație
- **POSIX Threads**: `man pthread_create`, `man pthread_mutex_init`
- **C11 Standard**: [cppreference.com/w/c](https://en.cppreference.com/w/c)

### Cărți Recomandate
- **Butenhof** - *Programming with POSIX Threads*
- **Herlihy & Shavit** - *The Art of Multiprocessor Programming*

### Instrumente Debug
```bash
# Verificare memory leaks
valgrind --leak-check=full ./homework1

# Verificare race conditions
valgrind --tool=helgrind ./homework1

# ThreadSanitizer (gcc)
gcc -fsanitize=thread -g -o homework1 homework1.c -lpthread
```

---

## ⚠️ Atenționări

1. **Plagiat**: Codul copiat = 0 puncte + raportare la decanat
2. **Deadline**: Întârzierile se penalizează cu 10p/zi
3. **Compilare**: Codul care nu compilează = 0 puncte
4. **Race conditions**: Programele cu comportament nedeterminist vor fi penalizate

---

*Material pentru cursul "Algoritmi și Tehnici de Programare" (ATP)*
*Academia de Studii Economice din București - CSIE*
*Actualizat: Ianuarie 2026*
