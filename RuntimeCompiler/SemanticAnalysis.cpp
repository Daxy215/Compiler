#include "SemanticAnalysis.h"

void SemanticAnalysis::traverseNodes(ASTNode* node, std::string currentScope) {
    if (node->type == NodeType::CLASS) {
        currentScope = split(node->value);
    } else if (node->type == NodeType::MEMBER_FUNCTION || node->type == NodeType::VARIABLE_DECLARATION) {
        std::string returnType = split(node->children[0]->value);
        std::string identifier = split(node->value);
        std::string scopePrefix = (currentScope != "Global Scope") ? currentScope + "::" : "";
        std::string fullName = scopePrefix + split(node->value);
        
        if (isFunctionOverloaded(identifier)) {
            fullName = generateUniqueFunctionName(fullName);
            
            addToSymbolTable(returnType + " " + identifier + " : " + fullName, node->type);
        } else {
            addToSymbolTable(returnType + " " + identifier + " : " + fullName, node->type);
        }
    }
    
    for (auto child : node->children) {
        traverseNodes(child, currentScope);
    }
}

bool SemanticAnalysis::isFunctionOverloaded(const std::string& functionName) {
    int count = 0;
    
    // Iterate through the symbol table to count occurrences of the function name
    for (const auto& entry : symbolTable) {
        std::string identifier = splitString(entry.first, ' ')[1];
        
        // Check if the identifier matches the function name
        size_t found = identifier.find(functionName);
        if (found != std::string::npos && found == 0) {
            count++;
        }
    }
    
    // If more than one occurrence, function is overloaded
    return count > 0;
}

void SemanticAnalysis::printSymbolTable() {
    std::cout << "Symbol Table Contents:" << std::endl;
    
    for (const auto& entry : symbolTable) {
        std::cout << entry.first << " : " << static_cast<int>(entry.second) << std::endl;
    }
    
    std::cout << "End of Symbol Table" << std::endl;
}
