#pragma once
#ifndef EVALUATOR_H
#define EVALUATOR_H

#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

#define _WIN64
#include <any>
#include <functional>
#include <windows.h>

class IR;

class Function;

class Evaluator {
public:
    Evaluator();
    
    void Evaluate(const std::vector<IR*>& instructions);

public:
    std::unordered_map<std::string, int> memory;
    std::stack<int> tempStack;
    
    std::unordered_map<std::string, HMODULE> loadedLibs;
    //std::unordered_map<std::string, FARPROC> functionPointers;
    std::unordered_map<std::string, Function*> functionPointers;
};

class Function {
public:
    Function() {}
    
    void call(Evaluator& evaluator) const {
        evaluator.Evaluate(bodyInstructions);
    }
    
    void addToBody(IR* ir) {
        bodyInstructions.push_back(ir);
    }
    
private:
    std::vector<IR*> bodyInstructions;
};

#endif