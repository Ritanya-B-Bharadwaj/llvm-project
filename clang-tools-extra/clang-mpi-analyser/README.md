## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [Motivation](#motivation)
- [Installation](#installation)
  - [Prerequisites](#prerequisites)
  - [Building the Project](#building-the-project)
- [Usage](#usage)
  - [Basic Usage](#basic-usage)
  - [Scatter/Gather Analysis](#scattergather-analysis)
  - [Understanding the Output](#understanding-the-output)

## Introduction

The `clang-mpi-analyser` is a static analysis tool built on the LLVM/Clang infrastructure. It is designed to analyze C/C++ source code that uses the Message Passing Interface (MPI) library. Its primary goal is to identify common MPI usage patterns, particularly inefficient or non-idiomatic manual implementations of collective operations (like Scatter and Gather) that could be optimized by using native MPI collective functions.

By providing actionable suggestions, this tool aims to help developers improve the performance, scalability, and readability of their MPI applications.

## Features

- **MPI Function Call Detection:** Identifies and extracts information about various MPI functions (`MPI_Send`, `MPI_Recv`, `MPI_Sendrecv`,`MPI_Comm_rank`, `MPI_Comm_size`, etc.).
- **Manual Collective Pattern Detection:**
    - **Manual Scatter:** Detects patterns where the root process manually sends distinct data chunks to other processes in a loop, and non-root processes receive their specific chunks from the root.
    - **Manual Gather:** Detects patterns where non-root processes send their data to the root, and the root manually receives distinct data chunks from each process in a loop.
    - **Manual Allgather:** Detects communication patterns where each process sends its local data to all other processes, and each process receives data from all others, typically implemented using point-to-point communication like MPI_Sendrecv or loops of MPI_Send/MPI_Recv.
    - **Manual AlltoAll:** Detects patterns where each process sends different data to every other process and receives different data from all others, often implemented with nested loops or indexed buffers using point-to-point calls.
- **Detailed Reporting:** Generates comprehensive reports for detected patterns, including:
    - Pattern type (e.g., Manual Scatter).
    - Explanation of the issue.
    - Optimization suggestion (e.g., use `MPI_Scatter`).
    - Source code location (function name and line number).
    - Representative code snippet of the detected pattern.

## Motivation

MPI applications often achieve high performance through the use of optimized collective communication routines (e.g., `MPI_Scatter`, `MPI_Gather`). However, developers sometimes implement these operations manually using point-to-point communication (`MPI_Send`, `MPI_Recv`, `MPI_Sendrecv`). While functionally correct, these manual implementations can be:

- **Less Performant:** Native MPI collectives are highly optimized for specific network topologies and hardware, often outperforming naive manual implementations.
- **Less Scalable:** Manual point-to-point communication can lead to bottlenecks as the number of processes increases.
- **Less Readable:** Complex loops and `if/else` constructs for manual collectives make the code harder to understand and maintain.

This tool aims to automatically identify such opportunities for optimization, guiding developers towards more efficient and idiomatic MPI programming practices.

## Installation

This project is built as a Clang tool within the LLVM ecosystem. The standard way to build it is as part of `clang-tools-extra`.

### Prerequisites

* **Linux/macOS:** A modern C++ compiler (GCC 10+ or Clang 10+ recommended).
* **CMake:** Version 3.13 or higher.
* **LLVM & Clang Source Code:** You'll need a clone of the `llvm-project` repository. It is highly recommended to use a specific release branch (e.g., `llvmorg-18.0.0-rc2`) or a recent `main` branch.
* **Open MPI Development Headers:** The MPI library headers are required for Clang to understand MPI types and functions.

    ```bash
    # For Debian/Ubuntu
    sudo apt update
    sudo apt install build-essential cmake python3 python3-pip libopenmpi-dev openmpi-bin

    # For Fedora/RHEL
    sudo dnf install @development-tools cmake python3 python3-pip openmpi-devel
    ```

### Building the Project

1.  **Clone LLVM/Clang:** If you haven't already, clone the `llvm-project` repository:

    ```bash
    git clone [https://github.com/llvm/llvm-project.git](https://github.com/llvm/llvm-project.git)
    cd llvm-project
    # Optional: Checkout a specific release branch for stability
    # git checkout llvmorg-18.1.0
    ```

2.  **Build the Project:**
    In llvm-project folder do cd build or create one
    ```bash
    cd build
    cmake -G "Unix Makefiles" -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../install ../llvm #or use ninja
    #To build the project do
    make -j 12 #use the number acoording to your system compatibility
    make -j 12 install
    ```
    This will compile LLVM, Clang, and your `clang-mpi-analyser` tool. The executable will be located at `install/bin/clang-mpi-analyser`.
3. **Test the build:**
   in llvm-project directory
   ```bash
    ./install/bin/clang++  ./test.cpp   # clang++ for cpp clang for c. This will generate exe file  
    ./a.out                             # This wil print the output
   ```

## Usage

The `clang-mpi-analyser` tool operates on C/C++ source files. You need to provide it with the necessary include paths for your compiler and MPI library, just as you would when compiling with `clang` or `g++`.

### Basic Usage

```bash
./install/bin/clang-mpi-analyser -analyze-mpi-scatter-gather <your_source_file.cpp> -- <Header files if necessary>
```

## Sample Input and output
```bash
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
```

## Sample output
```bash
=============================================================
Analysis of manual_allgather Function
=============================================================
Pattern Detected: Manual All-to-All Data Gathering (Allgather)
- Issue: This function implements a manual Allgather operation. Data from all processes is being gathered by all other processes using point-to-point communication within a loop.
    - Specifically, each process sends its local data to every other process, and receives data from every other process into a collective buffer indexed by the iterating rank.
- Suggestion: Consider using MPI_Allgather for better performance and scalability.
- Note : This may also be a case of manual data gathering at a root process using MPI_Sendrecv so use MPI_Gather if the data is being gathered at root process else use MPI_Allgather.
- Location: manual_allgather function, Loop starting at Line 21
Details:
- Representative code snippet:
MPI_Send(sendbuf, sendcount, sendtype, i, 0, comm)
=============================================================

=============================================================
Analysis of manual_alltoall Function
=============================================================
Pattern Detected: Manual All-to-All Data Exchange (Alltoall)
- Issue: This function implements a manual Alltoall operation. Data is being exchanged between all processes using point-to-point communication within a loop.
    - Specifically, each process sends a distinct chunk of its data to every other process (indexed by iterating rank) and receives a distinct chunk from every other process (indexed by iterating rank).
- Suggestion: Consider using MPI_Alltoall for better performance and scalability.
- Location: manual_alltoall function, Loop starting at Line 47
Details:
- Representative code snippet:
MPI_Send(sendbuf + i * sendcount, sendcount, sendtype, i, 0, comm)
=============================================================

=============================================================
Analysis of manual_gather_root_based Function
=============================================================
Pattern Detected: Manual Data Gathering
- Issue: This function implements a manual Gather operation. Data from all processes is being collected by the root process using point-to-point communication.
    - Specifically, non-root processes send their local data to the root, and the root process iteratively receives data from all other processes.
- Suggestion: Consider using MPI_Gather for better performance and scalability.
- Location: manual_gather_root_based function, Line 66
Details:
- Representative code snippet:
MPI_Recv(recvbuf + i * recvcount, recvcount, recvtype,
                     i, 0, comm, MPI_STATUS_IGNORE)
=============================================================

=============================================================
Analysis of manual_scatter_root_based Function
=============================================================
Pattern Detected: Manual Data Distribution (Scatter)
- Issue: This function implements a manual Scatter operation. Data from the root process is being distributed to all processes using point-to-point communication.
    - Specifically, the root process iteratively sends distinct data chunks to each rank, and non-root processes receive their respective chunks from the root.
- Suggestion: Consider using MPI_Scatter for better performance and scalability.
- Location: manual_scatter_root_based function, Line 96
Details:
- Representative code snippet:
MPI_Send(sendbuf + i * sendcount, sendcount, sendtype,
                     i, 0, comm)
=============================================================
```

