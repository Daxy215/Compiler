#include "TACGenerator.h"

using namespace std;

void TACGenerator::generateTAC(ASTNode* node, int& tempCounter) {
    if(node == nullptr)
        return;
   
    switch(node->type) {
    case NodeType::OPERATOR:
        handleOperator(node, tempCounter);
        break;
    case NodeType::ASSIGNMENT:
    case NodeType::DELETE_STATEMENT:
        handleAssignment(node, tempCounter);
        break;
    case NodeType::FUNCTION_CALL:
        handleFunctionCall(node, tempCounter);
        break;
    case NodeType::IF_STATEMENT:
    case NodeType::FOR_LOOP:
    case NodeType::WHILE_LOOP:
        handleControlFlow(node, tempCounter);
        break;
    default:
        for(auto child : node->children) {
            generateTAC(child, tempCounter);
        }
        
        break;
    }
}

void TACGenerator::handleOperator(ASTNode* node, int& tempCounter) {
    if(node->value == "+" || node->value == "-" || node->value == "*" || node->value == "/") {
        std::string leftOperand = "t" + std::to_string(tempCounter++);
        generateTAC(node->children[0], tempCounter);
        std::string rightOperand = "t" + std::to_string(tempCounter++);
        generateTAC(node->children[1], tempCounter);
        
        // Generate TAC for addition
        tac.push_back(leftOperand + " = " + node->children[0]->value);
        tac.push_back(rightOperand + " = " + node->children[1]->value);
        tac.push_back("t" + std::to_string(tempCounter) + " = " + leftOperand + " " + node->value + " " + rightOperand);
        tempCounter++;
        
        node->value = "t" + std::to_string(tempCounter - 1);
    }
}

void TACGenerator::handleAssignment(ASTNode* node, int& tempCounter) {
    if(node->children.size() != 2) {
        cout << "Error: Assignment node expects exactly two children" << endl;
        return;
    }
    
    if(node->isTypeOf(NodeType::DELETE_STATEMENT)) {
        std::string className = node->children[0]->value;
        tac.push_back("la $t1, " + className); // Load address of class
        tac.push_back("jalr $t1"); // Call class constructor
        tac.push_back("move $t0, $v0"); // Move returned pointer to $t0
        
        return;
    }
    
    // Generate TAC for addition
    generateTAC(node->children[0], tempCounter);
    generateTAC(node->children[1], tempCounter);
    tac.push_back("t" + std::to_string(tempCounter) + " = " + "t" + std::to_string(tempCounter - 2));
    
    //cout << "t" << ++tempCounter << " = " << "t" << tempCounter - 2 << endl;
}

void TACGenerator::handleFunctionCall(ASTNode* node, int& tempCounter) {
    std::string functionName = node->value;
    std::string argList = "";
    
    for (int i = 0; i < node->children.size(); i++) {
        generateTAC(node->children[i], tempCounter);
        argList += "t" + std::to_string(tempCounter) + ", ";
    }
    
    // Remove trailing comma
    argList = argList.substr(0, argList.length() - 2);
    
    std::string callInstruction = functionName + "(" + argList + ")";
    tac.push_back(callInstruction);
    //std::cout << callInstruction << "\n";
}

void TACGenerator::handleControlFlow(ASTNode* node, int& tempCounter) {
    switch (node->type) {
    case NodeType::IF_STATEMENT:
        generateTAC(node->children[0], tempCounter);
        tac.push_back("if " + node->children[0]->value + " goto " + node->children[1]->children[0]->value);
        //std::cout << "if " + node->children[0]->value << " goto " + node->children[1]->value << "\n";
        break;
    case NodeType::FOR_LOOP:
        // TODO: Implement this
            break;
    case NodeType::WHILE_LOOP:
        // TODO: Implement this
            break;
    default:
        break;
    }
}
