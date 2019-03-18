/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#include "Road.h"
#include "Inputs.h"

Road::Road(Inputs inputs) {
    // Create the Lane objects for the Road
    for (int i = 0; i < inputs.num_lanes; i++) {
        this->lanes.push_back(new Lane(inputs, i));
    }
}

Road::~Road() {
    // Delete the Lane objects of the Road
    for (int i = 0; i < this->lanes.size(); i++) {
        delete this->lanes[i];
    }
}

int Road::initializeCars(Inputs inputs, std::vector<Vehicle*>* vehicles) {
    // Initialize each each Lane in the Road with cars with given percentage
    for (int i = 0; i < this->lanes.size(); i++) {
        this->lanes[i]->initializeCars(inputs, vehicles);
    }

    // Return with no errors
    return 0;
}