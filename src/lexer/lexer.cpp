#include "lexer.hpp"

#include <iostream>

namespace mirage {

std::string to_string(TokenType type) {
  switch (type) {
    case TokenType::INT_LITERAL: return "INT_LITERAL";
    case TokenType::SEMICOLON: return "SEMICOLON";
    case TokenType::KEYWORD_RETURN: return "KEYWORD_RETURN";
    case TokenType::KEYWORD_INT: return "KEYWORD_INT";
  }
  return "UNKNOWN";
}

void print_tokens(const std::vector<Token>& tokens) {
  for (const auto& token : tokens) {
    std::cout << to_string(token.type) << std::endl;
  }
}

/**
 * Lexer to extract tokens from file contents. Classifies tokens into 7 classes:
 * Identifier, Operator, Constants, Keyword, Literals, Punctuators, Special Characters.
 *
 * TODO:
 *   - identifiers
 *   - operator
 *   - constants
 *   - keyword
 *   - literals
 *   - punctuators
 *   - special characters
 */
std::vector<Token> tokenize(const std::string& source) {
  std::vector<Token> tokens;
  size_t pos = 0;

  std::cerr << "source length: " << source.length() << std::endl;
  std::cerr << "First chars (hex) " << std::endl;
  for (size_t i = 0; i < std::min(size_t(10), source.length()); i++) {
    std::cerr << std::hex << (int)source[i] << std::endl;
  }
  std::cerr << std::dec << std::endl;

  while (pos < source.length()) {
    // skip whitespace
    if (isspace(source[pos])) {
      pos++;
      continue;
    } // if

    if (source.substr(pos, 6) == "return") {
      tokens.push_back({TokenType::KEYWORD_RETURN, "return"});
      pos += 6;
    } else if (isdigit(source[pos])) {
      std::string num;
      while (pos < source.length() && isdigit(source[pos])) {
        num += source[pos++];
      } // while
      tokens.push_back({TokenType::INT_LITERAL, num});
    } else if (source[pos] == ';') {
      tokens.push_back({TokenType::SEMICOLON, ";"});
      pos++;
    } // if
  } // while

  return tokens;
} // tokenize

}
