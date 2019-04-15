/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>

#include "Inputs.h"

std::string parseLine(std::string line) {
    return line.substr(0, line.find(' '));
}

int Inputs::loadFromFile() {
    // Open the input file for reading the simulation inputs
    std::fstream input_file;
    input_file.open("cats-input.txt", std::fstream::in);

    // Check of the input file was loaded properly
    if (!input_file) {
        std::cout << "error: failure to open \"cats-input.txt\" file!" << std::endl;
        return 1;
    }

    // Split the lines in the input file into a vector of strings
    std::vector<std::string> input_lines;
    std::string line;
    while (std::getline(input_file, line))
    {
        input_lines.push_back(line);
    }

    // Parse each line of the input file into the variable it corresponds to
    this->num_lanes           = std::stoi(parseLine(input_lines[0]));
    this->length              = std::stoi(parseLine(input_lines[1]));
    this->percent_full        = std::stod(parseLine(input_lines[2]));
    this->max_speed           = std::stoi(parseLine(input_lines[3]));
    this->look_forward        = std::stoi(parseLine(input_lines[4]));
    this->look_other_forward  = std::stoi(parseLine(input_lines[5]));
    this->look_other_backward = std::stoi(parseLine(input_lines[6]));
    this->prob_slow_down      = std::stod(parseLine(input_lines[7]));
    this->prob_change         = std::stod(parseLine(input_lines[8]));
    this->max_time            = std::stoi(parseLine(input_lines[9]));
    this->step_size           = std::stod(parseLine(input_lines[10]));

    // Close the input file
    input_file.close();

    // Return with zero errors
    return 0;
}