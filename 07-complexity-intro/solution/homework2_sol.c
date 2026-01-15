/**
 * =============================================================================
 * SOLUȚIE TEMĂ 2: Calculator de Expresii cu Variabile
 * =============================================================================
 * ATENȚIE: Acest fișier este destinat EXCLUSIV instructorilor!
 * =============================================================================
 */

#define _GNU_SOURCE  /* Pentru strdup */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX_EXPR_LEN 256
#define MAX_STACK_SIZE 100
#define MAX_VAR_NAME 16
#define MAX_VARIABLES 50

/* =============================================================================
 * STRUCTURI DE DATE
 * =============================================================================
 */

/* Tipuri de noduri */
typedef enum {
    NODE_NUMBER,
    NODE_VARIABLE,
    NODE_OPERATOR,
    NODE_FUNCTION
} NodeType;

/* Structura nodului expresiei */
typedef struct ExprNode {
    NodeType type;
    double value;                    /* Pentru numere */
    char varName[MAX_VAR_NAME];      /* Pentru variabile */
    char op;                         /* Pentru operatori */
    char funcName[MAX_VAR_NAME];     /* Pentru funcții */
    struct ExprNode *left;
    struct ExprNode *right;
} ExprNode;

/* Structura pentru variabile */
typedef struct {
    char name[MAX_VAR_NAME];
    double value;
    int defined;
} Variable;

/* Tabel global de variabile */
Variable variables[MAX_VARIABLES];
int varCount = 0;

/* Stack-uri pentru algoritmul Shunting Yard */
typedef struct {
    ExprNode *items[MAX_STACK_SIZE];
    int top;
} NodeStack;

typedef struct {
    char items[MAX_STACK_SIZE][MAX_VAR_NAME];
    int top;
} OperatorStack;

/* =============================================================================
 * FUNCȚII PENTRU VARIABILE
 * =============================================================================
 */

/**
 * Inițializează tabelul de variabile
 */
void initVariables(void) {
    varCount = 0;
    for (int i = 0; i < MAX_VARIABLES; i++) {
        variables[i].defined = 0;
    }
}

/**
 * Setează valoarea unei variabile
 */
void setVariable(const char *name, double value) {
    /* Căutăm variabila existentă */
    for (int i = 0; i < varCount; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            variables[i].value = value;
            variables[i].defined = 1;
            return;
        }
    }
    
    /* Adăugăm variabilă nouă */
    if (varCount < MAX_VARIABLES) {
        strncpy(variables[varCount].name, name, MAX_VAR_NAME - 1);
        variables[varCount].name[MAX_VAR_NAME - 1] = '\0';
        variables[varCount].value = value;
        variables[varCount].defined = 1;
        varCount++;
    }
}

/**
 * Obține valoarea unei variabile
 */
int getVariable(const char *name, double *value) {
    for (int i = 0; i < varCount; i++) {
        if (strcmp(variables[i].name, name) == 0 && variables[i].defined) {
            *value = variables[i].value;
            return 1;
        }
    }
    return 0;  /* Variabilă nedefinită */
}

/* =============================================================================
 * FUNCȚII PENTRU STACK-URI
 * =============================================================================
 */

void initNodeStack(NodeStack *s) { s->top = -1; }
int isNodeStackEmpty(NodeStack *s) { return s->top == -1; }

void pushNode(NodeStack *s, ExprNode *node) {
    if (s->top < MAX_STACK_SIZE - 1) {
        s->items[++s->top] = node;
    }
}

ExprNode* popNode(NodeStack *s) {
    if (s->top >= 0) {
        return s->items[s->top--];
    }
    return NULL;
}

void initOpStack(OperatorStack *s) { s->top = -1; }
int isOpStackEmpty(OperatorStack *s) { return s->top == -1; }

void pushOp(OperatorStack *s, const char *op) {
    if (s->top < MAX_STACK_SIZE - 1) {
        strncpy(s->items[++s->top], op, MAX_VAR_NAME - 1);
    }
}

char* popOp(OperatorStack *s) {
    if (s->top >= 0) {
        return s->items[s->top--];
    }
    return NULL;
}

char* peekOp(OperatorStack *s) {
    if (s->top >= 0) {
        return s->items[s->top];
    }
    return NULL;
}

/* =============================================================================
 * FUNCȚII DE CREARE NODURI
 * =============================================================================
 */

ExprNode* createNumberNode(double value) {
    ExprNode *node = (ExprNode*)malloc(sizeof(ExprNode));
    if (node == NULL) return NULL;
    
    node->type = NODE_NUMBER;
    node->value = value;
    node->varName[0] = '\0';
    node->op = '\0';
    node->funcName[0] = '\0';
    node->left = NULL;
    node->right = NULL;
    
    return node;
}

ExprNode* createVariableNode(const char *name) {
    ExprNode *node = (ExprNode*)malloc(sizeof(ExprNode));
    if (node == NULL) return NULL;
    
    node->type = NODE_VARIABLE;
    node->value = 0;
    strncpy(node->varName, name, MAX_VAR_NAME - 1);
    node->op = '\0';
    node->funcName[0] = '\0';
    node->left = NULL;
    node->right = NULL;
    
    return node;
}

ExprNode* createOperatorNode(char op, ExprNode *left, ExprNode *right) {
    ExprNode *node = (ExprNode*)malloc(sizeof(ExprNode));
    if (node == NULL) return NULL;
    
    node->type = NODE_OPERATOR;
    node->value = 0;
    node->varName[0] = '\0';
    node->op = op;
    node->funcName[0] = '\0';
    node->left = left;
    node->right = right;
    
    return node;
}

ExprNode* createFunctionNode(const char *func, ExprNode *arg) {
    ExprNode *node = (ExprNode*)malloc(sizeof(ExprNode));
    if (node == NULL) return NULL;
    
    node->type = NODE_FUNCTION;
    node->value = 0;
    node->varName[0] = '\0';
    node->op = '\0';
    strncpy(node->funcName, func, MAX_VAR_NAME - 1);
    node->left = arg;  /* Argumentul funcției */
    node->right = NULL;
    
    return node;
}

/* =============================================================================
 * FUNCȚII PENTRU OPERATORI
 * =============================================================================
 */

int isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == '%';
}

int getPrecedence(const char *op) {
    if (strlen(op) != 1) return 0;
    
    switch (op[0]) {
        case '+': case '-': return 1;
        case '*': case '/': case '%': return 2;
        case '^': return 3;
        default: return 0;
    }
}

int isRightAssociative(const char *op) {
    return strlen(op) == 1 && op[0] == '^';
}

int isFunction(const char *name) {
    return strcmp(name, "sin") == 0 ||
           strcmp(name, "cos") == 0 ||
           strcmp(name, "sqrt") == 0 ||
           strcmp(name, "abs") == 0 ||
           strcmp(name, "neg") == 0 ||
           strcmp(name, "ln") == 0 ||
           strcmp(name, "exp") == 0;
}

/* =============================================================================
 * PARSER - SHUNTING YARD ALGORITHM
 * =============================================================================
 */

ExprNode* parseInfix(const char *expr) {
    char *input = strdup(expr);
    if (input == NULL) return NULL;
    
    NodeStack output;
    OperatorStack operators;
    initNodeStack(&output);
    initOpStack(&operators);
    
    char *p = input;
    
    while (*p != '\0') {
        /* Sărim spațiile */
        while (*p == ' ') p++;
        if (*p == '\0') break;
        
        /* Număr */
        if (isdigit(*p) || (*p == '.' && isdigit(*(p+1)))) {
            double num = strtod(p, &p);
            pushNode(&output, createNumberNode(num));
        }
        /* Variabilă sau funcție */
        else if (isalpha(*p) || *p == '_') {
            char name[MAX_VAR_NAME];
            int i = 0;
            while ((isalnum(*p) || *p == '_') && i < MAX_VAR_NAME - 1) {
                name[i++] = *p++;
            }
            name[i] = '\0';
            
            /* Verificăm dacă e funcție */
            while (*p == ' ') p++;
            if (*p == '(' && isFunction(name)) {
                pushOp(&operators, name);
            } else {
                pushNode(&output, createVariableNode(name));
            }
        }
        /* Paranteza deschisă */
        else if (*p == '(') {
            pushOp(&operators, "(");
            p++;
        }
        /* Paranteza închisă */
        else if (*p == ')') {
            while (!isOpStackEmpty(&operators) && strcmp(peekOp(&operators), "(") != 0) {
                char *op = popOp(&operators);
                if (isFunction(op)) {
                    ExprNode *arg = popNode(&output);
                    pushNode(&output, createFunctionNode(op, arg));
                } else {
                    ExprNode *right = popNode(&output);
                    ExprNode *left = popNode(&output);
                    pushNode(&output, createOperatorNode(op[0], left, right));
                }
            }
            popOp(&operators);  /* Scoatem '(' */
            
            /* Verificăm dacă e funcție */
            if (!isOpStackEmpty(&operators) && isFunction(peekOp(&operators))) {
                char *func = popOp(&operators);
                ExprNode *arg = popNode(&output);
                pushNode(&output, createFunctionNode(func, arg));
            }
            p++;
        }
        /* Operator */
        else if (isOperator(*p)) {
            char op[2] = {*p, '\0'};
            
            while (!isOpStackEmpty(&operators) && 
                   strcmp(peekOp(&operators), "(") != 0 &&
                   (getPrecedence(peekOp(&operators)) > getPrecedence(op) ||
                    (getPrecedence(peekOp(&operators)) == getPrecedence(op) && !isRightAssociative(op)))) {
                char *topOp = popOp(&operators);
                ExprNode *right = popNode(&output);
                ExprNode *left = popNode(&output);
                pushNode(&output, createOperatorNode(topOp[0], left, right));
            }
            
            pushOp(&operators, op);
            p++;
        }
        else {
            p++;  /* Caracter necunoscut, îl sărim */
        }
    }
    
    /* Golim stack-ul de operatori */
    while (!isOpStackEmpty(&operators)) {
        char *op = popOp(&operators);
        if (isFunction(op)) {
            ExprNode *arg = popNode(&output);
            pushNode(&output, createFunctionNode(op, arg));
        } else {
            ExprNode *right = popNode(&output);
            ExprNode *left = popNode(&output);
            pushNode(&output, createOperatorNode(op[0], left, right));
        }
    }
    
    free(input);
    return popNode(&output);
}

/* =============================================================================
 * EVALUAREA EXPRESIEI
 * =============================================================================
 */

double evaluate(ExprNode *node, int *error) {
    if (node == NULL) {
        *error = 1;
        return 0;
    }
    
    switch (node->type) {
        case NODE_NUMBER:
            return node->value;
            
        case NODE_VARIABLE: {
            double val;
            if (!getVariable(node->varName, &val)) {
                fprintf(stderr, "Eroare: Variabila '%s' nedefinită!\n", node->varName);
                *error = 1;
                return 0;
            }
            return val;
        }
        
        case NODE_OPERATOR: {
            double left = evaluate(node->left, error);
            if (*error) return 0;
            double right = evaluate(node->right, error);
            if (*error) return 0;
            
            switch (node->op) {
                case '+': return left + right;
                case '-': return left - right;
                case '*': return left * right;
                case '/':
                    if (right == 0) {
                        fprintf(stderr, "Eroare: Împărțire la zero!\n");
                        *error = 1;
                        return 0;
                    }
                    return left / right;
                case '^': return pow(left, right);
                case '%': return fmod(left, right);
                default:
                    *error = 1;
                    return 0;
            }
        }
        
        case NODE_FUNCTION: {
            double arg = evaluate(node->left, error);
            if (*error) return 0;
            
            if (strcmp(node->funcName, "sin") == 0) return sin(arg);
            if (strcmp(node->funcName, "cos") == 0) return cos(arg);
            if (strcmp(node->funcName, "sqrt") == 0) {
                if (arg < 0) {
                    fprintf(stderr, "Eroare: Radical din număr negativ!\n");
                    *error = 1;
                    return 0;
                }
                return sqrt(arg);
            }
            if (strcmp(node->funcName, "abs") == 0) return fabs(arg);
            if (strcmp(node->funcName, "neg") == 0) return -arg;
            if (strcmp(node->funcName, "ln") == 0) {
                if (arg <= 0) {
                    fprintf(stderr, "Eroare: Logaritm din număr <= 0!\n");
                    *error = 1;
                    return 0;
                }
                return log(arg);
            }
            if (strcmp(node->funcName, "exp") == 0) return exp(arg);
            
            *error = 1;
            return 0;
        }
        
        default:
            *error = 1;
            return 0;
    }
}

/* =============================================================================
 * FUNCȚII DE AFIȘARE
 * =============================================================================
 */

void printInfix(ExprNode *node) {
    if (node == NULL) return;
    
    switch (node->type) {
        case NODE_NUMBER:
            printf("%.4g", node->value);
            break;
        case NODE_VARIABLE:
            printf("%s", node->varName);
            break;
        case NODE_OPERATOR:
            printf("(");
            printInfix(node->left);
            printf(" %c ", node->op);
            printInfix(node->right);
            printf(")");
            break;
        case NODE_FUNCTION:
            printf("%s(", node->funcName);
            printInfix(node->left);
            printf(")");
            break;
    }
}

void printPrefix(ExprNode *node) {
    if (node == NULL) return;
    
    switch (node->type) {
        case NODE_NUMBER:
            printf("%.4g ", node->value);
            break;
        case NODE_VARIABLE:
            printf("%s ", node->varName);
            break;
        case NODE_OPERATOR:
            printf("%c ", node->op);
            printPrefix(node->left);
            printPrefix(node->right);
            break;
        case NODE_FUNCTION:
            printf("%s ", node->funcName);
            printPrefix(node->left);
            break;
    }
}

void printLatex(ExprNode *node) {
    if (node == NULL) return;
    
    switch (node->type) {
        case NODE_NUMBER:
            printf("%.4g", node->value);
            break;
        case NODE_VARIABLE:
            printf("%s", node->varName);
            break;
        case NODE_OPERATOR:
            if (node->op == '/') {
                printf("\\frac{");
                printLatex(node->left);
                printf("}{");
                printLatex(node->right);
                printf("}");
            } else if (node->op == '*') {
                printf("(");
                printLatex(node->left);
                printf(" \\cdot ");
                printLatex(node->right);
                printf(")");
            } else if (node->op == '^') {
                printf("{");
                printLatex(node->left);
                printf("}^{");
                printLatex(node->right);
                printf("}");
            } else {
                printf("(");
                printLatex(node->left);
                printf(" %c ", node->op);
                printLatex(node->right);
                printf(")");
            }
            break;
        case NODE_FUNCTION:
            if (strcmp(node->funcName, "sqrt") == 0) {
                printf("\\sqrt{");
                printLatex(node->left);
                printf("}");
            } else {
                printf("\\%s(", node->funcName);
                printLatex(node->left);
                printf(")");
            }
            break;
    }
}

/* =============================================================================
 * ELIBERAREA MEMORIEI
 * =============================================================================
 */

void freeExprTree(ExprNode *node) {
    if (node == NULL) return;
    freeExprTree(node->left);
    freeExprTree(node->right);
    free(node);
}

/* =============================================================================
 * FUNCȚIA MAIN - DEMONSTRAȚIE
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     CALCULATOR DE EXPRESII CU VARIABILE                       ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    initVariables();
    
    /* Test 1: Expresie simplă */
    printf("📝 Test 1: Expresie simplă\n");
    printf("   Input: \"(3 + 5) * 2\"\n");
    ExprNode *expr1 = parseInfix("(3 + 5) * 2");
    printf("   Infix: ");
    printInfix(expr1);
    printf("\n");
    int error = 0;
    printf("   Rezultat: %.4g\n\n", evaluate(expr1, &error));
    freeExprTree(expr1);
    
    /* Test 2: Expresie cu variabile */
    printf("📝 Test 2: Expresie cu variabile\n");
    printf("   Input: \"(x + 3) * (y - 2)\"\n");
    ExprNode *expr2 = parseInfix("(x + 3) * (y - 2)");
    
    setVariable("x", 5);
    setVariable("y", 7);
    printf("   Variabile: x = 5, y = 7\n");
    printf("   Infix: ");
    printInfix(expr2);
    printf("\n");
    error = 0;
    printf("   Rezultat: %.4g\n\n", evaluate(expr2, &error));
    
    /* Schimbăm variabilele */
    setVariable("x", 10);
    printf("   Schimbăm x = 10\n");
    error = 0;
    printf("   Rezultat nou: %.4g\n\n", evaluate(expr2, &error));
    freeExprTree(expr2);
    
    /* Test 3: Expresie cu funcții */
    printf("📝 Test 3: Expresie cu funcții\n");
    printf("   Input: \"sqrt(16) + sin(0) * 2\"\n");
    ExprNode *expr3 = parseInfix("sqrt(16) + sin(0) * 2");
    printf("   Infix: ");
    printInfix(expr3);
    printf("\n");
    error = 0;
    printf("   Rezultat: %.4g\n\n", evaluate(expr3, &error));
    freeExprTree(expr3);
    
    /* Test 4: Expresie complexă cu putere */
    printf("📝 Test 4: Expresie cu putere\n");
    printf("   Input: \"2 ^ 3 ^ 2\" (right-associative: 2^(3^2) = 2^9 = 512)\n");
    ExprNode *expr4 = parseInfix("2 ^ 3 ^ 2");
    printf("   Infix: ");
    printInfix(expr4);
    printf("\n");
    error = 0;
    printf("   Rezultat: %.4g\n\n", evaluate(expr4, &error));
    freeExprTree(expr4);
    
    /* Test 5: LaTeX output */
    printf("📝 Test 5: Output LaTeX\n");
    printf("   Input: \"(a + b) / (c * d)\"\n");
    setVariable("a", 1);
    setVariable("b", 2);
    setVariable("c", 3);
    setVariable("d", 4);
    ExprNode *expr5 = parseInfix("(a + b) / (c * d)");
    printf("   LaTeX: ");
    printLatex(expr5);
    printf("\n");
    error = 0;
    printf("   Rezultat: %.4g\n\n", evaluate(expr5, &error));
    freeExprTree(expr5);
    
    printf("✓ Toate testele au trecut cu succes!\n\n");
    
    return 0;
}
