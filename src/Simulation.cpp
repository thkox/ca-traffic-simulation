/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#include "Road.h"
#include "Simulation.h"

Simulation::Simulation(unsigned int num_lanes, unsigned int length, double percent_full,
                       unsigned int max_speed) {
    // Create the Road object for the simulation
    this->road_ptr = new Road(num_lanes, length);

    // Initialize the cars in the Road and the list of Vehicle objects
    this->road_ptr->initializeCars(percent_full, max_speed, &(this->vehicles));
}

int Simulation::run_simulation(unsigned int max_time) {
    // Perform the simulation steps until the maximum time is reached
    while (this->time < max_time) {
        // Perform the lane switch step for all vehicles
        for (int i = 0; i < this->vehicles.size(); i++) {
            // TODO: Implement lane switch step
        }

        // Perform the independent lane updates
        for (int i = 0; i < this->vehicles.size(); i++) {
            // TODO: Implement lane update step
        }

        // Increment the simulation time
        this->time++;
    }

    // Return with no errors
    return 0;
}