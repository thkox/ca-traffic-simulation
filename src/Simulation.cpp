/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#include "Road.h"
#include "Simulation.h"

Simulation::Simulation(unsigned int num_lanes, unsigned int length, double percent_full,
                       unsigned int max_speed) {
    // Create the Road object for the simulation
    this->road_ptr = new Road(num_lanes, length);

    // Initialize the cars in the Road
    this->road_ptr->initializeCars(percent_full, max_speed);
}

int Simulation::run_simulation(unsigned int max_time) {
    // TODO: Implement run method

    // Return with no errors
    return 0;
}