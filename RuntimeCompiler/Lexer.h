#pragma once

#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <unordered_set>
#include <vector>

//TODO; Handle += or -=.. or *=.. /=.. etc

enum class TokenType {
    // ???
    IDENTIFIER,
    KEYWORD,

    // Statements
    BREAK_STATEMENT,
    CONTINUE_STATEMENT,
    RETURN_STATEMENT,
    
    // Operations
    OPERATOR,
    
    // Statement types
    FOR_LOOP,
    IF_STATEMENT,
    ELSE_STATEMENT,
    
    // Logical operations
    LOGICAL_AND,
    LOGICAL_OR,
    EQUAL,
    NOT_EQUAL,
    LESS_THAN_EQUAL,
    GREATER_THAN_EQUAL,
    LEFT_SHIFT_ASSIGN,
    RIGHT_SHIFT_ASSIGN,
    POINTER_MEMBER_ACCESS,
    SCOPE_POINTER_MEMBER_ACCESS,
    ELLIPSIS,
    SINGLE_LINE_COMMENT,
    MULTI_LINE_COMMENT_START,
    MULTI_LINE_COMMENT_END,
    MODULUS_ASSIGN,
    BITWISE_XOR_ASSIGN,
    BITWISE_OR_ASSIGN,
    BITWISE_AND_ASSIGN,
    BITWISE_XOR,
    BITWISE_OR,
    MODULUS,
    BITWISE_NOT,
    BITWISE_AND,
    
    // Special cases
    /*INCREMENT,
    INCREMENT_EQUAL,
    DECREMENT,
    DECREMENT_EQUAL,
    MULTIPLICATION_EQUAL,
    DIVISION_EQUAL,*/
    
    POINTER,
    REFERENCE,
    
    // Variable types
    STRING_LITERAL,
    INTEGER_LITERAL,
    FLOATING_POINT_LITERAL,

    // Modifiers??
    STRUCT,
    CLASS,
    
    // ()
    LEFT_PAREN,
    RIGHT_PAREN,

    // {}
    LEFT_BRACE,
    RIGHT_BRACE,

    // []
    LEFT_SQUARE_BRACE,
    RIGHT_SQUARE_BRACE,
    
    // <>
    LEFT_ANGLE_BRACE,
    RIGHT_ANGLE_BRACE,
    
    // Something..
    SEMICOLON,
    COMMA,
    COLON,
    DOT,
    
    // Idek
    NAMESPACE,
    PREPROCESSOR_DIRECTIVE,
};

struct Token {
    TokenType type;
    std::string value;
};

class Lexer {
public:
    Lexer() {}
    
    std::vector<Token> generateTokens(std::string code);
private:
    void pushToken(char c, TokenType type);
    std::string extractNamespaceName(const std::string& input);
    
private:
    std::string currentToken;
    Token prevToken = {};
    
    std::vector<Token> tokens;
    
    /**
     * List of keywords
     */
    const std::unordered_set<std::string> keywords = {
        "return", "public", "private", "protected", "virtual", "final",
        "void", "const", "override", "int", "float", "char",
        "double", "long", "new", "delete"
    };
};

#endif