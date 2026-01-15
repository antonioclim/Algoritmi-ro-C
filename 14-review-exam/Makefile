# =============================================================================
# SĂPTĂMÂNA 14: RECAPITULARE AVANSATĂ ȘI OPTIMIZARE - Makefile
# Algoritmi și Tehnici de Programare (ATP) - ASE București
# =============================================================================
#
# Acest Makefile oferă automatizare completă pentru compilare, testare și
# verificarea memoriei. Suportă toate exercițiile și exemplele săptămânii.
#
# Utilizare:
#   make          - Compilează toate targeturile
#   make run      - Rulează exemplul principal
#   make test     - Execută testele automate
#   make valgrind - Verifică memory leaks
#   make clean    - Șterge fișierele generate
#   make help     - Afișează acest meniu
#
# =============================================================================

# Compilator și opțiuni
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -O2
CFLAGS_DEBUG = -Wall -Wextra -std=c11 -g -O0 -DDEBUG
LDFLAGS = -lm

# Directoare
SRC_DIR = src
DATA_DIR = data
TEST_DIR = tests
SOL_DIR = solution
SLIDES_DIR = slides

# Coduri de culori ANSI
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
ROCKET = 🚀
WARNING = ⚠
BOOK = 📚
TEST_ICON = 🧪
MEMORY = 💾
CLEAN_ICON = 🧹

# Targeturi principale
TARGETS = example1 exercise1 exercise2
SOLUTIONS = exercise1_sol exercise2_sol homework1_sol homework2_sol

# =============================================================================
# TARGET PRINCIPAL
# =============================================================================

.PHONY: all clean run test valgrind help demo solutions debug info

all: banner $(TARGETS)
	@echo ""
	@echo "$(GREEN)$(BOLD)$(CHECK) Toate targeturile au fost compilate cu succes!$(NC)"
	@echo ""

banner:
	@echo ""
	@echo "$(CYAN)$(BOLD)╔═══════════════════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(CYAN)$(BOLD)║     SĂPTĂMÂNA 14: RECAPITULARE AVANSATĂ ȘI OPTIMIZARE                     ║$(NC)"
	@echo "$(CYAN)$(BOLD)║     Algoritmi și Tehnici de Programare - ASE București                    ║$(NC)"
	@echo "$(CYAN)$(BOLD)╚═══════════════════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""

# =============================================================================
# COMPILARE EXEMPLE ȘI EXERCIȚII
# =============================================================================

example1: $(SRC_DIR)/example1.c
	@echo "$(CYAN)$(GEAR) Compilare example1...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) example1 compilat cu succes$(NC)"

exercise1: $(SRC_DIR)/exercise1.c
	@echo "$(CYAN)$(GEAR) Compilare exercise1 (Order Management System)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) 2>&1 | head -20 || true
	@if [ -f exercise1 ]; then \
		echo "$(GREEN)  $(CHECK) exercise1 compilat cu succes$(NC)"; \
	else \
		echo "$(YELLOW)  $(WARNING) exercise1 are erori de compilare (TODOs necompletate)$(NC)"; \
	fi

exercise2: $(SRC_DIR)/exercise2.c
	@echo "$(CYAN)$(GEAR) Compilare exercise2 (Social Network Analyzer)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) 2>&1 | head -20 || true
	@if [ -f exercise2 ]; then \
		echo "$(GREEN)  $(CHECK) exercise2 compilat cu succes$(NC)"; \
	else \
		echo "$(YELLOW)  $(WARNING) exercise2 are erori de compilare (TODOs necompletate)$(NC)"; \
	fi

# =============================================================================
# COMPILARE SOLUȚII (pentru instructori)
# =============================================================================

solutions: banner $(SOLUTIONS)
	@echo ""
	@echo "$(GREEN)$(BOLD)$(CHECK) Toate soluțiile au fost compilate!$(NC)"
	@echo ""

exercise1_sol: $(SOL_DIR)/exercise1_sol.c
	@echo "$(MAGENTA)$(GEAR) Compilare soluție exercise1...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) exercise1_sol compilat$(NC)"

exercise2_sol: $(SOL_DIR)/exercise2_sol.c
	@echo "$(MAGENTA)$(GEAR) Compilare soluție exercise2...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) exercise2_sol compilat$(NC)"

homework1_sol: $(SOL_DIR)/homework1_sol.c
	@echo "$(MAGENTA)$(GEAR) Compilare soluție homework1...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) homework1_sol compilat$(NC)"

homework2_sol: $(SOL_DIR)/homework2_sol.c
	@echo "$(MAGENTA)$(GEAR) Compilare soluție homework2...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) homework2_sol compilat$(NC)"

# =============================================================================
# RULARE
# =============================================================================

run: example1
	@echo ""
	@echo "$(YELLOW)$(BOLD)$(ROCKET) Rulare exemplu principal...$(NC)"
	@echo "$(CYAN)────────────────────────────────────────────────────────────$(NC)"
	@./example1
	@echo "$(CYAN)────────────────────────────────────────────────────────────$(NC)"
	@echo ""

run-ex1: exercise1
	@echo ""
	@echo "$(YELLOW)$(BOLD)$(ROCKET) Rulare Exercise 1...$(NC)"
	@echo "$(CYAN)────────────────────────────────────────────────────────────$(NC)"
	@./exercise1 < $(DATA_DIR)/orders.txt 2>&1 || echo "$(RED)$(CROSS) Eroare la rulare$(NC)"
	@echo ""

run-ex2: exercise2
	@echo ""
	@echo "$(YELLOW)$(BOLD)$(ROCKET) Rulare Exercise 2...$(NC)"
	@echo "$(CYAN)────────────────────────────────────────────────────────────$(NC)"
	@./exercise2 $(DATA_DIR)/social_network.csv 2>&1 || echo "$(RED)$(CROSS) Eroare la rulare$(NC)"
	@echo ""

demo: example1
	@echo ""
	@echo "$(MAGENTA)$(BOLD)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(MAGENTA)$(BOLD)║            DEMONSTRAȚIE COMPLETĂ - SĂPTĂMÂNA 14               ║$(NC)"
	@echo "$(MAGENTA)$(BOLD)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@./example1
	@echo ""
	@echo "$(GREEN)$(BOLD)$(CHECK) Demonstrație completă!$(NC)"
	@echo ""

# =============================================================================
# TESTARE AUTOMATĂ
# =============================================================================

test: banner
	@echo "$(YELLOW)$(BOLD)$(TEST_ICON) Rulare teste automate...$(NC)"
	@echo ""
	@PASS=0; FAIL=0; \
	echo "$(CYAN)── Test 1: Order Management System ──$(NC)"; \
	if [ -f exercise1_sol ]; then \
		./exercise1_sol < $(TEST_DIR)/test1_input.txt > /tmp/test1_output.txt 2>&1; \
		if diff -q /tmp/test1_output.txt $(TEST_DIR)/test1_expected.txt > /dev/null 2>&1; then \
			echo "$(GREEN)  $(CHECK) Test 1 PASSED$(NC)"; \
			PASS=$$((PASS+1)); \
		else \
			echo "$(RED)  $(CROSS) Test 1 FAILED$(NC)"; \
			echo "$(YELLOW)    Diferențe:$(NC)"; \
			diff $(TEST_DIR)/test1_expected.txt /tmp/test1_output.txt | head -10; \
			FAIL=$$((FAIL+1)); \
		fi; \
	else \
		echo "$(YELLOW)  $(WARNING) exercise1_sol nu există (compilați mai întâi cu 'make solutions')$(NC)"; \
	fi; \
	echo ""; \
	echo "$(CYAN)── Test 2: Social Network Analyzer ──$(NC)"; \
	if [ -f exercise2_sol ]; then \
		./exercise2_sol $(DATA_DIR)/social_network.csv < $(TEST_DIR)/test2_input.txt > /tmp/test2_output.txt 2>&1; \
		if diff -q /tmp/test2_output.txt $(TEST_DIR)/test2_expected.txt > /dev/null 2>&1; then \
			echo "$(GREEN)  $(CHECK) Test 2 PASSED$(NC)"; \
			PASS=$$((PASS+1)); \
		else \
			echo "$(RED)  $(CROSS) Test 2 FAILED$(NC)"; \
			echo "$(YELLOW)    Diferențe:$(NC)"; \
			diff $(TEST_DIR)/test2_expected.txt /tmp/test2_output.txt | head -10; \
			FAIL=$$((FAIL+1)); \
		fi; \
	else \
		echo "$(YELLOW)  $(WARNING) exercise2_sol nu există$(NC)"; \
	fi; \
	echo ""; \
	echo "$(CYAN)════════════════════════════════════════════════════════════$(NC)"; \
	echo "$(BOLD)Rezultate: $(GREEN)$$PASS passed$(NC) $(RED)$$FAIL failed$(NC)"; \
	echo "$(CYAN)════════════════════════════════════════════════════════════$(NC)"

test-quick: example1
	@echo "$(YELLOW)$(TEST_ICON) Test rapid pe exemplu...$(NC)"
	@./example1 > /dev/null 2>&1 && echo "$(GREEN)$(CHECK) Exemplu rulează corect$(NC)" || echo "$(RED)$(CROSS) Eroare$(NC)"

# =============================================================================
# VERIFICARE MEMORIE CU VALGRIND
# =============================================================================

valgrind: example1
	@echo ""
	@echo "$(YELLOW)$(BOLD)$(MEMORY) Verificare memorie cu Valgrind...$(NC)"
	@echo "$(CYAN)────────────────────────────────────────────────────────────$(NC)"
	@valgrind --leak-check=full \
	          --show-leak-kinds=all \
	          --track-origins=yes \
	          --verbose \
	          --error-exitcode=1 \
	          ./example1 2>&1 | grep -E "(definitely|indirectly|possibly|still reachable|ERROR SUMMARY|All heap)" || true
	@echo "$(CYAN)────────────────────────────────────────────────────────────$(NC)"
	@echo ""

valgrind-full: example1
	@echo ""
	@echo "$(YELLOW)$(BOLD)$(MEMORY) Verificare memorie completă cu Valgrind...$(NC)"
	@echo ""
	@valgrind --leak-check=full \
	          --show-leak-kinds=all \
	          --track-origins=yes \
	          --verbose \
	          ./example1

valgrind-sol: solutions
	@echo ""
	@echo "$(YELLOW)$(BOLD)$(MEMORY) Verificare memorie pentru soluții...$(NC)"
	@echo ""
	@for sol in $(SOLUTIONS); do \
		if [ -f $$sol ]; then \
			echo "$(CYAN)── Verificare $$sol ──$(NC)"; \
			valgrind --leak-check=full --error-exitcode=1 ./$$sol < $(TEST_DIR)/test1_input.txt > /dev/null 2>&1 && \
				echo "$(GREEN)  $(CHECK) $$sol: No memory leaks$(NC)" || \
				echo "$(RED)  $(CROSS) $$sol: Memory leaks detected$(NC)"; \
		fi; \
	done
	@echo ""

# =============================================================================
# DEBUG BUILD
# =============================================================================

debug: CFLAGS = $(CFLAGS_DEBUG)
debug: clean all
	@echo ""
	@echo "$(MAGENTA)$(BOLD)$(GEAR) Build pentru debugging complet!$(NC)"
	@echo "$(YELLOW)  Utilizare: gdb ./example1$(NC)"
	@echo ""

# =============================================================================
# CURĂȚARE
# =============================================================================

clean:
	@echo ""
	@echo "$(YELLOW)$(CLEAN_ICON) Curățare fișiere generate...$(NC)"
	@rm -f $(TARGETS) $(SOLUTIONS)
	@rm -f *.o *.out
	@rm -f /tmp/test*_output.txt
	@echo "$(GREEN)$(CHECK) Curățare completă!$(NC)"
	@echo ""

clean-all: clean
	@echo "$(YELLOW)$(CLEAN_ICON) Curățare totală (inclusiv backup-uri)...$(NC)"
	@rm -f *~ *.bak
	@rm -f $(SRC_DIR)/*~ $(SOL_DIR)/*~
	@echo "$(GREEN)$(CHECK) Curățare totală completă!$(NC)"

# =============================================================================
# INFORMAȚII ȘI HELP
# =============================================================================

info:
	@echo ""
	@echo "$(CYAN)$(BOLD)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(CYAN)$(BOLD)║              INFORMAȚII SĂPTĂMÂNA 14                           ║$(NC)"
	@echo "$(CYAN)$(BOLD)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(BOLD)Temă:$(NC) Recapitulare Avansată și Optimizare"
	@echo ""
	@echo "$(BOLD)Structuri de date acoperite:$(NC)"
	@echo "  • Arrays, Linked Lists, Stacks, Queues"
	@echo "  • Binary Trees, BST, AVL Trees"
	@echo "  • Heaps, Hash Tables, Graphs"
	@echo ""
	@echo "$(BOLD)Algoritmi acoperiți:$(NC)"
	@echo "  • Sortare: QuickSort, MergeSort, HeapSort"
	@echo "  • Căutare: Binary Search, BFS, DFS"
	@echo "  • Grafuri: Dijkstra, Bellman-Ford"
	@echo ""
	@echo "$(BOLD)Fișiere:$(NC)"
	@echo "  $(CYAN)src/example1.c$(NC)     - Exemplu complet (demonstrație)"
	@echo "  $(CYAN)src/exercise1.c$(NC)    - Order Management System (12 TODOs)"
	@echo "  $(CYAN)src/exercise2.c$(NC)    - Social Network Analyzer (15 TODOs)"
	@echo ""
	@echo "$(BOLD)Prezentări:$(NC)"
	@echo "  $(CYAN)slides/presentation-week14.html$(NC)      - Prezentare principală"
	@echo "  $(CYAN)slides/presentation-comparativ.html$(NC)  - Comparație limbaje"
	@echo ""

help:
	@echo ""
	@echo "$(CYAN)$(BOLD)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(CYAN)$(BOLD)║                    COMENZI DISPONIBILE                         ║$(NC)"
	@echo "$(CYAN)$(BOLD)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(BOLD)Compilare:$(NC)"
	@echo "  $(GREEN)make$(NC)              Compilează toate exercițiile"
	@echo "  $(GREEN)make solutions$(NC)    Compilează soluțiile (instructor)"
	@echo "  $(GREEN)make debug$(NC)        Build cu simboluri pentru debugging"
	@echo ""
	@echo "$(BOLD)Rulare:$(NC)"
	@echo "  $(GREEN)make run$(NC)          Rulează exemplul principal"
	@echo "  $(GREEN)make run-ex1$(NC)      Rulează Exercise 1"
	@echo "  $(GREEN)make run-ex2$(NC)      Rulează Exercise 2"
	@echo "  $(GREEN)make demo$(NC)         Demonstrație completă"
	@echo ""
	@echo "$(BOLD)Testare:$(NC)"
	@echo "  $(GREEN)make test$(NC)         Rulează testele automate"
	@echo "  $(GREEN)make test-quick$(NC)   Test rapid pe exemplu"
	@echo ""
	@echo "$(BOLD)Debugging:$(NC)"
	@echo "  $(GREEN)make valgrind$(NC)     Verificare scurtă memory leaks"
	@echo "  $(GREEN)make valgrind-full$(NC) Verificare completă Valgrind"
	@echo "  $(GREEN)make valgrind-sol$(NC) Verificare soluții"
	@echo ""
	@echo "$(BOLD)Utilități:$(NC)"
	@echo "  $(GREEN)make clean$(NC)        Șterge executabilele"
	@echo "  $(GREEN)make clean-all$(NC)    Șterge toate fișierele generate"
	@echo "  $(GREEN)make info$(NC)         Informații despre săptămâna curentă"
	@echo "  $(GREEN)make help$(NC)         Afișează acest meniu"
	@echo ""
	@echo "$(BOLD)Exemplu flux de lucru:$(NC)"
	@echo "  1. $(CYAN)make$(NC)              # Compilează"
	@echo "  2. $(CYAN)make run$(NC)          # Rulează exemplul"
	@echo "  3. $(CYAN)make valgrind$(NC)     # Verifică memoria"
	@echo "  4. $(CYAN)make test$(NC)         # Testează soluțiile"
	@echo ""

# =============================================================================
# REGULI SPECIALE
# =============================================================================

# Previne ștergerea fișierelor intermediare
.PRECIOUS: $(TARGETS) $(SOLUTIONS)

# Declară targeturile care nu sunt fișiere
.PHONY: all clean run test valgrind help demo solutions debug info \
        run-ex1 run-ex2 test-quick valgrind-full valgrind-sol \
        clean-all banner
