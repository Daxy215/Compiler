#pragma once
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <sstream>
#include <stdexcept>

template <typename T = int> requires std::is_arithmetic_v<T>
class Calculator
{
public:
    Calculator(const std::string& expression) : mExpression(expression) {
        next();
    }

    T exp() {
        T result = term();
        
        while (!mCurrent.empty()) {
            if (mCurrent == "+") {
                next();
                result += term();
            } else if (mCurrent == "-") {
                next();
                result -= term();
            } else {
                throw std::invalid_argument("Invalid expression");
            }
        }
        
        return result;
    }
    
private:
    std::string mExpression;
    std::string mCurrent;
    
    void next() {
        while (!mExpression.empty() && std::isspace(mExpression.front())) {
            mExpression.erase(mExpression.begin());
        }
        
        if (mExpression.empty()) {
            mCurrent.clear();
        } else {
            auto pos = mExpression.find_first_of("+-*/ ");
            if (pos == std::string::npos) {
                mCurrent = mExpression;
                mExpression.clear();
            } else {
                mCurrent = mExpression.substr(0, pos);
                mExpression.erase(0, pos + 1);
            }
        }
    }

    T term() {
        T result = factor();
        while (!mCurrent.empty()) {
            if (mCurrent == "*") {
                next();
                result *= factor();
            } else if (mCurrent == "/") {
                next();
                if (factor() == 0) {
                    throw std::invalid_argument("Division by zero");
                }
                
                result /= factor();
            } else {
                next();
                std::cout << "Invalid expression\n";
                //throw std::invalid_argument("Invalid expression");
            }
        }
        
        return result;
    }

    T factor() {
        if (mCurrent == "(") {
            next();
            T result = exp();
            if (mCurrent != ")") {
                throw std::invalid_argument("Missing closing parenthesis");
            }
            
            next();
            return result;
        } else {
            T result = toNum(mCurrent);
            next();
            return result;
        }
    }

    T toNum(const std::string& s) const {
        try {
            return std::stod(s);
        } catch (...) {
            std::cout << "Invalid number\n";
        }
    }
};

template<typename T = int> requires std::is_arithmetic_v<T>
T calculate(const std::string& s) {
   return Calculator<T> { s }.exp();
}

class TACEvaluation {
public:
    void interpretTAC(const std::vector<std::string> &tac);
private:
    std::vector<std::string> split(const std::string &s, char delimiter);
    void executeInstruction(const std::string &instruction);
public:
    std::map<std::string, double> variables;
};
