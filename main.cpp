//
// Created by Paul on 2/13/2018.
//
#include <iostream>
#include <vector>


struct node{
    std::vector<node> prev;
    std::vector<node> next;
};
void rw(std::vector<node> pi, std::vector<node> po){
    
}
int main() {
    std::cout << "Hello, RWClustering World!" << std::endl;
    //TODO: read in .blif file
    std::vector<node> inputs;
    std::vector<node> outputs;
    rw(inputs,outputs);
    return 0;
}
