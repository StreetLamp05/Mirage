#ifndef MIRAGEC_LEXER_HPP
#define MIRAGEC_LEXER_HPP

#include <optional>
#include <string>
#include <vector>

namespace mirage {

enum class TokenType {
  /* Literals */
  INT_LITERAL,

  /* Identifiers */

  /* Operators */

  /* Constants */

  /* Keywords */
  KEYWORD_RETURN,
  KEYWORD_INT,

  /* Punctuators */
  SEMICOLON,
};

std::string to_string(TokenType type);

struct Token {
  TokenType type;
  std::optional<std::string> value;
};

std::vector<Token> tokenize(const std::string& source);
void print_tokens(const std::vector<Token>& tokens);

}

#endif
