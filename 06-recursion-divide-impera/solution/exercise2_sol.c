/**
 * =============================================================================
 * SOLUȚIE EXERCIȚIU 2: Priority Queue pentru Task Scheduler
 * =============================================================================
 *
 * Aceasta este soluția completă pentru exercițiul 2.
 * DOAR PENTRU INSTRUCTORI - Nu distribuiți studenților!
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -o exercise2_sol exercise2_sol.c
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
#define AGING_THRESHOLD 5

/* Culori pentru terminal */
#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_RED     "\033[31m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_MAGENTA "\033[35m"

/* =============================================================================
 * TIPURI DE DATE - SOLUȚIE COMPLETĂ
 * =============================================================================
 */

typedef enum {
    PRIORITY_CRITICAL = 0,
    PRIORITY_HIGH = 1,
    PRIORITY_NORMAL = 2,
    PRIORITY_LOW = 3
} Priority;

const char *priority_names[] = {"CRITICAL", "HIGH", "NORMAL", "LOW"};
const char *priority_colors[] = {COLOR_RED, COLOR_MAGENTA, COLOR_YELLOW, COLOR_GREEN};

/**
 * Structura Task - SOLUȚIE
 */
typedef struct {
    int id;
    char description[MAX_DESCRIPTION_LENGTH];
    Priority priority;
    time_t created_at;
    int wait_cycles;
} Task;

/**
 * Structura TaskNode - SOLUȚIE
 */
typedef struct TaskNode {
    Task task;
    struct TaskNode *next;
} TaskNode;

/**
 * Structura LinkedTaskQueue - SOLUȚIE
 */
typedef struct {
    TaskNode *front;
    TaskNode *rear;
    int count;
} LinkedTaskQueue;

/**
 * Structura TaskScheduler - Priority Queue
 */
typedef struct {
    LinkedTaskQueue levels[NUM_PRIORITIES];
    int total_tasks;
    int total_executed;
    int execution_cycles;
} TaskScheduler;

static int next_task_id = 1;

/* =============================================================================
 * FUNCȚII PENTRU LINKED TASK QUEUE - SOLUȚII
 * =============================================================================
 */

/**
 * Inițializare linked queue - SOLUȚIE
 */
void ltqueue_init(LinkedTaskQueue *q) {
    q->front = NULL;
    q->rear = NULL;
    q->count = 0;
}

/**
 * isEmpty - SOLUȚIE
 */
bool ltqueue_is_empty(const LinkedTaskQueue *q) {
    return q->front == NULL;
}

/**
 * Enqueue - SOLUȚIE
 */
bool ltqueue_enqueue(LinkedTaskQueue *q, Task task) {
    TaskNode *newNode = (TaskNode *)malloc(sizeof(TaskNode));
    if (newNode == NULL) {
        return false;
    }
    
    newNode->task = task;
    newNode->next = NULL;
    
    if (q->rear == NULL) {
        q->front = newNode;
        q->rear = newNode;
    } else {
        q->rear->next = newNode;
        q->rear = newNode;
    }
    
    q->count++;
    return true;
}

/**
 * Dequeue - SOLUȚIE
 */
bool ltqueue_dequeue(LinkedTaskQueue *q, Task *result) {
    if (ltqueue_is_empty(q)) {
        return false;
    }
    
    TaskNode *temp = q->front;
    *result = temp->task;
    
    q->front = q->front->next;
    
    if (q->front == NULL) {
        q->rear = NULL;
    }
    
    free(temp);
    q->count--;
    
    return true;
}

/**
 * Peek - SOLUȚIE
 */
bool ltqueue_peek(const LinkedTaskQueue *q, Task *result) {
    if (ltqueue_is_empty(q)) {
        return false;
    }
    *result = q->front->task;
    return true;
}

/**
 * Destroy - SOLUȚIE
 */
void ltqueue_destroy(LinkedTaskQueue *q) {
    TaskNode *current = q->front;
    while (current != NULL) {
        TaskNode *temp = current;
        current = current->next;
        free(temp);
    }
    q->front = NULL;
    q->rear = NULL;
    q->count = 0;
}

/**
 * Size
 */
int ltqueue_size(const LinkedTaskQueue *q) {
    return q->count;
}

/* =============================================================================
 * FUNCȚII PENTRU TASK SCHEDULER - SOLUȚII
 * =============================================================================
 */

/**
 * Inițializare scheduler - SOLUȚIE
 */
void scheduler_init(TaskScheduler *s) {
    for (int i = 0; i < NUM_PRIORITIES; i++) {
        ltqueue_init(&s->levels[i]);
    }
    s->total_tasks = 0;
    s->total_executed = 0;
    s->execution_cycles = 0;
}

/**
 * Adăugare task - SOLUȚIE
 */
int scheduler_add_task(TaskScheduler *s, const char *description, Priority priority) {
    Task task;
    task.id = next_task_id++;
    
    strncpy(task.description, description, MAX_DESCRIPTION_LENGTH - 1);
    task.description[MAX_DESCRIPTION_LENGTH - 1] = '\0';
    
    task.priority = priority;
    task.created_at = time(NULL);
    task.wait_cycles = 0;
    
    if (!ltqueue_enqueue(&s->levels[priority], task)) {
        return -1;
    }
    
    s->total_tasks++;
    return task.id;
}

/**
 * Executare următorul task - SOLUȚIE
 */
bool scheduler_execute_next(TaskScheduler *s, Task *result) {
    for (int i = 0; i < NUM_PRIORITIES; i++) {
        if (!ltqueue_is_empty(&s->levels[i])) {
            if (ltqueue_dequeue(&s->levels[i], result)) {
                s->total_executed++;
                s->execution_cycles++;
                return true;
            }
        }
    }
    return false;
}

/**
 * Aging - SOLUȚIE BONUS
 * Promovează task-urile care așteaptă prea mult
 */
void scheduler_apply_aging(TaskScheduler *s) {
    /* Pentru fiecare nivel (în afară de CRITICAL) */
    for (int level = NUM_PRIORITIES - 1; level > 0; level--) {
        LinkedTaskQueue *queue = &s->levels[level];
        
        if (ltqueue_is_empty(queue)) {
            continue;
        }
        
        /* Parcurgem task-urile și incrementăm wait_cycles */
        TaskNode *current = queue->front;
        TaskNode *prev = NULL;
        
        while (current != NULL) {
            current->task.wait_cycles++;
            
            /* Dacă a așteptat prea mult, promovăm */
            if (current->task.wait_cycles > AGING_THRESHOLD) {
                /* Eliminăm din lista curentă */
                TaskNode *to_promote = current;
                
                if (prev == NULL) {
                    queue->front = current->next;
                } else {
                    prev->next = current->next;
                }
                
                if (current == queue->rear) {
                    queue->rear = prev;
                }
                
                queue->count--;
                current = current->next;
                
                /* Resetăm wait_cycles și promovăm */
                to_promote->task.wait_cycles = 0;
                to_promote->task.priority = level - 1;
                to_promote->next = NULL;
                
                /* Adăugăm la nivelul superior */
                LinkedTaskQueue *upper = &s->levels[level - 1];
                if (upper->rear == NULL) {
                    upper->front = to_promote;
                    upper->rear = to_promote;
                } else {
                    upper->rear->next = to_promote;
                    upper->rear = to_promote;
                }
                upper->count++;
                
                printf(COLOR_MAGENTA "[↑] Task #%d promovat de la %s la %s (aging)\n" COLOR_RESET,
                       to_promote->task.id, priority_names[level], priority_names[level - 1]);
            } else {
                prev = current;
                current = current->next;
            }
        }
    }
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
 * Task-uri în așteptare
 */
int scheduler_pending_tasks(const TaskScheduler *s) {
    int total = 0;
    for (int i = 0; i < NUM_PRIORITIES; i++) {
        total += ltqueue_size(&s->levels[i]);
    }
    return total;
}

/**
 * Afișare toate task-urile - SOLUȚIE
 */
void scheduler_print_all(const TaskScheduler *s) {
    printf("\n" COLOR_CYAN "═══ Task-uri în așteptare ═══" COLOR_RESET "\n");
    
    for (int i = 0; i < NUM_PRIORITIES; i++) {
        printf("\n%s[%s]%s:\n", priority_colors[i], priority_names[i], COLOR_RESET);
        
        if (ltqueue_is_empty(&s->levels[i])) {
            printf("  (gol)\n");
        } else {
            TaskNode *current = s->levels[i].front;
            int index = 1;
            while (current != NULL) {
                printf("  %d. Task #%d: %s (wait_cycles: %d)\n",
                       index++, current->task.id, 
                       current->task.description,
                       current->task.wait_cycles);
                current = current->next;
            }
        }
    }
}

/**
 * Afișare statistici
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
 * Eliberare memorie
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

Priority string_to_priority(const char *str) {
    if (strcmp(str, "CRITICAL") == 0) return PRIORITY_CRITICAL;
    if (strcmp(str, "HIGH") == 0) return PRIORITY_HIGH;
    if (strcmp(str, "NORMAL") == 0) return PRIORITY_NORMAL;
    if (strcmp(str, "LOW") == 0) return PRIORITY_LOW;
    return PRIORITY_NORMAL;
}

void process_command(TaskScheduler *s, const char *line) {
    char command[20];
    char arg1[50];
    char arg2[MAX_DESCRIPTION_LENGTH];
    
    if (sscanf(line, "%19s", command) != 1) {
        return;
    }
    
    if (strcmp(command, "SCHEDULE") == 0) {
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
    printf("║     " COLOR_CYAN "SOLUȚIE EXERCIȚIU 2: Task Scheduler" COLOR_RESET "                        ║\n");
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
    printf("  AGING                      - Aplică aging\n");
    printf("  EXIT                       - Ieșire\n\n");
    
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
    
    scheduler_print_stats(&scheduler);
    scheduler_destroy(&scheduler);
    
    printf("\n" COLOR_GREEN "✓ Program terminat. Memoria eliberată.\n" COLOR_RESET "\n");
    
    return 0;
}
