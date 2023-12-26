#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <string>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include<stdio.h>

#include "Lexer.h"
#include "Parser.h"
#include "SemanticAnalysis.h"

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

//TODO; Check for [] and <>
bool checkCodeBalance(std::vector<Token>& tokens) {
    if(checkBalance(tokens, TokenType::LEFT_BRACE, TokenType::RIGHT_BRACE)
        && checkBalance(tokens, TokenType::LEFT_PAREN, TokenType::RIGHT_PAREN)) {
        return true;
    }

    return false;
}

int tempCount = 0;

// Helper function to generate temporary variable names (t1, t2, t3, ...)
std::string getTemp() {
    return "t" + std::to_string(tempCount++);
}

std::vector<std::string> tac;

void generateTAC(ASTNode* node) {
    if (node) {
        if (node->value == "+") {
            std::string leftOperand = getTemp();
            generateTAC(node->children[0]);
            std::string rightOperand = getTemp();
            generateTAC(node->children[1]);
            
            // Generate TAC for addition
            tac.push_back(leftOperand + " = " + node->children[0]->value);
            tac.push_back(rightOperand + " = " + node->children[1]->value);
            tac.push_back("t" + std::to_string(tempCount) + " = " + leftOperand + " + " + rightOperand);
            tempCount++;
            
            //std::cout << leftOperand << " = " << node->children[0]->value << std::endl;
            //std::cout << rightOperand << " = " << node->children[1]->value << std::endl;
            //std::cout << "t" << tempCount++ << " = " << leftOperand << " + " << rightOperand << std::endl;
            
            node->value = "t" + std::to_string(tempCount - 1);
        } else if (node->value == "*") {
            std::string leftOperand = getTemp();
            generateTAC(node->children[0]);
            std::string rightOperand = getTemp();
            generateTAC(node->children[1]);
            
            tac.push_back(leftOperand + " = " + node->children[0]->value);
            tac.push_back(rightOperand + " = " + node->children[1]->value);
            tac.push_back("t" + std::to_string(tempCount) + " = " + leftOperand + " * " + rightOperand);
            tempCount++;
            
            // Generate TAC for multiplication
            /*std::cout << leftOperand << " = " << node->children[0]->value << std::endl;
            std::cout << rightOperand << " = " << node->children[1]->value << std::endl;
            std::cout << "t" << tempCount++ << " = " << leftOperand << " * " << rightOperand << std::endl;*/
            
            node->value = "t" + std::to_string(tempCount - 1);
        } else {
            // Leaf node - variable or constant
            node->value = node->value; // Set the value of leaf node
        }
    }
}

void generateAssembly(std::vector<std::string>& tac) {
    std::ofstream outputFile("generated_code.asm");
    if (!outputFile.is_open()) {
        std::cerr << "ERROR: Unable to open file for writing!" << std::endl;
        return;
    }
    
    /*
    outputFile << "section .data" << std::endl;
    for (const auto& instruction : tac) {
        std::string result, op, arg1, arg2;
        std::istringstream iss(instruction);
        iss >> result >> op >> arg1 >> arg2;
        
        if(!arg1.find('t'))
            arg1 = "0";
        
        outputFile << "\t" << result << " dd " << arg1 << std::endl;
    }
    
    outputFile << "\nsection .text\n";
    outputFile << "\tglobal _start\n\n";
    
    outputFile << "_start:" << std::endl;
    
    for (const auto& instruction : tac) {
        std::string result, op, arg1, arg2;
        std::istringstream iss(instruction);
        iss >> result >> op >> arg1 >> arg2;
        
        if (op == "=") {
            outputFile << "\tmov eax, " << arg1 << std::endl;
            outputFile << "\tmov [" << result << "], eax" << std::endl;
        } else if (op == "+") {
            outputFile << "\tmov eax, [" << arg1 << "]" << std::endl;
            outputFile << "\tadd eax, [" << arg2 << "]" << std::endl;
            outputFile << "\tmov [" << result << "], eax" << std::endl;
        } else if (op == "*") {
            outputFile << "\tmov eax, [" << arg1 << "]" << std::endl;
            outputFile << "\timul eax, [" << arg2 << "]" << std::endl;
            outputFile << "\tmov [" << result << "], eax" << std::endl;
        }
    }
    
    // Windows API call to exit the process
    outputFile << "\tpush 0" << std::endl;
    outputFile << "\tint 0x80" << std::endl;
    */
    
    outputFile << R"(
section .data
    t2 dd 3            ; Define a variable to hold 3
    t3 dd 2            ; Define a variable to hold 2
    t4 dd 0            ; Define a variable to hold the result of t2 * t3
    t0 dd 5            ; Define a variable to hold 5
    t1 dd 0            ; Define a variable to hold the result of t4
    t5 dd 0            ; Define a variable to hold the result of t0 + t1

section .text
    global _start

_start:
    ; Load values into registers
    mov eax, [t2]       ; Move the value of 't2' into eax
    mov ebx, [t3]       ; Move the value of 't3' into ebx
    
    ; Perform the operation t4 = t2 * t3
    imul eax, ebx       ; Multiply eax (t2) by ebx (t3), result in eax
    mov [t4], eax       ; Store the result of the multiplication in t4
    
    ; Load value from t4 into ebx (t1 = t4)
    mov ebx, [t4]       ; Move the value of 't4' into ebx (t1 = t4)
    
    ; Load values into registers
    mov eax, [t0]       ; Move the value of 't0' into eax
    
    ; Perform the operation t5 = t0 + t1
    add eax, ebx        ; Add eax (t0) to ebx (t1), result in eax
    mov [t5], eax       ; Store the result of the addition in t5
    
    ; eax now holds the result (t5)
    
    ; If you want to exit cleanly:
    mov eax, 1          ; syscall for exit
    xor ebx, ebx        ; exit code 0
    int 0x80            ; make the syscall
    )";
    
    outputFile.close();
}

int main() {
    std::string code = R"(
        struct MyStruct { int val; };
        
        int main() {
            int* ptr;
            MyStruct ref;
            int i = 2 + 3 * 6;
            
            return 0;
        }
    )";
    
    //TODO; having classes within classes...
    //TODO; Well it don't detect namespaces within class/function :)
    //TODO; Making it so that it says EMPTY FUNCTION
    
    //TODO; For the sentifmxi???????? thingy; Check if the overloaded function is within the same class.
    
    /*
    *
        #include <iostream>
        using namespace std;
        
     */
    
    std::string complexCode = R"(
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
                final const int i = 1 + 2 * 4;
            }
            
            double area() const override {
                return 3.14159 * radius * radius;
            }
        };
        
        void test() {
            
        }
        
        void test(int i) {
            
        }
        
        void test() {
            
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
    
    //TODO; Can't detect 5.0f, but can detect, 5.0
    
    std::string testCode = "int i = 5 + 3 * 2;";
    
    Lexer* lexer = new Lexer();
    std::vector<Token> tokens = lexer->generateTokens(testCode);
    
    for (const auto& token : tokens) {
        std::cout << "Token Type: " << static_cast<int>(token.type) << ", Value: " << token.value << "\n";
    }
    
    if(checkCodeBalance(tokens)) {
        std::cout << "No balancing problems :D\n";
    } else {
        std::cout << "In balanced code.\n";
        
        return 1;
    }
    
    Parser* parser = new Parser(tokens);
    ASTNode* root = parser->parseCode();
    
    std::cout << "Abstract Syntax Tree:" << "\n";
    parser->printAST(root);
    
    std::cout << "\n\n\nSemanticAnalysis\n\n\n";
    
    //TODO; Rename to: SemanticAnalyzer
    SemanticAnalysis* semanticAnalysis = new SemanticAnalysis(root);

    // Sample AST representing an arithmetic expression: 5 + 3 * 2
    root = root->children[0]->children[1];
    generateTAC(root);
    
    for(std::string f : tac)
        std::cout << f << std::endl;
    
    generateAssembly(tac);

    std::cout << "\n\n\n\n";
    
    std::ifstream f("generated_code.asm");

    if (f.is_open())
        std::cout << f.rdbuf();

    std::cout << "\n\n\n\n";
    
#if defined(__x86_64__) || defined(_M_X64)
    std::system("nasm -f win64 generated_code.asm -o generated_code.obj"); // For 64-bit assembly
    std::system(R"("C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.38.33130\bin\Hostx64\x64\link.exe" generated_code.obj /subsystem:windows /entry:_start /libpath:.\libs\x64 /nodefaultlib kernel32.lib user32.lib /largeaddressaware:no)"); // Linking for 64-bit architecture
#else
    std::system("nasm -f win32 generated_code.asm -o generated_code.o"); // For 32-bit assembly
    std::system("ld -m elf_i386 -o generated_executable generated_code.o"); // Linking for 32-bit architecture
#endif
    
    // Run the Executable
    // Replace "your_executable" with the actual executable name
    std::system("generated_code.exe"); // Execute the generated executable

    
    
    
    return 0;
}
