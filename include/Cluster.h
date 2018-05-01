/*
Copyright 2018 Akshay Nagendra
Copyright 2018 Paul Yates

This file is part of RWClustering

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef RW_CLUSTER_H
#define RW_CLUSTER_H


#include <vector>
#include "Node.h"

class Node;

struct Cluster {
    std::vector<Node *> members;
    int id;
    int delay;
    int calcL1Value();
    Cluster(int);
    std::vector<Node *> inputSet;
    bool static isClusterInList(int cID,std::vector<Cluster*>& cList);
    bool static isClusterInList_str(std::string sID,std::vector<Cluster*>& cList);
    //TODO: need lists of input/output Nodes? Clusters?
};



#endif //RW_CLUSTER_H
