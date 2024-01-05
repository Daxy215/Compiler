#pragma once

#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <vector>
#include "Lexer.h"

// Please make this tidier.........
enum class NodeType {
    // Basic Types
    PROGRAM,
    CLASS,
    CLASS_BODY,
    MEMBER_FUNCTION,
    MEMBER_VARIABLE,
    VARIABLE_TYPE,
    INITIALIZER_VARIABLE,
    PARAMETER_VARIABLE,
    FUNCTION_BODY,
    OPERATOR,
    EXPRESSION,
    INCLUDE_DIRECTIVE,
    NAMESPACE,
    LOCAL_VARIABLE_DECLARATION,
    PARAMETER,
    ASSIGNMENT,
    INHERITANCE,
    CONSTRUCTOR,
    CONDITIONS,
    BLOCK,
    
    // Control Structures
    FOR_LOOP,
    WHILE_LOOP,
    DO_WHILE_LOOP,
    
    POINTER,
    REFERENCE,
    FUNCTION_CALL,
    
    // Logicals
    LOGICAL_OR,
    LOGICAL_AND,
    
    // Statements
    BREAK_STATEMENT,
    CONTINUE_STATEMENT,
    RETURN_STATEMENT,
    DELETE_STATEMENT,
    COMPOUND_ASSIGNMENT,
    IF_STATEMENT,
    ELSE_STATEMENT,
    
    // Modifiers
    MODIFIERS,
    
    // Access Modifiers
    ConstModifier,
    OverrideModifier,
    FinalModifier,
    ProtectedModifier,
    VirtualModifier,
    AbstractModifier,
    
    // Variable Modifiers
    ShortModifier,
 };

enum class DataType {
    INT,
    FLOAT,
    DOUBLE,
    VOID,
    // Other data types...
};

struct ASTNode {
    NodeType type;
    DataType dataType;
    
    std::string value;
    bool isGlobal = false;
    
    std::vector<ASTNode*> children;
    
    //ASTNode(const std::string& val) : type(NodeType::ConstModifier), value(val) {}
    ASTNode(NodeType t, const std::string& val) : type(t), value(val) {}
    ~ASTNode() {
        for (auto child : children) {
            delete child;
        }
    }
    
    void setIsGlobal(ASTNode* node, bool cond) {
        if(node == nullptr)
            return;
        
        node->isGlobal = cond;
        
        for (auto child : node->children) {
            setIsGlobal(child, cond);
        }
    }
};

class Parser {
public:
    Parser(const std::vector<Token>& t) : isInClass(false), currentTokenIndex(0), tokens(t) {}

    ASTNode* parseCode();
private:
    ASTNode* parseIncludeDirective();
    ASTNode* parseNameSpace();
    ASTNode* parseClassDeclaration();

    /**
     * className is used to check for constructor.
     */
    ASTNode* parseClassBody(std::string className);

    /**
     * \brief
     * Currently supports;\n
     * \n
     * Circle(double r) : radius(r) {}\n
     * Circle(double r) {}\n
     * Circle() {}\n
     * 
     * \return
     * - Constructor: Circle(double r)\n
     * - Parameter: r\n
     * - Initialization: radius(r)\n
     * \n
     * - Constructor: Circle(double r)\n
     * - Parameter: r\n
     *\n
     * - Constructor: Circle()\n
     */
    ASTNode* parseConstructor();

    /**
     * Handles Variables
     * TODO; If empty, add "EmptyBody" tag.
     */
    ASTNode* parseMember(NodeType memberDeclarationType);

    /**
     * Handles function parameters
     */
    ASTNode* parseFunction(const std::string& returnType, const std::string& functionName);
    ASTNode* parseFunctionBody();
    ASTNode* parseStatement();
    ASTNode* parseFunctionCall();
    ASTNode* parseIfStatement();
    ASTNode* parseElseStatement();
    ASTNode* parseConditions();
    ASTNode* parseCondition();
    ASTNode* parseVariable(NodeType memberDeclarationType, const std::string& memberType);
    ASTNode* parseReference(NodeType memberDeclarationType, const std::string& memberType);
    std::vector<std::string> parseFunctionParameters();
    
    ASTNode* expression();
    ASTNode* term();
    ASTNode* factor();
private: // Helper functions
    auto match(TokenType type) -> bool {
        return currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type == type;    
    }
    
    auto match(TokenType type, std::string value) -> bool {
        return currentTokenIndex < tokens.size() &&  tokens[currentTokenIndex].type == type && tokens[currentTokenIndex].value._Equal(value);
    }
    
    auto matchNext(TokenType type) -> bool {
        return currentTokenIndex + 1 < tokens.size() - 1 && tokens[currentTokenIndex + 1].type == type;
    }
    
    auto matchNext(TokenType type, std::string value) -> bool {
        return currentTokenIndex + 1 < tokens.size() - 1 && tokens[currentTokenIndex + 1].type == type && tokens[currentTokenIndex + 1].value._Equal(value);
    }
    
    Token consumeToken() {
        if (currentTokenIndex < tokens.size() - 1) {
            return tokens[currentTokenIndex++];
        }
        
        std::cout << "Trying to consume beyond available tokens" << '\n';
        
        return {};
    }

public:
    void printAST(ASTNode* node, int depth = 0) {
        if (!node) {
            return;
        }

        for (int i = 0; i < depth; ++i) {
            std::cout << "  "; // Add indentation based on depth for better visualization
        }

        std::cout << static_cast<int>(node->type) << " - " << node->value << '\n';

        for (ASTNode* child : node->children) {
            printAST(child, depth + 1); // Recursively print child nodes with increased depth
        }
    }
private:
    bool isInClass;
    std::string className;
    
    size_t currentTokenIndex;
    std::vector<Token> tokens;
};

#endif