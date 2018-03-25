//
// Created by Paul on 3/1/2018.
//

#include "Node.h"


// recursively inserts all predecessors into vector
void Node::predecessors_r(std::set<Node, compare_lv>& V){
   for(std::vector<Node>::iterator it; it != prev.end(); ++it){
       V.insert(*it); // will only insert if not already in V
       it->predecessors_r(V);
   }
}
//Node::Node() {
//
//}
