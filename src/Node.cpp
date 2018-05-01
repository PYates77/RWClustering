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

#include "Node.h"

// for ordering nodes in S set, nodes are ordered first by label, then by ID
bool compare_lv (const Node* lhs, const Node* rhs) {
    if(lhs->label_v == rhs->label_v){
        return lhs->id > rhs->id;
    }
    return lhs->label_v > rhs->label_v; //sorting should be in DECREASING order
}


