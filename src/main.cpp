/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#include <iostream>

#include "Inputs.h"
#include "Simulation.h"

int main() {
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

    // Create a Simulation object for the current simulation
    Simulation* simulation_ptr = new Simulation(inputs);

    // Delete the Simulation object
    delete simulation_ptr;

    return 0;
}