//
// Created by Paul on 2/13/2018.
// TODO: Create a license or whatever up here so that lovely students after us can reference our nice code
//
#include <iostream>
#include <fstream>
#include <set>
#include "Node.h"
#include "Cluster.h"
#include "common.h"

int max_cluster_size = 20; //default value = 20
std::string BLIFFile = "../example_lecture.blif";

void addToMaster(std::vector<Node *> &m, Node *n);

int main(int argc, char **argv) {
    std::cout << "Hello, RWClustering World!" << std::endl;
    std::vector<Node> rawNodeList;
    parseBLIF(BLIFFile,rawNodeList);
    std::cout << "Parsing Complete" << std::endl;

    //todo: interpret command-line arguments for input file and cluster size limit
    // REQUIREMENT: All arrays containing node objects MUST point to rawNodeList. No copies of Nodes may be made at any time.
    std::vector<Node *> PIs = obtainPINodes(rawNodeList);
    std::vector<Node *> POs = obtainPONodes(rawNodeList);

    std::cout << "Populated PI and PO lists" << std::endl;

    int N = rawNodeList.size(); //the number of total nodes

    std::vector<Node *> master;
    addToMaster(master, PIs.front()); //recursively add all of the nodes to the master list in topological order
    //number the nodes in order for use in indexing the delay_matrix array
    int id = 0;
    for(auto it = master.begin(); it != master.end(); ++it){
        (*it)->id = id++;
        //apply initial labeling (PI label = delay, non-PI label = 0)
        if(!(*it)->prev.empty()) (*it)->label = 0;
        else (*it)->label = (*it)->delay;
    }

    //ADDED BY AKSHAY
    std::cout << "TOPOLOGICAL ORDER: [";
    for (auto it = master.begin(); it != master.end(); ++it){
        std::cout << (*it)->strID;
        if (it == master.end() -1){
            std::cout << "]" << std::endl;
        }
        else {
            std::cout << ",";
        }
    }
    std::cout << "REFERENCE LECTURE ORDER (EXAMPLE): [D,E,F,G,H,I,J,K,L]" << std::endl;


    //////     COMPUTE DELAY MATRIX //////

    // delay_matrix[x][y] = max delay from output x to output y (node delay only)

    int* delay_matrix = new int[N*N]; // Delay matrix is NxN square matrix.
    //todo: convert this into a sparse matrix

    //TODO: double check this delay_matrix filling algorithm for correctness
    //delay_matrix[N*r+c] (aka delay_matrix[r][c]) represents max delay from node r to node c
    //the matrix entry = 0 if c precedes r in topological order
    std::vector<Node *>::const_iterator r; //the node corresponding to the current matrix row
    std::vector<Node *>::const_iterator c; // the node corresponding to the  current matrix column
    std::vector<Node *>::const_iterator p; //the node's predecessors
    for(r = master.begin(); r != master.end();++r){ // iterate across every row
        //delay between a node and any previous node (and itself) is 0
        for(c = master.begin(); c != std::next(r,1); ++c) delay_matrix[N * (*r)->id + (*c)->id] = 0; //c!=next(r,1) is because c<=r doesn't work
        for(c = r; c != master.end(); ++c) {
            //max_delay(r,c) = max( max_delay(r, c->prev) )
            int max=0;
            int prev_delay;
            for(p = (*c)->prev.begin(); p != (*c)->prev.end(); ++p){
                prev_delay = delay_matrix[N*(*r)->id+(*p)->id];
                if(prev_delay > max) max = prev_delay;
            }
            delay_matrix[N*(*r)->id+(*c)->id] = (*c)->delay + max;
        }

    }
    //ADDED BY AKSHAY (DEBUG)
    //Print out NXN Delay Matrix
    std::cout << N << "x" << N << " DELAY MATRIX" << std::endl;
    for (int i=0; i < N; ++i){
        for (int j=0; j < N; ++j){
            std::cout << delay_matrix[i*j] << "\t";
        }
        std::cout << std::endl;
    }

    std::cout << "Delay Matrix Calculation Complete" << std::endl;

   //////      CALCULATE LABELS    ///////

    // Let Gv be the subgraph containing v and all its predecessors
        // label_v(x) for all x in Gv\{v}
            // label_v(x) = label(x) + delay_matrix[x][v]
    // Let S be the set of nodes in Gv\{v} sorted by decreasing label_v values
        // remove node one-by-one in sorted order  from S and add it to cluster(v) until size constraint is met
        // let l1 = max(label_v) of any PI node in cluster(v)
        // let l2 = max(label_v+delay) of any node remaining in S
        // label(v) = max(l1,l2)

    //todo: QUESTION. Do PI's get their own cluster? Does the vertex v need to be inserted into it's cluster?
    // right now this algorithm is answering these questions with yes and yes

    std::vector<Cluster> clusters;
    for(auto v = master.begin(); v != master.end(); ++v) {

        std::set<Node *, compare_lv> Gv;

        //skip PIs (label(PI) = delay(pi) already implemented)
        if (!(*v)->prev.empty()) {
            (*v)->predecessors_r(Gv); //recursively insert all predecessors (once and only once) into Gv
        }

        // copy Gv to S, a vector. Useful because we cannot directly modify set members.
        std::vector<Node *> S;
        std::copy(Gv.begin(), Gv.end(), std::back_inserter(S));

        // calculate label_v(x)
        for(auto x = S.begin(); x != S.end(); ++x){
            (*x)->label_v = (*x)->label + delay_matrix[N*(*x)->id+(*v)->id];
        }

        Cluster cl((*v)->id);
        cl.members.push_back(*v);

        // pop first element from S and add to c until max cluster size reached or S is empty
        for(int i=0; i<max_cluster_size; ++i) {
            if(S.size() == 0) break;
            cl.members.push_back(*S.begin());
            S.erase(S.begin());
        }

        (*v)->label = cl.max(); //Cluster::max returns label(v) as the max of all label_v+delay (of non PIs) and label_v (of PIs)
        clusters.push_back(cl);
    }

    std::cout << "Calculation of Labels and Clusters Complete" << std::endl;

    for(auto cluster : clusters){
        std::cout << "\nCluster " << cluster.id << " has: " << std::endl;
        for(auto member : cluster.members){
            std::cout << member->id << " (" << member->strID << ")" << std::endl;
        }
    }


    //todo: output to file and possibly GUI

    delete[] delay_matrix;


    return 0;
}

void addToMaster(std::vector<Node *> &m, Node *n){
    if(n->visited) {
        std::cout << "ERROR: Attempted to add a visited node to master. This should never happen!" << std::endl;
        return;
    }
    n->visited = true;
    std::vector<Node *>::iterator it;
    for(it = n->prev.begin(); it != n->prev.end(); ++it){
        if(!(*it)->visited) addToMaster(m, *it); //place ALL predecessors of n onto the list first
    }
    m.push_back(n); //push n onto the list
    for(it = n->next.begin(); it != n->next.end(); ++it){
        if(!(*it)->visited) addToMaster(m, *it); // place all unvisited successors of n onto the list
    }
}

