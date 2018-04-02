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
    int delay;
    int max(void);
    //TODO: need lists of input/output Nodes? Clusters?
};


#endif //RW_CLUSTER_H
