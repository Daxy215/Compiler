#include "SemanticAnalysis.h"

SemanticAnalysis::SemanticAnalysis(ASTNode* node) {
    traverseNodes(node, "GLOBAL");
    performSemanticChecks();
    printSymbolTable();
    
    std::cout << "\n\nErrrors\n\n";
    
    for (const auto& error : errors) {
        std::cout << error << std::endl;
    }
}

void SemanticAnalysis::traverseNodes(ASTNode* node, std::string currentScope) {
    if (node->type == NodeType::CLASS) {
        currentScope = split(node->value);
    } else if (node->type == NodeType::MEMBER_FUNCTION || node->type == NodeType::MEMBER_VARIABLE
                || node->type == NodeType::LOCAL_VARIABLE_DECLARATION) {
        std::string returnType = split(node->children[0]->value);
        std::string identifier = split(node->value);
        std::string scopePrefix = (currentScope != "Global Scope") ? currentScope + "::" : "";
        std::string fullName = scopePrefix + identifier;
        
        if (isFunctionOverloaded(identifier, currentScope)) {
            identifier = generateUniqueFunctionName(identifier);
            
            //TODO; Update fullName
            addToSymbolTable(returnType, identifier, fullName, currentScope, node->type)->isFunction = true;
        } else {
            if(node->type == NodeType::MEMBER_VARIABLE) {
                addToSymbolTable(returnType, identifier, fullName, currentScope, node->type)->isVariable = true;
            } else if(node->type == NodeType::LOCAL_VARIABLE_DECLARATION) {
                identifier = splitString(node->value, ' ')[2];
                
                // TODO; DO PARSER 'parseMember' merge with 'parseStatement'
                int size = node->children.size();
                
                if(size > 2)
                    returnType = node->children[2]->value;
                else
                    returnType = "no :(";
                
                fullName = scopePrefix + identifier;
                
                addToSymbolTable(returnType, identifier, fullName, currentScope, node->type)->isVariable = true;
            } else { //TODO; Add more types
                addToSymbolTable(returnType, identifier, fullName, currentScope, node->type);
            }
        }
    }
    
    for (auto child : node->children) {
        traverseNodes(child, currentScope);
    }
}

void SemanticAnalysis::performSemanticChecks() {
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

bool SemanticAnalysis::isFunctionOverloaded(const std::string& functionName, const std::string& scope) {
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
