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
    outputFile << "section .data" << '\n';
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
    outputFile << "\tint 0x80" << '\n';

    /*
     * int sum = 0;
     * for(int i = 0; i < 10; i++) {
     *  sum += i;
     * }
     *
     * Gives:
     *
     * mov ecx, 0 ; initialize counter i to 0
     * mov esi, 0 ; initialize sum to 0
     * loop_start:
     * cmp ecx, 10 ; compare i with 10
     * jge loop_end ; jump to loop_end if i >= 10
     * add esi, ecx ; add i to sum
     * inc ecx ; increment i
     * jmp loop_start ; jump back to the start of the loop
     * loop_end:
     * ; esi now contains the sum
     */
    
    outputFile.close();
}
