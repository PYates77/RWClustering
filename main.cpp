//
// Created by Paul on 2/13/2018.
// TODO: Create a license or whatever up here so that lovely students after us can reference our nice code
//
#include <iostream>
#include "Node.h"

int max_cluster_size = 20; //default value = 20

void addToMaster(std::vector<Node> &m, Node &n);

int main(int argc, char **argv) {
    std::cout << "Hello, RWClustering World!" << std::endl;
    //todo: interpret command-line arguments for input file and cluster size limit
    std::vector<Node> PIs;
    std::vector<Node> POs;
    int N; //the number of total nodes
    //todo: construct DAG from BLIF a-la PODEM

    std::vector<Node> master;
    addToMaster(master, PIs.front()); //recursively add all of the nodes to the master list in topological order

    int* delay_matrix; //TODO: allocate me

    //TODO: double check this delay_matrix filling algorithm for correctness
    //delay_matrix[N*r+c] (aka delay_matrix[r][c]) represents max delay from node r to node c
    //the matrix entry = 0 if c precedes r in topological order
    std::vector<Node>::const_iterator r; //the current matrix row
    std::vector<Node>::const_iterator c; // the current matrix column
    std::vector<Node>::const_iterator p; //the node's predecessors
    for(r = master.begin(); r != master.end();++r){
        //delay between a node and any previous node (and itself) is 0
        for(c = master.begin(); c != std::next(r,1); ++c) delay_matrix[N * r->id + c->id] = 0;
        for(c = r; c != master.end(); ++c) {
            //max_delay(r,c) = max( max_delay(r, c->prev) )
            int max=0;
            int pred_delay;
            for(p = p->prev.begin(); p != p->prev.end(); ++p){
                pred_delay = delay_matrix[N*r->id+p->id];
                if(pred_delay > max) max = pred_delay;
            }
            delay_matrix[N*r->id+c->id] = c->delay + max;
        }

    }

    std::vector<Node> L = POs; //L starts equal to the PO set

    return 0;
}

void addToMaster(std::vector<Node> &m, Node &n){
    std::vector<Node>::iterator it;
    for(it = n.prev.begin(); it != n.prev.end(); ++it){
        if(!it->visited) addToMaster(m, *it); //place ALL predecessors of n onto the list first
    }
    if(n.visited) std::cout << "ERROR: This should never happen. The programmers are clearly morons!" << std::endl;
    else m.push_back(n); //push n onto the list
    n.visited = true;
    for(it = n.next.begin(); it != n.next.end(); ++it){
        if(!it->visited) addToMaster(m, *it); // place all unvisited successors of n onto the list
    }
}

