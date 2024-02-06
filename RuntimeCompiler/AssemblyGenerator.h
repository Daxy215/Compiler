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
    bool isNumber(const std::string& str) {
        for (char const &c : str) {
            if (!std::isdigit(c)) return false;
        }
        return !str.empty();
    }
    
    std::vector<std::string> splitString(const std::string &s, char delimiter) {
        std::vector<std::string> tokens;
        std::istringstream iss(s);
        std::string token;
        
        while (std::getline(iss, token, delimiter)) {
            tokens.push_back(token);
        }
        
        return tokens;
    }
    
    std::string getVariable(std::string temp) {
        if(isNumber(temp)) {
            return temp;
        }
    
        return "[" + temp + "]";
    }

private:
    std::vector<std::string> functions;
};
