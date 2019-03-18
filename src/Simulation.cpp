/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#include "Road.h"
#include "Simulation.h"
#include "Vehicle.h"

Simulation::Simulation(Inputs inputs) {
    // Create the Road object for the simulation
    this->road_ptr = new Road(inputs);

    // Initialize the cars in the Road and the list of Vehicle objects
    this->road_ptr->initializeCars(inputs, &(this->vehicles));
}

Simulation::~Simulation() {
    // Delete the Road object in the simulation
    delete this->road_ptr;

    // Delete all the Vehicle objects in the Simulation
    for (int i = 0; i < this->vehicles.size(); i++) {
        delete this->vehicles[i];
    }
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