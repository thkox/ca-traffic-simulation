/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#ifndef CA_TRAFFIC_SIMULATION_STATISTIC_H
#define CA_TRAFFIC_SIMULATION_STATISTIC_H

#include <vector>

class Statistic {
private:
    std::vector<double> values;
public:
    Statistic();
    ~Statistic();
    void addValue(double value);
    double getAverage();
    double getVariance();
    int getNumSamples();
};


#endif //CA_TRAFFIC_SIMULATION_STATISTIC_H
