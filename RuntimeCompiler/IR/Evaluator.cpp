#include "Evaluator.h"

#include <algorithm>
#include <strsafe.h>

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

void ErrorExit(LPCTSTR lpszFunction) { 
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );
    
    // Display the error message and exit the process
    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR)); 
    StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"), 
        lpszFunction, dw, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 
    
    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    ExitProcess(dw); 
}

int Evaluator::Evaluate(const std::vector<IR*>& instructions) {
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
    
    HMODULE hMod = LoadLibrary(reinterpret_cast<LPCWSTR>("dll\\libstdc++-6.dll"));
    if(hMod == NULL) {
        //gErrorExit(TEXT("LoadLibrary"));
    }
    
    FreeLibrary(hMod);
    
    /*for(auto& instruction : instructions) {
        // TODO; Make this as a map;
        if(instruction->command == "FUNCTION")
            pointers[instruction->temp1] = new Pointer();
        
        if(instruction->hasParent()) {
            if(instruction->command == "IF_STATEMENT")
                pointers[instruction->temp1] = new Pointer();
            
            if(pointers.contains(instruction->parent)) {
                pointers[instruction->parent]->addToBody(instruction);
                
                // Remove parent
                instruction->parent = "";
                
                continue;
            } else if(memory.contains(instruction->parent)) {
                // Idk if I want to do anything else..
                instruction->parent = "";
            } else {
                std::cout << "Couldn't find parent; " << instruction->parent <<  "\n";
            }
        }
        
        if (instruction->command == "INCLUDE") {
            std::string libName = instruction->temp2;
            
            std::wstring ws(libName.begin(), libName.end());
            
            HMODULE lib = LoadLibrary(ws.c_str());
            if (!lib) {
                // Handle error
            }
            
            loadedLibs[libName] = lib;
        } else if(instruction->command == "FUNCTION_CALL") {
            std::string functionName = instruction->temp1;
            
            if(pointers.contains(functionName)) {
                // Call the function.
                int results = pointers[functionName]->call(*this);
                
                if(results != -1) {
                    memory[functionName] = results;
                    //return results;
                }
            }
        } else if(instruction->command == "IF_STATEMENT") {
            std::string name = instruction->temp1;
            
            if(pointers.contains(name)) {
                // If the condition is correct.
                if(memory[instruction->temp2]) {
                    const int results = pointers[name]->call(*this);
                    
                    if(results != -1) {
                        //results = 5;
                        return results;
                    }
                } else {
                    //Condition is not met.
                    continue;
                }
            }
        } else if(instruction->command == "RETURN") {
            int value;
            
            if (!is_number(instruction->temp1)) {
                value = memory[instruction->temp1];
            } else {
                value = std::stoi(instruction->temp1);
            }
            
            return value;
        } else if (instruction->command == "ALLOC") {
            memory[instruction->temp2] = 0;
        } else if (instruction->command == "STORE") {
            std::string cmd = instruction->command;
            std::string pr = instruction->parent;
            
            int value;
            
            if (!is_number(instruction->temp1)) {
                value = memory[instruction->temp1];
                //if(!tempStack.empty()) {
                //    value = tempStack.top();
                //    tempStack.pop();
                //} else {
                //    std::cerr << "Error stack is empty and trying to retrive a value :D\n";
               // }
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
        } else if (instruction->command == "==") {
            const int operand2 = tempStack.top();
            tempStack.pop();
            const int operand1 = tempStack.top();
            tempStack.pop();
            
            int result = operand1 == operand2;
            tempStack.push(result);
            memory[instruction->temp3] = result;
        } else {
            std::cerr << "Couldn't find command; " << instruction->command << "\n";
        }
    }*/
    
    return -1;
}
