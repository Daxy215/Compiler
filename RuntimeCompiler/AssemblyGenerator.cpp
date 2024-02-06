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

bool isNumber(const std::string& str) {
    for (char const &c : str) {
        if (!std::isdigit(c)) return false;
    }
    return !str.empty();
}

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
        } else {
            outputFile << "\n; couldn't find command: " << ir->command << "\n\n";
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

/*
void AssemblyGenerator::generateAssembly(std::string fileName) {
    std::ofstream outputFile(fileName);
    if (!outputFile.is_open()) {
        std::cerr << "ERROR: Unable to open file for writing!" << '\n';
        return;
    }

    // for (auto &node : IRNode) {
    //     auto &op = node.;
    //     auto &arg = node.second;
    //
    //     if (op == "ALLOC" || op == "ASSIGN") {
    //         std::cout << opMap[op] << " " << arg << "\n";
    //     } else if (op == "INT") {
    //         std::cout << "mov " << arg << ", eax\n";
    //     } else {
    //         std::cout << opMap[op] << " eax, ebx\n";
    //     }
    // }
    
    // Variables
    /*outputFile << "section .data" << '\n';
    for (const auto& instruction : tac) {
        std::string varName, op, arg1, arg2;
        std::istringstream iss(instruction);
        iss >> varName >> op >> arg1 >> arg2;
        
        // Handle string assignment
        //TODO;
        if (op == "=" && arg2[0] == 'w') {
            std::string labelName = varName.substr(1); // Remove leading 't'
            std::vector<std::string> variable = splitString(instruction, '"');
            
            outputFile << "\t" << varName << " db '" << variable[1] << "', " << labelName << "\n";
            
            continue;
        }
        
        if(arg2._Equal("DEFINE"))
            continue;
        
        if(!arg1.find('t')) {
            //arg1 = "0";
            continue;
        }
        
        outputFile << "\t" << varName << " dd " << arg1 << '\n';
    }
    
    // Tell program to start code:
    outputFile << "\nsection .text\n";
    
    // Define main function
    outputFile << "\tglobal main\n";
    outputFile << "\textern puts\n";
    
    // Main function
    outputFile << "main:" << '\n';
    
    for (const auto& instruction : tac) {
        std::string varName, op, arg1, arg2;
        std::istringstream iss(instruction);
        iss >> varName >> op >> arg1 >> arg2;
        
        if(varName._Equal("GLOBAL::Function:"))
            continue;
        
        // Print
        /*outputFile << std::format(R"(
        sub rsp, 28h                      ; Reserve the shadow space
        mov rcx, {}                      ; First argument is address of message
        call puts                          ; puts(message)
        add rsp, 28h                      ; Remove shadow space)", varName);*//*
        
        // if (op == "=") {
        //     outputFile << "\tmov eax, " << arg1 << '\n';
        //     outputFile << "\tmov [" << varName << "], eax" << '\n';
        // } else if (op == "+") {
        //     outputFile << "\tmov eax, [" << arg1 << "]" << '\n';
        //     outputFile << "\tadd eax, [" << arg2 << "]" << '\n';
        //     outputFile << "\tmov [" << varName << "], eax" << '\n';
        // } else if (op == "*") {
        //     outputFile << "\tmov eax, [" << arg1 << "]" << '\n';
        //     outputFile << "\timul eax, [" << arg2 << "]" << '\n';
        //     outputFile << "\tmov [" << varName << "], eax" << '\n';
        // }
    }*//*
    
    // Exit the process
    //outputFile << "\n\tret" << '\n';
    
    outputFile.close();
}*/