# Provocări Extinse - Săptămâna 09: Arbori AVL

## 🚀 Provocări Avansate (Opționale)

Fiecare provocare rezolvată corect: **+10 puncte bonus**

---

## ⭐ Provocarea 1: Range Query (Dificultate: Medie)

### Descriere

Implementați funcția `range_query()` care returnează toate elementele din arbore cu chei în intervalul `[min, max]`.

### Cerințe

1. Funcția trebuie să fie eficientă - să nu parcurgă noduri în afara intervalului
2. Rezultatele vor fi returnate într-un array alocat dinamic
3. Complexitate țintă: O(log n + k) unde k este numărul de rezultate

### Semnătură

```c
typedef struct {
    int *values;
    int count;
    int capacity;
} QueryResult;

QueryResult* range_query(AVLNode *root, int min, int max);
void free_query_result(QueryResult *result);
```

### Exemplu

```c
// Arbore: 10, 20, 30, 40, 50, 60, 70
QueryResult *result = range_query(root, 25, 55);
// result->values = {30, 40, 50}
// result->count = 3
```

### Puncte Bonus: +10

---

## ⭐ Provocarea 2: K-th Smallest Element (Dificultate: Medie)

### Descriere

Găsirea eficientă a celui de-al k-lea cel mai mic element din arbore.

### Cerințe

1. Implementare naivă (parcurgere inorder) și optimizată (cu câmp de dimensiune)
2. Pentru versiunea optimizată, adăugați câmpul `size` în structura nodului

### Semnătură

```c
// Versiunea de bază
int kth_smallest_basic(AVLNode *root, int k);

// Versiunea optimizată (necesită modificare structură)
typedef struct AVLNodeAug {
    int key;
    struct AVLNodeAug *left, *right;
    int height;
    int size;  // Numărul de noduri în subarbore
} AVLNodeAug;

int kth_smallest_optimal(AVLNodeAug *root, int k);
```

### Complexitate

- Versiunea de bază: O(n)
- Versiunea optimizată: O(log n)

### Puncte Bonus: +10

---

## ⭐ Provocarea 3: Lowest Common Ancestor (Dificultate: Medie)

### Descriere

Găsirea celui mai apropiat strămoș comun (LCA) pentru două noduri date.

### Cerințe

1. Exploatați proprietatea BST pentru eficiență
2. Tratați cazurile când unul sau ambele noduri nu există
3. Tratați cazul când un nod este strămoșul celuilalt

### Semnătură

```c
AVLNode* lca(AVLNode *root, int key1, int key2);
```

### Algoritm

```
FUNCTION LCA(root, a, b):
    IF root = NULL:
        RETURN NULL
    
    IF a < root.key AND b < root.key:
        RETURN LCA(root.left, a, b)
    
    IF a > root.key AND b > root.key:
        RETURN LCA(root.right, a, b)
    
    RETURN root  // Divergență = LCA găsit
```

### Puncte Bonus: +10

---

## ⭐ Provocarea 4: Serializare și Deserializare (Dificultate: Grea)

### Descriere

Implementați funcții pentru salvarea arborelui în fișier și reconstrucția acestuia.

### Cerințe

1. Serializare în format text (o linie per nod)
2. Păstrarea structurii arborelui
3. Validare la deserializare

### Semnătură

```c
// Serializare - salvează în fișier
bool avl_serialize(AVLNode *root, const char *filename);

// Deserializare - încarcă din fișier
AVLNode* avl_deserialize(const char *filename);
```

### Format Fișier (exemplu)

```
# AVL Tree Serialization v1.0
# Preorder traversal with NULL markers
50
30
20
#
25
#
#
40
35
#
#
#
70
60
#
#
80
#
#
```

### Puncte Bonus: +10

---

## ⭐ Provocarea 5: AVL Tree Iterator (Dificultate: Grea)

### Descriere

Implementați un iterator pentru parcurgerea arborelui AVL fără recursie, folosind o stivă explicită.

### Cerințe

1. Structură de iterator cu stare internă
2. Funcții: `iter_create()`, `iter_has_next()`, `iter_next()`, `iter_destroy()`
3. Suport pentru multiple iteratoare simultane pe același arbore
4. Parcurgere inorder

### Semnătură

```c
typedef struct AVLIterator {
    AVLNode **stack;
    int stack_top;
    int stack_capacity;
    AVLNode *current;
} AVLIterator;

AVLIterator* avl_iter_create(AVLNode *root);
bool avl_iter_has_next(AVLIterator *iter);
int avl_iter_next(AVLIterator *iter);
void avl_iter_destroy(AVLIterator *iter);
```

### Exemplu de Utilizare

```c
AVLIterator *iter = avl_iter_create(root);
while (avl_iter_has_next(iter)) {
    int value = avl_iter_next(iter);
    printf("%d ", value);
}
avl_iter_destroy(iter);
```

### Puncte Bonus: +10

---

## 📊 Sistem de Puncte Bonus

| Provocări Completate | Total Bonus |
|---------------------|-------------|
| 1 | +10 puncte |
| 2 | +20 puncte |
| 3 | +30 puncte |
| 4 | +40 puncte |
| Toate 5 | +50 puncte + Badge **🏆 AVL Master** |

---

## 🎯 Criterii de Evaluare Bonus

Pentru a primi punctele bonus, soluția trebuie să:

1. ✅ Compileze fără warning-uri
2. ✅ Treacă toate testele furnizate
3. ✅ Nu aibă memory leaks (verificat cu Valgrind)
4. ✅ Respecte complexitatea cerută (unde aplicabil)
5. ✅ Aibă cod curat și comentat

---

## 📝 Format Trimitere

Includeți fișierele bonus în același director cu temele principale:

```
Nume_Prenume_Grupa_S09/
├── homework1_dict.c
├── homework2_generic.c
├── bonus/
│   ├── challenge1_range.c
│   ├── challenge2_kth.c
│   ├── challenge3_lca.c
│   ├── challenge4_serialize.c
│   └── challenge5_iterator.c
└── README.md
```

---

## 💡 Indicii pentru Provocări

### Provocarea 1 (Range Query)
- Verifică dacă trebuie să cobori în stânga: `min <= node->key`
- Verifică dacă trebuie să cobori în dreapta: `max >= node->key`
- Adaugă la rezultat dacă: `min <= node->key && node->key <= max`

### Provocarea 2 (K-th Smallest)
- Versiunea de bază: parcurgere inorder cu counter
- Versiunea optimizată: `size(left) + 1` = poziția nodului curent

### Provocarea 3 (LCA)
- Exploatează proprietatea BST
- Când cheile divergă (una < root, una > root), ai găsit LCA

### Provocarea 4 (Serializare)
- Parcurgere preorder pentru serializare
- Folosește un marker special pentru NULL
- La deserializare, reconstruiește recursiv

### Provocarea 5 (Iterator)
- Folosește o stivă pentru a simula call stack-ul recursiv
- La început, mergi în stânga cât poți, adăugând noduri pe stivă
- `next()` returnează vârful stivei și procesează subarborele drept

---

**Succes! 🚀**
