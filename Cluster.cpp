//
// Created by Paul on 3/1/2018.
//

#include "Cluster.h"

// computes a L1 value from a cluster
// let l1 = max(label_v) of any PI node in cluster(v)
int Cluster::calcL1Value(){
    int currentMax = 0;
    for(auto node: members){
        int m = 0;
        if(node->isPI) { // if *it is not a PI
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

void Cluster::generateInputSet(){
    /*
    //Description: generates the input() set for a cluster
    std::copy(this->members.begin(),this->members.end(),std::back_inserter(this->inputSet));

    for(auto cNode : this->members){
        for (auto pNode : cNode->prev){
            //check if node isn't already part of input set
            Node *nPtr = retrieveNodeByStr_ptr(pNode->strID,this->inputSet);
            if (nPtr == nullptr){
                this->inputSet.push_back(pNode);
            }
        }
    }

    //delete the cluster elements from the inputSet
    this->inputSet.erase(this->members.begin(),this->members.begin() + this->members.size());
    */
 }