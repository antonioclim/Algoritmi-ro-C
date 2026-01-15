# Tema Săptămânii 09: Arbori AVL

## 📋 Informații Generale

- **Termen limită:** Sfârșitul săptămânii 10
- **Punctaj total:** 100 puncte (10% din nota finală)
- **Limbaj:** C (standard C11)
- **Compilator:** GCC cu `-Wall -Wextra -std=c11`

---

## 📝 Tema 1: Dicționar cu Arbore AVL (50 puncte)

### Descriere

Implementați un dicționar (structură cheie-valoare) folosind un arbore AVL. Dicționarul va stoca perechi de forma `(cuvânt, definiție)` unde `cuvânt` este cheia de căutare și `definiție` este valoarea asociată.

### Cerințe

1. **Structura nodului** (5p)
   - Definește structura `DictEntry` cu câmpurile:
     - `char word[64]` - cuvântul (cheia)
     - `char definition[256]` - definiția
     - `DictEntry *left, *right` - pointeri către copii
     - `int height` - înălțimea nodului

2. **Funcția de comparare** (5p)
   - Implementează compararea alfabetică folosind `strcmp()`

3. **Inserare cu auto-echilibrare** (15p)
   - `DictEntry* dict_insert(DictEntry *root, const char *word, const char *def)`
   - Suportă actualizarea definiției dacă cuvântul există deja

4. **Căutare** (10p)
   - `const char* dict_lookup(DictEntry *root, const char *word)`
   - Returnează definiția sau NULL dacă nu există

5. **Ștergere cu reechilibrare** (10p)
   - `DictEntry* dict_delete(DictEntry *root, const char *word)`

6. **Afișare alfabetică** (5p)
   - Parcurgere inorder pentru afișare în ordine alfabetică

### Exemplu de Utilizare

```c
DictEntry *dict = NULL;

dict = dict_insert(dict, "algoritm", "Secvență finită de instrucțiuni");
dict = dict_insert(dict, "arbore", "Structură de date ierarhică");
dict = dict_insert(dict, "recursie", "Tehnică de programare");

const char *def = dict_lookup(dict, "algoritm");
printf("algoritm: %s\n", def);

dict_print_all(dict);  // Afișare alfabetică
```

### Fișier: `homework1_dict.c`

---

## 📝 Tema 2: Arbore AVL Generic cu Funcții Callback (50 puncte)

### Descriere

Implementați un arbore AVL generic care poate stoca orice tip de date folosind pointeri `void*` și funcții callback pentru comparare și afișare.

### Cerințe

1. **Structura generică** (10p)
   ```c
   typedef int (*CompareFunc)(const void*, const void*);
   typedef void (*PrintFunc)(const void*);
   typedef void (*FreeFunc)(void*);
   
   typedef struct GenericAVL {
       void *data;
       struct GenericAVL *left, *right;
       int height;
   } GenericAVL;
   
   typedef struct {
       GenericAVL *root;
       CompareFunc compare;
       PrintFunc print;
       FreeFunc free_data;
   } AVLTree;
   ```

2. **Inițializare și distrugere** (5p)
   - `AVLTree* avl_create(CompareFunc cmp, PrintFunc print, FreeFunc free_fn)`
   - `void avl_destroy(AVLTree *tree)`

3. **Operații cu callback-uri** (20p)
   - `bool avl_insert(AVLTree *tree, void *data)`
   - `void* avl_search(AVLTree *tree, const void *key)`
   - `bool avl_delete(AVLTree *tree, const void *key)`

4. **Demonstrație cu multiple tipuri** (15p)
   - Arbore de întregi
   - Arbore de structuri Student (nume, medie)
   - Arbore de șiruri de caractere

### Exemplu de Utilizare

```c
// Comparator pentru întregi
int compare_int(const void *a, const void *b) {
    return *(int*)a - *(int*)b;
}

// Printer pentru întregi
void print_int(const void *data) {
    printf("%d", *(int*)data);
}

AVLTree *tree = avl_create(compare_int, print_int, free);

int *val = malloc(sizeof(int));
*val = 42;
avl_insert(tree, val);

avl_traverse_inorder(tree);  // Parcurgere cu callback

avl_destroy(tree);
```

### Fișier: `homework2_generic.c`

---

## 📊 Criterii de Evaluare

| Criteriu | Puncte |
|----------|--------|
| Corectitudine funcțională | 40 |
| Implementarea corectă a rotațiilor AVL | 25 |
| Gestionarea cazurilor limită | 15 |
| Calitatea codului (lizibilitate, comentarii) | 10 |
| Fără warning-uri la compilare | 10 |

### Penalizări

- **-10p:** Warning-uri la compilare
- **-20p:** Memory leaks (verificat cu Valgrind)
- **-30p:** Crash pe input valid
- **-50p:** Plagiat (notă 0 pe întreaga temă)

---

## 📤 Trimitere

1. Creați un director `Nume_Prenume_Grupa_S09`
2. Includeți fișierele:
   - `homework1_dict.c`
   - `homework2_generic.c`
   - `Makefile` (opțional)
   - `README.md` cu instrucțiuni de compilare și rulare
3. Arhivați directorul în format `.zip`
4. Încărcați pe platforma de e-learning până la termenul limită

---

## 💡 Sfaturi

1. **Testați incremental** - Implementați și testați fiecare funcție separat
2. **Folosiți Valgrind** - `valgrind --leak-check=full ./homework1`
3. **Desenați pe hârtie** - Vizualizați rotațiile înainte de implementare
4. **Cazuri de test:**
   - Inserare în ordine crescătoare (provoacă rotații RR)
   - Inserare în ordine descrescătoare (provoacă rotații LL)
   - Inserare alternată (provoacă rotații LR/RL)
   - Ștergere rădăcină
   - Arbore cu un singur element
   - Arbore gol

5. **Debugging:**
   - Adăugați funcție de afișare după fiecare operație
   - Verificați proprietatea AVL cu o funcție `is_valid_avl()`
   - Folosiți GDB pentru debugging pas cu pas

---

## 📚 Resurse Recomandate

- Cormen et al. - *Introduction to Algorithms*, Capitolul 13
- [Visualgo - AVL Tree](https://visualgo.net/en/bst)
- [GeeksforGeeks - AVL Tree](https://www.geeksforgeeks.org/avl-tree-set-1-insertion/)
