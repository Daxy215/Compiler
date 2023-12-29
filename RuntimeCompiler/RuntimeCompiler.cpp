#include <array>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <string>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <stdio.h>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <iostream>

#include "AssemblyGenerator.h"
#include "Lexer.h"
#include "Parser.h"
#include "SemanticAnalysis.h"
#include "TACGenerator.h"

/* CODE BALANCING CHECK */
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
    if(checkBalance(tokens, TokenType::LEFT_BRACE, TokenType::RIGHT_BRACE)
        && checkBalance(tokens, TokenType::LEFT_PAREN, TokenType::RIGHT_PAREN)
        && checkBalance(tokens, TokenType::LEFT_SQUARE_BRACE, TokenType::RIGHT_SQUARE_BRACE)
        && checkBalance(tokens, TokenType::LEFT_ANGLE_BRACKET, TokenType::RIGHT_ANGLE_BRACKET)) {
        return true;
    }
    
    return false;
}

std::string execCommand(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(_popen(cmd, "r"), _pclose);
    if (!pipe) {
        throw std::runtime_error("_popen() failed!");
    }
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr) {
            result += buffer.data();
        }
    }
    return result;
}

int main() {
    //TODO; having classes within classes...
    //TODO; Well it don't detect namespaces within class/function :)
    //TODO; Making it so that it says EMPTY FUNCTION
    //TODO; Can't detect 5.0f, but can detect, 5.0
    
    //TODO; For the sentifmxi???????? thingy; Check if the overloaded function is within the same class.
    
    std::string complexCode = R"(
        #include <iostream>
        using namespace std;
        
        class Shape {
        public:
            virtual void draw() const = 0;
            virtual double area() const = 0;
        };
        
        class Circle : public Shape {
        private:
            double radius;
        
        public:
            Circle(double r) : radius(r) {}
            
            void draw() const override {
                const int i = 1 + 2 * 4;
            }
            
            double area() const override {
                return 3.14159 * radius * radius;
            }
        };
        
        void test() {
            
        }
        
        void test(int i) {
            
        }
        
        int main() {
            int i = 0;
            int i = 0;
            Shape* shape = new Circle(5.0);
            shape->draw();
            delete shape;
            
            return 0;
        }
    )";
    
    std::string testCode = "int i = 5 + 3 * 2;";

    std::string test1 = R"(
        class Test {
            public:
            int test = 4;
        };
    )";
    
    std::string test2 = R"(
        void foo(int a, int b) {
            return a + b;
        }
    )";
    
    std::string test3 = R"(
        void test() {
            for(int i = 0; i < 10; i++) {
                if(i > 5) {
                    break;
                }
            }
        }
    )";
    
    Lexer* lexer = new Lexer();
    std::vector<Token> tokens = lexer->generateTokens(test3);

    std::cout << "Generated tokens: \n\n";
    
    for (const auto& token : tokens) {
        std::cout << "Token Type: " << static_cast<int>(token.type) << ", Value: " << token.value << "\n";
    }

    std::cout << "\n";
    
    // TODO; Check make this a class.. Perhaps inside lexer
    if(checkCodeBalance(tokens)) {
        std::cout << "No balancing problems :D\n";
    } else {
        std::cout << "In balanced code.\n";
        
        return 1;
    }
    
    Parser* parser = new Parser(tokens);
    ASTNode* root = parser->parseCode();
    
    std::cout << "\n\nAbstract Syntax Tree:" << "\n";
    parser->printAST(root);
    
    std::cout << "\n\n\nSemanticAnalysis\n\n\n";
    
    //TODO; Rename to: SemanticAnalyzer
    SemanticAnalysis* semanticAnalysis = new SemanticAnalysis();
    semanticAnalysis->generateSymbolTable(root);

    TACGenerator* tac = new TACGenerator();
    int c = 0;
    tac->generateTAC(root,c);

    for(std::string f : tac->tac)
        std::cout << f << std::endl;

    return 0;
    
    // Sample AST representing an arithmetic expression: 5 + 3 * 2
    
    AssemblyGenerator* assemblyGenerator = new AssemblyGenerator();
    assemblyGenerator->generateAssembly("generated_code.asm", tac->tac);
    
    std::cout << "\n\n\n\n";
    
    // Print file content(for testing)
    std::ifstream f("generated_code.asm");
    
    if (f.is_open())
        std::cout << f.rdbuf();
    
    std::cout << "\n\n\n\n";
    
#if defined(__x86_64__) || defined(_M_X64)
    std::system("nasm -f win64 generated_code.asm -o generated_code.obj"); // For 64-bit assembly
    std::system(R"("C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.38.33130\bin\Hostx64\x64\link.exe" generated_code.obj /subsystem:windows /entry:_start /libpath:./libs/x64 /nodefaultlib kernel32.lib user32.lib /largeaddressaware:no)"); // Linking for 64-bit architecture
#else
    std::system("nasm -f win32 generated_code.asm -o generated_code.o"); // For 32-bit assembly
    std::system("ld -m elf_i386 -o generated_executable generated_code.o"); // Linking for 32-bit architecture
#endif
    
    // Run the Executable
    std::string output = execCommand("generated_code.exe");
    std::cout << "Output:\n" << output << std::endl;
    
    return 0;
}
