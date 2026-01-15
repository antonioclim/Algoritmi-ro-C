/**
 * =============================================================================
 * EXERCIȚIUL 2: Convertor Infix → Postfix și Evaluator RPN
 * =============================================================================
 *
 * OBIECTIV:
 *   Implementează algoritmul Shunting Yard pentru a converti expresii
 *   matematice din notație infixă în notație postfixă (RPN), apoi
 *   evaluează expresia postfixă rezultată.
 *
 * CERINȚE:
 *   1. Suportă operatorii: + - * / % ^
 *   2. Respectă precedența operatorilor (^ > * / % > + -)
 *   3. Respectă asociativitatea (^ este asociativ la dreapta)
 *   4. Gestionează paranteze ()
 *   5. Funcționează cu operanzi de o singură cifră (0-9)
 *   6. Evaluează expresia postfixă și returnează rezultatul
 *
 * PRECEDENȚA OPERATORILOR:
 *   ^       → 3 (cea mai mare)  - asociativ la DREAPTA
 *   * / %   → 2                 - asociativi la STÂNGA
 *   + -     → 1 (cea mai mică)  - asociativi la STÂNGA
 *
 * EXEMPLE:
 *   Infix: "3+4"         → Postfix: "3 4 +"       → Rezultat: 7
 *   Infix: "3+4*5"       → Postfix: "3 4 5 * +"  → Rezultat: 23
 *   Infix: "(3+4)*5"     → Postfix: "3 4 + 5 *"  → Rezultat: 35
 *   Infix: "2^3^2"       → Postfix: "2 3 2 ^ ^"  → Rezultat: 512
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -o exercise2 exercise2.c -lm
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

/* =============================================================================
 * CONSTANTE
 * =============================================================================
 */

#define MAX_STACK_SIZE 100
#define MAX_EXPR_LEN 256

/* =============================================================================
 * STRUCTURILE STIVELOR
 * =============================================================================
 */

/**
 * Stivă pentru caractere (folosită pentru operatori în Shunting Yard)
 */
typedef struct {
    char data[MAX_STACK_SIZE];
    int top;
} CharStack;

/**
 * Stivă pentru numere întregi (folosită pentru evaluarea RPN)
 */
typedef struct {
    int data[MAX_STACK_SIZE];
    int top;
} IntStack;

/* =============================================================================
 * FUNCȚII PENTRU STIVA DE CARACTERE
 * =============================================================================
 */

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
 * FUNCȚII PENTRU STIVA DE ÎNTREGI
 * =============================================================================
 */

/**
 * TODO 1: Implementează funcția de inițializare pentru stiva de întregi
 */
void int_stack_init(IntStack *s) {
    /* YOUR CODE HERE */

}

/**
 * TODO 2: Implementează funcția is_empty pentru stiva de întregi
 */
bool int_stack_is_empty(const IntStack *s) {
    /* YOUR CODE HERE */
    return true;  /* Înlocuiește */
}

/**
 * TODO 3: Implementează funcția push pentru stiva de întregi
 */
bool int_stack_push(IntStack *s, int value) {
    /* YOUR CODE HERE */
    return false;  /* Înlocuiește */
}

/**
 * TODO 4: Implementează funcția pop pentru stiva de întregi
 */
bool int_stack_pop(IntStack *s, int *value) {
    /* YOUR CODE HERE */
    return false;  /* Înlocuiește */
}

/* =============================================================================
 * FUNCȚII HELPER PENTRU OPERATORI
 * =============================================================================
 */

/**
 * TODO 5: Implementează funcția care verifică dacă un caracter este operator
 *
 * @param c Caracterul de verificat
 * @return true dacă c este unul dintre: + - * / % ^
 */
bool is_operator(char c) {
    /* YOUR CODE HERE */
    return false;  /* Înlocuiește */
}

/**
 * TODO 6: Implementează funcția care returnează precedența unui operator
 *
 * @param op Operatorul
 * @return Precedența: 3 pentru ^, 2 pentru * / %, 1 pentru + -, 0 pentru altele
 *
 * Tabel de precedență:
 *   ^       → 3
 *   * / %   → 2
 *   + -     → 1
 *   altele  → 0
 */
int get_precedence(char op) {
    /* YOUR CODE HERE */
    return 0;  /* Înlocuiește */
}

/**
 * TODO 7: Implementează funcția care verifică asociativitatea
 *
 * @param op Operatorul
 * @return true dacă operatorul este asociativ la STÂNGA, false pentru DREAPTA
 *
 * Notă: Doar ^ (putere) este asociativ la dreapta!
 *       Toate celelalte sunt asociative la stânga.
 *
 * Exemplu asociativitate stânga: 6 - 3 - 1 = (6 - 3) - 1 = 2
 * Exemplu asociativitate dreapta: 2 ^ 3 ^ 2 = 2 ^ (3 ^ 2) = 512
 */
bool is_left_associative(char op) {
    /* YOUR CODE HERE */
    return true;  /* Înlocuiește */
}

/**
 * TODO 8: Implementează funcția care aplică un operator pe doi operanzi
 *
 * @param op Operatorul de aplicat
 * @param a Primul operand (cel din stânga în expresia originală)
 * @param b Al doilea operand (cel din dreapta)
 * @return Rezultatul operației
 *
 * Operații de implementat:
 *   '+' → a + b
 *   '-' → a - b
 *   '*' → a * b
 *   '/' → a / b (atenție la împărțirea cu 0!)
 *   '%' → a % b
 *   '^' → a la puterea b (folosește pow() din math.h)
 *
 * ATENȚIE la ordinea operanzilor! În RPN, când întâlnim un operator:
 *   - Pop b (al doilea)
 *   - Pop a (primul)
 *   - Calculează a OP b
 */
int apply_operator(char op, int a, int b) {
    /* YOUR CODE HERE */
    
    switch (op) {
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
            /* Verifică împărțirea cu zero! */
            break;
        case '%':
            /* YOUR CODE HERE */
            break;
        case '^':
            /* YOUR CODE HERE */
            /* Folosește: (int)pow(a, b) */
            break;
    }
    
    return 0;  /* Înlocuiește cu return-ul corect în fiecare case */
}

/* =============================================================================
 * ALGORITMUL SHUNTING YARD
 * =============================================================================
 */

/**
 * TODO 9: Implementează algoritmul Shunting Yard pentru conversie infix → postfix
 *
 * @param infix Expresia în notație infixă (ex: "3+4*5")
 * @param postfix Buffer pentru rezultatul în notație postfixă
 *
 * ALGORITMUL SHUNTING YARD (Dijkstra):
 *
 * Pentru fiecare caracter din expresia infixă:
 *
 *   1. Dacă e CIFRĂ (operand):
 *      → Adaugă direct la ieșire (postfix)
 *      → Adaugă și un spațiu după cifră
 *
 *   2. Dacă e PARANTEZĂ DESCHISĂ '(':
 *      → Push pe stiva de operatori
 *
 *   3. Dacă e PARANTEZĂ ÎNCHISĂ ')':
 *      → Pop și adaugă la ieșire PÂNĂ când întâlnești '('
 *      → Elimină '(' din stivă (nu o adăuga la ieșire!)
 *
 *   4. Dacă e OPERATOR:
 *      → Cât timp stiva NU e goală ȘI vârful NU e '(' ȘI:
 *          (precedența(vârf) > precedența(op)) SAU
 *          (precedența(vârf) == precedența(op) ȘI op e asociativ la stânga)
 *        → Pop de pe stivă și adaugă la ieșire
 *      → Push operatorul curent pe stivă
 *
 *   5. Dacă e SPAȚIU:
 *      → Ignoră (continuă la următorul caracter)
 *
 * La final:
 *   → Pop toți operatorii rămași și adaugă-i la ieșire
 *
 * HINT: Folosește out_idx pentru a ține evidența poziției în buffer-ul postfix
 */
void infix_to_postfix(const char *infix, char *postfix) {
    CharStack op_stack;
    char_stack_init(&op_stack);
    
    int out_idx = 0;  /* Indexul curent în buffer-ul de ieșire */
    
    /* TODO: Parcurge expresia infixă */
    for (int i = 0; infix[i] != '\0'; i++) {
        char c = infix[i];
        
        /* Ignoră spațiile */
        if (c == ' ') {
            continue;
        }
        
        /* TODO 9a: Dacă e cifră (operand) */
        if (isdigit(c)) {
            /* YOUR CODE HERE */
            /* Adaugă cifra și un spațiu la postfix */
            
        }
        /* TODO 9b: Dacă e paranteză deschisă */
        else if (c == '(') {
            /* YOUR CODE HERE */
            
        }
        /* TODO 9c: Dacă e paranteză închisă */
        else if (c == ')') {
            /* YOUR CODE HERE */
            /* Pop până la '(' și adaugă operatorii la ieșire */
            
        }
        /* TODO 9d: Dacă e operator */
        else if (is_operator(c)) {
            /* YOUR CODE HERE */
            /* Implementează regula de precedență și asociativitate */
            
            /* Pseudocod:
             * WHILE stiva nu e goală AND vârful nu e '(':
             *     IF precedență(vârf) > precedență(c) OR
             *        (precedență(vârf) == precedență(c) AND c e asociativ la stânga):
             *         pop și adaugă la ieșire
             *     ELSE:
             *         break
             * push(c)
             */
            
        }
    }
    
    /* TODO 9e: Golește stiva de operatori rămasă */
    /* YOUR CODE HERE */
    /* Pop toți operatorii și adaugă-i la ieșire */
    
    
    /* Elimină ultimul spațiu dacă există și termină șirul */
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
 * TODO 10: Implementează evaluatorul de expresii postfixe (RPN)
 *
 * @param postfix Expresia în notație postfixă (ex: "3 4 5 * +")
 * @return Rezultatul evaluării
 *
 * ALGORITM:
 *   Pentru fiecare token din expresia postfixă:
 *     - Dacă e NUMĂR → push pe stivă
 *     - Dacă e OPERATOR:
 *         → Pop b (al doilea operand)
 *         → Pop a (primul operand)
 *         → Calculează rezultatul: a OP b
 *         → Push rezultatul pe stivă
 *   La final, stiva conține un singur element: rezultatul
 *
 * ATENȚIE: În postfix, când întâlnești un operator, operanzii sunt deja
 *          pe stivă în ordinea corectă. Pop-ul îi scoate în ordine inversă!
 *          Deci: pop → b, pop → a, apoi calculează a OP b
 *
 * HINT: Tokens-urile sunt separate prin spații
 */
int evaluate_postfix(const char *postfix) {
    IntStack stack;
    int_stack_init(&stack);
    
    /* Variabile pentru construirea numerelor multi-cifră */
    int current_num = 0;
    bool building_number = false;
    
    for (int i = 0; postfix[i] != '\0'; i++) {
        char c = postfix[i];
        
        /* TODO 10a: Dacă e cifră */
        if (isdigit(c)) {
            /* YOUR CODE HERE */
            /* Construiește numărul (poate fi multi-cifră) */
            /* current_num = current_num * 10 + (c - '0'); */
            /* building_number = true; */
            
        }
        /* Dacă e spațiu sau operator */
        else {
            /* Dacă tocmai am terminat de construit un număr */
            if (building_number) {
                /* YOUR CODE HERE */
                /* Push numărul pe stivă și resetează */
                
                building_number = false;
                current_num = 0;
            }
            
            /* TODO 10b: Dacă e operator */
            if (is_operator(c)) {
                /* YOUR CODE HERE */
                /* Pop b, pop a, calculează a OP b, push rezultat */
                
            }
        }
    }
    
    /* Ultimul număr (dacă expresia nu se termină cu spațiu) */
    if (building_number) {
        int_stack_push(&stack, current_num);
    }
    
    /* TODO 10c: Returnează rezultatul final */
    int result = 0;
    /* YOUR CODE HERE */
    /* Pop rezultatul final din stivă */
    
    return result;
}

/* =============================================================================
 * FUNCȚIE COMBINATĂ: EVALUEAZĂ EXPRESIE INFIXĂ
 * =============================================================================
 */

/**
 * Evaluează o expresie infixă
 * Combină conversia și evaluarea
 */
int evaluate_infix(const char *infix) {
    char postfix[MAX_EXPR_LEN];
    infix_to_postfix(infix, postfix);
    return evaluate_postfix(postfix);
}

/* =============================================================================
 * PROGRAMUL PRINCIPAL
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║     EXERCIȚIUL 2: Convertor Infix → Postfix + Evaluator       ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    /* Cazuri de test */
    typedef struct {
        const char *infix;
        int expected;
    } TestCase;
    
    TestCase tests[] = {
        {"3+4",         7},
        {"3+4*5",       23},
        {"(3+4)*5",     35},
        {"2+3*4-5",     9},
        {"(2+3)*(4-1)", 15},
        {"8/4/2",       1},      /* Asociativitate stânga: (8/4)/2 = 1 */
        {"2^3",         8},
        {"2^3^2",       512},    /* Asociativitate dreapta: 2^(3^2) = 2^9 = 512 */
        {"(1+2)*(3+4)", 21},
        {"5%3",         2},
        {NULL,          0}
    };
    
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("                      TESTE AUTOMATE                           \n");
    printf("═══════════════════════════════════════════════════════════════\n\n");
    
    int passed = 0;
    int total = 0;
    
    for (int i = 0; tests[i].infix != NULL; i++) {
        total++;
        char postfix[MAX_EXPR_LEN];
        infix_to_postfix(tests[i].infix, postfix);
        int result = evaluate_postfix(postfix);
        
        printf("Test %2d: ", i + 1);
        printf("Infix: %-15s", tests[i].infix);
        printf("→ Postfix: %-15s", postfix);
        printf("→ Rezultat: %4d ", result);
        
        if (result == tests[i].expected) {
            printf("\033[0;32m✓ CORECT\033[0m\n");
            passed++;
        } else {
            printf("\033[0;31m✗ GREȘIT (așteptat: %d)\033[0m\n", tests[i].expected);
        }
    }
    
    printf("\n═══════════════════════════════════════════════════════════════\n");
    printf("Rezultat: %d/%d teste trecute", passed, total);
    if (passed == total) {
        printf(" \033[0;32m- FELICITĂRI!\033[0m\n");
    } else {
        printf(" \033[0;33m- Mai e de lucru!\033[0m\n");
    }
    printf("═══════════════════════════════════════════════════════════════\n");
    
    /* Mod interactiv */
    printf("\n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("                     MOD INTERACTIV                            \n");
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("Introdu expresii infixe (doar cifre 0-9, operatori + - * / %% ^)\n");
    printf("Tastează 'q' pentru a ieși.\n\n");
    
    char input[MAX_EXPR_LEN];
    char postfix[MAX_EXPR_LEN];
    
    while (1) {
        printf("Infix> ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }
        
        /* Elimină newline */
        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }
        
        /* Verifică ieșire */
        if (strcmp(input, "q") == 0 || strcmp(input, "Q") == 0) {
            break;
        }
        
        if (strlen(input) == 0) {
            continue;
        }
        
        /* Conversie și evaluare */
        infix_to_postfix(input, postfix);
        int result = evaluate_postfix(postfix);
        
        printf("  Postfix: %s\n", postfix);
        printf("  Rezultat: \033[0;32m%d\033[0m\n\n", result);
    }
    
    printf("\nLa revedere!\n\n");
    
    return 0;
}

/* =============================================================================
 * PROVOCĂRI BONUS (Opționale)
 * =============================================================================
 *
 * 1. Extinde pentru a suporta numere cu mai multe cifre și numere negative
 *    Hint: Parsează token-urile corect, nu caracter cu caracter
 *
 * 2. Adaugă suport pentru funcții matematice: sin, cos, sqrt, log
 *    Hint: Tratează funcțiile similar cu parantezele
 *
 * 3. Implementează variabile (ex: "x+y" cu x=3, y=4)
 *    Hint: Folosește un dicționar/hashmap pentru valori
 *
 * 4. Adaugă validare de expresii (detectează erori de sintaxă)
 *    Hint: Verifică operanzi consecutivi, operatori la început/sfârșit
 *
 * 5. Implementează și conversie postfix → infix (inversa)
 *    Hint: Folosește o stivă de stringuri
 *
 * =============================================================================
 */
