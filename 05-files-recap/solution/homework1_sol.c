/**
 * =============================================================================
 * SOLUȚIE - TEMA 1: Calculator de Expresii Matematice
 * =============================================================================
 *
 * Calculator complet cu suport pentru:
 *   - Numere întregi multi-cifră și negative
 *   - Operatori: + - * / % ^
 *   - Paranteze la orice nivel
 *   - Gestionare erori (împărțire cu zero, paranteze invalide)
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -o homework1_sol homework1_sol.c -lm
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX_STACK_SIZE 256
#define MAX_EXPR_LEN 1024
#define MAX_TOKEN_LEN 32

/* =============================================================================
 * TIPURI DE ERORI
 * =============================================================================
 */

typedef enum {
    CALC_OK,
    CALC_ERR_EMPTY,
    CALC_ERR_UNBALANCED_PARENS,
    CALC_ERR_INVALID_CHAR,
    CALC_ERR_DIVISION_BY_ZERO,
    CALC_ERR_SYNTAX,
    CALC_ERR_OVERFLOW
} CalcError;

/* =============================================================================
 * STIVE
 * =============================================================================
 */

/* Stivă pentru operatori */
typedef struct {
    char data[MAX_STACK_SIZE];
    int top;
} CharStack;

void char_stack_init(CharStack *s) { s->top = -1; }
bool char_stack_is_empty(const CharStack *s) { return s->top == -1; }
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

/* Stivă pentru numere (long long pentru valori mai mari) */
typedef struct {
    long long data[MAX_STACK_SIZE];
    int top;
} NumStack;

void num_stack_init(NumStack *s) { s->top = -1; }
bool num_stack_is_empty(const NumStack *s) { return s->top == -1; }
bool num_stack_push(NumStack *s, long long val) {
    if (s->top >= MAX_STACK_SIZE - 1) return false;
    s->data[++s->top] = val;
    return true;
}
bool num_stack_pop(NumStack *s, long long *val) {
    if (s->top < 0) return false;
    *val = s->data[s->top--];
    return true;
}

/* =============================================================================
 * FUNCȚII HELPER
 * =============================================================================
 */

bool is_operator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '^';
}

int get_precedence(char op) {
    switch (op) {
        case '^':           return 3;
        case '*': case '/': case '%': return 2;
        case '+': case '-': return 1;
        default:            return 0;
    }
}

bool is_left_associative(char op) {
    return op != '^';
}

/* =============================================================================
 * VERIFICARE PARANTEZE
 * =============================================================================
 */

bool check_balanced_parens(const char *expr) {
    int count = 0;
    for (int i = 0; expr[i]; i++) {
        if (expr[i] == '(') count++;
        else if (expr[i] == ')') count--;
        if (count < 0) return false;
    }
    return count == 0;
}

/* =============================================================================
 * TOKENIZER - Extrage numere și operatori
 * =============================================================================
 */

typedef struct {
    char type;  /* 'N' = număr, 'O' = operator, 'P' = paranteză */
    long long value;
    char op;
} Token;

typedef struct {
    Token tokens[MAX_STACK_SIZE];
    int count;
} TokenList;

CalcError tokenize(const char *expr, TokenList *list) {
    list->count = 0;
    int i = 0;
    bool expect_operand = true;  /* Pentru a gestiona minus unar */
    
    while (expr[i] != '\0') {
        /* Sari peste spații */
        if (isspace(expr[i])) {
            i++;
            continue;
        }
        
        /* Paranteză deschisă */
        if (expr[i] == '(') {
            list->tokens[list->count].type = 'P';
            list->tokens[list->count].op = '(';
            list->count++;
            expect_operand = true;
            i++;
        }
        /* Paranteză închisă */
        else if (expr[i] == ')') {
            list->tokens[list->count].type = 'P';
            list->tokens[list->count].op = ')';
            list->count++;
            expect_operand = false;
            i++;
        }
        /* Număr (sau minus unar) */
        else if (isdigit(expr[i]) || (expr[i] == '-' && expect_operand && isdigit(expr[i+1]))) {
            long long num = 0;
            int sign = 1;
            
            if (expr[i] == '-') {
                sign = -1;
                i++;
            }
            
            while (isdigit(expr[i])) {
                num = num * 10 + (expr[i] - '0');
                i++;
            }
            
            list->tokens[list->count].type = 'N';
            list->tokens[list->count].value = sign * num;
            list->count++;
            expect_operand = false;
        }
        /* Operator */
        else if (is_operator(expr[i])) {
            list->tokens[list->count].type = 'O';
            list->tokens[list->count].op = expr[i];
            list->count++;
            expect_operand = true;
            i++;
        }
        /* Caracter invalid */
        else {
            return CALC_ERR_INVALID_CHAR;
        }
    }
    
    return CALC_OK;
}

/* =============================================================================
 * SHUNTING YARD CU TOKENS
 * =============================================================================
 */

CalcError shunting_yard(TokenList *infix, TokenList *postfix) {
    CharStack op_stack;
    char_stack_init(&op_stack);
    postfix->count = 0;
    
    for (int i = 0; i < infix->count; i++) {
        Token *t = &infix->tokens[i];
        
        if (t->type == 'N') {
            /* Număr - direct la ieșire */
            postfix->tokens[postfix->count++] = *t;
        }
        else if (t->type == 'P' && t->op == '(') {
            char_stack_push(&op_stack, '(');
        }
        else if (t->type == 'P' && t->op == ')') {
            char top;
            while (char_stack_pop(&op_stack, &top)) {
                if (top == '(') break;
                postfix->tokens[postfix->count].type = 'O';
                postfix->tokens[postfix->count].op = top;
                postfix->count++;
            }
        }
        else if (t->type == 'O') {
            char top;
            while (!char_stack_is_empty(&op_stack)) {
                char_stack_peek(&op_stack, &top);
                if (top != '(' &&
                    (get_precedence(top) > get_precedence(t->op) ||
                     (get_precedence(top) == get_precedence(t->op) && is_left_associative(t->op)))) {
                    char_stack_pop(&op_stack, &top);
                    postfix->tokens[postfix->count].type = 'O';
                    postfix->tokens[postfix->count].op = top;
                    postfix->count++;
                } else {
                    break;
                }
            }
            char_stack_push(&op_stack, t->op);
        }
    }
    
    /* Golește stiva */
    char top;
    while (char_stack_pop(&op_stack, &top)) {
        postfix->tokens[postfix->count].type = 'O';
        postfix->tokens[postfix->count].op = top;
        postfix->count++;
    }
    
    return CALC_OK;
}

/* =============================================================================
 * EVALUARE POSTFIX
 * =============================================================================
 */

CalcError evaluate_postfix_tokens(TokenList *postfix, long long *result) {
    NumStack stack;
    num_stack_init(&stack);
    
    for (int i = 0; i < postfix->count; i++) {
        Token *t = &postfix->tokens[i];
        
        if (t->type == 'N') {
            num_stack_push(&stack, t->value);
        }
        else if (t->type == 'O') {
            long long b, a, r;
            num_stack_pop(&stack, &b);
            num_stack_pop(&stack, &a);
            
            switch (t->op) {
                case '+': r = a + b; break;
                case '-': r = a - b; break;
                case '*': r = a * b; break;
                case '/':
                    if (b == 0) return CALC_ERR_DIVISION_BY_ZERO;
                    r = a / b;
                    break;
                case '%':
                    if (b == 0) return CALC_ERR_DIVISION_BY_ZERO;
                    r = a % b;
                    break;
                case '^':
                    r = (long long)pow((double)a, (double)b);
                    break;
                default:
                    return CALC_ERR_SYNTAX;
            }
            
            num_stack_push(&stack, r);
        }
    }
    
    num_stack_pop(&stack, result);
    return CALC_OK;
}

/* =============================================================================
 * FUNCȚIA PRINCIPALĂ DE CALCUL
 * =============================================================================
 */

CalcError calculate(const char *expr, long long *result) {
    /* Verificări preliminare */
    if (strlen(expr) == 0) {
        return CALC_ERR_EMPTY;
    }
    
    if (!check_balanced_parens(expr)) {
        return CALC_ERR_UNBALANCED_PARENS;
    }
    
    /* Tokenizare */
    TokenList infix, postfix;
    CalcError err = tokenize(expr, &infix);
    if (err != CALC_OK) return err;
    
    if (infix.count == 0) {
        return CALC_ERR_EMPTY;
    }
    
    /* Conversie în postfix */
    err = shunting_yard(&infix, &postfix);
    if (err != CALC_OK) return err;
    
    /* Evaluare */
    err = evaluate_postfix_tokens(&postfix, result);
    return err;
}

/* =============================================================================
 * INTERFAȚĂ UTILIZATOR
 * =============================================================================
 */

const char* error_message(CalcError err) {
    switch (err) {
        case CALC_OK:                   return "OK";
        case CALC_ERR_EMPTY:            return "Expresie goală";
        case CALC_ERR_UNBALANCED_PARENS: return "Paranteze neechilibrate";
        case CALC_ERR_INVALID_CHAR:     return "Caracter invalid";
        case CALC_ERR_DIVISION_BY_ZERO: return "Împărțire cu zero";
        case CALC_ERR_SYNTAX:           return "Eroare de sintaxă";
        case CALC_ERR_OVERFLOW:         return "Overflow numeric";
        default:                        return "Eroare necunoscută";
    }
}

int main(int argc, char *argv[]) {
    char input[MAX_EXPR_LEN];
    
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║           CALCULATOR DE EXPRESII MATEMATICE                   ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    printf("Operatori suportați: + - * / %% ^ (putere)\n");
    printf("Suportă paranteze și numere negative.\n");
    printf("Tastează 'quit' sau 'q' pentru a ieși.\n");
    printf("\n");
    
    /* Mod fișier dacă e specificat argument */
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f == NULL) {
            fprintf(stderr, "Eroare: Nu pot deschide fișierul %s\n", argv[1]);
            return 1;
        }
        
        while (fgets(input, sizeof(input), f) != NULL) {
            /* Elimină newline */
            size_t len = strlen(input);
            if (len > 0 && input[len-1] == '\n') input[len-1] = '\0';
            if (strlen(input) == 0) continue;
            
            long long result;
            CalcError err = calculate(input, &result);
            
            printf("%s = ", input);
            if (err == CALC_OK) {
                printf("%lld\n", result);
            } else {
                printf("Eroare: %s\n", error_message(err));
            }
        }
        
        fclose(f);
        return 0;
    }
    
    /* Mod interactiv */
    while (1) {
        printf("> ");
        fflush(stdout);
        
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }
        
        /* Elimină newline */
        size_t len = strlen(input);
        if (len > 0 && input[len-1] == '\n') input[len-1] = '\0';
        
        /* Verifică ieșire */
        if (strcmp(input, "quit") == 0 || strcmp(input, "q") == 0) {
            break;
        }
        
        if (strlen(input) == 0) continue;
        
        /* Calculează și afișează */
        long long result;
        CalcError err = calculate(input, &result);
        
        if (err == CALC_OK) {
            printf("Rezultat: \033[0;32m%lld\033[0m\n\n", result);
        } else {
            printf("\033[0;31mEroare: %s\033[0m\n\n", error_message(err));
        }
    }
    
    printf("\nLa revedere!\n");
    return 0;
}
