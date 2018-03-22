//
// Created by Paul on 3/1/2018.
//

#ifndef RW_NODE_H
#define RW_NODE_H


#include <vector>
#include <set>
#include "Cluster.h"

struct compare_lv;
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
    int label_v;

    Node(){
        delay = 1;
        visited = false;
    }
    Node(int d){
        delay = d;
        visited = false;
    }
    void predecessors_r(std::set<Node>&);
};


#endif //RW_NODE_H
