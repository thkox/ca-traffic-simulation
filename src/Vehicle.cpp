/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#include <cstdlib>
#include <iomanip>
#include <fstream>  // For std::ofstream
#include <iostream> // For std::cout
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
int Vehicle::updateGaps(Road* road_ptr, int rank, int size, std::ofstream &log_file) {
    int local_gap_start = this->lane_ptr->getSize();  // Start with the maximum gap
    int local_gap_end = this->lane_ptr->getSize();    // Start with the maximum gap

    // Calculate local_gap_start (gap from start of the lane to the first vehicle)
    for (int i = 0; i < this->lane_ptr->getSize(); i++) {
        if (this->lane_ptr->hasVehicleInSite(i)) {
            local_gap_start = i;
            break;
        }
    }

    // Calculate local_gap_end (gap from last vehicle to the end of the lane)
    for (int i = this->lane_ptr->getSize() - 1; i >= 0; i--) {
        if (this->lane_ptr->hasVehicleInSite(i)) {
            local_gap_end = this->lane_ptr->getSize() - 1 - i;
            break;
        }
    }

#ifdef DEBUG
    std::cout << "Rank " << rank << ": local_gap_start = " << local_gap_start << ", local_gap_end = " << local_gap_end << std::endl;
    log_file << "Rank " << rank << ": local_gap_start = " << local_gap_start << ", local_gap_end = " << local_gap_end << std::endl;
#endif

    int gap_end_recv_from_prev = -1, gap_start_recv_from_next = -1;

    int num_of_req = 0;

    if (rank > 0) {
        num_of_req = 2;
    } else {
        num_of_req = 4;
    }
    MPI_Request requests[num_of_req];
    int request_count = 0;

    if (rank > 0) { // Communicate with previous process
        MPI_Isend(&local_gap_start, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &requests[request_count++]);
        MPI_Irecv(&gap_end_recv_from_prev, 1, MPI_INT, rank - 1, 1, MPI_COMM_WORLD, &requests[request_count++]);
#ifdef DEBUG
        std::cout << "Rank " << rank << ": Sent local_gap_start to rank " << rank - 1 << " and received gap_end_recv_from_prev: " << gap_end_recv_from_prev << std::endl;
        log_file << "Rank " << rank << ": Sent local_gap_start to rank " << rank - 1 << " and received gap_end_recv_from_prev: " << gap_end_recv_from_prev << std::endl;
#endif
    }
    if (rank < size - 1) {
        MPI_Isend(&local_gap_end, 1, MPI_INT, rank + 1, 1, MPI_COMM_WORLD, &requests[request_count++]);
        MPI_Irecv(&gap_start_recv_from_next, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, &requests[request_count++]);
#ifdef DEBUG
        std::cout << "Rank " << rank << ": Sent local_gap_end to rank " << rank + 1 << " and received gap_start_recv_from_next: " << gap_start_recv_from_next << std::endl;
        log_file << "Rank " << rank << ": Sent local_gap_end to rank " << rank + 1 << " and received gap_start_recv_from_next: " << gap_start_recv_from_next << std::endl;
#endif
    }

#ifdef DEBUG
    std::cout << "Rank " << rank << ": Starting MPI communication. gap_end_recv_from_prev = "
              << gap_end_recv_from_prev << ", gap_start_recv_from_next = " << gap_start_recv_from_next << std::endl;
    log_file << "Rank " << rank << ": Starting MPI communication. gap_end_recv_from_prev = "
             << gap_end_recv_from_prev << ", gap_start_recv_from_next = " << gap_start_recv_from_next << std::endl;
    log_file << "Rank " << rank << ": " << "request_count:" << request_count
             << "requests[0]:" << requests[0] << "requests[1]:" << requests[1]
             << "requests[2]:" << requests[2] << "requests[3]:" << requests[3]
    << std::endl;
#endif

    // Wait for all non-blocking operations to complete
    // MPI_Waitall(request_count, requests, MPI_STATUSES_IGNORE);

#ifdef DEBUG
    std::cout << "Rank " << rank << ": Completed MPI communication. gap_end_recv_from_prev = "
              << gap_end_recv_from_prev << ", gap_start_recv_from_next = " << gap_start_recv_from_next << std::endl;
    log_file << "Rank " << rank << ": Completed MPI communication. gap_end_recv_from_prev = "
             << gap_end_recv_from_prev << ", gap_start_recv_from_next = " << gap_start_recv_from_next << std::endl;
#endif

    if (rank > 0 && gap_end_recv_from_prev != -1) {
        this->gap_forward = std::min(this->gap_forward, gap_end_recv_from_prev);
#ifdef DEBUG
        std::cout << "Rank " << rank << ": Updated gap_forward to " << this->gap_forward << std::endl;
        log_file << "Rank " << rank << ": Updated gap_forward to " << this->gap_forward << std::endl;
#endif
    }
    if (rank < size - 1 && gap_start_recv_from_next != -1) {
        this->gap_other_backward = std::min(this->gap_other_backward, gap_start_recv_from_next);
#ifdef DEBUG
        std::cout << "Rank " << rank << ": Updated gap_other_backward to " << this->gap_other_backward << std::endl;
        log_file << "Rank " << rank << ": Updated gap_other_backward to " << this->gap_other_backward << std::endl;
#endif
    }

    std::vector<int> global_gap_start(size, this->lane_ptr->getSize());
    std::vector<int> global_gap_end(size, this->lane_ptr->getSize());

    MPI_Gather(&local_gap_start, 1, MPI_INT, global_gap_start.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Gather(&local_gap_end, 1, MPI_INT, global_gap_end.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);

#ifdef DEBUG
    if (rank == 0) {
        std::cout << "Rank " << rank << ": Gathered global gap data." << std::endl;
        log_file << "Rank " << rank << ": Gathered global gap data." << std::endl;
    }
#endif

    MPI_Bcast(global_gap_start.data(), size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(global_gap_end.data(), size, MPI_INT, 0, MPI_COMM_WORLD);

#ifdef DEBUG
    std::cout << "Rank " << rank << ": Broadcasted global gap data." << std::endl;
    log_file << "Rank " << rank << ": Broadcasted global gap data." << std::endl;
#endif

    if (rank > 0) {
        this->gap_forward = std::min(this->gap_forward, global_gap_end[rank - 1]);
    }
    if (rank < size - 1) {
        this->gap_other_backward = std::min(this->gap_other_backward, global_gap_start[rank + 1]);
    }

#ifdef DEBUG
    std::cout << "Rank " << rank << ": Final gap_forward = " << this->gap_forward
              << ", gap_other_backward = " << this->gap_other_backward << std::endl;
    log_file << "Rank " << rank << ": Final gap_forward = " << this->gap_forward
             << ", gap_other_backward = " << this->gap_other_backward << std::endl;
#endif

    // Update the perceived gaps within the local segment
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
    for (int i = this->position; i < this->lane_ptr->getSize(); i++) {
        if (other_lane_ptr->hasVehicleInSite(i)) {
            this->gap_other_forward = i - this->position - 1;
            break;
        }
    }

    // Update the backward gap in the other lane
    this->gap_other_backward = this->lane_ptr->getSize() - 1;
    for (int i = this->position; i >= 0; i--) {
        if (other_lane_ptr->hasVehicleInSite(i)) {
            this->gap_other_backward = this->position - i - 1;
            break;
        }
    }

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
            return this->time_on_road;
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
void Vehicle::printGaps(int rank, std::ofstream &log_file) {
    std::cout << "vehicle " << std::setw(2) << this->id << " gaps, >:" << this->gap_forward << " ^>:"
        << this->gap_other_forward << " ^<:" << this->gap_other_backward << std::endl;
    log_file << "Rank " << rank << ": " << "vehicle " << std::setw(2) << this->id << " gaps, >:" << this->gap_forward << " ^>:"
        << this->gap_other_forward << " ^<:" << this->gap_other_backward << std::endl;
}
#endif