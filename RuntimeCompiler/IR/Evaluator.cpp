#include "Evaluator.h"

#include <algorithm>
//#include <stringapiset.h>

#include "IntermediateRepresentation.h"

Evaluator::Evaluator() {
    
}

bool is_number(const std::string &s) {
    return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

/*void funcDoubleDouble(double a, double b) {
    std::cout << "Function with double parameters: " << a << " and " << b << "\n";
}*/

void Evaluator::Evaluate(const std::vector<IR*>& instructions) {
    /*functionPointers["std::cout << \"sometext\" << std::endl"] = [](){ std::cout << "sometext" << std::endl; };
    
    std::string functionName = "std::cout << \"sometext\" << std::endl";
    // Check if the function pointer is already loaded
    auto it = functionPointers.find(functionName);
    if (it == functionPointers.end()) {
        // Handle error
    }
    
    // Call the function
    it->second();*/
    
    /**
    functionPointers["voidFunction"] = &funcVoid;
    functionPointers["intFunction"] = std::bind(&funcInt, 42); // Binding argument to create a void() function
     */
    
    //auto& function = functionPointers["doubleDoubleFunction"] = std::bind(&funcDoubleDouble, 3.14, 2.718);
    //function();
    
    for(auto& instruction : instructions) {
        if(instruction->hasParent()) {
            functionPointers[instruction->parent]->addToBody(instruction);

            // Remove parent
            instruction->parent = "";
            
            continue;
        }
        
        if (instruction->command == "INCLUDE") {
            std::string libName = instruction->temp2;
            
            std::wstring ws(libName.begin(), libName.end());
            
            HMODULE lib = LoadLibrary(ws.c_str());
            if (!lib) {
                // Handle error
            }
            
            loadedLibs[libName] = lib;
        } else if(instruction->command == "FUNCTION") {
            std::string functionName = instruction->temp1;
            
            functionPointers[functionName] = new Function();
        } else if(instruction->command == "FUNCTION_CALL") {
            std::string functionName = instruction->temp1;
            
            if(functionPointers.contains(functionName)) {
                // Call the function.
                functionPointers[functionName]->call(*this);
            }
        } else if (instruction->command == "ALLOC") {
            memory[instruction->temp2] = 0;
        } else if (instruction->command == "STORE") {
            int value;
            
            if (!is_number(instruction->temp1)) {
                value = tempStack.top();
                tempStack.pop();
            } else {
                value = std::stoi(instruction->temp1);
            }
            
            tempStack.push(value);
            memory[instruction->temp2] = value;
        } else if (instruction->command == "+") {
            const int operand2 = tempStack.top();
            tempStack.pop();
            const int operand1 = tempStack.top();
            tempStack.pop();
            
            int result = operand1 + operand2;
            tempStack.push(result);
            memory[instruction->temp3] = result;
        } else if (instruction->command == "-") {
            const int operand2 = tempStack.top();
            tempStack.pop();
            const int operand1 = tempStack.top();
            tempStack.pop();
            
            int result = operand1 - operand2;
            tempStack.push(result);
            memory[instruction->temp3] = result;
        } else if (instruction->command == "*") {
            const int operand2 = tempStack.top();
            tempStack.pop();
            const int operand1 = tempStack.top();
            tempStack.pop();
            
            int result = operand1 * operand2;
            tempStack.push(result);
            memory[instruction->temp3] = result;
        } else if (instruction->command == "/") {
            const int operand2 = tempStack.top();
            tempStack.pop();
            const int operand1 = tempStack.top();
            tempStack.pop();
            
            int result = operand1 / operand2;
            tempStack.push(result);
            memory[instruction->temp3] = result;
        }
    }
}
