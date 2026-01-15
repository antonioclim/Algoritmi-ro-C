/**
 * =============================================================================
 * SOLUȚIE - TEMA 2: Browser History Navigator
 * =============================================================================
 *
 * Sistem de navigare similar cu istoricul unui browser web.
 * Funcționalități: visit, back, forward, history, clear
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -o homework2_sol homework2_sol.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#define MAX_HISTORY_SIZE 100
#define MAX_URL_LEN 256
#define MAX_CMD_LEN 512

/* =============================================================================
 * STRUCTURI DE DATE
 * =============================================================================
 */

/**
 * Structura pentru o pagină web
 */
typedef struct {
    char url[MAX_URL_LEN];
    char title[MAX_URL_LEN];
    time_t timestamp;
} Page;

/**
 * Stiva pentru istoric
 */
typedef struct {
    Page pages[MAX_HISTORY_SIZE];
    int top;
} PageStack;

/**
 * Structura browserului
 */
typedef struct {
    PageStack back_stack;      /* Istoricul pentru back */
    PageStack forward_stack;   /* Istoricul pentru forward */
    Page *current;             /* Pagina curentă */
    Page current_page;         /* Stocare pentru pagina curentă */
} Browser;

/* =============================================================================
 * FUNCȚII PENTRU STIVA DE PAGINI
 * =============================================================================
 */

void page_stack_init(PageStack *s) {
    s->top = -1;
}

bool page_stack_is_empty(const PageStack *s) {
    return s->top == -1;
}

bool page_stack_is_full(const PageStack *s) {
    return s->top >= MAX_HISTORY_SIZE - 1;
}

int page_stack_size(const PageStack *s) {
    return s->top + 1;
}

bool page_stack_push(PageStack *s, const Page *p) {
    if (page_stack_is_full(s)) {
        /* Dacă stiva e plină, elimină cel mai vechi element */
        for (int i = 0; i < s->top; i++) {
            s->pages[i] = s->pages[i + 1];
        }
        s->pages[s->top] = *p;
        return true;
    }
    s->pages[++s->top] = *p;
    return true;
}

bool page_stack_pop(PageStack *s, Page *p) {
    if (page_stack_is_empty(s)) {
        return false;
    }
    *p = s->pages[s->top--];
    return true;
}

bool page_stack_peek(const PageStack *s, Page *p) {
    if (page_stack_is_empty(s)) {
        return false;
    }
    *p = s->pages[s->top];
    return true;
}

void page_stack_clear(PageStack *s) {
    s->top = -1;
}

/* =============================================================================
 * FUNCȚII PENTRU BROWSER
 * =============================================================================
 */

/**
 * Inițializează browserul
 */
void browser_init(Browser *b) {
    page_stack_init(&b->back_stack);
    page_stack_init(&b->forward_stack);
    b->current = NULL;
}

/**
 * Vizitează o pagină nouă
 */
void browser_visit(Browser *b, const char *url) {
    /* Dacă există o pagină curentă, pune-o în back stack */
    if (b->current != NULL) {
        page_stack_push(&b->back_stack, b->current);
    }
    
    /* Șterge forward stack când vizitezi o pagină nouă */
    page_stack_clear(&b->forward_stack);
    
    /* Setează pagina curentă */
    strncpy(b->current_page.url, url, MAX_URL_LEN - 1);
    b->current_page.url[MAX_URL_LEN - 1] = '\0';
    
    /* Generează un titlu simplu din URL */
    strncpy(b->current_page.title, url, MAX_URL_LEN - 1);
    b->current_page.title[MAX_URL_LEN - 1] = '\0';
    
    b->current_page.timestamp = time(NULL);
    b->current = &b->current_page;
    
    printf("\033[0;32mNavigat la: %s\033[0m\n", url);
}

/**
 * Mergi înapoi n pagini
 */
bool browser_back(Browser *b, int n) {
    if (b->current == NULL || page_stack_is_empty(&b->back_stack)) {
        printf("\033[0;33mNu poți merge mai înapoi.\033[0m\n");
        return false;
    }
    
    for (int i = 0; i < n; i++) {
        if (page_stack_is_empty(&b->back_stack)) {
            printf("\033[0;33mNu mai sunt pagini în istoric.\033[0m\n");
            break;
        }
        
        /* Pune pagina curentă în forward */
        page_stack_push(&b->forward_stack, b->current);
        
        /* Ia pagina din back */
        Page prev;
        page_stack_pop(&b->back_stack, &prev);
        b->current_page = prev;
        b->current = &b->current_page;
    }
    
    printf("\033[0;36mÎnapoi la: %s\033[0m\n", b->current->url);
    return true;
}

/**
 * Mergi înainte n pagini
 */
bool browser_forward(Browser *b, int n) {
    if (page_stack_is_empty(&b->forward_stack)) {
        printf("\033[0;33mNu poți merge mai înainte.\033[0m\n");
        return false;
    }
    
    for (int i = 0; i < n; i++) {
        if (page_stack_is_empty(&b->forward_stack)) {
            printf("\033[0;33mNu mai sunt pagini în forward.\033[0m\n");
            break;
        }
        
        /* Pune pagina curentă în back */
        if (b->current != NULL) {
            page_stack_push(&b->back_stack, b->current);
        }
        
        /* Ia pagina din forward */
        Page next;
        page_stack_pop(&b->forward_stack, &next);
        b->current_page = next;
        b->current = &b->current_page;
    }
    
    printf("\033[0;36mÎnainte la: %s\033[0m\n", b->current->url);
    return true;
}

/**
 * Afișează pagina curentă
 */
void browser_show_current(const Browser *b) {
    if (b->current == NULL) {
        printf("Nu există pagină curentă.\n");
        return;
    }
    
    printf("\033[0;32mPagina curentă: %s\033[0m\n", b->current->url);
    
    char time_str[64];
    struct tm *tm_info = localtime(&b->current->timestamp);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
    printf("Vizitată la: %s\n", time_str);
}

/**
 * Afișează istoricul complet
 */
void browser_show_history(const Browser *b) {
    printf("\n\033[0;35m═══════════════════════════════════════════════════════════════\033[0m\n");
    printf("\033[0;35m                         ISTORIC BROWSER                        \033[0m\n");
    printf("\033[0;35m═══════════════════════════════════════════════════════════════\033[0m\n\n");
    
    int total = page_stack_size(&b->back_stack) + 
                (b->current != NULL ? 1 : 0) + 
                page_stack_size(&b->forward_stack);
    
    if (total == 0) {
        printf("Istoricul este gol.\n\n");
        return;
    }
    
    int index = 1;
    
    /* Afișează paginile din back (de la cea mai veche) */
    for (int i = 0; i <= b->back_stack.top; i++) {
        printf("  %2d. %s\n", index++, b->back_stack.pages[i].url);
    }
    
    /* Afișează pagina curentă */
    if (b->current != NULL) {
        printf("  %2d. \033[0;32m%s [CURENT]\033[0m\n", index++, b->current->url);
    }
    
    /* Afișează paginile din forward (de la cea mai apropiată) */
    for (int i = b->forward_stack.top; i >= 0; i--) {
        printf("  %2d. \033[0;90m%s (forward)\033[0m\n", index++, b->forward_stack.pages[i].url);
    }
    
    printf("\n");
    printf("Total: %d pagini\n", total);
    printf("  - Back: %d pagini\n", page_stack_size(&b->back_stack));
    printf("  - Forward: %d pagini\n", page_stack_size(&b->forward_stack));
    printf("\n");
}

/**
 * Șterge istoricul
 */
void browser_clear(Browser *b) {
    page_stack_clear(&b->back_stack);
    page_stack_clear(&b->forward_stack);
    b->current = NULL;
    printf("\033[0;33mIstoricul a fost șters.\033[0m\n");
}

/* =============================================================================
 * AJUTOR
 * =============================================================================
 */

void print_help(void) {
    printf("\n");
    printf("\033[0;36mComenzi disponibile:\033[0m\n");
    printf("  \033[0;33mvisit <url>\033[0m      - Vizitează o pagină nouă\n");
    printf("  \033[0;33mback [n]\033[0m         - Mergi înapoi n pagini (implicit 1)\n");
    printf("  \033[0;33mforward [n]\033[0m      - Mergi înainte n pagini (implicit 1)\n");
    printf("  \033[0;33mcurrent\033[0m          - Afișează pagina curentă\n");
    printf("  \033[0;33mhistory\033[0m          - Afișează istoricul complet\n");
    printf("  \033[0;33mclear\033[0m            - Șterge istoricul\n");
    printf("  \033[0;33mhelp\033[0m             - Afișează acest mesaj\n");
    printf("  \033[0;33mquit / q\033[0m         - Ieșire\n");
    printf("\n");
}

/* =============================================================================
 * PROGRAMUL PRINCIPAL
 * =============================================================================
 */

int main(void) {
    Browser browser;
    browser_init(&browser);
    
    char input[MAX_CMD_LEN];
    char cmd[64];
    char arg[MAX_URL_LEN];
    int n;
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║              BROWSER HISTORY SIMULATOR                        ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\nTastează 'help' pentru lista de comenzi.\n\n");
    
    while (1) {
        printf("\033[0;34mbrowser>\033[0m ");
        fflush(stdout);
        
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }
        
        /* Elimină newline */
        size_t len = strlen(input);
        if (len > 0 && input[len-1] == '\n') input[len-1] = '\0';
        if (strlen(input) == 0) continue;
        
        /* Parsează comanda */
        cmd[0] = '\0';
        arg[0] = '\0';
        n = 1;
        
        sscanf(input, "%63s %255s", cmd, arg);
        
        /* Procesează comanda */
        if (strcmp(cmd, "visit") == 0) {
            if (strlen(arg) == 0) {
                printf("\033[0;31mEroare: Specifică un URL.\033[0m\n");
            } else {
                browser_visit(&browser, arg);
            }
        }
        else if (strcmp(cmd, "back") == 0) {
            if (strlen(arg) > 0) {
                n = atoi(arg);
                if (n < 1) n = 1;
            }
            browser_back(&browser, n);
        }
        else if (strcmp(cmd, "forward") == 0) {
            if (strlen(arg) > 0) {
                n = atoi(arg);
                if (n < 1) n = 1;
            }
            browser_forward(&browser, n);
        }
        else if (strcmp(cmd, "current") == 0) {
            browser_show_current(&browser);
        }
        else if (strcmp(cmd, "history") == 0) {
            browser_show_history(&browser);
        }
        else if (strcmp(cmd, "clear") == 0) {
            browser_clear(&browser);
        }
        else if (strcmp(cmd, "help") == 0) {
            print_help();
        }
        else if (strcmp(cmd, "quit") == 0 || strcmp(cmd, "q") == 0) {
            break;
        }
        else {
            printf("\033[0;31mComandă necunoscută: %s\033[0m\n", cmd);
            printf("Tastează 'help' pentru lista de comenzi.\n");
        }
        
        printf("\n");
    }
    
    printf("\nLa revedere!\n\n");
    return 0;
}
