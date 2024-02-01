#pragma once
#include <map>

#include "../Parser.h"

class CFGNode {
public:
    CFGNode(ASTNode* node) : node(node) {}

    void addParent(CFGNode* parent) {
        parents.push_back(parent);
        parent->children.push_back(this);
    }

    void addChild(CFGNode* child) {
        children.push_back(child);
        child->parents.push_back(this);
    }

public:
    ASTNode* node;
    
    std::vector<CFGNode*> parents;
    std::vector<CFGNode*> children;
};

class ControlFlowGraph {
public:
    void generateCFG(ASTNode* node, std::map<ASTNode*, CFGNode*>& cfgNodes);
    
    void printCFG(CFGNode* root, std::map<ASTNode*, CFGNode*>& cfgNodes) {
        if(root->node->isTypeOf(NodeType::RETURN_STATEMENT)) {
            std::cout << "Node: " << root->node->value << " - " << root->node->children[0]->value << ", \nParents: ";
        } else
            std::cout << "Node: " << root->node->value << ", \nParents: ";
        
        for(CFGNode* parent : root->parents) {
            if(cfgNodes[parent->node]->node->isTypeOf(NodeType::RETURN_STATEMENT)) {
                std::cout << cfgNodes[parent->node]->node->value << " - " << cfgNodes[parent->node]->node->children[0]->value << " ";
            } else
                std::cout << cfgNodes[parent->node]->node->value << " ";
        }
        
        std::cout << ", \nChildren: ";
        for(CFGNode* child : root->children) {
            if(cfgNodes[child->node]->node->isTypeOf(NodeType::RETURN_STATEMENT)) {
                std::cout << cfgNodes[child->node]->node->value << " - " << cfgNodes[child->node]->node->children[0]->value << " ";
            } else
                std::cout << cfgNodes[child->node]->node->value << " ";
        }
        
        std::cout << "\n";
        
        for(CFGNode* child : root->children)
            printCFG(child, cfgNodes);
     }
private:
    
};
