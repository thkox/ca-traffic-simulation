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

int Vehicle::updateGaps() {
    // Locate the preceding Vehicle and update the forward gap
    for (int i = 1; i < this->lane_ptr->getSize(); i++) {
        if (this->lane_ptr->hasVehicleInSite((this->position+i) % this->lane_ptr->getSize())) {
            this->gap_forward = i - 1;
        }
    }
    this->gap_forward = this->lane_ptr->getSize() - 1;

    // TODO: Update other lane forward gap

    // TODO: Update other lane backward gap

    // Return with zero errors
    return 0;
}

int Vehicle::performLaneSwitch() {
    // TODO: Implement vehicle lane switch

    // Return with zero errors
    return 0;
}

int Vehicle::performLaneMove() {
    // TODO: Implement vehicle lane move

    // Return with no errors
    return 0;
}