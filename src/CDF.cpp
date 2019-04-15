/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#include "CDF.h"

#include <fstream>
#include <string>
#include <iostream>

int CDF::read_cdf(std::string file_name) {
    // Open the file containing the CDF information
    std::ifstream file(file_name);

    // Check of the input file was loaded properly
    if (!file) {
        std::cout << "error: failure to open "<< file_name <<  " file!" << std::endl;
        return 1;
    }

    // Read each line into the CDF information
    std::string line;
    while (std::getline(file, line))
    {
        this->x.push_back(std::stod(line.substr(0, line.find(','))));
        this->cdf.push_back(std::stod(line.substr(1, line.find(','))));
    }

    // Close the file
    file.close();

    // Return with no errors
    return 0;
}