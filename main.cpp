//
// Created by Paul on 2/13/2018.
// TODO: Create a license or whatever up here so that lovely students after us can reference our nice code
//
#include <iostream>
#include "Node.h"

int max_cluster_size = 20; //default value = 20

int main(int argc, char **argv) {
    std::cout << "Hello, RWClustering World!" << std::endl;
    //todo: interpret command-line arguments for input file and cluster size limit

    int* delay_matrix;
    std::vector<Node> PIs;
    std::vector<Node> POs;
    std::vector<Node> L = POs; //L starts equal to the PO set


    return 0;
}

