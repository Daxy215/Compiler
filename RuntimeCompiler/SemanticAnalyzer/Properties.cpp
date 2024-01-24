#include "Properties.h"
#include "SymbolTable.h"

void Properties::addFunction(Properties* functionProperties) {
    functions.push_back(functionProperties);
}

void Properties::addMember(Properties* functionProperties) {
    members.push_back(functionProperties);
}
