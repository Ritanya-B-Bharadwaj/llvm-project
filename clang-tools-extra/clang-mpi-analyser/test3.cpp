// File: test.cpp
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h> // For malloc/free

// --- Manual Allgather Implementation ---
// Simulates MPI_Allgather using point-to-point communication.
// Each process sends its local data to every other process, and
// receives data from every other process into its portion of the global buffer.
// This pattern should be flagged by your analyzer as a candidate for MPI_Allgather.
void manual_allgather(int* sendbuf, int sendcount, MPI_Datatype sendtype,
                      int* recvbuf, int recvcount, MPI_Datatype recvtype,
                      MPI_Comm comm) {
    int rank, num_procs;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &num_procs);

    // Each process iterates through all other processes (including itself).
    // For each 'i', it sends its local data to 'i' and receives data from 'i'.
    // The received data from rank 'i' goes into the i-th block of the recvbuf.
    for (int i = 0; i < num_procs; ++i) {
        // Each process sends its local 'sendbuf' to process 'i'
        MPI_Send(sendbuf, sendcount, sendtype, i, 0, comm);

        // Each process receives data from process 'i'
        // The received data from rank 'i' is placed into the 'i'-th block of the 'recvbuf'.
        MPI_Recv(recvbuf + i * recvcount, recvcount, recvtype, i, 0, comm, MPI_STATUS_IGNORE);
        // printf("Rank %d: Sent to %d, Received from %d\n", rank, i, i); // For verbose debug
    }
}

// --- Manual Alltoall Implementation ---
// Simulates MPI_Alltoall using point-to-point communication.
// Each process sends a different chunk of its data to each other process, and
// receives a different chunk from each other process.
// This pattern should be flagged by your analyzer as a candidate for MPI_Alltoall.
void manual_alltoall(int* sendbuf, int sendcount, MPI_Datatype sendtype,
                     int* recvbuf, int recvcount, MPI_Datatype recvtype,
                     MPI_Comm comm) {
    int rank, num_procs;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &num_procs);

    // Each process iterates through all other processes (including itself).
    // For each 'i', it sends its 'i'-th chunk to process 'i', and
    // receives its 'i'-th chunk from process 'i'.
    for (int i = 0; i < num_procs; ++i) {
        // Send the (i)-th chunk of local 'sendbuf' to process 'i'
        MPI_Send(sendbuf + i * sendcount, sendcount, sendtype, i, 0, comm);

        // Receive data from process 'i' into the (i)-th chunk of local 'recvbuf'
        MPI_Recv(recvbuf + i * recvcount, recvcount, recvtype, i, 0, comm, MPI_STATUS_IGNORE);
        // printf("Rank %d: Sent chunk %d to %d, Received chunk from %d\n", rank, i, i, i); // For verbose debug
    }
}

// --- Previous Manual Gather (Root-based) ---
// This should still be detected as Manual Gather by your analyzer.
void manual_gather_root_based(int* sendbuf, int sendcount, MPI_Datatype sendtype,
                         int* recvbuf, int recvcount, MPI_Datatype recvtype,
                         int root_rank_val, MPI_Comm comm) {
    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    if (rank == root_rank_val) {
        // Root process gathers data
        // Copy its own data first (or receive from self, less common but possible)
        MPI_Recv(recvbuf + rank * recvcount, recvcount, recvtype,
                 rank, 0, comm, MPI_STATUS_IGNORE); // Receiving from self for gather

        for (int i = 0; i < size; ++i) {
            if (i == rank) continue; // Skip self

            // Root receives from process 'i'
            MPI_Recv(recvbuf + i * recvcount, recvcount, recvtype,
                     i, 0, comm, MPI_STATUS_IGNORE);
            // printf("Root %d received from rank %d\n", rank, i);
        }
    } else {
        // Non-root processes send their data to the root
        MPI_Send(sendbuf, sendcount, sendtype, root_rank_val, 0, comm);
        // printf("Rank %d sent to root %d\n", rank, root_rank_val);
    }
}

// --- Previous Manual Scatter (Root-based) ---
// This should still be detected as Manual Scatter by your analyzer.
void manual_scatter_root_based(int* sendbuf, int sendcount, MPI_Datatype sendtype,
                           int* recvbuf, int recvcount, MPI_Datatype recvType,
                           int root_rank_val, MPI_Comm comm) {
    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    if (rank == root_rank_val) {
        // Root process scatters data
        for (int i = 0; i < size; ++i) {
            // Root sends a chunk of data to each process 'i'
            MPI_Send(sendbuf + i * sendcount, sendcount, sendtype,
                     i, 0, comm);
            // printf("Root %d sent to rank %d\n", rank, i);
        }
    } else {
        // Non-root processes receive their portion from the root
        MPI_Recv(recvbuf, recvcount, recvType,
                 root_rank_val, 0, comm, MPI_STATUS_IGNORE);
        // printf("Rank %d received from root %d\n", rank, root_rank_val);
    }
}


int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int num_procs;
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    if (num_procs < 2) {
        printf("Please run with at least 2 processes for these tests (e.g., mpirun -np 2 ./your_tool_executable test.cpp).\n");
        MPI_Finalize();
        return 1;
    }

    const int DATA_COUNT = 2; // Data elements per process for each operation

    // --- Test Manual Allgather ---
    printf("--- Rank %d: Testing Manual Allgather ---\n", rank);
    int* allgather_send_data = (int*)malloc(DATA_COUNT * sizeof(int));
    for (int i = 0; i < DATA_COUNT; ++i) {
        allgather_send_data[i] = rank * 10 + i; // e.g., Rank 0 sends [0,1], Rank 1 sends [10,11]
    }
    int* allgather_recv_data = (int*)malloc(num_procs * DATA_COUNT * sizeof(int));

    manual_allgather(allgather_send_data, DATA_COUNT, MPI_INT,
                     allgather_recv_data, DATA_COUNT, MPI_INT,
                     MPI_COMM_WORLD);

    printf("Rank %d: Allgathered data [", rank);
    for (int i = 0; i < num_procs * DATA_COUNT; ++i) {
        printf("%d%s", allgather_recv_data[i], (i == num_procs * DATA_COUNT - 1) ? "" : ", ");
    }
    printf("]\n");
    free(allgather_send_data);
    free(allgather_recv_data);
    MPI_Barrier(MPI_COMM_WORLD);
    printf("-----------------------------------------\n\n");


    // --- Test Manual Alltoall ---
    printf("--- Rank %d: Testing Manual Alltoall ---\n", rank);
    int* alltoall_send_data = (int*)malloc(num_procs * DATA_COUNT * sizeof(int));
    // Each rank prepares data specifically for each other rank
    for (int i = 0; i < num_procs; ++i) {
        for (int j = 0; j < DATA_COUNT; ++j) {
            // Data for rank 'i' from this process 'rank'
            alltoall_send_data[i * DATA_COUNT + j] = rank * 100 + i * 10 + j;
        }
    }
    int* alltoall_recv_data = (int*)malloc(num_procs * DATA_COUNT * sizeof(int));

    manual_alltoall(alltoall_send_data, DATA_COUNT, MPI_INT,
                    alltoall_recv_data, DATA_COUNT, MPI_INT,
                    MPI_COMM_WORLD);

    printf("Rank %d: Alltoall received data [", rank);
    for (int i = 0; i < num_procs * DATA_COUNT; ++i) {
        printf("%d%s", alltoall_recv_data[i], (i == num_procs * DATA_COUNT - 1) ? "" : ", ");
    }
    printf("]\n");
    free(alltoall_send_data);
    free(alltoall_recv_data);
    MPI_Barrier(MPI_COMM_WORLD);
    printf("-----------------------------------------\n\n");


    // --- Test Manual Gather (Root 0) ---
    printf("--- Rank %d: Testing Manual Gather (Root 0) ---\n", rank);
    int gather_my_val = rank + 1;
    int *gather_all_vals = NULL;
    if (rank == 0) {
        gather_all_vals = (int*)malloc(num_procs * sizeof(int));
    }
    manual_gather_root_based(&gather_my_val, 1, MPI_INT,
                             gather_all_vals, 1, MPI_INT,
                             0, MPI_COMM_WORLD);
    if (rank == 0) {
        printf("Rank 0: Gathered values [");
        for (int i = 0; i < num_procs; ++i) {
            printf("%d%s", gather_all_vals[i], (i == num_procs - 1) ? "" : ", ");
        }
        printf("]\n");
        free(gather_all_vals);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    printf("-----------------------------------------\n\n");

    // --- Test Manual Scatter (Root 0) ---
    printf("--- Rank %d: Testing Manual Scatter (Root 0) ---\n", rank);
    int *scatter_send_vals = NULL;
    if (rank == 0) {
        scatter_send_vals = (int*)malloc(num_procs * sizeof(int));
        for (int i = 0; i < num_procs; ++i) {
            scatter_send_vals[i] = (i + 1) * 100;
        }
    }
    int scatter_recv_val;
    manual_scatter_root_based(scatter_send_vals, 1, MPI_INT,
                              &scatter_recv_val, 1, MPI_INT,
                              0, MPI_COMM_WORLD);
    printf("Rank %d: Scattered value %d\n", rank, scatter_recv_val);
    if (rank == 0) {
        free(scatter_send_vals);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    printf("-----------------------------------------\n\n");


    MPI_Finalize();
    return 0;
}