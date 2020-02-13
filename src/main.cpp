/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#include <iostream>

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

    // Check that the correct number of arguments was given
    if (argc < 2) {
        std::cout << "error: not enough arguments given!" << std::endl;
        return 1;
    }

    // Obtain the number of threads to run with from the arguments
    int num_threads = std::stoi(argv[1]);

    // Check that the number of threads is valid
    if (num_threads < 1) {
        std::cout << "error: invalid number of threads specified!" << std::endl;
        return 1;
    }

#ifndef DEBUG
    srand(time(NULL));
#endif

#ifdef DEBUG
    num_threads = 1;
#endif

    // Create an Inputs object to contain the simulation parameters
    Inputs inputs = Inputs();
    if (inputs.loadFromFile() != 0) {
        return 1;
    }

    // Create a Simulation object for the current simulation
    Simulation* simulation_ptr = new Simulation(inputs);

    // Run the Simulation
    simulation_ptr->run_simulation(num_threads);

    // Delete the Simulation object
    delete simulation_ptr;

    // Return with no errors
    return 0;
}