#include <mpi.h>
#include <stdio.h>
#include <stdlib.h> // For NULL

// This is a standard, correct MPI program where Rank 0 sends
// a message to Rank 1. The tags and communicators match.
// Your analyser should identify this as a successful "uniform participation" pair.

int main(int argc, char** argv) {
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // We need at least two processes for this example
    if (world_size < 2) {
        fprintf(stderr, "World size must be greater than 1 for this example\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    if (world_rank == 0) {
        // If we are rank 0, create some data and send it to process 1
        int data_to_send = 12345;
        
        // MPI_Send(buffer, count, datatype, destination_rank, tag, communicator)
        MPI_Send(&data_to_send, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        
        printf("Process 0 sent data %d to process 1\n", data_to_send);

    } else if (world_rank == 1) {
        // If we are rank 1, receive the data from process 0
        int received_data;
        
        // MPI_Recv(buffer, count, datatype, source_rank, tag, communicator, status)
        MPI_Recv(&received_data, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        printf("Process 1 received data: %d\n", received_data);
    }

    // Finalize the MPI environment.
    MPI_Finalize();
    return 0;
}