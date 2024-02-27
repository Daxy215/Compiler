#include "Compiler.h"

#include <fstream>
#include <regex>

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>

#define DEBUG

namespace fs = std::experimental::filesystem;
std::vector<std::string> classesPaths;
std::vector<std::string> classesCode;

std::string Compiler::code = "";

Compiler::Compiler() {
    
}

bool Compiler::compile(std::string path) {
    for(const auto& entry : fs::directory_iterator(path)) {
        fs::path p(entry.path());
        
        std::string extention = p.extension().generic_string();
        
        std::cout << p.filename() << " ; " << extention << std::endl;
        
        if(extention._Equal(".cpp") || extention._Equal(".c") || extention._Equal(".h") || extention._Equal(".hpp"))
            classesPaths.push_back(entry.path().generic_string());
    }

    std::cout << "\n";
    
    for(const auto& entry : classesPaths) {
        constexpr auto read_size = std::size_t(4096);
        auto stream = std::ifstream(entry.data());
        stream.exceptions(std::ios_base::badbit);
        
        if (not stream) {
            throw std::ios_base::failure("file does not exist");
        }
        
        auto out = std::string();
        auto buf = std::string(read_size, '\0');
        
        while (stream.read(& buf[0], read_size)) {
            out.append(buf, 0, stream.gcount());
        }
        
        out.append(buf, 0, stream.gcount());

        if(out.size() < 10)
            continue;
        
        classesCode.push_back(out);
    }
    
    //for(auto& entry : classesCode) {
        std::string code = classesCode[0];
        code = preProcessor->proccess(code, path + "/");
        
        compileCode(code);
    //}
    
    return false;
}

bool Compiler::compileClass(std::string classPath) {
    return false;
}

bool Compiler::compileCode(std::string code) {
    // TODO; Preproccess the code.
    code = preProcessor->removeComments(code);
    
    Compiler::code = code;
    
    std::vector<Token> tokens = lexer->generateTokens(code);

#ifdef DEBUG
    std::cout << "Generated tokens: \n\n";
    
    for (const auto& token : tokens) {
        std::cout << "Token Type: " << token.type << ", Value: " << token.value << "\n";
    }
#endif
    
    // TODO; Check make this a class.. Perhaps inside lexer
    /*if(checkCodeBalance(tokens)) {
        std::cout << "No balancing problems :D\n";
    } else {
        std::cout << "Code not balanced\n";
        
        return 1;
    }*/
    
    ASTNode* root = parser->parseCode(tokens);

#ifdef DEBUG
    std::cout << "\n\nAbstract Syntax Tree:" << "\n";
    parser->printAST(root);
#endif
    
    //std::cout << "\n\nControl Flow Graph:" << "\n\n";
    //std::map<ASTNode*, CFGNode*> cfgNodes;
    
    //cfg->generateCFG(root, cfgNodes);
    //cfg->printCFG(cfgNodes[root], cfgNodes);

#ifdef DEBUG
    std::cout << "\n\n\nSemanticAnalyzer\n\n\n";
    
    semanticAnalyzer->generateSymbolTable(root);
    
    std::cout << "\n\nIR;\n\n";
#endif
    
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