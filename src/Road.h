/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#ifndef CA_TRAFFIC_SIMULATION_ROAD_H
#define CA_TRAFFIC_SIMULATION_ROAD_H

#include <vector>

#include "Lane.h"

class Road {
private:
    std::vector<Lane*> lanes;
public:
    Road(unsigned int num_lanes, unsigned int length);
    int initializeCars(double percent_full, unsigned int max_speed, std::vector<Vehicle*>* vehicles,
                       unsigned int look_forward, unsigned int look_other_forward, unsigned int look_other_backward,
                       double prob_slow_down);
};


#endif //CA_TRAFFIC_SIMULATION_ROAD_H
