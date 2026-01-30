#ifndef MIRAGEC_LEXER_HPP
#define MIRAGEC_LEXER_HPP

#include <optional>
#include <string>
#include <vector>

namespace mirage {

enum class TokenType {
    EOF_TOKEN,

  /* Literals */
  INT_LITERAL,
  FLOAT_LITERAL,
  STRING_LITERAL,
  BOOL_LITERAL,
  /* Identifiers */
  IDENTIFIER,
  /* Operators */

  // arithmetic operators
  PLUS,
  MINUS,
  STAR,
  SLASH,
  PERCENT,
  // comparison operators
  EQ_EQ,
  BANG_EQ,
  LESS,
  GREATER,
  LESS_EQ,
  GREATER_EQ,
  // logical operators
  AMP_AMP,
  PIPE_PIPE,
  BANG,

  // bitwise operators
  AMP,
  PIPE,
  CARET,
  TILDE,
  LESS_LESS,
  GREATER_GREATER,

  // assignment operators
  EQ,
  PLUS_EQ,
  MINUS_EQ,
  STAR_EQ,
  SLASH_EQ,
  PERCENT_EQ,
  AMP_EQ,
  PIPE_EQ,
  CARET_EQ,
  LESS_LESS_EQ,
  GREATER_GREATER_EQ,

  // increment/ decrement
  PLUS_PLUS,
  MINUS_MINUS,

  // special operators
  ARROW,
  COLON_COLON,
  QUESTION,
  DOT_DOT,
  DOT_DOT_EQ,

  // punctuators
  SEMICOLON,
  COMMA,
  DOT,
  COLON,
  LPAREN,
  RPAREN,
  LBRACE,
  RBRACE,
  LBRACKET,
  RBRACKET,

  /* Keywords */

  // type keywords
  // ints
  KW_INT,
  KW_INT8,
  KW_INT16,
  KW_INT32,
  KW_INT64,

  KW_UINT8,
  KW_UINT16,
  KW_UINT32,
  KW_UINT64,

  // floats
  KW_FLOAT,
  KW_FLOAT32,
  KW_FLOAT64,

  KW_BOOL,
  KW_STRING,
  KW_BYTE,


  // general keywords
  KW_FUNC,
  KW_VAR,
  KW_CONST,
  KW_RETURN,
  KW_IF,
  KW_ELSE,
  KW_WHILE,
  KW_FOR,
  KW_IN,
  KW_MATCH,
  KW_BREAK,
  KW_CONTINUE,
  KW_STRUCT,
  KW_ENUM,
  KW_MOD,
  KW_USE,
  KW_PUB,
  KW_SPAWN,
  KW_UNSAFE,
  KW_MUT,
  KW_TRUE,
  KW_FALSE,


};

  std::string to_string(TokenType type);

  struct SourceLocation {
    size_t line;
    size_t column;
  };


  struct Token {
    TokenType type;
    std::optional<std::string> value;
    SourceLocation loc;
  };


  std::vector<Token> tokenize(const std::string& source, const std::string& filename = "<stdin>");
  void print_tokens(const std::vector<Token>& tokens);

}
#endif
