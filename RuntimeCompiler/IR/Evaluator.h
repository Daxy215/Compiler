#pragma once
#ifndef EVALUATOR_H
#define EVALUATOR_H

#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

class IR;

class Evaluator {
public:
    Evaluator();
    
    void Evaluate(const std::vector<IR*>& instructions);

public:
    std::unordered_map<std::string, int> memory;
    std::stack<int> tempStack; 
};

#endif