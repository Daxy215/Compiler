#include "AssemblyGenerator.h"

#include <cstdlib> // For system function
#include <Windows.h>

#include <fstream>
#include <thread>

/*
     * 64 OPERATIONS ignore this
     * RAX is a 64 bits register
     *
     * 32 OPERATIONS
     *  EAX is a 32 bits register that is often, used for calculations.
     *  ESP is a 32 bits stack pointer register, used for storing values.
     *
     *  int y = c;
     *  mov eax, [c]       ; Load the value of 'c' into eax
     *  mov [y], eax       ; Store the value of 'eax' into the variable 'y'
     */

std::string mainFunction = "_main";
std::string currentFunction = "";

void AssemblyGenerator::generateCode(const std::vector<IR*>& instructions) {
    // Create file
    std::ofstream outputFile("Compiling/main.asm");

    outputFile << "global _main\n\n";
    
    outputFile << "section .bss\n";
    
    for (const IR* ir : instructions) {
        if (ir->command == "ALLOC") {
            outputFile << "\t" << ir->temp2 << ": resd " << ir->temp1 << "\n";
        }
    }
    
    outputFile << "\nsection .data\n";
    
    // There shouldn't be any data.
    
    outputFile << "\nsection .text\n";
    
    for (const IR* ir : instructions) {
        if (ir->command == "ALLOC") {
            
        } else if (ir->command == "STORE") {
            // int x = 5;
            outputFile << "\t; Store '" << ir->temp1 << "' into '" << ir->temp2 << "'\n";
            
            if(isNumber(ir->temp1)) {
                outputFile << "\tmov eax, " << ir->temp1 << "\n";
            } else {
                // Check if NOT it's a function
                if(std::find(functions.begin(), functions.end(), ir->temp1) == functions.end())
                    outputFile << "\tmov eax, [" << ir->temp1 << "]\n";
            }
            
            outputFile << "\tmov [" << ir->temp2 << "], eax\n\n";
        } else if (ir->command == "IF_STATEMENT") {
            // if true go to true branch
            outputFile << "\tje START" << ir->temp3 << "\n";
            // If false go to false branch
            outputFile << "\tjmp END" << ir->temp3 << "\n\n";
        } else if (ir->command == "LABEL") {
            outputFile << ir->temp1 << ":\n";
        } else if (ir->command == "FUNCTION") {
            std::string functionName = ir->temp1;
            
            if(functionName == "main")
                functionName = mainFunction;
            
            currentFunction = functionName;
            outputFile << functionName << ":\n";
        } else if (ir->command == "FUNCTION_CALL") {
            functions.push_back(ir->temp1);
            outputFile << "\tcall " << ir->temp1 << "\n\n";
        } else if (ir->command == "RETURN") {
            if(currentFunction == mainFunction) {
                outputFile << "\n\t; Exit Progam\n";
                outputFile << "\tmov eax, [" << ir->temp1 << "]\n";
                outputFile << "\tadd esp, 4\t; clear the stack\n";
                outputFile << "\tret\t\t; return\n";
            } else {
                outputFile << "\tmov eax, [" << ir->temp1 << "]\n";
                outputFile << "\tret\n";
            }
        } else if (ir->command == "==") {
            outputFile << "\tmov eax, [" << ir->temp1 << "]\n";
            outputFile << "\tcmp [" << ir->temp2 << "], eax\n\n";
        } else if(ir->command == "+" || ir->command == "-" ||
            ir->command == "*" || ir->command == "/") {
            /*
             * {command="+", temp1="temp4", temp2="temp5", ...}
             *
             * mov eax, 5        ; Operand 1
             * mov ebx, 3        ; Operand 2
             * add ebx           ; Add eax by ebx
             */
            
            std::string temp1 = getVariable(ir->temp1);
            std::string temp2 = getVariable(ir->temp2);
            std::string temp3 = getVariable(ir->temp3);
            
            outputFile << "\t; Add " << temp1 << " by " << temp2 << "\n";
            outputFile << "\tmov eax, " << temp1 << "\n";
            outputFile << "\tmov ebx, " << temp2 << "\n";

            if(ir->command == "+")
                outputFile << "\tadd eax, ebx\t\t; Add ebx to eax\n";
            else if(ir->command == "-")
                outputFile << "\tsub eax, ebx\t\t; Subtract eax by ebx\n";
            else if(ir->command == "*")
                outputFile << "\tmul ebx\t\t; Multiply eax by ebx\n";
            else if(ir->command == "/") {
                outputFile << "\txor edx, edx\t\t; Clear the high-order bits in edx\n";
                outputFile << "\tdiv ebx\t\t; Divide ebx by eax\n";
            }
            
            outputFile << "\n\t; Store results in " << temp3 << "\n";
            outputFile << "\tmov " << temp3 << ", eax\n\n";
        }else {
            std::cout << "\n\t; couldn't find command: " << ir->command << "\n\n";
        }
    }

    outputFile.close();

    // Compile
    int results = system("cd Compiling && build.bat");

    if(results == 0) {
        std::cout << "Compiled successfully!\n";
    } else {
        std::cerr << "Error while compiling: " << results << " \n";
    }
}