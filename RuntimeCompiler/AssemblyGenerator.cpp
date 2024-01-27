#include "AssemblyGenerator.h"
/*
void AssemblyGenerator::visit(IRVariableDeclaration* node) {
    // Allocate memory for the variable
    std::cout << "sub rsp, " << node->bytes << "\n"; // Subtract bytes from ESP to allocate memory
    std::cout << "mov rbp, [" << node->name << "]\n"; // Move the base pointer to the new variable
    
    // Assign a value to the variable
    //std::cout << "mov [" << node->name << "], 1\n"; // Move the value 1 to the new variable
}*/

void AssemblyGenerator::generateCode(const std::vector<IR*>& instructions) {
    std::cout << "section .data\n";

    // Assuming total is a global variable, and it's 4 bytes
    for (const IR* ir : instructions) {
        if (ir->command == "ALLOC" && ir->temp3 == "total") {
            std::cout << "total resb 4\n";
        }
        // Add additional sections for other data if needed
    }

    std::cout << "\nsection .text\n";

    std::cout << "global main\n";
    std::cout << "main:\n";

    for (const IR* ir : instructions) {
        if (ir->command == "STORE") {
            std::cout << "mov eax, " << ir->temp2 << "\n";
            std::cout << "mov dword [" << ir->temp1 << "], eax\n";
        } else {
            std::cout << "mov eax, " << ir->temp1 << "\n";
            std::cout << "mov ebx, " << ir->temp2 << "\n";

            if (ir->command == "/") {
                std::cout << "cdq\n";
                std::cout << "idiv ebx\n";
            } else if (ir->command == "*") {
                std::cout << "imul eax, ebx\n";
            }

            if (!ir->temp3.empty()) {
                std::cout << "mov dword [" << ir->temp3 << "], eax\n";
            }
        }
    }

    // Additional code for program termination
    std::cout << "\nmov eax, 1\n";
    std::cout << "xor ebx, ebx\n";
    std::cout << "int 0x80\n";
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