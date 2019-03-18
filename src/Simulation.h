/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#ifndef CA_TRAFFIC_SIMULATION_SIMULATION_H
#define CA_TRAFFIC_SIMULATION_SIMULATION_H

#include <vector>

#include "Road.h"

class Simulation {
private:
    Road* road_ptr;
    unsigned int time;
    std::vector<Vehicle*> vehicles;
public:
    Simulation(unsigned int num_lanes, unsigned int length, double percent_full, unsigned int max_speed);
    int run_simulation(unsigned int max_time);
};


#endif //CA_TRAFFIC_SIMULATION_SIMULATION_H
