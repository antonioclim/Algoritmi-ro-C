/**
 * =============================================================================
 * SOLUȚIE - EXERCIȚIUL 2: Convertor Infix → Postfix și Evaluator RPN
 * =============================================================================
 *
 * Această soluție implementează algoritmul Shunting Yard pentru conversie
 * și evaluatorul de expresii postfixe.
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -o exercise2_sol exercise2_sol.c -lm
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX_STACK_SIZE 100
#define MAX_EXPR_LEN 256

/* =============================================================================
 * STIVA PENTRU CARACTERE
 * =============================================================================
 */

typedef struct {
    char data[MAX_STACK_SIZE];
    int top;
} CharStack;

void char_stack_init(CharStack *s) {
    s->top = -1;
}

bool char_stack_is_empty(const CharStack *s) {
    return s->top == -1;
}

bool char_stack_push(CharStack *s, char c) {
    if (s->top >= MAX_STACK_SIZE - 1) return false;
    s->data[++s->top] = c;
    return true;
}

bool char_stack_pop(CharStack *s, char *c) {
    if (s->top < 0) return false;
    *c = s->data[s->top--];
    return true;
}

bool char_stack_peek(const CharStack *s, char *c) {
    if (s->top < 0) return false;
    *c = s->data[s->top];
    return true;
}

/* =============================================================================
 * STIVA PENTRU ÎNTREGI
 * =============================================================================
 */

typedef struct {
    int data[MAX_STACK_SIZE];
    int top;
} IntStack;

void int_stack_init(IntStack *s) {
    s->top = -1;
}

bool int_stack_is_empty(const IntStack *s) {
    return s->top == -1;
}

bool int_stack_push(IntStack *s, int value) {
    if (s->top >= MAX_STACK_SIZE - 1) return false;
    s->data[++s->top] = value;
    return true;
}

bool int_stack_pop(IntStack *s, int *value) {
    if (s->top < 0) return false;
    *value = s->data[s->top--];
    return true;
}

/* =============================================================================
 * FUNCȚII HELPER PENTRU OPERATORI
 * =============================================================================
 */

/**
 * Verifică dacă un caracter este operator
 */
bool is_operator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '^';
}

/**
 * Returnează precedența unui operator
 */
int get_precedence(char op) {
    switch (op) {
        case '^':           return 3;
        case '*': case '/': case '%': return 2;
        case '+': case '-': return 1;
        default:            return 0;
    }
}

/**
 * Verifică asociativitatea (true = stânga)
 */
bool is_left_associative(char op) {
    return op != '^';  /* Doar ^ este asociativ la dreapta */
}

/**
 * Aplică un operator pe doi operanzi
 */
int apply_operator(char op, int a, int b) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': 
            if (b == 0) {
                fprintf(stderr, "Eroare: Împărțire cu zero!\n");
                return 0;
            }
            return a / b;
        case '%': return a % b;
        case '^': return (int)pow(a, b);
        default:  return 0;
    }
}

/* =============================================================================
 * ALGORITMUL SHUNTING YARD
 * =============================================================================
 */

/**
 * Convertește o expresie din notație infixă în postfixă
 */
void infix_to_postfix(const char *infix, char *postfix) {
    CharStack op_stack;
    char_stack_init(&op_stack);
    
    int out_idx = 0;
    
    for (int i = 0; infix[i] != '\0'; i++) {
        char c = infix[i];
        
        /* Ignoră spațiile */
        if (c == ' ') {
            continue;
        }
        
        /* Dacă e cifră (operand) */
        if (isdigit(c)) {
            postfix[out_idx++] = c;
            postfix[out_idx++] = ' ';
        }
        /* Dacă e paranteză deschisă */
        else if (c == '(') {
            char_stack_push(&op_stack, c);
        }
        /* Dacă e paranteză închisă */
        else if (c == ')') {
            char top;
            while (char_stack_pop(&op_stack, &top)) {
                if (top == '(') break;
                postfix[out_idx++] = top;
                postfix[out_idx++] = ' ';
            }
        }
        /* Dacă e operator */
        else if (is_operator(c)) {
            char top;
            while (!char_stack_is_empty(&op_stack)) {
                char_stack_peek(&op_stack, &top);
                
                /* Verifică dacă trebuie să scoatem operatorul din stivă */
                if (top != '(' && 
                    (get_precedence(top) > get_precedence(c) ||
                     (get_precedence(top) == get_precedence(c) && is_left_associative(c)))) {
                    char_stack_pop(&op_stack, &top);
                    postfix[out_idx++] = top;
                    postfix[out_idx++] = ' ';
                } else {
                    break;
                }
            }
            char_stack_push(&op_stack, c);
        }
    }
    
    /* Golește stiva de operatori */
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
 * EVALUAREA EXPRESIILOR POSTFIXE
 * =============================================================================
 */

/**
 * Evaluează o expresie în notație postfixă
 */
int evaluate_postfix(const char *postfix) {
    IntStack stack;
    int_stack_init(&stack);
    
    int current_num = 0;
    bool building_number = false;
    
    for (int i = 0; postfix[i] != '\0'; i++) {
        char c = postfix[i];
        
        if (isdigit(c)) {
            current_num = current_num * 10 + (c - '0');
            building_number = true;
        }
        else {
            if (building_number) {
                int_stack_push(&stack, current_num);
                building_number = false;
                current_num = 0;
            }
            
            if (is_operator(c)) {
                int b, a;
                int_stack_pop(&stack, &b);
                int_stack_pop(&stack, &a);
                int result = apply_operator(c, a, b);
                int_stack_push(&stack, result);
            }
        }
    }
    
    /* Ultimul număr dacă expresia nu se termină cu spațiu */
    if (building_number) {
        int_stack_push(&stack, current_num);
    }
    
    int result = 0;
    int_stack_pop(&stack, &result);
    return result;
}

/* =============================================================================
 * PROGRAMUL PRINCIPAL - MOD BATCH PENTRU TESTARE
 * =============================================================================
 */

int main(void) {
    char input[MAX_EXPR_LEN];
    char postfix[MAX_EXPR_LEN];
    
    /* Citește linii de la stdin și afișează rezultatul */
    while (fgets(input, sizeof(input), stdin) != NULL) {
        /* Elimină newline */
        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }
        
        if (strlen(input) == 0) continue;
        
        /* Conversie și evaluare */
        infix_to_postfix(input, postfix);
        int result = evaluate_postfix(postfix);
        
        printf("%s | %d\n", postfix, result);
    }
    
    return 0;
}
