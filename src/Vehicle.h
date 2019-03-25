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
    int id;
    int position;
    int speed;
    int max_speed;
    int gap_forward;
    int gap_other_forward;
    int gap_other_backward;
    int look_forward;
    int look_other_forward;
    int look_other_backward;
    double prob_slow_down;
    double prob_change;

public:
    Vehicle(Lane* lane_ptr, int id, int initial_position, Inputs inputs);
    int updateGaps(Road* road_ptr);
    int performLaneSwitch(Road* road_ptr);
    int performLaneMove();
    int getId();

#ifdef DEBUG
    void printGaps();
#endif
};


#endif //CA_TRAFFIC_SIMULATION_VEHICLE_H
