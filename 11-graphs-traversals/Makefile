# =============================================================================
# SĂPTĂMÂNA 11: TABELE DE DISPERSIE (HASH TABLES) - Makefile
# =============================================================================
# Curs: Algoritmi și Tehnici de Programare (ATP)
# Academia de Studii Economice București - CSIE
# =============================================================================

# Compiler și flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -O2
LDFLAGS = 
DEBUG_FLAGS = -g -O0 -DDEBUG

# Directoare
SRC_DIR = src
DATA_DIR = data
TEST_DIR = tests
SOL_DIR = solution
BUILD_DIR = build

# Culori ANSI pentru output
GREEN = \033[0;32m
RED = \033[0;31m
YELLOW = \033[0;33m
CYAN = \033[0;36m
MAGENTA = \033[0;35m
BLUE = \033[0;34m
BOLD = \033[1m
NC = \033[0m

# Simboluri Unicode
CHECK = ✓
CROSS = ✗
ARROW = ➤
GEAR = ⚙
ROCKET = 🚀
BROOM = 🧹
TEST_ICON = 🧪
MEM_ICON = 🔍

# Targets principale
TARGETS = example1 exercise1 exercise2

# Solutions (pentru instructor)
SOLUTIONS = exercise1_sol exercise2_sol homework1_sol homework2_sol

.PHONY: all clean run test valgrind help debug solutions demo stats

# =============================================================================
# TARGET PRINCIPAL
# =============================================================================

all: banner $(TARGETS)
	@echo ""
	@echo "$(GREEN)$(BOLD)$(CHECK) Toate targeturile au fost compilate cu succes!$(NC)"
	@echo "$(CYAN)$(ARROW) Rulează 'make help' pentru a vedea comenzile disponibile$(NC)"
	@echo ""

banner:
	@echo ""
	@echo "$(BLUE)$(BOLD)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(BLUE)$(BOLD)║     $(GEAR) SĂPTĂMÂNA 11: TABELE DE DISPERSIE (HASH TABLES)      ║$(NC)"
	@echo "$(BLUE)$(BOLD)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""

# =============================================================================
# COMPILARE TARGETS PRINCIPALE
# =============================================================================

example1: $(SRC_DIR)/example1.c
	@echo "$(CYAN)$(GEAR) Compilare example1.c...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $<
	@echo "$(GREEN)  $(CHECK) example1 compilat$(NC)"

exercise1: $(SRC_DIR)/exercise1.c
	@echo "$(CYAN)$(GEAR) Compilare exercise1.c...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $<
	@echo "$(GREEN)  $(CHECK) exercise1 compilat$(NC)"

exercise2: $(SRC_DIR)/exercise2.c
	@echo "$(CYAN)$(GEAR) Compilare exercise2.c...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $<
	@echo "$(GREEN)  $(CHECK) exercise2 compilat$(NC)"

# =============================================================================
# COMPILARE SOLUȚII (DOAR PENTRU INSTRUCTOR)
# =============================================================================

solutions: banner $(SOLUTIONS)
	@echo ""
	@echo "$(MAGENTA)$(BOLD)$(CHECK) Toate soluțiile au fost compilate!$(NC)"
	@echo ""

exercise1_sol: $(SOL_DIR)/exercise1_sol.c
	@echo "$(MAGENTA)$(GEAR) Compilare exercise1_sol.c...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $<
	@echo "$(GREEN)  $(CHECK) exercise1_sol compilat$(NC)"

exercise2_sol: $(SOL_DIR)/exercise2_sol.c
	@echo "$(MAGENTA)$(GEAR) Compilare exercise2_sol.c...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $<
	@echo "$(GREEN)  $(CHECK) exercise2_sol compilat$(NC)"

homework1_sol: $(SOL_DIR)/homework1_sol.c
	@echo "$(MAGENTA)$(GEAR) Compilare homework1_sol.c...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $<
	@echo "$(GREEN)  $(CHECK) homework1_sol compilat$(NC)"

homework2_sol: $(SOL_DIR)/homework2_sol.c
	@echo "$(MAGENTA)$(GEAR) Compilare homework2_sol.c...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $<
	@echo "$(GREEN)  $(CHECK) homework2_sol compilat$(NC)"

# =============================================================================
# EXECUȚIE
# =============================================================================

run: all
	@echo ""
	@echo "$(YELLOW)$(BOLD)$(ROCKET) Rulare exemplu demonstrativ...$(NC)"
	@echo "$(YELLOW)─────────────────────────────────────────$(NC)"
	@./example1
	@echo ""
	@echo "$(GREEN)$(CHECK) Execuție finalizată$(NC)"

run-exercise1: exercise1
	@echo ""
	@echo "$(YELLOW)$(BOLD)$(ROCKET) Rulare exercițiul 1 (Chaining)...$(NC)"
	@echo "$(YELLOW)─────────────────────────────────────────$(NC)"
	@./exercise1 < $(DATA_DIR)/words_ro.txt
	@echo ""

run-exercise2: exercise2
	@echo ""
	@echo "$(YELLOW)$(BOLD)$(ROCKET) Rulare exercițiul 2 (Open Addressing)...$(NC)"
	@echo "$(YELLOW)─────────────────────────────────────────$(NC)"
	@./exercise2 < $(DATA_DIR)/names.txt
	@echo ""

demo: all
	@echo ""
	@echo "$(YELLOW)$(BOLD)$(ROCKET) Demonstrație completă...$(NC)"
	@echo ""
	@echo "$(CYAN)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(CYAN)                    EXEMPLU COMPLET                              $(NC)"
	@echo "$(CYAN)═══════════════════════════════════════════════════════════════$(NC)"
	@./example1
	@echo ""
	@echo "$(CYAN)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(CYAN)                    EXERCIȚIUL 1: CHAINING                       $(NC)"
	@echo "$(CYAN)═══════════════════════════════════════════════════════════════$(NC)"
	@./exercise1 < $(TEST_DIR)/test1_input.txt
	@echo ""
	@echo "$(CYAN)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(CYAN)                    EXERCIȚIUL 2: OPEN ADDRESSING                $(NC)"
	@echo "$(CYAN)═══════════════════════════════════════════════════════════════$(NC)"
	@./exercise2 < $(TEST_DIR)/test2_input.txt
	@echo ""
	@echo "$(GREEN)$(BOLD)$(CHECK) Demonstrație completă finalizată!$(NC)"

# =============================================================================
# TESTE AUTOMATE
# =============================================================================

test: all
	@echo ""
	@echo "$(YELLOW)$(BOLD)$(TEST_ICON) Rulare teste automate...$(NC)"
	@echo ""
	@echo "$(CYAN)Test 1: Hash Table cu Chaining$(NC)"
	@./exercise1 < $(TEST_DIR)/test1_input.txt > /tmp/test1_output.txt 2>&1 || true
	@if diff -q /tmp/test1_output.txt $(TEST_DIR)/test1_expected.txt > /dev/null 2>&1; then \
		echo "$(GREEN)  $(CHECK) Test 1 PASSED$(NC)"; \
	else \
		echo "$(RED)  $(CROSS) Test 1 FAILED$(NC)"; \
		echo "$(YELLOW)  Diferențe:$(NC)"; \
		diff $(TEST_DIR)/test1_expected.txt /tmp/test1_output.txt || true; \
	fi
	@echo ""
	@echo "$(CYAN)Test 2: Hash Table cu Open Addressing$(NC)"
	@./exercise2 < $(TEST_DIR)/test2_input.txt > /tmp/test2_output.txt 2>&1 || true
	@if diff -q /tmp/test2_output.txt $(TEST_DIR)/test2_expected.txt > /dev/null 2>&1; then \
		echo "$(GREEN)  $(CHECK) Test 2 PASSED$(NC)"; \
	else \
		echo "$(RED)  $(CROSS) Test 2 FAILED$(NC)"; \
		echo "$(YELLOW)  Diferențe:$(NC)"; \
		diff $(TEST_DIR)/test2_expected.txt /tmp/test2_output.txt || true; \
	fi
	@echo ""
	@rm -f /tmp/test1_output.txt /tmp/test2_output.txt
	@echo "$(GREEN)$(CHECK) Teste finalizate$(NC)"

# =============================================================================
# VERIFICARE MEMORIE
# =============================================================================

valgrind: all
	@echo ""
	@echo "$(YELLOW)$(BOLD)$(MEM_ICON) Verificare memory leaks cu Valgrind...$(NC)"
	@echo ""
	@echo "$(CYAN)Verificare example1:$(NC)"
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
		--error-exitcode=1 ./example1 2>&1 | tail -20 || true
	@echo ""
	@echo "$(CYAN)Verificare exercise1:$(NC)"
	@valgrind --leak-check=full --show-leak-kinds=all \
		./exercise1 < $(TEST_DIR)/test1_input.txt 2>&1 | tail -20 || true
	@echo ""
	@echo "$(CYAN)Verificare exercise2:$(NC)"
	@valgrind --leak-check=full --show-leak-kinds=all \
		./exercise2 < $(TEST_DIR)/test2_input.txt 2>&1 | tail -20 || true
	@echo ""
	@echo "$(GREEN)$(CHECK) Verificare memorie finalizată$(NC)"

# =============================================================================
# DEBUGGING
# =============================================================================

debug: CFLAGS += $(DEBUG_FLAGS)
debug: clean all
	@echo ""
	@echo "$(MAGENTA)$(BOLD)$(GEAR) Build debug completat$(NC)"
	@echo "$(CYAN)Folosește: gdb ./example1$(NC)"
	@echo "$(CYAN)Comenzi GDB utile:$(NC)"
	@echo "  $(YELLOW)break main$(NC)        - Setează breakpoint la main"
	@echo "  $(YELLOW)run$(NC)               - Pornește execuția"
	@echo "  $(YELLOW)next$(NC)              - Execută linia curentă"
	@echo "  $(YELLOW)step$(NC)              - Intră în funcție"
	@echo "  $(YELLOW)print var$(NC)         - Afișează valoarea variabilei"
	@echo "  $(YELLOW)backtrace$(NC)         - Afișează stack-ul"

# =============================================================================
# STATISTICI
# =============================================================================

stats:
	@echo ""
	@echo "$(CYAN)$(BOLD)📊 Statistici Proiect$(NC)"
	@echo "$(CYAN)─────────────────────────────────────────$(NC)"
	@echo "$(YELLOW)Fișiere sursă:$(NC)"
	@wc -l $(SRC_DIR)/*.c 2>/dev/null || echo "  Nu există fișiere sursă"
	@echo ""
	@echo "$(YELLOW)Fișiere soluție:$(NC)"
	@wc -l $(SOL_DIR)/*.c 2>/dev/null || echo "  Nu există fișiere soluție"
	@echo ""
	@echo "$(YELLOW)Total linii de cod:$(NC)"
	@find . -name "*.c" -exec cat {} \; 2>/dev/null | wc -l
	@echo ""
	@echo "$(YELLOW)Dimensiune fișiere:$(NC)"
	@du -sh $(SRC_DIR) $(SOL_DIR) slides teme 2>/dev/null || true

# =============================================================================
# CURĂȚARE
# =============================================================================

clean:
	@echo ""
	@echo "$(YELLOW)$(BOLD)$(BROOM) Curățare artefacte...$(NC)"
	@rm -f $(TARGETS) $(SOLUTIONS)
	@rm -f *.o *.out core
	@rm -rf $(BUILD_DIR)
	@rm -f /tmp/test*_output.txt
	@echo "$(GREEN)$(CHECK) Curățare completă$(NC)"
	@echo ""

# =============================================================================
# AJUTOR
# =============================================================================

help:
	@echo ""
	@echo "$(BLUE)$(BOLD)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(BLUE)$(BOLD)║                    COMENZI DISPONIBILE                         ║$(NC)"
	@echo "$(BLUE)$(BOLD)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(CYAN)$(BOLD)Compilare:$(NC)"
	@echo "  $(GREEN)make$(NC)            - Compilează toate targeturile principale"
	@echo "  $(GREEN)make solutions$(NC)  - Compilează soluțiile (instructor)"
	@echo "  $(GREEN)make debug$(NC)      - Compilează cu simboluri debug"
	@echo ""
	@echo "$(CYAN)$(BOLD)Execuție:$(NC)"
	@echo "  $(GREEN)make run$(NC)        - Rulează exemplul demonstrativ"
	@echo "  $(GREEN)make demo$(NC)       - Rulează demonstrația completă"
	@echo "  $(GREEN)make run-exercise1$(NC) - Rulează exercițiul 1"
	@echo "  $(GREEN)make run-exercise2$(NC) - Rulează exercițiul 2"
	@echo ""
	@echo "$(CYAN)$(BOLD)Testare:$(NC)"
	@echo "  $(GREEN)make test$(NC)       - Rulează testele automate"
	@echo "  $(GREEN)make valgrind$(NC)   - Verifică memory leaks"
	@echo ""
	@echo "$(CYAN)$(BOLD)Utilități:$(NC)"
	@echo "  $(GREEN)make stats$(NC)      - Afișează statistici proiect"
	@echo "  $(GREEN)make clean$(NC)      - Șterge fișierele compilate"
	@echo "  $(GREEN)make help$(NC)       - Afișează acest mesaj"
	@echo ""
	@echo "$(YELLOW)$(BOLD)Exemplu flux de lucru:$(NC)"
	@echo "  1. $(CYAN)make$(NC)          - Compilează proiectul"
	@echo "  2. $(CYAN)make run$(NC)      - Vezi exemplul în acțiune"
	@echo "  3. $(CYAN)make test$(NC)     - Verifică exercițiile"
	@echo "  4. $(CYAN)make valgrind$(NC) - Verifică memory leaks"
	@echo ""
