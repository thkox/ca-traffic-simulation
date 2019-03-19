/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#include <cstdlib>

#include "Lane.h"
#include "Vehicle.h"
#include "Inputs.h"

Lane::Lane(Inputs inputs, unsigned int lane_num) {
#ifdef DEBUG
    std::cout << "creating lane " << lane_num << "...";
#endif
    // Allocate memory in for the vehicle pointers
    this->sites.reserve(inputs.length);
    this->sites.resize(inputs.length);

    // Initialize each Vehicle pointer in the Lane to a null pointer
    for (int i = 0; i < this->sites.size(); i++) {
        this->sites[i] = nullptr;
    }

    // Set the lane number for the lane
    this->lane_num = lane_num;
#ifdef DEBUG
    std::cout << "done, lane " << lane_num << " created with length " << this->sites.size() << std::endl;
#endif
}

int Lane::initializeCars(Inputs inputs, std::vector<Vehicle*>* vehicles) {
    // Initialize cars in the sites of the Lane with given percentage
    for (int i = 0; i < this->sites.size(); i++) {
        if ( ((double) std::rand()) / ((double) RAND_MAX) <= inputs.percent_full ) {
            this->sites[i] = new Vehicle(this, i, inputs);
            vehicles->push_back(this->sites[i]);
        }
    }

    // Return with no errors
    return 0;
}

unsigned int Lane::getSize() {
    return this->sites.size();
}

unsigned int Lane::getLaneNumber() {
    return this->lane_num;
}

bool Lane::hasVehicleInSite(unsigned int site) {
    if (this->sites[site]) {
        return true;
    } else {
        return false;
    }
}

int Lane::moveVehicleInLane(unsigned int initial_site, unsigned int final_site) {
    // Copy the Vehicle pointer from the initial site to the final site
    this->sites[final_site] = this->sites[initial_site];

    // Remove the Vehicle pointer from the initial site
    this->sites[initial_site] = nullptr;

    // Return with zero errors
    return 0;
}

int Lane::addVehicleInLane(unsigned int site, Vehicle* vehicle) {
    // Place the Vehicle object in the Lane at the specified site
    this->sites[site] = vehicle;

    // Return with zero errors
    return 0;
}

int Lane::removeVehicleFromLane(unsigned int site) {
    // Set the pointer to the Vehicle in the specified site to a null pointer
    this->sites[site] = nullptr;

    // Return with zero errors
    return 0;
}

#ifdef DEBUG
void Lane::printLane() {
    for (int i = 0; i < this->getSize(); i++) {
        if (this->sites[i]) {
            std::cout << "1";
        } else {
            std::cout << "0";
        }
    }
    std::cout << std::endl;
}
#endif