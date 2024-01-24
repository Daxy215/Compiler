#include "Compiler.h"

#include <fstream>

Compiler::Compiler() {
    
}

bool Compiler::compileClass(std::string classPath) {
    return false;
}

bool Compiler::compileCode(std::string code) {
    std::vector<Token> tokens = lexer->generateTokens(code);
    
    std::cout << "Generated tokens: \n\n";
    
    for (const auto& token : tokens) {
        std::cout << "Token Type: " << static_cast<int>(token.type) << ", Value: " << token.value << "\n";
    }
    
    std::cout << "\n";
    
    // TODO; Check make this a class.. Perhaps inside lexer
    if(checkCodeBalance(tokens)) {
        std::cout << "No balancing problems :D\n";
    } else {
        std::cout << "Code not balanced\n";
        
        return 1;
    }
    
    ASTNode* root = parser->parseCode(tokens);
    
    std::cout << "\n\nAbstract Syntax Tree:" << "\n";
    parser->printAST(root);
    
    std::cout << "\n\n\nSemanticAnalyzer\n\n\n";
    
    semanticAnalyzer->generateSymbolTable(root);
    std::unique_ptr<IRNode> IRNode = intermediateRepresentation->generateIR(root);
    tacGenerator->generateTAC(root);
    
    for(std::string f : tacGenerator->tac)
        std::cout << f << '\n';
    
    //TACEvaluation* evaluation = new TACEvaluation();
    //evaluation->interpretTAC(tac->tac);
    
    assemblyGenerator->generateAssembly("generated_code.asm", IRNode);
    
    std::cout << "\n\n\n\n";
    
    // Print file content(for testing)
    std::ifstream f("generated_code.asm");
    
    if (f.is_open())
        std::cout << f.rdbuf();
    
    return true;
}
