#include <array>
#include <iostream>
#include <string>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <iostream>

#include <string>
#include <iostream>
#include <list>
#include <sstream>
#include <type_traits>
#include <iomanip>
#include <cctype>

#include "AssemblyGenerator.h"
#include "Lexer.h"
#include "Parser.h"
#include "SemanticAnalyzer.h"
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
    bool braces = checkBalance(tokens, TokenType::LEFT_BRACE, TokenType::RIGHT_BRACE);
    bool parens = checkBalance(tokens, TokenType::LEFT_PAREN, TokenType::RIGHT_PAREN);
    bool squareBraces = checkBalance(tokens, TokenType::LEFT_SQUARE_BRACE, TokenType::RIGHT_SQUARE_BRACE);
    //bool angleBrackets = checkBalance(tokens, TokenType::LEFT_ANGLE_BRACE, TokenType::RIGHT_ANGLE_BRACE);
    
    if(braces && parens && squareBraces/* && angleBrackets*/) {
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

/* Needs c++14 or later. */
template <typename T = int> requires std::is_arithmetic_v<T>
class Calculator {
public:
    Calculator(const std::string& expression);
    T exp();
    
private:
    //std::list<std::string> mTokens;
    std::string mCurrent;
    
    void next();
    T bshift();
    T band();
    T bxor();
    T bor();
    T plus();
    T term();
    T factor();
    T toNum(const std::string& s) const;
};

template<typename T = int> requires std::is_arithmetic_v<T>
T calculate(const std::string& s) {
    return Calculator<T> { s }.exp();
}

int main() {
    //TODO; having classes within classes...
    //TODO; Well it don't detect namespaces within class/function :)
    //TODO; Making it so that it says EMPTY FUNCTION
    //TODO; Expression doesn't work for '++i'
    //TODO; No support for structs, enums.
    
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
        
        bool test(int i) {
            if(i > 5 && i < 10 || (i > 5 && i < 10 || (i == 100))) {
                return true;  
            }  else if(i < 5) {
                return false;
            } else {
                return true;
            }
        }
        
        int main() {
            float i = 5.0f;
            Shape* shape = new Circle(i);
            shape->draw(5.0);
            delete shape;
            
            return 0;
            
            while(i < 10 && i > 10) {
                
            }
            
            do {
                
            } while(i > 10);
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
        int getSum() {
            int sum = 0;
            for(int i = 0; i < 10; i++) {
                if(i > 5) {
                    break;
                }
                
                sum += i;
            }
            
            return sum;
        }
    )";
    
    std::string test4 = R"(
        int main() {
            if(x < 5) {
                for (int i = 0; i < x; i++) {
                    
                }
            } else {
                for (int i = 0; i < x; i++) {
                    
                }
            }
        }
    )";
    
    std::string test5 = R"(
        int main() {
            for (int i = 0; i < x; i++) {
                
            }
        }
    )";
    
    std::string test6 = R"(
        T square(T num) {
            return num * num * num + num / num - num;
        }
    )";
    
    std::string test7 = R"(
           class TestClass {
               int x;
               public:
               TestClass(int val): x(val) {}
               void printValue() {
                   if (x > 0) {
                       for (int i = 0; i < x; i++) {
                          std::cout << "Test value: " << i << std::endl;
                       }
                   } else {
                       std::cout << "Invalid value" << std::endl;
                   }
               }
           }
    )";
    
    std::string test8 = R"(
           class TestClass {
               int x;
               public:
               TestClass(int val): x(val) {}
               void printValue() {
                   if (x > 0) {
                       for (int i = 0; i < x; i++) {
                          std::cout << "Test value: " << i << std::endl;
                       }
                   } else {
                       std::cout << "Invalid value" << std::endl;
                   }
               }
           };
           
           T square(T num) {
               return num * num * num;
           }
    )";
    
    std::string test9 = R"(
        int main() {
            pascalTriangle[i].resize(i + 1);
        }
    )";
    
    std::string test10 = R"(
        int main() {
            pascalTriangle[i][0] = pascalTriangle[i][i] = 1;
        }
    )";
    
    std::string test11 = R"(
            #include <iostream>
            #include <string>
            #include <vector>
            
            void GeneratePascalTriangle(int n) {
               std::vector<std::vector<int>> pascalTriangle(n);
               for (int i = 0; i < n; i++) {
                   pascalTriangle[i].resize(i + 1);
                   pascalTriangle[i][0] = pascalTriangle[i][i] = 1;
                   for (int j = 1; j < i; j++) {
                       pascalTriangle[i][j] = pascalTriangle[i - 1][j - 1] + pascalTriangle[i - 1][j];
                   }
               }
               
               for (int i = 0; i < n; i++) {
                   for (int j = 0; j <= i; j++) {
                        
                   }
               }
            }
            
            int main() {
               GeneratePascalTriangle(10);
               return 0;
            }
    )";

    std::string test12 = R"(
        int main() {
            Run(v + " " + boost::lexical_cast<std::string>(start), tgt - start, start + 1);
        }
    )";

    std::string test13 = R"(
        #include <iostream>
        #include <string>
        #include <boost/lexical_cast.hpp>
        
        void Run(const std::string& v, int tgt, int start) {
           for(; tgt >= 2 * start + 1; start++)
               Run(v + " " + boost::lexical_cast<std::string>(start), tgt - start, start + 1);
           std::cout << v << ' ' << tgt << std::endl;
        }
    
        int main() {
           Run(std::string(), 10, 1);
           getchar();
        }
    )";
    
    Lexer* lexer = new Lexer();
    std::vector<Token> tokens = lexer->generateTokens(complexCode);
    
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
    
    Parser* parser = new Parser(tokens);
    ASTNode* root = parser->parseCode();
    
    std::cout << "\n\nAbstract Syntax Tree:" << "\n";
    parser->printAST(root);
    
    std::cout << "\n\n\nSemanticAnalyzer\n\n\n";
    
    SemanticAnalyzer* semanticAnalyzer = new SemanticAnalyzer();
    semanticAnalyzer->generateSymbolTable(root);
    
    TACGenerator* tac = new TACGenerator();
    int c = 0;
    tac->generateTAC(root,c);
    
    for(std::string f : tac->tac)
        std::cout << f << '\n';
    
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
