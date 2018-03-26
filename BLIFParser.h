//
// Created by Akshay on 3/24/2018.
//

#ifndef RW_BLIFPARSER_H
#define RW_BLIFPARSER_H

#include <stdio.h>
#include <string.h>

void parseBLIF(std::string filename, std::vector<Node> rawNodeList){
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
                    if (!argCount) {

                    }
                    else {

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


#endif //RW_BLIFPARSER_H

