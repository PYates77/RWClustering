//
// Created by Akshay on 3/24/2018.
//

#ifndef RW_BLIFPARSER_H
#define RW_BLIFPARSER_H

#include <stdio.h>
#include <string.h>

Node* findNodeByStr(std::string nodeID, std::vector<Node>& nodeList){
    //DESCRIPTION: Helper function to retrieve a node's pointer
    for (std::vector<Node>::iterator in = nodeList.begin(); in < nodeList.end(); ++in){
        if (in->strID == nodeID){
            //Match found!
            return &(*in);
        }
    }
    return NULL;
}

void parseBLIF(std::string filename, std::vector<Node>& rawNodeList){
    std::cout << "Filename: " << filename << std::endl;
    std::ifstream blifFile;
    blifFile.open(filename);
    std::string line;

    if (blifFile.is_open()){
        //succesful file opening
        std::string latchStr = ".latch";
        std::string gateStr = ".names";
        std::string inputStr = ".inputs";
        std::string outputStr = ".outputs";
        while(std::getline(blifFile,line)){
            //std::cout << "Line from File: " << line << std::endl;
            //std::cout << "Line from File C Style: " << line.c_str() << std::endl;
            char * signalName = NULL;
            std::size_t found = line.find(inputStr);
            if (found != std::string::npos){
                continue;
            }
            found = line.find(outputStr);
            if (found != std::string::npos){
                continue;
            }
            found = line.find(latchStr);
            if (found != std::string::npos){
                char * lineCStyle = strdup(line.c_str());
                signalName = strtok(lineCStyle," ");
                int argCount = 0;
                while (signalName != NULL && argCount < 2){
                    if (latchStr == signalName){
                        signalName = strtok(NULL," ");
                        continue;
                    }
                    //std::cout << "Signal Parsed: " << std::string(signalName) + "_LATCH" << std::endl;
                    if (!argCount) {
                        //Input of Latch becomes PO
                        Node n;
                        n.strID = std::string(signalName) + "_LATCH";
                        n.nodeType = PO;
                        n.isPO = true;
                        if (findNodeByStr(std::string(signalName),rawNodeList) == NULL){
                            //no node as such exists thus far so just create it and we can fill it in later
                            Node prevGate;
                            prevGate.strID = std::string(signalName);
                            rawNodeList.push_back(prevGate);
                        }
                        Node *prevGPtr = findNodeByStr(std::string(signalName),rawNodeList);
                        //now the previous driving gate will exist in the list
                        n.prev.push_back(prevGPtr);
                        std::cout << std::hex << &rawNodeList.at(0) << std::endl;
                        rawNodeList.push_back(n);
                        std::cout << std::hex << &rawNodeList.at(0) << std::endl;
                    }
                    else {
                        //Output of Latch becomes PI
                        Node n(0);
                        n.strID = std::string(signalName) + "_LATCH";
                        n.nodeType = PI;
                        //TODO: Setup prev and next nodes
                    }
                    signalName = strtok(NULL," ");
                    ++argCount;
                }
                continue;
            }
            found = line.find(gateStr);
            if (found != std::string::npos){
                continue;
            }
        }
    }
}


#endif //RW_BLIFPARSER_H

