#include "codegen.hpp"

#include <sstream>

namespace mirage {

std::string generate(const Program& program) {
  std::stringstream asm_code;
  asm_code << "global _start\n";
  asm_code << "_start:\n";
  asm_code << "  mov rax, 60\n";
  asm_code << "  mov rdi, " << program.statement->value << "\n";
  asm_code << "  syscall\n";
  return asm_code.str();
}

}
