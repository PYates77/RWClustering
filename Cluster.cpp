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
    for(std::vector<Node>::iterator it=members.begin(); it != members.end(); ++it){
        int m = 0;
        if(!it->prev.empty()){ // if *it is not a PI
            m = it->label_v + it->delay;
        }
        else{ //if *it is a PI
            m = it->label_v;
        }
        if (m > currentMax){
            currentMax = m;
        }
    }
    return currentMax;
}
