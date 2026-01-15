# ==============================================================================
# MAKEFILE - Săptămâna 12: Grafuri - Noțiuni de Bază
# ==============================================================================
#
# Utilizare:
#   make              - Compilează toate executabilele
#   make example1     - Compilează doar example1
#   make exercise1    - Compilează exercise1
#   make exercise2    - Compilează exercise2
#   make solutions    - Compilează toate soluțiile
#   make test         - Rulează testele
#   make run          - Rulează toate exemplele
#   make clean        - Șterge fișierele compilate
#   make valgrind     - Verifică memory leaks
#   make help         - Afișează acest mesaj
#
# ==============================================================================

# Compilator și flags
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -g
LDFLAGS = 

# Culori pentru output
COLOR_RESET  = \033[0m
COLOR_GREEN  = \033[1;32m
COLOR_YELLOW = \033[1;33m
COLOR_BLUE   = \033[1;34m
COLOR_RED    = \033[1;31m
COLOR_CYAN   = \033[1;36m
COLOR_MAGENTA = \033[1;35m

# Simboluri
SYMBOL_OK    = ✓
SYMBOL_FAIL  = ✗
SYMBOL_ARROW = →
SYMBOL_BUILD = 🔨
SYMBOL_RUN   = 🚀
SYMBOL_TEST  = 🧪
SYMBOL_CLEAN = 🧹

# Directoare
SRC_DIR = src
SOL_DIR = solution
DATA_DIR = data
TEST_DIR = tests
BUILD_DIR = build

# Fișiere sursă
EXAMPLE = $(SRC_DIR)/example1.c
EXERCISE1 = $(SRC_DIR)/exercise1.c
EXERCISE2 = $(SRC_DIR)/exercise2.c

# Soluții
SOL_EX1 = $(SOL_DIR)/exercise1_sol.c
SOL_EX2 = $(SOL_DIR)/exercise2_sol.c
SOL_HW1 = $(SOL_DIR)/homework1_sol.c
SOL_HW2 = $(SOL_DIR)/homework2_sol.c

# Executabile
BINS = example1 exercise1 exercise2
SOL_BINS = exercise1_sol exercise2_sol homework1_sol homework2_sol

# ==============================================================================
# TARGETS PRINCIPALE
# ==============================================================================

.PHONY: all clean test run help solutions valgrind demo demo-all

# Target implicit
all: banner $(BINS)
	@echo ""
	@echo "$(COLOR_GREEN)$(SYMBOL_OK) Toate fișierele compilate cu succes!$(COLOR_RESET)"
	@echo ""

# Banner de start
banner:
	@echo ""
	@echo "$(COLOR_CYAN)╔═══════════════════════════════════════════════════════════════╗$(COLOR_RESET)"
	@echo "$(COLOR_CYAN)║     $(COLOR_YELLOW)$(SYMBOL_BUILD) Compilare Săptămâna 12: Grafuri$(COLOR_CYAN)                        ║$(COLOR_RESET)"
	@echo "$(COLOR_CYAN)╚═══════════════════════════════════════════════════════════════╝$(COLOR_RESET)"
	@echo ""

# ==============================================================================
# COMPILARE EXERCIȚII
# ==============================================================================

# Exemplu demonstrativ
example1: $(EXAMPLE)
	@echo "$(COLOR_BLUE)$(SYMBOL_ARROW) Compilare$(COLOR_RESET) $@..."
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "  $(COLOR_GREEN)$(SYMBOL_OK)$(COLOR_RESET) $@ compilat"

# Exercițiul 1: BFS
exercise1: $(EXERCISE1)
	@echo "$(COLOR_BLUE)$(SYMBOL_ARROW) Compilare$(COLOR_RESET) $@..."
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "  $(COLOR_GREEN)$(SYMBOL_OK)$(COLOR_RESET) $@ compilat"

# Exercițiul 2: DFS
exercise2: $(EXERCISE2)
	@echo "$(COLOR_BLUE)$(SYMBOL_ARROW) Compilare$(COLOR_RESET) $@..."
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "  $(COLOR_GREEN)$(SYMBOL_OK)$(COLOR_RESET) $@ compilat"

# ==============================================================================
# COMPILARE SOLUȚII (pentru profesori)
# ==============================================================================

solutions: banner-sol $(SOL_BINS)
	@echo ""
	@echo "$(COLOR_GREEN)$(SYMBOL_OK) Toate soluțiile compilate!$(COLOR_RESET)"
	@echo ""

banner-sol:
	@echo ""
	@echo "$(COLOR_MAGENTA)╔═══════════════════════════════════════════════════════════════╗$(COLOR_RESET)"
	@echo "$(COLOR_MAGENTA)║     $(COLOR_YELLOW)🔐 Compilare Soluții (CONFIDENȚIAL)$(COLOR_MAGENTA)                      ║$(COLOR_RESET)"
	@echo "$(COLOR_MAGENTA)╚═══════════════════════════════════════════════════════════════╝$(COLOR_RESET)"
	@echo ""

exercise1_sol: $(SOL_EX1)
	@echo "$(COLOR_MAGENTA)$(SYMBOL_ARROW) Compilare$(COLOR_RESET) $@..."
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "  $(COLOR_GREEN)$(SYMBOL_OK)$(COLOR_RESET) $@ compilat"

exercise2_sol: $(SOL_EX2)
	@echo "$(COLOR_MAGENTA)$(SYMBOL_ARROW) Compilare$(COLOR_RESET) $@..."
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "  $(COLOR_GREEN)$(SYMBOL_OK)$(COLOR_RESET) $@ compilat"

homework1_sol: $(SOL_HW1)
	@echo "$(COLOR_MAGENTA)$(SYMBOL_ARROW) Compilare$(COLOR_RESET) $@..."
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "  $(COLOR_GREEN)$(SYMBOL_OK)$(COLOR_RESET) $@ compilat"

homework2_sol: $(SOL_HW2)
	@echo "$(COLOR_MAGENTA)$(SYMBOL_ARROW) Compilare$(COLOR_RESET) $@..."
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "  $(COLOR_GREEN)$(SYMBOL_OK)$(COLOR_RESET) $@ compilat"

# ==============================================================================
# RULARE ȘI TESTARE
# ==============================================================================

# Rulează exemplul
run: example1
	@echo ""
	@echo "$(COLOR_CYAN)$(SYMBOL_RUN) Rulare example1...$(COLOR_RESET)"
	@echo ""
	@./example1

# Rulează exercițiile
run-exercises: exercise1 exercise2
	@echo ""
	@echo "$(COLOR_CYAN)$(SYMBOL_RUN) Rulare exerciții...$(COLOR_RESET)"
	@echo ""
	@echo "$(COLOR_YELLOW)═══════════════ EXERCIȚIUL 1 ═══════════════$(COLOR_RESET)"
	@./exercise1
	@echo ""
	@echo "$(COLOR_YELLOW)═══════════════ EXERCIȚIUL 2 ═══════════════$(COLOR_RESET)"
	@./exercise2

# Demo complet - rulează exemplul și exercițiile
demo: example1 exercise1 exercise2
	@echo ""
	@echo "$(COLOR_CYAN)╔═══════════════════════════════════════════════════════════════╗$(COLOR_RESET)"
	@echo "$(COLOR_CYAN)║     $(COLOR_YELLOW)$(SYMBOL_RUN) Demo Complet Săptămâna 12$(COLOR_CYAN)                            ║$(COLOR_RESET)"
	@echo "$(COLOR_CYAN)╚═══════════════════════════════════════════════════════════════╝$(COLOR_RESET)"
	@echo ""
	@echo "$(COLOR_GREEN)═══════════════ EXEMPLU DEMONSTRATIV ═══════════════$(COLOR_RESET)"
	@./example1
	@echo ""
	@echo "$(COLOR_YELLOW)═══════════════ EXERCIȚIUL 1: BFS ═══════════════$(COLOR_RESET)"
	@./exercise1
	@echo ""
	@echo "$(COLOR_YELLOW)═══════════════ EXERCIȚIUL 2: DFS ═══════════════$(COLOR_RESET)"
	@./exercise2

# Demo cu soluții (pentru profesori)
demo-all: all solutions
	@echo ""
	@echo "$(COLOR_MAGENTA)╔═══════════════════════════════════════════════════════════════╗$(COLOR_RESET)"
	@echo "$(COLOR_MAGENTA)║     $(COLOR_YELLOW)🎓 Demo Complet cu Soluții$(COLOR_MAGENTA)                               ║$(COLOR_RESET)"
	@echo "$(COLOR_MAGENTA)╚═══════════════════════════════════════════════════════════════╝$(COLOR_RESET)"
	@echo ""
	@echo "$(COLOR_GREEN)═══════════════ SOLUȚIE EXERCIȚIUL 1 ═══════════════$(COLOR_RESET)"
	@./exercise1_sol
	@echo ""
	@echo "$(COLOR_GREEN)═══════════════ SOLUȚIE EXERCIȚIUL 2 ═══════════════$(COLOR_RESET)"
	@./exercise2_sol
	@echo ""
	@echo "$(COLOR_GREEN)═══════════════ SOLUȚIE TEMA 1: REȚEA SOCIALĂ ═══════════════$(COLOR_RESET)"
	@./homework1_sol
	@echo ""
	@echo "$(COLOR_GREEN)═══════════════ SOLUȚIE TEMA 2: DEPENDENȚE ═══════════════$(COLOR_RESET)"
	@./homework2_sol

# Teste automate
test: all
	@echo ""
	@echo "$(COLOR_CYAN)$(SYMBOL_TEST) Rulare teste automate...$(COLOR_RESET)"
	@echo ""
	@echo "$(COLOR_YELLOW)Test 1: Verificare BFS$(COLOR_RESET)"
	@echo "Input: $(TEST_DIR)/test1_input.txt"
	@if [ -f "./exercise1" ]; then \
		echo "  $(COLOR_GREEN)$(SYMBOL_OK)$(COLOR_RESET) exercise1 există"; \
	else \
		echo "  $(COLOR_RED)$(SYMBOL_FAIL)$(COLOR_RESET) exercise1 nu există"; \
	fi
	@echo ""
	@echo "$(COLOR_YELLOW)Test 2: Verificare DFS$(COLOR_RESET)"
	@echo "Input: $(TEST_DIR)/test2_input.txt"
	@if [ -f "./exercise2" ]; then \
		echo "  $(COLOR_GREEN)$(SYMBOL_OK)$(COLOR_RESET) exercise2 există"; \
	else \
		echo "  $(COLOR_RED)$(SYMBOL_FAIL)$(COLOR_RESET) exercise2 nu există"; \
	fi
	@echo ""
	@echo "$(COLOR_GREEN)$(SYMBOL_OK) Teste completate$(COLOR_RESET)"

# Verificare memory leaks cu Valgrind
valgrind: all
	@echo ""
	@echo "$(COLOR_CYAN)🔍 Verificare memory leaks cu Valgrind...$(COLOR_RESET)"
	@echo ""
	@if command -v valgrind >/dev/null 2>&1; then \
		echo "$(COLOR_YELLOW)Verificare example1:$(COLOR_RESET)"; \
		valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 ./example1 2>&1 | tail -10; \
		echo ""; \
	else \
		echo "$(COLOR_RED)Valgrind nu este instalat!$(COLOR_RESET)"; \
		echo "Instalați cu: sudo apt install valgrind"; \
	fi

# ==============================================================================
# CURĂȚARE
# ==============================================================================

clean:
	@echo ""
	@echo "$(COLOR_RED)$(SYMBOL_CLEAN) Curățare fișiere...$(COLOR_RESET)"
	@rm -f $(BINS) $(SOL_BINS)
	@rm -f *.o
	@rm -f core
	@rm -rf $(BUILD_DIR)
	@echo "  $(COLOR_GREEN)$(SYMBOL_OK)$(COLOR_RESET) Fișiere șterse"
	@echo ""

# ==============================================================================
# AJUTOR
# ==============================================================================

help:
	@echo ""
	@echo "$(COLOR_CYAN)╔═══════════════════════════════════════════════════════════════╗$(COLOR_RESET)"
	@echo "$(COLOR_CYAN)║     $(COLOR_YELLOW)📖 Ajutor Makefile - Săptămâna 12$(COLOR_CYAN)                         ║$(COLOR_RESET)"
	@echo "$(COLOR_CYAN)╚═══════════════════════════════════════════════════════════════╝$(COLOR_RESET)"
	@echo ""
	@echo "$(COLOR_GREEN)Comenzi disponibile:$(COLOR_RESET)"
	@echo ""
	@echo "  $(COLOR_YELLOW)make$(COLOR_RESET)              - Compilează exercițiile"
	@echo "  $(COLOR_YELLOW)make example1$(COLOR_RESET)     - Compilează exemplul demonstrativ"
	@echo "  $(COLOR_YELLOW)make exercise1$(COLOR_RESET)    - Compilează exercițiul 1 (BFS)"
	@echo "  $(COLOR_YELLOW)make exercise2$(COLOR_RESET)    - Compilează exercițiul 2 (DFS)"
	@echo ""
	@echo "$(COLOR_MAGENTA)Pentru profesori:$(COLOR_RESET)"
	@echo ""
	@echo "  $(COLOR_YELLOW)make solutions$(COLOR_RESET)    - Compilează soluțiile"
	@echo "  $(COLOR_YELLOW)make demo-all$(COLOR_RESET)     - Rulează demo complet cu soluții"
	@echo ""
	@echo "$(COLOR_CYAN)Testare și rulare:$(COLOR_RESET)"
	@echo ""
	@echo "  $(COLOR_YELLOW)make run$(COLOR_RESET)          - Rulează exemplul"
	@echo "  $(COLOR_YELLOW)make demo$(COLOR_RESET)         - Rulează demo complet"
	@echo "  $(COLOR_YELLOW)make test$(COLOR_RESET)         - Rulează teste automate"
	@echo "  $(COLOR_YELLOW)make valgrind$(COLOR_RESET)     - Verifică memory leaks"
	@echo ""
	@echo "$(COLOR_RED)Curățare:$(COLOR_RESET)"
	@echo ""
	@echo "  $(COLOR_YELLOW)make clean$(COLOR_RESET)        - Șterge fișierele compilate"
	@echo ""
	@echo "$(COLOR_BLUE)Exemplu de utilizare:$(COLOR_RESET)"
	@echo ""
	@echo "  \$$ make                # Compilează tot"
	@echo "  \$$ make demo           # Rulează demonstrația"
	@echo "  \$$ make valgrind       # Verifică memory leaks"
	@echo "  \$$ make clean          # Curăță"
	@echo ""

# ==============================================================================
# INFORMAȚII
# ==============================================================================

info:
	@echo ""
	@echo "$(COLOR_CYAN)Informații proiect:$(COLOR_RESET)"
	@echo "  Compilator: $(CC)"
	@echo "  Flags: $(CFLAGS)"
	@echo "  Exerciții: $(BINS)"
	@echo "  Soluții: $(SOL_BINS)"
	@echo ""
