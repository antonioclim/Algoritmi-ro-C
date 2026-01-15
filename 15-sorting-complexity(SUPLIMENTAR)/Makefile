# =============================================================================
# SĂPTĂMÂNA 15: ALGORITMI DE SORTARE AVANSAȚI ȘI ANALIZA COMPLEXITĂȚII - Makefile
# =============================================================================
#
# Utilizare:
#   make          - Compilează toate targeturile
#   make run      - Rulează exemplul demonstrativ
#   make test     - Rulează testele automate
#   make valgrind - Verifică memory leaks
#   make benchmark- Rulează benchmark complet
#   make clean    - Șterge fișierele generate
#   make help     - Afișează acest ajutor
#
# =============================================================================

# Compilator și flaguri
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -pedantic -O2
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
NC = \033[0m  # No Color
BOLD = \033[1m

# Targeturi principale
EXAMPLE = example1
EXERCISE1 = exercise1
EXERCISE2 = exercise2

# Lista de executabile
TARGETS = $(EXAMPLE) $(EXERCISE1) $(EXERCISE2)

# =============================================================================
# REGULI PRINCIPALE
# =============================================================================

.PHONY: all clean run test valgrind help solutions benchmark generate-data

all: banner $(TARGETS)
	@echo ""
	@echo "$(GREEN)$(BOLD)✓ Toate targeturile au fost compilate cu succes!$(NC)"
	@echo ""

banner:
	@echo ""
	@echo "$(CYAN)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(CYAN)║     $(BOLD)SĂPTĂMÂNA 15: SORTARE AVANSATĂ & COMPLEXITATE$(NC)$(CYAN)             ║$(NC)"
	@echo "$(CYAN)╚═══════════════════════════════════════════════════════════════╝$(NC)"
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
# GENERARE DATE DE TEST
# =============================================================================

generate-data:
	@echo ""
	@echo "$(MAGENTA)$(BOLD)═══ GENERARE DATE DE TEST ═══$(NC)"
	@echo ""
	@echo "$(YELLOW)Generare random_10k.txt...$(NC)"
	@shuf -i 1-100000 -n 10000 > $(DATA_DIR)/random_10k.txt 2>/dev/null || \
		(for i in $$(seq 1 10000); do echo $$((RANDOM % 100000)); done > $(DATA_DIR)/random_10k.txt)
	@echo "$(GREEN)  ✓ random_10k.txt generat$(NC)"
	@echo "$(YELLOW)Generare sorted_10k.txt...$(NC)"
	@seq 1 10000 > $(DATA_DIR)/sorted_10k.txt
	@echo "$(GREEN)  ✓ sorted_10k.txt generat$(NC)"
	@echo "$(YELLOW)Generare reversed_10k.txt...$(NC)"
	@seq 10000 -1 1 > $(DATA_DIR)/reversed_10k.txt
	@echo "$(GREEN)  ✓ reversed_10k.txt generat$(NC)"
	@echo "$(YELLOW)Generare nearly_sorted.txt...$(NC)"
	@seq 1 10000 | awk 'BEGIN{srand()} {if(rand()<0.05) print int(rand()*10000); else print}' > $(DATA_DIR)/nearly_sorted.txt
	@echo "$(GREEN)  ✓ nearly_sorted.txt generat$(NC)"
	@echo ""
	@echo "$(GREEN)$(BOLD)✓ Toate fișierele de date au fost generate!$(NC)"

# =============================================================================
# RULARE
# =============================================================================

run: $(EXAMPLE)
	@echo ""
	@echo "$(MAGENTA)$(BOLD)═══ RULARE EXEMPLU DEMONSTRATIV ═══$(NC)"
	@echo ""
	@./$(EXAMPLE)

run-ex1: $(EXERCISE1) generate-data
	@echo ""
	@echo "$(MAGENTA)$(BOLD)═══ RULARE EXERCIȚIUL 1: BENCHMARK SORTARE ═══$(NC)"
	@echo ""
	@./$(EXERCISE1)

run-ex2: $(EXERCISE2)
	@echo ""
	@echo "$(MAGENTA)$(BOLD)═══ RULARE EXERCIȚIUL 2: EXTERNAL SORT ═══$(NC)"
	@echo ""
	@./$(EXERCISE2)

# =============================================================================
# BENCHMARK
# =============================================================================

benchmark: $(EXAMPLE) generate-data
	@echo ""
	@echo "$(MAGENTA)$(BOLD)═══ BENCHMARK COMPLET ALGORITMI DE SORTARE ═══$(NC)"
	@echo ""
	@echo "$(YELLOW)Rulare benchmark pe toate tipurile de date...$(NC)"
	@./$(EXAMPLE) --benchmark
	@echo ""
	@echo "$(GREEN)$(BOLD)═══ BENCHMARK COMPLET ═══$(NC)"

# =============================================================================
# TESTARE
# =============================================================================

test: $(EXERCISE1) $(EXERCISE2)
	@echo ""
	@echo "$(MAGENTA)$(BOLD)═══ RULARE TESTE AUTOMATE ═══$(NC)"
	@echo ""
	@echo "$(YELLOW)Test 1: Exercițiul 1 (Benchmark Sortare)...$(NC)"
	@if [ -f $(TEST_DIR)/test1_input.txt ]; then \
		./$(EXERCISE1) < $(TEST_DIR)/test1_input.txt > /tmp/test1_output.txt 2>&1; \
		if diff -q /tmp/test1_output.txt $(TEST_DIR)/test1_expected.txt > /dev/null 2>&1; then \
			echo "$(GREEN)  ✓ Test 1 PASSED$(NC)"; \
		else \
			echo "$(RED)  ✗ Test 1 FAILED$(NC)"; \
			echo "    Diferențe:"; \
			diff /tmp/test1_output.txt $(TEST_DIR)/test1_expected.txt | head -10 || true; \
		fi; \
	else \
		echo "$(YELLOW)  ⚠ Fișier test negăsit, rulare simplă...$(NC)"; \
		./$(EXERCISE1) --quick-test > /dev/null 2>&1 && echo "$(GREEN)  ✓ Exercițiul 1 rulează$(NC)"; \
	fi
	@echo ""
	@echo "$(YELLOW)Test 2: Exercițiul 2 (External Sort)...$(NC)"
	@if [ -f $(TEST_DIR)/test2_input.txt ]; then \
		./$(EXERCISE2) < $(TEST_DIR)/test2_input.txt > /tmp/test2_output.txt 2>&1; \
		if diff -q /tmp/test2_output.txt $(TEST_DIR)/test2_expected.txt > /dev/null 2>&1; then \
			echo "$(GREEN)  ✓ Test 2 PASSED$(NC)"; \
		else \
			echo "$(RED)  ✗ Test 2 FAILED$(NC)"; \
		fi; \
	else \
		echo "$(YELLOW)  ⚠ Fișier test negăsit, rulare simplă...$(NC)"; \
		./$(EXERCISE2) --quick-test > /dev/null 2>&1 && echo "$(GREEN)  ✓ Exercițiul 2 rulează$(NC)"; \
	fi
	@echo ""
	@echo "$(GREEN)$(BOLD)═══ TESTARE COMPLETĂ ═══$(NC)"

# =============================================================================
# VALGRIND - VERIFICARE MEMORY LEAKS
# =============================================================================

valgrind: $(EXAMPLE)
	@echo ""
	@echo "$(MAGENTA)$(BOLD)═══ VERIFICARE MEMORY LEAKS CU VALGRIND ═══$(NC)"
	@echo ""
	@if command -v valgrind > /dev/null 2>&1; then \
		echo "$(YELLOW)Verificare $(EXAMPLE)...$(NC)"; \
		valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
			--error-exitcode=1 ./$(EXAMPLE) --quick 2>&1 | tail -20; \
		echo ""; \
	else \
		echo "$(RED)Valgrind nu este instalat!$(NC)"; \
		echo "Instalați cu: sudo apt install valgrind"; \
	fi

valgrind-ex1: $(EXERCISE1)
	@echo "$(YELLOW)Verificare $(EXERCISE1)...$(NC)"
	@valgrind --leak-check=full ./$(EXERCISE1) --quick 2>&1 | tail -15

valgrind-ex2: $(EXERCISE2)
	@echo "$(YELLOW)Verificare $(EXERCISE2)...$(NC)"
	@valgrind --leak-check=full ./$(EXERCISE2) --quick 2>&1 | tail -15

# =============================================================================
# SOLUȚII (doar pentru instructori)
# =============================================================================

solutions:
	@echo ""
	@echo "$(MAGENTA)$(BOLD)═══ COMPILARE SOLUȚII ═══$(NC)"
	@echo ""
	@if [ -d $(SOL_DIR) ]; then \
		for f in $(SOL_DIR)/*.c; do \
			if [ -f "$$f" ]; then \
				name=$$(basename "$$f" .c); \
				echo "$(YELLOW)⚙ Compilare $$name...$(NC)"; \
				$(CC) $(CFLAGS) -o $(SOL_DIR)/$$name $$f $(LDFLAGS) && \
				echo "$(GREEN)  ✓ $$name compilat$(NC)"; \
			fi; \
		done; \
	else \
		echo "$(YELLOW)Directorul solution/ nu există.$(NC)"; \
	fi

# =============================================================================
# CURĂȚARE
# =============================================================================

clean:
	@echo ""
	@echo "$(YELLOW)🧹 Curățare fișiere generate...$(NC)"
	@rm -f $(TARGETS)
	@rm -f $(SOL_DIR)/exercise1_sol $(SOL_DIR)/exercise2_sol
	@rm -f $(SOL_DIR)/homework1_sol $(SOL_DIR)/homework2_sol
	@rm -f *.o *.out
	@rm -f /tmp/test*_output.txt
	@rm -f /tmp/run_*.txt /tmp/sorted_*.txt
	@echo "$(GREEN)✓ Curățare completă$(NC)"
	@echo ""

# =============================================================================
# AJUTOR
# =============================================================================

help:
	@echo ""
	@echo "$(CYAN)$(BOLD)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(CYAN)$(BOLD)║     SĂPTĂMÂNA 15: SORTARE - COMENZI DISPONIBILE               ║$(NC)"
	@echo "$(CYAN)$(BOLD)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(BOLD)Compilare:$(NC)"
	@echo "  $(GREEN)make$(NC)            - Compilează toate targeturile"
	@echo "  $(GREEN)make example1$(NC)   - Compilează doar exemplul"
	@echo "  $(GREEN)make exercise1$(NC)  - Compilează exercițiul 1"
	@echo "  $(GREEN)make exercise2$(NC)  - Compilează exercițiul 2"
	@echo ""
	@echo "$(BOLD)Execuție:$(NC)"
	@echo "  $(GREEN)make run$(NC)        - Rulează exemplul demonstrativ"
	@echo "  $(GREEN)make run-ex1$(NC)    - Rulează exercițiul 1 (Benchmark)"
	@echo "  $(GREEN)make run-ex2$(NC)    - Rulează exercițiul 2 (External Sort)"
	@echo "  $(GREEN)make benchmark$(NC)  - Benchmark complet pe toate algoritmii"
	@echo ""
	@echo "$(BOLD)Date:$(NC)"
	@echo "  $(GREEN)make generate-data$(NC) - Generează fișiere de test"
	@echo ""
	@echo "$(BOLD)Testare:$(NC)"
	@echo "  $(GREEN)make test$(NC)       - Rulează testele automate"
	@echo "  $(GREEN)make valgrind$(NC)   - Verifică memory leaks"
	@echo ""
	@echo "$(BOLD)Utilitare:$(NC)"
	@echo "  $(GREEN)make clean$(NC)      - Șterge fișierele generate"
	@echo "  $(GREEN)make help$(NC)       - Afișează acest mesaj"
	@echo ""

# =============================================================================
# SFÂRȘIT MAKEFILE
# =============================================================================
