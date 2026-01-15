/**
 * =============================================================================
 * EXERCIȚIU 2: Priority Queue pentru Task Scheduler
 * =============================================================================
 *
 * OBIECTIV:
 *   Implementarea unui scheduler de task-uri cu 4 nivele de prioritate
 *   folosind un array de linked-list queues. Task-urile cu prioritate mai
 *   mare sunt executate primele.
 *
 * CERINȚE:
 *   1. Completați structura Task și TaskNode
 *   2. Implementați operațiile pentru linked-list queue
 *   3. Implementați priority queue cu array de queues
 *   4. Adăugați round-robin pentru task-uri cu aceeași prioritate
 *   5. Implementați aging pentru prevenirea starvation
 *
 * EXEMPLU INPUT (din data/tasks.txt):
 *   SCHEDULE CRITICAL System_backup
 *   SCHEDULE NORMAL Email_check
 *   SCHEDULE LOW Log_cleanup
 *   EXECUTE
 *   STATUS
 *
 * OUTPUT AȘTEPTAT:
 *   [+] Task #1 programat: System_backup (CRITICAL)
 *   [+] Task #2 programat: Email_check (NORMAL)
 *   [+] Task #3 programat: Log_cleanup (LOW)
 *   [✓] Executăm Task #1: System_backup (CRITICAL)
 *   [i] Task-uri în așteptare: 2
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -o exercise2 exercise2.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

/* =============================================================================
 * CONSTANTE
 * =============================================================================
 */

#define MAX_DESCRIPTION_LENGTH 100
#define NUM_PRIORITIES 4
#define AGING_THRESHOLD 5  /* După câte execuții se face aging */

/* Culori pentru terminal */
#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_RED     "\033[31m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_MAGENTA "\033[35m"

/* =============================================================================
 * TIPURI DE DATE
 * =============================================================================
 */

/**
 * Nivele de prioritate (0 = cea mai mare)
 */
typedef enum {
    PRIORITY_CRITICAL = 0,
    PRIORITY_HIGH = 1,
    PRIORITY_NORMAL = 2,
    PRIORITY_LOW = 3
} Priority;

/**
 * Nume pentru priorități (pentru afișare)
 */
const char *priority_names[] = {"CRITICAL", "HIGH", "NORMAL", "LOW"};
const char *priority_colors[] = {COLOR_RED, COLOR_MAGENTA, COLOR_YELLOW, COLOR_GREEN};

/**
 * TODO 1: Completați structura Task
 *
 * Un task trebuie să conțină:
 * - id: identificator unic (int)
 * - description: descrierea task-ului (șir de caractere)
 * - priority: nivelul de prioritate (Priority)
 * - created_at: momentul creării (time_t)
 * - wait_cycles: câte cicluri de execuție a așteptat (pentru aging)
 */
typedef struct {
    int id;
    char description[MAX_DESCRIPTION_LENGTH];  /* TODO 1a: descrierea task-ului */
    Priority priority;                          /* TODO 1b: nivelul de prioritate */
    time_t created_at;                          /* TODO 1c: momentul creării */
    int wait_cycles;                            /* TODO 1d: cicluri de așteptare */
} Task;

/**
 * TODO 2: Completați structura TaskNode pentru linked list
 *
 * Un nod conține:
 * - task: datele task-ului
 * - next: pointer la următorul nod
 */
typedef struct TaskNode {
    Task task;
    struct TaskNode *next;               /* TODO 2: pointer la următorul nod */
} TaskNode;

/**
 * TODO 3: Completați structura LinkedTaskQueue
 *
 * O linked list queue conține:
 * - front: pointer la primul nod
 * - rear: pointer la ultimul nod
 * - count: numărul de elemente
 */
typedef struct {
    TaskNode *front;                     /* TODO 3a: pointer la primul nod */
    TaskNode *rear;                      /* TODO 3b: pointer la ultimul nod */
    int count;                           /* TODO 3c: numărul de elemente */
} LinkedTaskQueue;

/**
 * Structura pentru Priority Queue
 * Array de 4 linked-list queues, una pentru fiecare prioritate
 */
typedef struct {
    LinkedTaskQueue levels[NUM_PRIORITIES];
    int total_tasks;
    int total_executed;
    int execution_cycles;  /* Pentru aging */
} TaskScheduler;

/* Variabilă globală pentru ID-ul task-urilor */
static int next_task_id = 1;

/* =============================================================================
 * FUNCȚII PENTRU LINKED TASK QUEUE
 * =============================================================================
 */

/**
 * TODO 4: Inițializați o linked task queue
 *
 * @param q Pointer la LinkedTaskQueue
 *
 * Setați front = NULL, rear = NULL, count = 0
 */
void ltqueue_init(LinkedTaskQueue *q) {
    /* YOUR CODE HERE */
    
}

/**
 * TODO 5: Verificați dacă linked queue este goală
 */
bool ltqueue_is_empty(const LinkedTaskQueue *q) {
    /* YOUR CODE HERE */
    return true;  /* Replace this */
}

/**
 * TODO 6: Adăugați un task la linked queue (enqueue)
 *
 * @param q Pointer la LinkedTaskQueue
 * @param task Task-ul de adăugat
 * @return true dacă a reușit, false dacă alocarea a eșuat
 *
 * Pași:
 *   1. Alocați memorie pentru un nou TaskNode
 *   2. Verificați dacă alocarea a reușit
 *   3. Copiați task-ul în noul nod
 *   4. Setați next = NULL
 *   5. Dacă queue este goală, front = rear = newNode
 *   6. Altfel, rear->next = newNode; rear = newNode
 *   7. Incrementați count
 */
bool ltqueue_enqueue(LinkedTaskQueue *q, Task task) {
    /* YOUR CODE HERE */
    
    return false;  /* Replace this */
}

/**
 * TODO 7: Eliminați și returnați primul task (dequeue)
 *
 * @param q Pointer la LinkedTaskQueue
 * @param result Pointer pentru stocarea task-ului
 * @return true dacă a reușit, false dacă queue este goală
 *
 * Pași:
 *   1. Verificați dacă queue este goală
 *   2. Salvați pointer la nodul front (temp)
 *   3. Copiați task-ul în result
 *   4. Avansați front = front->next
 *   5. Dacă front devine NULL, setați și rear = NULL
 *   6. Eliberați memoria nodului vechi (free)
 *   7. Decrementați count
 */
bool ltqueue_dequeue(LinkedTaskQueue *q, Task *result) {
    /* YOUR CODE HERE */
    
    return false;  /* Replace this */
}

/**
 * TODO 8: Vizualizați primul task fără eliminare (peek)
 */
bool ltqueue_peek(const LinkedTaskQueue *q, Task *result) {
    /* YOUR CODE HERE */
    
    return false;  /* Replace this */
}

/**
 * TODO 9: Eliberați toată memoria unei linked queue
 *
 * Parcurgeți lista și eliberați fiecare nod
 */
void ltqueue_destroy(LinkedTaskQueue *q) {
    /* YOUR CODE HERE */
    
}

/**
 * Returnează dimensiunea linked queue
 */
int ltqueue_size(const LinkedTaskQueue *q) {
    return q->count;
}

/* =============================================================================
 * FUNCȚII PENTRU TASK SCHEDULER (PRIORITY QUEUE)
 * =============================================================================
 */

/**
 * TODO 10: Inițializați scheduler-ul
 *
 * Inițializați fiecare nivel de prioritate și contoarele
 */
void scheduler_init(TaskScheduler *s) {
    /* YOUR CODE HERE */
    for (int i = 0; i < NUM_PRIORITIES; i++) {
        ltqueue_init(&s->levels[i]);
    }
    /* TODO: inițializați total_tasks, total_executed, execution_cycles */
    
}

/**
 * TODO 11: Programați un task nou
 *
 * @param s Pointer la TaskScheduler
 * @param description Descrierea task-ului
 * @param priority Nivelul de prioritate
 * @return ID-ul task-ului sau -1 dacă a eșuat
 *
 * Pași:
 *   1. Creați un Task nou
 *   2. Setați id = next_task_id și incrementați next_task_id
 *   3. Copiați description
 *   4. Setați priority și created_at = time(NULL)
 *   5. Setați wait_cycles = 0
 *   6. Adăugați în queue-ul corespunzător priorității
 *   7. Incrementați total_tasks
 */
int scheduler_add_task(TaskScheduler *s, const char *description, Priority priority) {
    /* YOUR CODE HERE */
    
    return -1;  /* Replace this */
}

/**
 * TODO 12: Executați următorul task (cel cu prioritatea cea mai mare)
 *
 * @param s Pointer la TaskScheduler
 * @param result Pointer pentru stocarea task-ului executat
 * @return true dacă a executat un task, false dacă nu există task-uri
 *
 * Pași:
 *   1. Parcurgeți nivelele de la 0 (CRITICAL) la 3 (LOW)
 *   2. La primul nivel non-gol, faceți dequeue
 *   3. Incrementați total_executed și execution_cycles
 *   4. Returnați true
 *   5. Dacă toate sunt goale, returnați false
 */
bool scheduler_execute_next(TaskScheduler *s, Task *result) {
    /* YOUR CODE HERE */
    
    return false;  /* Replace this */
}

/**
 * TODO 13: Implementați aging (opțional, pentru bonus)
 *
 * Aging previne "starvation" - task-urile cu prioritate mică care
 * așteaptă prea mult sunt promovate la o prioritate mai mare.
 *
 * Pentru fiecare AGING_THRESHOLD execuții:
 *   - Parcurgeți toate task-urile din nivelele LOW, NORMAL, HIGH
 *   - Incrementați wait_cycles
 *   - Dacă wait_cycles > AGING_THRESHOLD, promovați la nivelul superior
 */
void scheduler_apply_aging(TaskScheduler *s) {
    /* YOUR CODE HERE - BONUS */
    
}

/**
 * Verifică dacă scheduler-ul este gol
 */
bool scheduler_is_empty(const TaskScheduler *s) {
    for (int i = 0; i < NUM_PRIORITIES; i++) {
        if (!ltqueue_is_empty(&s->levels[i])) {
            return false;
        }
    }
    return true;
}

/**
 * Returnează numărul total de task-uri în așteptare
 */
int scheduler_pending_tasks(const TaskScheduler *s) {
    int total = 0;
    for (int i = 0; i < NUM_PRIORITIES; i++) {
        total += ltqueue_size(&s->levels[i]);
    }
    return total;
}

/**
 * TODO 14: Afișați toate task-urile din scheduler
 *
 * Afișați task-urile grupate pe nivele de prioritate
 */
void scheduler_print_all(const TaskScheduler *s) {
    printf("\n" COLOR_CYAN "═══ Task-uri în așteptare ═══" COLOR_RESET "\n");
    
    /* YOUR CODE HERE */
    /* Pentru fiecare nivel, parcurgeți linked list și afișați task-urile */
    /* Hint: folosiți un pointer temporar pentru a parcurge fără a modifica */
    
    for (int i = 0; i < NUM_PRIORITIES; i++) {
        printf("\n%s[%s]%s:\n", priority_colors[i], priority_names[i], COLOR_RESET);
        
        if (ltqueue_is_empty(&s->levels[i])) {
            printf("  (gol)\n");
        } else {
            /* TODO: parcurgeți și afișați task-urile */
            
        }
    }
}

/**
 * Afișează statisticile scheduler-ului
 */
void scheduler_print_stats(const TaskScheduler *s) {
    printf("\n" COLOR_CYAN "═══ Statistici Scheduler ═══" COLOR_RESET "\n");
    printf("  Task-uri în așteptare: %d\n", scheduler_pending_tasks(s));
    printf("  Task-uri programate total: %d\n", s->total_tasks);
    printf("  Task-uri executate: %d\n", s->total_executed);
    printf("  Cicluri de execuție: %d\n", s->execution_cycles);
    
    printf("\n  Distribuție pe priorități:\n");
    for (int i = 0; i < NUM_PRIORITIES; i++) {
        printf("    %s%-8s%s: %d task-uri\n", 
               priority_colors[i], priority_names[i], COLOR_RESET,
               ltqueue_size(&s->levels[i]));
    }
}

/**
 * Eliberează toată memoria scheduler-ului
 */
void scheduler_destroy(TaskScheduler *s) {
    for (int i = 0; i < NUM_PRIORITIES; i++) {
        ltqueue_destroy(&s->levels[i]);
    }
}

/* =============================================================================
 * PROCESARE COMENZI
 * =============================================================================
 */

/**
 * Convertește string la Priority
 */
Priority string_to_priority(const char *str) {
    if (strcmp(str, "CRITICAL") == 0) return PRIORITY_CRITICAL;
    if (strcmp(str, "HIGH") == 0) return PRIORITY_HIGH;
    if (strcmp(str, "NORMAL") == 0) return PRIORITY_NORMAL;
    if (strcmp(str, "LOW") == 0) return PRIORITY_LOW;
    return PRIORITY_NORMAL;  /* Default */
}

/**
 * Procesează o linie de comandă
 */
void process_command(TaskScheduler *s, const char *line) {
    char command[20];
    char arg1[50];
    char arg2[MAX_DESCRIPTION_LENGTH];
    
    if (sscanf(line, "%19s", command) != 1) {
        return;
    }
    
    if (strcmp(command, "SCHEDULE") == 0) {
        /* Format: SCHEDULE priority description */
        if (sscanf(line, "%*s %49s %99[^\n]", arg1, arg2) == 2) {
            Priority p = string_to_priority(arg1);
            int id = scheduler_add_task(s, arg2, p);
            if (id > 0) {
                printf("%s[+] Task #%d programat: %s (%s)%s\n",
                       priority_colors[p], id, arg2, priority_names[p], COLOR_RESET);
            } else {
                printf(COLOR_RED "[!] Eroare la programarea task-ului.\n" COLOR_RESET);
            }
        } else {
            printf(COLOR_RED "[!] Format: SCHEDULE <priority> <description>\n" COLOR_RESET);
        }
    }
    else if (strcmp(command, "EXECUTE") == 0) {
        Task task;
        if (scheduler_execute_next(s, &task)) {
            printf("%s[✓] Executăm Task #%d: %s (%s)%s\n",
                   priority_colors[task.priority], task.id, 
                   task.description, priority_names[task.priority], COLOR_RESET);
        } else {
            printf(COLOR_YELLOW "[i] Niciun task de executat.\n" COLOR_RESET);
        }
    }
    else if (strcmp(command, "EXECUTE_ALL") == 0) {
        printf(COLOR_YELLOW "→ Executăm toate task-urile:\n" COLOR_RESET);
        Task task;
        int count = 0;
        while (scheduler_execute_next(s, &task)) {
            printf("  %s[✓] Task #%d: %s (%s)%s\n",
                   priority_colors[task.priority], task.id,
                   task.description, priority_names[task.priority], COLOR_RESET);
            count++;
        }
        printf(COLOR_GREEN "[✓] %d task-uri executate.\n" COLOR_RESET, count);
    }
    else if (strcmp(command, "STATUS") == 0) {
        printf(COLOR_CYAN "[i] Task-uri în așteptare: %d\n" COLOR_RESET,
               scheduler_pending_tasks(s));
    }
    else if (strcmp(command, "LIST") == 0) {
        scheduler_print_all(s);
    }
    else if (strcmp(command, "STATS") == 0) {
        scheduler_print_stats(s);
    }
    else if (strcmp(command, "AGING") == 0) {
        scheduler_apply_aging(s);
        printf(COLOR_CYAN "[i] Aging aplicat.\n" COLOR_RESET);
    }
}

/* =============================================================================
 * PROGRAM PRINCIPAL
 * =============================================================================
 */

int main(int argc, char *argv[]) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════════╗\n");
    printf("║     " COLOR_CYAN "EXERCIȚIU 2: Task Scheduler cu Priority Queue" COLOR_RESET "              ║\n");
    printf("║     Array of Linked List Queues Implementation                    ║\n");
    printf("╚═══════════════════════════════════════════════════════════════════╝\n\n");
    
    TaskScheduler scheduler;
    scheduler_init(&scheduler);
    
    printf(COLOR_GREEN "✓ Scheduler inițializat (4 nivele de prioritate)\n" COLOR_RESET);
    printf("\n  Nivele: ");
    for (int i = 0; i < NUM_PRIORITIES; i++) {
        printf("%s%s%s ", priority_colors[i], priority_names[i], COLOR_RESET);
    }
    printf("\n");
    
    printf("\nComenzi disponibile:\n");
    printf("  SCHEDULE <priority> <desc> - Programează un task\n");
    printf("  EXECUTE                    - Execută următorul task\n");
    printf("  EXECUTE_ALL                - Execută toate task-urile\n");
    printf("  STATUS                     - Afișează numărul de task-uri\n");
    printf("  LIST                       - Listează toate task-urile\n");
    printf("  STATS                      - Afișează statistici\n");
    printf("  AGING                      - Aplică aging (bonus)\n");
    printf("  EXIT                       - Ieșire\n\n");
    
    /* Citim din fișier dacă este specificat */
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp != NULL) {
            printf(COLOR_YELLOW "→ Citim comenzi din: %s\n\n" COLOR_RESET, argv[1]);
            char line[300];
            while (fgets(line, sizeof(line), fp) != NULL) {
                line[strcspn(line, "\n")] = '\0';
                if (strlen(line) > 0 && line[0] != '#') {
                    printf(COLOR_CYAN "> %s\n" COLOR_RESET, line);
                    process_command(&scheduler, line);
                }
            }
            fclose(fp);
            printf("\n");
        }
    }
    
    /* Mod interactiv */
    printf(COLOR_YELLOW "→ Mod interactiv (tastați EXIT pentru a ieși):\n" COLOR_RESET);
    char line[300];
    while (1) {
        printf("\n> ");
        if (fgets(line, sizeof(line), stdin) == NULL) {
            break;
        }
        
        line[strcspn(line, "\n")] = '\0';
        
        if (strcmp(line, "EXIT") == 0 || strcmp(line, "exit") == 0) {
            break;
        }
        
        if (strlen(line) > 0) {
            process_command(&scheduler, line);
        }
    }
    
    /* Statistici finale și cleanup */
    scheduler_print_stats(&scheduler);
    scheduler_destroy(&scheduler);
    
    printf("\n" COLOR_GREEN "✓ Program terminat. Memoria eliberată.\n" COLOR_RESET "\n");
    
    return 0;
}

/* =============================================================================
 * PROVOCĂRI BONUS (Opțional)
 * =============================================================================
 *
 * 1. Implementați complet funcția scheduler_apply_aging() pentru a preveni
 *    starvation - task-urile care așteaptă prea mult sunt promovate.
 *
 * 2. Adăugați un câmp "deadline" la Task și implementați SCHEDULE_DEADLINE
 *    care programează task-uri cu deadline, procesând primele cele care
 *    expiră mai repede (Earliest Deadline First - EDF).
 *
 * 3. Implementați CANCEL <id> care elimină un task specific din queue
 *    (necesită traversare și eliminare din linked list).
 *
 * 4. Adăugați suport pentru "task dependencies" - un task poate depinde
 *    de finalizarea altuia.
 *
 * 5. Implementați Round-Robin în interiorul fiecărui nivel de prioritate:
 *    în loc să proceseze complet un task, procesează parțial și îl pune
 *    la sfârșitul queue-ului.
 *
 * =============================================================================
 */
