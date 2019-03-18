/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#ifndef CA_TRAFFIC_SIMULATION_INPUTS_H
#define CA_TRAFFIC_SIMULATION_INPUTS_H

#include <iostream>

class Inputs {
public:
    unsigned int num_lanes;
    unsigned int length;
    double percent_full;
    unsigned int max_speed;
    unsigned int look_forward;
    unsigned int look_other_forward;
    unsigned int look_other_backward;
    double prob_slow_down;
    double prob_change;
    unsigned int max_time;
    int loadFromFile();
};


#endif //CA_TRAFFIC_SIMULATION_INPUTS_H
