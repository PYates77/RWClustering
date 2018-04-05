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
    return nullptr;
}

std::vector<std::string> delimStr(std::string line,std::string delim){
    std::size_t pos = 0;
    std::vector<std::string> result;
    while ((pos=line.find_first_of(delim)) != std::string::npos){
        std::string subStr = line.substr(0,pos);
        result.push_back(subStr);
        line.erase(0,subStr.length() + delim.length());
    }
    result.push_back(line);
    return result;
}

void parseBLIF(std::string filename, std::vector<Node>& rawNodeList){
    //preliminary run
    std::cout << "Filename: " << filename << std::endl;
    std::ifstream blifFile;
    blifFile.open(filename);
    std::string line;

    std::string latchStr = ".latch";
    std::string gateStr = ".names";
    std::string inputStr = ".inputs";
    std::string outputStr = ".outputs";
    std::string prevStr = "";

    if (blifFile.is_open()){
        //succesful file opening
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
                    if (!argCount) {
                        //Input of Latch becomes PO
                        Node n;
                        n.strID = std::string(signalName) + "_OL";
                        n.isPO = true;
                        n.isPI = false;
                        rawNodeList.push_back(n);
                    }
                    else {
                        //Output of Latch becomes PI
                        Node n(0);
                        n.strID = std::string(signalName) + "_IL";
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
                std::vector<std::string> sigArgs = delimStr(line," ");
                Node *gateNode = findNodeByStr(sigArgs.at(sigArgs.size()-1),rawNodeList);
                if (gateNode != nullptr){
                    //this node has already been initiliazed as a primary output
                    gateNode->procStr = line;
                }
                else {
                    //this node has not already been initialized
                    Node n;
                    n.strID = sigArgs.at(sigArgs.size()-1);
                    n.procStr = line;
                    n.isPI = false;
                    n.isPO = false;
                    rawNodeList.push_back(n);
                }
            }
        }
    }

    //fix the rawNodeList structure

    for (std::vector<Node>::iterator iN = rawNodeList.begin(); iN < rawNodeList.end(); ++iN){
        iN->addr = &(*iN);
        if (iN->procStr != ""){
            //TODO (AKSHAY): the node is a gate which we need to fix
            std::vector<std::string> nStrList = delimStr(iN->procStr," ");
            for (std::vector<std::string>::iterator is = nStrList.begin()+1; is < nStrList.end()-1; ++is){
                Node *driver = findNodeByStr(*is,rawNodeList);
                if (driver != nullptr){
                    iN->prev.push_back(driver);
                    driver->next.push_back(&(*iN));
                }
                else {
                    std::cout << "Error: Driver not found" << std::endl;
                    exit(-1);
                }

            }
        }
        else if (iN->strID.find("_IL") != std::string::npos){
            //TODO (AKSHAY): the node is a PI latch which we need to setup correctly
        }
        else if (iN->strID.find("_OL") != std::string::npos){
            //TODO (AKSHAY): the node is a PO latch which we need to setup correctly
        }
    }

}


#endif //RW_BLIFPARSER_H

