/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#include <omp.h>
#include "Road.h"
#include "Simulation.h"
#include "Vehicle.h"

Simulation::Simulation(Inputs inputs) {
    // Create the Road object for the simulation
    this->road_ptr = new Road(inputs);

    // Initialize the cars in the Road and the list of Vehicle objects
    this->road_ptr->initializeCars(inputs, &(this->vehicles));

    // Obtain the simulation inputs
    this->inputs = inputs;
}

Simulation::~Simulation() {
    // Delete the Road object in the simulation
    delete this->road_ptr;

    // Delete all the Vehicle objects in the Simulation
    for (int i = 0; i < this->vehicles.size(); i++) {
        delete this->vehicles[i];
    }
}

int Simulation::run_simulation() {
    // Obtain the start time
    double start_time = omp_get_wtime();

    // Perform the simulation steps until the maximum time is reached
    while (this->time < this->inputs.max_time) {
#ifdef DEBUG
        std::cout << "road configuration at time " << time << ":" << std::endl;
        this->road_ptr->printRoad();
        std::cout << "performing lane switches..." << std::endl;
#endif

        // Perform the lane switch step for all vehicles
        for (int i = 0; i < this->vehicles.size(); i++) {
            this->vehicles[i]->updateGaps(this->road_ptr);
#ifdef DEBUG
            this->vehicles[i]->printGaps();
#endif
        }

        for (int i = 0; i < this->vehicles.size(); i++) {
            this->vehicles[i]->performLaneSwitch(this->road_ptr);
        }

#ifdef DEBUG
        this->road_ptr->printRoad();
        std::cout << "performing lane movements..." << std::endl;
#endif

        // Perform the independent lane updates
        for (int i = 0; i < this->vehicles.size(); i++) {
            this->vehicles[i]->updateGaps(this->road_ptr);
#ifdef DEBUG
            this->vehicles[i]->printGaps();
#endif
        }

        for (int i = 0; i < this->vehicles.size(); i++) {
            this->vehicles[i]->performLaneMove();
        }

        // Increment the simulation time
        this->time++;
    }

    // Print the total run time and average iterations per second and seconds per iteration
    double end_time = omp_get_wtime();
    std::cout << "total computation time: " << end_time - start_time  << " [s]" << std::endl;
    std::cout << "average time per iteration: " << (end_time - start_time) / inputs.max_time << " [s]" << std::endl;
    std::cout << "average iterating frequency: " << inputs.max_time / (end_time - start_time) << " [s^-1]" << std::endl;

    // Return with no errors
    return 0;
}