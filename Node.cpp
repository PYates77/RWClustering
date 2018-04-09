//
// Created by Paul on 3/1/2018.
//

#include "Node.h"


// recursively inserts all predecessors into set
void Node::predecessors_r(std::set<Node *, compare_lv>& V){
   for(auto node : prev ){
       V.insert(node); // will only insert if not already in V
       node->predecessors_r(V);
   }
}
