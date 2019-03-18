/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#include "Vehicle.h"
#include "Lane.h"

Vehicle::Vehicle(Lane* lane_ptr, unsigned int initial_position, unsigned int max_speed, unsigned int look_forward,
                 unsigned int look_other_forward, unsigned int look_other_backward) {
    // Set the initial position of the Vehicle
    this->position = initial_position;

    // Set the Lane pointer to the pointer to the Lane that contains the Vehicle
    this->lane_ptr = lane_ptr;

    // Set the maximum speed of the Vehicle
    this->max_speed = max_speed;

    // Set the look forward distance of the Vehicle
    this->look_forward = look_forward;

    // Set the other lane look forward distance of the Vehicle
    this->look_other_forward = look_other_forward;

    // Set the other lane look backward distance of the Vehicle
    this->look_other_backward = look_other_backward;
}

unsigned int Vehicle::getForwardGap() {
    // Locate the preceding Vehicle and return the gap between the current Vehicle and the preceding Vehicle
    for (int i = 1; i < this->lane_ptr->getSize(); i++) {
        if (this->lane_ptr->hasVehicleInSite((this->position+i) % this->lane_ptr->getSize())) {
            return i - 1;
        }
    }
    return this->lane_ptr->getSize() - 1;
}