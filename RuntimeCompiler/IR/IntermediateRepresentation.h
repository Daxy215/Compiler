#pragma once

#include <memory>
#include <string>

#include "IRVisitor.h"
#include "../Parser.h"

// https://cs.lmu.edu/~ray/notes/ir/
/*
class IRNode {
public:
    virtual ~IRNode() {}
    virtual void accept(IRVisitor* visitor) = 0;
};

class IRVariableDeclaration : public IRNode {
public:
    IRVariableDeclaration(std::string name, std::string type, size_t bytes)
        : name(name), type(type), bytes(bytes) {
        
    }
    
    void accept(IRVisitor* visitor) override {
        visitor->visit(this);
    }
    
public:
    std::string name;
    std::string type;
    size_t bytes;
};*/

class IR {
public:
    // (FUNCTION_CALL, "std::cout")
    IR(std::string command, std::string temp1, std::string parent)
        : command(command), temp1(temp1), parent(parent) {
        
    }
    
    // (ALLOC, 4, total)
    IR(std::string command, std::string temp1, std::string temp2, std::string parent)
        : command(command), temp1(temp1), temp2(temp2), parent(parent) {
        
    }
    
    // (MUL, temp1, temp2, temp3)
    IR(std::string command, std::string temp1, std::string temp2, std::string temp3, std::string parent)
        : command(command), temp1(temp1), temp2(temp2), temp3(temp3), parent(parent) {
        
    }
    
    bool hasParent() {
        return !parent.empty();
    }
    
public:
    std::string command, temp1, temp2, temp3, parent = "", moveToParent = "";
};

/*
class IRBinaryOperation : public IRNode {
public:
    std::string op;
    std::unique_ptr<IRNode> left;
    std::unique_ptr<IRNode> right;
};

class IRPrintStatement : public IRNode {
public:
    std::string message;
};

class IRAssignment : public IRNode {
public:
    std::string variableName;
    std::unique_ptr<IRNode> value;
};

class IROperator : public IRNode {
public:
    std::string op;
    std::unique_ptr<IRNode> leftOperand;
    std::unique_ptr<IRNode> rightOperand;
    
    IROperator(const std::string& op, std::unique_ptr<IRNode> left, std::unique_ptr<IRNode> right) 
        : op(op), leftOperand(std::move(left)), rightOperand(std::move(right)) {}
};

class IRIfElseStatement : public IRNode {
public:
    std::unique_ptr<IRNode> condition;
    std::unique_ptr<IRNode> thenPart;
    std::unique_ptr<IRNode> elsePart;
};
*/

class IntermediateRepresentation {
public:
    void generateIR(ASTNode* node, ASTNode* parent);
    void handleControlFlow(ASTNode* nod, ASTNode* parent);

    std::string handleConditions(ASTNode* node, ASTNode* parent);
    
    void addCommand(std::string command, std::string temp1, std::string parent) {
        commands.push_back(new IR(command, temp1, parent));
    }
    
    void addCommand(std::string command, std::string temp1, std::string temp2, std::string parent) {
        commands.push_back(new IR(command, temp1, temp2, parent));
    }
    
    void addCommand(std::string command, std::string temp1, std::string temp2, std::string temp3, std::string parent) {
        commands.push_back(new IR(command, temp1, temp2, temp3, parent));
    }
    
public:
    int tempCounter = 1;
    
    std::vector<IR*> commands;
};
