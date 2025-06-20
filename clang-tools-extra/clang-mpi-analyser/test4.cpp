#include <mpi.h>
#include <stdio.h>

void manual_sendrecv_scatter(int* sendbuf, int* recvbuf, int count, int root, MPI_Comm comm) {
    int rank, size;
    MPI_Status status;

    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    if (rank == root) {
        // Root sends to all (including self)
        for (int i = 0; i < size; ++i) {
            if (i == root) {
                // Self-copy
                recvbuf[0] = sendbuf[i];
            } else {
                // Root sends to others
                MPI_Sendrecv(&sendbuf[i], count, MPI_INT, i, 0,
                             &recvbuf[0], count, MPI_INT, i, 0,
                             comm, &status);
                printf("Root %d sent to and received from %d\n", root, i);
            }
        }
    } else {
        // Non-root does sendrecv with root
        MPI_Sendrecv(&sendbuf[0], count, MPI_INT, root, 0,
                     &recvbuf[0], count, MPI_INT, root, 0,
                     comm, &status);
        printf("Rank %d did sendrecv with root %d\n", rank, root);
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    int sendbuf[4], recvbuf[1];

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Fill sendbuf differently for each process
    for (int i = 0; i < size; i++) sendbuf[i] = 100 * rank + i;

    int root = 0;

    manual_sendrecv_scatter(sendbuf, recvbuf, 1, root, MPI_COMM_WORLD);

    printf("Rank %d received value %d\n", rank, recvbuf[0]);

    MPI_Finalize();
    return 0;
}
