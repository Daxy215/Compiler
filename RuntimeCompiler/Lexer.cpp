#include "Lexer.h"

#include <iostream>
#include <map>

std::map<std::string, TokenType> multiCharOps =
    {{"<<", TokenType::OPERATOR}, // Can also be a bit shift operator
    {">>", TokenType::OPERATOR}, // Can also be a bit shift operator
    {"::", TokenType::OPERATOR},
    {"->", TokenType::OPERATOR},
    {"++", TokenType::OPERATOR},
    {"--", TokenType::OPERATOR},
    {"+=", TokenType::OPERATOR},
    {"-=", TokenType::OPERATOR},
    {"*=", TokenType::OPERATOR},
    {"/=", TokenType::OPERATOR},
    {"&&", TokenType::LOGICAL_AND},
    {"||", TokenType::LOGICAL_OR},
    {"==", TokenType::EQUAL}, //OPERATOR?
    {"!=", TokenType::NOT_EQUAL},
    {"<=", TokenType::LESS_THAN_EQUAL},
    {">=", TokenType::GREATER_THAN_EQUAL},
    {"<<=", TokenType::LEFT_SHIFT_ASSIGN},
    {">>=", TokenType::RIGHT_SHIFT_ASSIGN},
    {"->*", TokenType::POINTER_MEMBER_ACCESS},
    {"::*", TokenType::SCOPE_POINTER_MEMBER_ACCESS},
    {"...", TokenType::ELLIPSIS},
    {"//", TokenType::SINGLE_LINE_COMMENT},
    {"/*", TokenType::MULTI_LINE_COMMENT_START},
    {"*/", TokenType::MULTI_LINE_COMMENT_END},
    {"%=", TokenType::MODULUS_ASSIGN},
    {"^=", TokenType::BITWISE_XOR_ASSIGN},
    {"|=", TokenType::BITWISE_OR_ASSIGN},
    {"&=", TokenType::BITWISE_AND_ASSIGN},
    {"^=", TokenType::BITWISE_XOR},
    {"&=", TokenType::BITWISE_AND},
    {"|=", TokenType::BITWISE_OR},
    {"%=", TokenType::MODULUS},
    {"~=", TokenType::BITWISE_NOT},
    };

std::vector<Token> Lexer::generateTokens(std::string code) {
    for(size_t i = 0; i < code.size(); ++i) {
        char c = code[i];
        
        if(c == ' ')
            continue;
        
        if(!tokens.empty())
            prevToken = tokens.back();
        
        if (i + 1 < code.size() && multiCharOps.count(std::string(1, c) + code[i + 1])) {
            std::string type = std::string(1, c) + code[i + 1];
            
            tokens.push_back({multiCharOps[type], type});
            i++;
            
            continue;
        }
        
        switch(c) {
        case '(':
            pushToken(c, TokenType::LEFT_PAREN);
            break;
        case ')':
            pushToken(c, TokenType::RIGHT_PAREN);
            break;
        case '{':
            pushToken(c, TokenType::LEFT_BRACE);
            break;
        case '}':
            pushToken(c, TokenType::RIGHT_BRACE);
            break;
        case '[':
            pushToken(c, TokenType::LEFT_SQUARE_BRACE);
            break;
        case ']':
            pushToken(c, TokenType::RIGHT_SQUARE_BRACE);
            break;
        case '<':
            pushToken(c, TokenType::LEFT_ANGLE_BRACE);
            break;
        case '>':
            pushToken(c, TokenType::RIGHT_ANGLE_BRACE);
            break;
        case ';':
            pushToken(c, TokenType::SEMICOLON);
            break;
        case ',':
            pushToken(c, TokenType::COMMA);
            break;
        case ':':
            pushToken(c, TokenType::COLON);
            break;
        case '.':
            pushToken(c, TokenType::DOT);
            break;
        case '+': case '-': case '/': case '=':
            pushToken(c, TokenType::OPERATOR);
            break;
        case '*': case '&':
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
            
            if (hasDecimal) {
                //TODO; Handle missing ';' after "float i = 5.0f"
                if(code[i] == 'f') {
                    literal += code[i++];
                }
                
                tokens.push_back({TokenType::FLOATING_POINT_LITERAL, literal});
            } else {
                tokens.push_back({TokenType::INTEGER_LITERAL, literal});
            }

            //???
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
            
            // TODO; Use a map instead.
            
            if (keywords.find(currentToken) != keywords.end()) {
                type = TokenType::KEYWORD;
            } else if(currentToken == "include") {
                type = TokenType::PREPROCESSOR_DIRECTIVE;
                
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
                type = TokenType::NAMESPACE;
                
                while (i + 1 < code.size() && code[i + 1] != ';') {
                    currentToken += code[++i];
                }
                
                currentToken = extractNamespaceName(currentToken);
                i++; // Skip ';'
            } else if (currentToken == "struct") {
                type = TokenType::STRUCT;
            } else if (currentToken == "class") {
                type = TokenType::CLASS;
            } else if (currentToken == "enum") {
                type = TokenType::ENUM;
            } else if(currentToken == "for") {
                type = TokenType::FOR_LOOP;
            } else if(currentToken == "while") {
                type = TokenType::WHILE_LOOP;
            } else if(currentToken == "do") {
                type = TokenType::DO_WHILE_LOOP;
            } else if(currentToken == "if") {
                type = TokenType::IF_STATEMENT;
            } else if(currentToken == "else") {
                type = TokenType::ELSE_STATEMENT;
            } else if(currentToken == "return") {
                type = TokenType::RETURN_STATEMENT;
            } else if(currentToken == "break") {
                type = TokenType::BREAK_STATEMENT;
            } else if(currentToken == "continue") {
                type = TokenType::CONTINUE_STATEMENT;
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

void Lexer::pushToken(char c, TokenType type) {
    tokens.push_back({type, std::string(1, c)});
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
