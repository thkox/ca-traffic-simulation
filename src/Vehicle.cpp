/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#include <cstdlib>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <mpi.h>
#include "Statistic.h"
#include "Vehicle.h"
#include "Lane.h"
#include "Road.h"

/**
 * Constructor for the Vehicle
 * @param lane_ptr pointer to the Lane in which the Vehicle starts in
 * @param id unique ID number of the Vehicle
 * @param initial_position initial site number of the Vehicle in the Lane
 * @param inputs instance of the Inputs class with the simulation inputs
 */
Vehicle::Vehicle(Lane* lane_ptr, int id, int initial_position, Inputs inputs) {
    // Set the ID number of the Vehicle
    this->id = id;

    // Set the initial position of the Vehicle
    this->position = initial_position;

    // Set the Lane pointer to the pointer to the Lane that contains the Vehicle
    this->lane_ptr = lane_ptr;

    // Set the maximum speed of the Vehicle
    this->max_speed = inputs.max_speed;

    // Set the initial speed of the Vehicle to the maximum speed
    this->speed = this->max_speed;

    // Set the look forward distance of the Vehicle
    this->look_forward = this->speed + 1;

    // Set the other lane look forward distance of the Vehicle
    this->look_other_forward = this->look_forward;

    // Set the other lane look backward distance of the Vehicle
    this->look_other_backward = inputs.look_other_backward;

    // Set the slow down probability of the Vehicle
    this->prob_slow_down = inputs.prob_slow_down;

    // Set the lane change probability of the Vehicle
    this->prob_change = inputs.prob_change;

    // Initialize the time spend on the Road
    this->time_on_road = 0;
}

Vehicle::~Vehicle() {}

/**
 * Update the perceived gaps between the Vehicle and the surrounding Vehicles in the Road
 * @param road_ptr pointer to the Road that the Vehicle is in
 * @return 0 if successful, nonzero otherwise
 */
int Vehicle::updateGaps(Road* road_ptr, int rank, int size) {
    // Locate the preceding Vehicle and update the forward gap
    this->gap_forward = this->lane_ptr->getSize() - 1;
    bool found_vehicle_ahead = false;
    for (int i = this->position + 1; i < this->lane_ptr->getSize(); i++) {
        if (this->lane_ptr->hasVehicleInSite(i)) {
            this->gap_forward = i - this->position - 1;
            found_vehicle_ahead = true;
            break;
        }
    }

    if (!found_vehicle_ahead) {
        if (size != 0 && rank < size - 1) {
            this->gap_forward = this->lane_ptr->getSize() - this->position - 1;
        }
        this->gap_forward += this->lane_ptr->getGapNextProcess();
    }

    // Update vehicle look forward distances
    this->look_forward = this->speed + 1;
    this->look_other_forward = this->look_forward;

    // Determine the other lane of interest
    Lane* other_lane_ptr;
    if (this->lane_ptr->getLaneNumber() == 0) {
        other_lane_ptr = road_ptr->getLanes()[1];
    } else {
        other_lane_ptr = road_ptr->getLanes()[0];
    }

    // Update the forward gap in the other lane
    this->gap_other_forward = this->lane_ptr->getSize() - 1;
    found_vehicle_ahead = false;
    for (int i = this->position; i < this->lane_ptr->getSize(); i++) {
        if (other_lane_ptr->hasVehicleInSite(i)) {
            this->gap_other_forward = i - this->position - 1;
            found_vehicle_ahead = true;
            break;
        }
    }


    if (!found_vehicle_ahead) {
        if (rank > 0 && rank < size - 1) {
            this->gap_other_forward = other_lane_ptr->getSize() - this->position - 1;
        }
        this->gap_other_forward += other_lane_ptr->getGapNextProcess();
    }

    // Update the backward gap in the other lane
    this->gap_other_backward = this->lane_ptr->getSize() - 1;
    bool found_vehicle_behind = false;
    for (int i = this->position; i >= 0; i--) {
        if (other_lane_ptr->hasVehicleInSite(i)) {
            this->gap_other_backward = this->position - i - 1;
            found_vehicle_behind = true;
            break;
        }
    }

    if (!found_vehicle_behind) {
        if (rank > 0 && rank < size - 1) {
            this->gap_other_backward = this->position;
        }
        this->gap_other_backward += other_lane_ptr->getGapPrevProcess();
    }

    // Return with zero errors
    return 0;
}

/**
 * Moved the Vehicle to the other Lane in the Road
 * @param road_ptr pointer to the Road in which the Vehicle is on
 * @return 0 if successful, nonzero otherwise
 */
int Vehicle::performLaneSwitch(Road* road_ptr, int rank, int size) {
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
        std::cout << "vehicle " << this->id << " switched lane " << this->lane_ptr->getLaneNumber() << " -> "
            << other_lane_ptr->getLaneNumber() << std::endl;
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

/**
 * Moves the Vehicle to the next site in the current Lane during the time-step based on the speed of the Vehicle
 * @return 0 if successful, nonzero otherwise
 */
int Vehicle::performLaneMove() {
    // Increment the time on road counter
    this->time_on_road++;

    // Update Vehicle speed based on vehicle speed update rules
    if (this->speed != this->max_speed) {
        this->speed++;
#ifdef DEBUG
        std::cout << "vehicle " << this->id << " increased speed " << this->speed - 1 << " -> " << this->speed
                  << std::endl;
#endif
    }

    this->speed = std::min(this->speed, this->gap_forward);
#ifdef DEBUG
    if (this->speed == 0) {
        std::cout << "vehicle " << this->id << " stopped behind preceding vehicle" << std::endl;
    }
#endif

    if (this->speed > 0) {
        if (((double)rand()) / ((double)RAND_MAX) <= this->prob_slow_down) {
            this->speed--;
#ifdef DEBUG
            std::cout << "vehicle " << this->id << " decreased speed " << this->speed + 1 << " -> " << this->speed
                      << std::endl;
#endif
        }
    }

    if (this->speed > 0) {
        // Compute the new position of the vehicle
        int new_position = this->position + this->speed;

        // If the vehicle exceeds the end of the road, remove it from the lane and update its position
        if (new_position >= this->lane_ptr->getSize()) {
#ifdef DEBUG
            std::cout << "vehicle " << this->id << " spent " << this->time_on_road << " steps on the road and exited."
                      << std::endl;
#endif
            // Remove vehicle from the current lane
            this->lane_ptr->removeVehicle(this->position);

            // Update position to reflect that it has left the process's domain
            this->position = new_position;

            // Return the time on the road as an indicator that it has exited
            return -1;
        }

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

    // Return 0 if the vehicle is still within bounds
    return 0;
}


/**
 * Getter method for the ID number of the Vehicle
 * @return
 */
int Vehicle::getId() {
    return this->id;
}

/**
 * Getter method for the total time the Vehicle has spent on the Road
 * @param inputs
 * @return
 */
double Vehicle::getTravelTime(Inputs inputs) {
    return inputs.step_size * this->time_on_road;
}

int Vehicle::getPosition() const {
    return this->position;
}

int Vehicle::getSpeed() const { return this->speed; }
int Vehicle::getMaxSpeed() const { return this->max_speed; }
int Vehicle::getGapForward() const { return this->gap_forward; }
int Vehicle::getGapOtherForward() const { return this->gap_other_forward; }
int Vehicle::getGapOtherBackward() const { return this->gap_other_backward; }
int Vehicle::getLookForward() const { return this->look_forward; }
int Vehicle::getLookOtherForward() const { return this->look_other_forward; }
int Vehicle::getLookOtherBackward() const { return this->look_other_backward; }
double Vehicle::getProbSlowDown() const { return this->prob_slow_down; }
double Vehicle::getProbChange() const { return this->prob_change; }
int Vehicle::getTimeOnRoad() const { return this->time_on_road; }

void Vehicle::setMaxSpeed(int max_speed) { this->max_speed = max_speed; }
void Vehicle::setGapForward(int gap) { this->gap_forward = gap; }
void Vehicle::setGapOtherForward(int gap) { this->gap_other_forward = gap; }
void Vehicle::setGapOtherBackward(int gap) { this->gap_other_backward = gap; }
void Vehicle::setLookForward(int look) { this->look_forward = look; }
void Vehicle::setLookOtherForward(int look) { this->look_other_forward = look; }
void Vehicle::setLookOtherBackward(int look) { this->look_other_backward = look; }
void Vehicle::setProbSlowDown(double prob) { this->prob_slow_down = prob; }
void Vehicle::setProbChange(double prob) { this->prob_change = prob; }
void Vehicle::setTimeOnRoad(int time) { this->time_on_road = time; }


/**
 * Setter method for the speed of the Vehicle
 * @param speed
 * @return
 */
int Vehicle::setSpeed(int speed) {
    this->speed = speed;

    // Return with no errors
    return 0;
}
/**
 * Debug method for printing the gap information of the Vehicle
 */
#ifdef DEBUG
void Vehicle::printGaps() {
    std::cout << "vehicle " << std::setw(2) << this->id << " gaps, >:" << this->gap_forward << " ^>:"
        << this->gap_other_forward << " ^<:" << this->gap_other_backward << std::endl;
}
#endif