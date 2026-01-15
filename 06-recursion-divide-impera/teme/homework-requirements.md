# Teme Săptămâna 6: Cozi (Queues)

## 📋 Informații Generale

- **Termen limită:** Sfârșitul săptămânii 7
- **Punctaj:** 100 puncte (10% din nota finală)
- **Limbaj:** C (standard C11)
- **Compilator:** GCC cu flag-urile `-Wall -Wextra -std=c11`

---

## 📝 Tema 1: Sistem de Gestionare a Clienților Bancari (50 puncte)

### Descriere

Implementați un sistem de gestionare a cozilor de așteptare pentru o bancă cu multiple ghișee. Sistemul trebuie să permită:

1. Adăugarea clienților în coada de așteptare
2. Alocarea clienților la ghișee disponibile
3. Gestionarea priorităților (clienți VIP, persoane în vârstă)
4. Generarea de statistici

### Cerințe Funcționale

#### Structura Client (10p)
```c
typedef struct {
    int ticket_number;           // Număr bon ordine
    char name[50];               // Nume client
    ClientType type;             // REGULAR, VIP, ELDERLY
    ServiceType service;         // DEPOSIT, WITHDRAWAL, TRANSFER, OTHER
    time_t arrival_time;         // Momentul sosirii
} Client;
```

#### Structura BankQueue (10p)
- Folosiți un **circular buffer** pentru coada principală
- Dimensiune: 50 de clienți
- Implementați toate operațiile cu complexitate O(1)

#### Funcții Obligatorii (20p)

1. `void bank_queue_init(BankQueue *bq)` - Inițializare (2p)
2. `int bank_add_client(BankQueue *bq, const char *name, ClientType type, ServiceType service)` - Adăugare client, returnează numărul bonului (4p)
3. `bool bank_call_next(BankQueue *bq, int window_id, Client *result)` - Cheamă următorul client (ținând cont de priorități) (6p)
4. `int bank_clients_waiting(const BankQueue *bq)` - Număr clienți în așteptare (2p)
5. `double bank_average_wait_time(const BankQueue *bq)` - Timp mediu de așteptare (3p)
6. `void bank_print_status(const BankQueue *bq)` - Afișare status (3p)

#### Logica de Prioritate (10p)
- Clienții VIP au prioritate maximă
- Persoanele în vârstă (ELDERLY) au prioritate înaintea clienților regulari
- În cadrul aceleiași priorități, se respectă ordinea sosirii (FIFO)

### Exemplu de Utilizare

```c
BankQueue bq;
bank_queue_init(&bq);

// Adăugăm clienți
bank_add_client(&bq, "Ion Popescu", REGULAR, DEPOSIT);      // Bon #1
bank_add_client(&bq, "Maria VIP", VIP, TRANSFER);           // Bon #2
bank_add_client(&bq, "Bunica Elena", ELDERLY, WITHDRAWAL);  // Bon #3
bank_add_client(&bq, "Andrei", REGULAR, OTHER);             // Bon #4

// Chemăm la ghișeul 1
Client c;
bank_call_next(&bq, 1, &c);  // → Maria VIP (VIP priority)
bank_call_next(&bq, 2, &c);  // → Bunica Elena (ELDERLY priority)
bank_call_next(&bq, 1, &c);  // → Ion Popescu (REGULAR, primul sosit)
```

### Fișierul de Ieșire: `homework1_bank.c`

---

## 📝 Tema 2: Simulator de Rețea cu Buffere de Pachete (50 puncte)

### Descriere

Implementați un simulator simplificat de router de rețea care procesează pachete de date folosind cozi cu buffere limitate. Routerul trebuie să gestioneze:

1. Primirea pachetelor pe o interfață de intrare
2. Stocarea în buffer (circular queue)
3. Transmiterea pe interfața de ieșire
4. Detectarea și raportarea pachetelor pierdute (buffer overflow)

### Cerințe Funcționale

#### Structura Packet (10p)
```c
typedef struct {
    uint32_t source_ip;          // IP sursă (format: 0xC0A80001 = 192.168.0.1)
    uint32_t dest_ip;            // IP destinație
    uint16_t source_port;        // Port sursă
    uint16_t dest_port;          // Port destinație
    uint8_t protocol;            // TCP=6, UDP=17, ICMP=1
    uint16_t size;               // Dimensiune în bytes
    uint8_t ttl;                 // Time To Live
    uint32_t sequence;           // Număr secvență (pentru identificare)
} Packet;
```

#### Structura RouterBuffer (10p)
```c
typedef struct {
    Packet buffer[BUFFER_SIZE];  // BUFFER_SIZE = 100
    int front;
    int rear;
    int count;
    
    // Statistici
    uint64_t packets_received;
    uint64_t packets_forwarded;
    uint64_t packets_dropped;
    uint64_t bytes_total;
} RouterBuffer;
```

#### Funcții Obligatorii (20p)

1. `void router_init(RouterBuffer *rb)` - Inițializare (2p)
2. `bool router_receive_packet(RouterBuffer *rb, Packet pkt)` - Primește pachet, returnează false dacă buffer plin (5p)
3. `bool router_forward_packet(RouterBuffer *rb, Packet *pkt)` - Transmite următorul pachet (3p)
4. `int router_buffer_usage(const RouterBuffer *rb)` - Procentaj ocupare buffer (2p)
5. `void router_print_stats(const RouterBuffer *rb)` - Statistici complete (4p)
6. `double router_drop_rate(const RouterBuffer *rb)` - Rata de pierdere (pachete pierdute / primite) (2p)
7. `void router_flush(RouterBuffer *rb)` - Golește bufferul și resetează statisticile (2p)

#### Simulare Trafic (10p)

Implementați funcția `simulate_network_traffic()` care:
1. Generează pachete aleatorii (surse/destinații diferite)
2. Trimite pachete cu o rată configurabilă
3. Procesează pachete cu o altă rată
4. Simulează congestia (când primirea > procesarea)
5. Afișează statistici la fiecare 100 de pachete

### Exemplu de Output

```
═══ Router Statistics ═══
  Packets received:  1000
  Packets forwarded:  950
  Packets dropped:     50
  Drop rate:          5.00%
  Buffer usage:       73%
  Total bytes:        1.2 MB
```

### Fișierul de Ieșire: `homework2_network.c`

---

## 📊 Criterii de Evaluare

| Criteriu | Puncte |
|----------|--------|
| Corectitudine funcțională | 40 |
| Utilizarea corectă a conceptelor de queue | 25 |
| Gestionarea cazurilor limită (buffer plin/gol) | 15 |
| Calitatea codului (stil, comentarii) | 10 |
| Compilare fără warnings | 10 |

### Penalizări

- **-10p**: Warning-uri la compilare
- **-20p**: Memory leaks detectate cu Valgrind
- **-30p**: Crash pe input valid
- **-50p**: Plagiat (notă 0 la laborator)

---

## 📤 Modalitate de Predare

1. Creați fișierele `homework1_bank.c` și `homework2_network.c`
2. Verificați că se compilează fără erori:
   ```bash
   gcc -Wall -Wextra -std=c11 -o homework1 homework1_bank.c
   gcc -Wall -Wextra -std=c11 -o homework2 homework2_network.c
   ```
3. Verificați cu Valgrind:
   ```bash
   valgrind --leak-check=full ./homework1
   valgrind --leak-check=full ./homework2
   ```
4. Încărcați pe platforma cursului până la termenul limită

---

## 💡 Sfaturi

1. **Începeți cu structurile de date** - asigurați-vă că înțelegeți bine circular buffer înainte de a implementa logica complexă.

2. **Testați incremental** - testați fiecare funcție separat înainte de integrare.

3. **Folosiți constante** - evitați "magic numbers", definiți constante pentru BUFFER_SIZE, etc.

4. **Documentați codul** - comentarii pentru funcții și pentru logica non-trivială.

5. **Gestionați edge cases** - ce se întâmplă când bufferul e plin? când e gol? primul/ultimul element?

6. **Verificați memoria** - folosiți Valgrind înainte de predare pentru a detecta memory leaks.

---

*Succes! Dacă aveți întrebări, folosiți forumul cursului sau orele de consultații.*
