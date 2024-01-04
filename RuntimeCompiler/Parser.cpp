#include "Parser.h"

#include <map>

std::map<std::string, NodeType> modifiersTypes =
    {
        // Normal modifiers
        {"const", NodeType::ConstModifier},
        {"final", NodeType::FinalModifier},
        {"protected", NodeType::ProtectedModifier},
        {"virtual", NodeType::VirtualModifier},
        {"override", NodeType::OverrideModifier},
        
        // Variable modifiers
    };

ASTNode* Parser::parseCode() {
    ASTNode* programAST = new ASTNode(NodeType::PROGRAM, "Program");
    
    while (currentTokenIndex < tokens.size() - 1) {
        ASTNode* node = nullptr;
        
        if(match(TokenType::SEMICOLON))
            consumeToken();
        
        Token toekziao = tokens[currentTokenIndex];
        
        if (match(TokenType::PREPROCESSOR_DIRECTIVE)) {
            node = parseIncludeDirective();
        } else if(match(TokenType::NAMESPACE)) { //TODO; Not detecting namespace.
            node = parseNameSpace();
        } else if (match(TokenType::CLASS)) {
            isInClass = true;
            
            node = parseClassDeclaration();
        } else {
            // Try parsing a function
            node = parseMember(NodeType::MEMBER_VARIABLE);
            
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

ASTNode* Parser::parseIncludeDirective() {
    if (match(TokenType::PREPROCESSOR_DIRECTIVE)) {
        std::string includeValue = tokens[currentTokenIndex].value;
        consumeToken(); // Consume the '#include' directive
        
        // Create an AST node representing the include directive
        ASTNode* includeNode = new ASTNode(NodeType::INCLUDE_DIRECTIVE, includeValue);
        
        return includeNode;
    }
    
    // Handle error or return nullptr for invalid or unexpected tokens
    return nullptr;
}

ASTNode* Parser::parseNameSpace() {
    if (match(TokenType::NAMESPACE)) {
        std::string namespaceValue = tokens[currentTokenIndex].value;
        consumeToken(); // Consume the '#include' directive
            
        // Create an AST node representing the include directive
        ASTNode* namespaceNode = new ASTNode(NodeType::NAMESPACE, "NAMESPACE: " + namespaceValue);
        
        return namespaceNode;
    }
        
    // Handle error or return nullptr for invalid or unexpected tokens
    return nullptr;
}

ASTNode* Parser::parseClassDeclaration() {
    if (match(TokenType::CLASS)) {
        Token t = tokens[currentTokenIndex];
        
        consumeToken(); // Consume 'class' keyword
        
        if (match(TokenType::IDENTIFIER)) {
            std::string className = tokens[currentTokenIndex].value;
            consumeToken(); // Consume class name token
            
            this->className = className;
            
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
                
                if (match(TokenType::KEYWORD)) {
                    accessSpecifier = tokens[currentTokenIndex].value;
                    
                    consumeToken(); // Consume access specifier (public/private/protected)
                }
                
                Token f = tokens[currentTokenIndex];
                
                // Move on normally
                if (match(TokenType::IDENTIFIER)) {
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
                    
                    std::cout << "Error; seems to be missing a '}' somewhere.. Gl finding it buddy; " <<
                        currentTokenIndex << '\n';
                }
            }
        }
    }

    // Handle error or return nullptr for invalid or unexpected tokens
    return nullptr;
}

ASTNode* Parser::parseClassBody(std::string className) {
    ASTNode* classBodyNode = new ASTNode(NodeType::CLASS_BODY, "ClassBody");
    
    Token t = tokens[currentTokenIndex];
    
    while (currentTokenIndex < tokens.size() && !match(TokenType::RIGHT_BRACE)) {
        if(match(TokenType::SEMICOLON))
            consumeToken(); // Consume ';'
        
        if(match(TokenType::KEYWORD) && (tokens[currentTokenIndex].value == "public" || tokens[currentTokenIndex].value == "private") && matchNext(TokenType::COLON)) {
            consumeToken(); // Consume ?
            consumeToken(); // Consume ':'
        }
        
        std::string memberType = tokens[currentTokenIndex].value;
        
        if(memberType._Equal(className)) {
            ASTNode* constructorNode = parseConstructor();
            
            if (constructorNode) {
                classBodyNode->children.push_back(constructorNode);
                
                if(match(TokenType::RIGHT_BRACE))
                    consumeToken(); // Consume '}'
            } else {
                // Handle error: Failed to parse constructor
                std::cout << "Erorr happened while doing custrctor bs" << '\n';
                consumeToken();
            }
        }
        
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
    
    //TODO; ??????????????????????????????????????????????????????????????
    while (!match(TokenType::RIGHT_PAREN)) {
        Token t = tokens[currentTokenIndex];
        
        //Parameters: Wtf is this ???????????????????
        std::string paramType = tokens[currentTokenIndex].value;
        consumeToken();
        
        std::string paramName = tokens[currentTokenIndex].value;
        constructor->children.push_back(new ASTNode(NodeType::PARAMETER_VARIABLE, paramType + " " + paramName));
        consumeToken();
    }
    
    consumeToken(); // Consume ')'
    
    Token tt = tokens[currentTokenIndex];
    
    // Circle(double r) -> this : radius(r) {}
    if (match(TokenType::COLON)) {
        consumeToken(); // Consume ':'
        
        while (!match(TokenType::LEFT_BRACE)) {
            // {
            //: radius(r), test(t)..

            // Skip comma
            if (match(TokenType::COMMA))
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
    
    ASTNode* modifiers = new ASTNode(NodeType::MODIFIERS, "Modifiers; ");
    
    while(match(TokenType::KEYWORD)) {
        std::string type = tokens[currentTokenIndex].value;
        
        if(tokens[currentTokenIndex].type == TokenType::REFERENCE) {
            // Check if it's a pointer/reference
            std::string pointerType = tokens[currentTokenIndex].value;
            
            NodeType nodeType = NodeType::POINTER;
            if(tokens[currentTokenIndex].type == TokenType::REFERENCE)
                nodeType = NodeType::REFERENCE;
            
            ASTNode* pointerNode = new ASTNode(nodeType, "POINTER_TYPE: " + pointerType);
            pointerNode->children.push_back(new ASTNode(NodeType::POINTER, "TO: " + tokens[currentTokenIndex - 1].value));
            
            modifiers->children.push_back(pointerNode);
        } else {
            // Does type exist
            if(modifiersTypes.count(type)) {
                modifiers->children.push_back(new ASTNode(modifiersTypes[type], type));
            }
        }
        
        consumeToken(); // Consume modifier
    }
    
    std::string memberType;
    
    // Ik junky code shush
    if(token.type == TokenType::IDENTIFIER)
        memberType = tokens[currentTokenIndex].value;
    else
        if(currentTokenIndex == 0)
            memberType = tokens[currentTokenIndex].value;
        else
            memberType = tokens[currentTokenIndex - 1].value;
    
    Token t = tokens[currentTokenIndex];
    
    // Function
    if(matchNext(TokenType::LEFT_PAREN)) {
        std::string memberName = tokens[currentTokenIndex].value;
        consumeToken(); // Consume member
        
        return parseFunction(memberType, memberName);
    }
    
    // Perhaps a variable usage?
    //TODO; So this is the current problem..
    /*if(match(TokenType::IDENTIFIER) && (!matchNext(TokenType::KEYWORD) && !matchNext(TokenType::REFERENCE) && !matchNext(TokenType::POINTER))) {
        ASTNode* variableNode = parseVariable(memberDeclarationType, "None");
        variableNode->children.push_back(modifiers);
        
        return variableNode;
    }*/
    
    Token z = tokens[currentTokenIndex];
    
    if(!match(TokenType::IDENTIFIER) && !match(TokenType::KEYWORD)) {
        consumeToken();
        return nullptr;
    }
    
    //consumeToken(); // Consume member name
    
    Token d = tokens[currentTokenIndex];
    
    if (matchNext(TokenType::REFERENCE) || matchNext(TokenType::POINTER)) {
        consumeToken(); // Consume reference
        
        ASTNode* variableNode = parseReference(memberDeclarationType, memberType);
        
        if(!modifiers->children.empty())
            variableNode->children.push_back(modifiers);
        
        return variableNode;
    }
    
    // Go back 1 index for, [MemberType] -> double radius
    //currentTokenIndex--;
    
    ASTNode* variableNode = parseVariable(memberDeclarationType, memberType);
    
    if(!modifiers->children.empty())
        variableNode->children.push_back(modifiers);
    
    if(match(TokenType::RIGHT_PAREN))
        consumeToken(); // Consume ')'
    
    return variableNode;
    //}
}

ASTNode* Parser::parseFunction(const std::string& returnType, const std::string& functionName) {
    ASTNode* memberFunction = new ASTNode(NodeType::MEMBER_FUNCTION, "Function: " + functionName);
    memberFunction->children.push_back(new ASTNode(NodeType::RETURN_STATEMENT, "ReturnType: " + returnType));
        
    // Parse function parameters
    if (match(TokenType::LEFT_PAREN)) { // '('
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
                }/* else {
                    std::cout << "Unknown modifier..; " << type << '\n';
                }*/
                
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
    
    while (currentTokenIndex < tokens.size() && !match(TokenType::RIGHT_BRACE)) { // '}'
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
    
    if(match(TokenType::RIGHT_BRACE) && isInClass)
        consumeToken(); // Consume '}'
    
    return functionBodyNode;
}

ASTNode* Parser::parseStatement() {
    Token fz = tokens[currentTokenIndex];
    
    if (match(TokenType::LEFT_BRACE)) {
        consumeToken(); // Consume '{'
        
        // Here, parse multiple statements within the block and create an AST node for the block
        ASTNode* blockNode = new ASTNode(NodeType::BLOCK, "Block");
        while (!match(TokenType::RIGHT_BRACE)) {
            if(match(TokenType::SEMICOLON)) {
                consumeToken();
                continue;
            }
            
            Token f = tokens[currentTokenIndex];
            
            ASTNode* statement = parseStatement(); // Recursively parse individual statements
            
            if (statement) {
                blockNode->children.push_back(statement);
            } else {
                // Handle error or unrecognized statement
                // Skip token or perform error recovery logic
                currentTokenIndex++;
            }
            
            if(match(TokenType::SEMICOLON))
                consumeToken(); // Consume ';'
            
            Token fxz = tokens[currentTokenIndex];
            
            // Function ended?
            if(match(TokenType::RIGHT_BRACE)) { // '}'
                consumeToken(); // Consume '};
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
    
    Token tz = tokens[currentTokenIndex];
    
    // Warning.. junky code..
    if(match(TokenType::BREAK_STATEMENT)) {
        consumeToken(); // Consume statement
        return new ASTNode(NodeType::BREAK_STATEMENT, "Break statement");
    } else if(match(TokenType::CONTINUE_STATEMENT)) {
        consumeToken(); // Consume statement
        return new ASTNode(NodeType::CONTINUE_STATEMENT, "Continue statement");
    }/* else if(match(TokenType::RETURN_STATEMENT)) {
        consumeToken(); // Consume statement
        return new ASTNode(NodeType::RETURN_STATEMENT, "Return statement");
    }*/
    
    Token f = tokens[currentTokenIndex];
    
    if(match(TokenType::RIGHT_BRACE)) // '}'
        return new ASTNode(NodeType::FUNCTION_BODY, "EmptyFunctionBody");
    
    // Check if it's a function call
    ASTNode* functionCallNode = parseFunctionCall();
    if (functionCallNode) {
        return functionCallNode; // Return the function call node or handle it as needed
    }
    
    // Handle return statement:
    if(match(TokenType::KEYWORD)) {
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
    
    // Handle loops (for, while, do-while)
    // if (match(TokenType::KEYWORD)) {
    //     if (tokens[currentTokenIndex].value == "if") {
    //  
    //     } else if (tokens[currentTokenIndex].value == "else") {
    //     
    //     } else if (tokens[currentTokenIndex].value == "else if") {
    //      
    //     } else if(tokens[currentTokenIndex].value == "while") {
    //      
    //     } else if(tokens[currentTokenIndex].value == "do-while") {
    //         // Fuck you.
    //     } else if(tokens[currentTokenIndex].value == "delete") {
    //         consumeToken(); // Consume 'delete'
    //         
    //         return new ASTNode(NodeType::DELETE, "DELETE_STATEMENT: " + tokens[currentTokenIndex].value);
    //     } else {
    //         //std::cout << "BRO WTF IS " << tokens[currentTokenIndex].value << std::endl;
    //     }
    // }
    
    switch (tokens[currentTokenIndex].type) {
    case TokenType::FOR_LOOP:
        {
            // Idk if there are other types of "for loops"
            if (tokens[currentTokenIndex].value == "for") {
                /**
                 * Types of for loops:
                 * for(int i = 0; i < #; i++) {}
                 * for(int f : fs) {}
                 */
                consumeToken(); // Consume 'for'
                
                ASTNode* forLoopNode = new ASTNode(NodeType::FOR_LOOP, "ForLoop");
                
                if(!match(TokenType::LEFT_PAREN)) {
                    std::cerr << "Syntax Error: Missing ')' after 'for' in for loop initialization.\n";
                } else
                    consumeToken(); // Consume '('
                
                // Parse initialization
                ASTNode* initNode = parseMember(NodeType::MEMBER_VARIABLE); // Assuming you have a function to parse expressions
                forLoopNode->children.push_back(initNode);
                
                if(!match(TokenType::SEMICOLON)) {
                    std::cerr << "Syntax Error: Missing semicolon after '" << initNode->value << "' in for loop initialization.\n";
                } else
                    consumeToken(); // Consume ';'
                
                // Parse condition
                ASTNode* conditionNode = expression(); // Assuming you have a function to parse expressions
                forLoopNode->children.push_back(conditionNode);
                
                if(!match(TokenType::SEMICOLON)) {
                    std::cerr << "Syntax Error: Missing semicolon after '" << conditionNode->value << "' in for loop condition.\n";
                } else
                    consumeToken(); // Consume ';'
                
                // Parse iteration
                ASTNode* iterationNode = expression(); // Assuming you have a function to parse expressions
                forLoopNode->children.push_back(iterationNode);
                
                if(tokens[currentTokenIndex - 1].type != TokenType::RIGHT_PAREN) {
                    std::cerr << "Syntax Error: Missing ')' after '" << iterationNode->value << "' in for loop iteration.\n";
                }/* else
                    consumeToken(); // Consume ')'*/
                
                Token t = tokens[currentTokenIndex];
                
                ASTNode* body = parseStatement();
                forLoopNode->children.push_back(body);
                
                return forLoopNode;
            }
        
            break;
        }
    case TokenType::IF_STATEMENT:
        {
            consumeToken(); // Consume 'if'

            if(!match(TokenType::LEFT_PAREN)) {
                std::cerr << "Error: Missing '(' after if statement.\n";
            
                return nullptr;
            }
        
            consumeToken(); // '('
        
            Token t = tokens[currentTokenIndex];
        
            ASTNode* conditionNode = parseCondition(); // Parse the condition expression
            ASTNode* ifBodyNode = parseStatement(); // Parse the body of 'if'
        
            // Create an 'if' statement node and attach the condition and body
            ASTNode* ifStatementNode = new ASTNode(NodeType::IF_STATEMENT, "If Statement");
            ifStatementNode->children.push_back(conditionNode);
            ifStatementNode->children.push_back(ifBodyNode);

            // Should be at the end of the if statement.
            if(match(TokenType::RIGHT_BRACE)) {
                consumeToken(); // '}'
            }
            
            Token zzz = tokens[currentTokenIndex];
            
            return ifStatementNode;
        }
    case TokenType::ELSE_STATEMENT:
        {
            consumeToken(); // Consume 'else'
            
            ASTNode* elseBodyNode = parseStatement(); // Parse the body of 'else'
        
            // Create an 'else' statement node and attach the condition and body
            ASTNode* elseStatementNode = new ASTNode(NodeType::ELSE_STATEMENT, "Else Statement");
            elseStatementNode->children.push_back(elseBodyNode);
        
            // Should be at the end of the else statement.
            if(match(TokenType::RIGHT_BRACE)) {
                consumeToken(); // '}'
            }
        
            return elseStatementNode;
        }
    }
    
    Token hghg = tokens[currentTokenIndex];
    
    ASTNode* variableNote2 = parseMember(NodeType::LOCAL_VARIABLE_DECLARATION);
    
    return variableNote2;
    
    //TODO; Move all of thise inside 'parseMember'
    //TODO; Still is the main cause of every problem.. fu old me for being too lazy
    bool isVariable = false;
    
    size_t curIndex = currentTokenIndex;
    
    while(curIndex < tokens.size() && tokens[curIndex].type != TokenType::SEMICOLON) {
        Token t = tokens[curIndex];

        //TODO..
        if (tokens[curIndex].type == TokenType::OPERATOR/* && tokens[curIndex].value == "="
            || tokens[curIndex].type == TokenType::INCREMENT_EQUAL*/) {
            isVariable = true;
            
            break;
        }
        
        curIndex++;
    }
    
    if(!isVariable)
        return nullptr;
    
    // As, curIndex is '=', the previous token, should be the name.
    
    //int originalIndex = currentTokenIndex;
    //currentTokenIndex = curIndex - 2;
    
    /*ASTNode* variableNode = *///parseMember(NodeType::LOCAL_VARIABLE_DECLARATION);
    //currentTokenIndex = originalIndex;
    
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
    
    size_t typeIndex = currentTokenIndex;
    
    while (true) {
        if(tokens[typeIndex].type == TokenType::IDENTIFIER
            || tokens[typeIndex].type == TokenType::KEYWORD) {
            break;
        }
        
        typeIndex--;
    }
    
    variableNode->children.push_back(new ASTNode(NodeType::VARIABLE_TYPE, tokens[typeIndex - 1].value));
    
    //currentTokenIndex = originalIndex;
   Token t = tokens[currentTokenIndex];
    
    if (match(TokenType::IDENTIFIER)) {
        consumeToken(); // Consume variable name token
    }
    
    Token d = tokens[currentTokenIndex];
    
    /*if (match(TokenType::OPERATOR) && tokens[currentTokenIndex].value == "=") {
        consumeToken(); // Consume '=' token
        ASTNode* expressionNode = expression(); // Parse expression on the right-hand side of assignment
        
        variableNode->children.push_back(expressionNode);
    } else {*/
        ASTNode* compoundAssignNode = new ASTNode(NodeType::COMPOUND_ASSIGNMENT, tokens[currentTokenIndex].value);
        
        consumeToken(); // Consume '=' token
        ASTNode* expressionNode = expression(); // Parse expression on the right-hand side of assignment
        
        compoundAssignNode->children.push_back(expressionNode);
        variableNode->children.push_back(compoundAssignNode);
   // }
    
    return variableNode;
}

ASTNode* Parser::parseFunctionCall() {
    Token t = tokens[currentTokenIndex];
    
    if (match(TokenType::IDENTIFIER) && matchNext(TokenType::LEFT_PAREN)) {
        std::string functionName = tokens[currentTokenIndex].value;
        std::string objectName = tokens[currentTokenIndex - 2].value;
        consumeToken(); // Consume function name token
        
        if (match(TokenType::LEFT_PAREN) || matchNext(TokenType::LEFT_PAREN)) {
            consumeToken(); // Consume '('
            
            Token z = tokens[currentTokenIndex];
            
            // Parse function arguments (if any)
            // You might call another function to handle this
            std::vector<std::string> parameters = parseFunctionParameters();
            
            if (match(TokenType::RIGHT_PAREN)) {
                consumeToken(); // Consume ')'
                
                // Create an AST node for the function call
                ASTNode* functionCallNode = new ASTNode(NodeType::FUNCTION_CALL, "METHOD CALL: ");
                functionCallNode->children.push_back(new ASTNode(NodeType::FUNCTION_CALL, "OBJECT: " + objectName));
                functionCallNode->children.push_back(new ASTNode(NodeType::FUNCTION_CALL, "METHOD: " + functionName));
                // Attach argument nodes as children to the function call node
                
                for (int i = 0; !parameters.empty() && i < parameters.size() - 1; i += 2) {
                    std::string paramName = parameters[i];
                    std::string paramType = parameters[i + 1];
                    
                    ASTNode* parameterNode = new ASTNode(NodeType::PARAMETER, paramName);
                    parameterNode->children.push_back(new ASTNode(NodeType::VARIABLE_TYPE, paramType));
                    
                    functionCallNode->children.push_back(parameterNode);
                }
                
                return functionCallNode;
            }
        }/* else {
            currentTokenIndex--;
        }*/
    }
    
    // Handle other cases or return nullptr for invalid or unexpected tokens
    return nullptr;
}

ASTNode* Parser::parseCondition() {
    // Parse the condition
    ASTNode* condition = expression();

    if(match(TokenType::RIGHT_PAREN)) { // ')'
        consumeToken(); // Consume ')'
    } else {
        std::cerr << "Error: Missing ')' after if statement condition.\n";
    }
    
    // Check for '{' symbol
    if (!match(TokenType::LEFT_BRACE)) {
        throw std::runtime_error("Expected '{' after 'if' condition");
    }
    
    return condition;
}

ASTNode* Parser::parseVariable(NodeType memberDeclarationType, const std::string& memberType) {
    std::string memberName = tokens[currentTokenIndex].value;
    consumeToken(); // Consume member name
    
    if (match(TokenType::LEFT_PAREN)) { // '('
        return parseFunction(memberType, memberName);
    }
    
    //TODO; global variable..? Can't remember what I meant by this...
    ASTNode* memberVariable = new ASTNode(memberDeclarationType, memberName);
    memberVariable->children.push_back(new ASTNode(NodeType::VARIABLE_TYPE, memberType));
    
    // If variable has no assignment.
    if (match(TokenType::SEMICOLON)) {
        return memberVariable;
    }
    
    ASTNode* compoundAssignNode = new ASTNode(NodeType::COMPOUND_ASSIGNMENT, tokens[currentTokenIndex].value);

    //TODO; this is returing nullptr
    consumeToken(); // Consume operator token
    ASTNode* expressionNode = expression(); // Parse expression on the right-hand side of assignment
    
    compoundAssignNode->children.push_back(expressionNode);
    memberVariable->children.push_back(compoundAssignNode);
    
    return memberVariable;
}

ASTNode* Parser::parseReference(NodeType memberDeclarationType, const std::string& memberType) {
    NodeType pointerType;
    
    if(tokens[currentTokenIndex].type == TokenType::POINTER)
        pointerType = NodeType::POINTER;
    else
        pointerType = NodeType::REFERENCE;

    consumeToken(); // Consume pointer
    
    std::string memberName = tokens[currentTokenIndex].value;
    consumeToken(); // Consume member name
    
    ASTNode* memberReference = new ASTNode(memberDeclarationType, memberName);
    memberReference->children.push_back(new ASTNode(NodeType::VARIABLE_TYPE, memberType));
    memberReference->children.push_back(new ASTNode(pointerType, memberType));
    
    Token t = tokens[currentTokenIndex];
    
    // If variable has no assignment.
    if (match(TokenType::SEMICOLON)) {
        return memberReference;
    }
    
    ASTNode* compoundAssignNode = new ASTNode(NodeType::COMPOUND_ASSIGNMENT, tokens[currentTokenIndex].value);
    
    consumeToken(); // Consume operator token
    ASTNode* expressionNode = expression(); // Parse expression on the right-hand side of assignment
    
    compoundAssignNode->children.push_back(expressionNode);
    memberReference->children.push_back(compoundAssignNode);
    
    if(match(TokenType::RIGHT_PAREN))
        consumeToken(); // Consume ')'
    
    Token z = tokens[currentTokenIndex];
    
    return memberReference;
}

std::vector<std::string> Parser::parseFunctionParameters() {
    std::vector<std::string> parameters;
    
    Token t = tokens[currentTokenIndex];
    
    while (!match(TokenType::RIGHT_PAREN)) {
        Token t = tokens[currentTokenIndex];
        
        if(match(TokenType::COMMA))
            consumeToken();
        
        /*std::string test = "";
        while(!match(TokenType::COMMA)) {
            if(match(TokenType::RIGHT_PAREN))
                break;
            
            //null
            ASTNode* node = parseMember(NodeType::PARAMETER_VARIABLE);
            test = node->children[0]->value;
        }*/
        
        //if (match(TokenType::KEYWORD)) {
            std::string paramType = tokens[currentTokenIndex].value;
            consumeToken(); // Consume parameter type token
            
            if (match(TokenType::IDENTIFIER)) {
                std::string paramName = tokens[currentTokenIndex].value;
                //parameters.push_back(paramType + " " + paramName);
                parameters.push_back(paramName);
                parameters.push_back(paramType);
                
                consumeToken(); // Consume parameter name token
                
                if (!match(TokenType::COMMA)) {
                    break; // Last parameter, exit loop
                }
                
                consumeToken(); // Consume ',' before the next parameter
            } else {
                // Handle syntax errors in parameter declaration
                std::cout << "ERROR; Seems to have a syntax error in the parameter declaration at: " << currentTokenIndex << "\n"; 
                //return nullptr;
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
    
    ASTNode* left = term();
    
    while (currentTokenIndex < tokens.size() - 2 && tokens[currentTokenIndex].type == TokenType::OPERATOR &&
           (tokens[currentTokenIndex].value == "+" || tokens[currentTokenIndex].value == "-") ||
           (tokens[currentTokenIndex].value == "<" || tokens[currentTokenIndex].value == ">") ||
           match(TokenType::OPERATOR) ||
           /*(match(TokenType::INCREMENT) || match(TokenType::DECREMENT)) ||
           (match(TokenType::INCREMENT_EQUAL) || match(TokenType::DECREMENT_EQUAL)) ||*/
           (match(TokenType::LESS_THAN_EQUAL) || match(TokenType::GREATER_THAN_EQUAL)) ||
           (match(TokenType::EQUAL) || match(TokenType::NOT_EQUAL))) {

        
        
        std::string op = tokens[currentTokenIndex++].value;
        
        ASTNode* right = term();
        ASTNode* newOpNode = new ASTNode(NodeType::OPERATOR, op);
        newOpNode->children.push_back(left);
        newOpNode->children.push_back(right);
        left = newOpNode;
    }
    
    return left;
}

ASTNode* Parser::term() {
    std::string assignmentType = "";
    std::vector<std::string> parameters;
    
    if(match(TokenType::KEYWORD, "new")) {
        assignmentType = "new";
        consumeToken();
    }
    
    std::string type = tokens[currentTokenIndex].value;
    ASTNode* left = new ASTNode(NodeType::ASSIGNMENT, assignmentType + "; " + type);
    Token t = tokens[currentTokenIndex];
    
    // Points to a class, perhaps?
    if(match(TokenType::IDENTIFIER) && matchNext(TokenType::LEFT_PAREN)) {
        consumeToken(); // Consume type
        
        if(match(TokenType::SEMICOLON)) {
            return left;
        }
        
        consumeToken(); // Consume '('
        
        while (!match(TokenType::RIGHT_PAREN)) { // ')'
            Token tz = tokens[currentTokenIndex];
            
            if (match(TokenType::COMMA)) {
                //Skip
                consumeToken(); // Consume ','
                
                continue;
            }
            
            if (match(TokenType::SEMICOLON)) {
                return left;
            }
            
            parameters.push_back(tokens[currentTokenIndex].value);
            left->children.push_back(new ASTNode(NodeType::PARAMETER, tokens[currentTokenIndex].value));
        
            consumeToken(); // Consume parameter
        }
        
        return left;
    }
    
    left = factor();
    
    while (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type == TokenType::OPERATOR &&
           (tokens[currentTokenIndex].value == "*" || tokens[currentTokenIndex].value == "/")) {
        
        std::string op = tokens[currentTokenIndex++].value;
        
        ASTNode* right = factor();
        ASTNode* newOpNode = new ASTNode(NodeType::OPERATOR, op);
        newOpNode->children.push_back(left);
        newOpNode->children.push_back(right);
        left = newOpNode;
    }
    
    return left;
}

ASTNode* Parser::factor() {
    Token currentToken = tokens[currentTokenIndex++];
    
    if (currentToken.type == TokenType::INTEGER_LITERAL ||
        currentToken.type == TokenType::FLOATING_POINT_LITERAL ||
        currentToken.type == TokenType::IDENTIFIER ||
        currentToken.type == TokenType::KEYWORD) {
        return new ASTNode(NodeType::EXPRESSION, currentToken.value); // Creating a generic expression node
    }
    
    std::cout << "huh " << currentToken.value << '\n';
    
    // TODO; Handle parentheses or other factors
    
    return nullptr;
}
