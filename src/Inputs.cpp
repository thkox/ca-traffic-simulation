/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#include <fstream>
#include <iostream>

#include "Inputs.h"

int Inputs::loadFromFile() {
    // Open the input file for reading the simulation inputs
    std::fstream input_file;
    input_file.open("cats-inuput.txt", std::fstream::in);

    // Check of the input file was loaded properly
    if (!input_file) {
        std::cout << "error: failure to open \"cats-input.txt\" file!" << std::endl;
        return 1;
    }

    // TODO: Read in inputs to member variables

    // Close the input file
    input_file.close();

    // Return with zero errors
    return 0;
}