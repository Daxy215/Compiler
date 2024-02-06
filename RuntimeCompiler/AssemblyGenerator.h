#pragma once
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>

#include "IR/IntermediateRepresentation.h"

struct Variable {
    std::string name;
    size_t size;
};

struct Function {
    std::string label;
    size_t size;
    std::vector<Variable*> parameters;
    std::vector<Variable*> variables;
    
    Function(std::string label, size_t size) : label(label), size(size) {}
    
    void addParamater(std::string name, size_t size) {
        parameters.push_back(new Variable(name, size));
    }
    
    void addVariable(std::string name, size_t size) {
        variables.push_back(new Variable(name, size));
    }
    
    void cleanUp(std::ofstream& outputFile) {
        if(size == 0)
            return;
        
        outputFile << "\t; Clean up " << label << " \n";
        outputFile << "\tadd esp, " << size << "\n\n";
    }
};

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
    Function* currentFunction = nullptr;
    std::map<std::string, Function*> functions;
};
