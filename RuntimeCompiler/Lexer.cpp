#include "Lexer.h"

#include <iostream>

Lexer::Lexer() {
    
}

std::vector<Token> Lexer::generateTokens(std::string code) {
    for(size_t i = 0; i < code.size(); ++i) {
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

std::string Lexer::extractNamespaceName(const std::string& input) {
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
    }

    // Nothing was found
    return "";
}
