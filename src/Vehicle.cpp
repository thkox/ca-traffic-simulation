/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#include <cstdlib>

#include "Vehicle.h"
#include "Lane.h"
#include "Road.h"

Vehicle::Vehicle(Lane* lane_ptr, unsigned int id, unsigned int initial_position, Inputs inputs) {
    // Set the ID number of the Vehicle
    this->id = id;

    // Set the initial position of the Vehicle
    this->position = initial_position;

    // Set the Lane pointer to the pointer to the Lane that contains the Vehicle
    this->lane_ptr = lane_ptr;

    // Set the maximum speed of the Vehicle
    this->max_speed = inputs.max_speed;

    // Set the initial speed of the Vehicle to zero
    this->speed = 0;

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

int Vehicle::updateGaps(Road* road_ptr) {
    // Locate the preceding Vehicle and update the forward gap
    this->gap_forward = this->lane_ptr->getSize() - 1;
    for (int i = 1; i < this->lane_ptr->getSize(); i++) {
        if (this->lane_ptr->hasVehicleInSite((this->position+i) % this->lane_ptr->getSize())) {
            this->gap_forward = i - 1;
        }
    }

    // Determine the lane of interest
    Lane* other_lane_ptr;
    if (this->lane_ptr->getLaneNumber() == 0) {
        other_lane_ptr = road_ptr->getLanes()[1];
    } else {
        other_lane_ptr = road_ptr->getLanes()[0];
    }

    // Update the forward gap in the other lane
    this->gap_other_forward = this->lane_ptr->getSize() - 1;
    for (int i = 0; i < this->lane_ptr->getSize(); i++) {
        if (other_lane_ptr->hasVehicleInSite((this->position+i) % this->lane_ptr->getSize())) {
            this->gap_other_forward = i - 1;
        }
    }

    // Update the backward gap in the other lane
    this->gap_other_backward = this->lane_ptr->getSize() - 1;
    for (int i = 0; i < this->lane_ptr->getSize(); i++) {
        if (other_lane_ptr->hasVehicleInSite((this->position-i) % this->lane_ptr->getSize())) {
            this->gap_other_backward = i - 1;
        }
    }

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
        Lane* other_lane_ptr;
        if (this->lane_ptr->getLaneNumber() == 0) {
            other_lane_ptr = road_ptr->getLanes()[1];
        } else {
            other_lane_ptr = road_ptr->getLanes()[0];
        }

#ifdef DEBUG
        std::cout << "vehicle at " << this->position << " switched lane " << this->lane_ptr->getLaneNumber() <<
            " -> " << other_lane_ptr->getLaneNumber() << std::endl;
#endif

        // Copy the Vehicle pointer to the other Lane
        other_lane_ptr->addVehicle(this->position, this);

        // Remove the Vehicle pointer from the current Lane
        this->lane_ptr->removeVehicle(this->position);

        // Set the pointer to the Lane in the Vehicle to the new lane
        this->lane_ptr = other_lane_ptr;
    }

    // Return with zero errors
    return 0;
}

int Vehicle::performLaneMove() {
    // Update Vehicle speed based on vehicle speed update rules
    if (this->speed != this->max_speed) {
        this->speed++;
#ifdef DEBUG
        std::cout << "vehicle " << this->id << " increased speed " << this->speed - 1 << " -> " << this->speed
            << std::endl;
#endif
    }
    this->speed = std::min(this->speed, this->gap_forward);
    if (this->speed > 0) {
        if ( ((double) rand()) / ((double) RAND_MAX) <= this->prob_slow_down ) {
            this->speed--;
#ifdef DEBUG
            std::cout << "vehicle " << this->id << " decreased speed " << this->speed + 1 << " -> " << this->speed
                << std::endl;
#endif
        }
    }

    if (this->speed > 0) {
        // Compute the new position of the vehicle
        unsigned int new_position = (this->position + this->speed) % this->lane_ptr->getSize();

#ifdef DEBUG
        std::cout << "vehicle " << this->id << " moved " << this->position << " -> " << new_position << std::endl;
#endif

        // Update Vehicle position in the Lane object sites
        this->lane_ptr->addVehicle(new_position, this);

        // Remove vehicle from the old site
        this->lane_ptr->removeVehicle(this->position);

        // Update the Vehicle position value
        this->position = new_position;
    }

    // Return with no errors
    return 0;
}

unsigned int Vehicle::getId() {
    return this->id;
}