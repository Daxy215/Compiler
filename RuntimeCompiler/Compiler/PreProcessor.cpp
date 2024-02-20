#include "PreProcessor.h"

#include <regex>
#include <sstream>

std::string PreProcessor::proccess(std::string code, const std::string& currentPath) {
    std::istringstream iss(code);
    std::ostringstream oss;
    std::string line;
    
    while (std::getline(iss, line)) {
        auto pos = line.find("#include");
        if (pos != std::string::npos) {
            char quote = line[pos +  9]; // Assuming "#include" is  8 characters long
            std::string fileName;
            if (quote == '\"') {
                fileName = extractFileName(line, currentPath);
            } else if (quote == '<') {
                fileName = extractFileName(line, currentPath);
            }
            
            if (includedFiles.count(fileName) > 0) {
                continue;
            }
            
            std::string includedContent = readFileContents(fileName);
            includedContent = processIncludeGuards(includedContent);
            includedFiles.insert(fileName);
            
            std::string includedDir = std::filesystem::path(fileName).parent_path().string();
            includedContent = proccess(includedContent, includedDir);
            
            oss << includedContent;
        } else {
            oss << line << "\n";
        }
    }
    
    return oss.str();
}

std::string PreProcessor::removeComments(const std::string& sourceCode) {
    std::istringstream iss(sourceCode);
    std::ostringstream oss;
    std::string line;

    while (std::getline(iss, line)) {
        line = std::regex_replace(line, std::regex("//.*"), "");
        
        size_t commentStart = line.find("/*");
        while (commentStart != std::string::npos) {
            size_t commentEnd = line.find("*/", commentStart + 2);
            if (commentEnd != std::string::npos) {
                line.erase(commentStart, commentEnd - commentStart + 2);
            } else {
                //TODO; Error Handle multi-line comment not having an end :)
                break;
            }
            
            commentStart = line.find("/*");
        }
        
        if (!line.empty()) {
            oss << line << "\n";
        }
    }
    
    return oss.str();
}

std::string PreProcessor::processIncludeGuards(const std::string& code) {
    std::istringstream iss(code);
    std::ostringstream oss;
    std::string line;
    bool guardActive = false;
    
    while (std::getline(iss, line)) {
        if (line.find("#ifndef") != std::string::npos) {
            guardActive = true;
        } else if (line.find("#define") != std::string::npos) {
            continue;
        } else if (line.find("#endif") != std::string::npos) {
            guardActive = false;
        } else if (line.find("#pragma once") != std::string::npos) {
            if (!hasPragma) {
                hasPragma = true;
                oss << line << "\n";
            }
            
            continue;
        } else if (guardActive) {
            continue;
        }
        
        oss << line << "\n";
    }
    
    return oss.str();
}
