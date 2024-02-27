#include "SemanticAnalyzer.h"

#include "../Diagnosis/Diagnoser.h"

SemanticAnalyzer::SemanticAnalyzer() {
    
}

void SemanticAnalyzer::generateSymbolTable(ASTNode* node) {
    traverseNodes(node, "GLOBAL", nullptr);
    //performSemanticChecks();

#ifdef DEBUG
    printSymbolTable();
#endif
    
    if(errors.empty()) {
        std::cout << "\nNo errors found!\n\n";
        return;
    }
    
    std::cout << "\n\nErrors\n\n";
    
    for (const auto& error : errors) {
        std::cout << error << '\n';
    }
}

void SemanticAnalyzer::traverseNodes(ASTNode* node, std::string currentScope, SymbolTable* parent) {
    if (!node) {
        return;
    }
    
    switch (node->type) {
    case NodeType::NAMESPACE: {
            std::string namespaceType = node->value;
            symbolTable[namespaceType] = new SymbolTable("Namespace", namespaceType, namespaceType, currentScope, node);
            
            break;
        }
    case NodeType::CLASS: {
            std::string className = node->value;
            symbolTable[className] = new SymbolTable("class :D", className, "", currentScope, node);
            symbolTable[className]->isClass = true;
            symbolTable[className]->properties = new Properties();
            
            if(className._Equal("Circle")) {
                std::cout << "class;  " << className << " ; " << symbolTable[className]->identifier << '\n';
            }
            
            currentScope += "::" + className;
            
            for (auto child : node->children) {
                traverseNodes(child, currentScope, symbolTable[className]);
            }
            
            break;
    }
    case NodeType::CONSTRUCTOR: {
            std::string constructorName = node->value;
            symbolTable[currentScope + "::" + constructorName] = new SymbolTable("Constructor", constructorName, currentScope + "::" + constructorName, currentScope, node);
            symbolTable[currentScope + "::" + constructorName]->isConstructor = true;
            
            if(parent) {
                parent->properties->addFunction(symbolTable[currentScope + "::" + constructorName]->properties);
            }
            
            break;
        }
    case NodeType::MEMBER_FUNCTION: {
            std::string funcName = node->value;
            std::string returnType = node->getChildByType(NodeType::RETURN_TYPE)->getValue();
            symbolTable[funcName] = new SymbolTable(returnType, funcName, currentScope + "::" + funcName, currentScope, node);
            symbolTable[funcName]->isFunction = true;
            
            if(parent)
                parent->properties->addFunction(symbolTable[funcName]->properties);
            
            for (auto child : node->children) {
                traverseNodes(child, currentScope, symbolTable[funcName]);
            }
            
            break;
    }
    case NodeType::LOCAL_VARIABLE_DECLARATION:
    case NodeType::MEMBER_VARIABLE: {
            std::string varName = node->value;
            std::string varType = node->getChildByType(NodeType::VARIABLE_TYPE)->getValue();
            symbolTable[varName] = new SymbolTable(varType, varName, currentScope + "::" + varName, currentScope, node);
            symbolTable[varName]->isVariable = true;
            
            if(parent)
                parent->properties->addMember(symbolTable[varName]->properties);
            
            break;
    }
    default: {
            for (auto child : node->children) {
                traverseNodes(child, currentScope, parent);
            }
            
            break;
        }
    }
}

void SemanticAnalyzer::performSemanticChecks() {
    for (const auto& entry : symbolTable) {
        const std::string& member = entry.first;
        const SymbolTable* table  = entry.second;
        ASTNode* node             = table->node;
        
        switch(table->type) {
        case NodeType::CLASS:
            
            break;
        case NodeType::MEMBER_FUNCTION: {
                const std::string returnType = table->returnType;
                
                if(returnType != "void") {
                    const ASTNode* returnStatementNode = node->getChildByType(NodeType::RETURN_STATEMENT);
                    bool returns = functionReturnsSomething(node);
                    
                    if(returnStatementNode == nullptr) {
                        Diagnoser::logError("Couldn't find a return type for the function " + node->value, node->token);
                    }
                }
                
                break;
            }
        default:
            
            break;
        }
    }
}

bool SemanticAnalyzer::isFunctionOverloaded(const std::string& functionName, const std::string& scope) {
    int count = 0;
    
    for (const auto& entry : symbolTable) {
        if(entry.second == nullptr) {
            std::cout << "Heh? " << entry.first << '\n';
            continue;
        }
        
        std::string identifier = entry.second->identifier;
        
        size_t found = identifier.find(functionName);
        if (found != std::string::npos && found == 0) {
            count++;
        }
    }
    
    return count > 0;
}
