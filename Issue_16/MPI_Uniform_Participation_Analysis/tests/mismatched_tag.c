// Save this as tests/mismatched_tag.c
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    MPI_Init(NULL, NULL);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        int data = 100;
        // Rank 0 sends with tag 10
        MPI_Send(&data, 1, MPI_INT, 1, 10, MPI_COMM_WORLD);
    } else if (rank == 1) {
        int data;
        // Rank 1 receives with tag 20 (MISMATCH!)
        MPI_Recv(&data, 1, MPI_INT, 0, 20, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    MPI_Finalize();
    return 0;
}