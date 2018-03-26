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
    std::vector<Node> prev;
    std::vector<Node> next;
    int delay; //ASSUME INTEGER DELAY
    int id;
    bool visited;
    int label;
    int label_v;

    bool isPI = false;
    bool isPO = false;
    std::string nodeType = "INVALID";


    Node(){
        delay = 1;
        visited = false;
    }
    Node(int d){
        delay = d;
        visited = false;
    }
    void predecessors_r(std::set<Node, compare_lv>&);
};

// for ordering nodes in S set
struct compare_lv{
    bool operator() (const Node& lhs, const Node& rhs) const {
        return lhs.label_v > rhs.label_v; //sorting should be in DECREASING order
        // todo: ensure ordering is correct
    }
};

#endif //RW_NODE_H
