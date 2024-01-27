#include "Evaluator.h"
#include "IntermediateRepresentation.h"

Evaluator::Evaluator() {
    
}

void Evaluator::Evaluate(const std::vector<IR*>& instructions) {
    for(auto& instruction : instructions) {
        if (instruction->command == "ALLOC") {
            memory[instruction->temp2] = 0;
        } else if (instruction->command == "STORE") {
            int value;
            if (instruction->temp1.find("temp") != std::string::npos) {
                value = tempStack.top();
                tempStack.pop();
            } else {
                value = std::stoi(instruction->temp1);
            }
            
            if (instruction->temp2.find("temp") != std::string::npos) {
                tempStack.push(value);
            } else {
                memory[instruction->temp2] = value;
            }
        } else if (instruction->command == "+") {
            int operand2 = tempStack.top();
            tempStack.pop();
            int operand1 = tempStack.top();
            tempStack.pop();
            int result = operand1 + operand2;
            tempStack.push(result);
            memory[instruction->temp3] = result;
        } else if (instruction->command == "-") {
            int operand2 = tempStack.top();
            tempStack.pop();
            int operand1 = tempStack.top();
            tempStack.pop();
            int result = operand1 - operand2;
            tempStack.push(result);
            memory[instruction->temp3] = result;
        } else if (instruction->command == "*") {
            int operand2 = tempStack.top();
            tempStack.pop();
            int operand1 = tempStack.top();
            tempStack.pop();
            int result = operand1 * operand2;
            tempStack.push(result);
            memory[instruction->temp3] = result;
        } else if (instruction->command == "/") {
            int operand2 = tempStack.top();
            tempStack.pop();
            int operand1 = tempStack.top();
            tempStack.pop();
            int result = operand1 / operand2;
            tempStack.push(result);
            memory[instruction->temp3] = result;
        }
    }
}
