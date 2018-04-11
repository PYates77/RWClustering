//
// Created by Paul on 3/1/2018.
//

#ifndef RW_NODE_H
#define RW_NODE_H


#include <vector>
#include <set>
#include <string>

struct compare_lv;

class Node {
private:
public:
    void computeLabel();
    std::vector<Node *> prev;
    std::vector<Node *> next;
    int delay; //ASSUME INTEGER DELAY
    int id;
    bool visited;
    int label = 0;
    int label_v = 0;

    bool isPI = false;
    bool isPO = false;
    std::string strID = "INVALID";  //TODO: RIP OUT LATER (DEBUG)
    std::string procStr = ""; //used for second run through during BLIF parsing
    Node *addr = nullptr;

    Node(){
        delay = 1;
        visited = false;
    }
    Node(int d){
        delay = d;
        visited = false;
    }
    void predecessors_r(std::set<Node *, compare_lv>&);
};

// for ordering nodes in S set, nodes are ordered first by label, then by ID
struct compare_lv{
    bool operator() (const Node* lhs, const Node* rhs) const {
        if(lhs->label_v == rhs->label_v){
            return lhs->id > rhs->id;
        }
        return lhs->label_v > rhs->label_v; //sorting should be in DECREASING order
        // todo: ensure ordering is correct
    }
};



#endif //RW_NODE_H
