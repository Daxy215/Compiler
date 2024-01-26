#pragma once
#ifndef IRVisitor_H
#define IRVisitor_H

class IRVariableDeclaration;

class IRVisitor {
public:
    virtual void visit(IRVariableDeclaration* node) = 0;
};

#endif
