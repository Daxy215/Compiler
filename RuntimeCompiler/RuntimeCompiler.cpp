#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <string>
#include <sstream>
#include <unordered_map>

// Token types
enum class TokenType {
    IDENTIFIER,
    PREPROCESSOR_DIRECTIVE,
    KEYWORD,
    LEFT_PAREN,
    RIGHT_PAREN,
    OPERATOR,
    INTEGER_LITERAL,
    FLOATING_POINT_LITERAL,
    STRING_LITERAL,
    STRUCT,
    CLASS,
    POINTER,
    REFERENCE,
    SEMICOLON,
    COMMA,
    LEFT_BRACE,
    RIGHT_BRACE,
    NAMESPACE,
    RETURN,
    COLON,
    
    UNKNOWN,
    END
};

// Token structure
struct Token {
    TokenType type;
    std::string value;
};

struct SymbolEntry {
    std::string name;
    TokenType type; // Assuming a single type for simplicity (e.g., INTEGER_LITERAL)
};

struct ClassInfo {
    std::string name;
    std::unordered_map<std::string, TokenType> members; // Map to store class members (for simplicity, only types are stored)
};

const std::unordered_set<std::string> keywords =
    {  "using", "return", "public", "private", "protected", "virtual",
    "final", "void", "const", "override", "int", "float", "char",
    "double", "long",  "new", "delete", "if", "for", "else", "while"};

std::unordered_map<std::string, SymbolEntry> symbolTable;
std::unordered_map<std::string, ClassInfo> classTable;

// Lexer functions
std::string extractNamespaceName(const std::string& input) {
    const std::string usingNamespacePrefix = "namespace ";
    const size_t prefixLength = usingNamespacePrefix.length();

    if (input.substr(0, prefixLength) == usingNamespacePrefix) {
        std::string namespaceName = input.substr(prefixLength);
        
        // Remove any leading/trailing whitespace
        namespaceName.erase(
            namespaceName.begin(),
            std::find_if(namespaceName.begin(), namespaceName.end(), [](unsigned char ch) {
                return !std::isspace(ch);
            })
        );
        namespaceName.erase(
            std::find_if(namespaceName.rbegin(), namespaceName.rend(), [](unsigned char ch) {
                return !std::isspace(ch);
            }).base(),
            namespaceName.end()
        );

        return namespaceName;
    } else {
        return "";
    }
}

std::vector<Token> lexer(const std::string& code) {
    std::vector<Token> tokens;
    std::string currentToken;
    
    Token prevToken = {};
    
    for (size_t i = 0; i < code.size(); ++i) {
        char c = code[i];
        
        if(c == ' ')
            continue;
        
        if(!tokens.empty())
            prevToken = tokens.back();
        
        if(c == '(') {
            tokens.push_back({TokenType::LEFT_PAREN, std::string(1, c)});
            continue;
        } else if(c == ')') {
            tokens.push_back({TokenType::RIGHT_PAREN, std::string(1, c)});
            continue;
        } else if(c == '{') {
            tokens.push_back({TokenType::LEFT_BRACE, std::string(1, c)});
            continue;
        } else if(c == '}') {
            tokens.push_back({TokenType::RIGHT_BRACE, std::string(1, c)});
            continue;
        } else if(c == ';') {
            tokens.push_back({TokenType::SEMICOLON, std::string(1, c)});
            continue;
        } else if(c == ',') {
            tokens.push_back({TokenType::COMMA, std::string(1, c)});
            continue;
        } else if(c == '<' && i + 1 < code.size() && code[i + 1] == '<') {
            tokens.push_back({TokenType::OPERATOR, "<<"});
            i++;
            continue;
        } else if(c == ':' && i + 1 < code.size() && code[i + 1] == ':') {
            tokens.push_back({TokenType::OPERATOR, "::"});
            i++;
            continue;
        } else if(c == '-' && i + 1 < code.size() && code[i + 1] == '>') {
            tokens.push_back({TokenType::OPERATOR, "->"});
            i++;
            continue;
        } else if(c == ':') {
            tokens.push_back({TokenType::COLON, std::string(1, c)});
            continue;
        } else if (c == '+' || c == '-' || c == '/' || c == '=') {
            tokens.push_back({TokenType::OPERATOR, std::string(1, c)});
            continue;
        } else if(c == '*' || c == '&') {
            if (prevToken.type == TokenType::OPERATOR || prevToken.type == TokenType::REFERENCE ||
                prevToken.type == TokenType::POINTER || prevToken.type == TokenType::STRUCT ||
                prevToken.type == TokenType::CLASS || prevToken.type == TokenType::IDENTIFIER) {
                
                if(c == '*')
                    tokens.push_back({TokenType::POINTER, std::string(1, c)});
                else if(c == '&')
                    tokens.push_back({TokenType::REFERENCE, std::string(1, c)});
            } else {
                tokens.push_back({TokenType::OPERATOR, std::string(1, c)});
            }
            
            continue;
        }/* else if (c == '&') {
            if (prevToken.type == TokenType::OPERATOR || prevToken.type == TokenType::REFERENCE ||
                prevToken.type == TokenType::POINTER || prevToken.type == TokenType::STRUCT ||
                prevToken.type == TokenType::CLASS || prevToken.type == TokenType::IDENTIFIER) {
                    tokens.push_back({TokenType::REFERENCE, std::string(1, c)}); // Unary reference
                } else {
                    tokens.push_back({TokenType::OPERATOR, std::string(1, c)});
                }

            continue;
        }*/ else if (std::isdigit(c) || c == '.') {
            std::string literal;
            bool hasDecimal = false;

            while (std::isdigit(code[i]) || code[i] == '.') {
                if (code[i] == '.') {
                    
                    // Check if a decimal point is already encountered
                    if (hasDecimal) {
                        // Handle error: Multiple decimal points in the number
                        // Possibly set a flag to indicate an error occurred
                        std::cout << "Error: Multiple decimal points in the number" << "\n";
                        
                        break;
                    }
                    
                    hasDecimal = true;
                }
                
                literal += code[i++];
            }
            
            // If the accumulated literal has a decimal point, treat it as a floating-point literal
            if (hasDecimal) {
                tokens.push_back({TokenType::FLOATING_POINT_LITERAL, literal});
            } else {
                tokens.push_back({TokenType::INTEGER_LITERAL, literal});
            }

            --i;
            continue;
        } else if(c == '"') { //String
            i++;
            
            std::string literal;
            
            while(code[i] != '"') {
                literal += code[i++];
            }
            
            tokens.push_back({TokenType::STRING_LITERAL, literal});
            
            continue;
        }
        
        while (i < code.size() && (std::isalnum(c) || c == '_')) {
            currentToken += c;
            
            if (i + 1 >= code.size() || !(std::isalnum(code[i + 1]) || code[i + 1] == '_')) {
                break;
            }
            
            ++i;
            c = code[i];
        }
        
        if (!currentToken.empty()) {
            TokenType type;
            
            if (keywords.find(currentToken) != keywords.end()) {
                type = TokenType::KEYWORD;
            } else if(currentToken == "include") {
                type = TokenType::PREPROCESSOR_DIRECTIVE;
                
                while (i + 1 < code.size() && code[i + 1] != '>') {
                    currentToken += code[++i];
                }
                
                if (i < code.size()) {
                    currentToken += '>'; // Include the '>'
                } else {
                    std::cerr << "Error: Unclosed include directive\n";
                    
                    return tokens;
                }
            } else if (prevToken.value == "using") {
                type = TokenType::NAMESPACE;
                
                while (i + 1 < code.size() && code[i + 1] != ';') {
                    currentToken += code[++i];
                }
                
                currentToken = extractNamespaceName(currentToken);
            } else if (currentToken == "struct") {
                type = TokenType::STRUCT;
            } else if (currentToken == "class") {
                type = TokenType::CLASS;
            } else if(currentToken == "return") {
                type = TokenType::RETURN;
            } else {
                type = TokenType::IDENTIFIER;
            }
            
            tokens.push_back({type, currentToken});
            currentToken.clear();
        }
    }
    
    if (!currentToken.empty()) {
        std::cout << "currentToken is NOT empty.\n";
    }
    
    return tokens;
}

bool checkBalance(std::vector<Token>& tokens) {
    int leftBraceCount = 0;
    int rightBraceCount = 0;
    
    for (const auto& token : tokens) {
        if (token.type == TokenType::LEFT_BRACE) {
            leftBraceCount++;
        } else if (token.type == TokenType::RIGHT_BRACE) {
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

//??????
bool performSemanticAnalysis(const std::vector<Token>& tokens) {
    ClassInfo currentClassInfo; // Temporarily store information of the current class being defined
    bool insideClassBlock = false;
    bool expectMemberName = false;
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        const Token& token = tokens[i];
        
        if (token.type == TokenType::STRUCT || token.type == TokenType::CLASS) {
            // Handle class definitions
            if (i + 1 < tokens.size() && tokens[i + 1].type == TokenType::IDENTIFIER) {
                currentClassInfo.name = tokens[i + 1].value;
                classTable[tokens[i + 1].value] = currentClassInfo;
                i++; // Skip the identifier token
                insideClassBlock = false;
            } else {
                std::cout << "Error: Missing class name after 'struct' or 'class'." << '\n';
                return false;
            }
        } else if (token.type == TokenType::LEFT_BRACE) {
            // Start of class block
            insideClassBlock = true;
            expectMemberName = false;
        } else if (insideClassBlock) {
            if (expectMemberName && token.type == TokenType::IDENTIFIER) {
                // Store member information within the current class
                currentClassInfo.members[token.value] = TokenType::UNKNOWN; // Or set appropriate member type
                expectMemberName = false;
            } else if (token.type == TokenType::KEYWORD) {
                // Handling member type declarations within the class block
                expectMemberName = true;
            } else if (token.type == TokenType::RIGHT_BRACE) {
                // End of class block
                insideClassBlock = false;
            }
        }
    }
    
    std::cout << "Semantic analysis successful." << '\n';
    
    return true;
}

enum class NodeType {
    PROGRAM,
    CLASS,
    CLASS_BODY,
    MEMBER_FUNCTION,
    MEMBER_VARIABLE,
    INITIALIZER_VARIABLE,
    PARAMETER_VARIABLE,
    FUNCTION_BODY,
    OPERATOR,
    EXPRESSION,
    INCLUDE_DIRECTIVE,
    NAMESPACE,
    VARIABLE_DECLARATION,
    LOCAL_VARIABLE_DECLARATION,
    PARAMETER,
    ASSIGNMENT,
    INHERITANCE,
    CONSTRUCTOR,
    RETURN_STATEMENT,
    BLOCK,
    IF_STATEMENT,
    ELSE_STATEMENT,
    FOR_LOOP,
    POINTER,
    REFERENCE,
    FUNCTION_CALL,
    
    // Modifiers
    ConstModifier,
    OverrideModifier,
    FinalModifier,
    ProtectedModifier,
};

struct ASTNode {
    NodeType type;
    std::string value;
    std::vector<ASTNode*> children;

    ASTNode(NodeType t, const std::string& val) : type(t), value(val) {}
    ~ASTNode() {
        for (auto child : children) {
            delete child;
        }
    }
};

class Parser {
private:
    bool isInClass;
    
    std::vector<Token> tokens;
    size_t currentTokenIndex;

public:
    Parser(const std::vector<Token>& t) : isInClass(false), tokens(t), currentTokenIndex(0) {
        
    }
    
    ASTNode* parseCode() {
        ASTNode* programAST = new ASTNode(NodeType::PROGRAM, "Program");

        while (currentTokenIndex < tokens.size() - 1) {
            ASTNode* node = nullptr;
            
            if (match(TokenType::PREPROCESSOR_DIRECTIVE)) {
                node = parseIncludeDirective();
            } else if(match(TokenType::NAMESPACE)) { //TODO; Not detecting namespace.
                node = parseNameSpace();
            } else if (match(TokenType::CLASS)) {
                isInClass = true;
                
                node = parseClassDeclaration();
            } else {
                // Try parsing a function
                node = parseMember();
                
                if(node) {
                    currentTokenIndex++;
                    //isInClass = false;*/
                }
                
                //std::cout << "didnt find; " << tokens[currentTokenIndex].type << std::endl;
                // Handle other constructs here (variable declarations, functions, etc.)
                // For instance, parseVariableDeclaration, parseFunction, etc.
                // Use similar logic to create respective AST nodes for different constructs
                // ...
                
                // Skip unknown or unsupported constructs
                //currentTokenIndex++;
            }

            if (node) {
                programAST->children.push_back(node);
            }
        }

        return programAST;
    }

private:
    //TODO; I think it needs to be - 1?
    bool match(TokenType expected) {
        return currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type == expected;
    }

    bool match(TokenType expected, std::string value) {
        return tokens[currentTokenIndex].type == expected && tokens[currentTokenIndex].value._Equal(value);
    }

    bool matchNext(TokenType expected) {
        return currentTokenIndex + 1 < tokens.size() && tokens[currentTokenIndex + 1].type == expected;
    }

    bool matchNext(TokenType expected, std::string val) {
        return currentTokenIndex + 1 < tokens.size() && tokens[currentTokenIndex + 1].type == expected && tokens[currentTokenIndex + 1].value._Equal(val);
    }
    
    Token consumeToken() {
        // Check if there are more tokens available to consume
        if (currentTokenIndex < tokens.size() - 1) {
            return tokens[currentTokenIndex++];
        }
        
        std::cout << "Trying to consume beyond available tokens" << '\n';
        
        return {};
    }
    
    ASTNode* parseIncludeDirective() {
        /*
         * It'll read:
         * #include <iostream>
         */
        
        if (match(TokenType::PREPROCESSOR_DIRECTIVE)) {
            std::string includeValue = tokens[currentTokenIndex].value;
            consumeToken(); // Consume the '#include' directive
            
            // Create an AST node representing the include directive
            ASTNode* includeNode = new ASTNode(NodeType::INCLUDE_DIRECTIVE, includeValue);
            
            // Simulate token consumption for demonstration
            //TODO; ?? wtf is this
            while (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type != TokenType::SEMICOLON) {
                currentTokenIndex++;
            }
            
            if (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type == TokenType::SEMICOLON) {
                currentTokenIndex++; // Consume the ';' token
            }
            
            return includeNode;
        }
        
        // Handle error or return nullptr for invalid or unexpected tokens
        return nullptr;
    }

    ASTNode* parseNameSpace() {
        if (match(TokenType::NAMESPACE)) {
            std::string namespaceValue = tokens[currentTokenIndex].value;
            consumeToken(); // Consume the '#include' directive
            
            // Create an AST node representing the include directive
            ASTNode* namespaceNode = new ASTNode(NodeType::NAMESPACE, "NAMESPACE: " + namespaceValue);
            
            // Simulate token consumption for demonstration
            //TODO; ??
            while (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type != TokenType::SEMICOLON) {
                currentTokenIndex++;
            }
            
            if (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type == TokenType::SEMICOLON) {
                currentTokenIndex++; // Consume the ';' token
            }
            
            return namespaceNode;
        }
        
        // Handle error or return nullptr for invalid or unexpected tokens
        return nullptr;
    }
    
    ASTNode* parseClassDeclaration() {
        if (match(TokenType::CLASS)) {
            Token t = tokens[currentTokenIndex];
            
            consumeToken(); // Consume 'class' keyword
            
            if (match(TokenType::IDENTIFIER)) {
                std::string className = tokens[currentTokenIndex].value;
                consumeToken(); // Consume class name token
                
                ASTNode* classNode = new ASTNode(NodeType::CLASS, className);
                
                Token g = tokens[currentTokenIndex];
                
                // Check for inheritance
                if (match(TokenType::COLON)) {
                    consumeToken(); // Consume ':' token
                    
                    Token h = tokens[currentTokenIndex];
                    
                    /**
                     * If it has public/private or whatever,
                     * for example something as:
                     * class Circle : public Shape
                     */
                    std::string accessSpecifier = "";
                    
                    if(match(TokenType::KEYWORD)) {
                        accessSpecifier = tokens[currentTokenIndex].value;
                        
                        consumeToken(); // Consume access specifier (public/private/protected)
                    }
                    
                    Token f = tokens[currentTokenIndex];
                    
                    // Move on normally
                    if (match(TokenType::IDENTIFIER)) {
                        std::string baseClassName = tokens[currentTokenIndex].value;
                        consumeToken(); // Consume base class name token
                        
                        // Create an AST node for base class inheritance
                        ASTNode* inheritanceNode = new ASTNode(NodeType::INHERITANCE,  "Inherits: " + accessSpecifier + " " + baseClassName);
                        classNode->children.push_back(inheritanceNode);
                    } else {
                        // Handle error: Expected an identifier after ':'
                        std::cout << "Expected an identifier after ':'" << '\n';
                    }
                }
                
                if (match(TokenType::LEFT_BRACE)) {
                    consumeToken(); // Consume '{' for class body start
                    
                    Token ts = tokens[currentTokenIndex];
                    
                    ASTNode* classBody = parseClassBody(className); // Parse the class body
                    
                    if (classBody) {
                        classNode->children.push_back(classBody);
                        
                        Token tsz = tokens[currentTokenIndex];
                        
                        if (match(TokenType::RIGHT_BRACE)) { // }
                            consumeToken(); // Consume '}' for class body end
                            
                            isInClass = false;
                            
                            return classNode;
                        }
                        
                        std::cout << "Error; seems to be missing a '}' somewhere.. Gl finding it buddy; " << currentTokenIndex << '\n';
                    }
                }
            }
        }
        
        // Handle error or return nullptr for invalid or unexpected tokens
        return nullptr;
    }
    
    /*
     * className is used to check for constructor.
     */
    ASTNode* parseClassBody(std::string className) {
        ASTNode* classBodyNode = new ASTNode(NodeType::CLASS_BODY, "ClassBody");
        
        Token t = tokens[currentTokenIndex];
        
        while (!match(TokenType::RIGHT_BRACE)) {
            std::string memberType = tokens[currentTokenIndex].value;
            
            if(memberType._Equal(className)) {
                ASTNode* constructorNode = parseConstructor();
                
                if (constructorNode) {
                    classBodyNode->children.push_back(constructorNode);
                } else {
                    // Handle error: Failed to parse constructor
                    std::cout << "Erorr happened while doing custrctor bs" << '\n';
                }
                
                //return classBodyNode;
            }
            
            ASTNode* member = parseMember(); // Parse class members (functions, variables, etc.)
            
            if (member) {
                classBodyNode->children.push_back(member);
            } else {
                //TODO;
                //classBodyNode->children.push_back(new ASTNode(NodeType::FUNCTION_BODY, "EmptyFunctionBody"));
                // Handle errors or unknown constructs within class body
                // Skip token or perform error recovery logic
                //currentTokenIndex++;
            }
        }
        
        Token tdsa = tokens[currentTokenIndex];
        
        return classBodyNode;
    }
    
    /**
     * \brief
     * Currently supports;\n
     * \n
     * Circle(double r) : radius(r) {}\n
     * Circle(double r) {}\n
     * Circle() {}\n
     * 
     * \return
     * - Constructor: Circle(double r)\n
     * - Parameter: r\n
     * - Initialization: radius(r)\n
     * \n
     * - Constructor: Circle(double r)\n
     * - Parameter: r\n
     *\n
     * - Constructor: Circle()\n
     */
    ASTNode* parseConstructor() {
        std::string constructorName = tokens[currentTokenIndex].value;
        consumeToken(); // Consume member name
        
        Token token = tokens[currentTokenIndex];
    
        consumeToken(); // Consume '('
        
        ASTNode* constructor = new ASTNode(NodeType::CONSTRUCTOR, constructorName);
        
        while(!match(TokenType::RIGHT_PAREN)) {
            Token t = tokens[currentTokenIndex];

            //Parameters:
            std::string paramType = tokens[currentTokenIndex].value;
            consumeToken();
            
            std::string paramName = tokens[currentTokenIndex].value;
            constructor->children.push_back(new ASTNode(NodeType::PARAMETER_VARIABLE, paramType + " " + paramName));
            consumeToken();
        }

        consumeToken(); // Consume ')'
        
        Token tt = tokens[currentTokenIndex];
        
        // Circle(double r) -> this : radius(r) {}
        if(match(TokenType::COLON)) {
            consumeToken(); // Consume ':'
            
            while(!match(TokenType::LEFT_BRACE)) { // {
                //radius(r), test(t)..

                // Skip comma
                if(match(TokenType::COMMA))
                    consumeToken(); // Consume ','
                
                Token t = tokens[currentTokenIndex];
                std::string paramName = tokens[currentTokenIndex].value;
                consumeToken(); // Consume variable
                consumeToken(); // Consume '('
                
                // We are at the initializer value:
                std::string paramInitNam = tokens[currentTokenIndex].value;
                consumeToken(); // Consume initializer value
                Token ttz = tokens[currentTokenIndex];
                
                constructor->children.push_back(new ASTNode(NodeType::INITIALIZER_VARIABLE,  paramName + "(" + paramInitNam + ")"));
                
                consumeToken(); // Consume ')'
            }
        }
        
        consumeToken(); // Consume '{'
        
        return constructor;
    }
    
    // Handle Variables
    //TODO; If empty, add "EmptyBody" tag.
    ASTNode* parseMember() {
        Token token = tokens[currentTokenIndex];
        
        if (match(TokenType::KEYWORD)) {
            std::string memberType = tokens[currentTokenIndex].value;
            consumeToken(); // Consume member type
            
            Token d = tokens[currentTokenIndex];
            
            if (match(TokenType::IDENTIFIER)) {
                std::string memberName = tokens[currentTokenIndex].value;
                consumeToken(); // Consume member name
                
                if (match(TokenType::LEFT_PAREN)) { // '('
                    // Parsing a member function
                    return parseMemberFunction(memberType, memberName);
                } else {
                    //TODO; global variable..
                    ASTNode* memberVariable = new ASTNode(NodeType::MEMBER_VARIABLE, memberType + " " + memberName);
                    consumeToken(); // Consume the token representing the member variable
                    return memberVariable;
                }
            }
        } else {
            consumeToken(); //ERROR happened so skip
        }

        // Handle other member types (variables, constructors, etc.) if needed

        // If no valid member found, return nullptr or handle error
        return nullptr;
    }

    ASTNode* parseVariableDeclaration() {
        if (match(TokenType::KEYWORD)) {
            std::string varType = tokens[currentTokenIndex].value;
            consumeToken(); // Consume variable type token

            if (match(TokenType::IDENTIFIER)) {
                std::string varName = tokens[currentTokenIndex].value;
                consumeToken(); // Consume variable name token

                if (match(TokenType::SEMICOLON)) {
                    consumeToken(); // Consume ';' at the end of the declaration
                    return new ASTNode(NodeType::VARIABLE_DECLARATION, varType + " " + varName);
                }
            }
        }

        // Handle syntax errors or incomplete variable declarations
        return nullptr;
    }
    
    // Function parameters
    ASTNode* parseMemberFunction(const std::string& returnType, const std::string& functionName) {
        ASTNode* memberFunction = new ASTNode(NodeType::MEMBER_FUNCTION, "Function: " + functionName);
        memberFunction->children.push_back(new ASTNode(NodeType::RETURN_STATEMENT, "ReturnType: " + returnType));
        
        // Parse function parameters
        if (match(TokenType::LEFT_PAREN)) { // '('
            consumeToken(); // Consume '('
            
            // Parse parameters (if any)
            std::vector<std::string> parameters = parseParameters();
            
            consumeToken(); // Consume ')'
            
            Token c = tokens[currentTokenIndex];
            
            //This is for;
            //virtual void draw() const = 0;
            //void draw() {}
            /*TODO;
             * Use while loop instead for;
             * void draw() const override
             */
            if(match(TokenType::KEYWORD) && (matchNext(TokenType::OPERATOR, "=") || matchNext(TokenType::RIGHT_BRACE))) { //TODO; FOR virtual void draw() const = 0;
                std::string type = tokens[currentTokenIndex].value;
                
                consumeToken(); // Consume 'const' token
                
                Token dd = tokens[currentTokenIndex];
                
                // Has a body?
                if(match(TokenType::OPERATOR, "=")) {
                    // No body
                    
                    consumeToken(); // Consume '=' token
                    
                    Token d = tokens[currentTokenIndex];
                    
                    if (match(TokenType::INTEGER_LITERAL) && tokens[currentTokenIndex].value == "0") {
                        consumeToken(); // Consume '0' token
                        consumeToken(); // Consume ';' token
                        
                        // Indicate a pure virtual function and return without attempting to parse the function body
                        //TODO; Fix this.. "type" can be more than 1..
                        memberFunction->value = returnType + " " + functionName + "() " + type + " = 0";
                        
                        //No body found:
                        memberFunction->children.push_back(new ASTNode(NodeType::FUNCTION_BODY, "EmptyFunctionBody"));
                        
                        return memberFunction;
                    }
                }
            }
            
            // Check for modifiers
            while(!match(TokenType::LEFT_BRACE)) { // '{'
                std::string type = tokens[currentTokenIndex].value;
                
                if(match(TokenType::KEYWORD, "const")) {
                    memberFunction->children.push_back(new ASTNode(NodeType::ConstModifier, type + ": true"));
                } else if(match(TokenType::KEYWORD, "override")) {
                    memberFunction->children.push_back(new ASTNode(NodeType::OverrideModifier, type + ": true"));
                } else if(match(TokenType::KEYWORD, "final")) {
                    memberFunction->children.push_back(new ASTNode(NodeType::FinalModifier, type + ": true"));
                } else if(match(TokenType::KEYWORD, "protected")) {
                    memberFunction->children.push_back(new ASTNode(NodeType::ProtectedModifier, type + ": true"));
                } else {
                    std::cout << "Unknown modifier..; " << type << '\n';
                }
                
                consumeToken(); // Consume modifier token
            }
            
            Token bb = tokens[currentTokenIndex];
            
            // Parse function body
            //TODO; DONT SKIP
            ASTNode* functionBody = parseFunctionBody();
            
            if (functionBody) {
                // Add parameters to the member function node
                for (const auto& param : parameters) {
                    ASTNode* parameterNode = new ASTNode(NodeType::PARAMETER, param);
                    memberFunction->children.push_back(parameterNode);
                }

                //Empty function
                memberFunction->children.push_back(functionBody);
            }
        }
        
        return memberFunction;
    }
    
    ASTNode* parseFunctionBody() {
        ASTNode* functionBodyNode = new ASTNode(NodeType::FUNCTION_BODY, "FunctionBody");
        
        Token t = tokens[currentTokenIndex];
        
        while (currentTokenIndex < tokens.size() && !match(TokenType::RIGHT_BRACE)) { // '}'
            Token d = tokens[currentTokenIndex];
            
            ASTNode* statement = parseStatement(); // Parse individual statements within the function body
            
            if (statement) {
                functionBodyNode->children.push_back(statement);
            } else {
                // Handle syntax errors or unknown constructs within function body
                // Skip token or perform error recovery logic
                currentTokenIndex++;
            }
        }
        
        if(match(TokenType::RIGHT_BRACE) && isInClass)
            consumeToken(); // Consume '}'
        
        return functionBodyNode;
    }
    
    // Handling operators
    ASTNode* parseStatement() {
        Token fz = tokens[currentTokenIndex];
        
        if (match(TokenType::LEFT_BRACE)) {
            consumeToken(); // Consume '{'
            
            Token t = tokens[currentTokenIndex];
            
            // Here, parse multiple statements within the block and create an AST node for the block
            ASTNode* blockNode = new ASTNode(NodeType::BLOCK, "Block");
            while (!match(TokenType::RIGHT_BRACE)) {
                Token f = tokens[currentTokenIndex];
                
                ASTNode* statement = parseStatement(); // Recursively parse individual statements
                
                if (statement) {
                    blockNode->children.push_back(statement);
                } else {
                    // Handle error or unrecognized statement
                    // Skip token or perform error recovery logic
                    currentTokenIndex++;
                }
                
                // Function ended?
                if(match(TokenType::RIGHT_BRACE)) {
                    return blockNode;
                }
            }

            // Function ended?
            if(match(TokenType::RIGHT_BRACE))
                return blockNode;
            
            Token dd = tokens[currentTokenIndex];
            
            if(currentTokenIndex < tokens.size() - 1 && match(TokenType::RIGHT_BRACE)) { // '}'
                Token dsd = tokens[currentTokenIndex];
                consumeToken(); // Consume '}' at the end of the block
            }
            
            return blockNode;
        }
        
        Token f = tokens[currentTokenIndex];
        
        if(match(TokenType::RIGHT_BRACE)) // '}'
            return new ASTNode(NodeType::FUNCTION_BODY, "EmptyFunctionBody");

        // Check if it's a function call
        ASTNode* functionCallNode = parseFunctionCall();
        if (functionCallNode) {
            return functionCallNode; // Return the function call node or handle it as needed
        }
        
        // Handle return statement:
        if((tokens[currentTokenIndex].value == "return" && match(TokenType::KEYWORD))) {
            consumeToken(); // Consume 'return'
            
            Token fzz = tokens[currentTokenIndex];
            
            ASTNode* expressionNode = expression(); // Parse expression on the right-hand side of assignment
            
            // Create an assignment node manually, using type and value accordingly
            ASTNode* assignmentNode = new ASTNode(NodeType::RETURN_STATEMENT, "RETURN STATEMENT:");
            assignmentNode->children.push_back(expressionNode); // Attach the expression as a child
            
            Token e = tokens[currentTokenIndex];
            
            return assignmentNode;
        }
        
        // Handle conditional statements (if, else if, else)
        if (match(TokenType::KEYWORD)) {
            if (tokens[currentTokenIndex].value == "if") {
                // Handle 'if' statement
                // Parse the condition, then the body of the 'if' statement
                // Create AST nodes accordingly
                // For demonstration, assume a simple 'if' statement without 'else' or 'else if'
                consumeToken(); // Consume 'if'
                
                ASTNode* conditionNode = parseCondition(); // Parse the condition expression
                ASTNode* ifBodyNode = parseStatement();    // Parse the body of 'if'
            
                // Create an 'if' statement node and attach the condition and body
                ASTNode* ifStatementNode = new ASTNode(NodeType::IF_STATEMENT, "if");
                ifStatementNode->children.push_back(conditionNode);
                ifStatementNode->children.push_back(ifBodyNode);

                return ifStatementNode;
            }
            // ... Handle 'else if' and 'else' similarly
        }
        
        // Handle loops (for, while, do-while)
        if (match(TokenType::KEYWORD)) {
            if (tokens[currentTokenIndex].value == "for") {
                // Handle 'for' loop statement
                // Parse the loop structure and body, create AST nodes accordingly
                // For demonstration, assume a simple 'for' loop without initialization, condition, or increment
                consumeToken(); // Consume 'for'

                ASTNode* forBodyNode = parseStatement(); // Parse the body of the 'for' loop

                // Create a 'for' loop node and attach the body
                ASTNode* forLoopNode = new ASTNode(NodeType::FOR_LOOP, "for");
                forLoopNode->children.push_back(forBodyNode);

                return forLoopNode;
            } else if(tokens[currentTokenIndex].value == "while") {
                
            } else if(tokens[currentTokenIndex].value == "do-while") {
                //fuck you.
            } else {
                //std::cout << "BRO WTF IS " << tokens[currentTokenIndex].value << std::endl;
            }
        }
        
        //TODO; Cant remember y i did this ;-;
        bool isVariable = false;
        
        size_t curIndex = currentTokenIndex;
        
        while(curIndex < tokens.size() && tokens[curIndex].type != TokenType::SEMICOLON) {
            Token t = tokens[curIndex];
            
            if (tokens[curIndex].type == TokenType::OPERATOR && tokens[curIndex].value == "=") {
                isVariable = true;
                std::cout << "Found variable at; " << tokens[curIndex-1].value << " - " << curIndex << '\n';
                
                break;
            }
            
            curIndex++;
        }

        if(!isVariable)
            return nullptr;
        
        // As, curIndex is '=', the previous token, should be the name.
        std::string variableName = tokens[curIndex - 1].value;
        ASTNode* variableNode = new ASTNode(NodeType::LOCAL_VARIABLE_DECLARATION, "LOCAL VARIABLE: " + variableName);
        
        // Add modifiers
        //TODO; Make this a function instead..
        //TODO; As the same code, is being used.
        //No need to check for variable name, hence the - 1.
        for(size_t i = currentTokenIndex; i < curIndex - 1; i++) { // '{'
            std::string type = tokens[i].value;
            
            //TODO; Virtual
            if(tokens[i].type == TokenType::KEYWORD && tokens[i].value == "const") {
                variableNode->children.push_back(new ASTNode(NodeType::ConstModifier, type + ": true"));
            } else if(tokens[i].type == TokenType::KEYWORD && tokens[i].value == "override") {
                variableNode->children.push_back(new ASTNode(NodeType::OverrideModifier, type + ": true"));
            } else if(tokens[i].type == TokenType::KEYWORD && tokens[i].value == "final") {
                variableNode->children.push_back(new ASTNode(NodeType::FinalModifier, type + ": true"));
            } else if(tokens[i].type == TokenType::KEYWORD && tokens[i].value == "protected") {
                variableNode->children.push_back(new ASTNode(NodeType::ProtectedModifier, type + ": true"));
            } else if(tokens[i].type == TokenType::POINTER || tokens[i].type == TokenType::REFERENCE) {
                // Check if it's a pointer/reference
                std::string pointerType = tokens[i].value;
                
                NodeType nodeType = NodeType::POINTER;
                if(tokens[i].type == TokenType::REFERENCE)
                    nodeType = NodeType::REFERENCE;
                
                ASTNode* pointerNode = new ASTNode(nodeType, "POINTER_TYPE: " + pointerType);
                pointerNode->children.push_back(new ASTNode(NodeType::POINTER, "TO: " + tokens[i-1].value));
                
                variableNode->children.push_back(pointerNode);
            } else {
                std::cout << "Unknown modifier..; " << type << '\n';
            }
            
            consumeToken(); // Consume modifier token
        }
        
        Token t = tokens[currentTokenIndex];
        
        if (match(TokenType::IDENTIFIER)) {
            consumeToken(); // Consume variable name token
            
            Token d = tokens[currentTokenIndex];
            
            if (match(TokenType::OPERATOR) && tokens[currentTokenIndex].value == "=") {
                consumeToken(); // Consume '=' token
                ASTNode* expressionNode = expression(); // Parse expression on the right-hand side of assignment
                
                variableNode->children.push_back(expressionNode);
                
                //return assignmentNode;
            }
            
            return variableNode;
        }
        
        // If no valid statement found, return nullptr or handle error
        return nullptr;
    }

    ASTNode* parseFunctionCall() {
        Token t = tokens[currentTokenIndex];
        
        if (match(TokenType::IDENTIFIER)) {
            std::string functionName = tokens[currentTokenIndex].value;
            std::string objectName = tokens[currentTokenIndex - 2].value;
            consumeToken(); // Consume function name token
            
            if (match(TokenType::LEFT_PAREN) || matchNext(TokenType::LEFT_PAREN)) {
                consumeToken(); // Consume '('
                
                Token z = tokens[currentTokenIndex];
                
                // Parse function arguments (if any)
                // You might call another function to handle this
                std::vector<std::string> parameters = parseParameters();
                
                if (match(TokenType::RIGHT_PAREN)) {
                    consumeToken(); // Consume ')'
                
                    // Create an AST node for the function call
                    ASTNode* functionCallNode = new ASTNode(NodeType::FUNCTION_CALL, "METHOD CALL: ");
                    functionCallNode->children.push_back(new ASTNode(NodeType::FUNCTION_CALL, "OBJECT: " + objectName));
                    functionCallNode->children.push_back(new ASTNode(NodeType::FUNCTION_CALL, "METHOD: " + functionName));
                    // Attach argument nodes as children to the function call node

                    for (const auto& param : parameters) {
                        ASTNode* parameterNode = new ASTNode(NodeType::PARAMETER, param);
                        functionCallNode->children.push_back(parameterNode);
                    }
                
                    return functionCallNode;
                }
            }
        }
    
        // Handle other cases or return nullptr for invalid or unexpected tokens
        return nullptr;
    }
    
    ASTNode* parseCondition() {
        std::cout << "if statement isn't there buddy\n";
        return nullptr;
    }

    std::vector<std::string> parseParameters() {
        std::vector<std::string> parameters;
        
        Token t = tokens[currentTokenIndex];
            
        while (!match(TokenType::RIGHT_PAREN)) {
            if (match(TokenType::KEYWORD)) {
                std::string paramType = tokens[currentTokenIndex].value;
                consumeToken(); // Consume parameter type token
                    
                if (match(TokenType::IDENTIFIER)) {
                    std::string paramName = tokens[currentTokenIndex].value;
                    parameters.push_back(paramType + " " + paramName);
                    consumeToken(); // Consume parameter name token
                        
                    if (!match(TokenType::COMMA)) {
                        break; // Last parameter, exit loop
                    }
                        
                    consumeToken(); // Consume ',' before the next parameter
                } else {
                    // Handle syntax errors in parameter declaration
                    //return nullptr;
                }
            } else {
                // Handle syntax errors in parameter declaration
                //return nullptr;
            }
        }

        return parameters;
    }
    
    ASTNode* expression() {
        ASTNode* left = term();
        
        while (currentTokenIndex < tokens.size() &&
               (tokens[currentTokenIndex].value == "+" || tokens[currentTokenIndex].value == "-")) {
            std::string op = tokens[currentTokenIndex++].value;
            
            ASTNode* right = term();
            ASTNode* newOpNode = new ASTNode(NodeType::OPERATOR, "OPERATOR: " + op);
            newOpNode->children.push_back(left);
            newOpNode->children.push_back(right);
            left = newOpNode;
        }
        
        return left;
    }
    
    ASTNode* term() {
        std::string assignmentType = "";
        std::vector<std::string> parameters;
        
        if(match(TokenType::KEYWORD, "new")) {
            assignmentType = "new";
            consumeToken();
        }

        
        std::string type = tokens[currentTokenIndex].value;
        ASTNode* left = new ASTNode(NodeType::ASSIGNMENT, "ASSIGNMENT: " + assignmentType + "; " + type);
        Token t = tokens[currentTokenIndex];
        
        // Points to a class, perhaps?
        if(match(TokenType::IDENTIFIER)) {
            consumeToken(); // Consume type
            
            if(!match(TokenType::LEFT_PAREN)){
                std::cout << "BRO this shit sucks.. fix it..\n";
            }
            
            consumeToken(); // Consume '('
            
            while(!match(TokenType::RIGHT_PAREN)) { // ')'
                Token tz = tokens[currentTokenIndex];
                
                if(match(TokenType::COMMA)) { //Skip
                    consumeToken(); // Consume ','
                    
                    continue;
                }
                
                parameters.push_back(tokens[currentTokenIndex].value);
                left->children.push_back(new ASTNode(NodeType::PARAMETER, "PARAMETER: " + tokens[currentTokenIndex].value));
                
                consumeToken(); // Consume parameter
            }

            return left;
        }
        
        left = factor();
        
        while (currentTokenIndex < tokens.size() &&
               (tokens[currentTokenIndex].value == "*" || tokens[currentTokenIndex].value == "/")) {
            
            std::string op = tokens[currentTokenIndex++].value;
            
            ASTNode* right = factor();
            ASTNode* newOpNode = new ASTNode(NodeType::OPERATOR, "OPERATOR: " + op);
            newOpNode->children.push_back(left);
            newOpNode->children.push_back(right);
            left = newOpNode;
        }
        
        return left;
    }
    
    ASTNode* factor() {
        Token currentToken = tokens[currentTokenIndex++];
        
        if (currentToken.type == TokenType::INTEGER_LITERAL ||
            currentToken.type == TokenType::FLOATING_POINT_LITERAL ||
            currentToken.type == TokenType::IDENTIFIER ||
            currentToken.type == TokenType::KEYWORD) {
            
            return new ASTNode(NodeType::EXPRESSION, currentToken.value); // Creating a generic expression node
        } else {
            std::cout << "huh " << currentToken.value << '\n';
            // Handle parentheses or other factors if needed
            // Not implemented in this simple example
            return nullptr;
        }
    }
};

void printAST(ASTNode* node, int depth = 0) {
    if (!node) {
        return;
    }

    for (int i = 0; i < depth; ++i) {
        std::cout << "  "; // Add indentation based on depth for better visualization
    }

    std::cout << "- " << node->value << '\n';

    for (ASTNode* child : node->children) {
        printAST(child, depth + 1); // Recursively print child nodes with increased depth
    }
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
                final const final int i = 1 + 2 * 4;
            }
            
            double area() const override {
                return 3.14159 * radius * radius;
            }
        };
        
        void test() {
            
        }
        
        int main() {
            Shape* shape = new Circle(5.0f);
            shape->draw();
            cout << "Area: " << shape->area() << endl;
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
    
    std::vector<Token> tokens = lexer(complexCode);
    
    for (const auto& token : tokens) {
        std::cout << "Token Type: " << static_cast<int>(token.type) << ", Value: " << token.value << "\n";
    }
    
    if(checkBalance(tokens)) {
        std::cout << "No balancing problems :D\n";
    } else {
        std::cout << "Fix ur code dummy\n";
    }
    
    Parser* parser = new Parser(tokens);
    ASTNode* root = parser->parseCode();
    
    std::cout << "Abstract Syntax Tree:" << "\n";
    printAST(root);
    
    return 0;
}
