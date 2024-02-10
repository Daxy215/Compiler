#pragma once

namespace LexerNameSpace {
 enum TokenType {
  // ???
  IDENTIFIER,
  KEYWORD,
    
  // Statements
  BREAK_STATEMENT,
  CONTINUE_STATEMENT,
  RETURN_STATEMENT,
    
  // Operations
  OPERATOR,
    
  // Statement types
  FOR_LOOP,
  WHILE_LOOP,
  DO_WHILE_LOOP, // Bruhhhhhhhhhhhhhhhhhhhhhh im too lazy
  IF_STATEMENT,
  ELSE_STATEMENT,
  SWITCH_STATEMENT,
  // Logical operations
  LOGICAL_AND,
  LOGICAL_OR,
  EQUAL,
  NOT_EQUAL,
  LESS_THAN_EQUAL,
  GREATER_THAN_EQUAL,
  LEFT_SHIFT_ASSIGN,
  RIGHT_SHIFT_ASSIGN,
  POINTER_MEMBER_ACCESS,
  SCOPE_POINTER_MEMBER_ACCESS,
  ELLIPSIS,
  SINGLE_LINE_COMMENT,
  MULTI_LINE_COMMENT_START,
  MULTI_LINE_COMMENT_END,
  MODULUS_ASSIGN,
  BITWISE_XOR_ASSIGN,
  BITWISE_OR_ASSIGN,
  BITWISE_AND_ASSIGN,
  BITWISE_XOR,
  BITWISE_OR,
  MODULUS,
  BITWISE_NOT,
  BITWISE_AND,
    
  // Special cases
  /*INCREMENT,
  INCREMENT_EQUAL,
  DECREMENT,
  DECREMENT_EQUAL,
  MULTIPLICATION_EQUAL,
  DIVISION_EQUAL,*/
    
  POINTER,
  REFERENCE,
    
  // Variable types
  STRING_LITERAL,
  INTEGER_LITERAL,
  FLOATING_POINT_LITERAL,

  // Modifiers??
  STRUCT,
  CLASS,
  ENUM,
    
  // ()
  /**
   * \brief (
   */
  LEFT_PAREN,

  /**
   * \brief )
   */
  RIGHT_PAREN,

  // {}
  /**
   * \brief {
   */
  LEFT_BRACE,

  /**
   * \brief }
   */
  RIGHT_BRACE,
    
  // []
  /**
   * \brief [
   */
  LEFT_SQUARE_BRACE,

  /**
   * \brief ]
   */
  RIGHT_SQUARE_BRACE,
    
  // <>
  /**
   * \brief <
   */
  LEFT_ANGLE_BRACE,

  /**
   * \brief >
   */
  RIGHT_ANGLE_BRACE,
    
  // Something..
  /**
   * \brief ;
   */
  SEMICOLON,
  COMMA,
  COLON,
  DOT,
    
  // Idek
  NAMESPACE,
  PREPROCESSOR_DIRECTIVE,
};
}