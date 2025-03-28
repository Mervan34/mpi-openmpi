#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define DATA_SIZE 1000

// OpenMP ile paralel veri işleme fonksiyonu
void process_data(int *data, int size) {
    #pragma omp parallel for
    for (int i = 0; i < size; i++) {
        int thread_id = omp_get_thread_num();  // İş parçacığı numarasını al
        int core_id = sched_getcpu();  // İş parçacığının çalıştığı çekirdeğin id'sini al

        // Her iş parçacığının çalıştığı çekirdeği ve ID'yi yazdır
        printf("Thread %d is running on CPU core %d\n", thread_id, core_id);

        data[i] *= 2; 
    }
}

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);  //MPI başlatılır
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // ranka otomatik değer ataması yapılır
    MPI_Comm_size(MPI_COMM_WORLD, &size);  

    int chunk_size = DATA_SIZE / size;  // Her düğümün alacağı veri boyutu
    int *data = (int *)malloc(chunk_size * sizeof(int));  // Veriyi tutacak bellek ayrılır

    if (rank == 0) {
        int full_data[DATA_SIZE];
        for (int i = 0; i < DATA_SIZE; i++)
            full_data[i] = i;  

        // Veriyi düğümlere dağıt
        for (int i = 1; i < size; i++)
            MPI_Send(&full_data[i * chunk_size], chunk_size, MPI_INT, i, 0, MPI_COMM_WORLD);
        
        for (int i = 0; i < chunk_size; i++)
            data[i] = full_data[i];
    } else {
        MPI_Recv(data, chunk_size, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    // Her düğüm kendi içinde OpenMP ile paralel hesaplama yapıyor
    process_data(data, chunk_size);

    // Sonuçları geri gönder
    if (rank == 0) {
        int full_result[DATA_SIZE];

        // Ana düğüm, kendi kısmını alır
        for (int i = 0; i < chunk_size; i++)
            full_result[i] = data[i];

        // Diğer düğümlerden sonuçları al
        for (int i = 1; i < size; i++)
            MPI_Recv(&full_result[i * chunk_size], chunk_size, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("Final Result: \n");
        for (int i = 0; i < DATA_SIZE; i++)
            printf("%d ", full_result[i]);  // Sonuçları ekrana yazdır
        printf("\n");
    } else {
        MPI_Send(data, chunk_size, MPI_INT, 0, 0, MPI_COMM_WORLD);  // Sonuçları ana düğüme gönder
    }

    free(data);  // Belleği serbest bırak
    MPI_Finalize();  // MPI'yi sonlandır
    return 0;
}
