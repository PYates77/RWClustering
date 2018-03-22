//
// Created by Paul on 2/13/2018.
// TODO: Create a license or whatever up here so that lovely students after us can reference our nice code
//
#include <iostream>
#include <set>
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
    //number the nodes in order for use in indexing the delay_matrix array
    int id = 0;
    for(std::vector::iterator it = master.begin(); it != master.end(); ++it){
        it->id = id++;
        //apply initial labeling (PI label = delay, non-PI label = 0)
        if(it->prev == NULL) it->label = it->delay;
        else it->label = 0; 
    }


    //////     COMPUTE DELAY MATRIX //////

    // delay_matrix[x][y] = max delay from output x to output y (node delay only)

    int* delay_matrix; //TODO: allocate me

    //TODO: double check this delay_matrix filling algorithm for correctness
    //delay_matrix[N*r+c] (aka delay_matrix[r][c]) represents max delay from node r to node c
    //the matrix entry = 0 if c precedes r in topological order
    std::vector<Node>::const_iterator r; //the current matrix row
    std::vector<Node>::const_iterator c; // the current matrix column
    std::vector<Node>::const_iterator p; //the node's predecessors
    for(r = master.begin(); r != master.end();++r){
        //delay between a node and any previous node (and itself) is 0
        for(c = master.begin(); c != std::next(r,1); ++c) delay_matrix[N * r->id + c->id] = 0; //c!=next(r,1) is because c<=r doesn't work
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


   //////      CALCULATE LABELS    ///////

    // Let Gv be the subgraph containing v and all its predecessors
        // label_v(x) for all x in Gv\{v}
            // label_v(x) = label(x) + delay_matrix[x][v]
    // Let S be the set of nodes in Gv\{v} sorted by decreasing label_v values
        // remove node one-by-one in sorted order  from S and add it to cluster(v) until size constraint is met
        // let l1 = max(label_v) of any PI node in cluster(v)
        // let l2 = max(label_v+delay) of any node remaining in S
        // label(v) = max(l1,l2)


    std::vector<Cluster> clusters;
    for(std::vector::iterator v = master.begin(); v != master.end(); ++v) {

        //skip PIs (label(PI) = delay(pi) already implemented)
        if (v->prev) {
            std::set<Node, compare_lv> Gv;
            v->predecessors_r(Gv); //recursively insert all predecessors (once and only once) into Gv

            // calculate label_v(x)
            for(std::set::iterator x = Gv.begin(); x != Gv.end(); ++x){
                x->label_v = x->label + delay_matrix[N*x->id+v->id];
            }
            std::set<Node, compare_lv> S = Gv; // todo: can't we just use Gv as S, since Gv is already sorted? Do they have to be separate variables?
            Cluster c;

            // pop first element from S and add to c until max cluster size reached
            for(int i=0; i<max_cluster_size; ++i) {
                c.members.push_back(*S.begin());
                S.erase(S.begin());
            }

            v->label = c.max(); //Cluster::max returns label(v) as the max of all label_v+delay (of non PIs) and label_v (of PIs)
            clusters.push_back(c);
        }
    }


    return 0;
}

void addToMaster(std::vector<Node> &m, Node &n){
    std::vector<Node>::iterator it;
    for(it = n.prev.begin(); it != n.prev.end(); ++it){
        if(!it->visited) addToMaster(m, *it); //place ALL predecessors of n onto the list first
    }
    if(n.visited) std::cout << "ERROR: This should never happen. The programmers are clearly morons!" << std::endl; //todo: better error message
    else m.push_back(n); //push n onto the list
    n.visited = true;
    for(it = n.next.begin(); it != n.next.end(); ++it){
        if(!it->visited) addToMaster(m, *it); // place all unvisited successors of n onto the list
    }
}

