#include "TACGenerator.h"

using namespace std;

void TACGenerator::generateTAC(ASTNode* node, int& tempCounter) {
    if(node == nullptr)
        return;
    
    switch(node->type) {
    case NodeType::OPERATOR:
        if(node->value == "+" || node->value == "-" || node->value == "*" || node->value == "/") {
            // Generate TAC for addition
            /*
            generateTAC(node->children[0], tempCounter);
            generateTAC(node->children[1], tempCounter);
            
        */
            
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
        
        break;
    case NodeType::ASSIGNMENT:
        if(node->children.size() != 2) {
            cout << "Error: Assignment node expects exactly two children" << endl;
            break;
        }
        
        // Generate TAC for addition
        generateTAC(node->children[0], tempCounter);
        generateTAC(node->children[1], tempCounter);
        cout << "t" << ++tempCounter << " = " << "t" << tempCounter - 2 << endl;
        break;
    default:
        for(auto child : node->children) {
            generateTAC(child, tempCounter);
        }
        break;
    }
    
    /*if (node) {
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
    }*/
}
