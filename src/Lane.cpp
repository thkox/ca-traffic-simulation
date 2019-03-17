/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#include <cstdlib>

#include "Lane.h"
#include "Vehicle.h"

Lane::Lane(unsigned int size) {
    // Allocate memory in for the vehicle pointers
    sites.reserve(size);

    // Initialize each Vehicle pointer in the Lane to a null pointer
    for (int i = 0; i < sites.size(); i++) {
        sites[i] = NULL;
    }
}

int Lane::initializeCars(double percent_full) {
    // Initialize cars in the sites of the Lane with given percentage
    for (int i = 0; i < sites.size(); i++) {
        if (std::rand() / RAND_MAX <= 1.0) {
            sites[i] = new Vehicle();
        }
    }

    // Return with no errors
    return 0;
}