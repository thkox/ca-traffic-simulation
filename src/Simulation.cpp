/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#include <chrono>
#include <algorithm>
#include <cmath>
#include "Road.h"
#include "Simulation.h"
#include <fstream>  // For std::ofstream
#include <iostream> // For std::cout
#include <unistd.h>

#include "Vehicle.h"

/**
 * Constructor for the Simulation
 * @param inputs
 */
Simulation::Simulation(Inputs inputs, int rank, int size, std::ofstream &log_file) {

    // Calculate the section of the road for this process
    const int length_per_process = inputs.length / size;

    this->start_site =rank * length_per_process;
    this->end_site = (rank + 1) * length_per_process;

    // Create the Road object for the simulation
    this->road_ptr = new Road(inputs, start_site, end_site, rank, log_file);

    // Initialize the first Vehicle id
    this->next_id = 0;

    // Obtain the simulation inputs
    this->inputs = inputs;

    // Initialize Statistic for travel time
    this->travel_time = new Statistic();
}

/**
 * Destructor for the Simulation
 */
Simulation::~Simulation() {
    // Delete the Road object in the simulation
    delete this->road_ptr;

    // Delete all the Vehicle objects in the Simulation
    for (int i = 0; i < (int) this->vehicles.size(); i++) {
        delete this->vehicles[i];
    }
}

/**
 * Executes the simulation in parallel using the specified number of threads
 * @param num_threads number of threads to run the simulation with
 * @return 0 if successful, nonzero otherwise
 */
int Simulation::run_simulation(int rank, int size, std::ofstream &log_file) {

    // Obtain the start time
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    // Set the simulation time to zero
    this->time = 0;

    // Declare a vector for vehicles to be removed each step
    std::vector<int> vehicles_to_remove;

    while (this->time < this->inputs.max_time) {

#ifdef DEBUG
        std::cout << "road configuration at time " << time << ":" << std::endl;
        this->road_ptr->printRoad(rank, log_file);

        log_file << "Rank " << rank << ": " << "Road configuration at time " << time << ":" << std::endl;
        this->road_ptr->printRoad(rank, log_file);

        std::cout << "performing lane switches..." << std::endl;
        log_file << "Rank " << rank << ": " << "Performing lane switches..." << std::endl;
#endif

        // Perform the lane switch step for all vehicles
        for (int n = 0; n < (int) this->vehicles.size(); n++) {
            this->vehicles[n]->updateGaps(this->road_ptr);
#ifdef DEBUG
            this->vehicles[n]->printGaps(rank, log_file);
#endif
        }

        for (int n = 0; n < (int) this->vehicles.size(); n++) {
            this->vehicles[n]->performLaneSwitch(this->road_ptr);
        }

#ifdef DEBUG

        this->road_ptr->printRoad(rank, log_file);
        std::cout << "performing lane movements..." << std::endl;
        log_file << "Rank " << rank << ": " << "Performing lane movements..." << std::endl;

#endif

        // Perform the independent lane updates
        for (int n = 0; n < (int) this->vehicles.size(); n++) {
            this->vehicles[n]->updateGaps(this->road_ptr);
#ifdef DEBUG
            this->vehicles[n]->printGaps(rank, log_file);
#endif
        }

        for (int n = 0; n < (int) this->vehicles.size(); n++) {
            int time_on_road = this->vehicles[n]->performLaneMove();

            if (time_on_road != 0) {
                    vehicles_to_remove.push_back(n);
            }
        }

        // End of iteration steps
        // Increment time
        this->time++;

        // Remove finished vehicles
        std::sort(vehicles_to_remove.begin(), vehicles_to_remove.end());
        for (int i = vehicles_to_remove.size() - 1; i >= 0; i--) {
            // Update travel time statistic if beyond warm-up period
            if (this->time > this->inputs.warmup_time) {
                this->travel_time->addValue(this->vehicles[vehicles_to_remove[i]]->getTravelTime(this->inputs));
            }

            // Delete the Vehicle
            delete this->vehicles[vehicles_to_remove[i]];
            this->vehicles.erase(this->vehicles.begin() + vehicles_to_remove[i]);
        }
        vehicles_to_remove.clear();

        // Spawn new Vehicles
        if (rank == 0 )
            this->road_ptr->attemptSpawn(this->inputs, &(this->vehicles), &(this->next_id));
    }

    // Print the total run time and average iterations per second and seconds per iteration
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    auto time_elapsed = (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) /1000000.0;
    std::cout << "--- Simulation Performance ---" << std::endl;
    std::cout << "total computation time: " << time_elapsed << " [s]" << std::endl;
    std::cout << "average time per iteration: " << time_elapsed / inputs.max_time << " [s]" << std::endl;
    std::cout << "average iterating frequency: " << inputs.max_time / time_elapsed << " [iter/s]" << std::endl;

    log_file << "Rank " << rank << ": " << "--- Simulation Performance ---" << std::endl;
    log_file << "Rank " << rank << ": " << "Total computation time: " << time_elapsed << " [s]" << std::endl;
    log_file << "Rank " << rank << ": " << "Average time per iteration: " << time_elapsed / inputs.max_time << " [s]" << std::endl;
    log_file << "Rank " << rank << ": " << "Average iterating frequency: " << inputs.max_time / time_elapsed << " [iter/s]" << std::endl;

#ifdef DEBUG
    // Print final road configuration
    std::cout << "final road configuration" << std::endl;
    log_file << "Rank " << rank << ": " << "Final road configuration" << std::endl;

    this->road_ptr->printRoad(rank, log_file);
#endif

    // Print the average Vehicle time on the Road
    std::cout << "--- Simulation Results ---" << std::endl;
    std::cout << "time on road: avg=" << this->travel_time->getAverage() << ", std="
              << pow(this->travel_time->getVariance(), 0.5) << ", N=" << this->travel_time->getNumSamples()
              << std::endl;

    log_file << "Rank " << rank << ": " << "--- Simulation Results ---" << std::endl;
    log_file << "Rank " << rank << ": " << "time on road: avg=" << this->travel_time->getAverage() << ", std="
                                        << pow(this->travel_time->getVariance(), 0.5) << ", N=" << this->travel_time->getNumSamples()
                                        << std::endl;

    // Return with no errors
    return 0;
}
