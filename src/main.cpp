/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#include <fstream>
#include <iostream>
#include <mpi.h>
#include <unistd.h>

#include "Inputs.h"
#include "Simulation.h"

/**
 * Main point of execution of the program
 * @param argc number of command line arguments
 * @param argv command line arguments
 * @return 0 if successful, nonzero otherwise
 */
int main(int argc, char** argv) {
    std::cout << "================================================" << std::endl;
    std::cout << "||    CELLULAR AUTOMATA TRAFFIC SIMULATION    ||" << std::endl;
    std::cout << "================================================" << std::endl;

#ifndef DEBUG
    srand(time(NULL));
#endif


    // Initialize MPI
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    std::ofstream log_file("debug_log.txt", std::ios::app);
    if (!log_file.is_open()) {
        std::cerr << "Failed to open log file" << std::endl;
        MPI_Finalize();
        return 1;
    }

    log_file << "Rank " << rank << ": " << "I am rank " << rank << " of " << size << std::endl;

    // Create an Inputs object to contain the simulation parameters
    Inputs inputs = Inputs();
    if (rank == 0) {
        if (inputs.loadFromFile() != 0) {
            log_file.close();
            MPI_Finalize();
            return 1;
        }
    }

    // Broadcast the inputs to all processes
    MPI_Bcast(&inputs, sizeof(Inputs), MPI_BYTE, 0, MPI_COMM_WORLD);

    // Create a Simulation object for the current simulation only in the master process
    Simulation* simulation_ptr = nullptr;
    if (rank == 0) {
        simulation_ptr = new Simulation(inputs);
    }

    // Run the Simulation
    if (simulation_ptr != nullptr) {
        simulation_ptr->run_simulation(rank, size, log_file);
    }

    // Delete the Simulation object only in the master process
    if (simulation_ptr != nullptr) {
        delete simulation_ptr;
    }

    // MPI Finalize
    MPI_Finalize();

    // Close the log file
    log_file.close();

    // Return with no errors
    return 0;
}