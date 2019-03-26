/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#include "Road.h"
#include "Inputs.h"

Road::Road(Inputs inputs) {
#ifdef DEBUG
    std::cout << "creating new road with " << inputs.num_lanes << " lanes..." << std::endl;
#endif
    // Create the Lane objects for the Road
    for (int i = 0; i < inputs.num_lanes; i++) {
        this->lanes.push_back(new Lane(inputs, i));
    }
#ifdef DEBUG
    std::cout << "done creating road" << std::endl;
#endif
}

Road::~Road() {
    // Delete the Lane objects of the Road
    for (int i = 0; i < (int) this->lanes.size(); i++) {
        delete this->lanes[i];
    }
}

int Road::initializeCars(Inputs inputs, std::vector<Vehicle*>* vehicles) {
    // Initialize each each Lane in the Road with cars with given percentage
    for (int i = 0; i < (int) this->lanes.size(); i++) {
        this->lanes[i]->initializeCars(inputs, vehicles);
    }

    // Return with no errors
    return 0;
}

std::vector<Lane*> Road::getLanes() {
    return this->lanes;
}

#ifdef DEBUG
void Road::printRoad() {
    for (int i = this->lanes.size() - 1; i >= 0; i--) {
        this->lanes[i]->printLane();
    }
}
#endif