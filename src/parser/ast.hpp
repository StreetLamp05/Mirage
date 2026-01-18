#ifndef MIRAGEC_AST_HPP
#define MIRAGEC_AST_HPP

#include <memory>

namespace mirage {

struct ASTNode {
  virtual ~ASTNode() = default;
};

struct ReturnStatement : ASTNode {
  int value;
};

struct Program : ASTNode {
  std::unique_ptr<ReturnStatement> statement;
};

}

#endif
