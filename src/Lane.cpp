/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#include <cstdlib>
#include <cstdio>
#include <sstream>
#include <iomanip>
#include <fstream>  // For std::ofstream
#include <iostream> // For std::cout
#include "Lane.h"

#include <mpi.h>

#include "Vehicle.h"
#include "Inputs.h"

/**
 * Constructor for the Lane class
 * @param inputs instance of the Inputs class with simulation inputs
 * @param lane_num the number of lane in the road, starting with zero as the first lane
 */
Lane::Lane(Inputs inputs, int lane_num, int start_site, int end_site, int rank) {
#ifdef DEBUG
    if (rank == 0) {
        std::cout << "creating lane " << lane_num << "...";
    }
#endif
    // Allocate memory for the vehicle pointers list
    this->sites.reserve(end_site - start_site + 1);
    this->sites.resize(end_site - start_site + 1);

    // Set the lane number for the lane
    this->lane_num = lane_num;
#ifdef DEBUG
    if (rank == 0) {
        std::cout << "done, lane " << lane_num << " created with length " << inputs.length << std::endl;
    }
#endif

    this->steps_to_spawn = 0;

    this->gap_prev_process = 0;
    this->gap_next_process = 0;
}

/**
 * Getter method for the number of sites in the Lane
 * @return number of sites in the Lane
 */
int Lane::getSize() {
    return this->sites.size();
}

/**
 * Getter method for the Lane's number
 * @return the number of the Lane
 */
int Lane::getLaneNumber() {
    return this->lane_num;
}

/**
 * Checks if the Lane has a Vehicle in a specific site
 * @param site the site in which to check for a Vehicle
 * @return whether or not the Lane has a Vehicle in the site
 */
bool Lane::hasVehicleInSite(int site) {
    return !(this->sites[site].empty());
}

/**
 * Adds a Vehicle to a site in the Lane
 * @param site which site to add the Vehicle to
 * @param vehicle_ptr pointer to the Vehicle to add to the site
 * @return 0 if successful, nonzero otherwise
 */
int Lane::addVehicle(int site, Vehicle* vehicle_ptr) {
    // Place the Vehicle in the site
    this->sites[site].push_back(vehicle_ptr);

    // Return with zero errors
    return 0;
}

/**
 * Removes a Vehicle from a site in the Lane
 * @param site which site to remove the Vehicle from
 * @return 0 if successful, nonzero otherwise
 */
int Lane::removeVehicle(int site) {
    // Remove the Vehicle from the site
    this->sites[site].pop_front();

    // Return with zero errors
    return 0;
}

/**
 * Attempts to spawn a Vehicle that has entered the Lane at the first site. Uses a CDF to sample to determine whether
 * or not a Vehicle was spawned.
 * @param inputs instance of the Inputs class with the simulation inputs
 * @param vehicles pointer to list of Vehicles to add the spawned Vehicles to
 * @param next_id_ptr pointer to the id number of the next spawned Vehicle
 * @param interarrival_time_cdf CDF of the Vehicle interarrival times
 * @return
 */
int Lane::attemptSpawn(Inputs inputs, std::vector<Vehicle*>* vehicles, int* next_id_ptr, CDF* interarrival_time_cdf) {
    if (this->steps_to_spawn == 0) {
        if (!this->hasVehicleInSite(0)) {
            // Spawn Vehicle
#ifdef DEBUG
            std::cout << "creating vehicle " << (*next_id_ptr) << " in lane " << this->lane_num << " at site " << 0
                      << std::endl;
#endif
            this->sites[0].push_front(new Vehicle(this, *next_id_ptr, 0, inputs));
            (*next_id_ptr)++;
            vehicles->push_back(this->sites[0].front());

            // Randomly choose the Vehicles initial speed to be zero bases in slow down probability
            if (((double) std::rand()) / ((double) RAND_MAX) < inputs.prob_slow_down) {
                vehicles->back()->setSpeed(0);
            }

            // "Schedule" next Vehicle spawn
            this->steps_to_spawn = (int) (interarrival_time_cdf->query() / inputs.step_size);
        }
    } else {
        this->steps_to_spawn--;
    }

    // Return with no error
    return 0;
}

/**
 * Debug function to print the Lane to visualize the sites
 */
#ifdef DEBUG
void Lane::printLane(int rank, int size) {
    std::ostringstream lane_string_stream;
    for (int i = 0; i < (int) this->sites.size(); i++) {
        if (this->sites[i].empty()) {
            lane_string_stream << "[   ]";
        } else {
            lane_string_stream << "[" << std::setw(3) << this->sites[i].front()->getId() << "]";
        }
    }
    std::cout << lane_string_stream.str() << std::endl;
}
#endif

int Lane::getGapFromStart() {
    size_t local_gap_start = this->sites.size();  // Start with the maximum gap

    // Iterate through sites to find the first vehicle
    for (size_t i = 0; i < this->sites.size(); i++) {
        if (!this->sites[i].empty()) {
            local_gap_start = i;  // Found the first vehicle, set gap
            break;
        }
    }

    return static_cast<int>(local_gap_start); // Return the gap from the start of the lane
}

int Lane::getGapFromEnd() {
    size_t local_gap_end = this->sites.size();  // Start with the maximum gap

    // Iterate through sites to find the last vehicle
    for (size_t i = this->sites.size() - 1; i >= 0; i--) {
        if (!this->sites[i].empty()) {
            local_gap_end = this->sites.size() - 1 - i;  // Found the last vehicle, set gap
            break;
        }
    }

    return static_cast<int>(local_gap_end); // Return the gap from the end of the lane
}

int Lane::getGapPrevProcess() {
    return this->gap_prev_process;
}

int Lane::getGapNextProcess() {
    return this->gap_next_process;
}

void Lane::setGapPrevProcess(int gap) {
    this->gap_prev_process = gap;
}

void Lane::setGapNextProcess(int gap) {
    this->gap_next_process = gap;
}