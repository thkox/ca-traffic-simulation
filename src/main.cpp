/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#include <iostream>

#include "Simulation.h"

int main() {
    std::cout << "================================================" << std::endl;
    std::cout << "||    CELLULAR AUTOMATA TRAFFIC SIMULATION    ||" << std::endl;
    std::cout << "================================================" << std::endl;

#ifndef DEBUG
    srand(time(NULL));
#endif

    // Create a Simulation object for the current simulation
    // TODO: Add actual simulation paramters from input file
    Simulation* simulation_ptr = new Simulation(2, 10, 0.5, 5, 6, 6, 5);

    // Delete the Simulation object
    delete simulation_ptr;

    std::cout << "Hello, World!" << std::endl;
    return 0;
}