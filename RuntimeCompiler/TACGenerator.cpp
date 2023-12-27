#include "TACGenerator.h"

void TACGenerator::generateTAC(ASTNode* node) {
    if (node) {
        if (node->value == "+") {
            std::string leftOperand = getTemp();
            generateTAC(node->children[0]);
            std::string rightOperand = getTemp();
            generateTAC(node->children[1]);
            
            // Generate TAC for addition
            tac.push_back(leftOperand + " = " + node->children[0]->value);
            tac.push_back(rightOperand + " = " + node->children[1]->value);
            tac.push_back("t" + std::to_string(tempCount) + " = " + leftOperand + " + " + rightOperand);
            tempCount++;
            
            node->value = "t" + std::to_string(tempCount - 1);
        } else if (node->value == "*") {
            std::string leftOperand = getTemp();
            generateTAC(node->children[0]);
            std::string rightOperand = getTemp();
            generateTAC(node->children[1]);
            
            tac.push_back(leftOperand + " = " + node->children[0]->value);
            tac.push_back(rightOperand + " = " + node->children[1]->value);
            tac.push_back("t" + std::to_string(tempCount) + " = " + leftOperand + " * " + rightOperand);
            tempCount++;
            
            node->value = "t" + std::to_string(tempCount - 1);
        } else {
            // Leaf node - variable or constant
            node->value = node->value; // Set the value of leaf node
        }
    }
}
