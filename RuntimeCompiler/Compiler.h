#pragma once
#include <string>

#include "AssemblyGenerator.h"
#include "IntermediateRepresentation.h"
#include "Lexer.h"
#include "Parser.h"
#include "SemanticAnalyzer/SemanticAnalyzer.h"
#include "TAC/TACGenerator.h"

class Compiler {
public:
    Compiler();
    
    bool compileClass(std::string classPath);
    bool compileCode(std::string code);

public:
    bool checkBalance(std::vector<Token>& tokens, TokenType left, TokenType right) {
        int leftBraceCount = 0;
        int rightBraceCount = 0;
    
        for (const auto& token : tokens) {
            if (token.type == left) {
                leftBraceCount++;
            } else if (token.type == right) {
                rightBraceCount++;
                if (rightBraceCount > leftBraceCount) {
                    return false;
                }
            }
        }
    
        if (leftBraceCount != rightBraceCount) {
            return false;
        }
    
        return true;
    }

    bool checkCodeBalance(std::vector<Token>& tokens) {
        bool braces = checkBalance(tokens, TokenType::LEFT_BRACE, TokenType::RIGHT_BRACE);
        bool parens = checkBalance(tokens, TokenType::LEFT_PAREN, TokenType::RIGHT_PAREN);
        bool squareBraces = checkBalance(tokens, TokenType::LEFT_SQUARE_BRACE, TokenType::RIGHT_SQUARE_BRACE);
        //bool angleBrackets = checkBalance(tokens, TokenType::LEFT_ANGLE_BRACE, TokenType::RIGHT_ANGLE_BRACE);
    
        if(braces && parens && squareBraces/* && angleBrackets*/) {
            return true;
        }
    
        return false;
    }
    
private:
    Lexer* lexer = new Lexer();
    Parser* parser = new Parser();
    SemanticAnalyzer* semanticAnalyzer = new SemanticAnalyzer();
    TACGenerator* tacGenerator = new TACGenerator();
    IntermediateRepresentation* intermediateRepresentation = new IntermediateRepresentation();
    AssemblyGenerator* assemblyGenerator = new AssemblyGenerator();
};
