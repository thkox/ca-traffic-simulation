/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#include <cstdlib>

#include "Lane.h"
#include "Vehicle.h"

Lane::Lane(unsigned int size) {
    // Allocate memory in for the vehicle pointers
    this->sites.reserve(size);

    // Initialize each Vehicle pointer in the Lane to a null pointer
    for (int i = 0; i < this->sites.size(); i++) {
        this->sites[i] = nullptr;
    }
}

int Lane::initializeCars(double percent_full, unsigned int max_speed) {
    // Initialize cars in the sites of the Lane with given percentage
    for (int i = 0; i < this->sites.size(); i++) {
        if (std::rand() / RAND_MAX <= 1.0) {
            this->sites[i] = new Vehicle(this, i, max_speed);
        }
    }

    // Return with no errors
    return 0;
}