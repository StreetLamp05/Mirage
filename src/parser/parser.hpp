#ifndef MIRAGEC_PARSER_HPP
#define MIRAGEC_PARSER_HPP

#include "ast.hpp"
#include "../lexer/lexer.hpp"

#include <memory>
#include <vector>

namespace mirage {

std::unique_ptr<Program> parse(const std::vector<Token>& tokens);

}

#endif
