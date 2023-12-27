#pragma once
#include <string>

#include "Parser.h"

class TACGenerator {
public:
    void generateTAC(ASTNode* node);

private:
    std::string getTemp() {
        return "t" + std::to_string(tempCount++);
    }
    
private:
    int tempCount = 0;
    
public:
    std::vector<std::string> tac;
};
