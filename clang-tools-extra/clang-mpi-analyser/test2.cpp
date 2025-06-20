// File: test.cpp
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h> // For malloc and free

// A function mimicking MPI_Gather, but manually implemented for rank 2
void manual_gather_rank2(int* sendbuf, int sendcount, int* recvbuf, int recvcount, int root_rank_val, MPI_Comm comm) {
    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    if (rank == root_rank_val) {
        // Root process (rank 2) gathers data
        printf("Rank %d (root) gathering data...\n", rank);
        // Copy its own data first
        for (int k = 0; k < sendcount; ++k) {
            recvbuf[rank * recvcount + k] = sendbuf[k];
        }

        // Receive from all other processes
        for (int i = 0; i < size; ++i) {
            if (i != rank) {
                // Root receives from process 'i'
                MPI_Recv(recvbuf + i * recvcount, recvcount, MPI_INT, i, 0, comm, MPI_STATUS_IGNORE);
            }
        }
        printf("Rank %d (root) finished gathering.\n", rank);

    } else {
        // Non-root processes send their data to the root (rank 2)
        printf("Rank %d sending data to root %d...\n", rank, root_rank_val);
        MPI_Send(sendbuf, sendcount, MPI_INT, root_rank_val, 0, comm);
        printf("Rank %d finished sending.\n", rank);
    }
}

// A function mimicking MPI_Scatter, but manually implemented for rank 2
void manual_scatter_rank2(int* sendbuf, int sendcount, int* recvbuf, int recvcount, int root_rank_val, MPI_Comm comm) {
    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    if (rank == root_rank_val) {
        // Root process (rank 2) scatters data
        printf("Rank %d (root) scattering data...\n", rank);
        // Copy its own data first
        for (int k = 0; k < recvcount; ++k) {
            recvbuf[k] = sendbuf[rank * sendcount + k];
        }

        // Send to all other processes
        for (int i = 0; i < size; ++i) {
            if (i != rank) {
                // Root sends to process 'i'
                MPI_Send(sendbuf + i * sendcount, sendcount, MPI_INT, i, 0, comm);
            }
        }
        printf("Rank %d (root) finished scattering.\n", rank);

    } else {
        // Non-root processes receive their data from the root (rank 2)
        printf("Rank %d receiving data from root %d...\n", rank, root_rank_val);
        MPI_Recv(recvbuf, recvcount, MPI_INT, root_rank_val, 0, comm, MPI_STATUS_IGNORE);
        printf("Rank %d finished receiving.\n", rank);
    }
}


int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if (world_size < 3) {
        printf("Please run with at least 3 processes for this test (e.g., mpirun -np 3 ./your_tool_executable test.cpp).\n");
        MPI_Finalize();
        return 1;
    }

    const int DATA_PER_PROCESS = 5;
    const int TEST_ROOT_RANK = 2; // Our custom root rank

    // --- Test Manual Gather with Rank 2 as Root ---
    printf("--- Testing Manual Gather with Rank %d as Root ---\n", TEST_ROOT_RANK);

    int* gather_send_data = (int*)malloc(DATA_PER_PROCESS * sizeof(int));
    for (int i = 0; i < DATA_PER_PROCESS; ++i) {
        gather_send_data[i] = world_rank * 100 + i; // Unique data for each process
    }

    int* gather_recv_data = NULL;
    if (world_rank == TEST_ROOT_RANK) {
        gather_recv_data = (int*)malloc(world_size * DATA_PER_PROCESS * sizeof(int));
    }

    manual_gather_rank2(gather_send_data, DATA_PER_PROCESS, gather_recv_data, DATA_PER_PROCESS, TEST_ROOT_RANK, MPI_COMM_WORLD);

    if (world_rank == TEST_ROOT_RANK) {
        printf("Gathered data at Rank %d (root):\n", world_rank);
        for (int i = 0; i < world_size; ++i) {
            printf("  From rank %d: ", i);
            for (int j = 0; j < DATA_PER_PROCESS; ++j) {
                printf("%d ", gather_recv_data[i * DATA_PER_PROCESS + j]);
            }
            printf("\n");
        }
        free(gather_recv_data);
    }
    free(gather_send_data);
    printf("--------------------------------------------------\n\n");

    MPI_Barrier(MPI_COMM_WORLD); // Synchronize for cleaner output

    // --- Test Manual Scatter with Rank 2 as Root ---
    printf("--- Testing Manual Scatter with Rank %d as Root ---\n", TEST_ROOT_RANK);

    int* scatter_send_data = NULL;
    if (world_rank == TEST_ROOT_RANK) {
        scatter_send_data = (int*)malloc(world_size * DATA_PER_PROCESS * sizeof(int));
        for (int i = 0; i < world_size * DATA_PER_PROCESS; ++i) {
            scatter_send_data[i] = i + 1000; // Unique data to scatter
        }
    }

    int* scatter_recv_data = (int*)malloc(DATA_PER_PROCESS * sizeof(int));

    manual_scatter_rank2(scatter_send_data, DATA_PER_PROCESS, scatter_recv_data, DATA_PER_PROCESS, TEST_ROOT_RANK, MPI_COMM_WORLD);

    printf("Rank %d received data:\n", world_rank);
    for (int i = 0; i < DATA_PER_PROCESS; ++i) {
        printf("%d ", scatter_recv_data[i]);
    }
    printf("\n");

    if (world_rank == TEST_ROOT_RANK) {
        free(scatter_send_data);
    }
    free(scatter_recv_data);
    printf("--------------------------------------------------\n\n");


    MPI_Finalize();
    return 0;
}