#include "TACGenerator.h"

#include <set>

using namespace std;

void TACGenerator::generateTAC(ASTNode* node, ASTNode* parent) {
    if(node == nullptr)
        return;
    
    switch(node->type) {
    case NodeType::CLASS: {
            const std::string className = node->value;
            
            tac.push_back(className + " = DEFINE CLASS");
            
            for(auto child : node->children) {
                generateTAC(child, node);
            }
            
            break;
        }
    case NodeType::CONSTRUCTOR: {
            tac.push_back(parent->value + "::" + node->value + " = DEFINE CONSTRUCTOR");
        }
        
        break;
    case NodeType::MEMBER_FUNCTION:
        if(parent)
            tac.push_back(parent->value + "::" + node->value + " = DEFINE FUNCTION");
        else
            tac.push_back("GLOBAL::" + node->value + " = DEFINE FUNCTION");
        
        for(auto child : node->children) {
            generateTAC(child, node);
        }
        
        break;
    case NodeType::LOCAL_VARIABLE_DECLARATION:
        handleLocalVariableDeclaration(node);
        break;
    case NodeType::EXPRESSION:
    case NodeType::STRING:
    case NodeType::VALUE:
        handleExpression(node);
        break;
    case NodeType::OPERATOR:
        handleOperator(node);
        break;
    case NodeType::ASSIGNMENT:
    case NodeType::DELETE_STATEMENT:
        handleAssignment(node);
        break;
    case NodeType::COMPOUND_ASSIGNMENT:
        handleCompoundAssignment(node);
        break;
    case NodeType::FUNCTION_CALL:
        handleFunctionCall(node);
        break;
    case NodeType::IF_STATEMENT:
    case NodeType::ELSEIF_STATEMENT:
    case NodeType::ELSE_STATEMENT:
    case NodeType::FOR_LOOP:
    case NodeType::WHILE_LOOP:
    case NodeType::DO_WHILE_LOOP:
        handleControlFlow(node);
        break;
    case NodeType::COUT_STATEMENT: {
            // Access the children of the COUT_STATEMENT node
            auto children = node->children;
            
            std::string tempString = "t" + std::to_string(tempCounter++);
            std::string tempEndl = "t" + std::to_string(tempCounter++);
            
            tac.push_back(tempString + " = \"" + children[1]->value + "\"");
            
            break;
        }
    default:
        for(auto child : node->children) {
            generateTAC(child, parent);
        }
        
        break;
    }
}

void TACGenerator::handleLocalVariableDeclaration(ASTNode* node) {
    ASTNode* assignment = node->getChildByType(NodeType::COMPOUND_ASSIGNMENT);
    generateTAC(assignment->children[0]);
    tac.push_back("t" + std::to_string(tempCounter++) + " = " + assignment->children[0]->value);
}

void TACGenerator::handleExpression(ASTNode* node) {
    switch(node->type) {
    case NodeType::STRING:
    case NodeType::VALUE:
    //case NodeType::EXPRESSION:
        // t1 = 5.0
        //tac.push_back("t" + std::to_string(tempCounter++) + " = " + node->value);
        
        //break;
    //case NodeType::I:
        // Handle identifiers
        // Example: t1 = x
        //tac.push_back("t" + std::to_string(tempCounter++) + " = " + node->value);
        //break;
    case NodeType::OPERATOR:
    case NodeType::LOGICAL_OR:
    case NodeType::LOGICAL_AND:
        // t1 = t2 + t3
        generateTAC(node->children[0]);
        generateTAC(node->children[1]);
        tac.push_back(
            "t" + std::to_string(tempCounter) + " = t" + std::to_string(tempCounter - 2) + " " + node->value + " t" +
            std::to_string(tempCounter - 1));
        
        tempCounter++;
        break;
    default:
        for (auto child : node->children) {
            generateTAC(child);
        }
        
        break;
    }
}

void TACGenerator::handleOperator(ASTNode* node) {
    if (node->value == "+" || node->value == "-" || node->value == "*" || node->value == "/" ||
        node->value == ">" || node->value == "<" || node->value == "==" || node->value == "&&" || node->value == "||") {
        
        std::string leftOperand = "t" + std::to_string(tempCounter++);
        generateTAC(node->children[0]);
        
        std::string rightOperand = "t" + std::to_string(tempCounter++);
        generateTAC(node->children[1]);
        
        // Generate TAC for comparison or logical operation
        tac.push_back(leftOperand + " = " + node->children[0]->value);
        tac.push_back(rightOperand + " = " + node->children[1]->value);
        
        tac.push_back("t" + std::to_string(tempCounter) + " = " + leftOperand + " " + node->value + " " + rightOperand);
        tempCounter++;
        
        node->value = "t" + std::to_string(tempCounter - 1);
    }
}

void TACGenerator::handleAssignment(ASTNode* node) {
    if(node->isTypeOf(NodeType::DELETE_STATEMENT)) {
        std::string className = node->children[0]->value;
        
        tac.push_back("delete " + className);
        
        return;
    }
    
    if(node->children.size() != 2) {
        cout << "Error: Assignment node expects exactly two children" << '\n';
        return;
    }
    
    // Generate TAC for addition
    generateTAC(node->children[0]);
    generateTAC(node->children[1]);
    tac.push_back("t" + std::to_string(tempCounter) + " = " + "t" + std::to_string(tempCounter - 2));
}

void TACGenerator::handleCompoundAssignment(ASTNode* node) {
    std::string varName = node->children[0]->value;
    
    ASTNode* assignment = node->getChildByType(NodeType::COMPOUND_ASSIGNMENT);
    generateTAC(assignment->children[0]);
    tac.push_back("t" + std::to_string(tempCounter++) + " = " + assignment->children[0]->value);
}

void TACGenerator::handleFunctionCall(ASTNode* node) {
    std::string functionName = node->value;
    std::string argList = "";
    
    ASTNode* paramaters = node->getChildByType(NodeType::PARAMETER);
    
    for (auto& i : paramaters->children) {
        generateTAC(i);
        argList += "t" + std::to_string(tempCounter) + ", ";
    }
    
    // Remove trailing comma
    argList = argList.substr(0, argList.length() - 2);
    
    std::string callInstruction = functionName + "(" + argList + ")";
    tac.push_back(callInstruction);
}

void TACGenerator::handleControlFlow(ASTNode* node) {
    switch (node->type) {
    case NodeType::IF_STATEMENT: {
            ASTNode* conditions = node->getChildByType(NodeType::CONDITIONS);
            const std::string combinedCondition = handleConditions(conditions);
            
            tac.push_back("if " + combinedCondition + " goto " + node->children[1]->children[0]->value);
            
            break;
    }
    case NodeType::ELSEIF_STATEMENT: {
            ASTNode* conditions = node->getChildByType(NodeType::CONDITIONS);
            const std::string combinedCondition = handleConditions(conditions);
            
            tac.push_back("elseif " + combinedCondition + " goto " + node->children[1]->children[0]->value);
            
            break;
    }
    case NodeType::ELSE_STATEMENT: {
            tac.push_back("else goto " + node->children[0]->children[0]->value);
            
            break;
        }
    case NodeType::FOR_LOOP: {
            //ASTNode* conditions = node->getChildByType(NodeType::CONDITIONS);
            //const std::string combinedCondition = handleConditions(conditions);
           /* ASTNode* conditions = node->getChildByType(NodeType::CONDITIONS);
            generateTAC(conditions->children[0]); // Generate TAC for the initializer
            generateTAC(conditions->children[1]); // Generate TAC for the condition
            tac.push_back("if not " + conditions->children[1]->value + " goto " + node->children[2]->children[0]->value);
            
            // If the condition is not met, go to the end of the loop
            generateTAC(node->children[1]); // Generate TAC for the body of the loop
            generateTAC(conditions->children[0]); // Generate TAC for the increment
            tac.push_back("goto " + node->children[1]->children[0]->value); // Go back to the start of the loop
            */
            break;
        }
    case NodeType::WHILE_LOOP: {
            //ASTNode* conditions = node->getChildByType(NodeType::CONDITIONS);
            //tac.push_back(
            //    "if not " + conditions->children[1]->value + " goto " + node->children[2]->children[0]->value);
            // If the condition is not met, go to the end of the loop
            //generateTAC(node->children[1]); // Generate TAC for the body of the loop
            //tac.push_back("goto " + node->children[1]->children[0]->value); // Go back to the start of the loop
            break;
        }
    case NodeType::DO_WHILE_LOOP: {
            //ASTNode* conditions = node->getChildByType(NodeType::CONDITIONS);
            //tac.push_back(
            //    "if not " + conditions->children[1]->value + " goto " + node->children[2]->children[0]->value);
            // If the condition is not met, go to the end of the loop
            //generateTAC(node->children[1]); // Generate TAC for the body of the loop
            //tac.push_back("goto " + node->children[1]->children[0]->value); // Go back to the start of the loop
            break;
    }
    default:
        break;
    }
}

std::string TACGenerator::handleConditions(ASTNode* node) {
    std::string combinedCondition = "";
    
    for(size_t i = 0; i < node->children.size(); ++i) {
        if(node->children[i]->type == NodeType::CONDITIONS) {
            combinedCondition += "(" + handleConditions(node->children[i]) + ")";
        } else {
            generateTAC(node->children[i]);
            combinedCondition += node->children[i]->value;
        }
        
        if(i < node->children.size() - 1) {
            combinedCondition += " ";
            
            if(i+1 < node->children.size() && node->children[i+1] != nullptr) {
                combinedCondition += " " + node->children[i+1]->value + " ";
            }
            
            i++;
        }
    }
    
    return combinedCondition;
}
