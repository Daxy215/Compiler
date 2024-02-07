#include "Compiler.h"

#include <fstream>
#include <regex>

std::string Compiler::code = "";

Compiler::Compiler() {
    
}

bool Compiler::compileClass(std::string classPath) {
    return false;
}

bool Compiler::compileCode(std::string code) {
    // TODO; Preproccess the code.
    
    Compiler::code = code;
    
    std::vector<Token> tokens = lexer->generateTokens(code);
    
    std::cout << "Generated tokens: \n\n";
    
    for (const auto& token : tokens) {
        std::cout << "Token Type: " << static_cast<int>(token.type) << ", Value: " << token.value << "\n";
    }
    
    // TODO; Check make this a class.. Perhaps inside lexer
    /*if(checkCodeBalance(tokens)) {
        std::cout << "No balancing problems :D\n";
    } else {
        std::cout << "Code not balanced\n";
        
        return 1;
    }*/
    
    ASTNode* root = parser->parseCode(tokens);
    
    std::cout << "\n\nAbstract Syntax Tree:" << "\n";
    parser->printAST(root);
    
    //std::cout << "\n\nControl Flow Graph:" << "\n\n";
    //std::map<ASTNode*, CFGNode*> cfgNodes;
    
    //cfg->generateCFG(root, cfgNodes);
    //cfg->printCFG(cfgNodes[root], cfgNodes);
    
    std::cout << "\n\n\nSemanticAnalyzer\n\n\n";
    
    semanticAnalyzer->generateSymbolTable(root);
    
    std::cout << "\n\nIR;\n\n";
    
    intermediateRepresentation->generateIR(root, nullptr);
    //intermediateRepresentation->addCommand("FUNCTION_CALL", "main", "");
    
    //for(IR* ir : intermediateRepresentation->commands)
        //std::cout << "(" << ir->command << ", " << ir->temp1 << ", " << ir->temp2 << ", " << ir->temp3 << ") " << ir->parent << '\n';
    
    //std::cout << "\n\nEVALUATOR;\n\n";
    
    //int value = evaluator->Evaluate(intermediateRepresentation->commands);
    
    /*std::cerr << "\n";
    
    for(auto& val : evaluator->memory) {
        if(!val.first.find("temp"))
            continue;
        
        std::cerr << "Variable: " << val.first << " = " << val.second << '\n';
    }
    
    std::cout << "\n\n\n\n";*/
    
    //std::cout << "\n\nTAC;\n\n";
    
    //tacGenerator->generateTAC(root);
    
    //for(std::string f : tacGenerator->tac)
    //    std::cout << f << '\n';
    
    //TACEvaluation* evaluation = new TACEvaluation();
    //evaluation->interpretTAC(tac->tac);
    
    assemblyGenerator->generateCode(intermediateRepresentation->commands);
    
    //std::cout << "\n\n\n\n";
    
    // Print file content(for testing)
    //std::ifstream f("generated_code.asm");
    
    //if (f.is_open())
      //  std::cout << f.rdbuf();
    
    return true;
}