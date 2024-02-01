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

void ErrorExit(LPCTSTR lpszFunction) 
{ 
    // Retrieve the system error message for the last-error code
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
    
    HMODULE hMod = LoadLibrary(reinterpret_cast<LPCWSTR>("dll\\libstdc++-6.dll"));
    if(hMod == NULL) {
        //ErrorExit(TEXT("LoadLibrary"));
    }
    
    FreeLibrary(hMod);
    
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
            int value = 0;
            
            if (!is_number(instruction->temp1)) {
                if(!tempStack.empty()) {
                    value = tempStack.top();
                    tempStack.pop();
                } else {
                    std::cerr << "Error stack is empty and trying to retrive a value :D\n";
                }
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
