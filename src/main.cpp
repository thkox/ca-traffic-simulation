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

    // Create an Inputs object to contain the simulation parameters
    Inputs inputs = Inputs();
    if (inputs.loadFromFile() != 0) {
        return 1;
    }

    std::ofstream log_file("debug_log.txt", std::ios::app);

    if (!log_file.is_open()) {
        std::cerr << "Failed to open log file" << std::endl;
        return 1;
    }


    // Create a Simulation object for the current simulation
    Simulation* simulation_ptr = new Simulation(inputs);

    // Initialize MPI
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );



    log_file << "Rank " << rank << ": " << "I am rank " << rank << " of " << size << std::endl;


    // Run the Simulation
    simulation_ptr->run_simulation(rank, size, log_file);


    // MPI Finalize
    MPI_Finalize();

    // Delete the Simulation object
    delete simulation_ptr;



    // Close the log file
    log_file.close();

    // Return with no errors
    return 0;
}
