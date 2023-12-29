#pragma once
#include <string>

#include "Parser.h"

class TACGenerator {
public:
    void generateTAC(ASTNode* nodel, int& tempCounter);

private:
    std::string getTemp() {
        return "t" + std::to_string(tempCounter++);
    }
    
private:
    int tempCounter = 0;
    
public:
    std::vector<std::string> tac;
};
