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
Simulation::Simulation(Inputs inputs, int rank, int size) {

    // Calculate the section of the road for this process
    const int length_per_process = inputs.length / size;

    this->start_site =rank * length_per_process;
    this->end_site = (rank + 1) * length_per_process - 1;

    // Create the Road object for the simulation
    this->road_ptr = new Road(inputs, start_site, end_site, rank);

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
int Simulation::run_simulation(int rank, int size) {

    // Obtain the start time
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    // Set the simulation time to zero
    this->time = 0;

    // Declare a vector for vehicles to be removed each step
    std::vector<int> vehicles_to_remove;

    while (this->time < this->inputs.max_time) {

#ifdef DEBUG
        MPI_Barrier(MPI_COMM_WORLD);

        std::cout << "road configuration at time " << time << ":" << std::endl;
        this->road_ptr->printRoad();

        MPI_Barrier(MPI_COMM_WORLD);

        this->road_ptr->printRoad();

        MPI_Barrier(MPI_COMM_WORLD);

        std::cout << "performing lane switches..." << std::endl;
#endif

        this->road_ptr->calculate_gaps_from_neighbor_processes(rank, size);

        // Perform the lane switch step for all vehicles
        for (int n = 0; n < (int) this->vehicles.size(); n++) {
            this->vehicles[n]->updateGaps(this->road_ptr, rank, size);
#ifdef DEBUG
            this->vehicles[n]->printGaps();
#endif
        }

        for (int n = 0; n < (int) this->vehicles.size(); n++) {
            this->vehicles[n]->performLaneSwitch(this->road_ptr, rank, size);
        }

#ifdef DEBUG

        MPI_Barrier(MPI_COMM_WORLD);

        this->road_ptr->printRoad();
        std::cout << "performing lane movements..." << std::endl;

        MPI_Barrier(MPI_COMM_WORLD);

#endif

        // Recalculate gaps after lane switches
        this->road_ptr->calculate_gaps_from_neighbor_processes(rank, size);

        // Perform the independent lane updates
        for (int n = 0; n < (int) this->vehicles.size(); n++) {
            this->vehicles[n]->updateGaps(this->road_ptr, rank, size);
#ifdef DEBUG
            this->vehicles[n]->printGaps();
#endif
        }

        // Handle vehicles crossing boundaries
        handle_boundary_vehicles(rank, size, start_site, end_site);

        for (int n = 0; n < (int) this->vehicles.size(); n++) {
            int move_result = this->vehicles[n]->performLaneMove();

            // If the vehicle has exited, mark it for transfer
            if (move_result == -1) {

                vehicles_to_remove.push_back(n); // Mark for removal
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

    MPI_Barrier(MPI_COMM_WORLD);

    // Calculate the time elapsed for this process
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    double time_elapsed = (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) / 1000000.0;

    // Use MPI_Reduce to find the maximum time_elapsed across all processes
    double max_time_elapsed;
    MPI_Reduce(&time_elapsed, &max_time_elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        // Rank 0 will print the overall execution time
        std::cout << "--- Simulation Performance ---" << std::endl;
        std::cout << "Total computation time (max across all processes): " << max_time_elapsed << " [s]" << std::endl;
        std::cout << "Average time per iteration: " << max_time_elapsed / inputs.max_time << " [s]" << std::endl;
        std::cout << "Average iterating frequency: " << inputs.max_time / max_time_elapsed << " [iter/s]" << std::endl;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    // Return with no errors
    return 0;
}

/**
 * Handles vehicles crossing the boundaries of the current segment
 */
void Simulation::handle_boundary_vehicles(int rank, int size, int start_pos, int end_pos) {
    std::vector<Vehicle *> outgoing_vehicles;
    for (auto it = vehicles.begin(); it != vehicles.end();) {
        Vehicle *vehicle = *it;
        int position = vehicle->getPosition();

        // Identify vehicles that have moved past the end boundary
        if (position + (rank * (inputs.length / size)) >= end_pos) {  // Adjusted condition to include exact boundary crossing

            // Add the vehicle to the outgoing list
            outgoing_vehicles.push_back(vehicle);

            ++it;
        } else {
            ++it;
        }
    }

    // Transfer vehicles to the next process
    communicate_vehicles(rank, size, outgoing_vehicles);
}



/**
 * Communicates vehicles across boundaries using MPI
 */
void Simulation::communicate_vehicles(int rank, int size, std::vector<Vehicle *> &outgoing_vehicles) {

    // Serialize outgoing vehicles into a buffer
    std::vector<double> send_buffer;
    for (Vehicle *vehicle : outgoing_vehicles) {
        send_buffer.push_back(vehicle->getId());
        send_buffer.push_back(0);
        send_buffer.push_back(vehicle->getSpeed());
        send_buffer.push_back(vehicle->getMaxSpeed());
        send_buffer.push_back(vehicle->getGapForward());
        send_buffer.push_back(vehicle->getGapOtherForward());
        send_buffer.push_back(vehicle->getGapOtherBackward());
        send_buffer.push_back(vehicle->getLookForward());
        send_buffer.push_back(vehicle->getLookOtherForward());
        send_buffer.push_back(vehicle->getLookOtherBackward());
        send_buffer.push_back(vehicle->getProbSlowDown());
        send_buffer.push_back(vehicle->getProbChange());
        send_buffer.push_back(vehicle->getTimeOnRoad());
    }

    // Determine send and receive ranks
    int send_rank = (rank < size - 1) ? rank + 1 : MPI_PROC_NULL;
    int recv_rank = (rank > 0) ? rank - 1 : MPI_PROC_NULL;

    // Send and receive buffer sizes
    int send_size = send_buffer.size();
    int recv_size = 0;

    MPI_Sendrecv(
        &send_size, 1, MPI_INT, send_rank, 0,
        &recv_size, 1, MPI_INT, recv_rank, 0,
        MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    // Send and receive vehicle data
    std::vector<double> recv_buffer(recv_size);
    MPI_Sendrecv(
        send_buffer.data(), send_size, MPI_DOUBLE, send_rank, 0,
        recv_buffer.data(), recv_size, MPI_DOUBLE, recv_rank, 0,
        MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    // Deserialize received vehicles
    for (int i = 0; i < recv_size; i += 13) {
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
        int local_position = position;
        if (local_position < 0 || local_position >= (this->end_site - this->start_site)) {
            continue;
        }

        // Determine the lane
        int lane_number = (local_position < this->road_ptr->getLanes()[0]->getSize()) ? 0 : 1;
        if (lane_number < 0 || lane_number >= (int) this->road_ptr->getLanes().size()) {
            continue;
        }

        Lane *lane = this->road_ptr->getLanes()[lane_number];

        // Create and add the vehicle
        Vehicle *new_vehicle = new Vehicle(lane, id, local_position, this->inputs);
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

        lane->addVehicle(local_position, new_vehicle);
        this->vehicles.push_back(new_vehicle);
    }
}








