/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#ifndef CA_TRAFFIC_SIMULATION_LANE_H
#define CA_TRAFFIC_SIMULATION_LANE_H

#include <vector>
#include <deque>

#include "Inputs.h"
#include "CDF.h"

// Forward Declarations
class Vehicle;

class Lane {
private:
    std::vector<std::deque<Vehicle*>> sites;
    int lane_num;
    int steps_to_spawn;
public:
    Lane(Inputs inputs, int lane_num);
    int initializeCars(Inputs inputs, std::vector<Vehicle*>* vehicles, int* next_id_ptr);
    int getSize();
    int getLaneNumber();
    bool hasVehicleInSite(int site);
    int addVehicle(int site, Vehicle* vehicle_ptr);
    int removeVehicle(int site);
    int attemptSpawn(Inputs inputs, std::vector<Vehicle*>* vehicles, int* next_id_ptr, CDF* interarrival_time_cdf);
#ifdef DEBUG
    void printLane();
#endif
};


#endif //CA_TRAFFIC_SIMULATION_LANE_H
