#pragma once

#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <string>

#include "Properties.h"
#include "../Parser.h"

class SymbolTable {
public:
    SymbolTable() {}
    SymbolTable(const std::string& return_type, const std::string& identifier, const std::string& full_name,
                const std::string& scope,
                ASTNode* node)
        : returnType(return_type),
          identifier(identifier),
          fullName(full_name),
          scope(scope),
          node(node),
          type(node->type) {
        properties = new Properties(identifier, returnType);
    }

// TODO; Make this private
public:
    std::string returnType, identifier, fullName, scope;
    
    bool isClass = false, isConstructor = false, isFunction = false, isVariable = false;
    bool hasConstructor = false;
    
    Properties* properties;

    ASTNode* node;
    NodeType type;
};

#endif