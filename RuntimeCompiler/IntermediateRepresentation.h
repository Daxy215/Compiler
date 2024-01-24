#pragma once
#include <memory>
#include <string>

#include "Parser.h"

class IRVariableDeclaration;

class IRVisitor {
public:
    virtual void visit(IRVariableDeclaration* node) = 0;
};

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
};

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

class CodeGenerator : public IRVisitor {
public:
    void visit(IRVariableDeclaration* node) override {
        // Allocate memory for the variable
        std::cout << "sub esp, " << node->bytes << "\n"; // Subtract bytes from ESP to allocate memory
        std::cout << "mov ebp, [" << node->name << "]\n"; // Move the base pointer to the new variable
        
        // Assign a value to the variable
        std::cout << "mov [" << node->name << "], 1\n"; // Move the value 1 to the new variable
    }
};

class IntermediateRepresentation {
public:
    std::unique_ptr<IRNode> generateIR(ASTNode* node);
};
