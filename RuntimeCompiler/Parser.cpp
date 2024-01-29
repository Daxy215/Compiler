#include "Parser.h"

#include <map>

#include "Diagnosis/Diagnoser.h"

std::map<std::string, NodeType> modifiersTypes =
    {
        // Normal modifiers
        {"const", NodeType::ConstModifier},
        {"final", NodeType::FinalModifier},
        {"protected", NodeType::ProtectedModifier},
        {"virtual", NodeType::VirtualModifier},
        {"override", NodeType::OverrideModifier},
        
        // Variable modifiers
        {"short", NodeType::ShortModifier},
    };

ASTNode* Parser::parseCode(const std::vector<Token>& t) {
    this->tokens = t;
    
    ASTNode* programAST = new ASTNode(NodeType::PROGRAM, "Program");
    
    while (currentTokenIndex < tokens.size() - 1) {
        ASTNode* node = nullptr;
        
        if(match(LexerNameSpace::TokenType::SEMICOLON))
            consumeToken();
        
        Token toekziao = tokens[currentTokenIndex];
        
        if (match(LexerNameSpace::TokenType::PREPROCESSOR_DIRECTIVE)) {
            node = parseIncludeDirective();
        } else if(match(LexerNameSpace::TokenType::NAMESPACE)) { //TODO; Not detecting namespace.
            node = parseNameSpace();
        } else if(match(LexerNameSpace::TokenType::ENUM)) {
            node = parseEnumDeclaration();
        } else if(match(LexerNameSpace::TokenType::STRUCT)) {
            node = parseStructDeclaration();
        } else if (match(LexerNameSpace::TokenType::CLASS)) {
            isInClass = true;
            
            node = parseClassDeclaration();
        } else {
            // Try parsing a function
            node = parseMember(NodeType::MEMBER_VARIABLE);
            
            node->setIsGlobal(node, true);
            
            if(node) {
                currentTokenIndex++;
            }
        }
        
        if (node) {
            programAST->children.push_back(node);
        }
    }
    
    return programAST;
}

ASTNode* Parser::parseIncludeDirective() {
    if (match(LexerNameSpace::TokenType::PREPROCESSOR_DIRECTIVE)) {
        std::string includeValue = tokens[currentTokenIndex].value, includeName;
        consumeToken(); // Consume the '#include' directive
        
        size_t startPos = includeValue.find_first_of("<");
        size_t endPos = includeValue.find_first_of(">");
        
        if (startPos != std::string::npos && endPos != std::string::npos) {
            includeName = includeValue.substr(startPos + 1, endPos - startPos - 1);
        }
        
        ASTNode* includeNode = new ASTNode(NodeType::INCLUDE_DIRECTIVE, includeValue);
        includeNode->addChild(NodeType::INCLUDE_DIRECTIVE, includeName);
        
        return includeNode;
    }
    
    return nullptr;
}

ASTNode* Parser::parseNameSpace() {
    if (match(LexerNameSpace::TokenType::NAMESPACE)) {
        std::string namespaceValue = tokens[currentTokenIndex].value;
        consumeToken(); // Consume the '#include' directive
        
        ASTNode* namespaceNode = new ASTNode(NodeType::NAMESPACE, namespaceValue);
        
        return namespaceNode;
    }
    
    return nullptr;
}

ASTNode* Parser::parseEnumDeclaration() {
    if(match(LexerNameSpace::ENUM)) {
        consumeToken(); // Consume 'enum'
        
        std::string enumName = tokens[currentTokenIndex].value;
        ASTNode* enumNode = new ASTNode(NodeType::ENUM, enumName);
        consumeToken(); // Consume enum name
        
        Token t = tokens[currentTokenIndex];
        
        if(!match(LexerNameSpace::LEFT_BRACE)) { // '{'
            Diagnoser::logError("Missing semicolon after enum", tokens[currentTokenIndex - 1]);
        }
        
        consumeToken(); // Consume '{'
        
        while(!match(LexerNameSpace::RIGHT_BRACE)) { // '}'
            Token zt = tokens[currentTokenIndex];
            Token ztz = tokens[currentTokenIndex+1];
            
            if(match(LexerNameSpace::COMMA)) {
                consumeToken(); // Consume ',' 
                continue;
            } else if (match(LexerNameSpace::IDENTIFIER)
                && !matchNext(LexerNameSpace::COMMA) && !matchNext(LexerNameSpace::RIGHT_BRACE)) {
                Diagnoser::logError("Missing a comma before", tokens[currentTokenIndex ]);
            }
            
            ASTNode* enumValueNode = new ASTNode(NodeType::VALUE, tokens[currentTokenIndex].value);
            enumNode->addChild(enumValueNode);
            
            consumeToken();
        }
        
        if(!match(LexerNameSpace::RIGHT_BRACE)) { // '{'
            Diagnoser::logError("Missing semicolon after enum", tokens[currentTokenIndex - 2]);
        }
        
        consumeToken(); // Consume '}'
        
        return enumNode;
    }
    
    return nullptr;
}

ASTNode* Parser::parseStructDeclaration() {
    return nullptr;
}

ASTNode* Parser::parseClassDeclaration() {
    if (match(LexerNameSpace::TokenType::CLASS)) {
        Token t = tokens[currentTokenIndex];
        
        consumeToken(); // Consume 'class' keyword
        
        if (match(LexerNameSpace::TokenType::IDENTIFIER)) {
            std::string className = tokens[currentTokenIndex].value;
            consumeToken(); // Consume class name token
            
            this->className = className;
            
            ASTNode* classNode = new ASTNode(NodeType::CLASS, className);
            
            Token g = tokens[currentTokenIndex];
            
            // Check for inheritance
            if (match(LexerNameSpace::TokenType::COLON)) {
                consumeToken(); // Consume ':' token
                
                Token h = tokens[currentTokenIndex];
                
                /**
                 * If it has public/private or whatever,
                 * for example something as:
                 * class Circle : public Shape
                 */
                std::string accessSpecifier = "";
                
                if (match(LexerNameSpace::TokenType::KEYWORD)) {
                    accessSpecifier = tokens[currentTokenIndex].value;
                    
                    consumeToken(); // Consume access specifier (public/private/protected)
                }
                
                Token f = tokens[currentTokenIndex];
                
                // Move on normally
                if (match(LexerNameSpace::TokenType::IDENTIFIER)) {
                    std::string baseClassName = tokens[currentTokenIndex].value;
                    consumeToken(); // Consume base class name token
                    
                    // Create an AST node for base class inheritance
                    ASTNode* inheritanceNode = new ASTNode(NodeType::INHERITANCE,
                                                           "Inherits: " + accessSpecifier + " " + baseClassName);
                    classNode->children.push_back(inheritanceNode);
                } else {
                    // Handle error: Expected an identifier after ':'
                    std::cout << "Expected an identifier after ':'" << '\n';
                }
            }
            
            if (match(LexerNameSpace::TokenType::LEFT_BRACE)) {
                consumeToken(); // Consume '{' for class body start
                
                Token ts = tokens[currentTokenIndex];
                
                ASTNode* classBody = parseClassBody(className); // Parse the class body
                
                if (classBody) {
                    classNode->children.push_back(classBody);
                    
                    Token tsz = tokens[currentTokenIndex];
                    
                    if (match(LexerNameSpace::TokenType::RIGHT_BRACE)) { // }
                        consumeToken(); // Consume '}' for class body end
                        
                        if(match(LexerNameSpace::TokenType::SEMICOLON)) // ';'
                            consumeToken(); // Consume ';'
                        
                        isInClass = false;
                        
                        return classNode;
                    }
                    
                    std::cout << "Error; seems to be missing a '}' somewhere.. Gl finding it buddy; " <<
                        currentTokenIndex << '\n';
                }
            }
        }
    }
    
    return nullptr;
}

ASTNode* Parser::parseClassBody(std::string className) {
    ASTNode* classBodyNode = new ASTNode(NodeType::CLASS_BODY, "ClassBody");
    
    Token t = tokens[currentTokenIndex];
    
    while (currentTokenIndex < tokens.size() && !match(LexerNameSpace::TokenType::RIGHT_BRACE)) {
        if(match(LexerNameSpace::TokenType::SEMICOLON))
            consumeToken(); // Consume ';'
        
        if(match(LexerNameSpace::TokenType::KEYWORD) && (tokens[currentTokenIndex].value == "public" || tokens[currentTokenIndex].value == "private") && matchNext(LexerNameSpace::TokenType::COLON)) {
            consumeToken(); // Consume ?
            consumeToken(); // Consume ':'
        }
        
        std::string memberType = tokens[currentTokenIndex].value;
        
        if(memberType._Equal(className)) {
            ASTNode* constructorNode = parseConstructor();
            
            if (constructorNode) {
                classBodyNode->children.push_back(constructorNode);
                
                if(match(LexerNameSpace::TokenType::RIGHT_BRACE))
                    consumeToken(); // Consume '}'
            } else {
                // Handle error: Failed to parse constructor
                std::cout << "Error; happened while doing constructor bs" << '\n';
                consumeToken();
            }
        }
        
        // HERE !!!!!!!!!!!!!!!!
        ASTNode* member = parseMember(NodeType::MEMBER_VARIABLE); // Parse class members (functions, variables, etc.)
        
        if (member) {
            classBodyNode->children.push_back(member);
        } else {
            //TODO;
            //classBodyNode->children.push_back(new ASTNode(NodeType::FUNCTION_BODY, "EmptyFunctionBody"));
            // Handle errors or unknown constructs within class body
            // Skip token or perform error recovery logic
            //currentTokenIndex++;
        }
        
        Token endofclassig = tokens[currentTokenIndex];
    }
    
    Token tdsa = tokens[currentTokenIndex];
    
    return classBodyNode;
}

ASTNode* Parser::parseConstructor() {
    std::string constructorName = tokens[currentTokenIndex].value;
    consumeToken(); // Consume member name
    
    Token token = tokens[currentTokenIndex];
    
    consumeToken(); // Consume '('
    
    ASTNode* constructor = new ASTNode(NodeType::CONSTRUCTOR, constructorName);
    
    while (!match(LexerNameSpace::TokenType::RIGHT_PAREN)) {
        Token t = tokens[currentTokenIndex];
         
        // Parameters handling
        std::string paramType = tokens[currentTokenIndex].value;
        consumeToken();
        
        std::string paramName = tokens[currentTokenIndex].value;
        constructor->children.push_back(new ASTNode(NodeType::PARAMETER_VARIABLE, paramType + " " + paramName));
        consumeToken();
    }
    
    consumeToken(); // Consume ')'
    
    Token tt = tokens[currentTokenIndex];
    
    // Circle(double r) -> this : radius(r) {}
    if (match(LexerNameSpace::TokenType::COLON)) {
        consumeToken(); // Consume ':'
        
        while (!match(LexerNameSpace::TokenType::LEFT_BRACE)) {
            // {
            //: radius(r), test(t)..
            
            // Skip comma
            if (match(LexerNameSpace::TokenType::COMMA))
                consumeToken(); // Consume ','
            
            Token t = tokens[currentTokenIndex];
            std::string paramName = tokens[currentTokenIndex].value;
            consumeToken(); // Consume variable
            consumeToken(); // Consume '('
            
            // We are at the initializer value:
            std::string paramInitNam = tokens[currentTokenIndex].value;
            consumeToken(); // Consume initializer value
            Token ttz = tokens[currentTokenIndex];
            
            constructor->children.push_back(new ASTNode(NodeType::INITIALIZER_VARIABLE,
                                                        paramName + "(" + paramInitNam + ")"));
            
            consumeToken(); // Consume ')'
        }
    }
    
    consumeToken(); // Consume '{'
    
    return constructor;
}

ASTNode* Parser::parseMember(NodeType memberDeclarationType) {
    Token token = tokens[currentTokenIndex];
    
    if(match(LexerNameSpace::TokenType::INTEGER_LITERAL) || match(LexerNameSpace::TokenType::FLOATING_POINT_LITERAL) || match(LexerNameSpace::TokenType::STRING_LITERAL)) {
        consumeToken();
        return new ASTNode(NodeType::VALUE, tokens[currentTokenIndex - 1].value);
    }
    
    ASTNode* namespaceUsageNode = parseNamespacesUsage(0);
    if(namespaceUsageNode)
        return namespaceUsageNode;
    
    // Handle it here..
    //pascalTriangle[i].resize(i + 1);
    ASTNode* functionCall = parseFunctionCall();
    if(functionCall)
        return functionCall;
    
    ASTNode* modifiers = new ASTNode(NodeType::MODIFIERS, "Modifiers; ");
    
    while(match(LexerNameSpace::TokenType::KEYWORD)) {
        std::string type = tokens[currentTokenIndex].value;
        
        // Does type exist
        if(modifiersTypes.count(type)) {
            modifiers->children.push_back(new ASTNode(modifiersTypes[type], type));
        }
        
        consumeToken(); // Consume modifier
    }
    
    std::string memberType;
    
    // Ik junky code shush
    if(token.type == LexerNameSpace::TokenType::IDENTIFIER) {
        memberType = tokens[currentTokenIndex - 1].value;
        //consumeToken();
    } else {
        if(currentTokenIndex == 0)
            memberType = tokens[currentTokenIndex].value;
        else
            memberType = tokens[currentTokenIndex - 1].value;
    }
    
    /*if(match(LexerNameSpace::TokenType::POINTER) || match(LexerNameSpace::TokenType::REFERENCE)) {
        // Check if it's a pointer/reference
        std::string pointerType = tokens[currentTokenIndex].value;
        
        NodeType nodeType = NodeType::POINTER;
        if(tokens[currentTokenIndex].type == LexerNameSpace::TokenType::REFERENCE)
            nodeType = NodeType::REFERENCE;
        
        consumeToken(); // Consume reference
        
        ASTNode* pointerNode = new ASTNode(nodeType, "POINTER_TYPE: " + pointerType);
        pointerNode->children.push_back(new ASTNode(NodeType::POINTER, "TO: " + tokens[currentTokenIndex - 1].value));
        
        parseReference();
        
        modifiers->children.push_back(pointerNode);
    }*/
    
    Token t = tokens[currentTokenIndex];
    
    // Function
    if(matchNext(LexerNameSpace::TokenType::LEFT_PAREN)) {
        std::string memberName = tokens[currentTokenIndex].value;
        consumeToken(); // Consume member
        
        ASTNode* functionNode = parseFunction(memberType, memberName);
        
        if(!isInClass)
            functionNode->isGlobal = true;
        
        return functionNode;
    }
    
    // Perhaps a variable usage?
    //TODO; So this is the current problem.. Cant even remember what this was :)
    /*if(match(LexerNameSpace::TokenType::IDENTIFIER) && (!matchNext(LexerNameSpace::TokenType::KEYWORD) && !matchNext(LexerNameSpace::TokenType::REFERENCE) && !matchNext(LexerNameSpace::TokenType::POINTER))) {
        ASTNode* variableNode = parseVariable(memberDeclarationType, "None");
        variableNode->children.push_back(modifiers);
        
        return variableNode;
    }*/
    
    Token z = tokens[currentTokenIndex];
    
    if (match(LexerNameSpace::TokenType::REFERENCE) || match(LexerNameSpace::TokenType::POINTER)) {
        //consumeToken(); // Consume reference
        
        ASTNode* variableNode = parseReference(memberDeclarationType, memberType);
        
        if(!modifiers->children.empty())
            variableNode->children.push_back(modifiers);
        
        if(!isInClass)
            variableNode->isGlobal = true;
        
        return variableNode;
    }
    
    if(!match(LexerNameSpace::TokenType::IDENTIFIER) && !match(LexerNameSpace::TokenType::KEYWORD)) {
        consumeToken();
        return nullptr;
    }
    
    Token d = tokens[currentTokenIndex];
    
    // Go back 1 index for, [MemberType] -> double radius
    //currentTokenIndex--;
    
    ASTNode* variableNode = parseVariable(memberDeclarationType, memberType);
    
    if(!modifiers->children.empty())
        variableNode->children.push_back(modifiers);
    
    if(match(LexerNameSpace::TokenType::RIGHT_PAREN))
        consumeToken(); // Consume ')'
    
    if(!isInClass)
        variableNode->isGlobal = true;
    
    return variableNode;
    //}
}

ASTNode* Parser::parseFunction(const std::string& returnType, const std::string& functionName) {
    ASTNode* memberFunction = new ASTNode(NodeType::MEMBER_FUNCTION, functionName);
    memberFunction->children.push_back(new ASTNode(NodeType::RETURN_TYPE, returnType));
    
    // Parse function parameters
    if (match(LexerNameSpace::TokenType::LEFT_PAREN)) { // '('
        consumeToken(); // Consume '('
        
        // Parse parameters (if any)
        std::vector<std::string> parameters = parseFunctionParameters();
        
        consumeToken(); // Consume ')'
        
        Token c = tokens[currentTokenIndex];
        
        //This is for;
        //virtual void draw() const = 0;
        //void draw() {}
        /*TODO;
         * Use while loop instead for;
         * void draw() const override
         */
        if(match(LexerNameSpace::TokenType::KEYWORD) && (matchNext(LexerNameSpace::TokenType::OPERATOR, "=") || matchNext(LexerNameSpace::TokenType::RIGHT_BRACE))) { //TODO; FOR virtual void draw() const = 0;
            std::string type = tokens[currentTokenIndex].value;
            
            consumeToken(); // Consume 'const' token
            
            Token dd = tokens[currentTokenIndex];
            
            // Has a body?
            if(match(LexerNameSpace::TokenType::OPERATOR, "=")) {
                // No body
                
                consumeToken(); // Consume '=' token
                
                Token d = tokens[currentTokenIndex];
                
                if (match(LexerNameSpace::TokenType::INTEGER_LITERAL) && tokens[currentTokenIndex].value == "0") {
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
        // TODO; Make this as a function. I'm too lazy :D
        while(!match(LexerNameSpace::TokenType::LEFT_BRACE)) { // '{'
            std::string type = tokens[currentTokenIndex].value;
            
            if(match(LexerNameSpace::TokenType::KEYWORD, "const")) {
                memberFunction->children.push_back(new ASTNode(NodeType::ConstModifier, type + ": true"));
            } else if(match(LexerNameSpace::TokenType::KEYWORD, "override")) {
                memberFunction->children.push_back(new ASTNode(NodeType::OverrideModifier, type + ": true"));
            } else if(match(LexerNameSpace::TokenType::KEYWORD, "final")) {
                memberFunction->children.push_back(new ASTNode(NodeType::FinalModifier, type + ": true"));
            } else if(match(LexerNameSpace::TokenType::KEYWORD, "protected")) {
                memberFunction->children.push_back(new ASTNode(NodeType::ProtectedModifier, type + ": true"));
            }
            
            consumeToken(); // Consume modifier token
        }
        
        Token bb = tokens[currentTokenIndex];
        
        // Parse function body
        //TODO; DONT SKIP
        ASTNode* functionBody = parseFunctionBody();
        
        if (functionBody) {
            // Add parameters to the member function node
            
            for (int i = 0; !parameters.empty() && i < parameters.size() - 1; i+=2) {
                std::string paramName = parameters[i];
                std::string paramType = parameters[i + 1];
                
                ASTNode* parameterNode = new ASTNode(NodeType::PARAMETER, paramName);
                parameterNode->children.push_back(new ASTNode(NodeType::VARIABLE_TYPE, paramType));
                
                memberFunction->children.push_back(parameterNode);
            }
            
            //Empty function
            memberFunction->children.push_back(functionBody);
        }
    }
    
    return memberFunction;
}

ASTNode* Parser::parseFunctionBody() {
    ASTNode* functionBodyNode = new ASTNode(NodeType::FUNCTION_BODY, "FunctionBody");
    
    Token t = tokens[currentTokenIndex];
    
    while (currentTokenIndex < tokens.size() && !match(LexerNameSpace::TokenType::RIGHT_BRACE)) { // '}'
        Token d = tokens[currentTokenIndex];
        
        ASTNode* statement = parseStatement(); // Parse individual statements within the function body
        
        /*if(statement->children.size() < 2)
            return new ASTNode(NodeType::FUNCTION_BODY, "EMPTY BODY");*/
        
        if (statement) {
            functionBodyNode->children.push_back(statement);
        } else {
            // Handle syntax errors or unknown constructs within function body
            // Skip token or perform error recovery logic
            currentTokenIndex++;
        }
    }
    
    if(match(LexerNameSpace::TokenType::RIGHT_BRACE) && isInClass)
        consumeToken(); // Consume '}'
    
    return functionBodyNode;
}

ASTNode* Parser::parseStatement() {
    Token fz = tokens[currentTokenIndex];
    
    if (match(LexerNameSpace::TokenType::LEFT_BRACE)) {
        consumeToken(); // Consume '{'
        
        ASTNode* blockNode = new ASTNode(NodeType::BLOCK, "Block");
        while (!match(LexerNameSpace::TokenType::RIGHT_BRACE)) {
            if(match(LexerNameSpace::TokenType::SEMICOLON)) {
                consumeToken();
                continue;
            }
            
            Token f = tokens[currentTokenIndex];
            
            ASTNode* statement = parseStatement();
            
            if (statement) {
                blockNode->children.push_back(statement);
            } else {
                currentTokenIndex++;
            }
            
            if(match(LexerNameSpace::TokenType::SEMICOLON))
                consumeToken(); // Consume ';'
            
            Token fxz = tokens[currentTokenIndex];
            
            // Function ended?
            if(match(LexerNameSpace::TokenType::RIGHT_BRACE)) { // '}'
                //THIS WAS THE PROBLEM?
                //consumeToken(); // Consume '};
                return blockNode;
            }
        }
        
        // Function ended?
        if(match(LexerNameSpace::TokenType::RIGHT_BRACE))
            return blockNode;
        
        Token dd = tokens[currentTokenIndex];
        
        if(currentTokenIndex < tokens.size() - 1 && match(LexerNameSpace::TokenType::RIGHT_BRACE)) { // '}'
            Token dsd = tokens[currentTokenIndex];
            consumeToken(); // Consume '}' at the end of the block
        }
        
        return blockNode;
    }
    
    Token tz = tokens[currentTokenIndex];
    
    // Warning.. junky code..
    if(match(LexerNameSpace::TokenType::BREAK_STATEMENT)) {
        consumeToken(); // Consume statement
        return new ASTNode(NodeType::BREAK_STATEMENT, "Break statement");
    } else if(match(LexerNameSpace::TokenType::CONTINUE_STATEMENT)) {
        consumeToken(); // Consume statement
        return new ASTNode(NodeType::CONTINUE_STATEMENT, "Continue statement");
    }/* else if(match(LexerNameSpace::TokenType::RETURN_STATEMENT)) {
        consumeToken(); // Consume statement
        return new ASTNode(NodeType::RETURN_STATEMENT, "Return statement");
    }*/
    
    Token f = tokens[currentTokenIndex];
    
    if(match(LexerNameSpace::TokenType::RIGHT_BRACE)) // '}'
        return new ASTNode(NodeType::FUNCTION_BODY, "EmptyFunctionBody");
    
    // Check if it's a function call
    /*ASTNode* functionCallNode = parseFunctionCall();
    if (functionCallNode) {
        return functionCallNode; // Return the function call node or handle it as needed
    }*/
    
    // Handle return statement:
    if(match(LexerNameSpace::TokenType::KEYWORD)) {
        if(tokens[currentTokenIndex].value == "return") {
            consumeToken(); // Consume 'return'
            
            Token fzz = tokens[currentTokenIndex];
            
            ASTNode* expressionNode = expression(); // Parse expression on the right-hand side of assignment
            
            // Create an assignment node manually, using type and value accordingly
            ASTNode* assignmentNode = new ASTNode(NodeType::RETURN_STATEMENT, "RETURN STATEMENT:");
            assignmentNode->children.push_back(expressionNode); // Attach the expression as a child
            
            Token e = tokens[currentTokenIndex];
            
            return assignmentNode;
        } else if(tokens[currentTokenIndex].value == "delete") {
            consumeToken(); // Consume 'delete'
            
            Token fzz = tokens[currentTokenIndex];
            
            ASTNode* expressionNode = expression(); // Parse expression on the right-hand side of assignment
            
            // Create an assignment node manually, using type and value accordingly
            ASTNode* assignmentNode = new ASTNode(NodeType::DELETE_STATEMENT, "DELETE STATEMENT:");
            assignmentNode->children.push_back(expressionNode); // Attach the expression as a child
            
            Token e = tokens[currentTokenIndex];
            
            return assignmentNode;
        }
    }
    
    switch (tokens[currentTokenIndex].type) {
    case LexerNameSpace::TokenType::FOR_LOOP: {
            // Idk if there are other types of "for loops"
            if (tokens[currentTokenIndex].value == "for") {
                /**
                 * Types of for loops:
                 * for(int i = 0; i < #; i++) {}
                 * for(int f : fs) {}
                 */
                consumeToken(); // Consume 'for'
                
                ASTNode* forLoopNode = new ASTNode(NodeType::FOR_LOOP, "ForLoop");
                
                if(!match(LexerNameSpace::TokenType::LEFT_PAREN)) {
                    std::cerr << "Syntax Error: Missing ')' after 'for' in for loop initialization.\n";
                } else
                    consumeToken(); // Consume '('
                
                // Parse initialization
                if(!match(LexerNameSpace::TokenType::SEMICOLON)) {
                    ASTNode* initNode = parseMember(NodeType::MEMBER_VARIABLE); // Assuming you have a function to parse expressions
                    forLoopNode->children.push_back(initNode);
                    
                    if(!match(LexerNameSpace::TokenType::SEMICOLON)) {
                        std::cerr << "Syntax Error: Missing semicolon after '" << initNode->value << "' in for loop initialization.\n";
                    } else
                        consumeToken(); // Consume ';'
                } else {
                    consumeToken(); // Consume ';'
                }
                
                // Parse condition
                ASTNode* conditionNode = parseConditions(); // Assuming you have a function to parse expressions
                forLoopNode->children.push_back(conditionNode);
                
                if(!match(LexerNameSpace::TokenType::SEMICOLON)) {
                    std::cerr << "Syntax Error: Missing semicolon after '" << conditionNode->value << "' in for loop condition.\n";
                } else
                    consumeToken(); // Consume ';'
                
                // Parse iteration
                ASTNode* iterationNode = expression(); // Assuming you have a function to parse expressions
                forLoopNode->children.push_back(iterationNode);
                
                if(tokens[currentTokenIndex - 1].type != LexerNameSpace::TokenType::RIGHT_PAREN) {
                    std::cerr << "Syntax Error: Missing ')' after '" << iterationNode->value << "' in for loop iteration.\n";
                }/* else
                    consumeToken(); // Consume ')'*/
                
                Token t = tokens[currentTokenIndex];
                
                ASTNode* body = parseStatement();
                forLoopNode->children.push_back(body);
                
                Token za = tokens[currentTokenIndex];
                
                if(match(LexerNameSpace::TokenType::RIGHT_BRACE)) { // '}'
                    consumeToken(); // Consume '}'
                }
                
                return forLoopNode;
            }
            
            break;
        }
    case LexerNameSpace::TokenType::WHILE_LOOP: {
            if(tokens[currentTokenIndex].value == "while") {
                consumeToken(); // Consume 'while'
                
                ASTNode* whileloopNode = new ASTNode(NodeType::WHILE_LOOP, "While Loop");
                
                if(!match(LexerNameSpace::TokenType::LEFT_PAREN)) {
                    std::cout << "Missing '(' after while loop\n";
                } else
                    consumeToken(); // Consume '('
                
                ASTNode* conditionsNode = parseConditions();
                ASTNode* bodyNode = parseStatement();
                
                whileloopNode->children.push_back(conditionsNode);
                whileloopNode->children.push_back(bodyNode);
                
                Token tfd = tokens[currentTokenIndex];
                
                if(!match(LexerNameSpace::TokenType::RIGHT_BRACE)) {
                    std::cout << "Missing '}' after while loop\n";
                } else
                    consumeToken(); // Consume '}'
                
                return whileloopNode;
            }
            
            break;
        }
    case LexerNameSpace::TokenType::DO_WHILE_LOOP: {
            if(tokens[currentTokenIndex].value == "do") {
                consumeToken(); // Consume 'do'
                
                ASTNode* doWhileloopNode = new ASTNode(NodeType::WHILE_LOOP, "Do While Loop");
                ASTNode* bodyNode = parseStatement();
                
                Token tfjjd = tokens[currentTokenIndex];
                
                if(!match(LexerNameSpace::TokenType::RIGHT_BRACE)) {
                    std::cout << "Missing '}' after do while loop\n";
                } else
                    consumeToken(); // Consume '}'
                
                Token tfdz = tokens[currentTokenIndex];
                
                consumeToken(); // Consume 'while'
                
                if(!match(LexerNameSpace::TokenType::LEFT_PAREN)) {
                    std::cout << "Missing '( after while in do while loop\n";
                } else
                    consumeToken(); // Consume '('
                
                ASTNode* conditionsNode = parseConditions();
                
                doWhileloopNode->children.push_back(conditionsNode);
                doWhileloopNode->children.push_back(bodyNode);
                
                return doWhileloopNode;
            }
            
            break;
        }
    case LexerNameSpace::TokenType::IF_STATEMENT: {
            return parseIfStatement();
        }
    case LexerNameSpace::TokenType::ELSE_STATEMENT: {
            if(matchNext(LexerNameSpace::TokenType::IF_STATEMENT)) {
                consumeToken(); // Consume 'else'
                
                ASTNode* ifStatement = parseIfStatement();
                ifStatement->value = "Else if Statement";
                ifStatement->type = NodeType::ELSEIF_STATEMENT;
                
                return ifStatement;
            }
            
            return parseElseStatement();
        }
    }
    
    Token hghg = tokens[currentTokenIndex];
    
    if(match(LexerNameSpace::TokenType::OPERATOR))
        return expression();
    
    return parseMember(NodeType::LOCAL_VARIABLE_DECLARATION);
}

ASTNode* Parser::parseFunctionCall() {
    Token t = tokens[currentTokenIndex];
    
    if (match(LexerNameSpace::TokenType::IDENTIFIER)) {
        std::string functionName = tokens[currentTokenIndex].value;
        consumeToken(); // Consume function name.
        
        // Handling things like "shape.somefunction();"
        
        /*
        if(match(LexerNameSpace::TokenType::DOT) || match(LexerNameSpace::TokenType::OPERATOR, "->")) {
            
        }
        */
        
        if(match(LexerNameSpace::TokenType::LEFT_PAREN)) {
            ASTNode* functionCallNode = new ASTNode(NodeType::FUNCTION_CALL, functionName);
            Token txx = tokens[currentTokenIndex];
            
            if (match(LexerNameSpace::TokenType::LEFT_PAREN)) {
                consumeToken(); // Consume '('
                
                Token z = tokens[currentTokenIndex];
                
                ASTNode* parameters = new ASTNode(NodeType::PARAMETER, "PARAMETERS;");
                
                while(!match(LexerNameSpace::TokenType::RIGHT_PAREN)) { // ')'
                    if(match(LexerNameSpace::TokenType::COMMA))
                        consumeToken();
                    
                    Token zzaToken = tokens[currentTokenIndex];
                    
                    ASTNode* paramaterNode = expression();
                    parameters->children.push_back(paramaterNode);
                }
                
                if(parameters)
                    functionCallNode->children.push_back(parameters);
                
                if (match(LexerNameSpace::TokenType::RIGHT_PAREN)) {
                    consumeToken(); // Consume ')'
                } else {
                    std::cerr << "Error; Missing ')' after function call\n";
                }
                
                return functionCallNode;
            }
        } else if(match(LexerNameSpace::TokenType::LEFT_SQUARE_BRACE)) {
            ASTNode* arrayCall = new ASTNode(NodeType::ARRAY_CALL, "Array call: " + functionName);
            
            Token tokenz = tokens[currentTokenIndex];
            
            while(match(LexerNameSpace::TokenType::LEFT_SQUARE_BRACE)) {
                Token token = tokens[currentTokenIndex];
                consumeToken(); // Consume '['
                
                Token tz = tokens[currentTokenIndex];
                ASTNode* indexNode = expression();
                arrayCall->children.push_back(indexNode);
                
                if(match(LexerNameSpace::TokenType::RIGHT_SQUARE_BRACE)) {
                    consumeToken(); // Consume ']'
                } else {
                    std::cerr << "Error; Forgot ']' after array calling\n";
                }
            }
            
            Token z = tokens[currentTokenIndex];
            
            if(match(LexerNameSpace::TokenType::DOT)) {
                ASTNode* statement = parseStatement();
                arrayCall->children.push_back(statement);
            } else if(match(LexerNameSpace::TokenType::OPERATOR)) {
                if(match(LexerNameSpace::TokenType::OPERATOR, "+")) {
                    std::cout << "Yahoo\n";
                }
                
                ASTNode* assignment = new ASTNode(NodeType::ASSIGNMENT, tokens[currentTokenIndex].value);
                
                consumeToken(); // Consume 'operator'
                
                ASTNode* statement = parseStatement();
                
                assignment->children.push_back(statement);
                arrayCall->children.push_back(assignment);
            }
            //}
            
            Token tzzz = tokens[currentTokenIndex];

            if(match(LexerNameSpace::TokenType::SEMICOLON))
                consumeToken(); // Consume ';'
            
            return arrayCall;
        } else {
            Token curToken = tokens[currentTokenIndex];
            ASTNode* functionNode = new ASTNode(NodeType::FUNCTION_CALL, functionName);
            
            while(!match(LexerNameSpace::SEMICOLON)) {
                ASTNode* statement = parseStatement();
                functionNode->addChild(statement);
            }
            
            return functionNode;
        }
    }
    
    return nullptr;
}

ASTNode* Parser::parseIfStatement() {
    consumeToken(); // Consume 'if'
    
    if(!match(LexerNameSpace::TokenType::LEFT_PAREN)) {
        std::cerr << "Error: Missing '(' after if statement.\n";
        
        return nullptr;
    }
    
    consumeToken(); // '('
    
    Token t = tokens[currentTokenIndex];
    
    ASTNode* conditionsNode = parseConditions();
    ASTNode* ifBodyNode = parseStatement(); // Parse the body of 'if'
    
    // Create an 'if' statement node
    ASTNode* ifStatementNode = new ASTNode(NodeType::IF_STATEMENT, "If Statement");
    ifStatementNode->children.push_back(conditionsNode);
    ifStatementNode->children.push_back(ifBodyNode);

    Token zaOne = tokens[currentTokenIndex];
    
    // Should be at the end of the if statement.
    if(match(LexerNameSpace::TokenType::RIGHT_BRACE)) {
        consumeToken(); // '}'
    }
    
    Token zzz = tokens[currentTokenIndex];
    
    return ifStatementNode;
}

ASTNode* Parser::parseElseStatement() {
    consumeToken(); // Consume 'else'
    
    ASTNode* elseBodyNode = parseStatement(); // Parse the body of 'else'
    
    // Create an 'else' statement node and attach the condition and body
    ASTNode* elseStatementNode = new ASTNode(NodeType::ELSE_STATEMENT, "Else Statement");
    elseStatementNode->children.push_back(elseBodyNode);
    
    // Should be at the end of the else statement.
    if(match(LexerNameSpace::TokenType::RIGHT_BRACE)) {
        consumeToken(); // '}'
    }
    
    return elseStatementNode;
}

ASTNode* Parser::parseConditions() {
    ASTNode* conditionsNode = new ASTNode(NodeType::CONDITIONS, "CONDITIONS;");
    
    while(!match(LexerNameSpace::TokenType::LEFT_BRACE) && !match(LexerNameSpace::TokenType::SEMICOLON)) {
        Token z = tokens[currentTokenIndex];
        
        if(match(LexerNameSpace::TokenType::LEFT_PAREN)) {
            consumeToken(); // Consume '('
            conditionsNode->children.push_back(parseConditions());
            
            if(match(LexerNameSpace::TokenType::LEFT_BRACE))
                break;
        }
        
        Token zxx = tokens[currentTokenIndex];
        
        conditionsNode->children.push_back(parseCondition());
        Token tzz = tokens[currentTokenIndex];
        
        if(match(LexerNameSpace::TokenType::LOGICAL_OR) || match(LexerNameSpace::TokenType::LOGICAL_AND)) {
            conditionsNode->children.push_back(new ASTNode(match(LexerNameSpace::TokenType::LOGICAL_OR) ? NodeType::LOGICAL_OR : NodeType::LOGICAL_AND, tokens[currentTokenIndex].value));
            
            consumeToken(); // Consume logical gate.
        }
    }
    
    return conditionsNode;
}

ASTNode* Parser::parseCondition() {
    // Parse the condition
    ASTNode* condition = expression();
    
    if(match(LexerNameSpace::TokenType::RIGHT_PAREN)) { // ')'
        consumeToken(); // Consume ')'
    } else {
        std::cerr << "Error: Missing ')' after if statement condition.\n";
    }
    
    Token tzz = tokens[currentTokenIndex];
    
    // Assuming this will be for conditions such as:
    // (i > 5 && i < 10)
    //if(match(LexerNameSpace::TokenType::RIGHT_PAREN))
        //consumeToken(); // Consume ')'
    
    // Check for '{' symbol
    if (!match(LexerNameSpace::TokenType::SEMICOLON) && !match(LexerNameSpace::TokenType::RIGHT_PAREN) && !match(LexerNameSpace::TokenType::LEFT_BRACE) && (!match(LexerNameSpace::TokenType::LOGICAL_AND) && !match(LexerNameSpace::TokenType::LOGICAL_OR))) {
        throw std::runtime_error("Expected '{' after 'if' condition");
    }
    
    return condition;
}

ASTNode* Parser::parseNamespacesUsage(int counter) {
    std::string functionName = tokens[currentTokenIndex].value;
    Token t = tokens[currentTokenIndex];
    
    //.someFunction();
    if(match(LexerNameSpace::TokenType::DOT)) {
        consumeToken(); // Consume '.'
        
        return parseStatement();
    }
    
    // Possible namespace or class.somefunction;
    if(match(LexerNameSpace::TokenType::IDENTIFIER) && (matchNext(LexerNameSpace::TokenType::DOT) || matchNext(LexerNameSpace::TokenType::OPERATOR, "->") || matchNext(LexerNameSpace::TokenType::OPERATOR, "::"))) {
        consumeToken(); // Consume namespace/class.
        
        std::string referenceType = "";
        
        if(match(LexerNameSpace::TokenType::OPERATOR, "::")) {
            referenceType = "STATIC";
        } else {
            referenceType = "LOCAL";
        }
        
        consumeToken(); // Consume reference
        
        Token zaToken = tokens[currentTokenIndex];
        
        // Just a functioncall.
        //shape.draw(5);
        if(matchNext(LexerNameSpace::TokenType::LEFT_PAREN)) {
            ASTNode* functionCallNode = new ASTNode(NodeType::FUNCTION_CALL, functionName);
            functionCallNode->addChild(NodeType::RETURN_TYPE, referenceType);
            functionCallNode->addChild(parseFunctionCall());

            return functionCallNode;
        }
        
        if(matchNext(LexerNameSpace::TokenType::LEFT_ANGLE_BRACE, "<")) {
        //if (match("vector")) {
            std::string vectorType = tokens[currentTokenIndex].value;
            consumeToken(); // Consume "vector"
            
            Token ddtzz = tokens[currentTokenIndex];
            
            if (!match(LexerNameSpace::TokenType::LEFT_ANGLE_BRACE)) {
                std::cerr << "Expected '<' after 'vector'\n";
            } else
                consumeToken(); // Consume "<"
            
            // std::vector<std::vector<int>> pascalTriangle(n);
            Token ddzztzz = tokens[currentTokenIndex];
            
            ASTNode* vectorTypeNode = new ASTNode(NodeType::VECTOR_TYPE, "Vector Type: " + vectorType);
            
            // std::vector<std::vector<int>> pascalTriangle(n);
            while(!match(LexerNameSpace::TokenType::RIGHT_ANGLE_BRACE)) {
                // Parse type.
                ASTNode* type = parseNamespacesUsage(++counter);
                
                if(!type) {
                    Token z = tokens[currentTokenIndex];
                    
                    vectorTypeNode->children.push_back(new ASTNode(NodeType::VARIABLE_TYPE, tokens[currentTokenIndex].value));
                    
                    consumeToken(); // Consume type.
                } else {
                    vectorTypeNode->children.push_back(type);
                }
            }
            
            Token tsz = tokens[currentTokenIndex];
            
            if(match(LexerNameSpace::TokenType::RIGHT_ANGLE_BRACE))
                consumeToken(); // Consume '>'
            else
                std::cerr << "Missing '>' after vector\n";
            
            return vectorTypeNode;
        } /*else if(match("cout")) {
            ASTNode* coutStatementNode = new ASTNode(NodeType::COUT_STATEMENT, "cout");
            consumeToken(); // Consume cout
            Token za = tokens[currentTokenIndex];
            
            while(!match(LexerNameSpace::TokenType::SEMICOLON)) {
                if(match("<<")) {
                    coutStatementNode->children.push_back(new ASTNode(NodeType::OPERATOR, "<<"));
                    
                    consumeToken(); // Consume <<
                }
                
                Token ss = tokens[currentTokenIndex];
                
                if(match(LexerNameSpace::TokenType::STRING_LITERAL)) {
                    coutStatementNode->children.push_back(new ASTNode(NodeType::STRING, tokens[currentTokenIndex].value));
                    consumeToken();
                } else if(match(LexerNameSpace::TokenType::IDENTIFIER) && matchNext(LexerNameSpace::TokenType::OPERATOR, "<<")) {
                    coutStatementNode->children.push_back(new ASTNode(NodeType::STRING, tokens[currentTokenIndex].value));
                    consumeToken();
                } else {
                    coutStatementNode->children.push_back(parseStatement());
                }
                
                Token end = tokens[currentTokenIndex];
            }
            
            return coutStatementNode;
        } else if(match("endl")) {
            consumeToken(); // Consume 'endl'
            return new ASTNode(NodeType::STRING, tokens[currentTokenIndex - 1].value);
        }*/ else {
            ASTNode* functionCall = new ASTNode(NodeType::FUNCTION_CALL, functionName);
            ASTNode* accessType   = new ASTNode(NodeType::ACCESSTYPE, referenceType);
            ASTNode* statementNode = parseStatement();
            accessType->addChild(statementNode);
            functionCall->addChild(accessType);
            
            return functionCall;
        }
    }
    
    return nullptr;
}

ASTNode* Parser::parseVariable(NodeType memberDeclarationType, const std::string& memberType) {
    std::string memberName = tokens[currentTokenIndex].value;
    consumeToken(); // Consume member name
    
    Token t = tokens[currentTokenIndex];
    
    if (match(LexerNameSpace::TokenType::LEFT_PAREN)) { // '('
        return parseFunction(memberType, memberName);
    }
    
    //TODO; global variable..? Can't remember what I meant by this...
    ASTNode* memberVariable = new ASTNode(memberDeclarationType, memberName);
    memberVariable->children.push_back(new ASTNode(NodeType::VARIABLE_TYPE, memberType));
    
    // If variable has no assignment.
    if (match(LexerNameSpace::TokenType::SEMICOLON)) {
        return memberVariable;
    }
    
    ASTNode* compoundAssignNode = new ASTNode(NodeType::COMPOUND_ASSIGNMENT, tokens[currentTokenIndex].value);
    
    //TODO; this is returing nullptr. When????????????
    consumeToken(); // Consume operator token
    ASTNode* expressionNode = expression(); // Parse expression on the right-hand side of assignment
    
    compoundAssignNode->children.push_back(expressionNode);
    memberVariable->children.push_back(compoundAssignNode);
    
    return memberVariable;
}

ASTNode* Parser::parseReference(NodeType memberDeclarationType, const std::string& memberType) {
    Token tzzz = tokens[currentTokenIndex];
    
    NodeType pointerType;
    
    if(tokens[currentTokenIndex].type == LexerNameSpace::TokenType::POINTER)
        pointerType = NodeType::POINTER;
    else
        pointerType = NodeType::REFERENCE;
    
    consumeToken(); // Consume pointer
    
    std::string memberName = tokens[currentTokenIndex].value;
    consumeToken(); // Consume memberName
    
    ASTNode* memberReference = new ASTNode(memberDeclarationType, memberName);
    memberReference->children.push_back(new ASTNode(NodeType::VARIABLE_TYPE, memberType));
    memberReference->children.push_back(new ASTNode(pointerType, memberType));
    
    Token t = tokens[currentTokenIndex];
    
    // If variable has no assignment.
    if (match(LexerNameSpace::TokenType::SEMICOLON)) {
        return memberReference;
    }
    
    ASTNode* compoundAssignNode = new ASTNode(NodeType::COMPOUND_ASSIGNMENT, tokens[currentTokenIndex].value);
    
    consumeToken(); // Consume operator token
    ASTNode* expressionNode = expression(); // Parse expression on the right-hand side of assignment
    
    compoundAssignNode->children.push_back(expressionNode);
    memberReference->children.push_back(compoundAssignNode);
    
    if(match(LexerNameSpace::TokenType::RIGHT_PAREN))
        consumeToken(); // Consume ')'
    
    Token z = tokens[currentTokenIndex];
    
    return memberReference;
}

std::vector<std::string> Parser::parseFunctionParameters() {
    std::vector<std::string> parameters;
    
    Token t = tokens[currentTokenIndex];
    
    while (!match(LexerNameSpace::TokenType::RIGHT_PAREN)) {
        Token t = tokens[currentTokenIndex];
        
        if(match(LexerNameSpace::TokenType::COMMA))
            consumeToken();

        std::string paramType = tokens[currentTokenIndex].value;
        consumeToken(); // Consume parameter type token

        Token zzaToken = tokens[currentTokenIndex];

        if (match(LexerNameSpace::TokenType::IDENTIFIER)) {
            std::string paramName = tokens[currentTokenIndex].value;
            //parameters.push_back(paramType + " " + paramName);
            parameters.push_back(paramName);
            parameters.push_back(paramType);
            
            consumeToken(); // Consume parameter name token
            
            if (!match(LexerNameSpace::TokenType::COMMA)) {
                break; // Last parameter, exit loop
            }

            consumeToken(); // Consume ',' before the next parameter
        } else {
            // Handle syntax errors in parameter declaration
            std::cout << "ERROR; Seems to have a syntax error in the parameter declaration at: " << currentTokenIndex << "\n";
        }
        
        /*} else {
            // Handle syntax errors in parameter declaration
            std::cout << "ERROR; Seems to have a syntax error in the parameter declaration at: " << currentTokenIndex << "\n"; 
            
            //return nullptr;
        }*/
    }
    
    return parameters;
}

ASTNode* Parser::expression() {
    if(currentTokenIndex >= tokens.size() - 1)
        return nullptr;
    
    Token tZ = tokens[currentTokenIndex];
    
    ASTNode* left = term();
    
    Token t = tokens[currentTokenIndex];
    
    while (currentTokenIndex < tokens.size() - 2 && (!match(LexerNameSpace::TokenType::OPERATOR, "*") || !match(LexerNameSpace::TokenType::OPERATOR, "/"))  && tokens[currentTokenIndex].type == LexerNameSpace::TokenType::OPERATOR &&
           (tokens[currentTokenIndex].value == "+" || tokens[currentTokenIndex].value == "-") ||
           (tokens[currentTokenIndex].value == "<" || tokens[currentTokenIndex].value == ">") ||
           match(LexerNameSpace::TokenType::OPERATOR) ||
           /*(match(LexerNameSpace::TokenType::INCREMENT) || match(LexerNameSpace::TokenType::DECREMENT)) ||
           (match(LexerNameSpace::TokenType::INCREMENT_EQUAL) || match(LexerNameSpace::TokenType::DECREMENT_EQUAL)) ||*/
           (match(LexerNameSpace::TokenType::LESS_THAN_EQUAL) || match(LexerNameSpace::TokenType::GREATER_THAN_EQUAL)) ||
           (match(LexerNameSpace::TokenType::EQUAL) || match(LexerNameSpace::TokenType::NOT_EQUAL))) {
        std::string op = tokens[currentTokenIndex++].value;
        
        ASTNode* right = term();
        
        if(right == nullptr) {
            consumeToken(); // Consume 'operator'
            right = parseStatement();
        }
        
        ASTNode* newOpNode = new ASTNode(NodeType::OPERATOR, op);
        newOpNode->children.push_back(left);
        newOpNode->children.push_back(right);
        left = newOpNode;
    }

    if(left == nullptr) {
        
    }
    
    return left;
}

//TODO; So for:
//"std::cout << "somevalue" << std::endl;
//This doesn't return anything for a "<<".
ASTNode* Parser::term() {
    std::string assignmentType = "";
    std::vector<std::string> parameters;
    
    if(match(LexerNameSpace::TokenType::KEYWORD, "new")) {
        assignmentType = "new";
        consumeToken();
    }
    
    std::string type = tokens[currentTokenIndex].value;
    ASTNode* left = new ASTNode(NodeType::ASSIGNMENT, assignmentType + "; " + type);
    Token t = tokens[currentTokenIndex];
    
    // Points to a class, perhaps?
    if(match(LexerNameSpace::TokenType::IDENTIFIER) && matchNext(LexerNameSpace::TokenType::LEFT_PAREN)) {
        consumeToken(); // Consume type
        
        if(match(LexerNameSpace::TokenType::SEMICOLON)) {
            return left;
        }
        
        consumeToken(); // Consume '('
        
        while (!match(LexerNameSpace::TokenType::RIGHT_PAREN)) { // ')'
            Token tz = tokens[currentTokenIndex];
            
            if (match(LexerNameSpace::TokenType::COMMA)) {
                //Skip
                consumeToken(); // Consume ','
                
                continue;
            }
            
            if (match(LexerNameSpace::TokenType::SEMICOLON)) {
                return left;
            }
            
            parameters.push_back(tokens[currentTokenIndex].value);
            left->children.push_back(new ASTNode(NodeType::PARAMETER, tokens[currentTokenIndex].value));
        
            consumeToken(); // Consume parameter
        }
        
        return left;
    }
    
    left = factor();
    
    Token zzzt = tokens[currentTokenIndex];
    
    while (currentTokenIndex < tokens.size() && (match(LexerNameSpace::TokenType::POINTER) || match(LexerNameSpace::TokenType::OPERATOR)) &&
           (tokens[currentTokenIndex].value == "*" || tokens[currentTokenIndex].value == "/")) {
        
        std::string op = tokens[currentTokenIndex++].value;
        
        ASTNode* right = factor();
        ASTNode* newOpNode = new ASTNode(NodeType::OPERATOR, op);
        newOpNode->children.push_back(left);
        newOpNode->children.push_back(right);
        left = newOpNode;
    }

    if(left == nullptr) {
        return new ASTNode(NodeType::OPERATOR, type);
    }
    
    return left;
}

ASTNode* Parser::factor() {
    Token currentToken = tokens[currentTokenIndex++];
    
    if (currentToken.type == LexerNameSpace::TokenType::INTEGER_LITERAL ||
        currentToken.type == LexerNameSpace::TokenType::FLOATING_POINT_LITERAL ||
        currentToken.type == LexerNameSpace::TokenType::IDENTIFIER ||
        currentToken.type == LexerNameSpace::TokenType::KEYWORD ||
        currentToken.type == LexerNameSpace::TokenType::STRING_LITERAL) {
        return new ASTNode(NodeType::EXPRESSION, currentToken.value);
    }
    
    std::cout << "huh " << currentToken.value << '\n';
    
    // TODO; Handle parentheses or other factors
    
    return nullptr;
}
