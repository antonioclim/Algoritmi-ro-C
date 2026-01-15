/**
 * =============================================================================
 * EXERCIȚIUL 2: Arbori de Expresii (Expression Trees)
 * =============================================================================
 *
 * OBIECTIV:
 *   Implementați un evaluator de expresii matematice folosind arbori binari.
 *   Expresiile sunt date în notație postfix (Reverse Polish Notation).
 *
 * CERINȚE:
 *   1. Completați structura ExprNode pentru nodurile arborelui de expresii
 *   2. Implementați construcția arborelui din expresie postfix
 *   3. Implementați evaluarea expresiei
 *   4. Implementați afișarea în diferite notații (infix, prefix, postfix)
 *   5. Tratați erorile (împărțire la zero, expresii invalide)
 *
 * NOTAȚIE POSTFIX:
 *   - Operanzii (numerele) apar înaintea operatorilor
 *   - Exemplu: "3 5 + 2 *" înseamnă (3 + 5) * 2 = 16
 *   - Exemplu: "10 5 / 3 +" înseamnă (10 / 5) + 3 = 5
 *
 * EXEMPLU INPUT:
 *   3 5 + 2 *
 *
 * OUTPUT AȘTEPTAT:
 *   Postfix: 3 5 + 2 *
 *   Prefix: * + 3 5 2
 *   Infix: ((3 + 5) * 2)
 *   Result: 16
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -o exercise2 exercise2.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Dimensiuni maxime */
#define MAX_EXPR_LEN 256
#define MAX_STACK_SIZE 100

/* =============================================================================
 * DEFINIREA STRUCTURILOR
 * =============================================================================
 */

/**
 * TODO 1: Completați structura ExprNode
 *
 * Un nod dintr-un arbore de expresii poate fi:
 *   - Un operand (număr): stochează valoarea în câmpul 'value', 'isOperator' = 0
 *   - Un operator (+, -, *, /): stochează operatorul în câmpul 'op', 'isOperator' = 1
 *
 * Structura trebuie să conțină:
 *   - int isOperator: 1 dacă nodul e operator, 0 dacă e operand
 *   - char op: caracterul operatorului ('+', '-', '*', '/')
 *   - int value: valoarea numerică (pentru operanzi)
 *   - struct ExprNode *left: pointer către copilul stâng
 *   - struct ExprNode *right: pointer către copilul drept
 */

typedef struct ExprNode {
    /* YOUR CODE HERE */
    
} ExprNode;


/**
 * Stack pentru pointeri la ExprNode (folosit la construcția arborelui)
 */
typedef struct {
    ExprNode *items[MAX_STACK_SIZE];
    int top;
} NodeStack;


/* =============================================================================
 * FUNCȚII PENTRU STACK
 * =============================================================================
 */

/**
 * Inițializează stack-ul
 */
void initStack(NodeStack *s) {
    s->top = -1;
}

/**
 * Verifică dacă stack-ul este gol
 */
int isStackEmpty(NodeStack *s) {
    return s->top == -1;
}

/**
 * TODO 2: Implementați funcția push
 *
 * Adaugă un element în vârful stack-ului.
 *
 * @param s Pointer către stack
 * @param node Nodul de adăugat
 *
 * Pași:
 *   1. Verificați dacă stack-ul nu este plin (top < MAX_STACK_SIZE - 1)
 *   2. Incrementați top
 *   3. Salvați nodul în items[top]
 */
void push(NodeStack *s, ExprNode *node) {
    /* YOUR CODE HERE */
    
}

/**
 * TODO 3: Implementați funcția pop
 *
 * Scoate și returnează elementul din vârful stack-ului.
 *
 * @param s Pointer către stack
 * @return Nodul din vârf sau NULL dacă stack-ul e gol
 *
 * Pași:
 *   1. Verificați dacă stack-ul nu este gol
 *   2. Salvați items[top] într-o variabilă
 *   3. Decrementați top
 *   4. Returnați nodul salvat
 */
ExprNode* pop(NodeStack *s) {
    /* YOUR CODE HERE */
    
    return NULL;  /* Înlocuiți această linie */
}


/* =============================================================================
 * FUNCȚII DE CREARE NODURI
 * =============================================================================
 */

/**
 * TODO 4: Implementați funcția createOperandNode
 *
 * Creează un nod operand (frunză) care stochează o valoare numerică.
 *
 * @param value Valoarea numerică
 * @return Pointer către nodul creat sau NULL la eroare
 *
 * Pași:
 *   1. Alocați memorie pentru un ExprNode
 *   2. Verificați dacă alocarea a reușit
 *   3. Setați isOperator = 0
 *   4. Setați value la valoarea primită
 *   5. Setați op la '\0' (sau orice, nu contează)
 *   6. Setați left și right la NULL
 *   7. Returnați nodul
 */
ExprNode* createOperandNode(int value) {
    /* YOUR CODE HERE */
    
    return NULL;  /* Înlocuiți această linie */
}

/**
 * TODO 5: Implementați funcția createOperatorNode
 *
 * Creează un nod operator care stochează un operator și are doi copii.
 *
 * @param op Operatorul ('+', '-', '*', '/')
 * @param left Copilul stâng (primul operand)
 * @param right Copilul drept (al doilea operand)
 * @return Pointer către nodul creat sau NULL la eroare
 *
 * Pași:
 *   1. Alocați memorie pentru un ExprNode
 *   2. Verificați dacă alocarea a reușit
 *   3. Setați isOperator = 1
 *   4. Setați op la operatorul primit
 *   5. Setați value la 0 (nu contează pentru operatori)
 *   6. Setați left și right la copiii primiți
 *   7. Returnați nodul
 */
ExprNode* createOperatorNode(char op, ExprNode *left, ExprNode *right) {
    /* YOUR CODE HERE */
    
    return NULL;  /* Înlocuiți această linie */
}


/* =============================================================================
 * FUNCȚII UTILITARE
 * =============================================================================
 */

/**
 * Verifică dacă un caracter este operator
 */
int isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/';
}

/**
 * Verifică dacă un șir reprezintă un număr (poate fi negativ)
 */
int isNumber(const char *str) {
    if (str == NULL || *str == '\0') {
        return 0;
    }
    
    /* Permitem semn negativ la început */
    if (*str == '-') {
        str++;
        if (*str == '\0') return 0;  /* Doar '-' nu e număr */
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
 * CONSTRUCȚIA ARBORELUI
 * =============================================================================
 */

/**
 * TODO 6: Implementați funcția buildExpressionTree
 *
 * Construiește arborele de expresii din notație postfix.
 *
 * ALGORITM:
 *   Pentru fiecare token din expresie:
 *     - Dacă e număr: creăm nod operand și îl punem în stack
 *     - Dacă e operator: scoatem 2 noduri din stack (right, apoi left),
 *       creăm nod operator cu acești copii, punem rezultatul în stack
 *   La final, în stack rămâne rădăcina arborelui.
 *
 * @param postfix Expresia în notație postfix (ex: "3 5 + 2 *")
 * @return Pointer către rădăcina arborelui sau NULL la eroare
 *
 * Pași:
 *   1. Inițializați stack-ul
 *   2. Folosiți strtok pentru a separa tokenurile (delimiter: " ")
 *   3. Pentru fiecare token:
 *      a) Dacă e număr (folosiți isNumber): creați nod operand, push în stack
 *      b) Dacă e operator (un singur caracter din "+-/*"):
 *         - Pop două noduri (ATENȚIE: ordinea! right = pop(), left = pop())
 *         - Creați nod operator cu left și right
 *         - Push nodul operator în stack
 *   4. La final, pop și returnați rădăcina
 *
 * Hint: atoi(token) convertește string la int
 * Hint: strlen(token) == 1 && isOperator(token[0]) verifică dacă e operator
 */
ExprNode* buildExpressionTree(const char *postfix) {
    /* Facem o copie a stringului deoarece strtok modifică stringul */
    char *expr = strdup(postfix);
    if (expr == NULL) {
        return NULL;
    }
    
    NodeStack stack;
    initStack(&stack);
    
    /* YOUR CODE HERE */
    /* Folosiți strtok(expr, " ") pentru primul token */
    /* și strtok(NULL, " ") pentru următoarele */
    
    
    free(expr);
    
    /* Returnați rădăcina (ultimul element din stack) */
    return NULL;  /* Înlocuiți această linie */
}


/* =============================================================================
 * EVALUAREA EXPRESIEI
 * =============================================================================
 */

/**
 * TODO 7: Implementați funcția evaluateExpression
 *
 * Evaluează recursiv arborele de expresii și returnează rezultatul.
 *
 * @param node Rădăcina (sub)arborelui
 * @param error Pointer pentru semnalarea erorilor (1 = eroare, 0 = ok)
 * @return Rezultatul evaluării
 *
 * ALGORITM (traversare postorder implicită):
 *   1. Dacă node == NULL: setați *error = 1, returnați 0
 *   2. Dacă nodul e operand (isOperator == 0): returnați node->value
 *   3. Dacă nodul e operator:
 *      a) Evaluați recursiv copilul stâng
 *      b) Evaluați recursiv copilul drept
 *      c) Aplicați operatorul și returnați rezultatul
 *      d) ATENȚIE la împărțirea la zero! Setați *error = 1 și returnați 0
 *
 * Hint pentru switch:
 *   switch (node->op) {
 *       case '+': return left + right;
 *       case '-': return left - right;
 *       ...
 *   }
 */
int evaluateExpression(ExprNode *node, int *error) {
    /* YOUR CODE HERE */
    
    return 0;  /* Înlocuiți această linie */
}


/* =============================================================================
 * AFIȘAREA ÎN DIFERITE NOTAȚII
 * =============================================================================
 */

/**
 * TODO 8: Implementați funcția printPostfix
 *
 * Afișează expresia în notație postfix (traversare postorder).
 *
 * @param node Rădăcina arborelui
 *
 * Pași:
 *   1. Dacă node == NULL, returnați
 *   2. Apelați recursiv pentru copilul stâng
 *   3. Apelați recursiv pentru copilul drept
 *   4. Afișați nodul curent:
 *      - Dacă e operand: printf("%d ", node->value)
 *      - Dacă e operator: printf("%c ", node->op)
 */
void printPostfix(ExprNode *node) {
    /* YOUR CODE HERE */
    
}

/**
 * TODO 9: Implementați funcția printPrefix
 *
 * Afișează expresia în notație prefix (traversare preorder).
 *
 * @param node Rădăcina arborelui
 *
 * Similar cu postfix, dar afișați nodul ÎNAINTE de copii.
 */
void printPrefix(ExprNode *node) {
    /* YOUR CODE HERE */
    
}

/**
 * TODO 10: Implementați funcția printInfix
 *
 * Afișează expresia în notație infix cu paranteze complete.
 *
 * @param node Rădăcina arborelui
 *
 * ALGORITM:
 *   1. Dacă node == NULL, returnați
 *   2. Dacă e operand: afișați valoarea
 *   3. Dacă e operator:
 *      a) Afișați "("
 *      b) Apelați recursiv pentru stânga
 *      c) Afișați " op " (operatorul cu spații)
 *      d) Apelați recursiv pentru dreapta
 *      e) Afișați ")"
 *
 * Exemplu output pentru (3+5)*2: "((3 + 5) * 2)"
 */
void printInfix(ExprNode *node) {
    /* YOUR CODE HERE */
    
}


/* =============================================================================
 * ELIBERAREA MEMORIEI
 * =============================================================================
 */

/**
 * TODO 11: Implementați funcția freeExprTree
 *
 * Eliberează toată memoria ocupată de arborele de expresii.
 * Folosiți traversarea POSTORDER!
 *
 * @param node Rădăcina arborelui
 */
void freeExprTree(ExprNode *node) {
    /* YOUR CODE HERE */
    
}


/* =============================================================================
 * FUNCȚII AUXILIARE PENTRU AFIȘARE
 * =============================================================================
 */

/**
 * Afișează arborele ca structură orizontală (pentru debugging)
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
 * FUNCȚIA MAIN - NU MODIFICAȚI
 * =============================================================================
 */

int main(void) {
    char expression[MAX_EXPR_LEN];
    
    /* Citim expresia postfix */
    if (fgets(expression, MAX_EXPR_LEN, stdin) == NULL) {
        fprintf(stderr, "Eroare: Nu s-a putut citi expresia\n");
        return 1;
    }
    
    /* Eliminăm newline-ul de la final */
    size_t len = strlen(expression);
    if (len > 0 && expression[len - 1] == '\n') {
        expression[len - 1] = '\0';
    }
    
    /* Verificăm dacă expresia e goală */
    if (strlen(expression) == 0) {
        fprintf(stderr, "Eroare: Expresie goală\n");
        return 1;
    }
    
    /* Construim arborele de expresii */
    ExprNode *root = buildExpressionTree(expression);
    
    if (root == NULL) {
        fprintf(stderr, "Eroare: Nu s-a putut construi arborele de expresii\n");
        return 1;
    }
    
    /* Afișăm în diferite notații */
    printf("Postfix: ");
    printPostfix(root);
    printf("\n");
    
    printf("Prefix: ");
    printPrefix(root);
    printf("\n");
    
    printf("Infix: ");
    printInfix(root);
    printf("\n");
    
    /* Evaluăm expresia */
    int error = 0;
    int result = evaluateExpression(root, &error);
    
    if (error) {
        printf("Result: ERROR (division by zero or invalid expression)\n");
    } else {
        printf("Result: %d\n", result);
    }
    
    /* Eliberăm memoria */
    freeExprTree(root);
    
    return 0;
}


/* =============================================================================
 * PROVOCĂRI BONUS (OPȚIONAL)
 * =============================================================================
 *
 * BONUS 1: Adăugați suport pentru operatorul putere (^)
 *          Hint: folosiți funcția pow() din <math.h>
 *
 * BONUS 2: Implementați funcția simplifyTree care simplifică expresii:
 *          - x + 0 = x, x * 1 = x, x * 0 = 0, etc.
 *
 * BONUS 3: Implementați suport pentru numere cu virgulă (double)
 *          Modificați structura și funcțiile corespunzător.
 *
 * BONUS 4: Implementați conversia din notație infix în postfix
 *          folosind algoritmul Shunting Yard.
 *
 * BONUS 5: Adăugați suport pentru variabile (ex: "x 5 + 2 *")
 *          și evaluați expresia pentru diferite valori ale lui x.
 *
 * =============================================================================
 */
