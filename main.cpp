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
    for(auto out : POs){ //recursively add all nodes to master in topological order
        addToMaster(master, out);
    }
    //number the nodes in order for use in indexing the delay_matrix array
    int id = 0;
    for(auto node : master){
        node->id = id++;
        //apply initial labeling (PI label = delay, non-PI label = 0)
        if(!node->prev.empty()) node->label = 0;
        else node->label = node->delay;
    }


    //////     COMPUTE DELAY MATRIX //////

    // delay_matrix[x][y] = max delay from output x to output y (node delay only)

    int* delay_matrix = new int[N*N]; // Delay matrix is NxN square matrix.
    //todo: convert this into a sparse matrix

    //delay_matrix[N*r+c] (aka delay_matrix[r][c]) represents max delay from node r to node c
    //the matrix entry = 0 if c precedes r in topological order
    std::vector<Node *>::const_iterator r; //the node corresponding to the current matrix row
    std::vector<Node *>::const_iterator c; // the node corresponding to the  current matrix column
    for(r = master.begin(); r != master.end();++r){ // iterate across every row
        //delay between a node and any previous node (and itself) is 0
        for(c = master.begin(); c != r+1; ++c) delay_matrix[N * (*r)->id + (*c)->id] = 0;
        for(c = r+1; c != master.end(); ++c) {
            //max_delay(r,c) = max( max_delay(r, c->prev) )
            int max=0;
            int prev_delay;
            for(Node* p : (*c)->prev){
                prev_delay = delay_matrix[N*(*r)->id+p->id];
                if(prev_delay > max) {
                    max = prev_delay;
                    //std::cout << "Found that " << (*c)->strID << "'s predecessor, " << (*p)->strID << ", has delay " << prev_delay << " to " << (*r)->strID << std::endl; //debug
                }
            }
            //if no predecessors of c have a delay to r, then either r is a direct predecessor, or there is no link
            if(max == 0) {
                delay_matrix[N * (*r)->id + (*c)->id] = 0;
                for (Node *p : (*c)->prev) {
                    if (p->id == (*r)->id) {
                        delay_matrix[N * (*r)->id + (*c)->id] = (*c)->delay;
                    }
                }
            }
            else{
                delay_matrix[N * (*r)->id + (*c)->id] = (*c)->delay + max;
            }
            //std::cout << "Delay from " << (*r)->strID << " to " << (*c)->strID << " is " << delay_matrix[N*(*r)->id+(*c)->id]<< std::endl; //debug
        }

    }
    // DEBUG
    /*
    std::cout << "DELAY MATRIX:" << std::endl;
    for(int i = 0; i < N; ++i){
        for(int j=0; j < N; ++j){
            std::cout << delay_matrix[N*i+j];
        }
        std::cout << std::endl;
    }
     */


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

    // right now this algorithm is answering these questions with yes and yes

    std::vector<Cluster> clusters;
    for(auto v : master) {

        std::set<Node *, compare_lv> Gv;

        //skip PIs (label(PI) = delay(pi) already implemented)
        if (!v->prev.empty()) {
            v->predecessors_r(Gv); //recursively insert all predecessors (once and only once) into Gv
        }

        // copy Gv to S, a vector. Useful because we cannot directly modify set members.
        std::vector<Node *> S;
        std::copy(Gv.begin(), Gv.end(), std::back_inserter(S));

        // calculate label_v(x)
        for(auto x : S){
            x->label_v = x->label + delay_matrix[N*x->id+v->id];
        }


        Cluster cl(v->id);
        cl.members.push_back(v);

        // pop first element from S and add to c until max cluster size reached or S is empty
        for(int i=0; i<max_cluster_size; ++i) {
            if(S.size() == 0) break;
            cl.members.push_back(*S.begin());
            S.erase(S.begin());
        }

        (v)->label = cl.max(); //Cluster::max returns label(v) as the max of all label_v+delay (of non PIs) and label_v (of PIs)
        clusters.push_back(cl);
    }

    std::cout << "Calculation of Labels and Clusters Complete" << std::endl;

    //DEBUG

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

//adds a node to master in topological order by first ensuring that all predecessors have been added to master
void addToMaster(std::vector<Node *> &m, Node *n){
    for(auto node : n->prev){
        if (!node->visited) addToMaster(m, node);
    }
    n->visited = true;
    m.push_back(n);
    //std::cout << "Adding " << n->strID  << " to master." << std::endl; //debug
}


