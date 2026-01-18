#include "parser.hpp"

#include <stdexcept>

namespace mirage {

std::unique_ptr<Program> parse(const std::vector<Token>& tokens) {
  auto program = std::make_unique<Program>();

  size_t pos = 0;

  if (tokens[pos].type != TokenType::KEYWORD_RETURN) {
    throw std::runtime_error("expected 'return'");
  }
  pos++;

  if (tokens[pos].type != TokenType::INT_LITERAL) {
    throw std::runtime_error("expected int literal");
  }
  const int value = std::stoi(tokens[pos++].value.value());

  if (tokens[pos].type != TokenType::SEMICOLON) {
    throw std::runtime_error("expected ';' after return statement");
  }

  auto ret_stmt = std::make_unique<ReturnStatement>();
  ret_stmt->value = value;
  program->statement = std::move(ret_stmt);

  return program;
}

}
