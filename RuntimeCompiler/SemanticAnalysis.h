#pragma once

#ifndef SEMANTICANALYSIS_H
#define SEMANTICANALYSIS_H

#include <sstream>
#include <string>
#include <unordered_map>

#include "Parser.h"

class SemanticAnalysis {
public:
    void traverseNodes(ASTNode* node, std::string currentScope);
    bool isFunctionOverloaded(const std::string& functionName);
    
public:
    void printSymbolTable();
private:
    void addToSymbolTable(const std::string& name, NodeType type) {
        symbolTable[name] = type;
    }

    std::string generateUniqueFunctionName(const std::string& functionName, int count) {
        return functionName + "_" + std::to_string(count);
    }
    
    std::string generateUniqueFunctionName(const std::string& functionName) {
        static std::unordered_map<std::string, int> functionCount;
        
        int count = ++functionCount[functionName];
        return generateUniqueFunctionName(functionName, count);
    }
    
    std::string split(const std::string &s) {
        std::vector<std::string> tokens;
        std::istringstream iss(s);
        std::string token;
        
        while (std::getline(iss, token, ' ')) {
            tokens.push_back(token);
        }
        
        int size = tokens.size();
        
        if(size > 1)
            return tokens[1];
        
        return tokens[tokens.size() - 1];
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
private:
    std::unordered_map<std::string, NodeType> symbolTable;
};

#endif