# ==============================================================================
# Dockerfile - Săptămâna 17: Structuri de Date Probabilistice pentru Big Data
# ==============================================================================
# Algoritmi și Tehnici de Programare - ASE București CSIE
# Imagine Docker pentru compilare, testare și benchmarking
# ==============================================================================

FROM ubuntu:22.04

LABEL maintainer="ASE CSIE - Algoritmi și Tehnici de Programare"
LABEL description="Mediu de dezvoltare C11 pentru structuri de date probabilistice"
LABEL version="1.0"
LABEL week="17"

# ==============================================================================
# METADATA ȘI ARG-uri
# ==============================================================================
ARG DEBIAN_FRONTEND=noninteractive
ARG BUILD_DATE
ARG VERSION=1.0

LABEL org.opencontainers.image.created="${BUILD_DATE}"
LABEL org.opencontainers.image.version="${VERSION}"
LABEL org.opencontainers.image.title="ATP Week 17 - Probabilistic Data Structures"
LABEL org.opencontainers.image.description="Bloom Filter, HyperLogLog, Count-Min Sketch, Skip List"

# ==============================================================================
# INSTALARE PACHETE
# ==============================================================================
RUN apt-get update && apt-get install -y --no-install-recommends \
    # Compilator și tools C
    gcc \
    g++ \
    make \
    gdb \
    # Analiză și debugging
    valgrind \
    cppcheck \
    clang-format \
    # Utilitare sistem
    time \
    bc \
    # Python pentru comparații
    python3 \
    python3-pip \
    python3-venv \
    # Generare date de test
    curl \
    wget \
    # Documentație
    man-db \
    manpages-dev \
    # Utilitare text
    wc \
    sort \
    uniq \
    # Cleanup
    && rm -rf /var/lib/apt/lists/* \
    && apt-get clean

# ==============================================================================
# INSTALARE PACHETE PYTHON PENTRU COMPARAȚIE
# ==============================================================================
RUN pip3 install --no-cache-dir \
    mmh3 \
    pybloom-live \
    datasketch \
    hyperloglog \
    numpy \
    matplotlib

# ==============================================================================
# CONFIGURARE DIRECTOARE
# ==============================================================================
WORKDIR /app

# Structura directorului de lucru
RUN mkdir -p /app/src \
             /app/data \
             /app/tests \
             /app/python_comparison \
             /app/slides \
             /app/teme \
             /app/solution \
             /app/build \
             /app/output \
             /app/benchmarks

# ==============================================================================
# COPIERE FIȘIERE
# ==============================================================================
# Fișiere de build
COPY Makefile /app/
COPY README.md /app/

# Cod sursă
COPY src/*.c /app/src/

# Date de test
COPY data/*.txt /app/data/
COPY data/*.csv /app/data/

# Teste
COPY tests/*.txt /app/tests/

# Comparații Python
COPY python_comparison/*.py /app/python_comparison/

# Prezentări
COPY slides/*.html /app/slides/

# Teme
COPY teme/*.md /app/teme/

# Soluții (opțional, pentru verificare)
COPY solution/*.c /app/solution/

# ==============================================================================
# VARIABILE DE MEDIU
# ==============================================================================
ENV CC=gcc
ENV CFLAGS="-Wall -Wextra -std=c11 -g -pedantic -O2"
ENV LDFLAGS="-lm"

# Culori pentru output (ANSI)
ENV TERM=xterm-256color

# Locale pentru caractere speciale
ENV LANG=C.UTF-8
ENV LC_ALL=C.UTF-8

# ==============================================================================
# SCRIPT DE ENTRY POINT
# ==============================================================================
COPY <<'EOF' /app/docker-entrypoint.sh
#!/bin/bash
set -e

# Culori
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m'

echo -e "${CYAN}╔════════════════════════════════════════════════════════════════╗${NC}"
echo -e "${CYAN}║  ATP Săptămâna 17: Structuri de Date Probabilistice           ║${NC}"
echo -e "${CYAN}║  Bloom Filter | HyperLogLog | Count-Min Sketch | Skip List    ║${NC}"
echo -e "${CYAN}╚════════════════════════════════════════════════════════════════╝${NC}"
echo ""

# Verificare că avem fișierele necesare
if [ ! -f "/app/src/example1.c" ]; then
    echo -e "${RED}[EROARE] Fișierul example1.c lipsește!${NC}"
    exit 1
fi

# Execută comanda dată sau rulează shell
if [ "$1" = "" ]; then
    echo -e "${YELLOW}[INFO] Mod interactiv. Comenzi disponibile:${NC}"
    echo -e "  ${GREEN}make all${NC}      - Compilează tot"
    echo -e "  ${GREEN}make run${NC}      - Rulează exemplul principal"
    echo -e "  ${GREEN}make exercise1${NC} - Rulează exercițiul 1"
    echo -e "  ${GREEN}make exercise2${NC} - Rulează exercițiul 2"
    echo -e "  ${GREEN}make test${NC}     - Rulează testele"
    echo -e "  ${GREEN}make benchmark${NC} - Benchmarking"
    echo -e "  ${GREEN}make help${NC}     - Ajutor complet"
    echo ""
    exec /bin/bash
else
    exec "$@"
fi
EOF

RUN chmod +x /app/docker-entrypoint.sh

# ==============================================================================
# HEALTH CHECK
# ==============================================================================
HEALTHCHECK --interval=30s --timeout=10s --start-period=5s --retries=3 \
    CMD gcc --version > /dev/null && python3 --version > /dev/null || exit 1

# ==============================================================================
# COMPILARE INIȚIALĂ (opțional, pentru pre-warming)
# ==============================================================================
# RUN make all 2>/dev/null || true

# ==============================================================================
# ENTRY POINT
# ==============================================================================
ENTRYPOINT ["/app/docker-entrypoint.sh"]

# ==============================================================================
# PORTURI EXPUSE (pentru server de prezentări, dacă este cazul)
# ==============================================================================
EXPOSE 8080

# ==============================================================================
# VOLUME PENTRU PERSISTENȚĂ
# ==============================================================================
VOLUME ["/app/output", "/app/benchmarks"]

# ==============================================================================
# UTILIZATOR NON-ROOT (securitate)
# ==============================================================================
# RUN useradd -m -s /bin/bash student
# USER student

# ==============================================================================
# COMANDĂ IMPLICITĂ
# ==============================================================================
CMD ["bash"]
