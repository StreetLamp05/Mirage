#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>


enum class TokenType {

  /* Literals */
  INT_LITERAL,
  /* Identifiers */

  /* Operators */

  /* Constants */

  /* Keyword */
  KEYWORD_RETURN,

  // types
  KEYWORD_INT,

  /* Punctuators */
  SEMICOLON,
};

std::string to_string(TokenType type) {
  switch (type) {
    case TokenType::INT_LITERAL: return "INT_LITERAL";
    case TokenType::SEMICOLON: return "SEMICOLON";
    case TokenType::KEYWORD_RETURN: return "KEYWORD_RETURN";
    case TokenType::KEYWORD_INT: return "KEYWORD_INT";
  }
  return "UNKNOWN";
}





struct Token {
  TokenType type;
  std::optional<std::string> value;
};

/**
 * DEBUG: prints all the tokens in a vector of tokens.
 * @param tokens
 */
void print_tokens(const std::vector<Token>& tokens) {
  for (const auto& token: tokens) {
    std::cout << to_string(token.type) << std::endl;
  } // for
} // print_tokens

/**
 * Lexer to extract tokens from file contents. Classifies tokens into 7 classes: Identifier, Operator, Constants,
 * Keyword, Literals, Punctuators, Special Characters.
 * https://www.youtube.com/watch?v=MZ9NZdZteG4
 * https://en.wikipedia.org/wiki/Lexical_analysis
 *
 * @param input
 * @return
 */
/**
 * TODO:
identifiers
operator
constants
keyword
literals
punctuators
special characters
 */

std::vector <Token> tokenize(const std::string& source) {
  std::vector<Token> tokens;
  size_t pos = 0;


  std::cerr << "source length: "  << source.length() << std::endl;
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
    } // if

    else if (isdigit(source[pos])) {
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



struct ASTNode {
  virtual ~ASTNode() = default;
};

struct ReturnStatement: ASTNode {
  int value;
};

struct Program: ASTNode {
  std::unique_ptr<ReturnStatement> statement;
};


std::unique_ptr<ASTNode> parse(const std::vector<Token>& tokens) {
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
  ret_stmt-> value = value;
  program -> statement = std::move(ret_stmt);

  return program;
} // parse


std::string generate(const Program& program) {
  std::stringstream asm_code;
  asm_code << "global _start\n";
  asm_code << "_start:\n";
  asm_code << "  mov rax, 60\n";
  asm_code << "  mov rdi, " << program.statement->value << "\n";
  asm_code << "  syscall\n";
  return asm_code.str();
}



int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Incorrect usage. Usage is..." << std::endl;
    std::cerr << "MirageC <input.mir>" << argv[0] << " <file>" << std::endl;
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
  file.close();

  auto tokens = tokenize(file_contents);

  print_tokens(tokens);


  auto ast = parse(tokens);
  std::string asm_code = generate(dynamic_cast<const Program &>(*ast));

  std::ofstream out("output.asm");
  out << asm_code;
  out.close();

  system("nasm -f elf64 output.asm -o output.o");
  system("ld output.o -o program");

  return EXIT_SUCCESS;
}

