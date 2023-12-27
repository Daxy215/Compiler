#pragma once

#ifndef SEMANTICANALYSIS_H
#define SEMANTICANALYSIS_H

#include <sstream>
#include <string>
#include <unordered_map>

#include "Parser.h"

class SemanticAnalysis {
    /*struct FunctionProperties {
        std::string returnType;
        bool isConst, isFinal, isVirtual;
        
        std::vector<std::string> parameters;
    };*/
    
    struct SymbolTable {
        SymbolTable(const std::string& return_type, const std::string& identifier, const std::string& full_name,
                    const std::string& scope,
                    NodeType type)
            : returnType(return_type),
              identifier(identifier),
              fullName(full_name),
              scope(scope),
              type(type) {
            
        }

        std::string returnType, identifier, fullName, scope;

        //FunctionProperties* functionProperties;
        bool isFunction = false;
        bool isVariable = false;

        NodeType type;
    };

public:
    SemanticAnalysis();
    
    void generateSymbolTable(ASTNode* node);
private:
    void traverseNodes(ASTNode* node, std::string currentScope);
    void performSemanticChecks();
    bool isFunctionOverloaded(const std::string& functionName, const std::string& scope);
    
    bool checkVariableDeclaration(const std::string& variable) {
        return symbolTable.find(variable) != symbolTable.end();
    }
    
    bool checkType(const std::string& variable, const std::string& value) {
        auto it = symbolTable.find(variable);
        
        if (it != symbolTable.end()) {
            // Compare the expected type with the stored type in the symbol table
            return it->second->returnType == value;
        }
        
        return false; // Variable not found in symbol table
    }
    
public:
    void printSymbolTable() {
        std::cout << "Symbol Table Contents:" << std::endl;
        
        std::cout << "Functions: \n";
        for (const auto& entry : symbolTable) {
            if(entry.second->isFunction)
                std::cout << entry.second->returnType + " " + entry.second->identifier + " : " + entry.second->fullName <<
                    " : " << static_cast<int>(entry.second->type) << std::endl;
        }

        std::cout << "\nVariables: \n";
        for (const auto& entry : symbolTable) {
            if(entry.second->isVariable)
                std::cout << entry.second->returnType + " " + entry.second->identifier + " : " + entry.second->fullName <<
                    " : " << static_cast<int>(entry.second->type) << std::endl;
        }
    
        std::cout << "End of Symbol Table" << std::endl;
    }
private:
    SymbolTable* addToSymbolTable(const std::string& returnType, const std::string& identifier, const std::string& fullName, const std::string& scope,  NodeType type) {
        return symbolTable[identifier] = new SymbolTable(returnType, identifier, fullName, scope, type);
    }
    
    std::string generateUniqueFunctionName(const std::string& functionName) {
        static std::unordered_map<std::string, int> functionCount;
        
        int count = ++functionCount[functionName];
        return functionName + "_" + std::to_string(count);
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
    std::vector<std::string> errors;
    std::unordered_map<std::string, SymbolTable*> symbolTable;
};

#endif