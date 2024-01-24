#pragma once
#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <string>
#include <vector>

class Properties {
public:
    Properties() {}
    Properties(std::string name, std::string type) : name(name), type(type) {}
    
    void addFunction(Properties* functionProperties);
    void addMember(Properties* functionProperties);
    
public:
    std::string name;
    std::string type;
    
    std::vector<std::string> inheritance;
    std::vector<std::string> parameters;
    std::vector<std::string> modifiers;
    
    std::vector<Properties*> functions;
    std::vector<Properties*> members;
};

#endif