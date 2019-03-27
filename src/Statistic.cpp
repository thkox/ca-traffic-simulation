/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#include "Statistic.h"

Statistic::Statistic() {}

Statistic::~Statistic() {}

void Statistic::addValue(double value) {
    // Add the value to the vector of values
    this->values.push_back(value);
}

double Statistic::getAverage() {
    // Initialize a sum for the average
    double sum = 0.0;

    // Add each value in the vector of values to the sum
    for (int i = 0; i < (int) this->values.size(); i++) {
        sum += this->values[i];
    }

    // Divide the sum by the number of points and return the average
    return sum / (double) this->values.size();
}