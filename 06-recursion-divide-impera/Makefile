# =============================================================================
# SĂPTĂMÂNA 6: COZI (QUEUES) - Makefile
# Algoritmi și Tehnici de Programare
# =============================================================================

# Compiler și flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
LDFLAGS =

# Directoare
SRC_DIR = src
DATA_DIR = data
TEST_DIR = tests
SOL_DIR = solution

# Culori pentru output
GREEN = \033[0;32m
RED = \033[0;31m
YELLOW = \033[0;33m
CYAN = \033[0;36m
MAGENTA = \033[0;35m
NC = \033[0m

# Executabile
TARGETS = example1 exercise1 exercise2
SOLUTIONS = exercise1_sol exercise2_sol homework1_sol homework2_sol

# Fișiere sursă
EXAMPLE1_SRC = $(SRC_DIR)/example1.c
EXERCISE1_SRC = $(SRC_DIR)/exercise1.c
EXERCISE2_SRC = $(SRC_DIR)/exercise2.c

# =============================================================================
# TARGETS PRINCIPALE
# =============================================================================

.PHONY: all clean run test valgrind help solutions demo

# Target implicit - compilează toate exercițiile
all: $(TARGETS)
	@echo "$(GREEN)✓ Toate target-urile compilate cu succes!$(NC)"
	@echo ""

# Compilare exemple
example1: $(EXAMPLE1_SRC)
	@echo "$(CYAN)Compilare example1...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)✓ example1 compilat$(NC)"

# Compilare exerciții
exercise1: $(EXERCISE1_SRC)
	@echo "$(CYAN)Compilare exercise1...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)✓ exercise1 compilat$(NC)"

exercise2: $(EXERCISE2_SRC)
	@echo "$(CYAN)Compilare exercise2...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)✓ exercise2 compilat$(NC)"

# =============================================================================
# RULARE
# =============================================================================

# Rulare exemplul demonstrativ
run: example1
	@echo ""
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(YELLOW)  Rulare Example1 - Demonstrație completă Queues$(NC)"
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@./example1

# Rulare exercițiul 1 cu date de test
run-ex1: exercise1
	@echo ""
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(YELLOW)  Rulare Exercise1 - Sistem Comenzi Restaurant$(NC)"
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@./exercise1 $(DATA_DIR)/orders.txt

# Rulare exercițiul 2 cu date de test
run-ex2: exercise2
	@echo ""
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(YELLOW)  Rulare Exercise2 - Task Scheduler$(NC)"
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@./exercise2 $(DATA_DIR)/tasks.txt

# Rulare toate demonstrațiile
demo: all run run-ex1 run-ex2
	@echo ""
	@echo "$(GREEN)✓ Toate demonstrațiile complete!$(NC)"

# =============================================================================
# TESTARE AUTOMATĂ
# =============================================================================

test: test1 test2
	@echo ""
	@echo "$(GREEN)✓ Toate testele complete!$(NC)"

test1: exercise1
	@echo ""
	@echo "$(CYAN)═══ Test Exercise 1 ═══$(NC)"
	@echo "$(YELLOW)Input:$(NC)"
	@cat $(TEST_DIR)/test1_input.txt
	@echo ""
	@echo "$(YELLOW)Output:$(NC)"
	@./exercise1 < $(TEST_DIR)/test1_input.txt 2>/dev/null | grep -E "^\[" || true
	@echo ""
	@echo "$(YELLOW)Expected:$(NC)"
	@cat $(TEST_DIR)/test1_expected.txt
	@echo ""

test2: exercise2
	@echo ""
	@echo "$(CYAN)═══ Test Exercise 2 ═══$(NC)"
	@echo "$(YELLOW)Input:$(NC)"
	@cat $(TEST_DIR)/test2_input.txt
	@echo ""
	@echo "$(YELLOW)Output:$(NC)"
	@./exercise2 < $(TEST_DIR)/test2_input.txt 2>/dev/null | grep -E "^\[|^→|^  \[" || true
	@echo ""
	@echo "$(YELLOW)Expected:$(NC)"
	@cat $(TEST_DIR)/test2_expected.txt
	@echo ""

# =============================================================================
# VALGRIND - VERIFICARE MEMORIE
# =============================================================================

valgrind: all
	@echo ""
	@echo "$(MAGENTA)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(MAGENTA)  Verificare memorie cu Valgrind$(NC)"
	@echo "$(MAGENTA)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@echo "$(CYAN)→ Verificare example1:$(NC)"
	@valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 \
		./example1 2>&1 | tail -20
	@echo ""
	@echo "$(GREEN)✓ Verificare memorie completă$(NC)"

valgrind-ex1: exercise1
	@echo "$(CYAN)→ Verificare exercise1:$(NC)"
	@echo "ADD Test Pizza\nPROCESS\nEXIT" | valgrind --leak-check=full ./exercise1 2>&1 | tail -15

valgrind-ex2: exercise2
	@echo "$(CYAN)→ Verificare exercise2:$(NC)"
	@echo "SCHEDULE NORMAL Test\nEXECUTE\nEXIT" | valgrind --leak-check=full ./exercise2 2>&1 | tail -15

# =============================================================================
# SOLUȚII (pentru instructori)
# =============================================================================

solutions:
	@echo "$(CYAN)Compilare soluții...$(NC)"
	@if [ -f $(SOL_DIR)/exercise1_sol.c ]; then \
		$(CC) $(CFLAGS) -o exercise1_sol $(SOL_DIR)/exercise1_sol.c; \
		echo "$(GREEN)✓ exercise1_sol compilat$(NC)"; \
	fi
	@if [ -f $(SOL_DIR)/exercise2_sol.c ]; then \
		$(CC) $(CFLAGS) -o exercise2_sol $(SOL_DIR)/exercise2_sol.c; \
		echo "$(GREEN)✓ exercise2_sol compilat$(NC)"; \
	fi
	@if [ -f $(SOL_DIR)/homework1_sol.c ]; then \
		$(CC) $(CFLAGS) -o homework1_sol $(SOL_DIR)/homework1_sol.c; \
		echo "$(GREEN)✓ homework1_sol compilat$(NC)"; \
	fi
	@if [ -f $(SOL_DIR)/homework2_sol.c ]; then \
		$(CC) $(CFLAGS) -o homework2_sol $(SOL_DIR)/homework2_sol.c; \
		echo "$(GREEN)✓ homework2_sol compilat$(NC)"; \
	fi

# =============================================================================
# CURĂȚARE
# =============================================================================

clean:
	@echo "$(YELLOW)Curățare fișiere generate...$(NC)"
	@rm -f $(TARGETS) $(SOLUTIONS) *.o
	@rm -f *.exe  # Pentru Windows/MinGW
	@rm -rf *.dSYM  # Pentru macOS
	@echo "$(GREEN)✓ Curățare completă$(NC)"

# =============================================================================
# AJUTOR
# =============================================================================

help:
	@echo ""
	@echo "$(CYAN)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(CYAN)  SĂPTĂMÂNA 6: COZI (QUEUES) - Makefile Help$(NC)"
	@echo "$(CYAN)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@echo "$(YELLOW)Target-uri disponibile:$(NC)"
	@echo ""
	@echo "  $(GREEN)make$(NC)            - Compilează toate exercițiile"
	@echo "  $(GREEN)make all$(NC)        - La fel ca 'make'"
	@echo ""
	@echo "  $(GREEN)make run$(NC)        - Rulează exemplul demonstrativ"
	@echo "  $(GREEN)make run-ex1$(NC)    - Rulează exercițiul 1 cu date test"
	@echo "  $(GREEN)make run-ex2$(NC)    - Rulează exercițiul 2 cu date test"
	@echo "  $(GREEN)make demo$(NC)       - Rulează toate demonstrațiile"
	@echo ""
	@echo "  $(GREEN)make test$(NC)       - Rulează testele automate"
	@echo "  $(GREEN)make test1$(NC)      - Testează exercițiul 1"
	@echo "  $(GREEN)make test2$(NC)      - Testează exercițiul 2"
	@echo ""
	@echo "  $(GREEN)make valgrind$(NC)   - Verifică memory leaks"
	@echo ""
	@echo "  $(GREEN)make solutions$(NC)  - Compilează soluțiile (instructori)"
	@echo ""
	@echo "  $(GREEN)make clean$(NC)      - Șterge fișierele generate"
	@echo "  $(GREEN)make help$(NC)       - Afișează acest mesaj"
	@echo ""
	@echo "$(CYAN)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""

# =============================================================================
# TARGETS INDIVIDUALE PENTRU DEPANARE
# =============================================================================

# Afișare variabile (pentru debugging Makefile)
debug-vars:
	@echo "CC = $(CC)"
	@echo "CFLAGS = $(CFLAGS)"
	@echo "TARGETS = $(TARGETS)"
	@echo "SRC_DIR = $(SRC_DIR)"

# Verificare sintaxă (compilare fără linking)
check-syntax:
	@echo "$(CYAN)Verificare sintaxă...$(NC)"
	@$(CC) $(CFLAGS) -fsyntax-only $(SRC_DIR)/*.c
	@echo "$(GREEN)✓ Sintaxă corectă$(NC)"
