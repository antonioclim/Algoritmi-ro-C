# =============================================================================
# SĂPTĂMÂNA 18: FUNDAMENTE MACHINE LEARNING ÎN C
# Dockerfile pentru mediu de dezvoltare și testare
# =============================================================================

FROM ubuntu:24.04

LABEL maintainer="ATP Course - ASE CSIE"
LABEL description="Machine Learning Fundamentals in C - Development Environment"
LABEL version="1.0"

# Evită prompturi interactive
ENV DEBIAN_FRONTEND=noninteractive

# Instalare pachete necesare
RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    gdb \
    valgrind \
    make \
    git \
    vim \
    nano \
    less \
    tree \
    htop \
    python3 \
    python3-pip \
    python3-numpy \
    python3-scipy \
    && rm -rf /var/lib/apt/lists/*

# Instalare scikit-learn pentru comparații Python (opțional)
RUN pip3 install --break-system-packages scikit-learn matplotlib pandas

# Creare director de lucru
WORKDIR /workspace

# Copiere conținut (când se folosește docker build în directorul modulului)
COPY . /workspace/

# Compilare automată la pornire (opțional)
RUN if [ -f Makefile ]; then make clean && make all; fi

# Setare prompt colorat
RUN echo 'PS1="\[\033[1;36m\]ATP-ML\[\033[0m\]:\[\033[1;34m\]\w\[\033[0m\]$ "' >> /root/.bashrc

# Setare alias-uri utile
RUN echo 'alias ll="ls -la --color=auto"' >> /root/.bashrc && \
    echo 'alias la="ls -A --color=auto"' >> /root/.bashrc && \
    echo 'alias l="ls -CF --color=auto"' >> /root/.bashrc && \
    echo 'alias valgrind-full="valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes"' >> /root/.bashrc

# Afișare mesaj de bun venit
RUN echo 'echo ""' >> /root/.bashrc && \
    echo 'echo "╔═══════════════════════════════════════════════════════════════════════════╗"' >> /root/.bashrc && \
    echo 'echo "║     Săptămâna 18: Fundamente Machine Learning în C                        ║"' >> /root/.bashrc && \
    echo 'echo "║     Algoritmi și Tehnici de Programare (ATP) - ASE CSIE                   ║"' >> /root/.bashrc && \
    echo 'echo "╠═══════════════════════════════════════════════════════════════════════════╣"' >> /root/.bashrc && \
    echo 'echo "║  Comenzi utile:                                                           ║"' >> /root/.bashrc && \
    echo 'echo "║    make help     - Vezi toate comenzile disponibile                       ║"' >> /root/.bashrc && \
    echo 'echo "║    make run      - Rulează exemplul demonstrativ                          ║"' >> /root/.bashrc && \
    echo 'echo "║    make test     - Rulează testele automate                               ║"' >> /root/.bashrc && \
    echo 'echo "║    make valgrind - Verifică memory leaks                                  ║"' >> /root/.bashrc && \
    echo 'echo "╚═══════════════════════════════════════════════════════════════════════════╝"' >> /root/.bashrc && \
    echo 'echo ""' >> /root/.bashrc

# Punctul de intrare
CMD ["/bin/bash"]
