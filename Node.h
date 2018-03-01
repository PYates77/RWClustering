//
// Created by Paul on 3/1/2018.
//

#ifndef RW_NODE_H
#define RW_NODE_H


#include <vector>

class Node {
private:
    char* id; //for debug printing
    std::vector<Node> ins;
    std::vector<Node> outs;
    int delay; //ASSUME INTEGER DELAY
    int label;
public:
    int getDelay(void);
    void computeLabel();
};


#endif //RW_NODE_H
