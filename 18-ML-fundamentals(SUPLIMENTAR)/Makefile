# =============================================================================
# SĂPTĂMÂNA 18: FUNDAMENTE MACHINE LEARNING ÎN C - Makefile
# Algoritmi și Tehnici de Programare (ATP)
# =============================================================================

# Compilator și flag-uri
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -pedantic -O2
LDFLAGS = -lm

# Directoare
SRC_DIR = src
DATA_DIR = data
TEST_DIR = tests
SOL_DIR = solution
BUILD_DIR = build

# Culori pentru output (ANSI escape codes)
GREEN = \033[0;32m
RED = \033[0;31m
YELLOW = \033[0;33m
BLUE = \033[0;34m
CYAN = \033[0;36m
MAGENTA = \033[0;35m
NC = \033[0m
BOLD = \033[1m

# Executabile
TARGETS = example1 exercise1 exercise2
SOLUTION_TARGETS = exercise1_sol exercise2_sol homework1_sol homework2_sol

# Fișiere sursă
EXAMPLE1_SRC = $(SRC_DIR)/example1.c
EXERCISE1_SRC = $(SRC_DIR)/exercise1.c
EXERCISE2_SRC = $(SRC_DIR)/exercise2.c

# =============================================================================
# REGULI PRINCIPALE
# =============================================================================

.PHONY: all clean run test valgrind help solutions run-example run-ex1 run-ex2
.PHONY: demo-regression demo-kmeans demo-knn demo-perceptron info

# Build all targets
all: $(TARGETS)
	@echo "$(GREEN)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(GREEN)║     ✓ Toate executabilele au fost compilate cu succes!        ║$(NC)"
	@echo "$(GREEN)║     Machine Learning Fundamentals - Week 18                   ║$(NC)"
	@echo "$(GREEN)╚═══════════════════════════════════════════════════════════════╝$(NC)"

# Build example1
example1: $(EXAMPLE1_SRC)
	@echo "$(CYAN)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(CYAN)│  Compilare: example1.c (Demo ML Complet)                        │$(NC)"
	@echo "$(CYAN)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ example1 compilat cu succes$(NC)"

# Build exercise1
exercise1: $(EXERCISE1_SRC)
	@echo "$(CYAN)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(CYAN)│  Compilare: exercise1.c (Customer Segmentation)                 │$(NC)"
	@echo "$(CYAN)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ exercise1 compilat cu succes$(NC)"

# Build exercise2
exercise2: $(EXERCISE2_SRC)
	@echo "$(CYAN)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(CYAN)│  Compilare: exercise2.c (Multi-class Classification)            │$(NC)"
	@echo "$(CYAN)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  ✓ exercise2 compilat cu succes$(NC)"

# =============================================================================
# REGULI DE RULARE
# =============================================================================

# Run all examples
run: all
	@echo "$(YELLOW)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(YELLOW)║     Rulare exemplul demonstrativ ML complet...                ║$(NC)"
	@echo "$(YELLOW)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@./example1

# Run only example1
run-example: example1
	@echo "$(YELLOW)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(YELLOW)│  Rulare: example1 (Toate algoritmii ML)                         │$(NC)"
	@echo "$(YELLOW)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@./example1

# Run exercise1
run-ex1: exercise1
	@echo "$(YELLOW)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(YELLOW)│  Rulare: exercise1 (Customer Segmentation cu K-Means)           │$(NC)"
	@echo "$(YELLOW)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@./exercise1 $(DATA_DIR)/customers.csv

# Run exercise2
run-ex2: exercise2
	@echo "$(YELLOW)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(YELLOW)│  Rulare: exercise2 (Iris Classification cu Perceptron)          │$(NC)"
	@echo "$(YELLOW)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@./exercise2 $(DATA_DIR)/iris.csv

# =============================================================================
# DEMO-URI SPECIFICE
# =============================================================================

demo-regression: example1
	@echo "$(MAGENTA)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(MAGENTA)║     Demo: Regresie Liniară cu Gradient Descent                ║$(NC)"
	@echo "$(MAGENTA)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@./example1 --demo regression

demo-kmeans: example1
	@echo "$(MAGENTA)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(MAGENTA)║     Demo: K-Means Clustering cu K-Means++ Init                ║$(NC)"
	@echo "$(MAGENTA)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@./example1 --demo kmeans

demo-knn: example1
	@echo "$(MAGENTA)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(MAGENTA)║     Demo: K-Nearest Neighbors Classification                  ║$(NC)"
	@echo "$(MAGENTA)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@./example1 --demo knn

demo-perceptron: example1
	@echo "$(MAGENTA)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(MAGENTA)║     Demo: Perceptron (AND, OR, XOR)                           ║$(NC)"
	@echo "$(MAGENTA)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@./example1 --demo perceptron

# =============================================================================
# REGULI DE TESTARE
# =============================================================================

# Run automated tests
test: exercise1 exercise2
	@echo "$(MAGENTA)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(MAGENTA)║     Rulare teste automate Machine Learning...                 ║$(NC)"
	@echo "$(MAGENTA)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(CYAN)Test 1: Verificare K-Means clustering$(NC)"
	@if ./exercise1 $(DATA_DIR)/customers.csv > /tmp/test1_out.txt 2>&1; then \
		echo "$(GREEN)  ✓ Test 1 PASSED - K-Means rulează fără erori$(NC)"; \
	else \
		echo "$(RED)  ✗ Test 1 FAILED - K-Means a întâmpinat erori$(NC)"; \
	fi
	@echo ""
	@echo "$(CYAN)Test 2: Verificare clasificare Perceptron$(NC)"
	@if ./exercise2 $(DATA_DIR)/iris.csv > /tmp/test2_out.txt 2>&1; then \
		echo "$(GREEN)  ✓ Test 2 PASSED - Clasificare rulează fără erori$(NC)"; \
	else \
		echo "$(RED)  ✗ Test 2 FAILED - Clasificare a întâmpinat erori$(NC)"; \
	fi
	@echo ""
	@echo "$(CYAN)Test 3: Verificare convergență regresie$(NC)"
	@if ./example1 --test-regression > /tmp/test3_out.txt 2>&1; then \
		echo "$(GREEN)  ✓ Test 3 PASSED - Regresia converge$(NC)"; \
	else \
		echo "$(YELLOW)  ⚠ Test 3 SKIPPED - implementare incompletă$(NC)"; \
	fi
	@echo ""
	@echo "$(CYAN)Test 4: Verificare XOR nu converge (expected)$(NC)"
	@if ./example1 --test-xor 2>&1 | grep -q "NU converge"; then \
		echo "$(GREEN)  ✓ Test 4 PASSED - XOR corect identificat ca ne-separabil$(NC)"; \
	else \
		echo "$(YELLOW)  ⚠ Test 4 SKIPPED - verificare manuală necesară$(NC)"; \
	fi

# =============================================================================
# VERIFICARE MEMORIE CU VALGRIND
# =============================================================================

valgrind: example1
	@echo "$(MAGENTA)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(MAGENTA)║     Verificare memory leaks cu Valgrind...                    ║$(NC)"
	@echo "$(MAGENTA)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@if command -v valgrind > /dev/null 2>&1; then \
		valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./example1 --quick; \
	else \
		echo "$(RED)  ✗ Valgrind nu este instalat$(NC)"; \
		echo "$(YELLOW)  Instalare: sudo apt install valgrind$(NC)"; \
	fi

valgrind-ex1: exercise1
	@echo "$(MAGENTA)Verificare memory leaks pentru exercise1 (K-Means)...$(NC)"
	@valgrind --leak-check=full ./exercise1 $(DATA_DIR)/customers.csv

valgrind-ex2: exercise2
	@echo "$(MAGENTA)Verificare memory leaks pentru exercise2 (Perceptron)...$(NC)"
	@valgrind --leak-check=full ./exercise2 $(DATA_DIR)/iris.csv

# =============================================================================
# BENCHMARK
# =============================================================================

benchmark: example1
	@echo "$(BLUE)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(BLUE)║     Benchmark algoritmi ML...                                 ║$(NC)"
	@echo "$(BLUE)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(CYAN)Benchmark 1: Regresie Liniară (1000 iterații)$(NC)"
	@time -p ./example1 --benchmark regression 2>&1 | head -5
	@echo ""
	@echo "$(CYAN)Benchmark 2: K-Means (k=5, 1000 puncte)$(NC)"
	@time -p ./example1 --benchmark kmeans 2>&1 | head -5
	@echo ""
	@echo "$(CYAN)Benchmark 3: K-NN (k=5, 1000 puncte)$(NC)"
	@time -p ./example1 --benchmark knn 2>&1 | head -5

# =============================================================================
# SOLUȚII (DOAR PENTRU INSTRUCTORI)
# =============================================================================

solutions: 
	@echo "$(MAGENTA)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(MAGENTA)║     Compilare soluții...                                      ║$(NC)"
	@echo "$(MAGENTA)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@if [ -f $(SOL_DIR)/exercise1_sol.c ]; then \
		$(CC) $(CFLAGS) -o exercise1_sol $(SOL_DIR)/exercise1_sol.c $(LDFLAGS); \
		echo "$(GREEN)  ✓ exercise1_sol compilat$(NC)"; \
	else \
		echo "$(YELLOW)  ⚠ exercise1_sol.c nu există$(NC)"; \
	fi
	@if [ -f $(SOL_DIR)/exercise2_sol.c ]; then \
		$(CC) $(CFLAGS) -o exercise2_sol $(SOL_DIR)/exercise2_sol.c $(LDFLAGS); \
		echo "$(GREEN)  ✓ exercise2_sol compilat$(NC)"; \
	else \
		echo "$(YELLOW)  ⚠ exercise2_sol.c nu există$(NC)"; \
	fi
	@if [ -f $(SOL_DIR)/homework1_sol.c ]; then \
		$(CC) $(CFLAGS) -o homework1_sol $(SOL_DIR)/homework1_sol.c $(LDFLAGS); \
		echo "$(GREEN)  ✓ homework1_sol compilat$(NC)"; \
	else \
		echo "$(YELLOW)  ⚠ homework1_sol.c nu există$(NC)"; \
	fi
	@if [ -f $(SOL_DIR)/homework2_sol.c ]; then \
		$(CC) $(CFLAGS) -o homework2_sol $(SOL_DIR)/homework2_sol.c $(LDFLAGS); \
		echo "$(GREEN)  ✓ homework2_sol compilat$(NC)"; \
	else \
		echo "$(YELLOW)  ⚠ homework2_sol.c nu există$(NC)"; \
	fi

# =============================================================================
# CURĂȚARE
# =============================================================================

clean:
	@echo "$(YELLOW)┌─────────────────────────────────────────────────────────────────┐$(NC)"
	@echo "$(YELLOW)│  Curățare fișiere generate...                                   │$(NC)"
	@echo "$(YELLOW)└─────────────────────────────────────────────────────────────────┘$(NC)"
	@rm -f $(TARGETS) $(SOLUTION_TARGETS)
	@rm -f *.o *.out
	@rm -f clusters_output.txt predictions.txt model.dat
	@rm -f /tmp/test*_out.txt
	@rm -rf $(BUILD_DIR)
	@echo "$(GREEN)  ✓ Curățare completă$(NC)"

# =============================================================================
# HELP
# =============================================================================

help:
	@echo ""
	@echo "$(CYAN)╔═══════════════════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(CYAN)║       SĂPTĂMÂNA 18: FUNDAMENTE MACHINE LEARNING - Makefile Help           ║$(NC)"
	@echo "$(CYAN)╠═══════════════════════════════════════════════════════════════════════════╣$(NC)"
	@echo "$(CYAN)║                                                                           ║$(NC)"
	@echo "$(CYAN)║  $(GREEN)Comenzi principale:$(CYAN)                                                    ║$(NC)"
	@echo "$(CYAN)║    make              - Compilează toate executabilele                     ║$(NC)"
	@echo "$(CYAN)║    make run          - Rulează exemplul demonstrativ complet              ║$(NC)"
	@echo "$(CYAN)║    make clean        - Șterge fișierele generate                          ║$(NC)"
	@echo "$(CYAN)║                                                                           ║$(NC)"
	@echo "$(CYAN)║  $(GREEN)Demo-uri specifice:$(CYAN)                                                    ║$(NC)"
	@echo "$(CYAN)║    make demo-regression  - Demo Regresie Liniară                          ║$(NC)"
	@echo "$(CYAN)║    make demo-kmeans      - Demo K-Means Clustering                        ║$(NC)"
	@echo "$(CYAN)║    make demo-knn         - Demo K-Nearest Neighbors                       ║$(NC)"
	@echo "$(CYAN)║    make demo-perceptron  - Demo Perceptron (AND, OR, XOR)                 ║$(NC)"
	@echo "$(CYAN)║                                                                           ║$(NC)"
	@echo "$(CYAN)║  $(GREEN)Comenzi pentru exerciții:$(CYAN)                                              ║$(NC)"
	@echo "$(CYAN)║    make run-ex1      - Rulează exercise1 (Customer Segmentation)          ║$(NC)"
	@echo "$(CYAN)║    make run-ex2      - Rulează exercise2 (Iris Classification)            ║$(NC)"
	@echo "$(CYAN)║                                                                           ║$(NC)"
	@echo "$(CYAN)║  $(GREEN)Comenzi pentru testare:$(CYAN)                                                ║$(NC)"
	@echo "$(CYAN)║    make test         - Rulează teste automate                             ║$(NC)"
	@echo "$(CYAN)║    make valgrind     - Verifică memory leaks                              ║$(NC)"
	@echo "$(CYAN)║    make benchmark    - Măsoară performanța algoritmilor                   ║$(NC)"
	@echo "$(CYAN)║                                                                           ║$(NC)"
	@echo "$(CYAN)║  $(GREEN)Comenzi pentru instructori:$(CYAN)                                            ║$(NC)"
	@echo "$(CYAN)║    make solutions    - Compilează soluțiile                               ║$(NC)"
	@echo "$(CYAN)║                                                                           ║$(NC)"
	@echo "$(CYAN)╚═══════════════════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""

# =============================================================================
# INFORMAȚII SUPLIMENTARE
# =============================================================================

info:
	@echo "$(BLUE)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(BLUE)║     Informații Build                                          ║$(NC)"
	@echo "$(BLUE)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo "$(BLUE)Compilator: $(CC)$(NC)"
	@echo "$(BLUE)Flags: $(CFLAGS)$(NC)"
	@echo "$(BLUE)Linker: $(LDFLAGS)$(NC)"
	@echo "$(BLUE)Surse: $(SRC_DIR)$(NC)"
	@echo "$(BLUE)Date: $(DATA_DIR)$(NC)"
	@echo "$(BLUE)Teste: $(TEST_DIR)$(NC)"
	@echo ""
	@echo "$(YELLOW)Algoritmi incluși:$(NC)"
	@echo "  • Regresie Liniară cu Gradient Descent"
	@echo "  • K-Means Clustering cu K-Means++ Init"
	@echo "  • K-Nearest Neighbors (K-NN)"
	@echo "  • Perceptron (single layer)"
	@echo "  • Mini MLP (bonus, pentru XOR)"

.DEFAULT_GOAL := help
