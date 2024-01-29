#pragma once
#include <cvt/wstring>

class Token;

class Diagnoser {
public:
    static void logError(std::string errorMessage, Token token);
};
