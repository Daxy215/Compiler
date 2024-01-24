#pragma once
#include <string>

#include "../Parser.h"

class TACGenerator {
public:
    void generateTAC(ASTNode* node, ASTNode* parent = nullptr);

private:
    void handleLocalVariableDeclaration(ASTNode* node);
    void handleExpression(ASTNode* node);
    void handleOperator(ASTNode* node);
    void handleAssignment(ASTNode* node);
    void handleCompoundAssignment(ASTNode* node);
    void handleFunctionCall(ASTNode* node);
    void handleControlFlow(ASTNode* node);
    
    std::string handleConditions(ASTNode* node);
    
private:
    int tempCounter = 0;
    
public:
    std::vector<std::string> tac;
};
