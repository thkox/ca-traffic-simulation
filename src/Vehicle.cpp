/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#include "Vehicle.h"

Vehicle::Vehicle(Lane* lane_ptr, unsigned int initial_position, unsigned int max_speed) {
    // Set the initial position of the Vehicle
    this->position = initial_position;

    // Set the Lane pointer to the pointer to the Lane that contains the vehicle
    this->lane_ptr = lane_ptr;

    // Set the maximum speed of the vehicle
    this->max_speed = max_speed;
}