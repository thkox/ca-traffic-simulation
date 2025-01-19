/*
 * Copyright (C) 2019 Maitreya Venkataswamy - All Rights Reserved
 */

#include "Road.h"
#include "Inputs.h"
#include <fstream>  // For std::ofstream
#include <iostream> // For std::cout
#include <mpi.h>

/**
 * Constructor for the Road
 * @param inputs instance of the Inputs class with simulation inputs
 */
Road::Road(Inputs inputs, int start_site, int end_site, int rank) {
#ifdef DEBUG
    std::cout << "creating new road with " << inputs.num_lanes << " lanes..." << std::endl;
#endif
    // Create the Lane objects for the Road
    for (int i = 0; i < inputs.num_lanes; i++) {
        this->lanes.push_back(new Lane(inputs, i, start_site, end_site, rank));
    }
#ifdef DEBUG
    std::cout << "done creating road" << std::endl;
#endif

    this->interarrival_time_cdf = new CDF();
    int status = this->interarrival_time_cdf->read_cdf("interarrival-cdf.dat");
    if (status != 0) {
        throw std::exception();
    }
}

/**
 * Destructor of the Road
 */
Road::~Road() {
    // Delete the Lane objects of the Road
    for (int i = 0; i < (int) this->lanes.size(); i++) {
        delete this->lanes[i];
    }
}

/**
 * Getter for the Lanes of the road
 * @return
 */
std::vector<Lane*> Road::getLanes() {
    return this->lanes;
}

/**
 * Attempts to spawn Vehicles on each Lane of the Road
 * @param inputs instance of the Inputs class with the simulation Inputs
 * @param vehicles pointer to the array of Vehicles that exist
 * @param next_id_ptr pointer to the id of the next spawned Vehicle
 * @return 0 if successful, nonzero otherwise
 */
int Road::attemptSpawn(Inputs inputs, std::vector<Vehicle*>* vehicles, int* next_id_ptr) {
    for (int i = 0; i < (int) this->lanes.size(); i++) {
        this->lanes[i]->attemptSpawn(inputs, vehicles, next_id_ptr, this->interarrival_time_cdf);
    }

    // Return with no errors
    return 0;
}

/**
 * Debug function to print all the Lanes of the Road for visualizing the sites in the Road
 */
#ifdef DEBUG
void Road::printRoad() {
    for (int i = this->lanes.size() - 1; i >= 0; i--) {
        this->lanes[i]->printLane();

    }
}
#endif

/**
 * Calculates the gaps between vehicles in the road
 * @param rank the rank of the process
 * @param size the number of processes
 * @param ofstream the output file stream
 */
void Road::calculate_gaps_from_neighbor_processes(int rank, int size) {
    // Temporary variables to store received gap values from neighbors


    // Iterate through all lanes of the current process
    for (auto lane : this->lanes) {
        int gap_last_vehicle_to_end = lane->getGapFromEnd();
        int gap_start_to_first_vehicle = lane->getGapFromStart();

        int received_gap_start_to_first_vehicle = -1;
        int received_gap_last_vehicle_to_end = -1;

        int TAG_GAP_LAST_TO_END = 6;
        int TAG_GAP_START_TO_FIRST = 8;

        if (rank > 0) {  // There is a previous process (rank - 1)
            MPI_Send(&gap_start_to_first_vehicle, 1, MPI_INT, rank - 1, TAG_GAP_START_TO_FIRST, MPI_COMM_WORLD);
        }
        if (rank < size - 1) {  // There is a next process (rank + 1)
            MPI_Send(&gap_last_vehicle_to_end, 1, MPI_INT, rank + 1, TAG_GAP_LAST_TO_END, MPI_COMM_WORLD);
        }

        if (rank > 0) {  // If there is a previous process
            MPI_Recv(&received_gap_last_vehicle_to_end, 1, MPI_INT, rank - 1, TAG_GAP_LAST_TO_END, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            lane->setGapPrevProcess(received_gap_last_vehicle_to_end);
        }
        if (rank < size - 1) {  // If there is a next process
            MPI_Recv(&received_gap_start_to_first_vehicle, 1, MPI_INT, rank + 1, TAG_GAP_START_TO_FIRST, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            lane->setGapNextProcess(received_gap_start_to_first_vehicle);
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
}
