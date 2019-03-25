/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#ifndef CA_TRAFFIC_SIMULATION_LANE_H
#define CA_TRAFFIC_SIMULATION_LANE_H

#include <vector>
#include <deque>

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
    int addVehicle(unsigned int site, Vehicle* vehicle_ptr);
    int removeVehicle(unsigned int site);
#ifdef DEBUG
    void printLane();
#endif
};


#endif //CA_TRAFFIC_SIMULATION_LANE_H
