/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#ifndef CA_TRAFFIC_SIMULATION_VEHICLE_H
#define CA_TRAFFIC_SIMULATION_VEHICLE_H

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

public:
    Vehicle(Lane* lane_ptr, unsigned int initial_position, unsigned int max_speed);
};


#endif //CA_TRAFFIC_SIMULATION_VEHICLE_H
