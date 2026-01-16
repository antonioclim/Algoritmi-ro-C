# =============================================================================
# SĂPTĂMÂNA 20: PROGRAMARE PARALELĂ ȘI CONCURENTĂ - Dockerfile
# =============================================================================
# Curs: Algoritmi și Tehnici de Programare (ATP)
# Academia de Studii Economice București - CSIE
# =============================================================================

FROM ubuntu:22.04

LABEL maintainer="ATP Course - ASE CSIE"
LABEL description="Mediu de dezvoltare pentru programare paralelă și concurentă în C"
LABEL version="1.0"

# Evitare prompt-uri interactive
ENV DEBIAN_FRONTEND=noninteractive

# Instalare pachete necesare
RUN apt-get update && apt-get install -y \
    # Compilatoare și build tools
    gcc \
    g++ \
    make \
    cmake \
    # Debugging și profiling
    gdb \
    valgrind \
    linux-tools-generic \
    # Utilități
    vim \
    nano \
    less \
    tree \
    htop \
    # Python pentru vizualizări
    python3 \
    python3-pip \
    python3-matplotlib \
    python3-numpy \
    # Documentație
    man-db \
    manpages-posix-dev \
    # Curățare cache
    && rm -rf /var/lib/apt/lists/*

# Instalare pachete Python pentru benchmark visualization
RUN pip3 install --no-cache-dir \
    matplotlib \
    numpy \
    pandas

# Setare director de lucru
WORKDIR /workspace

# Copiere fișiere proiect
COPY . /workspace/

# Setare permisiuni
RUN chmod -R 755 /workspace

# Compilare proiect
RUN make all 2>/dev/null || true

# Variabile de mediu pentru threading
ENV OMP_NUM_THREADS=4
ENV PTHREAD_STACK_SIZE=8388608

# Mesaj de întâmpinare
RUN echo '#!/bin/bash' > /entrypoint.sh && \
    echo 'echo ""' >> /entrypoint.sh && \
    echo 'echo "╔═══════════════════════════════════════════════════════════════════════════════╗"' >> /entrypoint.sh && \
    echo 'echo "║  🧵 SĂPTĂMÂNA 20: PROGRAMARE PARALELĂ ȘI CONCURENTĂ                           ║"' >> /entrypoint.sh && \
    echo 'echo "║     Mediu de dezvoltare Docker                                                ║"' >> /entrypoint.sh && \
    echo 'echo "╠═══════════════════════════════════════════════════════════════════════════════╣"' >> /entrypoint.sh && \
    echo 'echo "║  Comenzi utile:                                                               ║"' >> /entrypoint.sh && \
    echo 'echo "║    make              - Compilează proiectul                                   ║"' >> /entrypoint.sh && \
    echo 'echo "║    make run          - Rulează exemplul demonstrativ                          ║"' >> /entrypoint.sh && \
    echo 'echo "║    make demo         - Demonstrație completă                                  ║"' >> /entrypoint.sh && \
    echo 'echo "║    make benchmark    - Rulează benchmark paralel vs secvențial               ║"' >> /entrypoint.sh && \
    echo 'echo "║    make helgrind     - Verifică race conditions                              ║"' >> /entrypoint.sh && \
    echo 'echo "║    make help         - Ajutor complet                                        ║"' >> /entrypoint.sh && \
    echo 'echo "╚═══════════════════════════════════════════════════════════════════════════════╝"' >> /entrypoint.sh && \
    echo 'echo ""' >> /entrypoint.sh && \
    echo 'exec "$@"' >> /entrypoint.sh && \
    chmod +x /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]
CMD ["/bin/bash"]
