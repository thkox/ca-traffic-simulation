/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#include <iostream>

#include "Simulation.h"

int main() {
#ifndef DEBUG
    srand(time(NULL));
#endif

    // Create a Simulation object for the current simulation
    Simulation* simulation_ptr = new Simulation();

    // Delete the Simulation object
    delete simulation_ptr;

    std::cout << "Hello, World!" << std::endl;
    return 0;
}