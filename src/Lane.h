/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#ifndef CA_TRAFFIC_SIMULATION_LANE_H
#define CA_TRAFFIC_SIMULATION_LANE_H

#include <vector>

// Forward Declarations
class Vehicle;

class Lane {
private:
    std::vector<Vehicle*> sites;
    unsigned int lane_num;
public:
    Lane(unsigned int size, unsigned int lane_num);
    int initializeCars(double percent_full, unsigned int max_speed, std::vector<Vehicle*>* vehicles,
                       unsigned int look_forward, unsigned int look_other_forward, unsigned int look_other_backward);
    unsigned int getSize();
    bool hasVehicleInSite(unsigned int site);
};


#endif //CA_TRAFFIC_SIMULATION_LANE_H
