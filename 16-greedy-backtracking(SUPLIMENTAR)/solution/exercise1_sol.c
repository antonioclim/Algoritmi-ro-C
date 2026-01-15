/**
 * =============================================================================
 * SOLUȚIA EXERCIȚIULUI 1: PLANIFICATOR DE ACTIVITĂȚI (ACTIVITY SELECTION)
 * =============================================================================
 *
 * Acest fișier conține implementarea completă a algoritmului Greedy pentru
 * problema Activity Selection (Selecția Activităților).
 *
 * ALGORITM:
 *   1. Sortăm activitățile după timpul de terminare (finish time)
 *   2. Selectăm prima activitate (cu cel mai mic finish time)
 *   3. Pentru fiecare activitate următoare:
 *      - Dacă start >= finish-ul ultimei selectate → selectăm
 *      - Altfel, o ignorăm (overlap)
 *
 * DE CE FUNCȚIONEAZĂ:
 *   Alegerea greedy (selectarea activității cu cel mai mic finish time)
 *   garantează soluția optimă deoarece lasă maximum de timp disponibil
 *   pentru restul activităților.
 *
 * COMPLEXITATE: O(n log n) pentru sortare + O(n) pentru selecție = O(n log n)
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -o exercise1_sol exercise1_sol.c
 * Execuție:  ./exercise1_sol ../data/activities.txt
 * Valgrind:  valgrind --leak-check=full ./exercise1_sol ../data/activities.txt
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* =============================================================================
 * CONSTANTE ȘI DEFINIREA TIPURILOR
 * =============================================================================
 */

#define MAX_ACTIVITIES 100
#define MAX_NAME_LEN 64

/**
 * Structura pentru o activitate
 *
 * Câmpuri:
 *   - id: identificator unic al activității
 *   - name: numele activității
 *   - start: timpul de start (minute de la miezul nopții)
 *   - finish: timpul de terminare (minute de la miezul nopții)
 *   - profit: valoarea/profitul activității (pentru varianta bonus)
 */
typedef struct {
    int id;
    char name[MAX_NAME_LEN];
    int start;
    int finish;
    int profit;
} Activity;

/**
 * Structura pentru rezultatul selecției
 */
typedef struct {
    int selected_indices[MAX_ACTIVITIES];
    int count;
    int total_duration;
    int total_profit;
} SelectionResult;

/* =============================================================================
 * FUNCȚII DE PARSARE ȘI UTILITĂȚI
 * =============================================================================
 */

/**
 * Convertește un string de tip "HH:MM" în minute de la miezul nopții.
 *
 * @param time_str String în format "HH:MM"
 * @return Minutele de la 00:00, sau -1 pentru eroare
 *
 * Exemplu: "08:30" -> 510, "14:00" -> 840
 */
int parse_time(const char *time_str) {
    int hours, minutes;
    
    if (sscanf(time_str, "%d:%d", &hours, &minutes) != 2) {
        return -1;
    }
    
    if (hours < 0 || hours > 23 || minutes < 0 || minutes > 59) {
        return -1;
    }
    
    return hours * 60 + minutes;
}

/**
 * Convertește minute în format "HH:MM".
 *
 * @param minutes Minutele de la miezul nopții
 * @param buffer Buffer pentru rezultat (minim 6 caractere)
 */
void minutes_to_time(int minutes, char *buffer) {
    int h = minutes / 60;
    int m = minutes % 60;
    sprintf(buffer, "%02d:%02d", h, m);
}

/**
 * Afișează informațiile despre o activitate.
 */
void print_activity(const Activity *act) {
    char start_str[8], finish_str[8];
    minutes_to_time(act->start, start_str);
    minutes_to_time(act->finish, finish_str);
    
    printf("  [%2d] %-20s %s - %s (durată: %d min", 
           act->id, act->name, start_str, finish_str,
           act->finish - act->start);
    if (act->profit > 1) {
        printf(", profit: %d", act->profit);
    }
    printf(")\n");
}

/* =============================================================================
 * FUNCȚII DE CITIRE
 * =============================================================================
 */

/**
 * Citește activitățile dintr-un fișier.
 *
 * @param filename Numele fișierului
 * @param activities Array pentru stocarea activităților
 * @param n Pointer la variabila care va conține numărul de activități
 * @return true dacă citirea a reușit, false altfel
 *
 * Format fișier:
 *   N (număr activități)
 *   NumeActivitate HH:MM HH:MM [profit]
 *   ...
 */
bool read_activities_from_file(const char *filename, Activity *activities, int *n) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Eroare: Nu s-a putut deschide fișierul %s\n", filename);
        return false;
    }
    
    /* Citim numărul de activități */
    if (fscanf(file, "%d", n) != 1) {
        fprintf(stderr, "Eroare: Format invalid în fișier\n");
        fclose(file);
        return false;
    }
    
    /* Consumă newline-ul rămas */
    fgetc(file);
    
    /* Citim fiecare activitate */
    for (int i = 0; i < *n; i++) {
        char line[256];
        if (fgets(line, sizeof(line), file) == NULL) {
            fprintf(stderr, "Eroare: Nu s-au putut citi toate activitățile\n");
            fclose(file);
            return false;
        }
        
        /* Parsăm linia */
        char name[MAX_NAME_LEN];
        char start_str[8], finish_str[8];
        int profit = 1;  /* Default profit */
        
        int parsed = sscanf(line, "%s %s %s %d", name, start_str, finish_str, &profit);
        if (parsed < 3) {
            fprintf(stderr, "Eroare: Format invalid pentru activitatea %d\n", i + 1);
            fclose(file);
            return false;
        }
        
        activities[i].id = i + 1;
        strncpy(activities[i].name, name, MAX_NAME_LEN - 1);
        activities[i].name[MAX_NAME_LEN - 1] = '\0';
        activities[i].start = parse_time(start_str);
        activities[i].finish = parse_time(finish_str);
        activities[i].profit = profit;
        
        if (activities[i].start < 0 || activities[i].finish < 0) {
            fprintf(stderr, "Eroare: Timp invalid pentru activitatea %d\n", i + 1);
            fclose(file);
            return false;
        }
    }
    
    fclose(file);
    return true;
}

/**
 * Citește activități de la stdin (pentru testare automată)
 */
bool read_activities_from_stdin(Activity *activities, int *n) {
    if (scanf("%d", n) != 1) {
        return false;
    }
    
    for (int i = 0; i < *n; i++) {
        char name[MAX_NAME_LEN];
        char start_str[8], finish_str[8];
        
        if (scanf("%s %s %s", name, start_str, finish_str) != 3) {
            return false;
        }
        
        activities[i].id = i + 1;
        strncpy(activities[i].name, name, MAX_NAME_LEN - 1);
        activities[i].name[MAX_NAME_LEN - 1] = '\0';
        activities[i].start = parse_time(start_str);
        activities[i].finish = parse_time(finish_str);
        activities[i].profit = 1;
    }
    
    return true;
}

/* =============================================================================
 * FUNCȚII DE SORTARE
 * =============================================================================
 */

/**
 * Funcție de comparare pentru sortarea după finish time.
 *
 * Folosită cu qsort() pentru a ordona activitățile crescător
 * după timpul de terminare.
 *
 * @param a Pointer la prima activitate
 * @param b Pointer la a doua activitate
 * @return Valoare negativă dacă a.finish < b.finish, pozitivă dacă >
 */
int compare_by_finish_time(const void *a, const void *b) {
    const Activity *act_a = (const Activity *)a;
    const Activity *act_b = (const Activity *)b;
    
    return act_a->finish - act_b->finish;
}

/**
 * Funcție de comparare pentru sortarea după raportul profit/durată.
 * (pentru varianta bonus - Weighted Job Scheduling)
 */
int compare_by_profit_ratio(const void *a, const void *b) {
    const Activity *act_a = (const Activity *)a;
    const Activity *act_b = (const Activity *)b;
    
    int duration_a = act_a->finish - act_a->start;
    int duration_b = act_b->finish - act_b->start;
    
    double ratio_a = (double)act_a->profit / duration_a;
    double ratio_b = (double)act_b->profit / duration_b;
    
    /* Returnăm în ordine descrescătoare */
    return (ratio_b > ratio_a) - (ratio_b < ratio_a);
}

/* =============================================================================
 * ALGORITM GREEDY - ACTIVITY SELECTION
 * =============================================================================
 */

/**
 * Implementează algoritmul Greedy pentru Activity Selection.
 *
 * ALGORITM:
 *   1. Sortează activitățile după finish time (crescător)
 *   2. Selectează prima activitate (are cel mai mic finish time)
 *   3. Pentru fiecare activitate i:
 *      - Dacă start[i] >= finish al ultimei selectate → selectează
 *      - Altfel → skip (activitățile se suprapun)
 *
 * @param activities Array de activități (va fi sortat!)
 * @param n Numărul de activități
 * @param result Pointer la structura de rezultat
 *
 * Complexitate: O(n log n) pentru sortare + O(n) pentru selecție
 *
 * Demonstrație corectitudine:
 *   Fie A = {a1, a2, ..., ak} soluția greedy și B soluția optimă.
 *   Dacă a1 ∉ B, atunci putem înlocui prima activitate din B cu a1
 *   (pentru că a1 are cel mai mic finish time), obținând o soluție
 *   cel puțin la fel de bună. Prin inducție, A = B (optim).
 */
void greedy_activity_selection(Activity *activities, int n, SelectionResult *result) {
    /* Inițializare rezultat */
    result->count = 0;
    result->total_duration = 0;
    result->total_profit = 0;
    
    if (n == 0) return;
    
    /* Pas 1: Sortare după finish time */
    qsort(activities, n, sizeof(Activity), compare_by_finish_time);
    
    /* Pas 2: Selectare greedy */
    int last_finish = 0;  /* Timpul de finish al ultimei activități selectate */
    
    for (int i = 0; i < n; i++) {
        /* Verificăm dacă activitatea curentă este compatibilă */
        if (activities[i].start >= last_finish) {
            /* Selectăm această activitate */
            result->selected_indices[result->count] = i;
            result->count++;
            
            /* Actualizăm statisticile */
            result->total_duration += (activities[i].finish - activities[i].start);
            result->total_profit += activities[i].profit;
            
            /* Actualizăm last_finish */
            last_finish = activities[i].finish;
        }
    }
}

/* =============================================================================
 * BONUS: WEIGHTED JOB SCHEDULING (folosind DP)
 * =============================================================================
 */

/**
 * Implementează Weighted Job Scheduling folosind Programare Dinamică.
 *
 * Când activitățile au profituri diferite, greedy simplu NU garantează optim.
 * Folosim DP pentru a găsi profitul maxim.
 *
 * ALGORITM:
 *   dp[i] = profitul maxim folosind primele i activități
 *   dp[i] = max(dp[i-1], profit[i] + dp[j])
 *          unde j este ultima activitate compatibilă înainte de i
 *
 * @param activities Array de activități (va fi sortat)
 * @param n Numărul de activități
 * @return Profitul maxim
 *
 * Complexitate: O(n²) - poate fi optimizată la O(n log n) cu binary search
 */
int weighted_job_scheduling(Activity *activities, int n) {
    if (n == 0) return 0;
    
    /* Sortare după finish time */
    qsort(activities, n, sizeof(Activity), compare_by_finish_time);
    
    int *dp = (int *)malloc(n * sizeof(int));
    if (dp == NULL) {
        fprintf(stderr, "Eroare: Nu s-a putut aloca memorie!\n");
        return -1;
    }
    
    /* Cazul de bază */
    dp[0] = activities[0].profit;
    
    for (int i = 1; i < n; i++) {
        /* Opțiunea 1: Nu includem activitatea i */
        int exclude = dp[i - 1];
        
        /* Opțiunea 2: Includem activitatea i */
        int include = activities[i].profit;
        
        /* Găsim ultima activitate compatibilă j < i */
        for (int j = i - 1; j >= 0; j--) {
            if (activities[j].finish <= activities[i].start) {
                include += dp[j];
                break;
            }
        }
        
        dp[i] = (include > exclude) ? include : exclude;
    }
    
    int result = dp[n - 1];
    free(dp);
    
    return result;
}

/* =============================================================================
 * FUNCȚII DE AFIȘARE
 * =============================================================================
 */

/**
 * Afișează rezultatul selecției
 */
void print_selection_result(const Activity *activities, const SelectionResult *result) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║  REZULTAT ACTIVITY SELECTION                                  ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    printf("  Activități selectate: %d\n\n", result->count);
    
    for (int i = 0; i < result->count; i++) {
        int idx = result->selected_indices[i];
        print_activity(&activities[idx]);
    }
    
    printf("\n  Statistici:\n");
    printf("  ├─ Durată totală ocupată: %d minute (%.1f ore)\n",
           result->total_duration, result->total_duration / 60.0);
    printf("  └─ Profit total: %d\n", result->total_profit);
}

/**
 * Afișează timeline-ul vizual
 */
void print_timeline(const Activity *activities, const SelectionResult *result,
                    int start_hour, int end_hour) {
    printf("\n  Timeline vizual (ora %d:00 - %d:00):\n\n", start_hour, end_hour);
    
    /* Header */
    printf("  Ora:  ");
    for (int h = start_hour; h <= end_hour; h++) {
        printf("%-4d", h);
    }
    printf("\n        ");
    for (int h = start_hour; h <= end_hour; h++) {
        printf("|   ");
    }
    printf("\n");
    
    /* Activități selectate */
    for (int i = 0; i < result->count; i++) {
        int idx = result->selected_indices[i];
        const Activity *act = &activities[idx];
        
        printf("        ");
        for (int t = start_hour * 60; t < end_hour * 60; t += 15) {
            if (t >= act->start && t < act->finish) {
                printf("█");
            } else {
                printf(" ");
            }
        }
        printf(" %s\n", act->name);
    }
}

/* =============================================================================
 * MAIN
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║  SOLUȚIE: PLANIFICATOR DE ACTIVITĂȚI (GREEDY)                 ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    Activity activities[MAX_ACTIVITIES];
    int n = 0;
    bool success;
    
    /* Citire date */
    if (argc > 1) {
        printf("  Citire din fișierul: %s\n\n", argv[1]);
        success = read_activities_from_file(argv[1], activities, &n);
    } else {
        printf("  Citire de la stdin...\n");
        printf("  (Introduceți numărul de activități, apoi fiecare activitate)\n\n");
        success = read_activities_from_stdin(activities, &n);
    }
    
    if (!success) {
        fprintf(stderr, "Eroare la citirea datelor!\n");
        return 1;
    }
    
    /* Păstrăm copie pentru afișarea inițială */
    Activity activities_copy[MAX_ACTIVITIES];
    memcpy(activities_copy, activities, n * sizeof(Activity));
    
    /* Afișare activități citite */
    printf("  Activități citite (%d):\n", n);
    printf("  ─────────────────────────────────────────────────────\n");
    for (int i = 0; i < n; i++) {
        print_activity(&activities_copy[i]);
    }
    
    /* Aplicare algoritm greedy */
    SelectionResult result;
    greedy_activity_selection(activities, n, &result);
    
    /* Afișare rezultat */
    print_selection_result(activities, &result);
    
    /* Determinăm intervalul orar pentru timeline */
    int min_hour = 24, max_hour = 0;
    for (int i = 0; i < n; i++) {
        int start_h = activities[i].start / 60;
        int end_h = (activities[i].finish + 59) / 60;
        if (start_h < min_hour) min_hour = start_h;
        if (end_h > max_hour) max_hour = end_h;
    }
    print_timeline(activities, &result, min_hour, max_hour);
    
    /* Output pentru testare automată (format simplu) */
    printf("\n  Output pentru verificare automată:\n");
    printf("  %d\n", result.count);
    for (int i = 0; i < result.count; i++) {
        int idx = result.selected_indices[i];
        printf("  %s\n", activities[idx].name);
    }
    
    /* BONUS: Weighted Job Scheduling */
    bool has_weights = false;
    for (int i = 0; i < n; i++) {
        if (activities_copy[i].profit > 1) {
            has_weights = true;
            break;
        }
    }
    
    if (has_weights) {
        printf("\n");
        printf("╔═══════════════════════════════════════════════════════════════╗\n");
        printf("║  BONUS: WEIGHTED JOB SCHEDULING (DP)                          ║\n");
        printf("╚═══════════════════════════════════════════════════════════════╝\n");
        
        int max_profit = weighted_job_scheduling(activities_copy, n);
        printf("\n  Profit maxim posibil (cu DP): %d\n", max_profit);
        printf("  Profit obținut cu Greedy: %d\n", result.total_profit);
        
        if (max_profit > result.total_profit) {
            printf("\n  ⚠ Greedy nu găsește optim când profiturile diferă!\n");
            printf("    Diferență: %d\n", max_profit - result.total_profit);
        }
    }
    
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════════\n");
    printf("  ✓ Soluție completată cu succes!\n");
    printf("═══════════════════════════════════════════════════════════════════\n");
    printf("\n");
    
    return 0;
}
