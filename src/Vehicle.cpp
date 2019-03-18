/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#include <cstdlib>

#include "Vehicle.h"
#include "Lane.h"
#include "Road.h"

Vehicle::Vehicle(Lane* lane_ptr, unsigned int initial_position, Inputs inputs) {
    // Set the initial position of the Vehicle
    this->position = initial_position;

    // Set the Lane pointer to the pointer to the Lane that contains the Vehicle
    this->lane_ptr = lane_ptr;

    // Set the maximum speed of the Vehicle
    this->max_speed = inputs.max_speed;

    // Set the look forward distance of the Vehicle
    this->look_forward = inputs.look_forward;

    // Set the other lane look forward distance of the Vehicle
    this->look_other_forward = inputs.look_other_forward;

    // Set the other lane look backward distance of the Vehicle
    this->look_other_backward = inputs.look_other_backward;

    // Set the slow down probability of the Vehicle
    this->prob_slow_down = inputs.prob_slow_down;

    // Set the lane change probability of the Vehicle
    this->prob_change = inputs.prob_change;
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

int Vehicle::performLaneSwitch(Road* road_ptr) {
    // Evaluate if the Vehicle will change lanes and then perform the lane change
    if (this->gap_forward < this->look_forward &&
        this->gap_other_forward > this->look_other_forward &&
        this->gap_other_backward > this->look_other_backward &&
        ((double) rand()) / ((double) RAND_MAX) <= this->prob_change ) {

        // Determine the lane that the Vehicle is switching to
        Lane* other_lane;
        if (this->lane_ptr->getLaneNumber() == 0) {
            other_lane = road_ptr->getLanes()[1];
        } else {
            other_lane = road_ptr->getLanes()[0];
        }

        // Copy the Vehicle pointer to the other Lane
        other_lane->addVehicleInLane(this->position, this);

        // Remove the Vehicle pointer from the current Lane
        this->lane_ptr->removeVehicleFromLane(this->position);
    }

    // Return with zero errors
    return 0;
}

int Vehicle::performLaneMove() {
    // Update Vehicle speed based on vehicle speed update rules
    if (this->speed != this->max_speed) {
        this->speed++;
    }
    this->speed = std::min(this->speed, this->gap_forward);
    if (this->speed > 0) {
        if ( ((double) rand()) / ((double) RAND_MAX) <= this->prob_slow_down ) {
            this->speed--;
        }
    }

    // Compute the new position of the vehicle
    unsigned int new_position = (this->position + this->speed) % this->lane_ptr->getSize();

    // Update Vehicle position in the Lane object sites
    this->lane_ptr->moveVehicleInLane(this->position, new_position);

    // Update the Vehicle position value
    this->position = new_position;

    // Return with no errors
    return 0;
}