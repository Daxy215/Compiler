#pragma once
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>

#include "IR/IntermediateRepresentation.h"

struct Variable {
    std::string name, registry, address;
    size_t size;
    
    Variable(const std::string& name, const std::string& registry, const std::string& address, const size_t size) : name(name), registry(registry), address(address), size(size) {
        
    }
};

struct Function {
    std::string label;
    size_t size;
    std::vector<Variable*> parameters;
    std::vector<Variable*> variables;
    
    Function(std::string label, size_t size) : label(label), size(size) {}
    
    void addParamater(const std::string& name, const std::string& registry, const std::string& address, const size_t size) {
        this->size += size;
        
        parameters.push_back(new Variable(name, registry, address, size));
    }
    
    void addVariable(const std::string& name, const std::string& registry, const std::string& address, const size_t size) {
        this->size += size;
        
        variables.push_back(new Variable(name, registry, address, size));
    }
    
    void cleanUp(std::ofstream& outputFile) {
        if(size == 0)
            return;
        
        outputFile << "\t; Function prologue for " << label << "\n";
        outputFile << "\tpush ebp\n";
        outputFile << "\tmov ebp, esp\n\n";
        
        outputFile << "\t; Clean up " << label << " \n";
        outputFile << "\tadd esp, " << size << "\n\n";
    }
    
    Variable* getVariable(const std::string& temp) {
        for(auto it = variables.begin(); it != variables.end(); it++) {
            if((*it)->name == temp) {
                return *it;
            }
        }
        
        // Check if it's a parameter
        for(auto it = parameters.begin(); it != parameters.end(); it++) {
            if((*it)->name == temp) {
                return *it;
            }
        }
        
        return nullptr; 
    }
};

class Class {
public:
    Class(std::string label) {
        this->label = label;    
    }
    
    std::string getName() {
        return label;
    }
    
    const std::vector<Function*>& getConstructors() {
        return constructors;
    }
    
    void addConstructor(Function* constructor) {
        this->size += constructor->size;
        constructors.push_back(constructor);
    }

    void addMember(std::string name, std::string registery, std::string address, size_t size) {
        this->size += size;
        
        members.push_back(new Variable(name, registery, address, size));
    }
    
    size_t getSize() {
        return size;
    }

    Variable* getMember(const std::string& name) {
        for(auto it = members.begin(); it != members.end(); it++) {
            if((*it)->name == name) {
                return *it;
            }
        }
        
        return nullptr;
    }

    Variable* getVariable(const std::string& temp) {
        // Check if it's a parameter
        for(auto cons : constructors) {
            for(auto it = cons->parameters.begin(); it != cons->parameters.end(); it++) {
                if((*it)->name == temp) {
                    return *it;
                }
            }
        }
        
        return nullptr; 
    }

    size_t calculateMemberSizeAddress(std::string name) {
        size_t size = 0;

        for(auto cons : constructors) {
            for(auto it = cons->parameters.begin(); it != cons->parameters.end(); ++it) {
                Variable* var = (*it);
                
                if(var->name == name) {
                    return size;
                }
                
                size += var->size;
            }
        }
        
        return -1;
    }

private:
    std::string label;
    
    size_t size;
    
    // Class can contain mulitple constructors.
    std::vector<Function*> constructors;
    std::vector<Function*> functions;
    std::vector<Variable*> members;
};

class AssemblyGenerator : public IRVisitor {
public:
    void generateCode(const std::vector<IR*>& irCommands);
    
private:
    bool isNumber(const std::string& str) {
        for (char const &c : str) {
            if (!std::isdigit(c)) return false;
        }
        
        return !str.empty();
    }
    
    std::vector<std::string> splitString(const std::string &s, char delimiter) {
        std::vector<std::string> tokens;
        std::istringstream iss(s);
        std::string token;
        
        while (std::getline(iss, token, delimiter)) {
            tokens.push_back(token);
        }
        
        return tokens;
    }
    
    std::string convertVariable(std::string var) {
        if(isNumber(var)) {
            return var;
        }
        
        return "[" + var + "]";
    }
    
    Variable* getVariable(std::string temp) {
        if(isNumber(temp))
            return new Variable(temp, "", temp, 4);
        
        Variable* variable = currentFunction->getVariable(temp);
        
        // Variable is found
        if(variable) {
            return variable;
        }
        
        return nullptr;
    }

private:
    Class* currentClass = nullptr;
    Function* currentFunction = nullptr;
    
    std::map<std::string, Class*> classes;
    std::map<std::string, Function*> functions;
};
