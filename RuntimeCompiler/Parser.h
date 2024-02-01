#pragma once

#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <vector>
#include "Lexer/Lexer.h"

// Please make this tidier.........
enum class NodeType {
    // Basic Types
    PROGRAM,
    
    CLASS,
    ENUM,
    ENUM_VALUE,
    
    CLASS_BODY,
    MEMBER_FUNCTION,
    MEMBER_VARIABLE,
    VARIABLE_TYPE,
    RETURN_TYPE,
    VECTOR_TYPE,
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
    ACCESSTYPE,
    
    // Control Structures
    FOR_LOOP,
    WHILE_LOOP,
    DO_WHILE_LOOP,
    
    POINTER,
    REFERENCE,
    FUNCTION_CALL,
    ARRAY_CALL,
    
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
    ELSEIF_STATEMENT,
    ELSE_STATEMENT,
    
    // Modifiers
    MODIFIERS,
    
    // Variable types?
    STRING,
    VALUE,
    
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

// enum class DataType {
//     INT,
//     FLOAT,
//     DOUBLE,
//     VOID,
//     // Other data types...
// };

struct ASTNode {
    Token token;
    NodeType type;
    //DataType dataType;
    
    std::string value;
    bool isGlobal = false;
    
    std::vector<ASTNode*> children;
    
    //ASTNode(const std::string& val) : type(NodeType::ConstModifier), value(val) {}
    ASTNode(Token token, NodeType t, const std::string& val) : token(token), type(t), value(val) {}
    ~ASTNode() {
        for (auto child : children) {
            delete child;
        }
    }

    ASTNode* getChildByType(NodeType nodeType) {
        return getChildByType(this, nodeType);
    }
    
    ASTNode* getChildByType(ASTNode* node, NodeType nodeType) {
        if (node->getType() == nodeType) {
            return node;
        }
        
        for (ASTNode* child : node->children) {
            ASTNode* grandChild = getChildByType(child, nodeType);
            if (grandChild != nullptr) {
                return grandChild;
            }
        }
        
        return nullptr;
    }
    
    void setIsGlobal(ASTNode* node, bool cond) {
        if(node == nullptr)
            return;
        
        node->isGlobal = cond;
        
        for (auto child : node->children) {
            setIsGlobal(child, cond);
        }
    }

    // Getters & Setters
    std::string getValue() {
        return value;
    }

    NodeType getType() {
        return type;
    }

    bool isTypeOf(NodeType other) {
        return type == other;
    }
    
    void addChild(Token token, const NodeType& nodeType, const std::string& nodeValue) {
        children.push_back(new ASTNode(token, nodeType, nodeValue));
    }

    void addChild(ASTNode* node) {
        children.push_back(node);
    }
};

class Parser {
public:
    Parser() : isInClass(false), currentTokenIndex(0) {}
    
    ASTNode* parseCode(const std::vector<Token>& t);
private:
    ASTNode* parseIncludeDirective();
    ASTNode* parseNameSpace();
    ASTNode* parseEnumDeclaration();
    ASTNode* parseStructDeclaration();
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
    ASTNode* parseNamespacesUsage(int counter);
    ASTNode* parseVariable(NodeType memberDeclarationType, const std::string& memberType);
    ASTNode* parseReference(NodeType memberDeclarationType, const std::string& memberType);
    std::vector<std::string> parseFunctionParameters();
    
    ASTNode* expression();
    ASTNode* term();
    ASTNode* factor();
private: // Helper functions
    auto match(std::string value) -> bool {
        return currentTokenIndex < tokens.size() && tokens[currentTokenIndex].value == value;
    }
    
    auto match(LexerNameSpace::TokenType type) -> bool {
        return currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type == type;    
    }
    
    auto match(LexerNameSpace::TokenType type, std::string value) -> bool {
        return currentTokenIndex < tokens.size() &&  tokens[currentTokenIndex].type == type && tokens[currentTokenIndex].value._Equal(value);
    }
    
    auto matchNext(LexerNameSpace::TokenType type) -> bool {
        return currentTokenIndex + 1 < tokens.size() - 1 && tokens[currentTokenIndex + 1].type == type;
    }
    
    auto matchNext(LexerNameSpace::TokenType type, std::string value) -> bool {
        return currentTokenIndex + 1 < tokens.size() - 1 && tokens[currentTokenIndex + 1].type == type && tokens[currentTokenIndex + 1].value._Equal(value);
    }
    
    Token consumeToken() {
        if (currentTokenIndex < tokens.size() - 1) {
            return tokens[currentTokenIndex++];
        }
        
        std::cout << "Trying to consume beyond available tokens" << '\n';
        
        return {};
    }

    Token currentToken() {
        return tokens[currentTokenIndex];
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
    //std::string className;
    
    size_t currentTokenIndex;
    std::vector<Token> tokens;
};

#endif