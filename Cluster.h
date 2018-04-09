//
// Created by Paul on 3/1/2018.
//

#ifndef RW_CLUSTER_H
#define RW_CLUSTER_H


#include <vector>
#include "Node.h"

class Node;

struct Cluster {
    std::vector<Node *> members;
    int id;
    int delay;
    int calcL1Value();
    Cluster(int);
    //TODO: need lists of input/output Nodes? Clusters?
};


#endif //RW_CLUSTER_H
