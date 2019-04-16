/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#include <omp.h>
#include <algorithm>
#include <cmath>
#include "Road.h"
#include "Simulation.h"
#include "Vehicle.h"

Simulation::Simulation(Inputs inputs) {
    // Create the Road object for the simulation
    this->road_ptr = new Road(inputs);

    // Initialize the first Vehicle id
    this->next_id = 0;

    // Obtain the simulation inputs
    this->inputs = inputs;

    // Initialize Statistic for travel time
    this->travel_time = new Statistic();
}

Simulation::~Simulation() {
    // Delete the Road object in the simulation
    delete this->road_ptr;

    // Delete all the Vehicle objects in the Simulation
    for (int i = 0; i < (int) this->vehicles.size(); i++) {
        delete this->vehicles[i];
    }
}

int Simulation::run_simulation(int num_threads) {
    // Obtain the start time
    double start_time = omp_get_wtime();

    // Set the number of threads for the execution
    omp_set_num_threads(num_threads);
    std::cout << "number of threads for simulation: " << num_threads << std::endl;

    // Set the simulation time to zero
    this->time = 0;

    // Declare a vector for vehicles to be removed each step
    std::vector<int> vehicles_to_remove;

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
            for (int n = 0; n * omp_get_num_threads() + omp_get_thread_num() < (int) this->vehicles.size(); n++) {
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

            for (int n = 0; n * omp_get_num_threads() + omp_get_thread_num() < (int) this->vehicles.size(); n++) {
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
            for (int n = 0; n * omp_get_num_threads() + omp_get_thread_num() < (int) this->vehicles.size(); n++) {
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

            for (int n = 0; n * omp_get_num_threads() + omp_get_thread_num() < (int) this->vehicles.size(); n++) {
                int i = n * omp_get_num_threads() + omp_get_thread_num();

                int time_on_road = this->vehicles[i]->performLaneMove();

                if (time_on_road != 0) {
                    #pragma omp critical
                    {
                        vehicles_to_remove.push_back(i);
                    }
                }
            }

            #pragma omp barrier

            // End of iteration steps
            if (omp_get_thread_num() == 0) {
                // Increment time
                this->time++;

                // Remove finished vehicles
                std::sort(vehicles_to_remove.begin(), vehicles_to_remove.end());
                for (int i = vehicles_to_remove.size() - 1; i >= 0; i--) {
                    // Update travel time statistic
                    this->travel_time->addValue(this->vehicles[vehicles_to_remove[i]]->getTravelTime(this->inputs));

                    // Delete the Vehicle
                    delete this->vehicles[vehicles_to_remove[i]];
                    this->vehicles.erase(this->vehicles.begin() + vehicles_to_remove[i]);
                }
                vehicles_to_remove.clear();

                // Spawn new Vehicles
                this->road_ptr->attemptSpawn(this->inputs, &(this->vehicles), &(this->next_id));
            }

            #pragma omp barrier
        }
    }

    // Print the total run time and average iterations per second and seconds per iteration
    double end_time = omp_get_wtime();
    std::cout << "--- Simulation Performance ---" << std::endl;
    std::cout << "total computation time: " << end_time - start_time << " [s]" << std::endl;
    std::cout << "average time per iteration: " << (end_time - start_time) / inputs.max_time << " [s]" << std::endl;
    std::cout << "average iterating frequency: " << inputs.max_time / (end_time - start_time) << " [iter/s]"
              << std::endl;

#ifdef DEBUG
    // Print final road configuration
    std::cout << "final road configuration" << std::endl;
    this->road_ptr->printRoad();
#endif

    // Print the average Vehicle time on the Road
    std::cout << "--- Simulation Results ---" << std::endl;
    std::cout << "time on road: avg=" << this->travel_time->getAverage() << ", std="
              << pow(this->travel_time->getVariance(), 0.5) << std::endl;

    // Return with no errors
    return 0;
}