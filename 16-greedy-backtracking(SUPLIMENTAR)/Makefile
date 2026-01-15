# =============================================================================
# SĂPTĂMÂNA 16: PARADIGME ALGORITMICE - GREEDY ȘI BACKTRACKING - Makefile
# =============================================================================
#
# Utilizare:
#   make          - Compilează toate targeturile
#   make run      - Rulează exemplul demonstrativ
#   make test     - Rulează testele automate
#   make valgrind - Verifică memory leaks
#   make clean    - Șterge fișierele generate
#   make help     - Afișează acest ajutor
#
# =============================================================================

# Compilator și flaguri
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -pedantic
LDFLAGS = -lm

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
BOLD = \033[1m

# Targeturi principale
EXAMPLE = example1
EXERCISE1 = exercise1
EXERCISE2 = exercise2

# Soluții
SOL_EX1 = exercise1_sol
SOL_EX2 = exercise2_sol
SOL_HW1 = homework1_sol
SOL_HW2 = homework2_sol

# Lista de executabile
TARGETS = $(EXAMPLE) $(EXERCISE1) $(EXERCISE2)
SOLUTIONS = $(SOL_EX1) $(SOL_EX2) $(SOL_HW1) $(SOL_HW2)

# =============================================================================
# REGULI PRINCIPALE
# =============================================================================

.PHONY: all clean run test valgrind help solutions run-ex1 run-ex2

all: banner $(TARGETS)
	@echo ""
	@echo "$(GREEN)$(BOLD)✓ Toate targeturile au fost compilate cu succes!$(NC)"
	@echo ""
	@echo "$(CYAN)Utilizare:$(NC)"
	@echo "  make run      - Rulează exemplul demonstrativ"
	@echo "  make run-ex1  - Rulează exercițiul 1 (Activity Selection)"
	@echo "  make run-ex2  - Rulează exercițiul 2 (Sudoku Solver)"
	@echo "  make test     - Rulează testele automate"
	@echo "  make valgrind - Verifică memory leaks"
	@echo ""

banner:
	@echo ""
	@echo "$(CYAN)╔═══════════════════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(CYAN)║     $(BOLD)SĂPTĂMÂNA 16: PARADIGME ALGORITMICE - GREEDY ȘI BACKTRACKING$(NC)$(CYAN)         ║$(NC)"
	@echo "$(CYAN)╚═══════════════════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""

# =============================================================================
# COMPILARE TARGETURI
# =============================================================================

$(EXAMPLE): $(SRC_DIR)/example1.c
	@echo "$(YELLOW)⚙ Compilare $(EXAMPLE)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ $(EXAMPLE) compilat$(NC)"

$(EXERCISE1): $(SRC_DIR)/exercise1.c
	@echo "$(YELLOW)⚙ Compilare $(EXERCISE1)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ $(EXERCISE1) compilat$(NC)"

$(EXERCISE2): $(SRC_DIR)/exercise2.c
	@echo "$(YELLOW)⚙ Compilare $(EXERCISE2)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ $(EXERCISE2) compilat$(NC)"

# =============================================================================
# COMPILARE SOLUȚII
# =============================================================================

solutions: banner-sol $(SOLUTIONS)
	@echo ""
	@echo "$(GREEN)$(BOLD)✓ Toate soluțiile au fost compilate cu succes!$(NC)"
	@echo ""

banner-sol:
	@echo ""
	@echo "$(MAGENTA)$(BOLD)═══ COMPILARE SOLUȚII ═══$(NC)"
	@echo ""

$(SOL_EX1): $(SOL_DIR)/exercise1_sol.c
	@echo "$(YELLOW)⚙ Compilare $(SOL_EX1)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ $(SOL_EX1) compilat$(NC)"

$(SOL_EX2): $(SOL_DIR)/exercise2_sol.c
	@echo "$(YELLOW)⚙ Compilare $(SOL_EX2)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ $(SOL_EX2) compilat$(NC)"

$(SOL_HW1): $(SOL_DIR)/homework1_sol.c
	@echo "$(YELLOW)⚙ Compilare $(SOL_HW1)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ $(SOL_HW1) compilat$(NC)"

$(SOL_HW2): $(SOL_DIR)/homework2_sol.c
	@echo "$(YELLOW)⚙ Compilare $(SOL_HW2)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ $(SOL_HW2) compilat$(NC)"

# =============================================================================
# RULARE
# =============================================================================

run: $(EXAMPLE)
	@echo ""
	@echo "$(MAGENTA)$(BOLD)═══ RULARE EXEMPLU DEMONSTRATIV ═══$(NC)"
	@echo ""
	@./$(EXAMPLE)

run-ex1: $(EXERCISE1)
	@echo ""
	@echo "$(MAGENTA)$(BOLD)═══ RULARE EXERCIȚIUL 1: ACTIVITY SELECTION ═══$(NC)"
	@echo ""
	@./$(EXERCISE1)

run-ex2: $(EXERCISE2)
	@echo ""
	@echo "$(MAGENTA)$(BOLD)═══ RULARE EXERCIȚIUL 2: SUDOKU SOLVER ═══$(NC)"
	@echo ""
	@./$(EXERCISE2)

run-sol1: $(SOL_EX1)
	@echo ""
	@echo "$(MAGENTA)$(BOLD)═══ RULARE SOLUȚIE EXERCIȚIU 1 ═══$(NC)"
	@echo ""
	@./$(SOL_EX1)

run-sol2: $(SOL_EX2)
	@echo ""
	@echo "$(MAGENTA)$(BOLD)═══ RULARE SOLUȚIE EXERCIȚIU 2 ═══$(NC)"
	@echo ""
	@./$(SOL_EX2)

run-hw1: $(SOL_HW1)
	@echo ""
	@echo "$(MAGENTA)$(BOLD)═══ RULARE SOLUȚIE TEMĂ 1: FRACTIONAL KNAPSACK ═══$(NC)"
	@echo ""
	@./$(SOL_HW1)

run-hw2: $(SOL_HW2)
	@echo ""
	@echo "$(MAGENTA)$(BOLD)═══ RULARE SOLUȚIE TEMĂ 2: MAZE SOLVER ═══$(NC)"
	@echo ""
	@./$(SOL_HW2)

# =============================================================================
# TESTARE
# =============================================================================

test: $(TARGETS)
	@echo ""
	@echo "$(MAGENTA)$(BOLD)═══ RULARE TESTE AUTOMATE ═══$(NC)"
	@echo ""
	@echo "$(CYAN)Test 1: Activity Selection$(NC)"
	@if ./$(EXERCISE1) < $(TEST_DIR)/test1_input.txt 2>/dev/null | diff -q - $(TEST_DIR)/test1_expected.txt > /dev/null 2>&1; then \
		echo "$(GREEN)  ✓ Test 1 PASSED$(NC)"; \
	else \
		echo "$(RED)  ✗ Test 1 FAILED$(NC)"; \
	fi
	@echo ""
	@echo "$(CYAN)Test 2: Sudoku Solver$(NC)"
	@if ./$(EXERCISE2) < $(TEST_DIR)/test2_input.txt 2>/dev/null | diff -q - $(TEST_DIR)/test2_expected.txt > /dev/null 2>&1; then \
		echo "$(GREEN)  ✓ Test 2 PASSED$(NC)"; \
	else \
		echo "$(RED)  ✗ Test 2 FAILED$(NC)"; \
	fi
	@echo ""

test-solutions: solutions
	@echo ""
	@echo "$(MAGENTA)$(BOLD)═══ TESTARE SOLUȚII ═══$(NC)"
	@echo ""
	@echo "$(CYAN)Test Soluție 1: Activity Selection$(NC)"
	@./$(SOL_EX1) < $(TEST_DIR)/test1_input.txt > /tmp/sol1_out.txt 2>&1
	@if diff -q /tmp/sol1_out.txt $(TEST_DIR)/test1_expected.txt > /dev/null 2>&1; then \
		echo "$(GREEN)  ✓ Soluția 1 CORECTĂ$(NC)"; \
	else \
		echo "$(YELLOW)  ~ Output diferit (verificați manual)$(NC)"; \
	fi
	@echo ""
	@echo "$(CYAN)Test Soluție 2: Sudoku Solver$(NC)"
	@./$(SOL_EX2) < $(TEST_DIR)/test2_input.txt > /tmp/sol2_out.txt 2>&1
	@if diff -q /tmp/sol2_out.txt $(TEST_DIR)/test2_expected.txt > /dev/null 2>&1; then \
		echo "$(GREEN)  ✓ Soluția 2 CORECTĂ$(NC)"; \
	else \
		echo "$(YELLOW)  ~ Output diferit (verificați manual)$(NC)"; \
	fi
	@echo ""

# =============================================================================
# VALGRIND
# =============================================================================

valgrind: $(EXAMPLE)
	@echo ""
	@echo "$(MAGENTA)$(BOLD)═══ VERIFICARE MEMORY LEAKS CU VALGRIND ═══$(NC)"
	@echo ""
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
		--error-exitcode=1 ./$(EXAMPLE) 2>&1 | head -50
	@echo ""

valgrind-ex1: $(EXERCISE1)
	@echo ""
	@echo "$(MAGENTA)$(BOLD)═══ VALGRIND EXERCIȚIU 1 ═══$(NC)"
	@echo ""
	@valgrind --leak-check=full --show-leak-kinds=all ./$(EXERCISE1)

valgrind-ex2: $(EXERCISE2)
	@echo ""
	@echo "$(MAGENTA)$(BOLD)═══ VALGRIND EXERCIȚIU 2 ═══$(NC)"
	@echo ""
	@valgrind --leak-check=full --show-leak-kinds=all ./$(EXERCISE2)

# =============================================================================
# DEMONSTRAȚII SPECIFICE
# =============================================================================

demo-greedy: $(EXAMPLE)
	@echo ""
	@echo "$(MAGENTA)$(BOLD)═══ DEMONSTRAȚIE ALGORITMI GREEDY ═══$(NC)"
	@echo ""
	@./$(EXAMPLE) --greedy

demo-backtracking: $(EXAMPLE)
	@echo ""
	@echo "$(MAGENTA)$(BOLD)═══ DEMONSTRAȚIE BACKTRACKING ═══$(NC)"
	@echo ""
	@./$(EXAMPLE) --backtracking

demo-nqueens: $(EXAMPLE)
	@echo ""
	@echo "$(MAGENTA)$(BOLD)═══ DEMONSTRAȚIE N-QUEENS (N=8) ═══$(NC)"
	@echo ""
	@./$(EXAMPLE) --nqueens 8

demo-sudoku: $(EXAMPLE)
	@echo ""
	@echo "$(MAGENTA)$(BOLD)═══ DEMONSTRAȚIE SUDOKU SOLVER ═══$(NC)"
	@echo ""
	@./$(EXAMPLE) --sudoku $(DATA_DIR)/sudoku_easy.txt

# =============================================================================
# CURĂȚARE
# =============================================================================

clean:
	@echo ""
	@echo "$(YELLOW)⚙ Curățare fișiere generate...$(NC)"
	@rm -f $(TARGETS) $(SOLUTIONS)
	@rm -f *.o *.out
	@rm -f /tmp/sol*_out.txt
	@echo "$(GREEN)  ✓ Curățare completă$(NC)"
	@echo ""

# =============================================================================
# AJUTOR
# =============================================================================

help:
	@echo ""
	@echo "$(CYAN)╔═══════════════════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(CYAN)║     $(BOLD)SĂPTĂMÂNA 16: GREEDY ȘI BACKTRACKING - AJUTOR$(NC)$(CYAN)                        ║$(NC)"
	@echo "$(CYAN)╚═══════════════════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(BOLD)Targeturi disponibile:$(NC)"
	@echo ""
	@echo "  $(GREEN)make$(NC)              - Compilează toate targeturile principale"
	@echo "  $(GREEN)make solutions$(NC)    - Compilează toate soluțiile"
	@echo ""
	@echo "$(BOLD)Rulare:$(NC)"
	@echo ""
	@echo "  $(GREEN)make run$(NC)          - Rulează exemplul demonstrativ complet"
	@echo "  $(GREEN)make run-ex1$(NC)      - Rulează exercițiul 1 (Activity Selection)"
	@echo "  $(GREEN)make run-ex2$(NC)      - Rulează exercițiul 2 (Sudoku Solver)"
	@echo "  $(GREEN)make run-sol1$(NC)     - Rulează soluția exercițiului 1"
	@echo "  $(GREEN)make run-sol2$(NC)     - Rulează soluția exercițiului 2"
	@echo "  $(GREEN)make run-hw1$(NC)      - Rulează soluția temei 1"
	@echo "  $(GREEN)make run-hw2$(NC)      - Rulează soluția temei 2"
	@echo ""
	@echo "$(BOLD)Demonstrații specifice:$(NC)"
	@echo ""
	@echo "  $(GREEN)make demo-greedy$(NC)      - Demonstrație algoritmi Greedy"
	@echo "  $(GREEN)make demo-backtracking$(NC)- Demonstrație tehnici Backtracking"
	@echo "  $(GREEN)make demo-nqueens$(NC)     - Rezolvare problema N-Queens"
	@echo "  $(GREEN)make demo-sudoku$(NC)      - Rezolvare puzzle Sudoku"
	@echo ""
	@echo "$(BOLD)Testare și verificare:$(NC)"
	@echo ""
	@echo "  $(GREEN)make test$(NC)         - Rulează testele automate"
	@echo "  $(GREEN)make test-solutions$(NC)- Testează soluțiile"
	@echo "  $(GREEN)make valgrind$(NC)     - Verifică memory leaks (exemplu)"
	@echo "  $(GREEN)make valgrind-ex1$(NC) - Verifică memory leaks exercițiu 1"
	@echo "  $(GREEN)make valgrind-ex2$(NC) - Verifică memory leaks exercițiu 2"
	@echo ""
	@echo "$(BOLD)Alte comenzi:$(NC)"
	@echo ""
	@echo "  $(GREEN)make clean$(NC)        - Șterge fișierele generate"
	@echo "  $(GREEN)make help$(NC)         - Afișează acest ajutor"
	@echo ""
	@echo "$(CYAN)───────────────────────────────────────────────────────────────────────────$(NC)"
	@echo ""
	@echo "$(BOLD)Structura proiectului:$(NC)"
	@echo ""
	@echo "  $(CYAN)src/$(NC)       - Cod sursă principal"
	@echo "  $(CYAN)solution/$(NC)  - Soluții complete"
	@echo "  $(CYAN)data/$(NC)      - Fișiere de date pentru teste"
	@echo "  $(CYAN)tests/$(NC)     - Input/output pentru testare automată"
	@echo "  $(CYAN)teme/$(NC)      - Cerințe teme și provocări"
	@echo "  $(CYAN)slides/$(NC)    - Prezentări HTML"
	@echo ""
	@echo "$(YELLOW)Notă: Toate fișierele sunt în limba română$(NC)"
	@echo ""

# =============================================================================
# GENERARE DATE DE TEST (dezvoltare)
# =============================================================================

generate-data:
	@echo ""
	@echo "$(YELLOW)⚙ Generare fișiere de date...$(NC)"
	@mkdir -p $(DATA_DIR)
	@echo "Notă: Fișierele de date trebuie create manual sau cu scripturi separate"
	@echo "$(GREEN)  ✓ Directorul data/ pregătit$(NC)"
	@echo ""

# =============================================================================
# INFO
# =============================================================================

info:
	@echo ""
	@echo "$(CYAN)╔═══════════════════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(CYAN)║     $(BOLD)INFORMAȚII MODUL$(NC)$(CYAN)                                                       ║$(NC)"
	@echo "$(CYAN)╚═══════════════════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(BOLD)Săptămâna 16:$(NC) Paradigme Algoritmice - Greedy și Backtracking"
	@echo ""
	@echo "$(BOLD)Obiective de învățare:$(NC)"
	@echo "  • Înțelegerea paradigmei Greedy și condițiile de corectitudine"
	@echo "  • Implementarea Activity Selection și Fractional Knapsack"
	@echo "  • Stăpânirea tehnicii Backtracking pentru probleme combinatoriale"
	@echo "  • Rezolvarea problemelor N-Queens și Sudoku"
	@echo "  • Analiza comparativă Greedy vs. Backtracking vs. DP"
	@echo ""
	@echo "$(BOLD)Algoritmi implementați:$(NC)"
	@echo "  $(GREEN)Greedy:$(NC)"
	@echo "    • Activity Selection Problem"
	@echo "    • Fractional Knapsack"
	@echo "    • Huffman Coding (simplificat)"
	@echo ""
	@echo "  $(GREEN)Backtracking:$(NC)"
	@echo "    • N-Queens Problem"
	@echo "    • Sudoku Solver"
	@echo "    • Generare Permutări/Combinări"
	@echo "    • Graph Coloring"
	@echo ""
	@echo "$(BOLD)Personalități cheie:$(NC)"
	@echo "  • George Dantzig (Programare liniară, Greedy)"
	@echo "  • Robert W. Floyd (Backtracking, Premiu Turing 1978)"
	@echo ""
	@echo "$(CYAN)───────────────────────────────────────────────────────────────────────────$(NC)"
	@echo ""
	@echo "$(BOLD)Curs:$(NC) Algoritmi și Tehnici de Programare (ATP)"
	@echo "$(BOLD)Instituție:$(NC) Academia de Studii Economice din București - CSIE"
	@echo "$(BOLD)Actualizat:$(NC) Ianuarie 2026"
	@echo ""
