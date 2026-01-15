# =============================================================================
# SĂPTĂMÂNA 5: STIVE (STACK) - Makefile
# =============================================================================
# 
# Utilizare:
#   make          - Compilează toate țintele
#   make run      - Rulează exemplul principal
#   make test     - Rulează testele automate
#   make valgrind - Verifică memory leaks
#   make clean    - Șterge fișierele compilate
#   make help     - Afișează acest mesaj
#
# =============================================================================

# Compilator și flaguri
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
LDFLAGS = -lm

# Directoare
SRC_DIR = src
SOL_DIR = solution
TEST_DIR = tests
DATA_DIR = data

# Culori pentru output
GREEN = \033[0;32m
RED = \033[0;31m
YELLOW = \033[0;33m
BLUE = \033[0;34m
CYAN = \033[0;36m
MAGENTA = \033[0;35m
NC = \033[0m

# Ținte principale
TARGETS = example1 exercise1 exercise2

# Ținte pentru soluții
SOLUTIONS = exercise1_sol exercise2_sol homework1_sol homework2_sol

# =============================================================================
# REGULI PRINCIPALE
# =============================================================================

.PHONY: all clean run test valgrind help solutions

all: $(TARGETS)
	@echo ""
	@echo "$(GREEN)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(GREEN)║  ✓ Toate țintele au fost compilate cu succes!                  ║$(NC)"
	@echo "$(GREEN)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(CYAN)Rulează:$(NC)"
	@echo "  $(YELLOW)./example1$(NC)   - Exemplu complet demonstrativ"
	@echo "  $(YELLOW)./exercise1$(NC)  - Exercițiu verificare paranteze"
	@echo "  $(YELLOW)./exercise2$(NC)  - Exercițiu conversie infix→postfix"
	@echo ""

# =============================================================================
# COMPILARE EXEMPLE ȘI EXERCIȚII
# =============================================================================

example1: $(SRC_DIR)/example1.c
	@echo "$(CYAN)Compilare$(NC) example1.c..."
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ example1 compilat$(NC)"

exercise1: $(SRC_DIR)/exercise1.c
	@echo "$(CYAN)Compilare$(NC) exercise1.c..."
	@$(CC) $(CFLAGS) -o $@ $<
	@echo "$(GREEN)  ✓ exercise1 compilat$(NC)"

exercise2: $(SRC_DIR)/exercise2.c
	@echo "$(CYAN)Compilare$(NC) exercise2.c..."
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ exercise2 compilat$(NC)"

# =============================================================================
# COMPILARE SOLUȚII
# =============================================================================

solutions: $(SOLUTIONS)
	@echo ""
	@echo "$(GREEN)✓ Toate soluțiile au fost compilate$(NC)"

exercise1_sol: $(SOL_DIR)/exercise1_sol.c
	@echo "$(CYAN)Compilare$(NC) exercise1_sol.c..."
	@$(CC) $(CFLAGS) -o $@ $<
	@echo "$(GREEN)  ✓ exercise1_sol compilat$(NC)"

exercise2_sol: $(SOL_DIR)/exercise2_sol.c
	@echo "$(CYAN)Compilare$(NC) exercise2_sol.c..."
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ exercise2_sol compilat$(NC)"

homework1_sol: $(SOL_DIR)/homework1_sol.c
	@echo "$(CYAN)Compilare$(NC) homework1_sol.c..."
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ homework1_sol compilat$(NC)"

homework2_sol: $(SOL_DIR)/homework2_sol.c
	@echo "$(CYAN)Compilare$(NC) homework2_sol.c..."
	@$(CC) $(CFLAGS) -o $@ $<
	@echo "$(GREEN)  ✓ homework2_sol compilat$(NC)"

# =============================================================================
# RULARE
# =============================================================================

run: example1
	@echo ""
	@echo "$(MAGENTA)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(MAGENTA)                    RULARE EXEMPLU COMPLET                      $(NC)"
	@echo "$(MAGENTA)═══════════════════════════════════════════════════════════════$(NC)"
	@./example1

run-ex1: exercise1
	@echo ""
	@echo "$(YELLOW)Rulare exercițiul 1...$(NC)"
	@./exercise1

run-ex2: exercise2
	@echo ""
	@echo "$(YELLOW)Rulare exercițiul 2...$(NC)"
	@./exercise2

# =============================================================================
# TESTARE
# =============================================================================

test: test-ex1 test-ex2
	@echo ""
	@echo "$(GREEN)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(GREEN)              TOATE TESTELE AU FOST RULATE                      $(NC)"
	@echo "$(GREEN)═══════════════════════════════════════════════════════════════$(NC)"

test-ex1: exercise1_sol
	@echo ""
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(YELLOW)           TESTARE EXERCIȚIUL 1: Verificare Paranteze          $(NC)"
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@echo "$(CYAN)Rulare teste pentru verificarea parantezelor...$(NC)"
	@./exercise1_sol < $(TEST_DIR)/test1_input.txt > /tmp/test1_output.txt 2>&1 || true
	@if diff -q /tmp/test1_output.txt $(TEST_DIR)/test1_expected.txt > /dev/null 2>&1; then \
		echo "$(GREEN)  ✓ Toate testele au trecut!$(NC)"; \
	else \
		echo "$(RED)  ✗ Unele teste au eșuat.$(NC)"; \
		echo "$(YELLOW)  Diferențe:$(NC)"; \
		diff /tmp/test1_output.txt $(TEST_DIR)/test1_expected.txt || true; \
	fi

test-ex2: exercise2_sol
	@echo ""
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(YELLOW)        TESTARE EXERCIȚIUL 2: Conversie Infix→Postfix          $(NC)"
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@echo "$(CYAN)Rulare teste pentru conversia expresiilor...$(NC)"
	@./exercise2_sol < $(TEST_DIR)/test2_input.txt > /tmp/test2_output.txt 2>&1 || true
	@echo "$(GREEN)  ✓ Testele au fost rulate. Verifică output-ul manual.$(NC)"

# =============================================================================
# VERIFICARE MEMORIE
# =============================================================================

valgrind: example1
	@echo ""
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(YELLOW)              VERIFICARE MEMORIE CU VALGRIND                   $(NC)"
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@if command -v valgrind > /dev/null 2>&1; then \
		valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./example1; \
	else \
		echo "$(RED)Eroare: Valgrind nu este instalat.$(NC)"; \
		echo "$(YELLOW)Instalează cu: sudo apt install valgrind$(NC)"; \
	fi

valgrind-ex1: exercise1_sol
	@echo "$(YELLOW)Verificare memorie pentru exercițiul 1...$(NC)"
	@echo "test" | valgrind --leak-check=full ./exercise1_sol

valgrind-ex2: exercise2_sol
	@echo "$(YELLOW)Verificare memorie pentru exercițiul 2...$(NC)"
	@echo "3+4" | valgrind --leak-check=full ./exercise2_sol

# =============================================================================
# CURĂȚARE
# =============================================================================

clean:
	@echo "$(YELLOW)Curățare fișiere compilate...$(NC)"
	@rm -f $(TARGETS) $(SOLUTIONS)
	@rm -f *.o
	@rm -f /tmp/test*_output.txt
	@echo "$(GREEN)  ✓ Curățare completă$(NC)"

# =============================================================================
# AJUTOR
# =============================================================================

help:
	@echo ""
	@echo "$(CYAN)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(CYAN)║            SĂPTĂMÂNA 5: STIVE - COMENZI DISPONIBILE           ║$(NC)"
	@echo "$(CYAN)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(YELLOW)Compilare:$(NC)"
	@echo "  $(GREEN)make$(NC)            - Compilează toate exercițiile"
	@echo "  $(GREEN)make solutions$(NC)  - Compilează soluțiile (instructor)"
	@echo "  $(GREEN)make example1$(NC)   - Compilează doar exemplul"
	@echo "  $(GREEN)make exercise1$(NC)  - Compilează doar exercițiul 1"
	@echo "  $(GREEN)make exercise2$(NC)  - Compilează doar exercițiul 2"
	@echo ""
	@echo "$(YELLOW)Rulare:$(NC)"
	@echo "  $(GREEN)make run$(NC)        - Rulează exemplul complet"
	@echo "  $(GREEN)make run-ex1$(NC)    - Rulează exercițiul 1"
	@echo "  $(GREEN)make run-ex2$(NC)    - Rulează exercițiul 2"
	@echo ""
	@echo "$(YELLOW)Testare:$(NC)"
	@echo "  $(GREEN)make test$(NC)       - Rulează toate testele"
	@echo "  $(GREEN)make test-ex1$(NC)   - Testează exercițiul 1"
	@echo "  $(GREEN)make test-ex2$(NC)   - Testează exercițiul 2"
	@echo ""
	@echo "$(YELLOW)Debugging:$(NC)"
	@echo "  $(GREEN)make valgrind$(NC)   - Verifică memory leaks"
	@echo ""
	@echo "$(YELLOW)Altele:$(NC)"
	@echo "  $(GREEN)make clean$(NC)      - Șterge fișierele compilate"
	@echo "  $(GREEN)make help$(NC)       - Afișează acest mesaj"
	@echo ""

# =============================================================================
# INFORMAȚII SUPLIMENTARE
# =============================================================================

info:
	@echo ""
	@echo "$(CYAN)Informații despre proiect:$(NC)"
	@echo "  Curs: Algoritmi și Tehnici de Programare"
	@echo "  Săptămâna: 5 - Stive (Stack)"
	@echo "  Compilator: $(CC)"
	@echo "  Flaguri: $(CFLAGS)"
	@echo ""
