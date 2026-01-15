# Provocări Extinse - Săptămâna 8

## 🚀 Provocări Avansate (Opționale)

Fiecare provocare rezolvată corect: **+10 puncte bonus**

---

## ⭐ Provocarea 1: BST Auto-Verificant (Dificultate: Medie)

### Descriere

Implementați un BST care își verifică automat validitatea după fiecare operație.

### Cerințe

```c
typedef struct {
    BSTNode *root;
    size_t size;
    int auto_verify;           /* Flag pentru verificare automată */
    int last_operation_valid;  /* Rezultatul ultimei verificări */
} ValidatedBST;

/* Funcții de implementat */
ValidatedBST* vbst_create(int auto_verify);
int vbst_insert(ValidatedBST *tree, int key);  /* Returnează 1 dacă valid după inserare */
int vbst_delete(ValidatedBST *tree, int key);
int vbst_verify(ValidatedBST *tree);           /* Verificare manuală completă */
void vbst_print_validation_report(ValidatedBST *tree);
```

### Criterii de Validare

1. Proprietatea BST (stânga < rădăcină < dreapta)
2. Niciun ciclu în arbore
3. Dimensiunea raportată corectă
4. Toate nodurile accesibile din rădăcină

### Puncte Bonus: +10

---

## ⭐ Provocarea 2: Range Query Eficient (Dificultate: Medie-Dificilă)

### Descriere

Implementați operații de interogare pe intervale pentru BST.

### Cerințe

```c
/* Găsește toate valorile în intervalul [low, high] */
/* Complexitate țintă: O(log n + k) unde k = numărul de rezultate */
int* bst_range_query(BST *tree, int low, int high, int *result_count);

/* Numără valorile în intervalul [low, high] fără a le enumera */
int bst_count_in_range(BST *tree, int low, int high);

/* Găsește al k-lea cel mai mic element */
BSTNode* bst_kth_smallest(BST *tree, int k);

/* Găsește rangul unui element (poziția în ordine sortată) */
int bst_rank(BST *tree, int key);
```

### Exemplu

```
Arbore: 50, 30, 70, 20, 40, 60, 80

range_query(25, 65) → [30, 40, 50, 60]
count_in_range(25, 65) → 4
kth_smallest(3) → 40
rank(60) → 5
```

### Puncte Bonus: +10

---

## ⭐ Provocarea 3: Serializare și Deserializare (Dificultate: Medie)

### Descriere

Implementați salvarea și restaurarea unui BST în/din format binar și text.

### Cerințe

```c
/* Serializare în format text (ușor de citit) */
int bst_serialize_text(BST *tree, const char *filename);
BST* bst_deserialize_text(const char *filename);

/* Serializare în format binar (eficient) */
int bst_serialize_binary(BST *tree, const char *filename);
BST* bst_deserialize_binary(const char *filename);

/* Serializare în format JSON */
char* bst_to_json(BST *tree);
BST* bst_from_json(const char *json);
```

### Format Text Sugerat (Preorder cu marcatori NULL)

```
50 30 20 # # 40 # # 70 60 # # 80 # #
```

Unde `#` marchează un nod NULL.

### Puncte Bonus: +10

---

## ⭐ Provocarea 4: BST cu Statistici de Ordin (Dificultate: Dificilă)

### Descriere

Extindeți BST-ul pentru a suporta statistici de ordin în O(log n).

### Cerințe

```c
typedef struct OrderStatNode {
    int key;
    int subtree_size;          /* Numărul de noduri în subarbore (inclusiv acest nod) */
    struct OrderStatNode *left;
    struct OrderStatNode *right;
} OrderStatNode;

/* Găsește al k-lea cel mai mic element în O(log n) */
OrderStatNode* os_select(OrderStatNode *root, int k);

/* Găsește rangul lui key în O(log n) */
int os_rank(OrderStatNode *root, int key);

/* Inserare cu menținerea subtree_size */
OrderStatNode* os_insert(OrderStatNode *root, int key);

/* Ștergere cu menținerea subtree_size */
OrderStatNode* os_delete(OrderStatNode *root, int key);
```

### Exemplu

```
         50(7)
        /     \
     30(3)    70(3)
     /   \    /   \
  20(1) 40(1) 60(1) 80(1)

os_select(root, 4) → 50 (al 4-lea cel mai mic)
os_rank(root, 60) → 5
```

### Puncte Bonus: +10

---

## ⭐ Provocarea 5: Transformare BST ↔ Sorted Doubly Linked List (Dificultate: Dificilă)

### Descriere

Convertiți un BST în listă dublu înlănțuită sortată **in-place** (fără alocare suplimentară de memorie).

### Cerințe

```c
typedef struct DLLNode {
    int key;
    struct DLLNode *prev;  /* Folosește pointerul left */
    struct DLLNode *next;  /* Folosește pointerul right */
} DLLNode;

/* Convertește BST în DLL sortată, in-place */
/* Returnează pointer către head-ul listei */
/* IMPORTANT: Nu alocați memorie nouă! Refolosiți nodurile existente */
DLLNode* bst_to_sorted_dll(BSTNode *root);

/* Convertește DLL sortată înapoi în BST echilibrat */
BSTNode* sorted_dll_to_balanced_bst(DLLNode *head, int length);
```

### Constrângeri

- **NU** alocați noduri noi pentru conversie
- Refolosiți pointerii `left` și `right` ca `prev` și `next`
- Complexitate: O(n) timp, O(log n) spațiu (pentru stack-ul de recursie)

### Exemplu

```
BST:
       50
      /  \
    30    70
   /  \
 20   40

→ DLL: 20 ↔ 30 ↔ 40 ↔ 50 ↔ 70

→ Balanced BST:
       40
      /  \
    30    50
   /        \
 20          70
```

### Puncte Bonus: +10

---

## 📊 Sistem de Puncte Bonus

| Provocări Completate | Total Bonus |
|----------------------|-------------|
| 1 | +10 puncte |
| 2 | +20 puncte |
| 3 | +30 puncte |
| 4 | +40 puncte |
| Toate 5 | +50 puncte + Badge "🏆 BST Master" |

---

## 📝 Instrucțiuni de Predare

### Format

Pentru fiecare provocare:
1. Fișier sursă: `challenge_X.c`
2. Comentarii explicative pentru algoritm
3. Funcție `main()` cu demonstrație/teste

### Verificare

```bash
# Compilare
gcc -Wall -Wextra -std=c11 -o challenge_X challenge_X.c

# Testare
./challenge_X

# Memory check
valgrind --leak-check=full ./challenge_X
```

---

## 💡 Sfaturi pentru Provocări

1. **Provocarea 1**: Gândiți-vă la invarianți - ce trebuie să fie adevărat mereu?

2. **Provocarea 2**: Optimizați evitând parcurgerea completă - opriți-vă când ieșiți din interval.

3. **Provocarea 3**: Preorder cu marcatori NULL este cel mai simplu pentru deserializare.

4. **Provocarea 4**: `subtree_size` trebuie actualizat la fiecare inserare/ștergere pe drumul către nod.

5. **Provocarea 5**: Parcurgeți inorder și legați nodurile pe măsură ce le vizitați. Pentru BST echilibrat, găsiți mijlocul listei recursiv.

---

*Succes și distracție plăcută cu provocările!*

*"The best way to learn algorithms is to implement them yourself."*
