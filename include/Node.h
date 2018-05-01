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

#ifndef RW_NODE_H
#define RW_NODE_H


#include <vector>
#include <set>
#include <string>

class Node {
private:
public:
    void computeLabel();
    std::vector<Node *> prev;
    std::vector<Node *> next;
    int delay; //ASSUME INTEGER DELAY
    uint32_t id;
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
};

bool compare_lv (const Node* lhs, const Node* rhs);

#endif //RW_NODE_H
