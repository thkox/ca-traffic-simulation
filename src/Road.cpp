/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#include "Road.h"

Road::Road(unsigned int num_lanes, unsigned int length) {
    // Create the Lane objects for the Road
    for (int i = 0; i < num_lanes; i++) {
        this->lanes.push_back(new Lane(length, i));
    }
}

int Road::initializeCars(double percent_full, unsigned int max_speed, std::vector<Vehicle*>* vehicles,
                         unsigned int look_forward, unsigned int look_other_forward,
                         unsigned int look_other_backward, double prob_slow_down) {
    // Initialize each each Lane in the Road with cars with given percentage
    for (int i = 0; i < this->lanes.size(); i++) {
        this->lanes[i]->initializeCars(percent_full, max_speed, vehicles, look_forward, look_other_forward,
                                       look_other_backward, prob_slow_down);
    }

    // Return with no errors
    return 0;
}