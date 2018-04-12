//
// Created by Paul on 2/13/2018.
// TODO: Create a license or whatever up here so that lovely students after us can reference our nice code
//


#include <set>
#include <unistd.h>
#include "Node.h"
#include "Cluster.h"
#include "common.h"
#include <algorithm>

namespace sc = std::chrono;

//TODO: USER PARAMETERS (SHOULD BE COMMAND LINE DRIVEN)
int MAX_CLUSTER_SIZE = 4; //default value = 20
int INTER_CLUSTER_DELAY = 4;
int PRIMARY_INPUT_DELAY = 0;
int PRIMARY_OUTPUT_DELAY = 1;
int NODE_DELAY = 1;
#if (defined(LINUX) || defined(__linux__))
    bool UNIX_RUN = true;
#else
    bool UNIX_RUN = false;
#endif

std::string BLIFFile;

void addPredecessors(std::vector<Node *> &m, Node *n);

int main(int argc, char **argv) {
    if (UNIX_RUN){
        BLIFFile = "example_lecture.blif";
    }
    else {
        BLIFFile = "../example_lecture.blif";
    }

    //parse arguments
    int flag;
    while((flag = getopt(argc, argv, "s:")) != -1){
        switch(flag){
            case 's':
                MAX_CLUSTER_SIZE = std::atoi(optarg);
                break;
        }
    }

    std::cout << "RWClustering Application" << "\nAuthors: Akshay Nagendra <akshaynag@gatech.edu>, Paul Yates <pyates6@gatech.edu>" << std::endl;
    std::cout << "------------------------------------" << std::endl;
    std::cout << "Max Cluster Size = " << MAX_CLUSTER_SIZE << std::endl;
    std::cout << "Inter Cluster Delay = " << INTER_CLUSTER_DELAY << std::endl;
    std::cout << "PI Node Delay = " << PRIMARY_INPUT_DELAY << std::endl;
    std::cout << "Normal Node/PO Node Delay = " << 1 << std::endl;
    std::vector<Node> rawNodeList;

    auto parsestart = sc::high_resolution_clock::now();
    parseBLIF(BLIFFile,PRIMARY_INPUT_DELAY,PRIMARY_OUTPUT_DELAY,NODE_DELAY,rawNodeList);


    //DEBUG
    /*
    std::cout << "NODE ORDER: [";
    int idN = 0;
    for (std::vector<Node>::iterator iN = rawNodeList.begin(); iN < rawNodeList.end(); ++iN){
        std::cout << iN->strID << "(" << idN << "),";
        idN += 1;
    }
    std::cout << "]" << std::endl;
     */

    //todo: interpret command-line arguments for input file and cluster size limit
    // REQUIREMENT: All arrays containing node objects MUST point to rawNodeList. No copies of Nodes may be made at any time.
    std::vector<Node *> PIs = obtainPINodes(rawNodeList);
    std::vector<Node *> POs = obtainPONodes(rawNodeList);
    auto parseEnd = sc::high_resolution_clock::now();

    std::cout << "Parsing and Population (PI & PO) Complete" << std::endl;

    int N = rawNodeList.size(); //the number of total nodes


    std::vector<Node *> master;
    auto topoStart = sc::high_resolution_clock::now();
    for(auto out : POs){ //recursively add all nodes to master in topological order
        addPredecessors(master, out);
    }
    auto topoEnd = sc::high_resolution_clock::now();

    //DEBUG (SHOULD BE DELETED); JUST FOR CHECKING WITH MY HANDWRITTEN SOLUTION
    /*master.clear();
    int indices[12] = {0,1,2,5,6,7,8,9,10,11,3,4};
    for (int i=0; i < 12; ++i){
        master.push_back(&rawNodeList.at(indices[i]));
    }*/


    //number the nodes in order for use in indexing the delay_matrix array

    auto labelInitialStart = sc::high_resolution_clock::now();
    int id = 0;
    for(auto node : master){
        node->id = id++;
        //apply initial labeling (PI label = delay, non-PI label = 0)
        if(!node->prev.empty()) node->label = 0;
        else node->label = node->delay;
    }
    auto labelInitialEnd = sc::high_resolution_clock::now();

    //DEBUG
    /*
    std::cout << "TOPOLOGICAL ORDER: [";
    for (std::vector<Node*>::iterator iM = master.begin(); iM < master.end(); ++iM){
        std::cout << (*iM)->strID << ",";
    }
    std::cout << "]" << std::endl;
    */


    //////     COMPUTE DELAY MATRIX //////

    // delay_matrix[x][y] = max delay from output x to output y (node delay only)

    int* delay_matrix = new int[N*N]; // Delay matrix is NxN square matrix.
    //todo: convert this into a sparse matrix

    //delay_matrix[N*r+c] (aka delay_matrix[r][c]) represents max delay from node r to node c
    //the matrix entry = 0 if c precedes r in topological order
    std::vector<Node *>::const_iterator r; //the node corresponding to the current matrix row
    std::vector<Node *>::const_iterator c; // the node corresponding to the  current matrix column

    auto delayMStart = sc::high_resolution_clock::now();
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
    auto delayMEnd = sc::high_resolution_clock::now();

    // DEBUG
    /*
    std::cout << "DELAY MATRIX:" << std::endl;
    for (auto m : master){
        std::cout << "\t" << m->strID;
    }
    std::cout << std::endl;
    for(int i = 0; i < N; ++i){
        std::cout << master.at(i)->strID;
        for(int j=0; j < N; ++j){
            std::cout << "\t" << delay_matrix[N*i+j];
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


    //todo: consider optimizing this code by changing how and when the ordered set container is used
    std::vector<Cluster> clusters;
    int maxIODelay = 0;

    auto labelClusterStart = sc::high_resolution_clock::now();
    for(auto v : master) {

        std::vector<Node *> S;

        //prepare to add all predecessors of v to Gv by first resetting the visited flag
        for (auto n : master){
            n->visited = false;
        }

        //skip PIs (label(PI) = delay(pi) already implemented)
        if (!v->prev.empty()) {
            for(auto n : v->prev){
                addPredecessors(S, n);
            }
        }

        // calculate label_v(x)
        for(auto x : S){
            x->label_v = x->label + delay_matrix[N*x->id+v->id];
        }

        // copy Gv to S, which forces ordering to occur
        std::sort(S.begin(),S.end(),compare_lv);

        Cluster cl(v->id);
        cl.members.push_back(v);

        // pop first element from S and add to c until max cluster size reached or S is empty
        for(int i=1; i<MAX_CLUSTER_SIZE; ++i) { //i starts at 1 to include initial element already in cluster
            if(S.size() == 0) break;
            cl.members.push_back(*S.begin());
            S.erase(S.begin());
        }

        int L2 = 0;
        if (S.size() != 0){

            L2 = (*S.begin())->label_v + INTER_CLUSTER_DELAY;
        }
        int L1 = cl.calcL1Value();
        //DEBUG
        //std::cout << v->strID << "'s L1 value: " << L1 << std::endl;
        //std::cout << v->strID << "'s L2 value: " << L2 << std::endl;


        v->label = (L1 > L2) ? L1 : L2;
        maxIODelay = (v->label > maxIODelay) ? v->label: maxIODelay;
        generateInputSet(cl);
        clusters.push_back(cl);
    }
    auto labelClusterEnd = sc::high_resolution_clock::now();

    std::cout << "Calculation of Labels and Clusters Complete" << std::endl;

    //DEBUG
    for (auto m : master){
        std::cout << "LABEL FOR NODE " << m->strID << ": " << m->label << std::endl;
    }
    /*
    for(auto cluster : clusters){
        std::cout << "\nCluster " << cluster.id << "(" << master.at(cluster.id)->strID << ") has: " << std::endl;
        for(auto member : cluster.members){
            std::cout << member->id << " (" << member->strID << ")" << std::endl;
        }

        std::cout << "\nCluster " << cluster.id << "(" << master.at(cluster.id)->strID << ") has inputs: " << std::endl;
        for(auto in : cluster.inputSet){
            std::cout << in->id << " (" << in->strID << ")" << std::endl;
        }

    }
    */

    //CLUSTERING PHASE
    std::vector<Cluster *> finalClusterList;
    std::vector<Node *> L;

    std::copy(POs.begin(),POs.end(),std::back_inserter(L)); //Generate L as the set of all POs in the circuit

    auto clusterPhaseStart = sc::high_resolution_clock::now();
    while (!L.empty()){
        //retrieve first element of L and pop from L
        Node *lNode = *L.begin();
        L.erase(L.begin());

        //add cluster to finalClusterList
        Cluster* c = &(clusters.at(lNode->id));
        finalClusterList.push_back(c);

        //add any node in input(lNode's cluster) whose cluster is not in the finalClusterList
        for (auto iNode : c->inputSet){
            if (!Cluster::isClusterInList(iNode->id,finalClusterList) && retrieveNodeByStr_ptr(iNode->strID,L) == nullptr){
                L.push_back(iNode);
            }
        }
    }
    auto clusterPhaseEnd = sc::high_resolution_clock::now();

    //DEBUG
    std::cout << "FINAL CLUSTER LIST: " << std::endl;
    for (auto c : finalClusterList){
        std::cout << "CLUSTER " << master.at(c->id)->strID << ": [";
        for (auto mem : c->members){
            std::cout << mem->strID << ",";
        }
        std::cout << "]" << std::endl;
    }


    std::cout << "PROGRAM COMPLETE" << std::endl;
    //todo: output to file and possibly GUI

    //STATISTICS
    std::vector<std::pair<long long int,std::string>> execTimes;
    std::vector<std::string> execStrs = {"PARSING AND POPULATION OF ALL NODES",
                                         "TOPOLOGICAL SORTING",
                                         "DELAY MATRIX CALCULATION",
                                         "LABEL INITIALIZATION",
                                         "LABEL AND INITIAL CLUSTERING PHASE",
                                         "CLUSTERING PHASE"};

    execTimes.push_back(measureExecTime(parsestart,parseEnd));
    execTimes.push_back(measureExecTime(topoStart,topoEnd));
    execTimes.push_back(measureExecTime(delayMStart,delayMEnd));
    execTimes.push_back(measureExecTime(labelInitialStart,labelInitialEnd));
    execTimes.push_back(measureExecTime(labelClusterStart,labelClusterEnd));
    execTimes.push_back(measureExecTime(clusterPhaseStart,clusterPhaseEnd));

    //print out execution times for each code section
    std::cout << "\n------------------------------------\nSTATISTICS:" << std::endl;
    std::cout << "TOTAL NUMBER OF NODES:\t" << N << std::endl;
    std::cout << "NUMBER OF CLUSTERS:\t" << finalClusterList.size() << std::endl;
    std::cout << "MAX IO PATH DELAY:\t" << maxIODelay << std::endl;
    std::cout << "----------EXECUTION TIMES----------" << std::endl;
    for (int i=0; i < execStrs.size(); ++i){
        std::cout << execStrs.at(i) << ":\t" << execTimes.at(i).first << execTimes.at(i).second << std::endl;
    }

    delete[] delay_matrix;

    return 0;
}

//adds a node to master in topological order by first ensuring that all predecessors have been added to master
void addPredecessors(std::vector<Node *> &m, Node *n){
    for(auto node : n->prev){
        if (!node->visited) addPredecessors(m, node);
    }
    n->visited = true;
    m.push_back(n);
    //std::cout << "Adding " << n->strID  << " to master." << std::endl; //debug
}


