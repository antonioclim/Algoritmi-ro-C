/**
 * =============================================================================
 * EXERCIȚIUL 2: Arbore de Expresii Aritmetice
 * =============================================================================
 *
 * OBIECTIV:
 *   Construiți și evaluați expresii aritmetice folosind arbori binari.
 *   Frunzele sunt operanzi (numere), nodurile interne sunt operatori (+,-,*,/).
 *
 * CERINȚE:
 *   1. Definiți structura pentru un nod de expresie
 *   2. Construiți arborele dintr-o expresie în notație postfix
 *   3. Evaluați expresia prin parcurgere postorder
 *   4. Afișați expresia în toate cele 3 notații: infix, prefix, postfix
 *   5. Calculați înălțimea arborelui de expresie
 *   6. (BONUS) Simplificați expresii: x*1=x, x+0=x, x*0=0
 *
 * EXEMPLU:
 *   Input (postfix): "3 4 + 2 *"
 *   Arbore:
 *          *
 *         / \
 *        +   2
 *       / \
 *      3   4
 *
 *   Infix:   ((3 + 4) * 2)
 *   Prefix:  * + 3 4 2
 *   Postfix: 3 4 + 2 *
 *   Rezultat: 14
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -o exercise2 exercise2.c -lm
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

/* Dimensiune maximă pentru expresie și stack */
#define MAX_EXPR_LENGTH 256
#define MAX_STACK_SIZE 100

/* =============================================================================
 * DEFINIREA TIPURILOR
 * =============================================================================
 */

/**
 * Tipul nodului: operand (număr) sau operator
 */
typedef enum {
    NODE_OPERAND,
    NODE_OPERATOR
} NodeType;

/**
 * TODO 1: Definiți structura ExprNode pentru un nod în arborele de expresie
 *
 * Trebuie să conțină:
 * - type: tipul nodului (NODE_OPERAND sau NODE_OPERATOR)
 * - value: valoarea numerică (pentru operanzi)
 * - op: caracterul operator (pentru operatori: +, -, *, /)
 * - left: pointer către subarborele stâng
 * - right: pointer către subarborele drept
 *
 * Hint: Folosiți union pentru value și op, sau păstrați ambele câmpuri
 */

typedef struct ExprNode {
    NodeType type;
    /* YOUR CODE HERE - adăugați câmpurile necesare */
    double value;           /* Pentru operanzi */
    char op;                /* Pentru operatori */
    struct ExprNode *left;
    struct ExprNode *right;
} ExprNode;

/**
 * Stack pentru construirea arborelui (implementare furnizată)
 */
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

/**
 * TODO 2: Creați un nod operand (cu valoare numerică)
 *
 * @param value Valoarea numerică
 * @return Pointer către nodul creat sau NULL la eroare
 */
ExprNode* create_operand_node(double value) {
    ExprNode *node = (ExprNode*)malloc(sizeof(ExprNode));
    if (node == NULL) {
        fprintf(stderr, "Eroare: Nu s-a putut aloca memorie!\n");
        return NULL;
    }
    
    /* YOUR CODE HERE */
    node->type = NODE_OPERAND;
    node->value = value;
    node->op = '\0';
    node->left = NULL;
    node->right = NULL;
    
    return node;
}

/**
 * TODO 3: Creați un nod operator
 *
 * @param op Caracterul operator (+, -, *, /)
 * @param left Subarborele stâng
 * @param right Subarborele drept
 * @return Pointer către nodul creat sau NULL la eroare
 */
ExprNode* create_operator_node(char op, ExprNode *left, ExprNode *right) {
    ExprNode *node = (ExprNode*)malloc(sizeof(ExprNode));
    if (node == NULL) {
        fprintf(stderr, "Eroare: Nu s-a putut aloca memorie!\n");
        return NULL;
    }
    
    /* YOUR CODE HERE */
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

/**
 * Verifică dacă un caracter este operator
 */
int is_operator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/';
}

/* =============================================================================
 * CONSTRUIREA ARBORELUI
 * =============================================================================
 */

/**
 * TODO 4: Construiți arborele de expresie din notație postfix
 *
 * @param postfix Expresia în notație postfix (ex: "3 4 + 2 *")
 * @return Rădăcina arborelui creat
 *
 * Algoritm:
 *   1. Parcurgeți expresia token cu token
 *   2. Dacă token-ul este un număr:
 *      - Creați un nod operand
 *      - Push pe stack
 *   3. Dacă token-ul este un operator:
 *      - Pop 2 noduri de pe stack (right, apoi left)
 *      - Creați un nod operator cu cei 2 copii
 *      - Push nodul operator pe stack
 *   4. La final, pop și returnați rădăcina
 *
 * Hint: Folosiți strtok() pentru tokenizare și atof() pentru conversie
 */
ExprNode* build_tree_from_postfix(const char *postfix) {
    if (postfix == NULL || strlen(postfix) == 0) {
        return NULL;
    }
    
    NodeStack stack;
    stack_init(&stack);
    
    /* Copie pentru tokenizare (strtok modifică stringul) */
    char expr[MAX_EXPR_LENGTH];
    strncpy(expr, postfix, MAX_EXPR_LENGTH - 1);
    expr[MAX_EXPR_LENGTH - 1] = '\0';
    
    char *token = strtok(expr, " ");
    
    while (token != NULL) {
        /* YOUR CODE HERE */
        
        /* Verificați dacă token-ul este operator sau număr */
        /* Hint: strlen(token) == 1 && is_operator(token[0]) pentru operator */
        /* Hint: atof(token) pentru a converti string la double */
        
        if (strlen(token) == 1 && is_operator(token[0])) {
            /* TODO: Tratați cazul operator */
            /* 1. Pop right și left de pe stack */
            /* 2. Creați nod operator */
            /* 3. Push noul nod pe stack */
        } else {
            /* TODO: Tratați cazul operand (număr) */
            /* 1. Convertiți la double */
            /* 2. Creați nod operand */
            /* 3. Push pe stack */
        }
        
        token = strtok(NULL, " ");
    }
    
    /* Returnăm rădăcina (singurul element rămas pe stack) */
    if (stack_is_empty(&stack)) {
        return NULL;
    }
    
    return stack_pop(&stack);
}

/* =============================================================================
 * EVALUAREA EXPRESIEI
 * =============================================================================
 */

/**
 * TODO 5: Evaluați expresia reprezentată de arbore
 *
 * @param node Rădăcina subarborelui
 * @return Rezultatul evaluării
 *
 * Algoritm (parcurgere postorder):
 *   1. Dacă nodul este NULL, returnați 0
 *   2. Dacă nodul este operand, returnați valoarea
 *   3. Dacă nodul este operator:
 *      - Evaluați recursiv stânga și dreapta
 *      - Aplicați operatorul și returnați rezultatul
 *
 * ATENȚIE: Tratați împărțirea la zero!
 */
double evaluate_tree(ExprNode *node) {
    if (node == NULL) {
        return 0;
    }
    
    /* Caz operand - returnăm valoarea */
    if (node->type == NODE_OPERAND) {
        return node->value;
    }
    
    /* Caz operator - evaluăm recursiv */
    /* YOUR CODE HERE */
    
    double left_val = evaluate_tree(node->left);
    double right_val = evaluate_tree(node->right);
    
    /* TODO: Implementați operațiile */
    switch (node->op) {
        case '+':
            /* YOUR CODE HERE */
            break;
        case '-':
            /* YOUR CODE HERE */
            break;
        case '*':
            /* YOUR CODE HERE */
            break;
        case '/':
            /* YOUR CODE HERE */
            /* ATENȚIE: verificați împărțirea la zero! */
            break;
        default:
            fprintf(stderr, "Operator necunoscut: %c\n", node->op);
            return 0;
    }
    
    return 0;  /* Înlocuiți cu implementarea corectă */
}

/* =============================================================================
 * AFIȘARE ÎN DIFERITE NOTAȚII
 * =============================================================================
 */

/**
 * TODO 6: Afișați expresia în notație INFIX (cu paranteze)
 *
 * Algoritm (parcurgere inorder):
 *   1. Dacă nodul este operand, afișați valoarea
 *   2. Dacă nodul este operator:
 *      - Afișați "("
 *      - Afișați subarborele stâng
 *      - Afișați operatorul
 *      - Afișați subarborele drept
 *      - Afișați ")"
 */
void print_infix(ExprNode *node) {
    if (node == NULL) return;
    
    if (node->type == NODE_OPERAND) {
        /* Afișăm valoarea (fără zecimale dacă e număr întreg) */
        if (node->value == (int)node->value) {
            printf("%.0f", node->value);
        } else {
            printf("%.2f", node->value);
        }
    } else {
        /* YOUR CODE HERE */
        /* Hint: paranteze pentru a păstra ordinea operațiilor */
    }
}

/**
 * TODO 7: Afișați expresia în notație PREFIX (poloneză)
 *
 * Algoritm (parcurgere preorder):
 *   Operator/Operand -> Stânga -> Dreapta
 */
void print_prefix(ExprNode *node) {
    if (node == NULL) return;
    
    /* YOUR CODE HERE */
}

/**
 * TODO 8: Afișați expresia în notație POSTFIX (poloneză inversă)
 *
 * Algoritm (parcurgere postorder):
 *   Stânga -> Dreapta -> Operator/Operand
 */
void print_postfix(ExprNode *node) {
    if (node == NULL) return;
    
    /* YOUR CODE HERE */
}

/* =============================================================================
 * FUNCȚII STATISTICE
 * =============================================================================
 */

/**
 * TODO 9: Calculați înălțimea arborelui de expresie
 */
int tree_height(ExprNode *node) {
    /* YOUR CODE HERE */
    
    return -1;  /* Înlocuiți cu implementarea corectă */
}

/**
 * Numără nodurile din arbore
 */
int count_nodes(ExprNode *node) {
    if (node == NULL) return 0;
    return 1 + count_nodes(node->left) + count_nodes(node->right);
}

/* =============================================================================
 * SIMPLIFICARE EXPRESII (BONUS)
 * =============================================================================
 */

/**
 * TODO 10 (BONUS): Simplificați expresii
 *
 * Reguli de simplificare:
 *   - x + 0 = x
 *   - 0 + x = x
 *   - x - 0 = x
 *   - x * 1 = x
 *   - 1 * x = x
 *   - x * 0 = 0
 *   - 0 * x = 0
 *   - x / 1 = x
 *   - 0 / x = 0 (dacă x != 0)
 *
 * @param node Rădăcina subarborelui
 * @return Rădăcina arborelui simplificat
 */
ExprNode* simplify_tree(ExprNode *node) {
    if (node == NULL) return NULL;
    
    /* Operanzii nu se simplifică */
    if (node->type == NODE_OPERAND) {
        return node;
    }
    
    /* Simplificăm mai întâi subarborii */
    node->left = simplify_tree(node->left);
    node->right = simplify_tree(node->right);
    
    /* YOUR CODE HERE - Implementați regulile de simplificare */
    
    /* Exemplu pentru x + 0 = x:
     * if (node->op == '+' && 
     *     node->right->type == NODE_OPERAND && 
     *     node->right->value == 0) {
     *     ExprNode *result = node->left;
     *     free(node->right);
     *     free(node);
     *     return result;
     * }
     */
    
    return node;
}

/* =============================================================================
 * AFIȘARE ARBORE (VIZUALIZARE)
 * =============================================================================
 */

void print_tree_visual(ExprNode *node, int level, char prefix) {
    if (node == NULL) return;
    
    /* Indentare */
    for (int i = 0; i < level; i++) {
        printf("    ");
    }
    
    if (level > 0) {
        printf("%c── ", prefix);
    }
    
    /* Afișare nod */
    if (node->type == NODE_OPERAND) {
        if (node->value == (int)node->value) {
            printf("%.0f\n", node->value);
        } else {
            printf("%.2f\n", node->value);
        }
    } else {
        printf("%c\n", node->op);
    }
    
    /* Afișare subarbori */
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
    printf("║     EXERCIȚIUL 2: ARBORE DE EXPRESII ARITMETICE               ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    
    /* Demonstrații cu expresii predefinite */
    printf("\n═══ DEMONSTRAȚII ═══\n");
    
    /* Expresie simplă: (3 + 4) * 2 = 14 */
    demo_expression("3 4 + 2 *");
    
    /* Expresie mai complexă: (5 + 3) * (10 - 2) = 64 */
    demo_expression("5 3 + 10 2 - *");
    
    /* Expresie cu împărțire: (8 / 2) + 3 = 7 */
    demo_expression("8 2 / 3 +");
    
    /* Mod interactiv */
    printf("\n═══ MOD INTERACTIV ═══\n");
    printf("Introduceți expresii în notație postfix (sau 'exit' pentru ieșire).\n");
    printf("Exemplu: 3 4 + 2 *  ->  (3 + 4) * 2 = 14\n\n");
    
    char input[MAX_EXPR_LENGTH];
    
    while (1) {
        printf("Expresie postfix: ");
        
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }
        
        /* Eliminăm newline */
        input[strcspn(input, "\n")] = 0;
        
        if (strcmp(input, "exit") == 0 || strlen(input) == 0) {
            break;
        }
        
        demo_expression(input);
    }
    
    printf("\nLa revedere!\n");
    
    return 0;
}

/* =============================================================================
 * PROVOCĂRI BONUS (Opțional)
 * =============================================================================
 *
 * 1. Implementați parsarea expresiilor INFIX (cu paranteze) și conversia la 
 *    arbore de expresie. Hint: Algoritmul Shunting-Yard
 *
 * 2. Adăugați suport pentru funcții: sin, cos, sqrt, pow
 *    Exemplu: "3 4 + sqrt" -> sqrt(3 + 4) = 2.646
 *
 * 3. Implementați derivarea simbolică a expresiilor
 *    Exemplu: derivata lui "x * x" = "2 * x"
 *
 * 4. Adăugați suport pentru variabile și evaluare cu valori specifice
 *    Exemplu: "x 2 *" cu x=5 -> 10
 *
 * 5. Implementați generarea de cod (ex: instrucțiuni pentru o mașină stivă)
 *
 * =============================================================================
 */
