#include "SemanticAnalysis.h"

SemanticAnalysis::SemanticAnalysis() {
    
}

void SemanticAnalysis::generateSymbolTable(ASTNode* node) {
    traverseNodes(node, "GLOBAL");
    performSemanticChecks();
    printSymbolTable();
    
    if(errors.empty()) {
        std::cout << "\nNo errors found!";
        return;
    }
    
    std::cout << "\n\nErrors\n\n";
    
    for (const auto& error : errors) {
        std::cout << error << std::endl;
    }
}

void SemanticAnalysis::traverseNodes(ASTNode* node, std::string currentScope) {
    if(node == nullptr)
        return;
    
    if (node->type == NodeType::CLASS) {
        currentScope = split(node->value);
    } else if (node->type == NodeType::MEMBER_FUNCTION || node->type == NodeType::MEMBER_VARIABLE
                || node->type == NodeType::LOCAL_VARIABLE_DECLARATION) {
        std::string returnType = split(node->children[0]->value);
        std::string identifier = split(node->value);
        std::string scopePrefix = (currentScope != "Global Scope") ? currentScope + "::" : "";
        std::string fullName = scopePrefix + identifier;
        
        //TODO; 
        /*if ((node->type == NodeType::MEMBER_FUNCTION || node->type == NodeType::LOCAL_VARIABLE_DECLARATION)
       && node->children.size() < 3
       || node->children.back()->type != NodeType::BRACE) {
            std::cerr << "Error: Incorrect function declaration" << std::endl;
       }*/
        
        if (isFunctionOverloaded(identifier, currentScope)) {
            identifier = generateUniqueFunctionName(identifier);
            
            //TODO; Update fullName
            addToSymbolTable(returnType, identifier, fullName, currentScope, node->type)->isFunction = true;
        } else {
            if(node->type == NodeType::MEMBER_VARIABLE) {
                addToSymbolTable(returnType, identifier, fullName, currentScope, node->type)->isVariable = true;
            } else if(node->type == NodeType::LOCAL_VARIABLE_DECLARATION) {
                addToSymbolTable(returnType, identifier, fullName, currentScope, node->type)->isVariable = true;
                
                /*std::vector<std::string> s = splitString(node->value, ' ');
                identifier = s[s.size() - 1];
                
                size_t size = node->children.size();
                
                if(size > 2)
                    returnType = node->children[2]->value;
                else
                    returnType = "no :(";
                
                fullName = scopePrefix + identifier;
                
                addToSymbolTable(returnType, identifier, fullName, currentScope, node->type)->isVariable = true;*/
            } else { //TODO; Add more types
                addToSymbolTable(returnType, identifier, fullName, currentScope, node->type);
            }
        }
    }
    
    for (auto child : node->children) {
        traverseNodes(child, currentScope);
    }

    //TODO;
    // Check for missing semicolon
    /*if (node->children.back()->type != NodeType::SEMICOLON) {
        std::cerr << "Error: Missing semicolon at the end of the statement" << std::endl;
    }*/
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
