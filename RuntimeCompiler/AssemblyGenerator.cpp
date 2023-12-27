#include "AssemblyGenerator.h"

#include <fstream>
#include <iostream>
#include <sstream>

void AssemblyGenerator::generateAssembly(std::string fileName, std::vector<std::string>& tac) {
    std::ofstream outputFile(fileName);
    if (!outputFile.is_open()) {
        std::cerr << "ERROR: Unable to open file for writing!" << '\n';
        return;
    }
    
    // Variables
    /*outputFile << "section .data" << '\n';
    for (const auto& instruction : tac) {
        std::string result, op, arg1, arg2;
        std::istringstream iss(instruction);
        iss >> result >> op >> arg1 >> arg2;
        
        if(!arg1.find('t'))
            arg1 = "0";
        
        outputFile << "\t" << result << " dd " << arg1 << '\n';
    }
    
    // Define main function
    outputFile << "\nglobal _start\n";
    
    // Tell program to start code:
    outputFile << "\nsection .text\n";
    
    // Main function
    outputFile << "_start:" << '\n';
    
    for (const auto& instruction : tac) {
        std::string result, op, arg1, arg2;
        std::istringstream iss(instruction);
        iss >> result >> op >> arg1 >> arg2;
        
        if (op == "=") {
            outputFile << "\tmov eax, " << arg1 << '\n';
            outputFile << "\tmov [" << result << "], eax" << '\n';
        } else if (op == "+") {
            outputFile << "\tmov eax, [" << arg1 << "]" << '\n';
            outputFile << "\tadd eax, [" << arg2 << "]" << '\n';
            outputFile << "\tmov [" << result << "], eax" << '\n';
        } else if (op == "*") {
            outputFile << "\tmov eax, [" << arg1 << "]" << '\n';
            outputFile << "\timul eax, [" << arg2 << "]" << '\n';
            outputFile << "\tmov [" << result << "], eax" << '\n';
        }
    }
    
    // Exit the process
    outputFile << "\tpush 0" << '\n';
    outputFile << "\tint 0x80" << '\n';*/
    
    outputFile.close();
}
