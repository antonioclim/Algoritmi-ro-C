/**
 * =============================================================================
 * EXERCIȚIUL 1: Verificator de Paranteze
 * =============================================================================
 *
 * OBIECTIV:
 *   Implementează un verificator care determină dacă o expresie are
 *   parantezele corect echilibrate și în ordine validă.
 *
 * CERINȚE:
 *   1. Suportă trei tipuri de paranteze: (), [], {}
 *   2. Verifică atât echilibrarea cât și ordinea corectă
 *   3. Returnează poziția primei erori sau -1 pentru expresii valide
 *   4. Gestionează corect șirurile goale și fără paranteze
 *   5. Ignoră toate celelalte caractere (litere, cifre, operatori)
 *
 * EXEMPLE:
 *   "{[()]}"         → Valid (returnează -1)
 *   "((()))"         → Valid (returnează -1)
 *   "{[(])}"         → Invalid (returnează 3 - ordinea greșită)
 *   "((("            → Invalid (returnează 3 - paranteze neînchise)
 *   "a + (b * c)"    → Valid (returnează -1)
 *   ""               → Valid (returnează -1)
 *   "]["             → Invalid (returnează 0)
 *
 * COMPILARE: gcc -Wall -Wextra -std=c11 -o exercise1 exercise1.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* =============================================================================
 * CONSTANTE
 * =============================================================================
 */

#define MAX_STACK_SIZE 1000
#define MAX_INPUT_LEN 1024

/* =============================================================================
 * STRUCTURA STIVEI PENTRU CARACTERE
 * =============================================================================
 */

/**
 * TODO 1: Definește structura CharStack
 *
 * Structura trebuie să conțină:
 *   - Un vector de caractere pentru stocare (dimensiune MAX_STACK_SIZE)
 *   - Un câmp întreg pentru indexul vârfului (top)
 *
 * Hint: Folosește convenția top = -1 pentru stivă goală
 */
typedef struct {
    /* YOUR CODE HERE */
    char data[MAX_STACK_SIZE];
    int top;
} CharStack;

/* =============================================================================
 * FUNCȚII PENTRU STIVĂ
 * =============================================================================
 */

/**
 * TODO 2: Implementează funcția de inițializare a stivei
 *
 * @param stack Pointer la stiva de inițializat
 *
 * Trebuie să setezi top la valoarea care indică stivă goală (-1)
 */
void stack_init(CharStack *stack) {
    /* YOUR CODE HERE */

}

/**
 * TODO 3: Implementează funcția care verifică dacă stiva este goală
 *
 * @param stack Pointer la stivă (const - nu modifică stiva)
 * @return true dacă stiva este goală, false altfel
 */
bool stack_is_empty(const CharStack *stack) {
    /* YOUR CODE HERE */
    return true;  /* Înlocuiește cu implementarea corectă */
}

/**
 * TODO 4: Implementează funcția push
 *
 * @param stack Pointer la stivă
 * @param c Caracterul de adăugat
 * @return true dacă operația a reușit, false la stack overflow
 *
 * Pași:
 *   1. Verifică dacă stiva nu este plină (top < MAX_STACK_SIZE - 1)
 *   2. Incrementează top
 *   3. Adaugă caracterul la poziția top
 *   4. Returnează true
 */
bool stack_push(CharStack *stack, char c) {
    /* YOUR CODE HERE */
    return false;  /* Înlocuiește cu implementarea corectă */
}

/**
 * TODO 5: Implementează funcția pop
 *
 * @param stack Pointer la stivă
 * @param c Pointer unde se va stoca caracterul eliminat
 * @return true dacă operația a reușit, false la stack underflow
 *
 * Pași:
 *   1. Verifică dacă stiva nu este goală
 *   2. Copiază caracterul de la vârf în *c
 *   3. Decrementează top
 *   4. Returnează true
 */
bool stack_pop(CharStack *stack, char *c) {
    /* YOUR CODE HERE */
    return false;  /* Înlocuiește cu implementarea corectă */
}

/* =============================================================================
 * FUNCȚII HELPER PENTRU PARANTEZE
 * =============================================================================
 */

/**
 * TODO 6: Implementează funcția care verifică dacă un caracter
 *         este o paranteză de deschidere
 *
 * @param c Caracterul de verificat
 * @return true dacă c este '(', '[' sau '{'
 */
bool is_opening_bracket(char c) {
    /* YOUR CODE HERE */
    return false;  /* Înlocuiește cu implementarea corectă */
}

/**
 * TODO 7: Implementează funcția care verifică dacă un caracter
 *         este o paranteză de închidere
 *
 * @param c Caracterul de verificat
 * @return true dacă c este ')', ']' sau '}'
 */
bool is_closing_bracket(char c) {
    /* YOUR CODE HERE */
    return false;  /* Înlocuiește cu implementarea corectă */
}

/**
 * TODO 8: Implementează funcția care verifică dacă două paranteze
 *         formează o pereche validă
 *
 * @param open Paranteza de deschidere
 * @param close Paranteza de închidere
 * @return true dacă formează pereche: () [] {}
 *
 * Perechile valide sunt:
 *   '(' cu ')'
 *   '[' cu ']'
 *   '{' cu '}'
 */
bool brackets_match(char open, char close) {
    /* YOUR CODE HERE */
    return false;  /* Înlocuiește cu implementarea corectă */
}

/* =============================================================================
 * FUNCȚIA PRINCIPALĂ DE VERIFICARE
 * =============================================================================
 */

/**
 * TODO 9: Implementează funcția principală de verificare a parantezelor
 *
 * @param expression Șirul de caractere de verificat
 * @return -1 dacă expresia este validă, sau poziția (0-indexed) a primei erori
 *
 * Algoritm:
 *   1. Inițializează o stivă goală
 *   2. Parcurge fiecare caracter din expresie (folosește un index i):
 *      a. Dacă e paranteză de deschidere → push pe stivă
 *      b. Dacă e paranteză de închidere:
 *         - Dacă stiva e goală → returnează i (eroare)
 *         - Pop de pe stivă și verifică potrivirea
 *         - Dacă nu se potrivesc → returnează i (eroare)
 *      c. Altfel (alte caractere) → ignoră
 *   3. După parcurgere:
 *      - Dacă stiva NU este goală → returnează lungimea șirului (paranteze neînchise)
 *      - Altfel → returnează -1 (valid)
 *
 * ATENȚIE: Returnează poziția primei erori, nu codul de eroare!
 */
int check_brackets(const char *expression) {
    CharStack stack;
    
    /* YOUR CODE HERE */
    
    /* Parcurge expresia caracter cu caracter */
    for (int i = 0; expression[i] != '\0'; i++) {
        char c = expression[i];
        
        /* YOUR CODE HERE */
        /* Implementează logica pentru paranteze de deschidere și închidere */
        
    }
    
    /* YOUR CODE HERE */
    /* Verifică dacă au rămas paranteze neînchise */
    
    return -1;  /* Placeholder - înlocuiește cu logica corectă */
}

/* =============================================================================
 * FUNCȚIE DE AFIȘARE REZULTAT
 * =============================================================================
 */

/**
 * Afișează rezultatul verificării într-un format frumos
 */
void print_result(const char *expression, int result) {
    printf("Expresie: \"%s\"\n", expression);
    printf("Rezultat: ");
    
    if (result == -1) {
        printf("\033[0;32m✓ Valid\033[0m\n");
    } else {
        printf("\033[0;31m✗ Invalid la poziția %d\033[0m\n", result);
        
        /* Afișează indicatorul de eroare */
        printf("          \"");
        for (int i = 0; expression[i] != '\0'; i++) {
            if (i == result) {
                printf("\033[0;31m%c\033[0m", expression[i]);
            } else {
                printf("%c", expression[i]);
            }
        }
        printf("\"\n");
        printf("           ");
        for (int i = 0; i < result; i++) {
            printf(" ");
        }
        printf("\033[0;31m^\033[0m eroare aici\n");
    }
    printf("\n");
}

/* =============================================================================
 * PROGRAMUL PRINCIPAL
 * =============================================================================
 */

int main(void) {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║         EXERCIȚIUL 1: Verificator de Paranteze                ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    
    /* Cazuri de test */
    const char *test_cases[] = {
        "{[()]}",           /* Valid */
        "((()))",           /* Valid */
        "[{()}]",           /* Valid */
        "a + (b * [c - d])", /* Valid */
        "",                 /* Valid (gol) */
        "hello world",      /* Valid (fără paranteze) */
        "{[(])}",           /* Invalid - ordine greșită */
        "(((",              /* Invalid - neînchise */
        "())",              /* Invalid - prea multe închideri */
        "][",               /* Invalid - închidere fără deschidere */
        "([)]",             /* Invalid - intercalare */
        "{[}]",             /* Invalid - intercalare */
        NULL
    };
    
    printf("Teste predefinite:\n");
    printf("─────────────────────────────────────────────────────────────────\n\n");
    
    for (int i = 0; test_cases[i] != NULL; i++) {
        int result = check_brackets(test_cases[i]);
        print_result(test_cases[i], result);
    }
    
    /* Mod interactiv */
    printf("─────────────────────────────────────────────────────────────────\n");
    printf("Mod interactiv (tastează 'q' pentru a ieși):\n");
    printf("─────────────────────────────────────────────────────────────────\n\n");
    
    char input[MAX_INPUT_LEN];
    while (1) {
        printf("Introdu expresia: ");
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
        
        int result = check_brackets(input);
        print_result(input, result);
    }
    
    printf("\nLa revedere!\n\n");
    
    return 0;
}

/* =============================================================================
 * PROVOCĂRI BONUS (Opționale)
 * =============================================================================
 *
 * 1. Extinde verificatorul pentru a suporta și paranteze unghiulare < >
 *
 * 2. Adaugă suport pentru comentarii C și C++ care ar trebui ignorate
 *    la verificare (comentarii bloc și comentarii linie)
 *
 * 3. Implementează o funcție care nu doar verifică, ci și corectează
 *    automat expresia (adăugând parantezele lipsă)
 *
 * 4. Adaugă statistici: câte paranteze de fiecare tip sunt în expresie
 *
 * 5. Implementează verificarea pentru expresii HTML cu tag-uri
 *    (ex: <div><p></p></div>)
 *
 * =============================================================================
 */
