/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#ifndef CA_TRAFFIC_SIMULATION_SIMULATION_H
#define CA_TRAFFIC_SIMULATION_SIMULATION_H

#include <vector>

#include "Road.h"
#include "Inputs.h"
#include "Statistic.h"

/**
 * Class for the simulation. Has a method for running the simulation.
 */
class Simulation {
private:
    Road* road_ptr;
    int time;
    std::vector<Vehicle*> vehicles;
    Inputs inputs;
    int next_id;
    Statistic* travel_time;
    int start_site;
    int end_site;
public:
    Simulation(Inputs inputs, int rank, int size);
    ~Simulation();
    int run_simulation(int rank, int size);
    void handle_boundary_vehicles(int rank, int size, int start_pos, int end_pos);
    void communicate_vehicles(int rank, int size, std::vector<Vehicle *> &outgoing_vehicles);

};


#endif //CA_TRAFFIC_SIMULATION_SIMULATION_H
