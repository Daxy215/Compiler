#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <string>
#include <sstream>
#include <unordered_map>

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

    //TODO; For the sentifmxi thingy; Check if the overloaded function is within the same class.
    
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
                final const final int i = 1 + 2 * 4;
            }
            
            double area() const override {
                return 3.14159 * radius * radius;
            }
        };
        
        void test() {
            
        }

        void test() {
            
        }
        
        int main() {
            Shape* shape = new Circle(5.0f);
            shape->draw();
            delete shape;
            
            return 0;
        }
    )";
    
    std::string input = "3.145 + 4";
    
    std::string test = R"(
        int main() {
            Shape* shape = new Circle(5.0, 4, "test");
            shape->draw();
            cout << "Area: " << shape->area() << endl;
            delete shape;
            
            return 0;
        }
    )";

    std::string methodCallTest = R"(
        shape->draw();
    )";
    
    std::string namespaceTest = R"(
        using namespace std;
    )";
    
    //TODO; Can't detect 5.0f, but can detect, 5.0
    
    Lexer* lexer = new Lexer();
    std::vector<Token> tokens = lexer->generateTokens(complexCode);
    
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
    
    std::cout << "\n\n\n\nSemanticAnalysis\n\n\n";
    
    SemanticAnalysis semanticAnalysis;
    semanticAnalysis.traverseNodes(root, "GLOBAL");
    semanticAnalysis.printSymbolTable();
    
    return 0;
}
