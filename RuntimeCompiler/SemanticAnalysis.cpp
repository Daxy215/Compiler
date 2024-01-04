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
       std::cout << "Class: " << currentScope;
       /*if (!node->parent->value.empty()) {
           std::cout << " (inherits from " << node->parent->value << ")";
       }*/
       std::cout << "\n";
   } else if (node->type == NodeType::MEMBER_FUNCTION || node->type == NodeType::CONSTRUCTOR) {
       std::string returnType = split(node->children[0]->value);
       std::string identifier = split(node->value);
       std::string scopePrefix = (currentScope != "") ? currentScope + "::" : "";
       std::string fullName = scopePrefix + identifier;
       
       std::cout << "   Member functions:\n";
       std::cout << "       - " << returnType << " " << fullName << " ; " << node->isGlobal << "\n";
       
       if (isFunctionOverloaded(identifier, currentScope)) {
           identifier = generateUniqueFunctionName(identifier);
           fullName = scopePrefix + identifier;
       }

       addToSymbolTable(returnType, identifier, fullName, currentScope, node->type);
   } else if (node->type == NodeType::MEMBER_VARIABLE) {
       std::string returnType = split(node->children[0]->value);
       std::string identifier = split(node->value);
       std::string scopePrefix = (currentScope != "") ? currentScope + "::" : "";
       std::string fullName = scopePrefix + identifier;

       std::cout << "   Member variables:\n";
       std::cout << "       - " << returnType << " " << fullName << " ; " << node->isGlobal << "\n";

       addToSymbolTable(returnType, identifier, fullName, currentScope, node->type);
   } else if (node->type == NodeType::MEMBER_FUNCTION) {
       std::string returnType = split(node->children[0]->value);
       std::string identifier = split(node->value);
       std::string fullName = identifier;

       std::cout << "Function: " << returnType << " " << fullName << " ; " << node->isGlobal << "\n";

       addToSymbolTable(returnType, identifier, fullName, "", node->type);
   } else if (node->type == NodeType::LOCAL_VARIABLE_DECLARATION) {
       std::string returnType = split(node->children[0]->value);
       std::string identifier = split(node->value);
       std::string fullName = identifier;

       std::cout << "Variables:\n";
       std::cout << "   - " << returnType << " " << fullName << "\n";

       addToSymbolTable(returnType, identifier, fullName, "", node->type);
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
