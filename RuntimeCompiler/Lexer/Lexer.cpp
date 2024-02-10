#include "Lexer.h"

#include <iostream>
#include <map>

std::map<std::string, LexerNameSpace::TokenType> multiCharOps =
    {{"<<", LexerNameSpace::TokenType::OPERATOR}, // Can also be a bit shift operator
    //{">>", LexerNameSpace::TokenType::OPERATOR}, // Can also be a bit shift operator
    {"::", LexerNameSpace::TokenType::OPERATOR},
    {"->", LexerNameSpace::TokenType::OPERATOR},
    {"++", LexerNameSpace::TokenType::OPERATOR},
    {"--", LexerNameSpace::TokenType::OPERATOR},
    {"+=", LexerNameSpace::TokenType::OPERATOR},
    {"-=", LexerNameSpace::TokenType::OPERATOR},
    {"*=", LexerNameSpace::TokenType::OPERATOR},
    {"/=", LexerNameSpace::TokenType::OPERATOR},
    {"&&", LexerNameSpace::TokenType::LOGICAL_AND},
    {"||", LexerNameSpace::TokenType::LOGICAL_OR},
    {"==", LexerNameSpace::TokenType::EQUAL}, //OPERATOR?
    {"!=", LexerNameSpace::TokenType::NOT_EQUAL},
    {"<=", LexerNameSpace::TokenType::LESS_THAN_EQUAL},
    {">=", LexerNameSpace::TokenType::GREATER_THAN_EQUAL},
    {"<<=", LexerNameSpace::TokenType::LEFT_SHIFT_ASSIGN},
    {">>=", LexerNameSpace::TokenType::RIGHT_SHIFT_ASSIGN},
    {"->*", LexerNameSpace::TokenType::POINTER_MEMBER_ACCESS},
    {"::*", LexerNameSpace::TokenType::SCOPE_POINTER_MEMBER_ACCESS},
    {"...", LexerNameSpace::TokenType::ELLIPSIS},
    {"//", LexerNameSpace::TokenType::SINGLE_LINE_COMMENT},
    {"/*", LexerNameSpace::TokenType::MULTI_LINE_COMMENT_START},
    {"*/", LexerNameSpace::TokenType::MULTI_LINE_COMMENT_END},
    {"%=", LexerNameSpace::TokenType::MODULUS_ASSIGN},
    {"^=", LexerNameSpace::TokenType::BITWISE_XOR_ASSIGN},
    {"|=", LexerNameSpace::TokenType::BITWISE_OR_ASSIGN},
    {"&=", LexerNameSpace::TokenType::BITWISE_AND_ASSIGN},
    {"^=", LexerNameSpace::TokenType::BITWISE_XOR},
    {"&=", LexerNameSpace::TokenType::BITWISE_AND},
    {"|=", LexerNameSpace::TokenType::BITWISE_OR},
    {"%=", LexerNameSpace::TokenType::MODULUS},
    {"~=", LexerNameSpace::TokenType::BITWISE_NOT},
    };

std::vector<Token> Lexer::generateTokens(std::string code) {
    this->code = code;
    
    for(size_t i = 0; i < code.size(); ++i) {
        char c = code[i];
        
        if(c == ' ') {
            currentPos++;
            continue;
        }
        
        if(c == '\n') {
            currentLine++;
            continue;
        }
        
        if(!tokens.empty())
            prevToken = tokens.back();
        
        if (i + 1 < code.size() && multiCharOps.count(std::string(1, c) + code[i + 1])) {
            std::string type = std::string(1, c) + code[i + 1];
            
            tokens.push_back({multiCharOps[type], type, currentPos, currentPos + 1, currentLine, currentColumn});
            currentPos++;
            
            i++;
            
            continue;
        }
        
        switch(c) {
        case '(':
            pushToken(c, LexerNameSpace::TokenType::LEFT_PAREN);
            break;
        case ')':
            pushToken(c, LexerNameSpace::TokenType::RIGHT_PAREN);
            break;
        case '{':
            pushToken(c, LexerNameSpace::TokenType::LEFT_BRACE);
            break;
        case '}':
            pushToken(c, LexerNameSpace::TokenType::RIGHT_BRACE);
            break;
        case '[':
            pushToken(c, LexerNameSpace::TokenType::LEFT_SQUARE_BRACE);
            break;
        case ']':
            pushToken(c, LexerNameSpace::TokenType::RIGHT_SQUARE_BRACE);
            break;
        case '<':
            pushToken(c, LexerNameSpace::TokenType::LEFT_ANGLE_BRACE);
            break;
        case '>':
            pushToken(c, LexerNameSpace::TokenType::RIGHT_ANGLE_BRACE);
            break;
        case ';':
            pushToken(c, LexerNameSpace::TokenType::SEMICOLON);
            break;
        case ',':
            pushToken(c, LexerNameSpace::TokenType::COMMA);
            break;
        case ':':
            pushToken(c, LexerNameSpace::TokenType::COLON);
            break;
        case '.':
            pushToken(c, LexerNameSpace::TokenType::DOT);
            continue;
        case '+': case '-': case '/': case '=':
            pushToken(c, LexerNameSpace::TokenType::OPERATOR);
            break;
        case '*': case '&':
            LexerNameSpace::TokenType type;
            
            if (prevToken.type == LexerNameSpace::TokenType::OPERATOR || prevToken.type == LexerNameSpace::TokenType::REFERENCE ||
                prevToken.type == LexerNameSpace::TokenType::POINTER || prevToken.type == LexerNameSpace::TokenType::STRUCT ||
                prevToken.type == LexerNameSpace::TokenType::CLASS || prevToken.type == LexerNameSpace::TokenType::IDENTIFIER) {
                std::string s = code.substr(currentPos, (currentPos + 1) - currentPos);
                
                if(c == '*')
                    type = LexerNameSpace::TokenType::POINTER;
                else if(c == '&')
                    type = LexerNameSpace::TokenType::REFERENCE;
            } else
                type = LexerNameSpace::TokenType::OPERATOR;
            
            tokens.push_back({type, std::string(1, c), currentPos, currentPos + 1, currentLine, currentColumn});
            
            break;
        default:
            //std::cout << "Error: Unknown token: " << c << "\n";
            
            break;
        }
        
        if (std::isdigit(c) || c == '.') {
            std::string literal;
            bool hasDecimal = false;
            
            while (std::isdigit(code[i]) || code[i] == '.') {
                if (code[i] == '.') {
                    if (hasDecimal) {
                        std::cout << "Error: Multiple decimal points in the number" << "\n";
                        
                        break;
                    }
                    
                    hasDecimal = true;
                }
                
                literal += code[i++];
            }
            
            LexerNameSpace::TokenType type;
            
            if (hasDecimal) {
                //TODO; Handle missing ';' after "float i = 5.0f"
                if(code[i] == 'f') {
                    literal += code[i++];
                }

                type = LexerNameSpace::TokenType::FLOATING_POINT_LITERAL;
            } else {
                type = LexerNameSpace::TokenType::INTEGER_LITERAL;
            }
            
            std::string s = code.substr(currentPos, (currentPos + literal.length()) - currentPos);
            tokens.push_back({type, literal, currentPos, currentPos + literal.length(), currentLine, currentColumn});
            currentPos += literal.length();
            
            //???
            --i;
            continue;
        } else if(c == '"') { //String
            i++;
            
            std::string literal;
            
            while(code[i] != '"') {
                literal += code[i++];
            }
            
            tokens.push_back({LexerNameSpace::TokenType::STRING_LITERAL,
                literal, currentPos, currentPos + literal.length(), currentLine, currentColumn});
            currentPos += literal.length();
            
            continue;
        }
        
        while (i < code.size() && (std::isalnum(c) || c == '_')) {
            currentToken += c;
            
            if (i + 1 >= code.size() || !(std::isalnum(code[i + 1]) || code[i + 1] == '_')) {
                break;
            }
            
            if (c == '\n') {
                currentLine++;
                currentColumn = 0;
            } else {
                //currentColumn++;
            }
            
            ++i;
            c = code[i];
        }
        
        if (!currentToken.empty()) {
            LexerNameSpace::TokenType type;
            
            // TODO; Use a map instead.
            
            if (keywords.find(currentToken) != keywords.end()) {
                type = LexerNameSpace::TokenType::KEYWORD;
            } else if(currentToken == "include") {
                type = LexerNameSpace::TokenType::PREPROCESSOR_DIRECTIVE;
                
                while (i + 1 < code.size() && code[i + 1] != '>') {
                    currentToken += code[++i];
                }
                
                if (i < code.size()) {
                    i++;
                    currentToken += '>'; // Include the '>'
                } else {
                    std::cerr << "Error: Unclosed include directive\n";
                    
                    return tokens;
                }
            } else if (currentToken == "using") {
                type = LexerNameSpace::TokenType::NAMESPACE;
                
                while (i + 1 < code.size() && code[i + 1] != ';') {
                    currentToken += code[++i];
                }
                
                currentToken = extractNamespaceName(currentToken);
                i++; // Skip ';'
            } else if (currentToken == "struct") {
                type = LexerNameSpace::TokenType::STRUCT;
            } else if (currentToken == "class") {
                type = LexerNameSpace::TokenType::CLASS;
            } else if (currentToken == "enum") {
                type = LexerNameSpace::TokenType::ENUM;
            } else if(currentToken == "for") {
                type = LexerNameSpace::TokenType::FOR_LOOP;
            } else if(currentToken == "while") {
                type = LexerNameSpace::TokenType::WHILE_LOOP;
            } else if(currentToken == "do") {
                type = LexerNameSpace::TokenType::DO_WHILE_LOOP;
            } else if(currentToken == "if") {
                type = LexerNameSpace::TokenType::IF_STATEMENT;
            } else if(currentToken == "else") {
                type = LexerNameSpace::TokenType::ELSE_STATEMENT;
            } else if(currentToken == "switch") {
              type = LexerNameSpace::TokenType::SWITCH_STATEMENT;  
            } else if(currentToken == "return") {
                type = LexerNameSpace::TokenType::RETURN_STATEMENT;
            } else if(currentToken == "break") {
                type = LexerNameSpace::TokenType::BREAK_STATEMENT;
            } else if(currentToken == "continue") {
                type = LexerNameSpace::TokenType::CONTINUE_STATEMENT;
            } else {
                type = LexerNameSpace::TokenType::IDENTIFIER;
            }
            
            size_t startPos = i - currentToken.length();
            // + 1 because .length starts from '0'
            size_t endPos = startPos + currentToken.length() + 1;
            size_t line = currentLine;
            size_t column = currentColumn;
            //std::string some = code.substr(startPos, endPos - startPos);
            tokens.push_back({type, currentToken, startPos, endPos, line, column});
            currentPos = endPos;
            currentToken.clear();
        }
    }
    
    if (!currentToken.empty()) {
        std::cout << "currentToken is NOT empty.\n";
    }
    
    return tokens;
}

void Lexer::pushToken(char c, LexerNameSpace::TokenType type) {
    size_t startPos = currentPos;
    size_t endPos = currentPos + 1;
    size_t line = currentLine;
    size_t column = currentColumn;
    //std::string s = code.substr(startPos, endPos - startPos);
    tokens.push_back({type, std::string(1, c), startPos, endPos, line, column});
    currentPos = endPos;
    
    if (c == '\n') {
        currentLine++;
        currentColumn = 0;
    } else {
        currentColumn++;
    }
}

std::string Lexer::extractNamespaceName(const std::string& input) {
    const std::string usingNamespacePrefix = "using namespace ";
    const size_t prefixLength = usingNamespacePrefix.length();
    
    if (input.substr(0, prefixLength) == usingNamespacePrefix) {
        std::string namespaceName = input.substr(prefixLength);
        
        // Remove any leading/trailing whitespace
        namespaceName.erase(
            namespaceName.begin(),
            std::find_if(namespaceName.begin(), namespaceName.end(), [](unsigned char ch)
            {
                return !std::isspace(ch);
            })
        );

        namespaceName.erase(
            std::find_if(namespaceName.rbegin(), namespaceName.rend(), [](unsigned char ch)
            {
                return !std::isspace(ch);
            }).base(),
            namespaceName.end()
        );
        
        return namespaceName;
    }

    // Nothing was found
    return "";
}
