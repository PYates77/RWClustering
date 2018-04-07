//
// Created by Paul on 3/1/2018.
//

#include "Cluster.h"

// computes a label value from a cluster
// let l1 = max(label_v) of any PI node in cluster(v)
// let l2 = max(label_v+delay) of any node remaining in S
// label(v) = max(l1,l2)
int Cluster::max(void){
    int currentMax = 0;
    for(auto node: members){
        int m = 0;
        if(!node->prev.empty()){ // if *it is not a PI
            m = node->label_v + node->delay;
        }
        else{ //if *it is a PI
            m = node->label_v;
        }
        if (m > currentMax){
            currentMax = m;
        }
    }
    return currentMax;
}
Cluster::Cluster(int id){
    this->id = id;
}