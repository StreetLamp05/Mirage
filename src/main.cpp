#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "codegen/codegen.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/wait.h>

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



  int status;
  pid_t pid = fork();
  if (pid == 0) {
    execl("/usr/bin/nasm", "nasm", "-f", "elf64", "output.asm", "-o", "output.o", nullptr);
    perror("execl nasm failed");
    _exit(EXIT_FAILURE);
  }
  waitpid(pid, &status, 0);
  if (!WIFEXITED(status)) {
    fprintf(stderr, "NASM assembler killed by signal %d\n", WTERMSIG(status));
    return EXIT_FAILURE;
  } else if (WEXITSTATUS(status) != EXIT_SUCCESS) {
    fprintf(stderr, "NASM assembler failed with status %d\n", WEXITSTATUS(status));
    return EXIT_FAILURE;
  }

  pid = fork();
  if (pid == 0) {
    execl("/usr/bin/ld", "ld", "output.o", "-o", "program", nullptr);
    perror("execl ld failed");
    _exit(EXIT_FAILURE);
  }
  waitpid(pid, &status, 0);
  if (!WIFEXITED(status)) {
    fprintf(stderr, "Linker killed by signal %d\n", WTERMSIG(status));
    return EXIT_FAILURE;
  } else if (WEXITSTATUS(status) != EXIT_SUCCESS) {
    fprintf(stderr, "Linker failed with status %d\n", WEXITSTATUS(status));
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
