# OpenMPI ve OpenMP içeren temel bir Ubuntu imajı
FROM ubuntu:latest

# Gerekli paketleri yükleyelim
RUN apt-get update && apt-get install -y \
    openmpi-bin \
    libopenmpi-dev \
    gcc \
    g++ \
    make \
    && rm -rf /var/lib/apt/lists/*

# OpenMP ve MPI içeren bir test dosyası ekleyelim
COPY main.c /home/main.c

# Çalışma dizini
WORKDIR /home

# C kodunu derleme
RUN mpicc -fopenmp main.c -o main.out

CMD ["mpirun", "--allow-run-as-root", "-np", "4", "./main.out"]
