//
// Created by Akshay on 3/24/2018.
//

#ifndef RW_COMMON_H
#define RW_COMMON_H

#include <vector>
#include <fstream>
#include <iostream>
#include <chrono>



Node* retrieveNodeByStr(std::string nodeID, std::vector<Node> &nodeList){
    //DESCRIPTION: Helper function to retrieve a node's pointer
    for (std::vector<Node>::iterator iN = nodeList.begin(); iN < nodeList.end(); ++iN){
        if (iN->strID == nodeID){
            //Match found!
            return &(*iN);
        }
    }
    return nullptr;
}
Node* retrieveNodeByStr_ptr(std::string nodeID, std::vector<Node*> &nodeList){
    //DESCRIPTION: Helper function to retrieve a node's pointer
    for (std::vector<Node*>::iterator iN = nodeList.begin(); iN < nodeList.end(); ++iN){
        if ((*iN)->strID == nodeID){
            //Match found!
            return *iN;
        }
    }
    return nullptr;
}

std::string ripBadChars(std::string str){
    std::string result = "";
    for (int i=0; i < str.length(); ++i){
        if (str[i] == '\0' || str[i] == '\r' || str[i] == ' ' || str[i] == '\n'){
            continue;
        }
        result.push_back(str[i]);
    }
    return result;
}

std::vector<std::string> delimStr(std::string line,std::string delim){
    std::size_t pos = 0;
    std::vector<std::string> result;
    while ((pos=line.find_first_of(delim)) != std::string::npos){
        std::string subStr = line.substr(0,pos);
        //std::cout << subStr << std::endl;
        if (subStr != "") {
            std::string resStr = ripBadChars(subStr);
            result.push_back(resStr);
        }
        line.erase(0,subStr.length() + delim.length());
    }
    std::string resStr = ripBadChars(line);
    result.push_back(resStr);
    return result;
}

void parseBLIF(std::string filename, int& piDelay, int& poDelay, int& nodeDelay, std::vector<Node>& rawNodeList){
    //preliminary run
    //std::cout << "Filename: " << filename << std::endl;
    std::ifstream blifFile;
    blifFile.open(filename);
    std::string line;

    std::string latchStr = ".latch";
    std::string gateStr = ".names";
    std::string inputStr = ".inputs";
    std::string outputStr = ".outputs";
    std::string prevStr = "";

    if (blifFile.is_open()){
        std::string modeStr = "";
        while(std::getline(blifFile,line)) {
            //std::cout << line << std::endl;
            std::vector<std::string> signals = delimStr(line," ");

            if (signals.at(0) == inputStr || modeStr == inputStr){
                std::vector<std::string>::iterator iS = (modeStr == "") ? signals.begin()+1 : signals.begin();
                modeStr = "";
                for (;iS < signals.end();++iS){
                    if ((*iS).c_str()[0] == '\\') {
                        modeStr = inputStr;
                        continue;
                    }
                    Node n(piDelay);
                    n.isPI = true;
                    n.isPO = false;
                    n.strID = *iS;
                    rawNodeList.push_back(n);
                    //std::cout << "PI NODE ADDED: " << n.strID << std::endl;
                }
                continue;
            }
            if (signals.at(0) == outputStr || modeStr == outputStr){
                std::vector<std::string>::iterator iS = (modeStr == "") ? signals.begin()+1 : signals.begin();
                modeStr = "";
                for (;iS < signals.end();++iS){
                    if ((*iS).c_str()[0] == '\\') {
                        modeStr = outputStr;
                        continue;
                    }
                    Node n(poDelay);
                    n.isPI = false;
                    n.isPO = true;
                    n.strID = *iS;
                    rawNodeList.push_back(n);
                    //std::cout << "PO NODE ADDED: " << n.strID << std::endl;
                }
                continue;
            }
            if (signals.at(0) == latchStr){
                modeStr = "";
                int argCount = 0;
                while (argCount < 2){
                    if (!argCount) {
                        //Input of Latch becomes PO
                        Node n(poDelay);
                        n.strID = signals.at(1) + "_OL";
                        n.isPO = true;
                        n.isPI = false;
                        rawNodeList.push_back(n);
                    }
                    else {
                        //Output of Latch becomes PI
                        Node n(piDelay);
                        n.strID = signals.at(2) + "_IL";
                        n.isPI = true;
                        n.isPO = false;
                        rawNodeList.push_back(n);
                    }
                    argCount += 1;
                }
                continue;
            }
            if (signals.at(0) == gateStr){
                modeStr = "";
                Node *gateNode = retrieveNodeByStr(signals.at(signals.size() - 1), rawNodeList);
                if (gateNode != nullptr){
                    //this node has already been initiliazed as a primary output
                    gateNode->procStr = line;
                }
                else {
                    //this node has not already been initialized
                    Node n(nodeDelay);
                    n.strID = signals.at(signals.size()-1);
                    n.procStr = line;
                    n.isPI = false;
                    n.isPO = false;
                    rawNodeList.push_back(n);
                }
            }
        }

    }  //ENDIF BLIF OPEN

    //fix the rawNodeList structure
    //std::cout << "SECONDARY PARSE RUN" << std::endl;

    for (std::vector<Node>::iterator iN = rawNodeList.begin(); iN < rawNodeList.end(); ++iN){
        iN->addr = &(*iN);
        if (iN->procStr != ""){
            //std::cout << iN->strID << std::endl;
            std::vector<std::string> nStrList = delimStr(iN->procStr," ");
            for (std::vector<std::string>::iterator is = nStrList.begin()+1; is < nStrList.end()-1; ++is){
                Node *driver = retrieveNodeByStr(*is, rawNodeList);
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

std::vector<Node*> obtainPONodes(std::vector<Node>& rawNodeList){
    std::vector<Node*> result;
    for (auto in = rawNodeList.begin(); in < rawNodeList.end(); ++in){
        if (in->isPO){
            result.push_back(&(*in));
        }
    }
    return result;
}

std::vector<Node*> obtainPINodes(std::vector<Node>& rawNodeList){
    std::vector<Node*> result;
    for (auto in = rawNodeList.begin(); in < rawNodeList.end(); ++in){
        if (in->isPI){
            result.push_back(&(*in));
        }
    }
    return result;
}

void generateInputSet(Cluster& c){

    //Description: generates the input() set for a cluster
    std::copy(c.members.begin(),c.members.end(),std::back_inserter(c.inputSet));

    for(auto cNode : c.members){
        for (auto pNode : cNode->prev){
            //check if node isn't already part of input set
            Node *nPtr = retrieveNodeByStr_ptr(pNode->strID,c.inputSet);
            if (nPtr == nullptr){
                c.inputSet.push_back(pNode);
            }
        }
    }

    //delete the cluster elements from the inputSet
    c.inputSet.erase(c.inputSet.begin(), c.inputSet.begin() + c.members.size());

}

std::pair<long long int,std::string> measureExecTime(std::chrono::time_point<std::chrono::high_resolution_clock>& start,std::chrono::time_point<std::chrono::high_resolution_clock>& end){
    std::pair<long long int,std::string> result;
    result.first = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    result.second = "us";
    if (result.first >= 1000){
        result.first = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        result.second = "ms";
        if (result.first >= 1000){
            result.first = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
            result.second = "s";
            if (result.first >= 60){
                result.first = std::chrono::duration_cast<std::chrono::minutes>(end - start).count();
                result.second = "min";
                if (result.first >= 60){
                    result.first = std::chrono::duration_cast<std::chrono::hours>(end - start).count();
                    result.second = "hrs";
                }
            }
        }
    }
    return result;
}



#endif //RW_COMMON_H

