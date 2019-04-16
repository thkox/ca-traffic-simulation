/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#ifndef CA_TRAFFIC_SIMULATION_ROAD_H
#define CA_TRAFFIC_SIMULATION_ROAD_H

#include <vector>

#include "Lane.h"
#include "Inputs.h"
#include "CDF.h"

class Road {
private:
    std::vector<Lane*> lanes;
    CDF* interarrival_time_cdf;
public:
    Road(Inputs inputs);
    ~Road();
    int initializeCars(Inputs inputs, std::vector<Vehicle*>* vehicles, int* next_id_ptr);
    std::vector<Lane*> getLanes();
    int attemptSpawn(Inputs inputs, std::vector<Vehicle*>* vehicles, int* next_id_ptr);

#ifdef DEBUG
    void printRoad();
#endif
};


#endif //CA_TRAFFIC_SIMULATION_ROAD_H
