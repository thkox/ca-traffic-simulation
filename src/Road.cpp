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

    this->interarrival_time_cdf = new CDF();
    int status = this->interarrival_time_cdf->read_cdf("interarrival-cdf.dat");
    if (status != 0) {
        throw 1;
    }
}

Road::~Road() {
    // Delete the Lane objects of the Road
    for (int i = 0; i < (int) this->lanes.size(); i++) {
        delete this->lanes[i];
    }
}

int Road::initializeCars(Inputs inputs, std::vector<Vehicle*>* vehicles, int* next_id_ptr) {
    // Initialize each each Lane in the Road with cars with given percentage
    for (int i = 0; i < (int) this->lanes.size(); i++) {
        this->lanes[i]->initializeCars(inputs, vehicles, next_id_ptr);
    }

    // Return with no errors
    return 0;
}

std::vector<Lane*> Road::getLanes() {
    return this->lanes;
}

int Road::attemptSpawn(Inputs inputs, std::vector<Vehicle*>* vehicles, int* next_id_ptr) {
    for (int i = 0; i < (int) this->lanes.size(); i++) {
        this->lanes[i]->attemptSpawn(inputs, vehicles, next_id_ptr, this->interarrival_time_cdf);
    }

    // Return with no errors
    return 0;
}

#ifdef DEBUG
void Road::printRoad() {
    for (int i = this->lanes.size() - 1; i >= 0; i--) {
        this->lanes[i]->printLane();
    }
}
#endif