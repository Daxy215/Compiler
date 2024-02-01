#include "ControlFlowGraph.h"

void ControlFlowGraph::generateCFG(ASTNode* node, std::map<ASTNode*, CFGNode*>& cfgNodes) {
    CFGNode* cfgNode = new CFGNode(node);
    cfgNodes[node] = cfgNode;
    
    //TODO;
    
    for(auto* child : node->children) {
        generateCFG(child, cfgNodes);
        cfgNode->addChild(cfgNodes[child]);
    }
}
