#pragma once
#include <sstream>
#include <string>
#include <vector>

#include "IntermediateRepresentation.h"

class AssemblyGenerator {
public:
    void generateAssembly(std::string fileName, std::unique_ptr<IRNode> IRNode);

private:
    std::vector<std::string> splitString(const std::string &s, char delimiter) {
        std::vector<std::string> tokens;
        std::istringstream iss(s);
        std::string token;
        
        while (std::getline(iss, token, delimiter)) {
            tokens.push_back(token);
        }
        
        return tokens;
    }
};
