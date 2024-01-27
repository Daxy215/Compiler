#pragma once
#include <sstream>
#include <string>
#include <vector>

#include "IR/IntermediateRepresentation.h"

class AssemblyGenerator : public IRVisitor {
public:
    //void visit(IRVariableDeclaration* node) override;

    void generateCode(const std::vector<IR*>& irCommands);
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
