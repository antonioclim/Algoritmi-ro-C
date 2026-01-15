/**
 * =============================================================================
 * EXERCIȚIUL 1: PLANIFICATOR DE ACTIVITĂȚI (ACTIVITY SELECTION)
 * =============================================================================
 *
 * OBIECTIV:
 *   Implementați un sistem complet de planificare a activităților folosind
 *   algoritmul Greedy Activity Selection. Sistemul trebuie să citească
 *   activități din fișier, să le sorteze și să selecteze setul maxim de
 *   activități compatibile (care nu se suprapun).
 *
 * CERINȚE:
 *   1. Citiți activitățile din fișierul de input
 *   2. Implementați sortarea după timpul de terminare
 *   3. Aplicați algoritmul greedy pentru selecție
 *   4. Afișați activitățile selectate cu statistici
 *   5. BONUS: Implementați varianta cu profit maxim (Weighted Job Scheduling)
 *
 * EXEMPLU INPUT (data/activities.txt):
 *   5
 *   Curs 08:00 10:00
 *   Lab 09:00 11:00
 *   Seminar 10:30 12:00
 *   Pranz 12:00 13:00
 *   Sport 11:00 14:00
 *
 * EXPECTED OUTPUT:
 *   Activități selectate: 3
 *   - Curs (08:00-10:00)
 *   - Seminar (10:30-12:00)
 *   - Pranz (12:00-13:00)
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -o exercise1 exercise1.c
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
 * TODO 1: Completați structura pentru o activitate
 *
 * O activitate are:
 *   - id: identificator unic
 *   - name: numele activității (șir de caractere)
 *   - start: timpul de start (în minute de la miezul nopții)
 *   - finish: timpul de finish (în minute de la miezul nopții)
 *   - profit: profit/valoare (pentru varianta bonus)
 *
 * Hint: Convertiți orele în minute pentru calcule mai simple
 *       Ex: 08:30 = 8*60 + 30 = 510 minute
 */
typedef struct {
    int id;
    char name[MAX_NAME_LEN];
    /* YOUR CODE HERE - adăugați câmpurile necesare */
    int start;      /* Timpul de start în minute */
    int finish;     /* Timpul de finish în minute */
    int profit;     /* Pentru varianta bonus */
} Activity;

/**
 * Structură pentru rezultatul selecției
 */
typedef struct {
    int selected_indices[MAX_ACTIVITIES];
    int count;
    int total_duration;
    int total_profit;  /* Pentru varianta bonus */
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
    
    printf("  [%2d] %-20s %s - %s (durată: %d min)\n",
           act->id, act->name, start_str, finish_str,
           act->finish - act->start);
}

/* =============================================================================
 * FUNCȚII DE CITIRE
 * =============================================================================
 */

/**
 * TODO 2: Implementați citirea activităților din fișier
 *
 * @param filename Numele fișierului de input
 * @param activities Array pentru stocarea activităților
 * @param n Pointer la variabila care va conține numărul de activități
 * @return true dacă citirea a reușit, false altfel
 *
 * Format fișier:
 *   Prima linie: numărul de activități (N)
 *   Următoarele N linii: nume start_time end_time [profit]
 *
 * Pași:
 *   1. Deschideți fișierul pentru citire
 *   2. Citiți numărul de activități
 *   3. Pentru fiecare activitate:
 *      a. Citiți linia
 *      b. Parsați numele, start, finish
 *      c. Convertiți timpii folosind parse_time()
 *      d. Setați id-ul activității
 *   4. Închideți fișierul
 *
 * Hint: Folosiți fgets() pentru a citi linii complete,
 *       apoi sscanf() pentru a extrage datele.
 */
bool read_activities_from_file(const char *filename, Activity *activities, int *n) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Eroare: Nu s-a putut deschide fișierul %s\n", filename);
        return false;
    }
    
    /* YOUR CODE HERE */
    
    /* Citiți numărul de activități */
    if (fscanf(file, "%d", n) != 1) {
        fprintf(stderr, "Eroare: Format invalid în fișier\n");
        fclose(file);
        return false;
    }
    
    /* Consumă newline-ul rămas */
    fgetc(file);
    
    /* Citiți fiecare activitate */
    for (int i = 0; i < *n; i++) {
        char line[256];
        if (fgets(line, sizeof(line), file) == NULL) {
            fprintf(stderr, "Eroare: Nu s-au putut citi toate activitățile\n");
            fclose(file);
            return false;
        }
        
        /* TODO: Parsați linia și completați activities[i] */
        /* Format: "NumeActivitate HH:MM HH:MM [profit]" */
        
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
 * TODO 3: Implementați funcția de comparare pentru sortarea după finish time
 *
 * @param a Pointer la prima activitate
 * @param b Pointer la a doua activitate
 * @return Diferența dintre finish times (pentru qsort)
 *
 * Hint: Returna a->finish - b->finish pentru ordine crescătoare
 */
int compare_by_finish_time(const void *a, const void *b) {
    /* YOUR CODE HERE */
    const Activity *act_a = (const Activity *)a;
    const Activity *act_b = (const Activity *)b;
    
    return act_a->finish - act_b->finish;
}

/**
 * Funcție de comparare pentru sortarea după raportul profit/durată
 * (pentru varianta bonus - Weighted Job Scheduling)
 */
int compare_by_profit_ratio(const void *a, const void *b) {
    const Activity *act_a = (const Activity *)a;
    const Activity *act_b = (const Activity *)b;
    
    int duration_a = act_a->finish - act_a->start;
    int duration_b = act_b->finish - act_b->start;
    
    double ratio_a = (double)act_a->profit / duration_a;
    double ratio_b = (double)act_b->profit / duration_b;
    
    return (ratio_b > ratio_a) - (ratio_b < ratio_a);  /* Descrescător */
}

/* =============================================================================
 * ALGORITM GREEDY - ACTIVITY SELECTION
 * =============================================================================
 */

/**
 * TODO 4: Implementați algoritmul greedy de selecție a activităților
 *
 * @param activities Array de activități (va fi sortat!)
 * @param n Numărul de activități
 * @param result Pointer la structura de rezultat
 *
 * Algoritmul:
 *   1. Sortează activitățile după timpul de finish (finish time)
 *   2. Selectează prima activitate (are cel mai mic finish time)
 *   3. Pentru fiecare activitate următoare:
 *      - Dacă start-ul ei >= finish-ul ultimei selectate -> selecteaz-o
 *      - Altfel, skip (activitățile se suprapun)
 *   4. Calculează statisticile (durată totală, număr selectate)
 *
 * De ce funcționează?
 *   - Selectând activitatea cu cel mai mic finish time, lăsăm
 *     maximum de timp disponibil pentru restul activităților
 *   - Aceasta este "alegerea greedy" care garantează optim
 *
 * Complexitate: O(n log n) pentru sortare + O(n) pentru selecție
 */
void greedy_activity_selection(Activity *activities, int n, SelectionResult *result) {
    /* Inițializare rezultat */
    result->count = 0;
    result->total_duration = 0;
    result->total_profit = 0;
    
    /* Pas 1: Sortare după finish time */
    qsort(activities, n, sizeof(Activity), compare_by_finish_time);
    
    /* YOUR CODE HERE */
    
    /* Pas 2: Selectare greedy */
    int last_finish = 0;  /* Timpul de finish al ultimei activități selectate */
    
    for (int i = 0; i < n; i++) {
        /* TODO: Verificați dacă activitatea curentă este compatibilă */
        /* (adică start >= last_finish) */
        
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
 * BONUS: WEIGHTED JOB SCHEDULING (folosind DP, nu doar Greedy)
 * =============================================================================
 */

/**
 * BONUS TODO: Implementați Weighted Job Scheduling
 *
 * Când activitățile au profituri diferite, greedy simplu NU garantează optim.
 * Se folosește Programare Dinamică.
 *
 * Ideea:
 *   - dp[i] = profitul maxim pentru primele i activități
 *   - Pentru fiecare activitate i, avem două opțiuni:
 *     1. Nu o includem: dp[i] = dp[i-1]
 *     2. O includem: dp[i] = profit[i] + dp[j], unde j este ultima
 *        activitate compatibilă înainte de i
 *
 * Complexitate: O(n²) sau O(n log n) cu căutare binară
 */
int weighted_job_scheduling(Activity *activities, int n) {
    /* Sortare după finish time */
    qsort(activities, n, sizeof(Activity), compare_by_finish_time);
    
    int *dp = (int *)malloc(n * sizeof(int));
    if (dp == NULL) return -1;
    
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
void print_timeline(const Activity *activities, const SelectionResult *result) {
    printf("\n  Timeline vizual (ora 8:00 - 18:00):\n\n");
    
    /* Header */
    printf("  Ora:  ");
    for (int h = 8; h <= 18; h++) {
        printf("%-4d", h);
    }
    printf("\n        ");
    for (int h = 8; h <= 18; h++) {
        printf("|   ");
    }
    printf("\n");
    
    /* Activități selectate */
    for (int i = 0; i < result->count; i++) {
        int idx = result->selected_indices[i];
        const Activity *act = &activities[idx];
        
        printf("        ");
        for (int t = 8 * 60; t < 18 * 60; t += 15) {  /* Incrementăm din 15 în 15 minute */
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
    printf("║  EXERCIȚIUL 1: PLANIFICATOR DE ACTIVITĂȚI (GREEDY)            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    Activity activities[MAX_ACTIVITIES];
    int n = 0;
    bool success;
    
    /* Citire date */
    if (argc > 1) {
        /* Citire din fișier */
        printf("  Citire din fișierul: %s\n\n", argv[1]);
        success = read_activities_from_file(argv[1], activities, &n);
    } else {
        /* Citire de la stdin (pentru testare automată) */
        printf("  Citire de la stdin...\n");
        printf("  (Introduceți numărul de activități, apoi fiecare activitate)\n\n");
        success = read_activities_from_stdin(activities, &n);
    }
    
    if (!success) {
        fprintf(stderr, "Eroare la citirea datelor!\n");
        return 1;
    }
    
    /* Afișare activități citite */
    printf("  Activități citite (%d):\n", n);
    printf("  ─────────────────────────────────────────────────────\n");
    for (int i = 0; i < n; i++) {
        print_activity(&activities[i]);
    }
    
    /* Aplicare algoritm greedy */
    SelectionResult result;
    greedy_activity_selection(activities, n, &result);
    
    /* Afișare rezultat */
    print_selection_result(activities, &result);
    print_timeline(activities, &result);
    
    /* Output pentru testare automată (format simplu) */
    printf("\n  Output pentru verificare automată:\n");
    printf("  %d\n", result.count);
    for (int i = 0; i < result.count; i++) {
        int idx = result->selected_indices[i];
        printf("  %s\n", activities[idx].name);
    }
    
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════════\n");
    printf("  Exercițiu completat! Verificați corectitudinea implementării.\n");
    printf("═══════════════════════════════════════════════════════════════════\n");
    printf("\n");
    
    return 0;
}
