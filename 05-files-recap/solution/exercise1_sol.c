/**
 * =============================================================================
 * SOLUȚIE - EXERCIȚIUL 1: Verificator de Paranteze
 * =============================================================================
 *
 * Această soluție implementează un verificator complet pentru expresii
 * cu paranteze echilibrate folosind o stivă.
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -o exercise1_sol exercise1_sol.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_STACK_SIZE 1000
#define MAX_INPUT_LEN 1024

/* =============================================================================
 * STRUCTURA STIVEI
 * =============================================================================
 */

typedef struct {
    char data[MAX_STACK_SIZE];
    int top;
} CharStack;

/* =============================================================================
 * FUNCȚII PENTRU STIVĂ
 * =============================================================================
 */

/**
 * Inițializează stiva cu top = -1 (stivă goală)
 */
void stack_init(CharStack *stack) {
    stack->top = -1;
}

/**
 * Verifică dacă stiva este goală
 */
bool stack_is_empty(const CharStack *stack) {
    return stack->top == -1;
}

/**
 * Adaugă un caracter în vârful stivei
 */
bool stack_push(CharStack *stack, char c) {
    if (stack->top >= MAX_STACK_SIZE - 1) {
        return false;  /* Stack overflow */
    }
    stack->top++;
    stack->data[stack->top] = c;
    return true;
}

/**
 * Elimină și returnează caracterul din vârful stivei
 */
bool stack_pop(CharStack *stack, char *c) {
    if (stack->top < 0) {
        return false;  /* Stack underflow */
    }
    *c = stack->data[stack->top];
    stack->top--;
    return true;
}

/* =============================================================================
 * FUNCȚII HELPER PENTRU PARANTEZE
 * =============================================================================
 */

/**
 * Verifică dacă caracterul este o paranteză de deschidere
 */
bool is_opening_bracket(char c) {
    return c == '(' || c == '[' || c == '{';
}

/**
 * Verifică dacă caracterul este o paranteză de închidere
 */
bool is_closing_bracket(char c) {
    return c == ')' || c == ']' || c == '}';
}

/**
 * Verifică dacă două paranteze formează o pereche validă
 */
bool brackets_match(char open, char close) {
    return (open == '(' && close == ')') ||
           (open == '[' && close == ']') ||
           (open == '{' && close == '}');
}

/* =============================================================================
 * FUNCȚIA PRINCIPALĂ DE VERIFICARE
 * =============================================================================
 */

/**
 * Verifică dacă parantezele din expresie sunt echilibrate
 *
 * @param expression Șirul de caractere de verificat
 * @return -1 dacă expresia este validă, sau poziția primei erori
 */
int check_brackets(const char *expression) {
    CharStack stack;
    stack_init(&stack);
    
    for (int i = 0; expression[i] != '\0'; i++) {
        char c = expression[i];
        
        if (is_opening_bracket(c)) {
            /* Paranteză de deschidere - push pe stivă */
            stack_push(&stack, c);
        }
        else if (is_closing_bracket(c)) {
            /* Paranteză de închidere - verifică potrivirea */
            char top_char;
            
            /* Dacă stiva e goală, avem o închidere fără deschidere */
            if (!stack_pop(&stack, &top_char)) {
                return i;
            }
            
            /* Verifică dacă perechea este validă */
            if (!brackets_match(top_char, c)) {
                return i;
            }
        }
        /* Toate celelalte caractere sunt ignorate */
    }
    
    /* Dacă mai sunt elemente în stivă = paranteze neînchise */
    if (!stack_is_empty(&stack)) {
        return (int)strlen(expression);
    }
    
    return -1;  /* Expresie validă */
}

/* =============================================================================
 * PROGRAMUL PRINCIPAL - MOD BATCH PENTRU TESTARE
 * =============================================================================
 */

int main(void) {
    char input[MAX_INPUT_LEN];
    
    /* Citește linii de la stdin și afișează rezultatul */
    while (fgets(input, sizeof(input), stdin) != NULL) {
        /* Elimină newline */
        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }
        
        /* Verifică și afișează rezultatul */
        int result = check_brackets(input);
        printf("%d\n", result);
    }
    
    return 0;
}
