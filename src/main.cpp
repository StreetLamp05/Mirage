#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

enum class TokenType {
  _return,
  int_lit,
  semi,
};

struct Token {
  TokenType type;
  std::optional<std::string> value;
};


/**
 * Lexer to extract tokens from file contents. Classifies tokens into 7 classes: Identifier, Operator, Constants,
 * Keyword, Literals, Punctuators, Special Characters.
 * https://www.youtube.com/watch?v=MZ9NZdZteG4
 *
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

vector <Token> tokenize(string input) {
  vector <Token> tokens;
  return tokens;
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
  std:: cout << "File contents: \n" << file_contents << std::endl;

  file.close();

  tokenize(file_contents);

  return EXIT_SUCCESS;
}
