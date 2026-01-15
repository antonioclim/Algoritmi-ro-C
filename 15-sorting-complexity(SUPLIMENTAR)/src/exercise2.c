/**
 * =============================================================================
 * EXERCIȚIUL 2: Sortare Externă (External Merge Sort)
 * =============================================================================
 *
 * OBIECTIV:
 *   Implementați sortarea unui fișier de date care nu încape în memorie
 *   folosind tehnica External Merge Sort cu simulare de buffer limitat.
 *
 * CERINȚE:
 *   1. Citiți date în chunk-uri de dimensiune fixă (simulează memoria limitată)
 *   2. Sortați fiecare chunk în memorie și salvați ca "run" pe disc
 *   3. Implementați k-way merge pentru combinarea runs
 *   4. Validați că rezultatul final este corect sortat
 *
 * EXEMPLU INPUT:
 *   Fișier: data/large_unsorted.txt cu 100000 numere
 *   Buffer: 1000 elemente
 *
 * EXPECTED OUTPUT:
 *   Fișier sortat și statistici despre procesul de sortare
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -O2 -o exercise2 exercise2.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

#define BUFFER_SIZE 1000    /* Dimensiunea buffer-ului (chunk) */
#define MAX_RUNS 100        /* Numărul maxim de runs */

/* =============================================================================
 * STRUCTURI DE DATE
 * =============================================================================
 */

/**
 * Structură pentru un element în min-heap (pentru k-way merge)
 */
typedef struct {
    int value;      /* Valoarea elementului */
    int run_idx;    /* Indexul run-ului de origine */
} HeapElement;

/**
 * Statistici despre sortare
 */
typedef struct {
    int total_elements;
    int num_runs;
    int merge_passes;
    long long comparisons;
} SortStats;

/* =============================================================================
 * FUNCȚII UTILITARE PENTRU HEAP (FURNIZATE)
 * =============================================================================
 */

/**
 * Swap pentru HeapElement
 */
void heap_swap(HeapElement *a, HeapElement *b) {
    HeapElement temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * Min-heapify pentru k-way merge
 */
void min_heapify(HeapElement heap[], int size, int i) {
    int smallest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    
    if (left < size && heap[left].value < heap[smallest].value)
        smallest = left;
    
    if (right < size && heap[right].value < heap[smallest].value)
        smallest = right;
    
    if (smallest != i) {
        heap_swap(&heap[i], &heap[smallest]);
        min_heapify(heap, size, smallest);
    }
}

/**
 * Construiește min-heap
 */
void build_min_heap(HeapElement heap[], int size) {
    for (int i = size / 2 - 1; i >= 0; i--) {
        min_heapify(heap, size, i);
    }
}

/**
 * Extrage minimul și înlocuiește cu o nouă valoare
 */
HeapElement extract_and_replace(HeapElement heap[], int *size, int new_value, int run_idx) {
    HeapElement min = heap[0];
    
    if (new_value == INT_MAX) {
        /* Run epuizat - reducem heap-ul */
        heap[0] = heap[*size - 1];
        (*size)--;
        if (*size > 0) {
            min_heapify(heap, *size, 0);
        }
    } else {
        /* Înlocuim cu noua valoare */
        heap[0].value = new_value;
        heap[0].run_idx = run_idx;
        min_heapify(heap, *size, 0);
    }
    
    return min;
}

/* =============================================================================
 * FUNCȚII DE SORTARE PENTRU CHUNK-URI (FURNIZATE)
 * =============================================================================
 */

/**
 * Quick Sort intern pentru sortarea chunk-urilor
 */
void swap_int(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int partition(int arr[], int low, int high) {
    int pivot_idx = low + (high - low) / 2;
    swap_int(&arr[pivot_idx], &arr[high]);
    int pivot = arr[high];
    int i = low - 1;
    
    for (int j = low; j < high; j++) {
        if (arr[j] <= pivot) {
            i++;
            swap_int(&arr[i], &arr[j]);
        }
    }
    swap_int(&arr[i + 1], &arr[high]);
    return i + 1;
}

void quick_sort_internal(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quick_sort_internal(arr, low, pi - 1);
        quick_sort_internal(arr, pi + 1, high);
    }
}

void quick_sort(int arr[], int n) {
    quick_sort_internal(arr, 0, n - 1);
}

/* =============================================================================
 * FUNCȚII DE IMPLEMENTAT
 * =============================================================================
 */

/**
 * TODO 1: Creează runs sortate din fișierul de intrare
 *
 * @param input_file Calea către fișierul de intrare
 * @param run_files Array de pointeri pentru fișierele runs (output)
 * @param num_runs Pointer pentru numărul de runs create (output)
 * @param stats Statistici (output)
 * @return true dacă a reușit, false altfel
 *
 * Pași:
 *   1. Deschideți fișierul de intrare pentru citire
 *   2. Într-o buclă:
 *      a. Citiți BUFFER_SIZE numere în buffer
 *      b. Sortați buffer-ul cu quick_sort
 *      c. Creați un fișier temporar pentru run (sprintf pentru nume)
 *      d. Scrieți buffer-ul sortat în fișierul run
 *      e. Adăugați fișierul în array-ul run_files
 *   3. Actualizați statisticile
 *   4. Închideți fișierul de intrare
 */
bool create_sorted_runs(const char *input_file, FILE *run_files[], int *num_runs, SortStats *stats) {
    /* YOUR CODE HERE */
    
    FILE *input = fopen(input_file, "r");
    if (input == NULL) {
        fprintf(stderr, "Eroare: Nu s-a putut deschide fișierul de intrare '%s'\n", input_file);
        return false;
    }
    
    int *buffer = (int*)malloc(BUFFER_SIZE * sizeof(int));
    if (buffer == NULL) {
        fprintf(stderr, "Eroare: Nu s-a putut aloca memorie pentru buffer!\n");
        fclose(input);
        return false;
    }
    
    *num_runs = 0;
    stats->total_elements = 0;
    
    int count;
    while ((count = 0, 1)) {
        /* Citim până la BUFFER_SIZE elemente */
        while (count < BUFFER_SIZE && fscanf(input, "%d", &buffer[count]) == 1) {
            count++;
        }
        
        if (count == 0) break;  /* EOF */
        
        stats->total_elements += count;
        
        /* Sortăm buffer-ul */
        quick_sort(buffer, count);
        
        /* Creăm fișierul pentru acest run */
        char run_filename[64];
        sprintf(run_filename, "/tmp/run_%d.txt", *num_runs);
        
        FILE *run_file = fopen(run_filename, "w+");
        if (run_file == NULL) {
            fprintf(stderr, "Eroare: Nu s-a putut crea fișierul run '%s'\n", run_filename);
            free(buffer);
            fclose(input);
            return false;
        }
        
        /* Scriem datele sortate */
        for (int i = 0; i < count; i++) {
            fprintf(run_file, "%d\n", buffer[i]);
        }
        
        /* Resetăm poziția la început pentru citire ulterioară */
        rewind(run_file);
        
        run_files[*num_runs] = run_file;
        (*num_runs)++;
        
        if (*num_runs >= MAX_RUNS) {
            fprintf(stderr, "Atenție: Număr maxim de runs atins!\n");
            break;
        }
    }
    
    free(buffer);
    fclose(input);
    
    stats->num_runs = *num_runs;
    printf("  → Create %d runs sortate\n", *num_runs);
    
    return true;
}

/**
 * TODO 2: Implementați k-way merge pentru combinarea runs
 *
 * @param run_files Array de fișiere runs deschise pentru citire
 * @param num_runs Numărul de runs
 * @param output_file Calea către fișierul de output
 * @param stats Statistici (input/output)
 * @return true dacă a reușit, false altfel
 *
 * Pași:
 *   1. Creați un min-heap cu primul element din fiecare run
 *   2. Deschideți fișierul de output pentru scriere
 *   3. Într-o buclă (cât timp heap-ul nu e gol):
 *      a. Extrageți minimul din heap
 *      b. Scrieți-l în fișierul de output
 *      c. Citiți următorul element din run-ul corespunzător
 *      d. Dacă run-ul s-a terminat, marcați cu INT_MAX
 *      e. Actualizați heap-ul
 *   4. Închideți fișierele
 *
 * Hint: Folosiți funcțiile heap furnizate!
 */
bool k_way_merge(FILE *run_files[], int num_runs, const char *output_file, SortStats *stats) {
    /* YOUR CODE HERE */
    
    if (num_runs == 0) {
        fprintf(stderr, "Eroare: Nu există runs de combinat!\n");
        return false;
    }
    
    /* Alocăm heap-ul */
    HeapElement *heap = (HeapElement*)malloc(num_runs * sizeof(HeapElement));
    if (heap == NULL) {
        fprintf(stderr, "Eroare: Nu s-a putut aloca memorie pentru heap!\n");
        return false;
    }
    
    /* Deschidem fișierul de output */
    FILE *output = fopen(output_file, "w");
    if (output == NULL) {
        fprintf(stderr, "Eroare: Nu s-a putut crea fișierul de output '%s'\n", output_file);
        free(heap);
        return false;
    }
    
    /* Inițializăm heap-ul cu primul element din fiecare run */
    int heap_size = 0;
    for (int i = 0; i < num_runs; i++) {
        int value;
        if (fscanf(run_files[i], "%d", &value) == 1) {
            heap[heap_size].value = value;
            heap[heap_size].run_idx = i;
            heap_size++;
        }
    }
    
    /* Construim min-heap */
    build_min_heap(heap, heap_size);
    
    /* K-way merge */
    int merged_count = 0;
    while (heap_size > 0) {
        /* Extragem minimul */
        HeapElement min = heap[0];
        fprintf(output, "%d\n", min.value);
        merged_count++;
        stats->comparisons++;
        
        /* Citim următorul element din run-ul corespunzător */
        int next_value;
        int run_idx = min.run_idx;
        
        if (fscanf(run_files[run_idx], "%d", &next_value) == 1) {
            /* Înlocuim în heap */
            heap[0].value = next_value;
            heap[0].run_idx = run_idx;
            min_heapify(heap, heap_size, 0);
        } else {
            /* Run epuizat - reducem heap-ul */
            heap[0] = heap[heap_size - 1];
            heap_size--;
            if (heap_size > 0) {
                min_heapify(heap, heap_size, 0);
            }
        }
    }
    
    printf("  → Merged %d elemente\n", merged_count);
    
    free(heap);
    fclose(output);
    
    return true;
}

/**
 * TODO 3: Verificați dacă fișierul rezultat este sortat
 *
 * @param filename Calea către fișier
 * @return true dacă fișierul este sortat, false altfel
 */
bool verify_sorted(const char *filename) {
    /* YOUR CODE HERE */
    
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Eroare: Nu s-a putut deschide fișierul pentru verificare!\n");
        return false;
    }
    
    int prev, current;
    if (fscanf(file, "%d", &prev) != 1) {
        /* Fișier gol - considerat sortat */
        fclose(file);
        return true;
    }
    
    int count = 1;
    while (fscanf(file, "%d", &current) == 1) {
        if (current < prev) {
            fprintf(stderr, "Eroare: Fișierul nu este sortat la poziția %d (%d > %d)\n", 
                    count, prev, current);
            fclose(file);
            return false;
        }
        prev = current;
        count++;
    }
    
    fclose(file);
    return true;
}

/* =============================================================================
 * FUNCȚII DE GENERARE DATE DE TEST (FURNIZATE)
 * =============================================================================
 */

/**
 * Generează un fișier de test cu numere aleatorii
 */
bool generate_test_file(const char *filename, int count) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        return false;
    }
    
    for (int i = 0; i < count; i++) {
        fprintf(file, "%d\n", rand() % 1000000);
    }
    
    fclose(file);
    return true;
}

/**
 * Curăță fișierele temporare
 */
void cleanup_temp_files(int num_runs) {
    for (int i = 0; i < num_runs; i++) {
        char filename[64];
        sprintf(filename, "/tmp/run_%d.txt", i);
        remove(filename);
    }
}

/* =============================================================================
 * MAIN
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    printf("═══════════════════════════════════════════════════════════════════════════════\n");
    printf("           EXERCIȚIUL 2: EXTERNAL MERGE SORT\n");
    printf("═══════════════════════════════════════════════════════════════════════════════\n");
    printf("\n");
    
    /* Test rapid pentru verificare automată */
    if (argc > 1 && strcmp(argv[1], "--quick-test") == 0) {
        /* Creăm un fișier mic de test */
        int test_data[] = {5, 2, 8, 1, 9, 3, 7, 4, 6, 10};
        FILE *test = fopen("/tmp/quick_test.txt", "w");
        for (int i = 0; i < 10; i++) {
            fprintf(test, "%d\n", test_data[i]);
        }
        fclose(test);
        
        /* Sortăm */
        FILE *run_files[MAX_RUNS];
        int num_runs = 0;
        SortStats stats = {0};
        
        create_sorted_runs("/tmp/quick_test.txt", run_files, &num_runs, &stats);
        k_way_merge(run_files, num_runs, "/tmp/quick_output.txt", &stats);
        
        /* Verificăm */
        bool sorted = verify_sorted("/tmp/quick_output.txt");
        printf("Quick test: %s\n", sorted ? "PASSED" : "FAILED");
        
        /* Curățăm */
        for (int i = 0; i < num_runs; i++) {
            fclose(run_files[i]);
        }
        cleanup_temp_files(num_runs);
        remove("/tmp/quick_test.txt");
        remove("/tmp/quick_output.txt");
        
        return sorted ? 0 : 1;
    }
    
    /* Mod normal */
    const char *input_file = "/tmp/external_sort_input.txt";
    const char *output_file = "/tmp/external_sort_output.txt";
    int num_elements = 50000;  /* 50K elemente pentru demonstrație */
    
    printf("Configurație:\n");
    printf("  • Număr elemente: %d\n", num_elements);
    printf("  • Dimensiune buffer: %d elemente\n", BUFFER_SIZE);
    printf("  • Runs estimate: ~%d\n", (num_elements + BUFFER_SIZE - 1) / BUFFER_SIZE);
    printf("\n");
    
    /* Generăm date de test */
    printf("Pas 1: Generare fișier de test...\n");
    if (!generate_test_file(input_file, num_elements)) {
        fprintf(stderr, "Eroare: Nu s-a putut genera fișierul de test!\n");
        return 1;
    }
    printf("  → Generat %s cu %d numere\n", input_file, num_elements);
    printf("\n");
    
    /* Creăm runs sortate */
    printf("Pas 2: Creare runs sortate...\n");
    FILE *run_files[MAX_RUNS];
    int num_runs = 0;
    SortStats stats = {0, 0, 0, 0};
    
    if (!create_sorted_runs(input_file, run_files, &num_runs, &stats)) {
        fprintf(stderr, "Eroare: Crearea runs a eșuat!\n");
        return 1;
    }
    printf("\n");
    
    /* K-way merge */
    printf("Pas 3: K-way merge (%d runs)...\n", num_runs);
    if (!k_way_merge(run_files, num_runs, output_file, &stats)) {
        fprintf(stderr, "Eroare: K-way merge a eșuat!\n");
        return 1;
    }
    printf("\n");
    
    /* Verificare */
    printf("Pas 4: Verificare rezultat...\n");
    if (verify_sorted(output_file)) {
        printf("  → ✓ Fișierul de output este CORECT SORTAT!\n");
    } else {
        printf("  → ✗ EROARE: Fișierul nu este sortat corect!\n");
    }
    printf("\n");
    
    /* Statistici finale */
    printf("═══════════════════════════════════════════════════════════════════════════════\n");
    printf("                           STATISTICI\n");
    printf("═══════════════════════════════════════════════════════════════════════════════\n");
    printf("  • Elemente totale: %d\n", stats.total_elements);
    printf("  • Runs create: %d\n", stats.num_runs);
    printf("  • Dimensiune chunk: %d elemente\n", BUFFER_SIZE);
    printf("  • Comparații (merge): %lld\n", stats.comparisons);
    printf("\n");
    
    /* Curățăm */
    printf("Curățare fișiere temporare...\n");
    for (int i = 0; i < num_runs; i++) {
        fclose(run_files[i]);
    }
    cleanup_temp_files(num_runs);
    remove(input_file);
    /* Păstrăm output-ul pentru inspecție */
    printf("  → Fișierul sortat: %s\n", output_file);
    
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════════════════════\n");
    printf("              EXTERNAL SORT COMPLETAT CU SUCCES!\n");
    printf("═══════════════════════════════════════════════════════════════════════════════\n");
    
    return 0;
}
