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
    int initializeCars(double percent_full);
};


#endif //CA_TRAFFIC_SIMULATION_ROAD_H
