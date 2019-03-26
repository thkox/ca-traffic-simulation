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

int Simulation::run_simulation(int num_threads) {
    // Obtain the start time
    double start_time = omp_get_wtime();

    // Set the number of threads for the execution
    omp_set_num_threads(num_threads);
    std::cout << "number of threads for simulation: " << omp_get_num_threads() << std::endl;

    // Perform the simulation steps in parallel until the maximum time is reached
    #pragma omp parallel
    {
        while (this->time < this->inputs.max_time) {
            #pragma omp barrier

#ifdef DEBUG
            if (omp_get_thread_num() == 0) {
                std::cout << "road configuration at time " << time << ":" << std::endl;
                this->road_ptr->printRoad();
                std::cout << "performing lane switches..." << std::endl;
            }
            #pragma omp barrier
#endif

            // Perform the lane switch step for all vehicles
            for (int n = 0; n * omp_get_num_threads() + omp_get_thread_num() < this->vehicles.size(); n++) {
                int i = n * omp_get_num_threads() + omp_get_thread_num();

                this->vehicles[i]->updateGaps(this->road_ptr);
#ifdef DEBUG
            #pragma omp critical
                {
                    this->vehicles[i]->printGaps();
                }
#endif
            }

            #pragma omp barrier

            for (int n = 0; n * omp_get_num_threads() + omp_get_thread_num() < this->vehicles.size(); n++) {
                int i = n * omp_get_num_threads() + omp_get_thread_num();

                this->vehicles[i]->performLaneSwitch(this->road_ptr);
            }

#ifdef DEBUG
            #pragma omp barrier
            if (omp_get_thread_num() == 0) {
                this->road_ptr->printRoad();
                std::cout << "performing lane movements..." << std::endl;
            }
#endif

            #pragma omp barrier

            // Perform the independent lane updates
            for (int n = 0; n * omp_get_num_threads() + omp_get_thread_num() < this->vehicles.size(); n++) {
                int i = n * omp_get_num_threads() + omp_get_thread_num();

                this->vehicles[i]->updateGaps(this->road_ptr);
#ifdef DEBUG
            #pragma omp critical
                {
                    this->vehicles[i]->printGaps();
                }
#endif
            }

            #pragma omp barrier

            for (int n = 0; n * omp_get_num_threads() + omp_get_thread_num() < this->vehicles.size(); n++) {
                int i = n * omp_get_num_threads() + omp_get_thread_num();

                this->vehicles[i]->performLaneMove();
            }

            // Increment the simulation time
            if (omp_get_thread_num() == 0) {
                this->time++;
            }

            #pragma omp barrier
        }
    }

    // Print the total run time and average iterations per second and seconds per iteration
    double end_time = omp_get_wtime();
    std::cout << "total computation time: " << end_time - start_time << " [s]" << std::endl;
    std::cout << "average time per iteration: " << (end_time - start_time) / inputs.max_time << " [s]" << std::endl;
    std::cout << "average iterating frequency: " << inputs.max_time / (end_time - start_time) << " [iter/s]"
              << std::endl;

#ifdef DEBUG
    // Print final road configuration
    std::cout << "final road configuration" << std::endl;
    this->road_ptr->printRoad();
#endif

    // Return with no errors
    return 0;
}