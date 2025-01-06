/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#include <chrono>
#include <algorithm>
#include <cmath>
#include "Road.h"
#include <mpi.h>
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
            this->vehicles[n]->updateGaps(this->road_ptr, rank, size);
#ifdef DEBUG
            this->vehicles[n]->printGaps(rank, log_file);
#endif
        }

        for (int n = 0; n < (int) this->vehicles.size(); n++) {
            this->vehicles[n]->performLaneSwitch(this->road_ptr, rank, size);
        }

#ifdef DEBUG

        this->road_ptr->printRoad(rank, log_file);
        std::cout << "performing lane movements..." << std::endl;
        log_file << "Rank " << rank << ": " << "Performing lane movements..." << std::endl;

#endif

        // Perform the independent lane updates
        for (int n = 0; n < (int) this->vehicles.size(); n++) {
            this->vehicles[n]->updateGaps(this->road_ptr, rank, size);
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

        // Handle vehicles crossing boundaries
        handle_boundary_vehicles(rank, size, start_site, end_site, log_file);

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

/**
 * Handles vehicles crossing the boundaries of the current segment
 */
void Simulation::handle_boundary_vehicles(int rank, int size, int start_pos, int end_pos, std::ofstream &log_file) {
    std::vector<Vehicle *> outgoing_vehicles;

    for (auto it = vehicles.begin(); it != vehicles.end();) {
        Vehicle *vehicle = *it;
        int position = vehicle->getPosition();

        // Identify vehicles that have moved past the end boundary
        if (position < end_pos && rank < size - 1) {
            log_file << "Rank " << rank << ": Vehicle " << vehicle->getId()
                     << " crossing boundary to rank " << rank + 1 << " at position " << position << "\n";
            outgoing_vehicles.push_back(vehicle);
            it = vehicles.erase(it); // Remove from local list
        } else {
            ++it;
        }
    }

    // Communicate vehicles with the next process
    communicate_vehicles(rank, size, outgoing_vehicles, log_file);
}


/**
 * Communicates vehicles across boundaries using MPI
 */
void Simulation::communicate_vehicles(int rank, int size, std::vector<Vehicle *> &outgoing_vehicles, std::ofstream &log_file) {
    MPI_Status status;

    // Serialize outgoing vehicles into a buffer
    std::vector<double> buffer;
    for (Vehicle *vehicle : outgoing_vehicles) {
        buffer.push_back(vehicle->getId()); // 0: id
        buffer.push_back(vehicle->getPosition()); // 1: position
        buffer.push_back(vehicle->getSpeed()); // 2: speed
        buffer.push_back(vehicle->getMaxSpeed()); // 3: max_speed
        buffer.push_back(vehicle->getGapForward()); // 4: gap_forward
        buffer.push_back(vehicle->getGapOtherForward()); // 5: gap_other_forward
        buffer.push_back(vehicle->getGapOtherBackward()); // 6: gap_other_backward
        buffer.push_back(vehicle->getLookForward()); // 7: look_forward
        buffer.push_back(vehicle->getLookOtherForward()); // 8: look_other_forward
        buffer.push_back(vehicle->getLookOtherBackward()); // 9: look_other_backward
        buffer.push_back(vehicle->getProbSlowDown()); // 10: prob_slow_down
        buffer.push_back(vehicle->getProbChange()); // 11: prob_change
        buffer.push_back(vehicle->getTimeOnRoad()); // 12: time_on_road
    }

    // Send the buffer to the next process
    if (rank < size - 1) {
        int buffer_size = buffer.size();
        MPI_Send(&buffer_size, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
        if (buffer_size > 0) {
            MPI_Send(buffer.data(), buffer_size, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD);
        }
        log_file << "Rank " << rank << ": Sent " << outgoing_vehicles.size() << " vehicles to rank " << rank + 1 << "\n";
    }

    // Receive vehicles from the previous process
    if (rank > 0) {
        log_file << "i am in "   << "\n";
        int buffer_size;
        MPI_Recv(&buffer_size, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
        if (buffer_size > 0) {
            std::vector<double> recv_buffer(buffer_size);
            MPI_Recv(recv_buffer.data(), buffer_size, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, &status);

            // Deserialize and add vehicles to the current process's road
            for (int i = 0; i < buffer_size; i += 13) { // 13 fields per vehicle
                int id = (int)recv_buffer[i];
                int position = (int)recv_buffer[i + 1];
                int speed = (int)recv_buffer[i + 2];
                int max_speed = (int)recv_buffer[i + 3];
                int gap_forward = (int)recv_buffer[i + 4];
                int gap_other_forward = (int)recv_buffer[i + 5];
                int gap_other_backward = (int)recv_buffer[i + 6];
                int look_forward = (int)recv_buffer[i + 7];
                int look_other_forward = (int)recv_buffer[i + 8];
                int look_other_backward = (int)recv_buffer[i + 9];
                double prob_slow_down = recv_buffer[i + 10];
                double prob_change = recv_buffer[i + 11];
                int time_on_road = (int)recv_buffer[i + 12];

                // Adjust position to local coordinates for the receiving process
                int local_position = position - this->start_site;
                log_file << "local_position " << local_position  << "\n";

                // Create a new vehicle
                Vehicle *new_vehicle = new Vehicle(nullptr, id, local_position, this->inputs);
                new_vehicle->setSpeed(speed);
                new_vehicle->setMaxSpeed(max_speed);
                new_vehicle->setGapForward(gap_forward);
                new_vehicle->setGapOtherForward(gap_other_forward);
                new_vehicle->setGapOtherBackward(gap_other_backward);
                new_vehicle->setLookForward(look_forward);
                new_vehicle->setLookOtherForward(look_other_forward);
                new_vehicle->setLookOtherBackward(look_other_backward);
                new_vehicle->setProbSlowDown(prob_slow_down);
                new_vehicle->setProbChange(prob_change);
                new_vehicle->setTimeOnRoad(time_on_road);

                // Determine the lane to add the vehicle
                int lane_number = (local_position < this->road_ptr->getLanes()[0]->getSize()) ? 0 : 1;
                Lane *lane = this->road_ptr->getLanes()[lane_number];

                // Add vehicle to the lane
                lane->addVehicle(local_position, new_vehicle);

                // Add vehicle to the vehicles list
                this->vehicles.push_back(new_vehicle);

                log_file << "Rank " << rank << ": Received and added vehicle with ID " << id << " to lane " << lane_number
                         << " at local position " << local_position << "\n";
            }
        }
    }
}

