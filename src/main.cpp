#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "codegen/codegen.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Incorrect usage. Usage is..." << std::endl;
    std::cerr << "MirageC <input.mir>" << std::endl;
    return EXIT_FAILURE;
  }

  std::ifstream file(argv[1]);
  if (!file.is_open()) {
    std::cerr << "Unable to open file: " << argv[1] << std::endl;
    return EXIT_FAILURE;
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  file.close();
  std::string file_contents = buffer.str();

  auto tokens = mirage::tokenize(file_contents);
  mirage::print_tokens(tokens);

  auto ast = mirage::parse(tokens);
  std::string asm_code = mirage::generate(*ast);

  std::ofstream out("output.asm");
  out << asm_code;
  out.close();

  // TODO: switch to fork/ execl
  system("nasm -f elf64 output.asm -o output.o");
  system("ld output.o -o program");


  return EXIT_SUCCESS;
}
