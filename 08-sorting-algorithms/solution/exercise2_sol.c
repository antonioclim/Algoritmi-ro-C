/**
 * =============================================================================
 * SOLUȚIE: EXERCIȚIUL 2 - Arbore de Expresii Aritmetice
 * =============================================================================
 *
 * NOTĂ: Acest fișier este doar pentru instructori!
 *
 * Compilare: gcc -Wall -Wextra -std=c11 -o exercise2_sol exercise2_sol.c -lm
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX_EXPR_LENGTH 256
#define MAX_STACK_SIZE 100

/* =============================================================================
 * DEFINIREA TIPURILOR
 * =============================================================================
 */

typedef enum {
    NODE_OPERAND,
    NODE_OPERATOR
} NodeType;

typedef struct ExprNode {
    NodeType type;
    double value;
    char op;
    struct ExprNode *left;
    struct ExprNode *right;
} ExprNode;

typedef struct {
    ExprNode *items[MAX_STACK_SIZE];
    int top;
} NodeStack;

/* =============================================================================
 * FUNCȚII PENTRU STACK
 * =============================================================================
 */

void stack_init(NodeStack *stack) {
    stack->top = -1;
}

int stack_is_empty(NodeStack *stack) {
    return stack->top == -1;
}

int stack_is_full(NodeStack *stack) {
    return stack->top == MAX_STACK_SIZE - 1;
}

void stack_push(NodeStack *stack, ExprNode *node) {
    if (stack_is_full(stack)) {
        fprintf(stderr, "Eroare: Stack plin!\n");
        return;
    }
    stack->items[++stack->top] = node;
}

ExprNode* stack_pop(NodeStack *stack) {
    if (stack_is_empty(stack)) {
        fprintf(stderr, "Eroare: Stack gol!\n");
        return NULL;
    }
    return stack->items[stack->top--];
}

/* =============================================================================
 * FUNCȚII DE CREARE NODURI
 * =============================================================================
 */

ExprNode* create_operand_node(double value) {
    ExprNode *node = (ExprNode*)malloc(sizeof(ExprNode));
    if (node == NULL) {
        fprintf(stderr, "Eroare: Nu s-a putut aloca memorie!\n");
        return NULL;
    }
    
    node->type = NODE_OPERAND;
    node->value = value;
    node->op = '\0';
    node->left = NULL;
    node->right = NULL;
    
    return node;
}

ExprNode* create_operator_node(char op, ExprNode *left, ExprNode *right) {
    ExprNode *node = (ExprNode*)malloc(sizeof(ExprNode));
    if (node == NULL) {
        fprintf(stderr, "Eroare: Nu s-a putut aloca memorie!\n");
        return NULL;
    }
    
    node->type = NODE_OPERATOR;
    node->op = op;
    node->value = 0;
    node->left = left;
    node->right = right;
    
    return node;
}

/* =============================================================================
 * FUNCȚII AUXILIARE
 * =============================================================================
 */

int is_operator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/';
}

/* =============================================================================
 * CONSTRUIREA ARBORELUI
 * =============================================================================
 */

ExprNode* build_tree_from_postfix(const char *postfix) {
    if (postfix == NULL || strlen(postfix) == 0) {
        return NULL;
    }
    
    NodeStack stack;
    stack_init(&stack);
    
    char expr[MAX_EXPR_LENGTH];
    strncpy(expr, postfix, MAX_EXPR_LENGTH - 1);
    expr[MAX_EXPR_LENGTH - 1] = '\0';
    
    char *token = strtok(expr, " ");
    
    while (token != NULL) {
        if (strlen(token) == 1 && is_operator(token[0])) {
            /* Operator: pop 2 operanzi și creăm nod operator */
            ExprNode *right = stack_pop(&stack);
            ExprNode *left = stack_pop(&stack);
            
            if (right == NULL || left == NULL) {
                fprintf(stderr, "Eroare: Expresie invalidă!\n");
                return NULL;
            }
            
            ExprNode *op_node = create_operator_node(token[0], left, right);
            stack_push(&stack, op_node);
        } else {
            /* Operand: convertim la double și creăm nod */
            double value = atof(token);
            ExprNode *num_node = create_operand_node(value);
            stack_push(&stack, num_node);
        }
        
        token = strtok(NULL, " ");
    }
    
    if (stack_is_empty(&stack)) {
        return NULL;
    }
    
    return stack_pop(&stack);
}

/* =============================================================================
 * EVALUAREA EXPRESIEI
 * =============================================================================
 */

double evaluate_tree(ExprNode *node) {
    if (node == NULL) {
        return 0;
    }
    
    if (node->type == NODE_OPERAND) {
        return node->value;
    }
    
    double left_val = evaluate_tree(node->left);
    double right_val = evaluate_tree(node->right);
    
    switch (node->op) {
        case '+':
            return left_val + right_val;
        case '-':
            return left_val - right_val;
        case '*':
            return left_val * right_val;
        case '/':
            if (right_val == 0) {
                fprintf(stderr, "Avertisment: Împărțire la zero!\n");
                return INFINITY;
            }
            return left_val / right_val;
        default:
            fprintf(stderr, "Operator necunoscut: %c\n", node->op);
            return 0;
    }
}

/* =============================================================================
 * AFIȘARE ÎN DIFERITE NOTAȚII
 * =============================================================================
 */

void print_infix(ExprNode *node) {
    if (node == NULL) return;
    
    if (node->type == NODE_OPERAND) {
        if (node->value == (int)node->value) {
            printf("%.0f", node->value);
        } else {
            printf("%.2f", node->value);
        }
    } else {
        printf("(");
        print_infix(node->left);
        printf(" %c ", node->op);
        print_infix(node->right);
        printf(")");
    }
}

void print_prefix(ExprNode *node) {
    if (node == NULL) return;
    
    if (node->type == NODE_OPERAND) {
        if (node->value == (int)node->value) {
            printf("%.0f ", node->value);
        } else {
            printf("%.2f ", node->value);
        }
    } else {
        printf("%c ", node->op);
        print_prefix(node->left);
        print_prefix(node->right);
    }
}

void print_postfix(ExprNode *node) {
    if (node == NULL) return;
    
    if (node->type == NODE_OPERAND) {
        if (node->value == (int)node->value) {
            printf("%.0f ", node->value);
        } else {
            printf("%.2f ", node->value);
        }
    } else {
        print_postfix(node->left);
        print_postfix(node->right);
        printf("%c ", node->op);
    }
}

/* =============================================================================
 * FUNCȚII STATISTICE
 * =============================================================================
 */

int tree_height(ExprNode *node) {
    if (node == NULL) {
        return -1;
    }
    
    int left_h = tree_height(node->left);
    int right_h = tree_height(node->right);
    
    return 1 + (left_h > right_h ? left_h : right_h);
}

int count_nodes(ExprNode *node) {
    if (node == NULL) return 0;
    return 1 + count_nodes(node->left) + count_nodes(node->right);
}

/* =============================================================================
 * SIMPLIFICARE EXPRESII (BONUS)
 * =============================================================================
 */

ExprNode* simplify_tree(ExprNode *node) {
    if (node == NULL) return NULL;
    
    if (node->type == NODE_OPERAND) {
        return node;
    }
    
    /* Simplificăm mai întâi subarborii */
    node->left = simplify_tree(node->left);
    node->right = simplify_tree(node->right);
    
    /* Regulile de simplificare */
    
    /* x + 0 = x */
    if (node->op == '+' && 
        node->right->type == NODE_OPERAND && 
        node->right->value == 0) {
        ExprNode *result = node->left;
        free(node->right);
        free(node);
        return result;
    }
    
    /* 0 + x = x */
    if (node->op == '+' && 
        node->left->type == NODE_OPERAND && 
        node->left->value == 0) {
        ExprNode *result = node->right;
        free(node->left);
        free(node);
        return result;
    }
    
    /* x - 0 = x */
    if (node->op == '-' && 
        node->right->type == NODE_OPERAND && 
        node->right->value == 0) {
        ExprNode *result = node->left;
        free(node->right);
        free(node);
        return result;
    }
    
    /* x * 1 = x */
    if (node->op == '*' && 
        node->right->type == NODE_OPERAND && 
        node->right->value == 1) {
        ExprNode *result = node->left;
        free(node->right);
        free(node);
        return result;
    }
    
    /* 1 * x = x */
    if (node->op == '*' && 
        node->left->type == NODE_OPERAND && 
        node->left->value == 1) {
        ExprNode *result = node->right;
        free(node->left);
        free(node);
        return result;
    }
    
    /* x * 0 = 0 */
    if (node->op == '*' && 
        node->right->type == NODE_OPERAND && 
        node->right->value == 0) {
        free_tree(node->left);
        free(node->right);
        node->type = NODE_OPERAND;
        node->value = 0;
        node->left = NULL;
        node->right = NULL;
        return node;
    }
    
    /* 0 * x = 0 */
    if (node->op == '*' && 
        node->left->type == NODE_OPERAND && 
        node->left->value == 0) {
        free_tree(node->right);
        free(node->left);
        node->type = NODE_OPERAND;
        node->value = 0;
        node->left = NULL;
        node->right = NULL;
        return node;
    }
    
    /* x / 1 = x */
    if (node->op == '/' && 
        node->right->type == NODE_OPERAND && 
        node->right->value == 1) {
        ExprNode *result = node->left;
        free(node->right);
        free(node);
        return result;
    }
    
    return node;
}

/* =============================================================================
 * AFIȘARE ARBORE (VIZUALIZARE)
 * =============================================================================
 */

void print_tree_visual(ExprNode *node, int level, char prefix) {
    if (node == NULL) return;
    
    for (int i = 0; i < level; i++) {
        printf("    ");
    }
    
    if (level > 0) {
        printf("%c── ", prefix);
    }
    
    if (node->type == NODE_OPERAND) {
        if (node->value == (int)node->value) {
            printf("%.0f\n", node->value);
        } else {
            printf("%.2f\n", node->value);
        }
    } else {
        printf("%c\n", node->op);
    }
    
    print_tree_visual(node->left, level + 1, 'L');
    print_tree_visual(node->right, level + 1, 'R');
}

/* =============================================================================
 * ELIBERARE MEMORIE
 * =============================================================================
 */

void free_tree(ExprNode *node) {
    if (node == NULL) return;
    
    free_tree(node->left);
    free_tree(node->right);
    free(node);
}

/* =============================================================================
 * PROGRAM PRINCIPAL
 * =============================================================================
 */

void print_separator(void) {
    printf("─────────────────────────────────────────\n");
}

void demo_expression(const char *postfix) {
    printf("\n");
    print_separator();
    printf("Expresie postfix: %s\n", postfix);
    print_separator();
    
    ExprNode *root = build_tree_from_postfix(postfix);
    
    if (root == NULL) {
        printf("Eroare la construirea arborelui!\n");
        return;
    }
    
    printf("\nStructura arborelui:\n");
    print_tree_visual(root, 0, ' ');
    
    printf("\nNotații:\n");
    printf("  Infix:   ");
    print_infix(root);
    printf("\n");
    
    printf("  Prefix:  ");
    print_prefix(root);
    printf("\n");
    
    printf("  Postfix: ");
    print_postfix(root);
    printf("\n");
    
    printf("\nStatistici:\n");
    printf("  Înălțime: %d\n", tree_height(root));
    printf("  Noduri:   %d\n", count_nodes(root));
    
    printf("\nRezultat evaluare: %.2f\n", evaluate_tree(root));
    
    free_tree(root);
}

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     EXERCIȚIUL 2: ARBORE DE EXPRESII (SOLUȚIE)                ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    printf("\n═══ DEMONSTRAȚII ═══\n");
    
    demo_expression("3 4 + 2 *");
    demo_expression("5 3 + 10 2 - *");
    demo_expression("8 2 / 3 +");
    
    printf("\n═══ MOD INTERACTIV ═══\n");
    printf("Introduceți expresii în notație postfix (sau 'exit' pentru ieșire).\n");
    printf("Exemplu: 3 4 + 2 *  ->  (3 + 4) * 2 = 14\n\n");
    
    char input[MAX_EXPR_LENGTH];
    
    while (1) {
        printf("Expresie postfix: ");
        
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }
        
        input[strcspn(input, "\n")] = 0;
        
        if (strcmp(input, "exit") == 0 || strlen(input) == 0) {
            break;
        }
        
        demo_expression(input);
    }
    
    printf("\nLa revedere!\n");
    
    return 0;
}
