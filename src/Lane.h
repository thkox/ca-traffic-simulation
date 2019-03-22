/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#ifndef CA_TRAFFIC_SIMULATION_LANE_H
#define CA_TRAFFIC_SIMULATION_LANE_H

#include <deque>
#include <vector>

#include "Inputs.h"

// Forward Declarations
class Vehicle;

class Lane {
private:
    std::vector<std::deque<Vehicle*>> sites;
    unsigned int lane_num;
public:
    Lane(Inputs inputs, unsigned int lane_num);
    int initializeCars(Inputs inputs, std::vector<Vehicle*>* vehicles);
    unsigned int getSize();
    unsigned int getLaneNumber();
    bool hasVehicleInSite(unsigned int site);
    int moveVehicleInLane(unsigned int initial_site, unsigned int final_site);
    int addVehicleInLane(unsigned int site, Vehicle* vehicle_ptr);
    int removeVehicleFromLane(unsigned int site);
#ifdef DEBUG
    void printLane();
#endif
};


#endif //CA_TRAFFIC_SIMULATION_LANE_H
