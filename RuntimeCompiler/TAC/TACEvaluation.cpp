#include "TACEvaluation.h"

#include <iostream>
#include <sstream>

// Not made by me, just copied but I am never using it anyways.
void TACEvaluation::interpretTAC(const std::vector<std::string>& tac) {
    for (const auto &instruction : tac) {
        executeInstruction(instruction);
    }
    
    // Iterate over the TAC instructions
    for (const auto &instruction : tac) {
        // Split the instruction into tokens
        std::vector<std::string> tokens = split(instruction, ' ');
        
        // Check if the instruction is an assignment
        if (tokens.size() > 1 && tokens[1] == "=") {
            // Evaluate the right-hand side of the assignment
            std::string expression = instruction.substr(instruction.find('=') + 1);
            double value = calculate<double>(expression);
            
            // Store the result in the map
            variables[tokens[0]] = value;
        }
    }
    
    // Print the final variable values
    for (const auto &pair : variables) {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }
}

std::vector<std::string> TACEvaluation::split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);

    while(std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

void TACEvaluation::executeInstruction(const std::string& instruction) {
    std::vector<std::string> tokens = split(instruction, ' ');

    if(tokens.size() <= 2)
        return;
    
    std::string var = tokens[0];
    std::string op = tokens[1];
    std::string val1 = tokens[2];

    if(tokens.size() <= 3) {
        variables[var] = variables[val1];

        return;
    }
    
    std::string val2 = tokens[3];
    
    // Perform the operation and store the result
    if (op == "+") {
        variables[var] = variables[val1] + variables[val2];
    } else if (op == "-") {
        variables[var] = variables[val1] - variables[val2];
    } else if (op == "*") {
        variables[var] = variables[val1] * variables[val2];
    } else if (op == "/") {
        variables[var] = variables[val1] / variables[val2];
    } else if (op == "=") {
        try {
            variables[var] = std::stod(val1);
        } catch (...) {
            std::cout << "Invalid number\n";
        }
    }
}