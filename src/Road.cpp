/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#include "Road.h"

Road::Road(unsigned int num_lanes, unsigned int length) {
    // Create the Lane objects for the Road
    for (int i = 0; i < num_lanes; i++) {
        this->lanes.push_back(new Lane(length));
    }
}

int Road::initializeCars(double percent_full, unsigned int max_speed, std::vector<Vehicle*>* vehicles) {
    // Initialize each each Lane in the Road with cars with given percentage
    for (int i = 0; i < this->lanes.size(); i++) {
        this->lanes[i]->initializeCars(percent_full, max_speed, vehicles);
    }

    // Return with no errors
    return 0;
}