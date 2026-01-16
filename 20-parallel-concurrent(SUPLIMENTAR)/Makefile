# =============================================================================
# SĂPTĂMÂNA 20: PROGRAMARE PARALELĂ ȘI CONCURENTĂ - Makefile
# =============================================================================
# Curs: Algoritmi și Tehnici de Programare (ATP)
# Academia de Studii Economice București - CSIE
# =============================================================================

# Compiler și flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -O2 -pedantic
LDFLAGS = -lpthread -lm
DEBUG_FLAGS = -g -O0 -DDEBUG -fsanitize=thread
RELEASE_FLAGS = -O3 -DNDEBUG

# Directoare
SRC_DIR = src
DATA_DIR = data
TEST_DIR = tests
SOL_DIR = solution
BENCH_DIR = benchmarks
BUILD_DIR = build

# Culori ANSI pentru output
GREEN = \033[0;32m
RED = \033[0;31m
YELLOW = \033[0;33m
CYAN = \033[0;36m
MAGENTA = \033[0;35m
BLUE = \033[0;34m
WHITE = \033[1;37m
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
THREAD_ICON = 🧵
BENCH_ICON = 📊
LOCK_ICON = 🔒

# Targets principale
TARGETS = example1 exercise1 exercise2

# Benchmarks
BENCHMARKS = benchmark_threads benchmark_sequential

# Solutions (pentru instructor)
SOLUTIONS = exercise1_sol exercise2_sol homework1_sol homework2_sol

.PHONY: all clean run test valgrind helgrind help debug release solutions
.PHONY: demo benchmark stats exercise1 exercise2 run-exercise1 run-exercise2

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
	@echo "$(BLUE)$(BOLD)╔═══════════════════════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(BLUE)$(BOLD)║  $(THREAD_ICON)  SĂPTĂMÂNA 20: PROGRAMARE PARALELĂ ȘI CONCURENTĂ                       ║$(NC)"
	@echo "$(BLUE)$(BOLD)║     Threads, Mutex, Semaphores, Atomics, Lock-Free                            ║$(NC)"
	@echo "$(BLUE)$(BOLD)╚═══════════════════════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""

# =============================================================================
# COMPILARE TARGETS PRINCIPALE
# =============================================================================

example1: $(SRC_DIR)/example1.c
	@echo "$(CYAN)$(GEAR) Compilare example1.c (demonstrație completă)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) example1 compilat cu succes$(NC)"

exercise1: $(SRC_DIR)/exercise1.c
	@echo "$(CYAN)$(GEAR) Compilare exercise1.c (Producer-Consumer)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) exercise1 compilat$(NC)"

exercise2: $(SRC_DIR)/exercise2.c
	@echo "$(CYAN)$(GEAR) Compilare exercise2.c (Parallel Quick Sort)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) exercise2 compilat$(NC)"

# =============================================================================
# COMPILARE BENCHMARKS
# =============================================================================

benchmarks: $(BENCHMARKS)
	@echo ""
	@echo "$(MAGENTA)$(BOLD)$(CHECK) Benchmark-uri compilate!$(NC)"

benchmark_threads: $(BENCH_DIR)/benchmark_threads.c
	@echo "$(MAGENTA)$(GEAR) Compilare benchmark_threads.c...$(NC)"
	@$(CC) $(CFLAGS) $(RELEASE_FLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) benchmark_threads compilat$(NC)"

benchmark_sequential: $(BENCH_DIR)/benchmark_sequential.c
	@echo "$(MAGENTA)$(GEAR) Compilare benchmark_sequential.c...$(NC)"
	@$(CC) $(CFLAGS) $(RELEASE_FLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) benchmark_sequential compilat$(NC)"

# =============================================================================
# COMPILARE SOLUȚII (DOAR PENTRU INSTRUCTOR)
# =============================================================================

solutions: banner $(SOLUTIONS)
	@echo ""
	@echo "$(MAGENTA)$(BOLD)$(LOCK_ICON) Toate soluțiile au fost compilate!$(NC)"
	@echo ""

exercise1_sol: $(SOL_DIR)/exercise1_sol.c
	@echo "$(MAGENTA)$(GEAR) Compilare exercise1_sol.c...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) exercise1_sol compilat$(NC)"

exercise2_sol: $(SOL_DIR)/exercise2_sol.c
	@echo "$(MAGENTA)$(GEAR) Compilare exercise2_sol.c...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) exercise2_sol compilat$(NC)"

homework1_sol: $(SOL_DIR)/homework1_sol.c
	@echo "$(MAGENTA)$(GEAR) Compilare homework1_sol.c...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) homework1_sol compilat$(NC)"

homework2_sol: $(SOL_DIR)/homework2_sol.c
	@echo "$(MAGENTA)$(GEAR) Compilare homework2_sol.c...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "$(GREEN)  $(CHECK) homework2_sol compilat$(NC)"

# =============================================================================
# EXECUȚIE
# =============================================================================

run: all
	@echo ""
	@echo "$(YELLOW)$(BOLD)$(ROCKET) Rulare exemplu demonstrativ complet...$(NC)"
	@echo "$(YELLOW)─────────────────────────────────────────────────────$(NC)"
	@./example1
	@echo ""
	@echo "$(GREEN)$(CHECK) Execuție finalizată$(NC)"

run-exercise1: exercise1
	@echo ""
	@echo "$(YELLOW)$(BOLD)$(ROCKET) Rulare exercițiul 1 (Producer-Consumer)...$(NC)"
	@echo "$(YELLOW)─────────────────────────────────────────────────────$(NC)"
	@./exercise1 $(DATA_DIR)/log_entries.txt 2 4
	@echo ""

run-exercise2: exercise2
	@echo ""
	@echo "$(YELLOW)$(BOLD)$(ROCKET) Rulare exercițiul 2 (Parallel Quick Sort)...$(NC)"
	@echo "$(YELLOW)─────────────────────────────────────────────────────$(NC)"
	@./exercise2 $(DATA_DIR)/large_numbers.txt 4
	@echo ""

demo: all
	@echo ""
	@echo "$(YELLOW)$(BOLD)$(ROCKET) Demonstrație completă programare paralelă...$(NC)"
	@echo ""
	@echo "$(CYAN)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(CYAN)║              EXEMPLU COMPLET - TOATE CONCEPTELE               ║$(NC)"
	@echo "$(CYAN)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@./example1
	@echo ""
	@echo "$(CYAN)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(CYAN)║          EXERCIȚIUL 1: PRODUCER-CONSUMER PATTERN              ║$(NC)"
	@echo "$(CYAN)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@./exercise1 $(DATA_DIR)/log_entries.txt 2 4 2>/dev/null || ./exercise1
	@echo ""
	@echo "$(CYAN)╔═══════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(CYAN)║           EXERCIȚIUL 2: PARALLEL QUICK SORT                   ║$(NC)"
	@echo "$(CYAN)╚═══════════════════════════════════════════════════════════════╝$(NC)"
	@./exercise2 $(DATA_DIR)/large_numbers.txt 4 2>/dev/null || ./exercise2
	@echo ""
	@echo "$(GREEN)$(BOLD)$(CHECK) Demonstrație completă finalizată!$(NC)"

# =============================================================================
# BENCHMARK
# =============================================================================

benchmark: benchmarks
	@echo ""
	@echo "$(YELLOW)$(BOLD)$(BENCH_ICON) Rulare benchmark paralel vs secvențial...$(NC)"
	@echo "$(YELLOW)─────────────────────────────────────────────────────$(NC)"
	@echo ""
	@echo "$(CYAN)Benchmark Secvențial:$(NC)"
	@./benchmark_sequential
	@echo ""
	@echo "$(CYAN)Benchmark Paralel (variind numărul de thread-uri):$(NC)"
	@for t in 1 2 4 8; do \
		echo ""; \
		echo "$(YELLOW)  Threads: $$t$(NC)"; \
		./benchmark_threads $$t; \
	done
	@echo ""
	@echo "$(GREEN)$(CHECK) Benchmark finalizat. Compară timpii pentru speedup!$(NC)"

# =============================================================================
# TESTE AUTOMATE
# =============================================================================

test: all
	@echo ""
	@echo "$(YELLOW)$(BOLD)$(TEST_ICON) Rulare teste automate...$(NC)"
	@echo ""
	@echo "$(CYAN)Test 1: Producer-Consumer correctness$(NC)"
	@./exercise1 2>/dev/null && echo "$(GREEN)  $(CHECK) Test 1 PASSED$(NC)" || echo "$(RED)  $(CROSS) Test 1 FAILED$(NC)"
	@echo ""
	@echo "$(CYAN)Test 2: Parallel Sort correctness$(NC)"
	@./exercise2 2>/dev/null && echo "$(GREEN)  $(CHECK) Test 2 PASSED$(NC)" || echo "$(RED)  $(CROSS) Test 2 FAILED$(NC)"
	@echo ""
	@echo "$(GREEN)$(CHECK) Teste finalizate$(NC)"

# =============================================================================
# VERIFICARE MEMORIE ȘI THREAD SAFETY
# =============================================================================

valgrind: all
	@echo ""
	@echo "$(YELLOW)$(BOLD)$(MEM_ICON) Verificare memory leaks cu Valgrind...$(NC)"
	@echo ""
	@echo "$(CYAN)Verificare example1:$(NC)"
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
		./example1 2>&1 | grep -E "(ERROR SUMMARY|LEAK SUMMARY|definitely|indirectly)" || true
	@echo ""
	@echo "$(GREEN)$(CHECK) Verificare memorie finalizată$(NC)"

helgrind: all
	@echo ""
	@echo "$(YELLOW)$(BOLD)$(LOCK_ICON) Verificare race conditions cu Helgrind...$(NC)"
	@echo ""
	@echo "$(CYAN)Verificare example1:$(NC)"
	@valgrind --tool=helgrind ./example1 2>&1 | grep -E "(ERROR SUMMARY|data race|lock order)" || true
	@echo ""
	@echo "$(GREEN)$(CHECK) Verificare thread safety finalizată$(NC)"

tsan: CFLAGS += -fsanitize=thread
tsan: clean all
	@echo ""
	@echo "$(MAGENTA)$(BOLD)$(LOCK_ICON) Build cu ThreadSanitizer completat$(NC)"
	@echo "$(CYAN)Rulează executabilele pentru detectare race conditions$(NC)"

# =============================================================================
# DEBUGGING
# =============================================================================

debug: CFLAGS += $(DEBUG_FLAGS)
debug: clean all
	@echo ""
	@echo "$(MAGENTA)$(BOLD)$(GEAR) Build debug completat$(NC)"
	@echo "$(CYAN)Folosește: gdb ./example1$(NC)"
	@echo "$(CYAN)Comenzi GDB utile pentru threads:$(NC)"
	@echo "  $(YELLOW)info threads$(NC)     - Lista thread-urilor"
	@echo "  $(YELLOW)thread N$(NC)         - Switch la thread N"
	@echo "  $(YELLOW)thread apply all bt$(NC) - Backtrace toate thread-urile"
	@echo "  $(YELLOW)set scheduler-locking on$(NC) - Lock current thread"

release: CFLAGS += $(RELEASE_FLAGS)
release: clean all benchmarks
	@echo ""
	@echo "$(GREEN)$(BOLD)$(ROCKET) Build release optimizat completat!$(NC)"

# =============================================================================
# STATISTICI
# =============================================================================

stats:
	@echo ""
	@echo "$(CYAN)$(BOLD)$(BENCH_ICON) Statistici Proiect - Săptămâna 20$(NC)"
	@echo "$(CYAN)─────────────────────────────────────────────────────$(NC)"
	@echo ""
	@echo "$(YELLOW)Fișiere sursă:$(NC)"
	@wc -l $(SRC_DIR)/*.c 2>/dev/null || echo "  Nu există fișiere sursă"
	@echo ""
	@echo "$(YELLOW)Benchmark-uri:$(NC)"
	@wc -l $(BENCH_DIR)/*.c 2>/dev/null || echo "  Nu există benchmark-uri"
	@echo ""
	@echo "$(YELLOW)Soluții:$(NC)"
	@wc -l $(SOL_DIR)/*.c 2>/dev/null || echo "  Nu există soluții"
	@echo ""
	@echo "$(YELLOW)Total linii de cod C:$(NC)"
	@find . -name "*.c" -exec cat {} \; 2>/dev/null | wc -l
	@echo ""
	@echo "$(YELLOW)Dimensiune directoare:$(NC)"
	@du -sh $(SRC_DIR) $(SOL_DIR) slides teme $(BENCH_DIR) 2>/dev/null || true

# =============================================================================
# CURĂȚARE
# =============================================================================

clean:
	@echo ""
	@echo "$(YELLOW)$(BOLD)$(BROOM) Curățare artefacte...$(NC)"
	@rm -f $(TARGETS) $(SOLUTIONS) $(BENCHMARKS)
	@rm -f *.o *.out core
	@rm -rf $(BUILD_DIR)
	@rm -f /tmp/test*_output.txt
	@rm -f *.log *.dat
	@echo "$(GREEN)$(CHECK) Curățare completă$(NC)"
	@echo ""

# =============================================================================
# AJUTOR
# =============================================================================

help:
	@echo ""
	@echo "$(BLUE)$(BOLD)╔═══════════════════════════════════════════════════════════════════════════════╗$(NC)"
	@echo "$(BLUE)$(BOLD)║                         COMENZI DISPONIBILE                                   ║$(NC)"
	@echo "$(BLUE)$(BOLD)║                   $(THREAD_ICON) Programare Paralelă și Concurentă                     ║$(NC)"
	@echo "$(BLUE)$(BOLD)╚═══════════════════════════════════════════════════════════════════════════════╝$(NC)"
	@echo ""
	@echo "$(CYAN)$(BOLD)Compilare:$(NC)"
	@echo "  $(GREEN)make$(NC)              - Compilează toate targeturile principale"
	@echo "  $(GREEN)make solutions$(NC)    - Compilează soluțiile (instructor)"
	@echo "  $(GREEN)make benchmarks$(NC)   - Compilează benchmark-urile"
	@echo "  $(GREEN)make debug$(NC)        - Build cu simboluri debug + ThreadSanitizer"
	@echo "  $(GREEN)make release$(NC)      - Build optimizat pentru performanță"
	@echo ""
	@echo "$(CYAN)$(BOLD)Execuție:$(NC)"
	@echo "  $(GREEN)make run$(NC)          - Rulează exemplul demonstrativ complet"
	@echo "  $(GREEN)make demo$(NC)         - Rulează demonstrația completă (toate)"
	@echo "  $(GREEN)make run-exercise1$(NC) - Rulează Producer-Consumer"
	@echo "  $(GREEN)make run-exercise2$(NC) - Rulează Parallel Quick Sort"
	@echo "  $(GREEN)make benchmark$(NC)    - Rulează comparație paralel vs secvențial"
	@echo ""
	@echo "$(CYAN)$(BOLD)Testare și Verificare:$(NC)"
	@echo "  $(GREEN)make test$(NC)         - Rulează testele automate"
	@echo "  $(GREEN)make valgrind$(NC)     - Verifică memory leaks"
	@echo "  $(GREEN)make helgrind$(NC)     - Verifică race conditions (Valgrind)"
	@echo "  $(GREEN)make tsan$(NC)         - Build cu ThreadSanitizer"
	@echo ""
	@echo "$(CYAN)$(BOLD)Utilități:$(NC)"
	@echo "  $(GREEN)make stats$(NC)        - Afișează statistici proiect"
	@echo "  $(GREEN)make clean$(NC)        - Șterge fișierele compilate"
	@echo "  $(GREEN)make help$(NC)         - Afișează acest mesaj"
	@echo ""
	@echo "$(YELLOW)$(BOLD)Flux de lucru recomandat:$(NC)"
	@echo "  1. $(CYAN)make$(NC)            - Compilează proiectul"
	@echo "  2. $(CYAN)make run$(NC)        - Vezi exemplul demonstrativ"
	@echo "  3. $(CYAN)make test$(NC)       - Verifică corectitudinea"
	@echo "  4. $(CYAN)make helgrind$(NC)   - Verifică thread safety"
	@echo "  5. $(CYAN)make benchmark$(NC)  - Măsoară speedup-ul"
	@echo ""
	@echo "$(WHITE)$(BOLD)Concepte acoperite:$(NC)"
	@echo "  $(THREAD_ICON)  POSIX Threads (pthread)"
	@echo "  $(LOCK_ICON)  Mutex, Semaphore, Condition Variables"
	@echo "  $(BENCH_ICON)  Producer-Consumer, Thread Pool"
	@echo "  $(ROCKET) Parallel Sort, Lock-Free Data Structures"
	@echo ""
