#include "SemanticAnalyzer.h"

SemanticAnalyzer::SemanticAnalyzer() {
    
}

void SemanticAnalyzer::generateSymbolTable(ASTNode* node) {
    traverseNodes(node, "GLOBAL", nullptr);
    performSemanticChecks();
    printSymbolTable();
    
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
            symbolTable[namespaceType] = new SymbolTable("Namespace", namespaceType, namespaceType, currentScope, node->type);
            //symbolTable[namespaceType]->isFunction = true;
            
            break;
        }
    case NodeType::CLASS: {
            std::string className = node->value;
            symbolTable[className] = new SymbolTable("class :D", className, "", currentScope, node->type);
            symbolTable[className]->isClass = true;
            symbolTable[className]->properties = new Properties();
            
            if(className._Equal("Circle")) {
                std::cout << "class;  " << className << " ; " << symbolTable[className]->identifier << std::endl;
            }
            
            currentScope += "::" + className;
            
            for (auto child : node->children) {
                traverseNodes(child, currentScope, symbolTable[className]);
            }
            
            break;
    }
    case NodeType::CONSTRUCTOR: {
            std::string constructorName = node->value;
            symbolTable[currentScope + "::" + constructorName] = new SymbolTable("Constructor", constructorName, currentScope + "::" + constructorName, currentScope, node->type);
            symbolTable[currentScope + "::" + constructorName]->isConstructor = true;
            
            if(parent) {
                parent->properties->addFunction(symbolTable[currentScope + "::" + constructorName]->properties);
            }
            
            break;
        }
    case NodeType::MEMBER_FUNCTION: {
            std::string funcName = node->value;
            std::string returnType = node->getChildByType(NodeType::RETURN_TYPE)->getValue();
            symbolTable[funcName] = new SymbolTable(returnType, funcName, currentScope + "::" + funcName, currentScope, node->type);
            symbolTable[funcName]->isFunction = true;
            
            if(parent)
                parent->properties->addFunction(symbolTable[funcName]->properties);
            
            break;
    }
    case NodeType::MEMBER_VARIABLE: {
            std::string varName = node->value;
            std::string varType = node->getChildByType(NodeType::VARIABLE_TYPE)->getValue();
            symbolTable[varName] = new SymbolTable(varType, varName, currentScope + "::" + varName, currentScope, node->type);
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
        // Check for each entry in the symbol table
        const std::string& variable = entry.first;
        const std::string& type = entry.second->identifier;
        
        // Perform checks (e.g., type checking, scope checking, etc.)
        if (!checkVariableDeclaration(variable)) {
            std::cout << "Error: Variable '" << variable << "' used without declaration" << std::endl;
            // Handle undeclared variable error
        }
        
        // Perform other checks...
        // Type checking, scope checking, etc.
    }
}

bool SemanticAnalyzer::isFunctionOverloaded(const std::string& functionName, const std::string& scope) {
    int count = 0;
    
    // Iterate through the symbol table to count occurrences of the function name
    for (const auto& entry : symbolTable) {
        if(entry.second == nullptr) {
            std::cout << "Heh? " << entry.first << std::endl;
            continue;
        }
        
        std::string identifier = entry.second->identifier;
        
        // Check if the identifier matches the function name
        size_t found = identifier.find(functionName);
        if (found != std::string::npos && found == 0) {
            count++;
        }
    }
    
    // If more than one occurrence, function is overloaded
    return count > 0;
}
