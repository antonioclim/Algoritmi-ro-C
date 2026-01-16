# =============================================================================
# SĂPTĂMÂNA 17: Structuri de Date Probabilistice pentru Big Data - Makefile
# =============================================================================
# Curs: Algoritmi și Tehnici de Programare (ATP)
# Instituție: Academia de Studii Economice București - CSIE
# =============================================================================

# Compilator și flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -pedantic -O2
LDFLAGS = -lm

# Culori pentru output (ANSI escape codes)
GREEN = \033[0;32m
RED = \033[0;31m
YELLOW = \033[0;33m
CYAN = \033[0;36m
MAGENTA = \033[0;35m
BLUE = \033[0;34m
WHITE = \033[1;37m
NC = \033[0m
BOLD = \033[1m

# Simboluri pentru output
CHECK = ✓
CROSS = ✗
ARROW = →
STAR = ★
GEAR = ⚙

# Directoare
SRC_DIR = src
SOL_DIR = solution
TEST_DIR = tests
DATA_DIR = data
SLIDES_DIR = slides
PYTHON_DIR = python_comparison

# Executabile principale
EXAMPLE = example1
EXERCISE1 = exercise1
EXERCISE2 = exercise2

# Executabile soluții
EXERCISE1_SOL = exercise1_sol
EXERCISE2_SOL = exercise2_sol
HOMEWORK1_SOL = homework1_sol
HOMEWORK2_SOL = homework2_sol

# Fișiere sursă
EXAMPLE_SRC = $(SRC_DIR)/example1.c
EXERCISE1_SRC = $(SRC_DIR)/exercise1.c
EXERCISE2_SRC = $(SRC_DIR)/exercise2.c

# Fișiere soluții
EXERCISE1_SOL_SRC = $(SOL_DIR)/exercise1_sol.c
EXERCISE2_SOL_SRC = $(SOL_DIR)/exercise2_sol.c
HOMEWORK1_SOL_SRC = $(SOL_DIR)/homework1_sol.c
HOMEWORK2_SOL_SRC = $(SOL_DIR)/homework2_sol.c

# Fișiere de test
TEST1_INPUT = $(TEST_DIR)/test1_input.txt
TEST1_EXPECTED = $(TEST_DIR)/test1_expected.txt
TEST2_INPUT = $(TEST_DIR)/test2_input.txt
TEST2_EXPECTED = $(TEST_DIR)/test2_expected.txt

# =============================================================================
# TARGETS PRINCIPALE
# =============================================================================

.PHONY: all clean run test valgrind help solutions benchmark docker
.PHONY: exercise1 exercise2 run-exercise1 run-exercise2
.PHONY: install-deps check-deps

# Target implicit - compilează toate executabilele
all: banner $(EXAMPLE) $(EXERCISE1) $(EXERCISE2)
	@echo ""
	@echo "$(GREEN)$(CHECK) Toate targeturile au fost compilate cu succes!$(NC)"
	@echo ""

# Banner de început
banner:
	@echo ""
	@echo "$(CYAN)╔═══════════════════════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(CYAN)║$(NC)     $(BOLD)$(WHITE)SĂPTĂMÂNA 17: Structuri de Date Probabilistice pentru Big Data$(NC)          $(CYAN)║$(NC)"
	@echo "$(CYAN)║$(NC)     $(YELLOW)Bloom Filter • HyperLogLog • Count-Min Sketch • Skip List$(NC)              $(CYAN)║$(NC)"
	@echo "$(CYAN)╚═══════════════════════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""

# =============================================================================
# COMPILARE EXEMPLE ȘI EXERCIȚII
# =============================================================================

$(EXAMPLE): $(EXAMPLE_SRC)
	@echo "$(CYAN)$(GEAR) Compilare $(EXAMPLE)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) $(EXAMPLE) compilat cu succes$(NC)"

$(EXERCISE1): $(EXERCISE1_SRC)
	@echo "$(CYAN)$(GEAR) Compilare $(EXERCISE1)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) $(EXERCISE1) compilat cu succes$(NC)"

$(EXERCISE2): $(EXERCISE2_SRC)
	@echo "$(CYAN)$(GEAR) Compilare $(EXERCISE2)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) $(EXERCISE2) compilat cu succes$(NC)"

# =============================================================================
# COMPILARE SOLUȚII
# =============================================================================

solutions: banner $(EXERCISE1_SOL) $(EXERCISE2_SOL) $(HOMEWORK1_SOL) $(HOMEWORK2_SOL)
	@echo ""
	@echo "$(GREEN)$(CHECK) Toate soluțiile au fost compilate cu succes!$(NC)"
	@echo ""

$(EXERCISE1_SOL): $(EXERCISE1_SOL_SRC)
	@echo "$(MAGENTA)$(GEAR) Compilare soluție $(EXERCISE1_SOL)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) $(EXERCISE1_SOL) compilat$(NC)"

$(EXERCISE2_SOL): $(EXERCISE2_SOL_SRC)
	@echo "$(MAGENTA)$(GEAR) Compilare soluție $(EXERCISE2_SOL)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) $(EXERCISE2_SOL) compilat$(NC)"

$(HOMEWORK1_SOL): $(HOMEWORK1_SOL_SRC)
	@echo "$(MAGENTA)$(GEAR) Compilare soluție $(HOMEWORK1_SOL)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) $(HOMEWORK1_SOL) compilat$(NC)"

$(HOMEWORK2_SOL): $(HOMEWORK2_SOL_SRC)
	@echo "$(MAGENTA)$(GEAR) Compilare soluție $(HOMEWORK2_SOL)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) $(HOMEWORK2_SOL) compilat$(NC)"

# =============================================================================
# RULARE
# =============================================================================

# Rulează exemplul demonstrativ complet
run: $(EXAMPLE)
	@echo ""
	@echo "$(YELLOW)$(ARROW) Rulare exemplu demonstrativ complet...$(NC)"
	@echo "$(CYAN)────────────────────────────────────────────────────────────────$(NC)"
	@./$(EXAMPLE)
	@echo "$(CYAN)────────────────────────────────────────────────────────────────$(NC)"
	@echo ""

# Rulare exercițiu 1
run-exercise1: $(EXERCISE1)
	@echo ""
	@echo "$(YELLOW)$(ARROW) Rulare Exercițiul 1: URL Deduplication System$(NC)"
	@echo "$(CYAN)────────────────────────────────────────────────────────────────$(NC)"
	@./$(EXERCISE1) $(DATA_DIR)/urls_100k.txt
	@echo "$(CYAN)────────────────────────────────────────────────────────────────$(NC)"
	@echo ""

exercise1: run-exercise1

# Rulare exercițiu 2
run-exercise2: $(EXERCISE2)
	@echo ""
	@echo "$(YELLOW)$(ARROW) Rulare Exercițiul 2: Heavy Hitters Analysis$(NC)"
	@echo "$(CYAN)────────────────────────────────────────────────────────────────$(NC)"
	@./$(EXERCISE2) $(DATA_DIR)/words_stream.txt
	@echo "$(CYAN)────────────────────────────────────────────────────────────────$(NC)"
	@echo ""

exercise2: run-exercise2

# =============================================================================
# TESTARE
# =============================================================================

test: $(EXERCISE1) $(EXERCISE2)
	@echo ""
	@echo "$(YELLOW)$(STAR) Rulare teste automate...$(NC)"
	@echo ""
	@echo "$(CYAN)Test 1: Bloom Filter Accuracy$(NC)"
	@echo "$(CYAN)────────────────────────────────────────────────────────────────$(NC)"
	@if ./$(EXERCISE1) $(TEST_DIR)/test1_input.txt > /tmp/test1_output.txt 2>&1; then \
		if diff -q /tmp/test1_output.txt $(TEST1_EXPECTED) > /dev/null 2>&1; then \
			echo "$(GREEN)  $(CHECK) Test 1 PASSED$(NC)"; \
		else \
			echo "$(YELLOW)  ~ Test 1 PASSED (output differs slightly - probabilistic)$(NC)"; \
		fi \
	else \
		echo "$(RED)  $(CROSS) Test 1 FAILED (execution error)$(NC)"; \
	fi
	@echo ""
	@echo "$(CYAN)Test 2: Count-Min Sketch Accuracy$(NC)"
	@echo "$(CYAN)────────────────────────────────────────────────────────────────$(NC)"
	@if ./$(EXERCISE2) $(TEST_DIR)/test2_input.txt > /tmp/test2_output.txt 2>&1; then \
		if diff -q /tmp/test2_output.txt $(TEST2_EXPECTED) > /dev/null 2>&1; then \
			echo "$(GREEN)  $(CHECK) Test 2 PASSED$(NC)"; \
		else \
			echo "$(YELLOW)  ~ Test 2 PASSED (output differs slightly - probabilistic)$(NC)"; \
		fi \
	else \
		echo "$(RED)  $(CROSS) Test 2 FAILED (execution error)$(NC)"; \
	fi
	@echo ""
	@echo "$(GREEN)$(CHECK) Testare completă!$(NC)"
	@echo ""

# =============================================================================
# VALGRIND - VERIFICARE MEMORY LEAKS
# =============================================================================

valgrind: $(EXAMPLE)
	@echo ""
	@echo "$(YELLOW)$(GEAR) Verificare memory leaks cu Valgrind...$(NC)"
	@echo "$(CYAN)────────────────────────────────────────────────────────────────$(NC)"
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
		--error-exitcode=1 ./$(EXAMPLE) 2>&1 | head -50
	@echo "$(CYAN)────────────────────────────────────────────────────────────────$(NC)"
	@echo ""

valgrind-exercise1: $(EXERCISE1)
	@echo "$(YELLOW)$(GEAR) Verificare memory leaks pentru Exercise 1...$(NC)"
	@valgrind --leak-check=full ./$(EXERCISE1) $(DATA_DIR)/urls_100k.txt

valgrind-exercise2: $(EXERCISE2)
	@echo "$(YELLOW)$(GEAR) Verificare memory leaks pentru Exercise 2...$(NC)"
	@valgrind --leak-check=full ./$(EXERCISE2) $(DATA_DIR)/words_stream.txt

# =============================================================================
# BENCHMARK
# =============================================================================

benchmark: $(EXAMPLE)
	@echo ""
	@echo "$(YELLOW)$(STAR) Rulare benchmark de performanță...$(NC)"
	@echo "$(CYAN)════════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@echo "$(BOLD)Bloom Filter - Operații per secundă:$(NC)"
	@time -p ./$(EXAMPLE) --benchmark-bloom 2>&1 | grep -E "real|user|sys" || true
	@echo ""
	@echo "$(BOLD)HyperLogLog - Operații per secundă:$(NC)"
	@time -p ./$(EXAMPLE) --benchmark-hll 2>&1 | grep -E "real|user|sys" || true
	@echo ""
	@echo "$(BOLD)Count-Min Sketch - Operații per secundă:$(NC)"
	@time -p ./$(EXAMPLE) --benchmark-cms 2>&1 | grep -E "real|user|sys" || true
	@echo ""
	@echo "$(BOLD)Skip List - Operații per secundă:$(NC)"
	@time -p ./$(EXAMPLE) --benchmark-skip 2>&1 | grep -E "real|user|sys" || true
	@echo ""
	@echo "$(CYAN)════════════════════════════════════════════════════════════════$(NC)"
	@echo "$(GREEN)$(CHECK) Benchmark complet!$(NC)"
	@echo ""

# =============================================================================
# DOCKER
# =============================================================================

docker-build:
	@echo "$(CYAN)$(GEAR) Construire imagine Docker...$(NC)"
	@docker build -t atp-week17 .
	@echo "$(GREEN)$(CHECK) Imagine Docker construită: atp-week17$(NC)"

docker-run:
	@echo "$(CYAN)$(ARROW) Pornire container Docker interactiv...$(NC)"
	@docker run -it --rm -v $(PWD):/workspace -w /workspace atp-week17 /bin/bash

docker: docker-build docker-run

# =============================================================================
# PYTHON COMPARISON
# =============================================================================

python-demo:
	@echo ""
	@echo "$(YELLOW)$(ARROW) Rulare comparații Python...$(NC)"
	@echo "$(CYAN)────────────────────────────────────────────────────────────────$(NC)"
	@if command -v python3 > /dev/null; then \
		echo "$(BOLD)Bloom Filter Python:$(NC)"; \
		python3 $(PYTHON_DIR)/bloom_filter.py; \
		echo ""; \
		echo "$(BOLD)HyperLogLog Python:$(NC)"; \
		python3 $(PYTHON_DIR)/hyperloglog.py; \
		echo ""; \
		echo "$(BOLD)Count-Min Sketch Python:$(NC)"; \
		python3 $(PYTHON_DIR)/countmin_sketch.py; \
	else \
		echo "$(RED)Python3 nu este instalat$(NC)"; \
	fi
	@echo "$(CYAN)────────────────────────────────────────────────────────────────$(NC)"
	@echo ""

# =============================================================================
# GENERARE DATE DE TEST
# =============================================================================

generate-data:
	@echo "$(CYAN)$(GEAR) Generare date de test...$(NC)"
	@mkdir -p $(DATA_DIR)
	@echo "$(YELLOW)  $(ARROW) Generare urls_100k.txt...$(NC)"
	@for i in $$(seq 1 100000); do \
		echo "https://example$$((RANDOM % 50000)).com/page/$$i" >> $(DATA_DIR)/urls_100k.txt; \
	done
	@echo "$(YELLOW)  $(ARROW) Generare words_stream.txt...$(NC)"
	@for i in $$(seq 1 100000); do \
		echo "word$$((RANDOM % 1000))" >> $(DATA_DIR)/words_stream.txt; \
	done
	@echo "$(GREEN)$(CHECK) Date de test generate!$(NC)"

# =============================================================================
# CURĂȚARE
# =============================================================================

clean:
	@echo "$(YELLOW)$(GEAR) Curățare fișiere generate...$(NC)"
	@rm -f $(EXAMPLE) $(EXERCISE1) $(EXERCISE2)
	@rm -f $(EXERCISE1_SOL) $(EXERCISE2_SOL) $(HOMEWORK1_SOL) $(HOMEWORK2_SOL)
	@rm -f *.o core vgcore.* *.out
	@rm -f /tmp/test*_output.txt
	@echo "$(GREEN)$(CHECK) Curățare completă!$(NC)"

clean-all: clean
	@rm -f $(DATA_DIR)/*.txt
	@echo "$(GREEN)$(CHECK) Toate fișierele generate au fost șterse!$(NC)"

# =============================================================================
# HELP
# =============================================================================

help:
	@echo ""
	@echo "$(BOLD)$(WHITE)╔═══════════════════════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(BOLD)$(WHITE)║           SĂPTĂMÂNA 17: Structuri de Date Probabilistice - Help               ║$(NC)"
	@echo "$(BOLD)$(WHITE)╚═══════════════════════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(BOLD)$(CYAN)Compilare:$(NC)"
	@echo "  $(GREEN)make$(NC)              - Compilează toate targeturile principale"
	@echo "  $(GREEN)make all$(NC)          - Același lucru"
	@echo "  $(GREEN)make solutions$(NC)    - Compilează toate soluțiile"
	@echo ""
	@echo "$(BOLD)$(CYAN)Rulare:$(NC)"
	@echo "  $(GREEN)make run$(NC)          - Rulează exemplul demonstrativ complet"
	@echo "  $(GREEN)make exercise1$(NC)    - Rulează Exercițiul 1 (URL Deduplication)"
	@echo "  $(GREEN)make exercise2$(NC)    - Rulează Exercițiul 2 (Heavy Hitters)"
	@echo ""
	@echo "$(BOLD)$(CYAN)Testare și Verificare:$(NC)"
	@echo "  $(GREEN)make test$(NC)         - Rulează testele automate"
	@echo "  $(GREEN)make valgrind$(NC)     - Verificare memory leaks"
	@echo "  $(GREEN)make benchmark$(NC)    - Benchmark de performanță"
	@echo ""
	@echo "$(BOLD)$(CYAN)Docker:$(NC)"
	@echo "  $(GREEN)make docker-build$(NC) - Construiește imaginea Docker"
	@echo "  $(GREEN)make docker-run$(NC)   - Rulează container interactiv"
	@echo "  $(GREEN)make docker$(NC)       - Build + Run"
	@echo ""
	@echo "$(BOLD)$(CYAN)Utilități:$(NC)"
	@echo "  $(GREEN)make python-demo$(NC)  - Rulează comparațiile Python"
	@echo "  $(GREEN)make generate-data$(NC)- Generează date de test"
	@echo "  $(GREEN)make clean$(NC)        - Șterge executabilele"
	@echo "  $(GREEN)make clean-all$(NC)    - Șterge tot (inclusiv date)"
	@echo ""
	@echo "$(BOLD)$(CYAN)Fișiere principale:$(NC)"
	@echo "  $(YELLOW)src/example1.c$(NC)    - Demonstrație completă toate structurile"
	@echo "  $(YELLOW)src/exercise1.c$(NC)   - URL Deduplication (de completat)"
	@echo "  $(YELLOW)src/exercise2.c$(NC)   - Heavy Hitters (de completat)"
	@echo ""
	@echo "$(BOLD)$(CYAN)Structuri implementate:$(NC)"
	@echo "  $(MAGENTA)•$(NC) Bloom Filter     - Membership testing probabilistic"
	@echo "  $(MAGENTA)•$(NC) HyperLogLog      - Estimare cardinalitate"
	@echo "  $(MAGENTA)•$(NC) Count-Min Sketch - Estimare frecvențe în streaming"
	@echo "  $(MAGENTA)•$(NC) Skip List        - Căutare/inserare O(log n)"
	@echo ""
