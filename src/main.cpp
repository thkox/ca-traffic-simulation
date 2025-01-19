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

    // Initialize MPI
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if ( rank == 0 ) {
        std::cout << "================================================" << std::endl;
        std::cout << "||    CELLULAR AUTOMATA TRAFFIC SIMULATION    ||" << std::endl;
        std::cout << "================================================" << std::endl;
    }
#ifndef DEBUG
    srand(time(NULL));
#endif

    // Create an Inputs object to contain the simulation parameters
    Inputs inputs = Inputs();
    if (rank == 0) {
        if (inputs.loadFromFile() != 0) {
            MPI_Finalize();
            return 1;
        }
    }

    // Broadcast the inputs to all processes
    MPI_Bcast(&inputs, sizeof(Inputs), MPI_BYTE, 0, MPI_COMM_WORLD);

    // Create a Simulation object for the current simulation only in the master process
    Simulation* simulation_ptr = new Simulation(inputs, rank, size);

    // Run the Simulation
    simulation_ptr->run_simulation(rank, size);

    // Delete the Simulation object only in the master process
    delete simulation_ptr;

    MPI_Finalize();

    // Return with no errors
    return 0;
}