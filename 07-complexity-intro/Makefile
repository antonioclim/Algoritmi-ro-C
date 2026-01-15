# =============================================================================
# SĂPTĂMÂNA 7: ARBORI BINARI (BINARY TREES) - Makefile
# =============================================================================
# Curs: Algoritmi și Tehnici de Programare (ATP)
# Academia de Studii Economice - CSIE, București
# =============================================================================

# Compilator și flaguri
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -g
LDFLAGS = 

# Directoare
SRC_DIR = src
DATA_DIR = data
TEST_DIR = tests
SOL_DIR = solution
SLIDES_DIR = slides

# Coduri de culoare ANSI
GREEN = \033[0;32m
RED = \033[0;31m
YELLOW = \033[0;33m
CYAN = \033[0;36m
MAGENTA = \033[0;35m
BLUE = \033[0;34m
WHITE = \033[1;37m
NC = \033[0m

# Simboluri
CHECK = ✓
CROSS = ✗
ARROW = →
STAR = ★

# Ținte executabile
TARGETS = example1 exercise1 exercise2
SOLUTIONS = exercise1_sol exercise2_sol homework1_sol homework2_sol

# =============================================================================
# ȚINTE PRINCIPALE
# =============================================================================

.PHONY: all clean run run-ex1 run-ex2 test valgrind help solutions slides

all: banner $(TARGETS)
	@echo ""
	@echo "$(GREEN)$(CHECK) Toate țintele au fost compilate cu succes!$(NC)"
	@echo ""

banner:
	@echo ""
	@echo "$(CYAN)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(CYAN)║$(WHITE)     SĂPTĂMÂNA 7: ARBORI BINARI (BINARY TREES)              $(CYAN)║$(NC)"
	@echo "$(CYAN)║$(WHITE)     Curs ATP - Academia de Studii Economice                $(CYAN)║$(NC)"
	@echo "$(CYAN)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""

# =============================================================================
# COMPILARE EXEMPLE ȘI EXERCIȚII
# =============================================================================

example1: $(SRC_DIR)/example1.c
	@echo "$(CYAN)$(ARROW) Compilare example1.c...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) example1 compilat$(NC)"

exercise1: $(SRC_DIR)/exercise1.c
	@echo "$(CYAN)$(ARROW) Compilare exercise1.c...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) exercise1 compilat$(NC)"

exercise2: $(SRC_DIR)/exercise2.c
	@echo "$(CYAN)$(ARROW) Compilare exercise2.c...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) exercise2 compilat$(NC)"

# =============================================================================
# COMPILARE SOLUȚII (PENTRU INSTRUCTORI)
# =============================================================================

solutions: $(SOLUTIONS)
	@echo ""
	@echo "$(MAGENTA)$(STAR) Toate soluțiile au fost compilate!$(NC)"

exercise1_sol: $(SOL_DIR)/exercise1_sol.c
	@echo "$(MAGENTA)$(ARROW) Compilare soluție exercițiu 1...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) exercise1_sol compilat$(NC)"

exercise2_sol: $(SOL_DIR)/exercise2_sol.c
	@echo "$(MAGENTA)$(ARROW) Compilare soluție exercițiu 2...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) exercise2_sol compilat$(NC)"

homework1_sol: $(SOL_DIR)/homework1_sol.c
	@echo "$(MAGENTA)$(ARROW) Compilare soluție temă 1...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) homework1_sol compilat$(NC)"

homework2_sol: $(SOL_DIR)/homework2_sol.c
	@echo "$(MAGENTA)$(ARROW) Compilare soluție temă 2...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) homework2_sol compilat$(NC)"

# =============================================================================
# RULARE
# =============================================================================

run: example1
	@echo ""
	@echo "$(YELLOW)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(YELLOW)║$(WHITE)           RULARE EXEMPLU DEMONSTRATIV                       $(YELLOW)║$(NC)"
	@echo "$(YELLOW)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@./example1

run-ex1: exercise1
	@echo ""
	@echo "$(YELLOW)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(YELLOW)║$(WHITE)           RULARE EXERCIȚIU 1                                $(YELLOW)║$(NC)"
	@echo "$(YELLOW)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@./exercise1

run-ex2: exercise2
	@echo ""
	@echo "$(YELLOW)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(YELLOW)║$(WHITE)           RULARE EXERCIȚIU 2                                $(YELLOW)║$(NC)"
	@echo "$(YELLOW)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@./exercise2

# =============================================================================
# TESTARE AUTOMATĂ
# =============================================================================

test: exercise1 exercise2
	@echo ""
	@echo "$(BLUE)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(BLUE)║$(WHITE)           TESTARE AUTOMATĂ                                   $(BLUE)║$(NC)"
	@echo "$(BLUE)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(CYAN)$(ARROW) Testare Exercițiu 1...$(NC)"
	@if ./exercise1 < $(TEST_DIR)/test1_input.txt > /tmp/test1_output.txt 2>&1; then \
		if diff -q /tmp/test1_output.txt $(TEST_DIR)/test1_expected.txt > /dev/null 2>&1; then \
			echo "$(GREEN)  $(CHECK) Exercițiu 1: PASSED$(NC)"; \
		else \
			echo "$(RED)  $(CROSS) Exercițiu 1: FAILED (output diferit)$(NC)"; \
			echo "$(YELLOW)  Diferențe:$(NC)"; \
			diff $(TEST_DIR)/test1_expected.txt /tmp/test1_output.txt || true; \
		fi \
	else \
		echo "$(RED)  $(CROSS) Exercițiu 1: EROARE la execuție$(NC)"; \
	fi
	@echo ""
	@echo "$(CYAN)$(ARROW) Testare Exercițiu 2...$(NC)"
	@if ./exercise2 < $(TEST_DIR)/test2_input.txt > /tmp/test2_output.txt 2>&1; then \
		if diff -q /tmp/test2_output.txt $(TEST_DIR)/test2_expected.txt > /dev/null 2>&1; then \
			echo "$(GREEN)  $(CHECK) Exercițiu 2: PASSED$(NC)"; \
		else \
			echo "$(RED)  $(CROSS) Exercițiu 2: FAILED (output diferit)$(NC)"; \
			echo "$(YELLOW)  Diferențe:$(NC)"; \
			diff $(TEST_DIR)/test2_expected.txt /tmp/test2_output.txt || true; \
		fi \
	else \
		echo "$(RED)  $(CROSS) Exercițiu 2: EROARE la execuție$(NC)"; \
	fi
	@echo ""

# =============================================================================
# VERIFICARE MEMORIE CU VALGRIND
# =============================================================================

valgrind: example1
	@echo ""
	@echo "$(MAGENTA)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(MAGENTA)║$(WHITE)           VERIFICARE MEMORIE CU VALGRIND                    $(MAGENTA)║$(NC)"
	@echo "$(MAGENTA)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(CYAN)$(ARROW) Analiză example1...$(NC)"
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
		--error-exitcode=1 ./example1 2>&1 | tail -20
	@echo ""

valgrind-ex1: exercise1
	@echo "$(CYAN)$(ARROW) Analiză exercise1...$(NC)"
	@valgrind --leak-check=full --show-leak-kinds=all ./exercise1 < $(TEST_DIR)/test1_input.txt

valgrind-ex2: exercise2
	@echo "$(CYAN)$(ARROW) Analiză exercise2...$(NC)"
	@valgrind --leak-check=full --show-leak-kinds=all ./exercise2 < $(TEST_DIR)/test2_input.txt

# =============================================================================
# CURĂȚARE
# =============================================================================

clean:
	@echo ""
	@echo "$(YELLOW)$(ARROW) Curățare fișiere generate...$(NC)"
	@rm -f $(TARGETS) $(SOLUTIONS)
	@rm -f *.o
	@rm -f /tmp/test*_output.txt
	@echo "$(GREEN)$(CHECK) Curățare completă!$(NC)"
	@echo ""

# =============================================================================
# DESCHIDERE PREZENTĂRI
# =============================================================================

slides:
	@echo ""
	@echo "$(CYAN)$(ARROW) Deschidere prezentări...$(NC)"
	@if command -v xdg-open > /dev/null; then \
		xdg-open $(SLIDES_DIR)/presentation-week07.html 2>/dev/null & \
		echo "$(GREEN)  $(CHECK) Prezentarea principală deschisă$(NC)"; \
	elif command -v open > /dev/null; then \
		open $(SLIDES_DIR)/presentation-week07.html; \
		echo "$(GREEN)  $(CHECK) Prezentarea principală deschisă$(NC)"; \
	else \
		echo "$(YELLOW)  Deschideți manual: $(SLIDES_DIR)/presentation-week07.html$(NC)"; \
	fi

slides-compare:
	@if command -v xdg-open > /dev/null; then \
		xdg-open $(SLIDES_DIR)/presentation-comparativ.html 2>/dev/null & \
	elif command -v open > /dev/null; then \
		open $(SLIDES_DIR)/presentation-comparativ.html; \
	fi

# =============================================================================
# AJUTOR
# =============================================================================

help:
	@echo ""
	@echo "$(WHITE)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(WHITE)║              SĂPTĂMÂNA 7: ARBORI BINARI - AJUTOR              ║$(NC)"
	@echo "$(WHITE)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(CYAN)Ținte disponibile:$(NC)"
	@echo ""
	@echo "  $(GREEN)make$(NC)              - Compilează toate fișierele sursă"
	@echo "  $(GREEN)make run$(NC)          - Rulează exemplul demonstrativ"
	@echo "  $(GREEN)make run-ex1$(NC)      - Rulează exercițiul 1"
	@echo "  $(GREEN)make run-ex2$(NC)      - Rulează exercițiul 2"
	@echo ""
	@echo "  $(YELLOW)make test$(NC)         - Execută testele automate"
	@echo "  $(YELLOW)make valgrind$(NC)     - Verifică memory leaks"
	@echo "  $(YELLOW)make valgrind-ex1$(NC) - Verifică exercițiul 1"
	@echo "  $(YELLOW)make valgrind-ex2$(NC) - Verifică exercițiul 2"
	@echo ""
	@echo "  $(MAGENTA)make solutions$(NC)    - Compilează soluțiile (instructori)"
	@echo "  $(MAGENTA)make slides$(NC)       - Deschide prezentarea principală"
	@echo "  $(MAGENTA)make slides-compare$(NC) - Deschide comparația limbaje"
	@echo ""
	@echo "  $(RED)make clean$(NC)        - Șterge fișierele generate"
	@echo ""
	@echo "$(CYAN)Structura directorului:$(NC)"
	@echo ""
	@echo "  src/         - Fișiere sursă (exerciții)"
	@echo "  solution/    - Soluții complete"
	@echo "  slides/      - Prezentări HTML"
	@echo "  data/        - Fișiere de date"
	@echo "  tests/       - Teste automate"
	@echo "  teme/        - Cerințe teme"
	@echo ""

# =============================================================================
# INFORMAȚII SUPLIMENTARE
# =============================================================================

info:
	@echo ""
	@echo "$(WHITE)Informații sistem:$(NC)"
	@echo "  Compilator: $(CC)"
	@echo "  Flaguri: $(CFLAGS)"
	@echo "  Sistem: $$(uname -s)"
	@echo ""
	@$(CC) --version | head -1
	@echo ""

# Dependency graph (pentru debug)
.PHONY: deps
deps:
	@echo "$(CYAN)Dependențe:$(NC)"
	@echo "  example1  <- $(SRC_DIR)/example1.c"
	@echo "  exercise1 <- $(SRC_DIR)/exercise1.c"
	@echo "  exercise2 <- $(SRC_DIR)/exercise2.c"
