/**
 * =============================================================================
 * SĂPTĂMÂNA 5: STIVE (STACK)
 * Exemplu Complet Demonstrativ
 * =============================================================================
 *
 * Acest exemplu demonstrează:
 *   1. Implementarea stivei pe vector static
 *   2. Implementarea stivei pe listă înlănțuită
 *   3. Verificarea parantezelor echilibrate
 *   4. Evaluarea expresiilor postfixe (RPN)
 *   5. Conversie infix → postfix (Shunting Yard simplificat)
 *   6. Aplicație practică: Undo/Redo
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -o example1 example1.c
 * Utilizare: ./example1
 *
 * Autor: Curs ATP - Universitatea de Științe Economice
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

/* =============================================================================
 * CONSTANTE ȘI TIPURI COMUNE
 * =============================================================================
 */

#define MAX_SIZE 100
#define MAX_EXPR_LEN 256

/* Coduri de culoare ANSI pentru afișare frumoasă */
#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[0;32m"
#define COLOR_YELLOW  "\033[0;33m"
#define COLOR_BLUE    "\033[0;34m"
#define COLOR_RED     "\033[0;31m"
#define COLOR_CYAN    "\033[0;36m"
#define COLOR_MAGENTA "\033[0;35m"

/* =============================================================================
 * PARTEA 1: STIVĂ PE VECTOR STATIC (Array-Based Stack)
 * =============================================================================
 */

/**
 * Structura stivei implementate pe vector
 * 
 * Avantaje:
 *   - Acces rapid O(1) pentru toate operațiile
 *   - Cache-friendly (elemente contigue în memorie)
 *   - Fără overhead de pointeri
 * 
 * Dezavantaje:
 *   - Dimensiune fixă (trebuie știută la compilare)
 *   - Poate irosi memorie dacă nu se folosește complet
 */
typedef struct {
    int data[MAX_SIZE];  /* Vectorul de stocare */
    int top;             /* Indexul vârfului (-1 = stivă goală) */
} ArrayStack;

/**
 * Inițializează stiva pe vector
 * @param s Pointer la stivă
 */
void array_stack_init(ArrayStack *s) {
    s->top = -1;
}

/**
 * Verifică dacă stiva este goală
 * @param s Pointer la stivă (const - nu modifică)
 * @return true dacă stiva este goală
 */
bool array_stack_is_empty(const ArrayStack *s) {
    return s->top == -1;
}

/**
 * Verifică dacă stiva este plină
 * @param s Pointer la stivă
 * @return true dacă stiva este plină
 */
bool array_stack_is_full(const ArrayStack *s) {
    return s->top == MAX_SIZE - 1;
}

/**
 * Returnează numărul de elemente din stivă
 * @param s Pointer la stivă
 * @return Numărul de elemente
 */
int array_stack_size(const ArrayStack *s) {
    return s->top + 1;
}

/**
 * Adaugă un element în vârful stivei (push)
 * @param s Pointer la stivă
 * @param value Valoarea de adăugat
 * @return true dacă operația a reușit, false la stack overflow
 */
bool array_stack_push(ArrayStack *s, int value) {
    if (array_stack_is_full(s)) {
        fprintf(stderr, COLOR_RED "Eroare: Stack Overflow!\n" COLOR_RESET);
        return false;
    }
    s->top++;
    s->data[s->top] = value;
    return true;
}

/**
 * Elimină și returnează elementul din vârful stivei (pop)
 * @param s Pointer la stivă
 * @param value Pointer unde se va stoca valoarea eliminată
 * @return true dacă operația a reușit, false la stack underflow
 */
bool array_stack_pop(ArrayStack *s, int *value) {
    if (array_stack_is_empty(s)) {
        fprintf(stderr, COLOR_RED "Eroare: Stack Underflow!\n" COLOR_RESET);
        return false;
    }
    *value = s->data[s->top];
    s->top--;
    return true;
}

/**
 * Returnează elementul din vârf fără a-l elimina (peek)
 * @param s Pointer la stivă
 * @param value Pointer unde se va stoca valoarea
 * @return true dacă operația a reușit
 */
bool array_stack_peek(const ArrayStack *s, int *value) {
    if (array_stack_is_empty(s)) {
        return false;
    }
    *value = s->data[s->top];
    return true;
}

/**
 * Afișează conținutul stivei (pentru debugging)
 * @param s Pointer la stivă
 * @param name Numele stivei pentru afișare
 */
void array_stack_print(const ArrayStack *s, const char *name) {
    printf("%s[size=%d]: ", name, array_stack_size(s));
    if (array_stack_is_empty(s)) {
        printf("(goală)\n");
        return;
    }
    printf("TOP → ");
    for (int i = s->top; i >= 0; i--) {
        printf("%d", s->data[i]);
        if (i > 0) printf(" → ");
    }
    printf(" → BAZĂ\n");
}

/* =============================================================================
 * PARTEA 2: STIVĂ PE LISTĂ ÎNLĂNȚUITĂ (Linked Stack)
 * =============================================================================
 */

/**
 * Nodul listei înlănțuite
 */
typedef struct Node {
    int data;           /* Datele stocate */
    struct Node *next;  /* Pointer la următorul nod */
} Node;

/**
 * Structura stivei pe listă înlănțuită
 * 
 * Avantaje:
 *   - Dimensiune dinamică (crește/scade la nevoie)
 *   - Nu irosește memorie
 * 
 * Dezavantaje:
 *   - Overhead de memorie pentru pointeri
 *   - Cache miss-uri frecvente (noduri dispersate în memorie)
 */
typedef struct {
    Node *top;      /* Pointer la vârful stivei */
    size_t size;    /* Numărul de elemente */
} LinkedStack;

/**
 * Inițializează stiva pe listă
 */
void linked_stack_init(LinkedStack *s) {
    s->top = NULL;
    s->size = 0;
}

/**
 * Verifică dacă stiva este goală
 */
bool linked_stack_is_empty(const LinkedStack *s) {
    return s->top == NULL;
}

/**
 * Returnează numărul de elemente
 */
size_t linked_stack_size(const LinkedStack *s) {
    return s->size;
}

/**
 * Adaugă un element în stivă (push)
 * @return true dacă alocarea a reușit
 */
bool linked_stack_push(LinkedStack *s, int value) {
    /* Alocă memorie pentru noul nod */
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (new_node == NULL) {
        fprintf(stderr, COLOR_RED "Eroare: Alocare memorie eșuată!\n" COLOR_RESET);
        return false;
    }
    
    /* Inițializează noul nod */
    new_node->data = value;
    new_node->next = s->top;  /* Leagă de vechiul vârf */
    
    /* Actualizează vârful */
    s->top = new_node;
    s->size++;
    
    return true;
}

/**
 * Elimină și returnează elementul din vârf (pop)
 */
bool linked_stack_pop(LinkedStack *s, int *value) {
    if (linked_stack_is_empty(s)) {
        fprintf(stderr, COLOR_RED "Eroare: Stack Underflow!\n" COLOR_RESET);
        return false;
    }
    
    /* Salvează referința la nodul de șters */
    Node *temp = s->top;
    *value = temp->data;
    
    /* Mută vârful */
    s->top = temp->next;
    s->size--;
    
    /* Eliberează memoria */
    free(temp);
    
    return true;
}

/**
 * Returnează elementul din vârf fără eliminare (peek)
 */
bool linked_stack_peek(const LinkedStack *s, int *value) {
    if (linked_stack_is_empty(s)) {
        return false;
    }
    *value = s->top->data;
    return true;
}

/**
 * Eliberează toată memoria ocupată de stivă
 * IMPORTANT: Trebuie apelată pentru a evita memory leaks!
 */
void linked_stack_destroy(LinkedStack *s) {
    Node *current = s->top;
    while (current != NULL) {
        Node *temp = current;
        current = current->next;
        free(temp);
    }
    s->top = NULL;
    s->size = 0;
}

/**
 * Afișează conținutul stivei
 */
void linked_stack_print(const LinkedStack *s, const char *name) {
    printf("%s[size=%zu]: ", name, linked_stack_size(s));
    if (linked_stack_is_empty(s)) {
        printf("(goală)\n");
        return;
    }
    printf("TOP → ");
    for (Node *n = s->top; n != NULL; n = n->next) {
        printf("%d", n->data);
        if (n->next != NULL) printf(" → ");
    }
    printf(" → NULL\n");
}

/* =============================================================================
 * PARTEA 3: VERIFICAREA PARANTEZELOR
 * =============================================================================
 */

/**
 * Stivă pentru caractere (pentru paranteze)
 */
typedef struct {
    char data[MAX_SIZE];
    int top;
} CharStack;

void char_stack_init(CharStack *s) { s->top = -1; }
bool char_stack_is_empty(const CharStack *s) { return s->top == -1; }
bool char_stack_push(CharStack *s, char c) {
    if (s->top >= MAX_SIZE - 1) return false;
    s->data[++s->top] = c;
    return true;
}
bool char_stack_pop(CharStack *s, char *c) {
    if (s->top < 0) return false;
    *c = s->data[s->top--];
    return true;
}

/**
 * Verifică dacă două paranteze se potrivesc
 */
bool brackets_match(char open, char close) {
    return (open == '(' && close == ')') ||
           (open == '[' && close == ']') ||
           (open == '{' && close == '}');
}

/**
 * Verifică dacă un caracter este paranteză de deschidere
 */
bool is_opening(char c) {
    return c == '(' || c == '[' || c == '{';
}

/**
 * Verifică dacă un caracter este paranteză de închidere
 */
bool is_closing(char c) {
    return c == ')' || c == ']' || c == '}';
}

/**
 * Verifică dacă parantezele din expresie sunt echilibrate
 * 
 * @param expression Șirul de caractere de verificat
 * @return Poziția erorii (0-indexed) sau -1 dacă expresia este validă
 * 
 * Algoritm:
 *   1. Pentru fiecare caracter din expresie:
 *      - Dacă e paranteză de deschidere → push pe stivă
 *      - Dacă e paranteză de închidere → pop și verifică potrivirea
 *   2. La final, stiva trebuie să fie goală
 */
int check_brackets(const char *expression) {
    CharStack stack;
    char_stack_init(&stack);
    
    for (int i = 0; expression[i] != '\0'; i++) {
        char c = expression[i];
        
        if (is_opening(c)) {
            char_stack_push(&stack, c);
        }
        else if (is_closing(c)) {
            char top_char;
            if (!char_stack_pop(&stack, &top_char)) {
                return i;  /* Stack gol - paranteză de închidere fără pereche */
            }
            if (!brackets_match(top_char, c)) {
                return i;  /* Tip nepotrivit */
            }
        }
    }
    
    /* Dacă mai sunt elemente în stivă = paranteze neînchise */
    if (!char_stack_is_empty(&stack)) {
        return (int)strlen(expression);  /* Eroare la sfârșit */
    }
    
    return -1;  /* Succes */
}

/* =============================================================================
 * PARTEA 4: EVALUAREA EXPRESIILOR POSTFIXE (RPN)
 * =============================================================================
 */

/**
 * Verifică dacă un caracter este operator
 */
bool is_operator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '%';
}

/**
 * Aplică operatorul pe doi operanzi
 */
int apply_operator(char op, int a, int b) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': 
            if (b == 0) {
                fprintf(stderr, COLOR_RED "Eroare: Împărțire la zero!\n" COLOR_RESET);
                return 0;
            }
            return a / b;
        case '%': return a % b;
        default: return 0;
    }
}

/**
 * Evaluează o expresie în notație postfixă (Reverse Polish Notation)
 * 
 * @param expression Expresia postfixă (ex: "3 4 + 5 *")
 * @return Rezultatul evaluării
 * 
 * Algoritm:
 *   - Pentru fiecare token:
 *     - Dacă e număr → push
 *     - Dacă e operator → pop doi operanzi, aplică, push rezultat
 *   - Returnează ultimul element din stivă
 */
int evaluate_postfix(const char *expression) {
    ArrayStack stack;
    array_stack_init(&stack);
    
    char token[32];
    int token_idx = 0;
    
    for (int i = 0; ; i++) {
        char c = expression[i];
        
        /* Construiește token-ul (număr sau operator) */
        if (c != ' ' && c != '\0') {
            token[token_idx++] = c;
        }
        else if (token_idx > 0) {
            token[token_idx] = '\0';
            token_idx = 0;
            
            /* Verifică dacă e număr */
            if (isdigit(token[0]) || (token[0] == '-' && isdigit(token[1]))) {
                int num = atoi(token);
                array_stack_push(&stack, num);
                printf("  Push: %d → ", num);
                array_stack_print(&stack, "Stack");
            }
            /* Sau operator */
            else if (strlen(token) == 1 && is_operator(token[0])) {
                int b, a;
                array_stack_pop(&stack, &b);
                array_stack_pop(&stack, &a);
                int result = apply_operator(token[0], a, b);
                array_stack_push(&stack, result);
                printf("  %d %c %d = %d → ", a, token[0], b, result);
                array_stack_print(&stack, "Stack");
            }
        }
        
        if (c == '\0') break;
    }
    
    int result;
    array_stack_pop(&stack, &result);
    return result;
}

/* =============================================================================
 * PARTEA 5: CONVERSIE INFIX → POSTFIX (Shunting Yard simplificat)
 * =============================================================================
 */

/**
 * Returnează precedența unui operator
 */
int precedence(char op) {
    switch (op) {
        case '+': case '-': return 1;
        case '*': case '/': case '%': return 2;
        case '^': return 3;
        default: return 0;
    }
}

/**
 * Verifică asociativitatea (true = stânga, false = dreapta)
 */
bool is_left_associative(char op) {
    return op != '^';  /* Doar ^ este asociativ la dreapta */
}

/**
 * Convertește o expresie din notație infixă în postfixă
 * 
 * @param infix Expresia infixă (ex: "3 + 4 * 5")
 * @param postfix Buffer pentru rezultat
 * 
 * Algoritmul Shunting Yard (Dijkstra):
 *   - Numere → direct la ieșire
 *   - Operatori → pe stivă (cu reguli de precedență)
 *   - '(' → pe stivă
 *   - ')' → golește până la '('
 */
void infix_to_postfix(const char *infix, char *postfix) {
    CharStack op_stack;
    char_stack_init(&op_stack);
    
    int out_idx = 0;
    
    for (int i = 0; infix[i] != '\0'; i++) {
        char c = infix[i];
        
        /* Ignoră spațiile */
        if (c == ' ') continue;
        
        /* Număr sau literă (operand) */
        if (isalnum(c)) {
            postfix[out_idx++] = c;
            postfix[out_idx++] = ' ';
        }
        /* Paranteză deschisă */
        else if (c == '(') {
            char_stack_push(&op_stack, c);
        }
        /* Paranteză închisă */
        else if (c == ')') {
            char top;
            while (!char_stack_is_empty(&op_stack)) {
                char_stack_pop(&op_stack, &top);
                if (top == '(') break;
                postfix[out_idx++] = top;
                postfix[out_idx++] = ' ';
            }
        }
        /* Operator */
        else if (is_operator(c) || c == '^') {
            char top;
            while (!char_stack_is_empty(&op_stack)) {
                /* Peek la vârf */
                char_stack_pop(&op_stack, &top);
                
                /* Verifică dacă trebuie să scoatem operatorul */
                if (top != '(' && 
                    (precedence(top) > precedence(c) ||
                     (precedence(top) == precedence(c) && is_left_associative(c)))) {
                    postfix[out_idx++] = top;
                    postfix[out_idx++] = ' ';
                } else {
                    char_stack_push(&op_stack, top);  /* Pune înapoi */
                    break;
                }
            }
            char_stack_push(&op_stack, c);
        }
    }
    
    /* Golește stiva de operatori rămasă */
    char top;
    while (char_stack_pop(&op_stack, &top)) {
        postfix[out_idx++] = top;
        postfix[out_idx++] = ' ';
    }
    
    /* Elimină ultimul spațiu și termină șirul */
    if (out_idx > 0 && postfix[out_idx - 1] == ' ') {
        out_idx--;
    }
    postfix[out_idx] = '\0';
}

/* =============================================================================
 * PARTEA 6: APLICAȚIE PRACTICĂ - SISTEM UNDO/REDO
 * =============================================================================
 */

/**
 * Structură pentru o acțiune în editor
 */
typedef struct {
    char type;      /* 'A' = add, 'D' = delete */
    char data[64];  /* Datele acțiunii */
} Action;

/**
 * Stivă pentru acțiuni
 */
typedef struct {
    Action actions[MAX_SIZE];
    int top;
} ActionStack;

void action_stack_init(ActionStack *s) { s->top = -1; }
bool action_stack_is_empty(const ActionStack *s) { return s->top == -1; }

bool action_stack_push(ActionStack *s, Action a) {
    if (s->top >= MAX_SIZE - 1) return false;
    s->actions[++s->top] = a;
    return true;
}

bool action_stack_pop(ActionStack *s, Action *a) {
    if (s->top < 0) return false;
    *a = s->actions[s->top--];
    return true;
}

void action_stack_clear(ActionStack *s) {
    s->top = -1;
}

/**
 * Structură pentru editorul cu undo/redo
 */
typedef struct {
    char text[256];         /* Textul curent */
    ActionStack undo_stack; /* Istoricul pentru undo */
    ActionStack redo_stack; /* Istoricul pentru redo */
} TextEditor;

void editor_init(TextEditor *e) {
    e->text[0] = '\0';
    action_stack_init(&e->undo_stack);
    action_stack_init(&e->redo_stack);
}

void editor_add_text(TextEditor *e, const char *text) {
    Action a = {'A', ""};
    strncpy(a.data, text, sizeof(a.data) - 1);
    
    /* Salvează acțiunea pentru undo */
    action_stack_push(&e->undo_stack, a);
    
    /* Șterge redo la acțiune nouă */
    action_stack_clear(&e->redo_stack);
    
    /* Aplică acțiunea */
    strncat(e->text, text, sizeof(e->text) - strlen(e->text) - 1);
    
    printf("  " COLOR_GREEN "Adăugat:" COLOR_RESET " \"%s\"\n", text);
}

void editor_undo(TextEditor *e) {
    Action a;
    if (!action_stack_pop(&e->undo_stack, &a)) {
        printf("  " COLOR_YELLOW "Nimic de anulat!\n" COLOR_RESET);
        return;
    }
    
    /* Mută în redo */
    action_stack_push(&e->redo_stack, a);
    
    /* Anulează acțiunea */
    if (a.type == 'A') {
        /* Elimină textul adăugat */
        size_t len = strlen(a.data);
        size_t text_len = strlen(e->text);
        if (text_len >= len) {
            e->text[text_len - len] = '\0';
        }
        printf("  " COLOR_YELLOW "Undo:" COLOR_RESET " eliminat \"%s\"\n", a.data);
    }
}

void editor_redo(TextEditor *e) {
    Action a;
    if (!action_stack_pop(&e->redo_stack, &a)) {
        printf("  " COLOR_YELLOW "Nimic de refăcut!\n" COLOR_RESET);
        return;
    }
    
    /* Mută în undo */
    action_stack_push(&e->undo_stack, a);
    
    /* Reaplică acțiunea */
    if (a.type == 'A') {
        strncat(e->text, a.data, sizeof(e->text) - strlen(e->text) - 1);
        printf("  " COLOR_CYAN "Redo:" COLOR_RESET " readăugat \"%s\"\n", a.data);
    }
}

void editor_print(const TextEditor *e) {
    printf("  Text curent: \"" COLOR_GREEN "%s" COLOR_RESET "\"\n", e->text);
}

/* =============================================================================
 * FUNCȚII DE DEMONSTRAȚIE
 * =============================================================================
 */

void print_header(const char *title) {
    printf("\n");
    printf(COLOR_CYAN "╔═══════════════════════════════════════════════════════════════╗\n" COLOR_RESET);
    printf(COLOR_CYAN "║" COLOR_RESET " %-61s " COLOR_CYAN "║\n" COLOR_RESET, title);
    printf(COLOR_CYAN "╚═══════════════════════════════════════════════════════════════╝\n" COLOR_RESET);
    printf("\n");
}

void demo_array_stack(void) {
    print_header("PARTEA 1: Stivă pe Vector");
    
    ArrayStack stack;
    array_stack_init(&stack);
    
    printf("Operații pe stiva pe vector:\n\n");
    
    /* Push */
    printf("Push 10, 20, 30, 40:\n");
    array_stack_push(&stack, 10);
    array_stack_push(&stack, 20);
    array_stack_push(&stack, 30);
    array_stack_push(&stack, 40);
    array_stack_print(&stack, "  Stack");
    
    /* Peek */
    int top_val;
    if (array_stack_peek(&stack, &top_val)) {
        printf("\nPeek: vârful este %d\n", top_val);
    }
    
    /* Pop */
    printf("\nPop două elemente:\n");
    int val;
    array_stack_pop(&stack, &val);
    printf("  Eliminat: %d\n", val);
    array_stack_pop(&stack, &val);
    printf("  Eliminat: %d\n", val);
    array_stack_print(&stack, "  Stack");
    
    printf("\nDimensiune finală: %d\n", array_stack_size(&stack));
}

void demo_linked_stack(void) {
    print_header("PARTEA 2: Stivă pe Listă Înlănțuită");
    
    LinkedStack stack;
    linked_stack_init(&stack);
    
    printf("Operații pe stiva pe listă:\n\n");
    
    /* Push */
    printf("Push 100, 200, 300:\n");
    linked_stack_push(&stack, 100);
    linked_stack_push(&stack, 200);
    linked_stack_push(&stack, 300);
    linked_stack_print(&stack, "  Stack");
    
    /* Pop */
    printf("\nPop un element:\n");
    int val;
    linked_stack_pop(&stack, &val);
    printf("  Eliminat: %d\n", val);
    linked_stack_print(&stack, "  Stack");
    
    /* Cleanup */
    printf("\nEliberare memorie (stack_destroy)...\n");
    linked_stack_destroy(&stack);
    linked_stack_print(&stack, "  Stack");
    printf("  ✓ Memorie eliberată cu succes!\n");
}

void demo_bracket_checker(void) {
    print_header("PARTEA 3: Verificarea Parantezelor");
    
    const char *test_cases[] = {
        "{[()]}",
        "((()))",
        "[{()}]",
        "a + (b * [c - d])",
        "{[(])}",
        "(((",
        "()))",
        "[]{}()",
        NULL
    };
    
    printf("Verificare expresii:\n\n");
    
    for (int i = 0; test_cases[i] != NULL; i++) {
        int result = check_brackets(test_cases[i]);
        printf("  \"%s\" → ", test_cases[i]);
        if (result == -1) {
            printf(COLOR_GREEN "✓ Valid\n" COLOR_RESET);
        } else {
            printf(COLOR_RED "✗ Invalid (eroare la poziția %d)\n" COLOR_RESET, result);
        }
    }
}

void demo_postfix_eval(void) {
    print_header("PARTEA 4: Evaluarea Expresiilor Postfixe");
    
    const char *expressions[] = {
        "3 4 +",           /* 3 + 4 = 7 */
        "3 4 + 5 *",       /* (3 + 4) * 5 = 35 */
        "10 2 /",          /* 10 / 2 = 5 */
        "5 1 2 + 4 * + 3 -", /* 5 + ((1 + 2) * 4) - 3 = 14 */
        NULL
    };
    
    printf("Evaluare expresii RPN:\n\n");
    
    for (int i = 0; expressions[i] != NULL; i++) {
        printf(COLOR_YELLOW "Expresie: \"%s\"\n" COLOR_RESET, expressions[i]);
        int result = evaluate_postfix(expressions[i]);
        printf(COLOR_GREEN "  Rezultat final: %d\n\n" COLOR_RESET, result);
    }
}

void demo_infix_to_postfix(void) {
    print_header("PARTEA 5: Conversie Infix → Postfix");
    
    const char *infix_expressions[] = {
        "3 + 4",
        "3 + 4 * 5",
        "(3 + 4) * 5",
        "a + b * c - d",
        "a * (b + c) / d",
        NULL
    };
    
    printf("Conversie cu algoritmul Shunting Yard:\n\n");
    
    char postfix[MAX_EXPR_LEN];
    
    for (int i = 0; infix_expressions[i] != NULL; i++) {
        infix_to_postfix(infix_expressions[i], postfix);
        printf("  Infix:   \"%s\"\n", infix_expressions[i]);
        printf("  Postfix: \"%s\"\n\n", postfix);
    }
}

void demo_undo_redo(void) {
    print_header("PARTEA 6: Sistem Undo/Redo");
    
    TextEditor editor;
    editor_init(&editor);
    
    printf("Simulare editor de text cu undo/redo:\n\n");
    
    printf("1. Adăugăm text:\n");
    editor_add_text(&editor, "Hello ");
    editor_print(&editor);
    
    editor_add_text(&editor, "World");
    editor_print(&editor);
    
    editor_add_text(&editor, "!");
    editor_print(&editor);
    
    printf("\n2. Undo de 2 ori:\n");
    editor_undo(&editor);
    editor_print(&editor);
    
    editor_undo(&editor);
    editor_print(&editor);
    
    printf("\n3. Redo:\n");
    editor_redo(&editor);
    editor_print(&editor);
    
    printf("\n4. Adăugăm text nou (șterge redo):\n");
    editor_add_text(&editor, " Everyone");
    editor_print(&editor);
    
    printf("\n5. Încercare redo (nu mai e posibil):\n");
    editor_redo(&editor);
    editor_print(&editor);
}

/* =============================================================================
 * PROGRAMUL PRINCIPAL
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf(COLOR_MAGENTA "╔═══════════════════════════════════════════════════════════════╗\n" COLOR_RESET);
    printf(COLOR_MAGENTA "║" COLOR_RESET "         " COLOR_CYAN "SĂPTĂMÂNA 5: STIVE (STACK)" COLOR_RESET "                       " COLOR_MAGENTA "║\n" COLOR_RESET);
    printf(COLOR_MAGENTA "║" COLOR_RESET "              Exemplu Complet Demonstrativ                   " COLOR_MAGENTA "║\n" COLOR_RESET);
    printf(COLOR_MAGENTA "╚═══════════════════════════════════════════════════════════════╝\n" COLOR_RESET);
    
    demo_array_stack();
    demo_linked_stack();
    demo_bracket_checker();
    demo_postfix_eval();
    demo_infix_to_postfix();
    demo_undo_redo();
    
    printf("\n");
    printf(COLOR_GREEN "═══════════════════════════════════════════════════════════════\n" COLOR_RESET);
    printf(COLOR_GREEN "  ✓ Toate demonstrațiile completate cu succes!\n" COLOR_RESET);
    printf(COLOR_GREEN "═══════════════════════════════════════════════════════════════\n" COLOR_RESET);
    printf("\n");
    
    return 0;
}
