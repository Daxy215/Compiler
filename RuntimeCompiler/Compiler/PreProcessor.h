#pragma once
#include <filesystem>
#include <fstream>
#include <set>
#include <stdexcept>
#include <string>

class PreProcessor {
public:
    std::string proccess(std::string code, const std::string& currentPath);

private:
    std::string removeComments(const std::string& sourceCode);
    std::string processIncludeGuards(const std::string& code);

private:
    std::string extractFileName(const std::string& line, const std::string& currentPath) {
        std::size_t start, end;
        if (line.find("<") != std::string::npos) {
            start = line.find("<") + 1;
            end = line.find(">");
        } else if (line.find("\"") != std::string::npos) {
            start = line.find("\"") + 1;
            end = line.find("\"", start);
        } else {
            throw std::invalid_argument("Invalid include directive format.");
        }
    
        std::string fileName = line.substr(start, end - start);
        
        if (std::filesystem::path(fileName).is_relative()) {
            fileName = std::filesystem::absolute(currentPath + fileName).string();
        }
        
        return fileName;
    }
    
    std::string readFileContents(const std::string& fileName) {
        std::ifstream fileStream(fileName);
        std::string fileContents((std::istreambuf_iterator<char>(fileStream)),
                                  std::istreambuf_iterator<char>());
        return fileContents;
    }
private:
    bool hasPragma = false;
    
    std::set<std::string> includedFiles;
};
