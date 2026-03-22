#ifndef MIRAGEC_AST_HPP
#define MIRAGEC_AST_HPP

#include <memory>
#include <string>
#include <vector>
#include <optional>
#include "../lexer/lexer.hpp"

namespace mirage {

  struct ASTNode {
    virtual ~ASTNode() = default;
  };

  struct Expression : ASTNode {
    SourceLocation loc;
  };

  struct IntLiteral : Expression {
    int value;
  };

  struct StringLiteral : Expression {
    std::string value;
  };

  struct BoolLiteral : Expression {
    bool value;
  };

  struct Identifier : Expression {
    std::string name;
  };

  struct BinaryExpr : Expression {
    std::unique_ptr<Expression> left;
    TokenType op;
    std::unique_ptr<Expression> right;
  };

  struct FunctionCall : Expression {
    std::string name;
    std::vector<std::unique_ptr<Expression>> args;
  };


  struct Statement : ASTNode {
    SourceLocation loc;
  };


  struct ReturnStatement : Statement {
    std::unique_ptr<Expression> value;
  };

  struct VarDeclaration : Statement {
    std::string name;
    std::optional<std::string> type_name;
    std::unique_ptr<Expression> initializer;
  };

  struct ConstDeclaration : Statement {
    std::string name;
    std::optional<std::string> type_name;
    std::unique_ptr<Expression> initializer;
  };

  struct ExpressionStatement : Statement {
    std::unique_ptr<Expression> expr;
  };

  struct Parameter {
    std::string name;
    std::string type_name;
  };

  struct FunctionDeclaration : ASTNode {
    std::string name;
    std::vector<Parameter> params;
    std::optional<std::string> return_type_name;
    std::vector<std::unique_ptr<Statement>> body;
  };

  struct Program : ASTNode {
    std::vector<std::unique_ptr<FunctionDeclaration>> functions;
  };

}

#endif
