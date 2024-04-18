#include "IntermediateRepresentation.h"

#include <map>

#include "../Compiler/Compiler.h"

// In bytes
std::map<std::string, size_t> variablesSize = {
  {"int", sizeof(int)},
  {"float", sizeof(float)},
};

/*
 * https://learn.microsoft.com/en-us/cpp/cpp/data-type-ranges?view=msvc-170
 * std::cout << "Size of int: " << sizeof(int) << " bytes";
 *
 * Int - 4 bytes - signed ;	-2,147,483,648 to 2,147,483,647
 * unsigned int - 4 bytes - unsigned	; 0 to 4,294,967,295
 * long - 4 bytes - long int, signed long int ; -2,147,483,648 to 2,147,483,647
 * float - 4 bytes - none ; 3.4E +/- 38 (7 digits)
 * double - 8 bytes - none ; 1.7E +/- 308 (15 digits)
 */

/*
 * int x = 1;
 *
 * IR;
 * (ALLOC, 4, x) <- Allocates 4 bytes of memory and return the address in x.
 * (INT, 1, X) <- Assign an integer value of 1 to x.
 */

/*
 * int total = 2 + 4 / 4 * 8;
 *
 * IR;
 * (ALLOC, 4, total) <- Allocates 4 bytes of memory and return the address in total.
 * (INT, 2, temp1) <- Assign an integer value of 2 to a temporary variable.
 * (INT, 4, temp2) <- Assign an integer value of 2 to a temporary variable.
 *
 * Divide temp2(4) by 4.
 * (DIV, temp2, 4, temp3) <- Divide temp2 by 4 and store it in temp3.
 * (INT, 8, temp4) <- Assign an integer value of 8 to a temporary variable.
 * (MUL, temp3, temp4, temp5) <- Multiply temp3 by temp4 and store it in temp5.
 * (ADD, temp1, temp5, total) <- Add temp1, temp5 and store the results total.
 */

void IntermediateRepresentation::generateIR(ASTNode* node, ASTNode* parent) {
    if(node == nullptr)
        return;
    
    std::string parentValue = parent ? parent->value : "";
    
    switch (node->getType()) {
    case NodeType::INCLUDE_DIRECTIVE: {
            addCommand(INCLUDE, node->value, node->children[0]->value, parentValue);
            
            break;
    }
    case NodeType::CLASS: {
            addCommand(CLASS_START, node->value, parentValue);
            
            for (auto child : node->children) {
                generateIR(child, node);
            }
            
            addCommand(CLASS_END, node->value, parentValue);
            
            break;
    }
    case NodeType::CONSTRUCTOR: {
            addCommand(CONSTRUCTOR_START, node->value, parentValue);
            
            ASTNode* parameters = node->getChildByType(NodeType::PARAMETERS);
            
            for(auto& child : parameters->children) {
                ASTNode* variableNodeType = child->getChildByType(NodeType::VARIABLE_TYPE);
                
                std::string name = child->value;
                std::string type = variableNodeType->value;
                const size_t variableSize = variablesSize[type];
                
                addCommand(PARAMETER, std::to_string(variableSize), type, name, node->value);
            }
            
            for (auto child : node->children) {
                generateIR(child, node);
            }
            
            addCommand(CONSTRUCTOR_END, node->value, parentValue);
            
            break;
    }
    case NodeType::MEMBER_FUNCTION: {
            ASTNode* returnType = node->getChildByType(NodeType::RETURN_TYPE);
            addCommand(FUNCTION, node->value, returnType->value, parentValue);
            
            ASTNode* parameters = node->getChildByType(NodeType::PARAMETERS);
            
            for(auto& child : parameters->children) {
                ASTNode* variableNodeType = child->getChildByType(NodeType::VARIABLE_TYPE);
                
                std::string name = child->value;
                std::string type = variableNodeType->value;
                const size_t variableSize = variablesSize[type];
                
                addCommand(PARAMETER, std::to_string(variableSize), type, name, node->value);
            }
            
            ASTNode* functionBody = node->getChildByType(NodeType::BLOCK);
            
            if(functionBody)
                for(auto child : functionBody->children) {
                    generateIR(child, node);
                }
            
            break;
        }
    case NodeType::FUNCTION_CALL: {
            if(node->token.type == LexerNameSpace::POINTER) {
                return generatePointerIR(node, parent);
            }

            // Special case for 'this' usage.
            if(node->value == "this") {
                ASTNode* variable = node->children[0]->getChildByType(NodeType::FUNCTION_CALL);
                
                // TODO; It's hardcoded.
                addCommand(STORETHIS, variable->children[1]->value, variable->value, parentValue);
                
                return;
            }
            
            /**
            * (FUNCTION_CALL, "std::cout")
            * (OPERATOR, "<<")
            * (STRING, "Values; ")
            * (VARIABLE, "total")
            * (OPERATOR, "<<")
            * (FUNCTION_CALL, "std::endl")
            */
            ASTNode* accessType = node->getChildByType(NodeType::ACCESSTYPE);
            
            // ?????????
            if(accessType == nullptr) {
                addCommand(FUNCTION_CALL, node->value, parentValue);
                
                ASTNode* parameters = node->getChildByType(NodeType::PARAMETERS);
                
                for(auto& child : parameters->children) {
                    addCommand(PARAMETER, child->value, parentValue);
                }
                
                return;
            }
            
            std::string name = node->value + (accessType->value == "STATIC" ? "::" : ".")
            + accessType->children[0]->value;
            std::string paramaters = name + " ";
            
            for(auto child : accessType->children[0]->children) {
                if(child->isTypeOf(NodeType::FUNCTION_CALL)) {
                    accessType = child->getChildByType(NodeType::ACCESSTYPE);
                    
                    paramaters += child->value + (accessType->value == "STATIC" ? "::" : ".")
                        + accessType->children[0]->value + " ";
                } else {
                    paramaters += child->value + " ";
                }
            }
            
            addCommand(FUNCTION_CALL, name, paramaters, parentValue);
            
            break;
        }
    case NodeType::MEMBER_VARIABLE:
    case NodeType::LOCAL_VARIABLE_DECLARATION: {
            /*
             * int x = 1;
             *
             * IR;
             * (ALLOC, 4, x) <- Allocates 4 bytes of memory and return the address in x.
             * (INT, 1, X) <- Assign an integer value of 1 to x.
             */
            
            ASTNode* varType = node->getChildByType(NodeType::VARIABLE_TYPE);
            const size_t variableSize = variablesSize[varType->value];
            
            // (ALLOC, 4, x)
            addCommand(ALLOC, std::to_string(variableSize), node->value, parentValue);
            
            ASTNode* assignment = node->getChildByType(NodeType::COMPOUND_ASSIGNMENT);
            
            if(assignment) {
                //if(assignment->children.size() == 1) {
                    //addCommand("FUNCTION_CALL", assignment->children[0]->value, "", parentValue);
                    //addCommand("STORE", assignment->children[0]->value, node->value, parentValue);
                //} else {
                    generateIR(assignment->children[0], parent);
                    addCommand(STORE, assignment->children[0]->value, node->value, parentValue);
                //}
            } else {
                //addCommand("STORE", "0",node->value, parentValue);
            }
            
            break;
        }
    case NodeType::IF_STATEMENT:
    case NodeType::ELSEIF_STATEMENT:
    case NodeType::ELSE_STATEMENT:
    case NodeType::FOR_LOOP:
    case NodeType::WHILE_LOOP:
    case NodeType::DO_WHILE_LOOP: {
            handleControlFlow(node, parent);
            
            break;
        }
    case NodeType::RETURN_STATEMENT: {
            /*
             * (LOAD, 76, temp_result) ; Load the value 76 into a temporary variable
             * (RETURN, temp_result)    ; Return the value stored in temp_result
             */
            
            //TODO; Somehow figure this out..
            //You need to change parser to handle this future me :)
            // Got it :D Just being stupid lol
            //addCommand("FUNCTION_CALL", node->children[0]->value, "", parentValue);
            
            generateIR(node->children[0], parent);
            addCommand(RETURN, node->children[0]->value, parentValue);
            
            break;
        }
    case NodeType::OPERATOR: {
            /*
             * int total = 2 + 4 / 4 * 8;
             *
             * IR;
             * (ALLOC, 4, total) <- Allocates 4 bytes of memory and return the address in total.
             * (INT, 2, temp1) <- Assign an integer value of 2 to a temporary variable.
             * (INT, 4, temp2) <- Assign an integer value of 2 to a temporary variable.
             *
             * Divide temp2(4) by 4.
             * (DIV, temp2, 4, temp3) <- Divide temp2 by 4 and store it in temp3.
             * (INT, 8, temp4) <- Assign an integer value of 8 to a temporary variable.
             * (MUL, temp3, temp4, temp5) <- Multiply temp3 by temp4 and store it in temp5.
             * (ADD, temp1, temp5, total) <- Add temp1, temp5 and store the results total.
             */
            
            std::string leftOperand = "temp" + std::to_string(tempCounter++);
            generateIR(node->children[0], parent);
            
            std::string rightOperand = "temp" + std::to_string(tempCounter++);
            generateIR(node->children[1], parent);
            
            //TODO; Store size for operators.
            addCommand(ALLOC, std::to_string(4), leftOperand, parentValue);
            addCommand(STORE, node->children[0]->value, leftOperand, parentValue);
            
            addCommand(ALLOC, std::to_string(4), rightOperand, parentValue);
            addCommand(STORE, node->children[1]->value, rightOperand, parentValue);

            // TODO; Variable size is hardcoded.
            addCommand(ALLOC, std::to_string(4), "temp" + std::to_string(tempCounter), parentValue);
            
            // node->value is the operator(+, -, *, /)
            addCommand(node->value,  leftOperand, rightOperand, "temp" + std::to_string(tempCounter), parentValue);
            node->value = "temp" + std::to_string(tempCounter);
            
            tempCounter++;
            
            break;
         }
    case NodeType::COMPOUND_ASSIGNMENT: {
            ASTNode* assignment = node->getChildByType(NodeType::COMPOUND_ASSIGNMENT);
            generateIR(assignment->children[0], node);
            // (ADD, temp1, temp5, total)
            // (INT, 1, X) <- Assign an integer value of 1 to x.
            // TODO; Wtf is this???????????????
            //addCommand("INT", "temp" + std::to_string(tempCounter - 1), parentValue);
            
            break;
        }
    default:
        for(auto& c : node->children)
            generateIR(c, parent);
    }
}

void IntermediateRepresentation::handleControlFlow(ASTNode* node, ASTNode* parent) {
    switch(node->type) {
    case NodeType::IF_STATEMENT: {
            ASTNode* conditionsNode = node->getChildByType(NodeType::CONDITIONS);
            const std::string combinedCondition = handleConditions(conditionsNode, parent);
            const ASTNode* trueBranch = node->getChildByType(NodeType::TRUE_BRANCH);
            
            std::string label = std::to_string(labelCounter++) + "LABEL";
            addCommand(IF_STATEMENT, "if", combinedCondition, label, parent->value);
            
            addCommand(LABEL, "START" + label, node->value);
            
            for(auto& child : trueBranch->children)
                generateIR(child, node);
            
            addCommand(LABEL, "END" + label, node->value);
            
            break;
        }
    case NodeType::ELSEIF_STATEMENT: {
            ASTNode* conditionsNode = node->getChildByType(NodeType::CONDITIONS);
            const std::string combinedCondition = handleConditions(conditionsNode, parent);
            const ASTNode* trueBranch = node->getChildByType(NodeType::TRUE_BRANCH);
            
            std::string label = std::to_string(labelCounter++) + "LABEL";
            addCommand(ELSEIF_STATEMENT, "elseif", combinedCondition, label, parent->value);
            
            addCommand(LABEL, "START" + label, node->value);
            
            for(auto& child : trueBranch->children)
                generateIR(child, node);
            
            addCommand(LABEL, "END" + label, node->value);
            
            break;
        }
    case NodeType::ELSE_STATEMENT: {
             const ASTNode* trueBranch = node->getChildByType(NodeType::BLOCK);
            
            std::string label = std::to_string(labelCounter++) + "LABEL";
            addCommand(ELSE_STATEMENT, "else", "", label, parent->value);
            
            addCommand(LABEL, "START" + label, node->value);
            
            for(auto& child : trueBranch->children)
                generateIR(child, node);
            
            addCommand(LABEL, "END" + label, node->value);
            
            break;
    }
    case NodeType::FOR_LOOP: {
            ASTNode* cur = node;
            ASTNode* trueBranch = node->getChildByType(NodeType::BLOCK);
            
            
            
            break;
        }
    case NodeType::WHILE_LOOP: {
            
            
            break;
        }
    case NodeType::DO_WHILE_LOOP: {
            
            
            break;
        }
    }
}

void IntermediateRepresentation::generatePointerIR(ASTNode* node, ASTNode* parent) {
    std::cout << "No pointer generation just yet.." << std::endl;
}

std::string IntermediateRepresentation::handleConditions(ASTNode* node, ASTNode* parent) {
    std::string combinedCondition;
    
    for(size_t i = 0; i < node->children.size(); i++) {
        if(node->children[i]->type == NodeType::CONDITIONS) {
            combinedCondition += "(" + handleConditions(node->children[i], parent) + ")";
        } else {
            generateIR(node->children[i], parent);
            combinedCondition += node->children[i]->value;
        }
        
        if(i < node->children.size() - 1) {
            combinedCondition += " ";
            
            if(i+1 < node->children.size() && node->children[i+1] != nullptr) {
                combinedCondition += " " + node->children[i+1]->value + " ";
            }
            
            i++;
        }
    }
    
    return combinedCondition;
}

/*
(COPY,x,y) // y := x
(ADD,x,y,z) // z := x + y
(SUB,x,y,z) // z := x - y
(MUL,x,y,z) // z := x * y
(DIV,x,y,z) // z := x / y
(MOD,x,y,z) // z := x mod y
(REM,x,y,z) // z := x rem y
(POWER,x,y,z) // z := pow x, y
(SHL,x,y,z) // z := x << y
(SHR,x,y,z) // z := x >> y
(SAR,x,y,z) // z := x >>> y
(AND,x,y,z) // z := x & y
(OR,x,y,z) // z := x | y
(XOR,x,y,z) // z := x xor y
(NOT,x,y) // y := !x
(NEG,x,y) // y := -x
(COMP,x,y) // y := ~x
(ABS,x,y) // y := abs x
(SIN,x,y) // y := sin x
(COS,x,y) // y := cos x
(ATAN,x,y,z) // z := atan x,y
(LN,x,y) // y := ln x
(SQRT,x,y) // y := sqrt x
(LT,x,y,z) // z := x < y
(LE,x,y,z) // z := x <= y
(EQ,x,y,z) // z := x == y
(NE,x,y,z) // z := x != y
(GE,x,y,z) // z := x >= y
(GT,x,y,z) // z := x > y
(MEM_ADDR,x,y) // y := &x
(MEM_GET,x,y) // y := *x
(MEM_SET,x,y) // *y := x
(ELEM_ADDR,a,i,x) // x := &(a[i])
(ELEM_GET,a,i,x) // x := a[i]
(ELEM_SET,a,i,x) // a[i] := x
(FIELD_ADDR,s,f,x) // x := &(s.f)
(FIELD_GET,s,f,x) // x := s.f
(FIELD_SET,s,f,x) // s.f := x
(LABEL,L) // Label
(JUMP,L) // Unconditional jump to a label
(JZERO,x,L) // Jump if zero / Jump if false
(JNZERO,x,L) // Jump if zero / Jump if true
(JLT,x,y,L) // Jump if less / Jump if not greater or equal
(JLE,x,y,L) // Jump if less or equal / Jump if not greater
(JEQ,x,y,L) // Jump if equal
(JNE,x,y,L) // Jump if not equal
(JGE,x,y,L) // Jump if greater or equal / Jump if not less
(JGT,x,y,L) // Jump if greater / Jump if not less
(MULADD,x,y,z,w) // w := x + y*z
(IJ,x,dx,L) // x := x + dx; goto L
(IJE,x,dx,y,L) // x += dx; if x == y goto L
(DJNZ,x,L) // Decrement and jump if not zero
(CALLP,f,x1,...,xn) // Call procedure (non-value returning function) f, with arguments x1, ..., xn
(CALLF,f,x1,...,xn,x) // Call function f, with arguments x1, ..., xn, and copy the result into x
(RETP) // Return from procedure
(RETF,x) // Return x from this function
(INT_TO_STR,x,y) // String representation of an integer
(FLOAT_TO_STR,x,y) // String representation of a float
(BOOL_TO_STR,x,y) // String representation of a boolean
(CHAR_TO_STR,x,y) // String representation of a character
(ALLOC,x,y) // Allocate x bytes of memory, returning the address in y
(ARRAY_ALLOC,x,y) // Allocate x*elsize bytes of memory, where elsize is the number of bytes of an element in array x, returning the address in y
(DEALLOC,x) // Deallocate the memory at address x
(INT_TO_FLOAT,x,y) // Integer to float
(ASSERT_NOT_NULL,x) // Do something if x is null
(ASSERT_NONZERO,x) // Do something if x is not```
 */
