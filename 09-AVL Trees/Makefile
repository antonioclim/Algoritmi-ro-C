# =============================================================================
# SĂPTĂMÂNA 09: ARBORI AVL - Makefile
# =============================================================================
# Algoritmi și Tehnici de Programare
# Academia de Studii Economice București - CSIE
# =============================================================================

# Compilator și opțiuni
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -g -O2
LDFLAGS = -lm

# Directoare
SRC_DIR = src
DATA_DIR = data
TEST_DIR = tests
SOL_DIR = solution

# Culori pentru output (ANSI escape codes)
RED = \033[0;31m
GREEN = \033[0;32m
YELLOW = \033[0;33m
BLUE = \033[0;34m
MAGENTA = \033[0;35m
CYAN = \033[0;36m
WHITE = \033[0;37m
BOLD = \033[1m
NC = \033[0m

# Simboluri
CHECK = ✓
CROSS = ✗
ARROW = →
GEAR = ⚙

# Targeturi executabile
TARGETS = example1 exercise1 exercise2
SOLUTIONS = exercise1_sol exercise2_sol homework1_sol homework2_sol

# =============================================================================
# REGULI PRINCIPALE
# =============================================================================

.PHONY: all clean run test valgrind help solutions debug info

all: banner $(TARGETS)
	@echo ""
	@echo "$(GREEN)$(BOLD)$(CHECK) Toate targeturile au fost compilate cu succes!$(NC)"
	@echo ""

banner:
	@echo ""
	@echo "$(CYAN)$(BOLD)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(CYAN)$(BOLD)║     SĂPTĂMÂNA 09: ARBORI AVL - Build System                   ║$(NC)"
	@echo "$(CYAN)$(BOLD)║     Algoritmi și Tehnici de Programare                        ║$(NC)"
	@echo "$(CYAN)$(BOLD)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""

# =============================================================================
# COMPILARE EXEMPLE ȘI EXERCIȚII
# =============================================================================

example1: $(SRC_DIR)/example1.c
	@echo "$(YELLOW)$(GEAR) Compilare $(BOLD)example1$(NC)$(YELLOW)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) example1 compilat$(NC)"

exercise1: $(SRC_DIR)/exercise1.c
	@echo "$(YELLOW)$(GEAR) Compilare $(BOLD)exercise1$(NC)$(YELLOW)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) exercise1 compilat$(NC)"

exercise2: $(SRC_DIR)/exercise2.c
	@echo "$(YELLOW)$(GEAR) Compilare $(BOLD)exercise2$(NC)$(YELLOW)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) exercise2 compilat$(NC)"

# =============================================================================
# COMPILARE SOLUȚII (doar pentru instructori)
# =============================================================================

solutions: $(SOLUTIONS)
	@echo ""
	@echo "$(MAGENTA)$(CHECK) Toate soluțiile au fost compilate$(NC)"

exercise1_sol: $(SOL_DIR)/exercise1_sol.c
	@echo "$(MAGENTA)$(GEAR) Compilare soluție exercise1...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

exercise2_sol: $(SOL_DIR)/exercise2_sol.c
	@echo "$(MAGENTA)$(GEAR) Compilare soluție exercise2...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

homework1_sol: $(SOL_DIR)/homework1_sol.c
	@echo "$(MAGENTA)$(GEAR) Compilare soluție homework1...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

homework2_sol: $(SOL_DIR)/homework2_sol.c
	@echo "$(MAGENTA)$(GEAR) Compilare soluție homework2...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# =============================================================================
# EXECUTARE
# =============================================================================

run: all
	@echo ""
	@echo "$(CYAN)$(BOLD)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(CYAN)$(BOLD)     RULARE EXEMPLU DEMONSTRATIV                               $(NC)"
	@echo "$(CYAN)$(BOLD)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@./example1

run-ex1: exercise1
	@echo ""
	@echo "$(BLUE)$(ARROW) Rulare Exercise 1...$(NC)"
	@echo ""
	@./exercise1

run-ex2: exercise2
	@echo ""
	@echo "$(BLUE)$(ARROW) Rulare Exercise 2...$(NC)"
	@echo ""
	@./exercise2

# =============================================================================
# TESTARE AUTOMATĂ
# =============================================================================

test: exercise1 exercise2
	@echo ""
	@echo "$(YELLOW)$(BOLD)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(YELLOW)$(BOLD)     RULARE TESTE AUTOMATE                                     $(NC)"
	@echo "$(YELLOW)$(BOLD)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@echo "$(CYAN)Test 1: Exercițiul 1 - AVL de bază$(NC)"
	@if ./exercise1 < $(TEST_DIR)/test1_input.txt > /tmp/test1_output.txt 2>&1; then \
		if diff -q /tmp/test1_output.txt $(TEST_DIR)/test1_expected.txt > /dev/null 2>&1; then \
			echo "$(GREEN)  $(CHECK) Test 1 PASSED$(NC)"; \
		else \
			echo "$(RED)  $(CROSS) Test 1 FAILED - output diferit$(NC)"; \
			echo "$(YELLOW)  Diferențe:$(NC)"; \
			diff /tmp/test1_output.txt $(TEST_DIR)/test1_expected.txt || true; \
		fi \
	else \
		echo "$(RED)  $(CROSS) Test 1 FAILED - eroare la execuție$(NC)"; \
	fi
	@echo ""
	@echo "$(CYAN)Test 2: Exercițiul 2 - AVL complet$(NC)"
	@if ./exercise2 < $(TEST_DIR)/test2_input.txt > /tmp/test2_output.txt 2>&1; then \
		if diff -q /tmp/test2_output.txt $(TEST_DIR)/test2_expected.txt > /dev/null 2>&1; then \
			echo "$(GREEN)  $(CHECK) Test 2 PASSED$(NC)"; \
		else \
			echo "$(RED)  $(CROSS) Test 2 FAILED - output diferit$(NC)"; \
			echo "$(YELLOW)  Diferențe:$(NC)"; \
			diff /tmp/test2_output.txt $(TEST_DIR)/test2_expected.txt || true; \
		fi \
	else \
		echo "$(RED)  $(CROSS) Test 2 FAILED - eroare la execuție$(NC)"; \
	fi
	@echo ""

# =============================================================================
# VERIFICARE MEMORIE CU VALGRIND
# =============================================================================

valgrind: all
	@echo ""
	@echo "$(MAGENTA)$(BOLD)═══════════════════════════════════════════════════════════════$(NC)"
	@echo "$(MAGENTA)$(BOLD)     VERIFICARE MEMORIE CU VALGRIND                            $(NC)"
	@echo "$(MAGENTA)$(BOLD)═══════════════════════════════════════════════════════════════$(NC)"
	@echo ""
	@echo "$(CYAN)Verificare example1...$(NC)"
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
		--error-exitcode=1 ./example1 2>&1 | tail -20 || true
	@echo ""

valgrind-ex1: exercise1
	@echo "$(CYAN)Verificare exercise1...$(NC)"
	@valgrind --leak-check=full --show-leak-kinds=all ./exercise1 < $(TEST_DIR)/test1_input.txt

valgrind-ex2: exercise2
	@echo "$(CYAN)Verificare exercise2...$(NC)"
	@valgrind --leak-check=full --show-leak-kinds=all ./exercise2 < $(TEST_DIR)/test2_input.txt

# =============================================================================
# DEBUGGING
# =============================================================================

debug: CFLAGS += -DDEBUG -g3 -O0
debug: clean all
	@echo "$(YELLOW)$(CHECK) Compilat în mod DEBUG$(NC)"

# =============================================================================
# CURĂȚARE
# =============================================================================

clean:
	@echo ""
	@echo "$(YELLOW)$(GEAR) Curățare fișiere generate...$(NC)"
	@rm -f $(TARGETS) $(SOLUTIONS)
	@rm -f *.o *.out
	@rm -f /tmp/test*_output.txt
	@rm -f core vgcore.*
	@echo "$(GREEN)$(CHECK) Curățare completă$(NC)"
	@echo ""

# =============================================================================
# INFORMAȚII
# =============================================================================

info:
	@echo ""
	@echo "$(BLUE)$(BOLD)Informații proiect:$(NC)"
	@echo "  Săptămâna: 09 - Arbori AVL"
	@echo "  Compilator: $(CC)"
	@echo "  Flags: $(CFLAGS)"
	@echo "  Fișiere sursă:"
	@ls -la $(SRC_DIR)/*.c 2>/dev/null || echo "    (niciun fișier)"
	@echo ""

# =============================================================================
# HELP
# =============================================================================

help:
	@echo ""
	@echo "$(CYAN)$(BOLD)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(CYAN)$(BOLD)║     SĂPTĂMÂNA 09: ARBORI AVL - Comenzi Disponibile            ║$(NC)"
	@echo "$(CYAN)$(BOLD)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(BOLD)Utilizare:$(NC) make [target]"
	@echo ""
	@echo "$(YELLOW)Targeturi principale:$(NC)"
	@echo "  $(GREEN)make$(NC)            - Compilează toate executabilele"
	@echo "  $(GREEN)make run$(NC)        - Rulează exemplul demonstrativ"
	@echo "  $(GREEN)make test$(NC)       - Rulează testele automate"
	@echo "  $(GREEN)make clean$(NC)      - Șterge fișierele generate"
	@echo ""
	@echo "$(YELLOW)Compilare individuală:$(NC)"
	@echo "  $(GREEN)make example1$(NC)   - Compilează exemplul complet"
	@echo "  $(GREEN)make exercise1$(NC)  - Compilează exercițiul 1"
	@echo "  $(GREEN)make exercise2$(NC)  - Compilează exercițiul 2"
	@echo ""
	@echo "$(YELLOW)Rulare individuală:$(NC)"
	@echo "  $(GREEN)make run-ex1$(NC)    - Rulează exercițiul 1"
	@echo "  $(GREEN)make run-ex2$(NC)    - Rulează exercițiul 2"
	@echo ""
	@echo "$(YELLOW)Verificare memorie:$(NC)"
	@echo "  $(GREEN)make valgrind$(NC)   - Verifică memory leaks cu Valgrind"
	@echo "  $(GREEN)make valgrind-ex1$(NC) - Valgrind pentru exercițiul 1"
	@echo "  $(GREEN)make valgrind-ex2$(NC) - Valgrind pentru exercițiul 2"
	@echo ""
	@echo "$(YELLOW)Debugging și info:$(NC)"
	@echo "  $(GREEN)make debug$(NC)      - Compilează cu simboluri debug"
	@echo "  $(GREEN)make info$(NC)       - Afișează informații proiect"
	@echo "  $(GREEN)make help$(NC)       - Afișează acest mesaj"
	@echo ""
	@echo "$(YELLOW)Soluții (doar instructori):$(NC)"
	@echo "  $(GREEN)make solutions$(NC)  - Compilează toate soluțiile"
	@echo ""

# =============================================================================
# DEPENDENȚE AUTOMATE
# =============================================================================

# Asigură că directoarele există înainte de compilare
$(shell mkdir -p $(SRC_DIR) $(DATA_DIR) $(TEST_DIR) $(SOL_DIR))
