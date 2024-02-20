#include "AssemblyGenerator.h"

#include <cstdlib> // For system function
#include <Windows.h>
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
 *
 *  const int SIZE = 4;
 *  SIZE equ 4  ; Define a constant
 */

std::string mainFunction = "_main";

void AssemblyGenerator::generateCode(const std::vector<IR*>& instructions) {
    // Create file
    std::ofstream outputFile("Compiling/main.asm");
    
    outputFile << "global _main\n\n";
    
    outputFile << "section .bss\n";
    
    //for (const IR* ir : instructions) {
        //if (ir->command == "ALLOC") {
        //    outputFile << "\t" << ir->temp2 << ": resd " << ir->temp1 << "\n";
        //}
    //}
    
    outputFile << "\nsection .data\n";
    
    // There shouldn't be any data.
    
    outputFile << "\nsection .text\n";
    
    for (int i = 0; i < instructions.size(); i++) {
        IR* ir = instructions[i];
        
        if (ir->command == "ALLOC") {
            //  {command="ALLOC", temp1="4", temp2="temp1", ...}
            outputFile << "\t; Allocate space for " << ir->temp2 << "\n";
            outputFile << "\tsub esp, " << std::stoi(ir->temp1) << "\n";
            
            std::string address;
            if(currentFunction->size > 0) {
                address = "[ebp - " + std::to_string(currentFunction->size) + "]";
            } else {
                address = "[ebp]";
            }
            
            outputFile << "\tmov dword " << address << ", 0\n\n";
            
            currentFunction->size += std::stoi(ir->temp1);
            currentFunction->addVariable(ir->temp2, address, std::stoi(ir->temp1));
        } else if (ir->command == "STORE") {
            // int x = 5;
            outputFile << "\t; Store '" << ir->temp1 << "' into '" << ir->temp2 << "'\n";
            
            Variable* temp1 = currentFunction->getVariable(ir->temp1);
            Variable* temp2 = currentFunction->getVariable(ir->temp2);
            
            if(isNumber(ir->temp1)) {
                outputFile << "\tmov eax, " << ir->temp1 << "\n";
            } else {
                // Check if NOT it's a function
                if(!functions.contains(ir->temp1)) {
                    outputFile << "\tmov eax, " << temp1->address << "\n";
                }
            }
            
            outputFile << "\tmov " << temp2->address << ", eax\n\n";
        } else if (ir->command == "IF_STATEMENT" || ir->command == "ELSEIF_STATEMENT" || ir->command == "ELSE_STATEMENT") {
            outputFile << "\t; " << ir->command << "\n";
            // if true go to true branch
            outputFile << "\tje START" << ir->temp3 << "\n";
            
            // If false go to false branch
            outputFile << "\tjmp END" << ir->temp3 << "\n";
        } else if (ir->command == "LABEL") {
            outputFile << "\n" << ir->temp1 << ":\n";
        } else if (ir->command == "FUNCTION") {
            std::string functionName = ir->temp1;
            
            if(functionName == "main") {
                functionName = mainFunction;
            }
            
            outputFile << "\n" << functionName << ":\n";
            
            if(functionName == mainFunction) {
                outputFile << "\t; Function prologue\n";
                outputFile << "\tpush ebp\n";
                outputFile << "\tmov ebp, esp\n\n";
            }
            
            // Add the function to the map of functions
            Function* function = new Function(functionName, 0);
            currentFunction = function;
            functions.emplace(functionName, function);
            
            // Parameters handling
            outputFile << "\t; Parameters\n";
            
            size_t sizeIndex = 8;
            int j = i + 1;
            for(; j < instructions.size(); j++) {
                // {command="PARAMETER", temp1="4", temp2="printHelloWorld", ...}
                IR* para = instructions[j];
                
                if(para->command == "PARAMETER") {
                    // To keep track of the address
                    size_t varSize = std::stoi(para->temp1);
                    
                    std::string address = "[ebp + " + std::to_string(sizeIndex) + "]";
                    
                    outputFile << "\t; Parameter " << para->temp2 << " " << para->temp3 << "\n";
                    
                    //TODO; Just make a simple list.. I'm too lazy
                    if(j == 1)
                        outputFile << "\tmov eax, " << address << "\n\n";
                    else if(j == 2)
                        outputFile << "\tmov ebx, " << address << "\n\n";
                    else if(j == 3)
                        outputFile << "\tmov ecx, " << address << "\n\n";
                    
                    currentFunction->addParamater(para->temp3, address, varSize);
                    
                    sizeIndex += varSize;
                } else
                    break;
            }
            
            // Update current index
            i = j - 1;
            
            // Update function size
            //function->size = sizeIndex - 8; // Start offset
            
            outputFile << "\t; Function body\n\n";
        } else if (ir->command == "FUNCTION_CALL") {
            int j = i + 1;
            for(; j < instructions.size(); j++) {
                IR* para = instructions[j];
                
                if(para->command == "PARAMETER") {
                    outputFile << "\t; Parameter value " << para->temp1 << "\n";
                    outputFile << "\tpush " << para->temp1 << "\n";
                } else
                    break;
            }
            
            // Update current index
            i = j;
            
            outputFile << "\t; Function call\n";
            outputFile << "\tcall " << ir->temp1 << "\n\n";
            
            Function* function = functions.find(ir->temp1)->second;
            
            function->cleanUp(outputFile);
        } else if (ir->command == "RETURN") {
            Variable* temp = getVariable(ir->temp1);
            
            if(currentFunction->label == mainFunction) {
                outputFile << "\t; Exit Progam\n";
                outputFile << "\tmov eax, " << temp->address << "\n";
                outputFile << "\tadd esp, 4\t; clear the stack\n";
            } else {
                outputFile << "\tmov eax, " << temp->address << "\n";
            }
            
            // ????????
            //outputFile << "\n\t; Function epilogue\n";
            //outputFile << "\tpop ebp\n\n";
            
            outputFile << "\tret\t\t; return\n";
        } else if (ir->command == "==") {
            Variable* temp1 = getVariable(ir->temp1);
            Variable* temp2 = getVariable(ir->temp2);
                        
            outputFile << "\tmov eax, " << temp1->address << "\n";
            outputFile << "\tcmp " << temp2->address << ", eax\n\n";
        } else if(ir->command == "+" || ir->command == "-" ||
            ir->command == "*" || ir->command == "/") {
            /*
             * {command="+", temp1="temp4", temp2="temp5", ...}
             *
             * mov eax, 5        ; Operand 1
             * mov ebx, 3        ; Operand 2
             * add ebx           ; Add eax by ebx
             */
            
            Variable* temp1 = getVariable(ir->temp1);
            Variable* temp2 = getVariable(ir->temp2);
            Variable* temp3 = getVariable(ir->temp3);
            
            outputFile << "\t; Add " << temp1->name << " by " << temp2->name << "\n";
            outputFile << "\tmov eax, " << temp1->address << "\n";
            outputFile << "\tmov ebx, " << temp2->address << "\n";
            
            if(ir->command == "+")
                outputFile << "\tadd eax, ebx\t\t\t\t; Add ebx to eax\n";
            else if(ir->command == "-")
                outputFile << "\tsub eax, ebx\t\t\t\t; Subtract eax by ebx\n";
            else if(ir->command == "*")
                outputFile << "\tmul ebx\t\t\t\t; Multiply eax by ebx\n";
            else if(ir->command == "/") {
                outputFile << "\txor edx, edx\t\t; Clear the high-order bits in edx\n";
                outputFile << "\tdiv ebx\t\t\t\t; Divide ebx by eax\n";
            }
            
            outputFile << "\n\t; Store results in " << temp3->name << "\n";
            outputFile << "\tmov " << temp3->address << ", eax\n\n";
        } else {
            std::cout << "\n\t; couldn't find command: " << ir->command << "\n\n";
        }
    }
    
    outputFile.close();

    std::cout << "\n\n";
    
    // Compile
    /*int results = system("cd Compiling && build.bat");
    
    if(results == 0) {
        std::cout << "Compiled successfully!\n";
    } else {
        std::cerr << "Error while compiling: " << results << " \n";
    }*/
}