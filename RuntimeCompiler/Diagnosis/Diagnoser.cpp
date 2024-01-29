#include "Diagnoser.h"

#include <iostream>

#include "../Compiler.h"
#include "../Lexer/Lexer.h"

std::string repeat(const std::string& str, int times) {
    std::string result;
    
    for(int i = 0; i < times; ++i) {
        result += str;
    }
    
    return result;
}

void Diagnoser::logError(std::string errorMessage, Token token) {
    int prefix = token.startPos - 8;
    std::string line = Compiler::code.substr(prefix, token.endPos - prefix);
    
    line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
    
    std::cerr << "Error at line " << token.line << ", column " << token.column << ": " << errorMessage << "\n";
    std::cerr << repeat(" ", min(token.startPos, 18)) << line << "\n";
    std::cerr << repeat(" ", min(token.startPos, 18)) << "^" << "\n";
    std::cerr << repeat(" ", min(token.startPos, 18)) << "|" << "\n";
    std::cerr << repeat(" ", min(token.startPos, 18)) << "+-- " << errorMessage << " (" << token.line << ":" << token.column << ")\n";
    
    std::cout << "\n";

    exit(-1);
}
