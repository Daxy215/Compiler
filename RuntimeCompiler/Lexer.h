#pragma once

#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <unordered_set>
#include <vector>

enum class TokenType {
    IDENTIFIER,
    PREPROCESSOR_DIRECTIVE,
    KEYWORD,
    LEFT_PAREN,
    RIGHT_PAREN,
    OPERATOR,
    INTEGER_LITERAL,
    FLOATING_POINT_LITERAL,
    STRING_LITERAL,
    STRUCT,
    CLASS,
    POINTER,
    REFERENCE,
    SEMICOLON,
    COMMA,
    LEFT_BRACE,
    RIGHT_BRACE,
    NAMESPACE,
    RETURN,
    COLON,
    
    UNKNOWN,
    END
};

struct Token {
    TokenType type;
    std::string value;
};

class Lexer {
public:
    Lexer();
    
    std::vector<Token> generateTokens(std::string code);

private:
    std::string extractNamespaceName(const std::string& input);
    
private:
    std::string currentToken;
    Token prevToken = {};
    
    std::vector<Token> tokens;
    
    const std::unordered_set<std::string> keywords =
    {  "using", "return", "public", "private", "protected", "virtual",
    "final", "void", "const", "override", "int", "float", "char",
    "double", "long",  "new", "delete", "if", "for", "else", "while"};
};

#endif