//
// Created by Akshay on 3/24/2018.
//

#ifndef RW_BLIFPARSER_H
#define RW_BLIFPARSER_H


void parseBLIF(std::string filename, std::vector<Node> rawNodeList){
  std::cout << "Filename: " << filename << std::endl;
  std::ifstream blifFile;
  blifFile.open(filename);
  if (blifFile.is_open()){
    //succesful file opening
    blifFile.close();
  }
}


#endif //RW_BLIFPARSER_H

