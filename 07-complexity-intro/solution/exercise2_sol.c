/**
 * =============================================================================
 * SOLUȚIE EXERCIȚIU 2: Arbori de Expresii (Expression Trees)
 * =============================================================================
 * ATENȚIE: Acest fișier este destinat EXCLUSIV instructorilor!
 * Nu distribuiți studenților înainte de deadline.
 * =============================================================================
 */

#define _GNU_SOURCE  /* Pentru strdup */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_EXPR_LEN 256
#define MAX_STACK_SIZE 100

/* =============================================================================
 * STRUCTURI - SOLUȚIE TODO 1
 * =============================================================================
 */

typedef struct ExprNode {
    int isOperator;          /* 1 = operator, 0 = operand */
    char op;                 /* Operatorul (+, -, *, /) */
    int value;               /* Valoarea numerică */
    struct ExprNode *left;   /* Copilul stâng */
    struct ExprNode *right;  /* Copilul drept */
} ExprNode;

typedef struct {
    ExprNode *items[MAX_STACK_SIZE];
    int top;
} NodeStack;

/* =============================================================================
 * FUNCȚII STACK - SOLUȚIE TODO 2-3
 * =============================================================================
 */

void initStack(NodeStack *s) {
    s->top = -1;
}

int isStackEmpty(NodeStack *s) {
    return s->top == -1;
}

/**
 * SOLUȚIE TODO 2: push
 */
void push(NodeStack *s, ExprNode *node) {
    if (s->top >= MAX_STACK_SIZE - 1) {
        fprintf(stderr, "Eroare: Stack overflow!\n");
        return;
    }
    s->top++;
    s->items[s->top] = node;
}

/**
 * SOLUȚIE TODO 3: pop
 */
ExprNode* pop(NodeStack *s) {
    if (isStackEmpty(s)) {
        return NULL;
    }
    ExprNode *node = s->items[s->top];
    s->top--;
    return node;
}

/* =============================================================================
 * FUNCȚII DE CREARE NODURI - SOLUȚIE TODO 4-5
 * =============================================================================
 */

/**
 * SOLUȚIE TODO 4: createOperandNode
 */
ExprNode* createOperandNode(int value) {
    ExprNode *node = (ExprNode*)malloc(sizeof(ExprNode));
    if (node == NULL) {
        fprintf(stderr, "Eroare: Alocare memorie eșuată!\n");
        return NULL;
    }
    
    node->isOperator = 0;
    node->value = value;
    node->op = '\0';
    node->left = NULL;
    node->right = NULL;
    
    return node;
}

/**
 * SOLUȚIE TODO 5: createOperatorNode
 */
ExprNode* createOperatorNode(char op, ExprNode *left, ExprNode *right) {
    ExprNode *node = (ExprNode*)malloc(sizeof(ExprNode));
    if (node == NULL) {
        fprintf(stderr, "Eroare: Alocare memorie eșuată!\n");
        return NULL;
    }
    
    node->isOperator = 1;
    node->op = op;
    node->value = 0;
    node->left = left;
    node->right = right;
    
    return node;
}

/* =============================================================================
 * FUNCȚII UTILITARE
 * =============================================================================
 */

int isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/';
}

int isNumber(const char *str) {
    if (str == NULL || *str == '\0') {
        return 0;
    }
    
    if (*str == '-') {
        str++;
        if (*str == '\0') return 0;
    }
    
    while (*str != '\0') {
        if (!isdigit((unsigned char)*str)) {
            return 0;
        }
        str++;
    }
    return 1;
}

/* =============================================================================
 * CONSTRUCȚIA ARBORELUI - SOLUȚIE TODO 6
 * =============================================================================
 */

/**
 * SOLUȚIE TODO 6: buildExpressionTree
 */
ExprNode* buildExpressionTree(const char *postfix) {
    char *expr = strdup(postfix);
    if (expr == NULL) {
        return NULL;
    }
    
    NodeStack stack;
    initStack(&stack);
    
    char *token = strtok(expr, " ");
    
    while (token != NULL) {
        if (isNumber(token)) {
            /* Este un operand - creăm nod și push în stack */
            ExprNode *operand = createOperandNode(atoi(token));
            if (operand == NULL) {
                free(expr);
                return NULL;
            }
            push(&stack, operand);
        } 
        else if (strlen(token) == 1 && isOperator(token[0])) {
            /* Este un operator */
            /* IMPORTANT: ordinea pop! right primul, apoi left */
            ExprNode *right = pop(&stack);
            ExprNode *left = pop(&stack);
            
            if (left == NULL || right == NULL) {
                fprintf(stderr, "Eroare: Expresie invalidă!\n");
                free(expr);
                return NULL;
            }
            
            ExprNode *opNode = createOperatorNode(token[0], left, right);
            if (opNode == NULL) {
                free(expr);
                return NULL;
            }
            push(&stack, opNode);
        }
        
        token = strtok(NULL, " ");
    }
    
    free(expr);
    
    /* Rădăcina este ultimul (și singurul) element din stack */
    return pop(&stack);
}

/* =============================================================================
 * EVALUAREA EXPRESIEI - SOLUȚIE TODO 7
 * =============================================================================
 */

/**
 * SOLUȚIE TODO 7: evaluateExpression
 */
int evaluateExpression(ExprNode *node, int *error) {
    if (node == NULL) {
        *error = 1;
        return 0;
    }
    
    /* Caz de bază: operand */
    if (node->isOperator == 0) {
        return node->value;
    }
    
    /* Evaluăm recursiv subarborii */
    int leftVal = evaluateExpression(node->left, error);
    if (*error) return 0;
    
    int rightVal = evaluateExpression(node->right, error);
    if (*error) return 0;
    
    /* Aplicăm operatorul */
    switch (node->op) {
        case '+':
            return leftVal + rightVal;
        case '-':
            return leftVal - rightVal;
        case '*':
            return leftVal * rightVal;
        case '/':
            if (rightVal == 0) {
                *error = 1;
                return 0;
            }
            return leftVal / rightVal;
        default:
            *error = 1;
            return 0;
    }
}

/* =============================================================================
 * AFIȘAREA ÎN DIFERITE NOTAȚII - SOLUȚIE TODO 8-10
 * =============================================================================
 */

/**
 * SOLUȚIE TODO 8: printPostfix
 */
void printPostfix(ExprNode *node) {
    if (node == NULL) {
        return;
    }
    
    printPostfix(node->left);
    printPostfix(node->right);
    
    if (node->isOperator) {
        printf("%c ", node->op);
    } else {
        printf("%d ", node->value);
    }
}

/**
 * SOLUȚIE TODO 9: printPrefix
 */
void printPrefix(ExprNode *node) {
    if (node == NULL) {
        return;
    }
    
    if (node->isOperator) {
        printf("%c ", node->op);
    } else {
        printf("%d ", node->value);
    }
    
    printPrefix(node->left);
    printPrefix(node->right);
}

/**
 * SOLUȚIE TODO 10: printInfix
 */
void printInfix(ExprNode *node) {
    if (node == NULL) {
        return;
    }
    
    if (node->isOperator == 0) {
        /* Operand - doar afișăm valoarea */
        printf("%d", node->value);
    } else {
        /* Operator - afișăm cu paranteze */
        printf("(");
        printInfix(node->left);
        printf(" %c ", node->op);
        printInfix(node->right);
        printf(")");
    }
}

/* =============================================================================
 * ELIBERAREA MEMORIEI - SOLUȚIE TODO 11
 * =============================================================================
 */

/**
 * SOLUȚIE TODO 11: freeExprTree
 */
void freeExprTree(ExprNode *node) {
    if (node == NULL) {
        return;
    }
    
    freeExprTree(node->left);
    freeExprTree(node->right);
    free(node);
}

/* =============================================================================
 * FUNCȚIE AUXILIARĂ PENTRU DEBUGGING
 * =============================================================================
 */

void printExprTreeVisual(ExprNode *node, int space) {
    if (node == NULL) {
        return;
    }
    
    space += 5;
    
    printExprTreeVisual(node->right, space);
    
    printf("\n");
    for (int i = 5; i < space; i++) {
        printf(" ");
    }
    
    if (node->isOperator) {
        printf("[%c]\n", node->op);
    } else {
        printf("[%d]\n", node->value);
    }
    
    printExprTreeVisual(node->left, space);
}

/* =============================================================================
 * FUNCȚIA MAIN
 * =============================================================================
 */

int main(void) {
    char expression[MAX_EXPR_LEN];
    
    if (fgets(expression, MAX_EXPR_LEN, stdin) == NULL) {
        fprintf(stderr, "Eroare: Nu s-a putut citi expresia\n");
        return 1;
    }
    
    size_t len = strlen(expression);
    if (len > 0 && expression[len - 1] == '\n') {
        expression[len - 1] = '\0';
    }
    
    if (strlen(expression) == 0) {
        fprintf(stderr, "Eroare: Expresie goală\n");
        return 1;
    }
    
    ExprNode *root = buildExpressionTree(expression);
    
    if (root == NULL) {
        fprintf(stderr, "Eroare: Nu s-a putut construi arborele de expresii\n");
        return 1;
    }
    
    printf("Postfix: ");
    printPostfix(root);
    printf("\n");
    
    printf("Prefix: ");
    printPrefix(root);
    printf("\n");
    
    printf("Infix: ");
    printInfix(root);
    printf("\n");
    
    int error = 0;
    int result = evaluateExpression(root, &error);
    
    if (error) {
        printf("Result: ERROR (division by zero or invalid expression)\n");
    } else {
        printf("Result: %d\n", result);
    }
    
    freeExprTree(root);
    
    return 0;
}
