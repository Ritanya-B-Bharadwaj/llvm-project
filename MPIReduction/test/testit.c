#include "mpi.h"
#include <stdio.h>

void manual_sum_reduction(int *sendbuf, int *recvbuf, int count, MPI_Comm comm) {
    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);
    // Initialize recvbuf
    for (int i = 0; i < count; i++) {
        recvbuf[i] = 0;
    }
    // Manual reduction
    for (int i = 0; i < size; i++) {
        int temp[count];
        MPI_Recv(temp, count, MPI_INT, i, 0, comm, MPI_STATUS_IGNORE);
        for (int j = 0; j < count; j++) {
            recvbuf[j] += temp[j];
        }
    }
    // Send data to all processes
    for (int i = 0; i < size; i++) {
        MPI_Send(recvbuf, count, MPI_INT, i, 0, comm);
    }
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int sendbuf[1] = {rank + 1}; // Example data to be reduced
    int recvbuf[1];
    manual_sum_reduction(sendbuf, recvbuf, 1, MPI_COMM_WORLD);
    printf("Process %d received reduced result %d\n", rank, recvbuf[0]);
    MPI_Finalize();
    return 0;
}
