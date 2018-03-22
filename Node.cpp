//
// Created by Paul on 3/1/2018.
//

#include "Node.h"

// for ordering nodes in S set
struct compare_lv{
    bool operator() (const Node& lhs, const Node& rhs) const {
        return lhs.label_v > rhs.label_v; //sorting should be in DECREASING order
        // todo: ensure ordering is correct
    }
};

// recursively inserts all predecessors into vector
void Node::predecessors_r(std::set<Node>& V){
   for(std::vector::iterator it; it != prev.end(); ++it){
       V.insert(it); // will only insert if not already in V
       it.predecessors_r(V);
   }
}
//Node::Node() {
//
//}
