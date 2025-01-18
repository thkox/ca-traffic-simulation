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

/**
 * Class for a lane in the road of the simulation. Each lane contains the "sites" for the vehicles and allows access
 * to all the information about the vehicles on the road through its methods.
 */
class Lane {
private:
    std::vector<std::deque<Vehicle*>> sites;
    int lane_num;
    int steps_to_spawn;
    int gap_from_start;
    int gap_from_end;
    int gap_prev_process;
    int gap_next_process;
public:
    Lane(Inputs inputs, int lane_num, int start_site, int end_site, int rank, std::ofstream &log_file);
    int getSize();
    int getLaneNumber();
    bool hasVehicleInSite(int site);
    int addVehicle(int site, Vehicle* vehicle_ptr);
    int removeVehicle(int site);
    int attemptSpawn(Inputs inputs, std::vector<Vehicle*>* vehicles, int* next_id_ptr, CDF* interarrival_time_cdf);
    int getGapFromStart();
    int getGapFromEnd();
    int getGapPrevProcess();
    int getGapNextProcess();
    void setGapPrevProcess(int gap);
    void setGapNextProcess(int gap);
#ifdef DEBUG
    void printLane(int rank, std::ofstream &log_file);
#endif
};


#endif //CA_TRAFFIC_SIMULATION_LANE_H
