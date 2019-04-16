/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#include <cstdlib>
#include <cstdio>
#include <sstream>
#include <iomanip>

#include "Lane.h"
#include "Vehicle.h"
#include "Inputs.h"

Lane::Lane(Inputs inputs, int lane_num) {
#ifdef DEBUG
    std::cout << "creating lane " << lane_num << "...";
#endif
    // Allocate memory for the vehicle pointers list
    this->sites.reserve(inputs.length);
    this->sites.resize(inputs.length);

    // Set the lane number for the lane
    this->lane_num = lane_num;
#ifdef DEBUG
    std::cout << "done, lane " << lane_num << " created with length " << this->sites.size() << std::endl;
#endif

    this->steps_to_spawn = 0;
}

int Lane::initializeCars(Inputs inputs, std::vector<Vehicle*>* vehicles, int* next_id_ptr) {
    // Initialize cars in the sites of the Lane with given percentage
    for (int i = 0; i < (int) this->sites.size(); i++) {
        if ( ((double) std::rand()) / ((double) RAND_MAX) <= inputs.percent_full ) {
#ifdef DEBUG
            std::cout << "creating vehicle " << vehicles->size() << " in lane " << this->lane_num << " at site " << i
                << std::endl;
#endif
            this->sites[i].push_back(new Vehicle(this, *next_id_ptr, i, inputs));
            (*next_id_ptr)++;
            vehicles->push_back(this->sites[i].front());
        }
    }

    // Return with no errors
    return 0;
}

int Lane::getSize() {
    return this->sites.size();
}

int Lane::getLaneNumber() {
    return this->lane_num;
}

bool Lane::hasVehicleInSite(int site) {
    return !(this->sites[site].empty());
}

int Lane::addVehicle(int site, Vehicle* vehicle_ptr) {
    // Place the Vehicle in the site
    this->sites[site].push_back(vehicle_ptr);

    // Return with zero errors
    return 0;
}

int Lane::removeVehicle(int site) {
    // Remove the Vehicle from the site
    this->sites[site].pop_front();

    // Return with zero errors
    return 0;
}

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

            // "Schedule" next Vehicle spawn
            this->steps_to_spawn = (int) (interarrival_time_cdf->query() / inputs.step_size);
        }
    } else {
        this->steps_to_spawn--;
    }

    // Return with no error
    return 0;
}

#ifdef DEBUG
void Lane::printLane() {
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