#pragma once
#include <filesystem>
#include <string>

#include "PreProcessor.h"
#include "../AssemblyGenerator.h"
#include "../IR/IntermediateRepresentation.h"
#include "../Lexer/Lexer.h"
#include "../CFG/ControlFlowGraph.h"
#include "../IR/Evaluator.h"
#include "../SemanticAnalyzer/SemanticAnalyzer.h"
#include "../TAC/TACGenerator.h"

class Compiler {
public:
    Compiler();

    bool compile(std::string path);
    bool compileClass(std::string classPath);
    bool compileCode(std::string code);

private:

private:
    bool checkBalance(std::vector<Token>& tokens, LexerNameSpace::TokenType left, LexerNameSpace::TokenType right) {
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
        bool braces = checkBalance(tokens, LexerNameSpace::TokenType::LEFT_BRACE, LexerNameSpace::TokenType::RIGHT_BRACE);
        bool parens = checkBalance(tokens, LexerNameSpace::TokenType::LEFT_PAREN, LexerNameSpace::TokenType::RIGHT_PAREN);
        bool squareBraces = checkBalance(tokens, LexerNameSpace::TokenType::LEFT_SQUARE_BRACE, LexerNameSpace::TokenType::RIGHT_SQUARE_BRACE);
        //bool angleBrackets = checkBalance(tokens, LexerNameSpace::TokenType::LEFT_ANGLE_BRACE, LexerNameSpace::TokenType::RIGHT_ANGLE_BRACE);
        
        if(braces && parens && squareBraces/* && angleBrackets*/) {
            return true;
        }
        
        return false;
    }
    
public:
    static std::string code;
private:
    Lexer* lexer = new Lexer();
    Parser* parser = new Parser();
    ControlFlowGraph* cfg = new ControlFlowGraph();
    SemanticAnalyzer* semanticAnalyzer = new SemanticAnalyzer();
    TACGenerator* tacGenerator = new TACGenerator();
    IntermediateRepresentation* intermediateRepresentation = new IntermediateRepresentation();
    Evaluator* evaluator = new Evaluator();
    AssemblyGenerator* assemblyGenerator = new AssemblyGenerator();
    PreProcessor* preProcessor = new PreProcessor();
};
