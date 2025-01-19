/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#ifndef CA_TRAFFIC_SIMULATION_VEHICLE_H
#define CA_TRAFFIC_SIMULATION_VEHICLE_H

#include "Inputs.h"
#include "Road.h"
#include "Statistic.h"

// Forward declarations
class Lane;

/**
 * Constructor for a Vehicle in the simulation. Has methods for performing movements based on the CA rules of the
 * simulation.
 */
class Vehicle {
private:
    Lane* lane_ptr;
    int id;
    int position;
    int speed;
    int max_speed;
    int gap_forward;
    int gap_other_forward;
    int gap_other_backward;
    int look_forward;
    int look_other_forward;
    int look_other_backward;
    double prob_slow_down;
    double prob_change;
    int time_on_road;

public:
    Vehicle(Lane* lane_ptr, int id, int initial_position, Inputs inputs);
    ~Vehicle();
    int updateGaps(Road* road_ptr, int rank, int size);
    int performLaneSwitch(Road* road_ptr, int rank, int size);
    int performLaneMove();
    int getId();
    double getTravelTime(Inputs inputs);
    int setSpeed(int speed);
    int getPosition() const;
    int getSpeed() const;
    int getMaxSpeed() const;
    int getGapForward() const;
    int getGapOtherForward() const;
    int getGapOtherBackward() const;
    int getLookForward() const;
    int getLookOtherForward() const;
    int getLookOtherBackward() const;
    double getProbSlowDown() const;
    double getProbChange() const;
    int getTimeOnRoad() const;

    void setMaxSpeed(int max_speed);
    void setGapForward(int gap);
    void setGapOtherForward(int gap);
    void setGapOtherBackward(int gap);
    void setLookForward(int look);
    void setLookOtherForward(int look);
    void setLookOtherBackward(int look);
    void setProbSlowDown(double prob);
    void setProbChange(double prob);
    void setTimeOnRoad(int time);


#ifdef DEBUG
    void printGaps();
#endif
};


#endif //CA_TRAFFIC_SIMULATION_VEHICLE_H
