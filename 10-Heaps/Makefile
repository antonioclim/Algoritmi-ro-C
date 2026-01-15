# =============================================================================
# SĂPTĂMÂNA 10: HEAP-URI ȘI COZI CU PRIORITĂȚI
# Makefile pentru compilare și testare
# =============================================================================

# Compilator și flag-uri
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -O2
LDFLAGS = 

# Directoare
SRC_DIR = src
SOL_DIR = solution
TEST_DIR = tests
DATA_DIR = data

# Culori pentru output
GREEN = \033[0;32m
RED = \033[0;31m
YELLOW = \033[0;33m
CYAN = \033[0;36m
PURPLE = \033[0;35m
NC = \033[0m

# Executabile
TARGETS = example1 exercise1 exercise2
SOLUTIONS = exercise1_sol exercise2_sol homework1_sol homework2_sol

# =============================================================================
# REGULI PRINCIPALE
# =============================================================================

.PHONY: all clean run test valgrind help solutions demo

all: $(TARGETS)
	@echo "$(GREEN)✓ Toate executabilele au fost compilate cu succes!$(NC)"

solutions: $(SOLUTIONS)
	@echo "$(GREEN)✓ Toate soluțiile au fost compilate cu succes!$(NC)"

# =============================================================================
# COMPILARE EXERCIȚII
# =============================================================================

example1: $(SRC_DIR)/example1.c
	@echo "$(CYAN)Compilare example1...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ example1 compilat$(NC)"

exercise1: $(SRC_DIR)/exercise1.c
	@echo "$(CYAN)Compilare exercise1...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) 2>/dev/null || echo "$(YELLOW)  ⚠ exercise1 are TODO-uri necompletate$(NC)"

exercise2: $(SRC_DIR)/exercise2.c
	@echo "$(CYAN)Compilare exercise2...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) 2>/dev/null || echo "$(YELLOW)  ⚠ exercise2 are TODO-uri necompletate$(NC)"

# =============================================================================
# COMPILARE SOLUȚII
# =============================================================================

exercise1_sol: $(SOL_DIR)/exercise1_sol.c
	@echo "$(PURPLE)Compilare soluție exercise1...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ exercise1_sol compilat$(NC)"

exercise2_sol: $(SOL_DIR)/exercise2_sol.c
	@echo "$(PURPLE)Compilare soluție exercise2...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ exercise2_sol compilat$(NC)"

homework1_sol: $(SOL_DIR)/homework1_sol.c
	@echo "$(PURPLE)Compilare soluție homework1...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ homework1_sol compilat$(NC)"

homework2_sol: $(SOL_DIR)/homework2_sol.c
	@echo "$(PURPLE)Compilare soluție homework2...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ homework2_sol compilat$(NC)"

# =============================================================================
# RULARE
# =============================================================================

run: example1
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(YELLOW)           Rulare Exemplu Demonstrativ                          $(NC)"
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@./example1

run-ex1: exercise1_sol
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(YELLOW)           Rulare Exercițiul 1 (cu date de test)                $(NC)"
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@./exercise1_sol < $(TEST_DIR)/test1_input.txt

run-ex2: exercise2_sol
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(YELLOW)           Rulare Exercițiul 2 (cu date de test)                $(NC)"
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@./exercise2_sol < $(TEST_DIR)/test2_input.txt

demo: example1 solutions
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(YELLOW)           DEMONSTRAȚIE COMPLETĂ                                $(NC)"
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@echo "$(CYAN)>>> Exemplu demonstrativ <<<$(NC)"
	@./example1

# =============================================================================
# TESTARE
# =============================================================================

test: test-ex1 test-ex2
	@echo "$(GREEN)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(GREEN)           Toate testele finalizate!                            $(NC)"
	@echo "$(GREEN)═══════════════════════════════════════════════════════════════$(NC)"

test-ex1: exercise1_sol
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(YELLOW)           Testare Exercițiul 1 (Priority Queue)               $(NC)"
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@./exercise1_sol < $(TEST_DIR)/test1_input.txt > /tmp/test1_output.txt 2>&1
	@if grep -q "50 40 30 20 10" /tmp/test1_output.txt; then \
		echo "$(GREEN)  ✓ Test Priority Queue PASSED$(NC)"; \
	else \
		echo "$(RED)  ✗ Test Priority Queue FAILED$(NC)"; \
	fi

test-ex2: exercise2_sol
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(YELLOW)           Testare Exercițiul 2 (Heap Sort)                    $(NC)"
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@./exercise2_sol < $(TEST_DIR)/test2_input.txt > /tmp/test2_output.txt 2>&1
	@if grep -q "11, 12, 22, 25, 34, 64, 90" /tmp/test2_output.txt; then \
		echo "$(GREEN)  ✓ Test Heap Sort PASSED$(NC)"; \
	else \
		echo "$(RED)  ✗ Test Heap Sort FAILED$(NC)"; \
	fi

# =============================================================================
# VERIFICARE MEMORIE
# =============================================================================

valgrind: example1
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(YELLOW)           Verificare Memory Leaks cu Valgrind                 $(NC)"
	@echo "$(YELLOW)═══════════════════════════════════════════════════════════════$(NC)"
	@valgrind --leak-check=full --show-leak-kinds=all ./example1 2>&1 | tail -20

valgrind-sol1: exercise1_sol
	@echo "$(YELLOW)Verificare memory leaks pentru soluția exercise1...$(NC)"
	@valgrind --leak-check=full ./exercise1_sol < $(TEST_DIR)/test1_input.txt 2>&1 | tail -15

valgrind-sol2: exercise2_sol
	@echo "$(YELLOW)Verificare memory leaks pentru soluția exercise2...$(NC)"
	@valgrind --leak-check=full ./exercise2_sol < $(TEST_DIR)/test2_input.txt 2>&1 | tail -15

# =============================================================================
# CURĂȚARE
# =============================================================================

clean:
	@echo "$(YELLOW)Curățare artefacte...$(NC)"
	@rm -f $(TARGETS) $(SOLUTIONS)
	@rm -f *.o
	@rm -f /tmp/test*_output.txt
	@echo "$(GREEN)  ✓ Curățare completă$(NC)"

# =============================================================================
# AJUTOR
# =============================================================================

help:
	@echo "$(CYAN)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(CYAN)    SĂPTĂMÂNA 10: HEAP-URI ȘI COZI CU PRIORITĂȚI               $(NC)"
	@echo "$(CYAN)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@echo "$(YELLOW)Comenzi disponibile:$(NC)"
	@echo ""
	@echo "  $(GREEN)make$(NC)            - Compilează toate exercițiile"
	@echo "  $(GREEN)make solutions$(NC)  - Compilează toate soluțiile"
	@echo "  $(GREEN)make run$(NC)        - Rulează exemplul demonstrativ"
	@echo "  $(GREEN)make run-ex1$(NC)    - Rulează soluția exercițiului 1"
	@echo "  $(GREEN)make run-ex2$(NC)    - Rulează soluția exercițiului 2"
	@echo "  $(GREEN)make demo$(NC)       - Rulează demonstrație completă"
	@echo "  $(GREEN)make test$(NC)       - Testează soluțiile"
	@echo "  $(GREEN)make valgrind$(NC)   - Verifică memory leaks"
	@echo "  $(GREEN)make clean$(NC)      - Șterge fișierele compilate"
	@echo "  $(GREEN)make help$(NC)       - Afișează acest mesaj"
	@echo ""
	@echo "$(CYAN)Fișiere:$(NC)"
	@echo "  src/example1.c  - Exemplu complet funcțional"
	@echo "  src/exercise1.c - Exercițiu Priority Queue (TODO)"
	@echo "  src/exercise2.c - Exercițiu Heap Sort (TODO)"
	@echo ""

# =============================================================================
# SFÂRȘIT MAKEFILE
# =============================================================================
