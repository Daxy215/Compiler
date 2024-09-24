#pragma once

#include <memory>
#include <string>

#include "IRVisitor.h"
#include "../Parser.h"

// https://cs.lmu.edu/~ray/notes/ir/

enum Command {
    INCLUDE,
    
    // Class Commands
    CLASS_START,
    CLASS_END,
    
    // Functions Commands
    PARAMETER,
    
    CONSTRUCTOR_START,
    CONSTRUCTOR_END,
    
    FUNCTION,
    FUNCTION_CALL,
    
    // Variable Commands
    ALLOC,
    STORE,
    STORETHIS, // Special case for this.x

    // Statements Commands
    CONTINUE,
    BREAK,
    RETURN,

    // Operators
    EQUALS,
    PLUS,
    MINUS,
    MULITPLY,
    DIVIDE,
    
    // Conditional Statements Commands
    IF_STATEMENT,
    ELSE_STATEMENT,
    ELSEIF_STATEMENT,
    
    FOR,
    FOREACH,
    WHILE,
    DO_WHILE,
    
    // Other Commands
    LABEL,
};

class IR {
public:
    // (FUNCTION_CALL, "std::cout")
    IR(Command command, std::string temp1, std::string parent)
        : command(command), temp1(temp1), parent(parent) {
        
    }
    
    // (ALLOC, 4, total)
    IR(Command command, std::string temp1, std::string temp2, std::string parent)
        : command(command), temp1(temp1), temp2(temp2), parent(parent) {
        
    }
    
    // (MUL, temp1, temp2, temp3)
    IR(Command command, std::string temp1, std::string temp2, std::string temp3, std::string parent)
        : command(command), temp1(temp1), temp2(temp2), temp3(temp3), parent(parent) {
        
    }
    
    bool hasParent() {
        return !parent.empty();
    }
    
public:
    Command command;
    std::string temp1, temp2, temp3, parent = "";
};

class IntermediateRepresentation {
public:
    void generateIR(ASTNode* node, ASTNode* parent);
    void handleControlFlow(ASTNode* nod, ASTNode* parent);
    void generatePointerIR(ASTNode* node, ASTNode* parent);
    
    std::string handleConditions(ASTNode* node, ASTNode* parent);
    
    void addCommand(Command command, std::string temp1, std::string parent) {
        commands.push_back(new IR(command, temp1, parent));
        std::cerr << "(" << command << ", " << temp1 << ") " << parent << '\n';
    }
    
    void addCommand(Command command, std::string temp1, std::string temp2, std::string parent) {
        commands.push_back(new IR(command, temp1, temp2, parent));
        std::cerr << "(" << command << ", " << temp1 << ", " << temp2 << ") " << parent << '\n';
    }
    
    void addCommand(Command command, std::string temp1, std::string temp2, std::string temp3, std::string parent) {
        commands.push_back(new IR(command, temp1, temp2, temp3, parent));
        std::cerr << "(" << command << ", " << temp1 << ", " << temp2 << ", " << temp3 << ") " << parent << '\n';
    }
    
public:
    int tempCounter = 1, labelCounter = 1;
    
    std::vector<IR*> commands;
};
