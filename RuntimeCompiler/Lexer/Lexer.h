#pragma once

#ifndef LEXER_H
#define LEXER_H

#include "TokenType.h"

#include <string>
#include <unordered_set>
#include <vector>

//TODO; Handle += or -=.. or *=.. /=.. etc I think I did??

struct Token {
    LexerNameSpace::TokenType type;
    std::string value;
    size_t startPos, endPos, line, column;
};

class Lexer {
public:
    Lexer() {}
    
    std::vector<Token> generateTokens(std::string code);
private:
    void pushToken(char c, LexerNameSpace::TokenType type);
    std::string extractNamespaceName(const std::string& input);
    
private:
    std::string code, currentToken;
    size_t currentPos, currentLine, currentColumn;
    
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