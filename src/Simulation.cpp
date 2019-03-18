/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#include "Road.h"
#include "Simulation.h"
#include "Vehicle.h"

Simulation::Simulation(unsigned int num_lanes, unsigned int length, double percent_full,
                       unsigned int max_speed, unsigned int look_forward, unsigned int look_other_forward,
                       unsigned int look_other_backward) {
    // Create the Road object for the simulation
    this->road_ptr = new Road(num_lanes, length);

    // Initialize the cars in the Road and the list of Vehicle objects
    this->road_ptr->initializeCars(percent_full, max_speed, &(this->vehicles), look_forward, look_other_forward,
                                   look_other_backward);
}

int Simulation::run_simulation(unsigned int max_time) {
    // Perform the simulation steps until the maximum time is reached
    while (this->time < max_time) {
        // Perform the lane switch step for all vehicles
        for (int i = 0; i < this->vehicles.size(); i++) {
            this->vehicles[i]->updateGaps();
            this->vehicles[i]->performLaneSwitch();
        }

        // Perform the independent lane updates
        for (int i = 0; i < this->vehicles.size(); i++) {
            this->vehicles[i]->updateGaps();
            this->vehicles[i]->performLaneMove();
        }

        // Increment the simulation time
        this->time++;
    }

    // Return with no errors
    return 0;
}