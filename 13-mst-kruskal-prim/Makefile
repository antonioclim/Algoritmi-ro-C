# =============================================================================
# SĂPTĂMÂNA 13: ALGORITMI PE GRAFURI - DRUMURI MINIME
# Makefile pentru compilare, testare și rulare
# =============================================================================

# Compilator și flag-uri
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -O2
LDFLAGS = -lm

# Directoare
SRC_DIR = src
DATA_DIR = data
TEST_DIR = tests
SOL_DIR = solution

# Culori ANSI pentru output
GREEN = \033[0;32m
RED = \033[0;31m
YELLOW = \033[0;33m
CYAN = \033[0;36m
MAGENTA = \033[0;35m
BLUE = \033[0;34m
BOLD = \033[1m
NC = \033[0m

# Executabile
TARGETS = example1 exercise1 exercise2
SOLUTIONS = exercise1_sol exercise2_sol homework1_sol homework2_sol

# =============================================================================
# REGULI PRINCIPALE
# =============================================================================

.PHONY: all clean run test valgrind help solutions demo

all: $(TARGETS)
	@echo ""
	@echo "$(GREEN)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(GREEN)║  ✓ Toate targeturile au fost compilate cu succes!             ║$(NC)"
	@echo "$(GREEN)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(CYAN)Executabile disponibile:$(NC)"
	@echo "  $(YELLOW)./example1$(NC)   - Exemplu complet Dijkstra & Bellman-Ford"
	@echo "  $(YELLOW)./exercise1$(NC)  - Exercițiu: Dijkstra cu Priority Queue"
	@echo "  $(YELLOW)./exercise2$(NC)  - Exercițiu: Bellman-Ford cu detecție cicluri"
	@echo ""

# =============================================================================
# COMPILARE EXECUTABILE PRINCIPALE
# =============================================================================

example1: $(SRC_DIR)/example1.c
	@echo "$(CYAN)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(CYAN)│  Compilare: example1.c                                          │$(NC)"
	@echo "$(CYAN)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ example1 compilat cu succes$(NC)"

exercise1: $(SRC_DIR)/exercise1.c
	@echo "$(CYAN)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(CYAN)│  Compilare: exercise1.c                                         │$(NC)"
	@echo "$(CYAN)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ exercise1 compilat cu succes$(NC)"

exercise2: $(SRC_DIR)/exercise2.c
	@echo "$(CYAN)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(CYAN)│  Compilare: exercise2.c                                         │$(NC)"
	@echo "$(CYAN)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ exercise2 compilat cu succes$(NC)"

# =============================================================================
# COMPILARE SOLUȚII (pentru instructor)
# =============================================================================

solutions: $(SOLUTIONS)
	@echo ""
	@echo "$(MAGENTA)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(MAGENTA)║  ✓ Toate soluțiile au fost compilate!                         ║$(NC)"
	@echo "$(MAGENTA)╚═══════════════════════════════════════════════════════════════╝$(NC)"

exercise1_sol: $(SOL_DIR)/exercise1_sol.c
	@echo "$(MAGENTA)  Compilare soluție: exercise1_sol.c$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ exercise1_sol compilat$(NC)"

exercise2_sol: $(SOL_DIR)/exercise2_sol.c
	@echo "$(MAGENTA)  Compilare soluție: exercise2_sol.c$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ exercise2_sol compilat$(NC)"

homework1_sol: $(SOL_DIR)/homework1_sol.c
	@echo "$(MAGENTA)  Compilare soluție: homework1_sol.c$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ homework1_sol compilat$(NC)"

homework2_sol: $(SOL_DIR)/homework2_sol.c
	@echo "$(MAGENTA)  Compilare soluție: homework2_sol.c$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ homework2_sol compilat$(NC)"

# =============================================================================
# EXECUȚIE
# =============================================================================

run: example1
	@echo ""
	@echo "$(YELLOW)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(YELLOW)║  Rulare exemplu demonstrativ                                  ║$(NC)"
	@echo "$(YELLOW)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@./example1

run-ex1: exercise1
	@echo ""
	@echo "$(YELLOW)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(YELLOW)║  Rulare Exercise 1: Dijkstra                                  ║$(NC)"
	@echo "$(YELLOW)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@./exercise1 < $(DATA_DIR)/graph1.txt

run-ex2: exercise2
	@echo ""
	@echo "$(YELLOW)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(YELLOW)║  Rulare Exercise 2: Bellman-Ford                              ║$(NC)"
	@echo "$(YELLOW)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@./exercise2 < $(DATA_DIR)/graph_negative.txt

demo: example1
	@echo ""
	@echo "$(BOLD)$(BLUE)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(BOLD)$(BLUE)║       DEMONSTRAȚIE COMPLETĂ - SĂPTĂMÂNA 13                    ║$(NC)"
	@echo "$(BOLD)$(BLUE)║       Algoritmi pe Grafuri: Dijkstra & Bellman-Ford           ║$(NC)"
	@echo "$(BOLD)$(BLUE)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@./example1
	@echo ""
	@echo "$(GREEN)═══════════════════════════════════════════════════════════════════$(NC)"
	@echo "$(GREEN)  Demonstrație completă! Verifică output-ul de mai sus.            $(NC)"
	@echo "$(GREEN)═══════════════════════════════════════════════════════════════════$(NC)"

# =============================================================================
# TESTARE AUTOMATĂ
# =============================================================================

test: exercise1 exercise2
	@echo ""
	@echo "$(YELLOW)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(YELLOW)║  Rulare teste automate                                        ║$(NC)"
	@echo "$(YELLOW)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(CYAN)Test 1: Dijkstra cu graf standard...$(NC)"
	@./exercise1 < $(TEST_DIR)/test1_input.txt > /tmp/test1_output.txt 2>&1 || true
	@if diff -q /tmp/test1_output.txt $(TEST_DIR)/test1_expected.txt > /dev/null 2>&1; then \
		echo "$(GREEN)  ✓ Test 1 PASSED$(NC)"; \
	else \
		echo "$(RED)  ✗ Test 1 FAILED$(NC)"; \
		echo "$(YELLOW)    Output primit:$(NC)"; \
		cat /tmp/test1_output.txt | head -10; \
		echo "$(YELLOW)    Output așteptat:$(NC)"; \
		cat $(TEST_DIR)/test1_expected.txt | head -10; \
	fi
	@echo ""
	@echo "$(CYAN)Test 2: Bellman-Ford cu detectare ciclu negativ...$(NC)"
	@./exercise2 < $(TEST_DIR)/test2_input.txt > /tmp/test2_output.txt 2>&1 || true
	@if diff -q /tmp/test2_output.txt $(TEST_DIR)/test2_expected.txt > /dev/null 2>&1; then \
		echo "$(GREEN)  ✓ Test 2 PASSED$(NC)"; \
	else \
		echo "$(RED)  ✗ Test 2 FAILED$(NC)"; \
		echo "$(YELLOW)    Output primit:$(NC)"; \
		cat /tmp/test2_output.txt | head -10; \
		echo "$(YELLOW)    Output așteptat:$(NC)"; \
		cat $(TEST_DIR)/test2_expected.txt | head -10; \
	fi
	@echo ""
	@rm -f /tmp/test1_output.txt /tmp/test2_output.txt

test-solutions: solutions
	@echo ""
	@echo "$(MAGENTA)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(MAGENTA)║  Testare soluții oficiale                                     ║$(NC)"
	@echo "$(MAGENTA)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(CYAN)Test soluție exercițiu 1...$(NC)"
	@./exercise1_sol < $(TEST_DIR)/test1_input.txt > /tmp/sol1_output.txt 2>&1
	@if diff -q /tmp/sol1_output.txt $(TEST_DIR)/test1_expected.txt > /dev/null 2>&1; then \
		echo "$(GREEN)  ✓ Soluția 1 corectă$(NC)"; \
	else \
		echo "$(RED)  ✗ Soluția 1 incorectă$(NC)"; \
	fi
	@echo ""
	@echo "$(CYAN)Test soluție exercițiu 2...$(NC)"
	@./exercise2_sol < $(TEST_DIR)/test2_input.txt > /tmp/sol2_output.txt 2>&1
	@if diff -q /tmp/sol2_output.txt $(TEST_DIR)/test2_expected.txt > /dev/null 2>&1; then \
		echo "$(GREEN)  ✓ Soluția 2 corectă$(NC)"; \
	else \
		echo "$(RED)  ✗ Soluția 2 incorectă$(NC)"; \
	fi
	@rm -f /tmp/sol1_output.txt /tmp/sol2_output.txt

# =============================================================================
# VERIFICARE MEMORIE CU VALGRIND
# =============================================================================

valgrind: example1
	@echo ""
	@echo "$(YELLOW)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(YELLOW)║  Verificare memory leaks cu Valgrind                          ║$(NC)"
	@echo "$(YELLOW)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./example1 2>&1 | tail -20
	@echo ""

valgrind-ex1: exercise1
	@echo "$(YELLOW)Verificare memorie pentru exercise1...$(NC)"
	@valgrind --leak-check=full ./exercise1 < $(DATA_DIR)/graph1.txt 2>&1 | tail -15

valgrind-ex2: exercise2
	@echo "$(YELLOW)Verificare memorie pentru exercise2...$(NC)"
	@valgrind --leak-check=full ./exercise2 < $(DATA_DIR)/graph_negative.txt 2>&1 | tail -15

# =============================================================================
# DEBUGGING
# =============================================================================

debug-ex1: exercise1
	@echo "$(CYAN)Pornire GDB pentru exercise1...$(NC)"
	@gdb ./exercise1

debug-ex2: exercise2
	@echo "$(CYAN)Pornire GDB pentru exercise2...$(NC)"
	@gdb ./exercise2

# =============================================================================
# CURĂȚARE
# =============================================================================

clean:
	@echo ""
	@echo "$(YELLOW)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(YELLOW)║  Curățare fișiere generate                                    ║$(NC)"
	@echo "$(YELLOW)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@rm -f $(TARGETS) $(SOLUTIONS)
	@rm -f *.o *.out
	@rm -f /tmp/test*_output.txt /tmp/sol*_output.txt
	@echo "$(GREEN)  ✓ Curățare completă$(NC)"
	@echo ""

# =============================================================================
# AJUTOR
# =============================================================================

help:
	@echo ""
	@echo "$(BOLD)$(BLUE)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(BOLD)$(BLUE)║     SĂPTĂMÂNA 13: ALGORITMI PE GRAFURI - MAKEFILE HELP        ║$(NC)"
	@echo "$(BOLD)$(BLUE)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(CYAN)Comenzi de compilare:$(NC)"
	@echo "  $(YELLOW)make$(NC)           - Compilează toate executabilele principale"
	@echo "  $(YELLOW)make example1$(NC)  - Compilează doar exemplul demonstrativ"
	@echo "  $(YELLOW)make exercise1$(NC) - Compilează exercițiul 1 (Dijkstra)"
	@echo "  $(YELLOW)make exercise2$(NC) - Compilează exercițiul 2 (Bellman-Ford)"
	@echo "  $(YELLOW)make solutions$(NC) - Compilează soluțiile (instructor)"
	@echo ""
	@echo "$(CYAN)Comenzi de execuție:$(NC)"
	@echo "  $(YELLOW)make run$(NC)       - Rulează exemplul demonstrativ"
	@echo "  $(YELLOW)make run-ex1$(NC)   - Rulează exercițiul 1 cu date de test"
	@echo "  $(YELLOW)make run-ex2$(NC)   - Rulează exercițiul 2 cu date de test"
	@echo "  $(YELLOW)make demo$(NC)      - Demonstrație completă"
	@echo ""
	@echo "$(CYAN)Comenzi de testare:$(NC)"
	@echo "  $(YELLOW)make test$(NC)      - Rulează testele automate"
	@echo "  $(YELLOW)make test-solutions$(NC) - Testează soluțiile oficiale"
	@echo ""
	@echo "$(CYAN)Comenzi de debugging:$(NC)"
	@echo "  $(YELLOW)make valgrind$(NC) - Verifică memory leaks (example1)"
	@echo "  $(YELLOW)make valgrind-ex1$(NC) - Verifică memory leaks (exercise1)"
	@echo "  $(YELLOW)make valgrind-ex2$(NC) - Verifică memory leaks (exercise2)"
	@echo "  $(YELLOW)make debug-ex1$(NC) - Pornește GDB pentru exercise1"
	@echo "  $(YELLOW)make debug-ex2$(NC) - Pornește GDB pentru exercise2"
	@echo ""
	@echo "$(CYAN)Alte comenzi:$(NC)"
	@echo "  $(YELLOW)make clean$(NC)     - Șterge fișierele generate"
	@echo "  $(YELLOW)make help$(NC)      - Afișează acest mesaj"
	@echo ""
	@echo "$(CYAN)Structura proiectului:$(NC)"
	@echo "  src/       - Fișiere sursă (.c)"
	@echo "  data/      - Date de intrare pentru teste"
	@echo "  tests/     - Teste automate (input + expected)"
	@echo "  solution/  - Soluții complete (instructor)"
	@echo "  slides/    - Prezentări HTML interactive"
	@echo "  teme/      - Cerințe teme și provocări"
	@echo ""
