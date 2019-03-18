/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#ifndef CA_TRAFFIC_SIMULATION_VEHICLE_H
#define CA_TRAFFIC_SIMULATION_VEHICLE_H

#include "Inputs.h"
#include "Road.h"

// Forward declarations
class Lane;

class Vehicle {
private:
    Lane* lane_ptr;
    unsigned int position;
    unsigned int speed;
    unsigned int max_speed;
    unsigned int gap_forward;
    unsigned int gap_other_forward;
    unsigned int gap_other_backward;
    unsigned int look_forward;
    unsigned int look_other_forward;
    unsigned int look_other_backward;
    double prob_slow_down;
    double prob_change;

public:
    Vehicle(Lane* lane_ptr, unsigned int initial_position, Inputs inputs);
    int updateGaps();
    int performLaneSwitch(Road* road_ptr);
    int performLaneMove();
};


#endif //CA_TRAFFIC_SIMULATION_VEHICLE_H
