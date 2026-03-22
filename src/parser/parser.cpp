#include "parser.hpp"
#include <stdexcept>


namespace mirage {

    class Parser {
    private:
        const std::vector<Token>& tokens_;
        size_t pos_ = 0;

        bool at_end() {
            return pos_ >= tokens_.size() || tokens_[pos_].type == TokenType::EOF_TOKEN;
        }

        Token current() {
            return tokens_[pos_];
        }

        Token peek(size_t offset) {
            size_t idx = pos_ + offset;
            if (idx >= tokens_.size()) return tokens_.back();
            return tokens_[idx];
        }

        Token advance() {
            return tokens_[pos_++];
        }

        bool match(TokenType type) {
            if (current().type == type) {
                advance();
                return true;
            }
            return false;
        }

        Token expect(TokenType type, const std::string& msg) {
            if (current().type == type) return advance();
            error(msg);
        }

        [[noreturn]] void error(const std::string& msg) {
            auto loc = current().loc;
            throw std::runtime_error(
                std::to_string(loc.line) + ":" + std::to_string(loc.column) + ": " + msg
            );
        }

        std::unique_ptr<Program> parse_program() {
            auto program = std::make_unique<Program>();
            while (!at_end()) {
                auto token = current();
                if (token.type == TokenType::KW_FUNC) {
                    program->functions.push_back(parse_function());
                } else {
                    error("expected function declaration");
                }
            };
            return program;
        }

        // helper: parse a type name like "int", "string", "[]string"
        std::string parse_type_name() {
            if (match(TokenType::LBRACKET)) {
                expect(TokenType::RBRACKET, "expected ']' in slice type");
                return "[]" + parse_type_name();
            }
            auto tok = advance();
            if (tok.value.has_value()) return tok.value.value();
            error("expected type name");
        }

        std::unique_ptr<FunctionDeclaration> parse_function() {
            expect(TokenType::KW_FUNC, "expected 'func'");
            auto name = expect(TokenType::IDENTIFIER, "expected function name").value.value();
            expect(TokenType::LPAREN, "expected '('");

            // parse parameters
            std::vector<Parameter> params;
            while (current().type != TokenType::RPAREN) {
                auto param_name = expect(TokenType::IDENTIFIER, "expected parameter name").value.value();
                auto type_name = parse_type_name();
                params.push_back(Parameter{param_name, type_name});
                if (current().type != TokenType::RPAREN) {
                    expect(TokenType::COMMA, "expected ',' or ')' in parameter list");
                }
            }
            expect(TokenType::RPAREN, "expected ')'");

            // parse optional return type
            std::optional<std::string> return_type;
            if (match(TokenType::ARROW)) {
                return_type = parse_type_name();
            }

            // parse body
            expect(TokenType::LBRACE, "expected '{'");
            std::vector<std::unique_ptr<Statement>> body;
            while (current().type != TokenType::RBRACE) {
                body.push_back(parse_statement());
            }
            expect(TokenType::RBRACE, "expected '}'");

            auto func = std::make_unique<FunctionDeclaration>();
            func->name = name;
            func->params = std::move(params);
            func->return_type_name = return_type;
            func->body = std::move(body);
            return func;
        }


        // ── type detection helper ──

        bool is_type_token() {
            auto t = current().type;
            return t == TokenType::KW_INT    || t == TokenType::KW_INT8   ||
                   t == TokenType::KW_INT16  || t == TokenType::KW_INT32  ||
                   t == TokenType::KW_INT64  || t == TokenType::KW_UINT8  ||
                   t == TokenType::KW_UINT16 || t == TokenType::KW_UINT32 ||
                   t == TokenType::KW_UINT64 || t == TokenType::KW_FLOAT  ||
                   t == TokenType::KW_FLOAT32 || t == TokenType::KW_FLOAT64 ||
                   t == TokenType::KW_BOOL   || t == TokenType::KW_STRING ||
                   t == TokenType::KW_BYTE   || t == TokenType::LBRACKET;
        }

        // ── expression parsing (precedence climbing) ──

        std::unique_ptr<Expression> parse_expression() {
            return parse_comparison();
        }

        std::unique_ptr<Expression> parse_comparison() {
            auto left = parse_additive();
            while (current().type == TokenType::EQ_EQ   || current().type == TokenType::BANG_EQ ||
                   current().type == TokenType::LESS     || current().type == TokenType::GREATER ||
                   current().type == TokenType::LESS_EQ  || current().type == TokenType::GREATER_EQ) {
                auto loc = current().loc;
                auto op = advance().type;
                auto right = parse_additive();
                auto node = std::make_unique<BinaryExpr>();
                node->loc = loc;
                node->left = std::move(left);
                node->op = op;
                node->right = std::move(right);
                left = std::move(node);
            }
            return left;
        }

        std::unique_ptr<Expression> parse_additive() {
            auto left = parse_multiplicative();
            while (current().type == TokenType::PLUS || current().type == TokenType::MINUS) {
                auto loc = current().loc;
                auto op = advance().type;
                auto right = parse_multiplicative();
                auto node = std::make_unique<BinaryExpr>();
                node->loc = loc;
                node->left = std::move(left);
                node->op = op;
                node->right = std::move(right);
                left = std::move(node);
            }
            return left;
        }

        std::unique_ptr<Expression> parse_multiplicative() {
            auto left = parse_primary();
            while (current().type == TokenType::STAR  || current().type == TokenType::SLASH ||
                   current().type == TokenType::PERCENT) {
                auto loc = current().loc;
                auto op = advance().type;
                auto right = parse_primary();
                auto node = std::make_unique<BinaryExpr>();
                node->loc = loc;
                node->left = std::move(left);
                node->op = op;
                node->right = std::move(right);
                left = std::move(node);
            }
            return left;
        }

        std::unique_ptr<Expression> parse_primary() {
            auto loc = current().loc;

            // integer literal
            if (current().type == TokenType::INT_LITERAL) {
                auto tok = advance();
                auto node = std::make_unique<IntLiteral>();
                node->loc = loc;
                node->value = std::stoi(tok.value.value());
                return node;
            }

            // string literal
            if (current().type == TokenType::STRING_LITERAL) {
                auto tok = advance();
                auto node = std::make_unique<StringLiteral>();
                node->loc = loc;
                node->value = tok.value.value();
                return node;
            }

            // bool literals
            if (current().type == TokenType::KW_TRUE) {
                advance();
                auto node = std::make_unique<BoolLiteral>();
                node->loc = loc;
                node->value = true;
                return node;
            }
            if (current().type == TokenType::KW_FALSE) {
                advance();
                auto node = std::make_unique<BoolLiteral>();
                node->loc = loc;
                node->value = false;
                return node;
            }

            // identifier or function call
            if (current().type == TokenType::IDENTIFIER) {
                auto name = advance().value.value();

                // function call: name(args...)
                if (current().type == TokenType::LPAREN) {
                    advance(); // consume (
                    std::vector<std::unique_ptr<Expression>> args;
                    while (current().type != TokenType::RPAREN) {
                        args.push_back(parse_expression());
                        if (current().type != TokenType::RPAREN) {
                            expect(TokenType::COMMA, "expected ',' or ')' in argument list");
                        }
                    }
                    expect(TokenType::RPAREN, "expected ')'");
                    auto node = std::make_unique<FunctionCall>();
                    node->loc = loc;
                    node->name = name;
                    node->args = std::move(args);
                    return node;
                }

                // plain identifier
                auto node = std::make_unique<Identifier>();
                node->loc = loc;
                node->name = name;
                return node;
            }

            // parenthesized expression
            if (current().type == TokenType::LPAREN) {
                advance(); // consume (
                auto expr = parse_expression();
                expect(TokenType::RPAREN, "expected ')'");
                return expr;
            }

            error("expected expression");
        }

        // ── statement parsing ──

        std::unique_ptr<Statement> parse_var_declaration() {
            auto loc = current().loc;
            advance(); // consume KW_VAR
            auto name = expect(TokenType::IDENTIFIER, "expected variable name").value.value();

            std::optional<std::string> type_name;
            if (is_type_token()) {
                type_name = parse_type_name();
            }

            std::unique_ptr<Expression> initializer;
            if (match(TokenType::EQ)) {
                initializer = parse_expression();
            }

            expect(TokenType::SEMICOLON, "expected ';' after variable declaration");

            auto decl = std::make_unique<VarDeclaration>();
            decl->loc = loc;
            decl->name = name;
            decl->type_name = type_name;
            decl->initializer = std::move(initializer);
            return decl;
        }

        std::unique_ptr<Statement> parse_const_declaration() {
            auto loc = current().loc;
            advance(); // consume KW_CONST
            auto name = expect(TokenType::IDENTIFIER, "expected constant name").value.value();

            std::optional<std::string> type_name;
            if (is_type_token()) {
                type_name = parse_type_name();
            }

            expect(TokenType::EQ, "expected '=' in const declaration");
            auto initializer = parse_expression();

            expect(TokenType::SEMICOLON, "expected ';' after const declaration");

            auto decl = std::make_unique<ConstDeclaration>();
            decl->loc = loc;
            decl->name = name;
            decl->type_name = type_name;
            decl->initializer = std::move(initializer);
            return decl;
        }

        std::unique_ptr<Statement> parse_return_statement() {
            auto loc = current().loc;
            advance(); // consume KW_RETURN

            std::unique_ptr<Expression> value;
            if (current().type != TokenType::SEMICOLON) {
                value = parse_expression();
            }

            expect(TokenType::SEMICOLON, "expected ';' after return statement");

            auto stmt = std::make_unique<ReturnStatement>();
            stmt->loc = loc;
            stmt->value = std::move(value);
            return stmt;
        }

        std::unique_ptr<Statement> parse_expression_statement() {
            auto loc = current().loc;
            auto expr = parse_expression();
            expect(TokenType::SEMICOLON, "expected ';' after expression");

            auto stmt = std::make_unique<ExpressionStatement>();
            stmt->loc = loc;
            stmt->expr = std::move(expr);
            return stmt;
        }

        std::unique_ptr<Statement> parse_statement() {
            switch (current().type) {
                case TokenType::KW_VAR:
                    return parse_var_declaration();
                case TokenType::KW_CONST:
                    return parse_const_declaration();
                case TokenType::KW_RETURN:
                    return parse_return_statement();
                default:
                    return parse_expression_statement();
            }
        }


    public:
        Parser(const std::vector<Token>& tokens) : tokens_(tokens) {}



        std::unique_ptr<Program> run() {
            return parse_program();
        }
    }; // class Parser

    std::unique_ptr<Program> parse(const std::vector<Token>& tokens) {
        Parser parser(tokens);
        return parser.run();
    }

} // namespace mirage