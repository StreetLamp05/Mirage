#ifndef MIRAGEC_CODEGEN_HPP
#define MIRAGEC_CODEGEN_HPP

#include "../parser/ast.hpp"

#include <string>

namespace mirage {

  std::string generate(const Program& program);

}

#endif
