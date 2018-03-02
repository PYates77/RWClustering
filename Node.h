//
// Created by Paul on 3/1/2018.
//

#ifndef RW_NODE_H
#define RW_NODE_H


#include <vector>
#include "Cluster.h"

class Node {
private:
public:

    void computeLabel();
    std::vector<Node> prev;
    std::vector<Node> next;
    Cluster *cluster;
    int delay; //ASSUME INTEGER DELAY
    int id;
    bool visited;
    int label;
    int s_label;
    
    Node(){
        delay = 1;
        visited = false;
    }
    Node(int d){
        delay = d;
        visited = false;
    }
};


#endif //RW_NODE_H
