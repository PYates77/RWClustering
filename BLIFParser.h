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
    //preliminary run
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
        std::string prevStr = "";
        while(std::getline(blifFile,line)){
            char * signalName = NULL;
            char * lineCStyle = strdup(line.c_str());
            signalName = strtok(lineCStyle," ");
            if (std::string(signalName) == inputStr || prevStr == inputStr){
                if (prevStr == "") {
                    signalName = strtok(NULL, " ");
                }
                prevStr = "";
                while (signalName != NULL){
                    if (std::string(signalName) == "\\"){
                        prevStr = inputStr;
                    }
                    else {
                        //std::cout << "ISignal Parsed: " << std::string(signalName) << std::endl;
                        Node n(0);
                        n.isPI = true;
                        n.isPO = false;
                        n.nodeType = PI;
                        n.strID = std::string(signalName);
                        rawNodeList.push_back(n);
                    }
                    signalName = strtok(NULL," ");
                }
                continue;
            }
            if (std::string(signalName) == outputStr || prevStr == outputStr){
                if (prevStr == "") {
                    signalName = strtok(NULL, " ");
                }
                prevStr = "";
                while (signalName != NULL){
                    if (std::string(signalName) == "\\"){
                        prevStr = outputStr;
                    }
                    else {
                        //std::cout << "ISignal Parsed: " << std::string(signalName) << std::endl;
                        Node n;
                        n.isPI = false;
                        n.isPO = true;
                        n.nodeType = PO;
                        n.strID = std::string(signalName);
                        rawNodeList.push_back(n);
                    }
                    signalName = strtok(NULL," ");
                }
                continue;
            }
            if (std::string(signalName) == latchStr){
                prevStr = "";
                int argCount = 0;
                signalName = strtok(NULL," ");
                while (signalName != NULL && argCount < 2){
                    //std::cout << "Signal Parsed: " << std::string(signalName) + "_LATCH" << std::endl;
                    if (!argCount) {
                        //Input of Latch becomes PO
                        Node n;
                        n.strID = std::string(signalName) + "_L";
                        n.nodeType = PO;
                        n.isPO = true;
                        n.isPI = false;
                        rawNodeList.push_back(n);
                    }
                    else {
                        //Output of Latch becomes PI
                        Node n(0);
                        n.strID = std::string(signalName) + "_L";
                        n.nodeType = PI;
                        n.isPI = true;
                        n.isPO = false;
                        rawNodeList.push_back(n);
                    }
                    signalName = strtok(NULL," ");
                    ++argCount;
                }
                continue;
            }
            if (std::string(signalName) == gateStr){
                prevStr = "";
                Node n;
                n.strID = line;
                n.nodeType = GATE;
                rawNodeList.push_back(n);
                continue;
            }
        }
    }

    //secondary run
    blifFile.open(filename);
    line = "";
    if (blifFile.is_open()){

    }
    else {
        //ERROR
        std::cout <<"Error opening " << filename << " for secondary pass" << std::endl;
    }
}


#endif //RW_BLIFPARSER_H

