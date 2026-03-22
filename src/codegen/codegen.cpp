#include "codegen.hpp"

#include <sstream>
#include <unordered_map>
#include <iomanip>

namespace mirage {

class CodeGen {
public:
    std::string run(const Program& program) {
        for (const auto& func : program.functions) {
            generate_function(*func);
        }

        std::string result;
        result += globals_.str();
        result += "\n";
        if (uses_puts_) result += "declare i32 @puts(i8*)\n";
        result += "\n";
        result += output_.str();
        return result;
    }

private:
    std::stringstream output_;
    std::stringstream globals_;
    int str_counter_ = 0;
    int reg_counter_ = 0;
    bool uses_puts_ = false;
    std::unordered_map<std::string, std::string> symbols_;

    std::string next_reg() {
        return "%" + std::to_string(reg_counter_++);
    }

    // escape a string for LLVM IR: \n -> \0A, \0 -> \00, etc.
    std::string escape_llvm_string(const std::string& s) {
        std::string result;
        for (char c : s) {
            if (c == '\n') { result += "\\0A"; }
            else if (c == '\t') { result += "\\09"; }
            else if (c == '\r') { result += "\\0D"; }
            else if (c == '\0') { result += "\\00"; }
            else if (c == '\\') { result += "\\5C"; }
            else if (c == '"') { result += "\\22"; }
            else { result += c; }
        }
        return result;
    }

    // add a string constant to globals, return the global name
    std::string add_string_constant(const std::string& value) {
        std::string name = "@.str." + std::to_string(str_counter_++);
        int len = value.size() + 1; // +1 for null terminator
        globals_ << name << " = private constant [" << len << " x i8] c\""
                 << escape_llvm_string(value) << "\\00\"\n";
        return name;
    }

    // ── function generation ──

    void generate_function(const FunctionDeclaration& func) {
        reg_counter_ = 0;
        symbols_.clear();

        // for now hardcode main's signature
        if (func.name == "main") {
            output_ << "define i32 @main(i32 %argc, i8** %argv) {\n";
        } else {
            output_ << "define i32 @" << func.name << "() {\n";
        }
        output_ << "entry:\n";

        bool has_return = false;
        for (const auto& stmt : func.body) {
            generate_statement(*stmt);
            if (dynamic_cast<const ReturnStatement*>(stmt.get())) {
                has_return = true;
            }
        }

        if (!has_return) {
            output_ << "  ret i32 0\n";
        }

        output_ << "}\n\n";
    }

    // ── statement generation ──

    void generate_statement(const Statement& stmt) {
        if (auto* ret = dynamic_cast<const ReturnStatement*>(&stmt)) {
            if (ret->value) {
                auto val = generate_expression(*ret->value);
                output_ << "  ret i32 " << val << "\n";
            } else {
                output_ << "  ret i32 0\n";
            }
        }
        else if (auto* var = dynamic_cast<const VarDeclaration*>(&stmt)) {
            auto alloca_name = "%" + var->name;
            output_ << "  " << alloca_name << " = alloca i32\n";
            symbols_[var->name] = alloca_name;
            if (var->initializer) {
                auto val = generate_expression(*var->initializer);
                output_ << "  store i32 " << val << ", i32* " << alloca_name << "\n";
            }
        }
        else if (auto* con = dynamic_cast<const ConstDeclaration*>(&stmt)) {
            auto alloca_name = "%" + con->name;
            output_ << "  " << alloca_name << " = alloca i32\n";
            symbols_[con->name] = alloca_name;
            if (con->initializer) {
                auto val = generate_expression(*con->initializer);
                output_ << "  store i32 " << val << ", i32* " << alloca_name << "\n";
            }
        }
        else if (auto* expr_stmt = dynamic_cast<const ExpressionStatement*>(&stmt)) {
            generate_expression(*expr_stmt->expr);
        }
    }

    // ── expression generation ──
    // returns a string that is the LLVM value (register name or constant)

    std::string generate_expression(const Expression& expr) {
        if (auto* lit = dynamic_cast<const IntLiteral*>(&expr)) {
            return std::to_string(lit->value);
        }

        if (auto* lit = dynamic_cast<const StringLiteral*>(&expr)) {
            auto global_name = add_string_constant(lit->value);
            int len = lit->value.size() + 1;
            auto reg = next_reg();
            output_ << "  " << reg << " = getelementptr ["
                    << len << " x i8], [" << len << " x i8]* "
                    << global_name << ", i32 0, i32 0\n";
            return reg;
        }

        if (auto* lit = dynamic_cast<const BoolLiteral*>(&expr)) {
            return lit->value ? "1" : "0";
        }

        if (auto* id = dynamic_cast<const Identifier*>(&expr)) {
            auto it = symbols_.find(id->name);
            if (it == symbols_.end()) {
                throw std::runtime_error("undefined variable: " + id->name);
            }
            auto reg = next_reg();
            output_ << "  " << reg << " = load i32, i32* " << it->second << "\n";
            return reg;
        }

        if (auto* bin = dynamic_cast<const BinaryExpr*>(&expr)) {
            auto left = generate_expression(*bin->left);
            auto right = generate_expression(*bin->right);
            auto reg = next_reg();

            switch (bin->op) {
                case TokenType::PLUS:
                    output_ << "  " << reg << " = add i32 " << left << ", " << right << "\n";
                    break;
                case TokenType::MINUS:
                    output_ << "  " << reg << " = sub i32 " << left << ", " << right << "\n";
                    break;
                case TokenType::STAR:
                    output_ << "  " << reg << " = mul i32 " << left << ", " << right << "\n";
                    break;
                case TokenType::SLASH:
                    output_ << "  " << reg << " = sdiv i32 " << left << ", " << right << "\n";
                    break;
                case TokenType::PERCENT:
                    output_ << "  " << reg << " = srem i32 " << left << ", " << right << "\n";
                    break;
                case TokenType::EQ_EQ:
                    output_ << "  " << reg << " = icmp eq i32 " << left << ", " << right << "\n";
                    break;
                case TokenType::BANG_EQ:
                    output_ << "  " << reg << " = icmp ne i32 " << left << ", " << right << "\n";
                    break;
                case TokenType::LESS:
                    output_ << "  " << reg << " = icmp slt i32 " << left << ", " << right << "\n";
                    break;
                case TokenType::GREATER:
                    output_ << "  " << reg << " = icmp sgt i32 " << left << ", " << right << "\n";
                    break;
                case TokenType::LESS_EQ:
                    output_ << "  " << reg << " = icmp sle i32 " << left << ", " << right << "\n";
                    break;
                case TokenType::GREATER_EQ:
                    output_ << "  " << reg << " = icmp sge i32 " << left << ", " << right << "\n";
                    break;
                default:
                    throw std::runtime_error("unsupported binary operator");
            }
            return reg;
        }

        if (auto* call = dynamic_cast<const FunctionCall*>(&expr)) {
            if (call->name == "println") {
                uses_puts_ = true;
                if (call->args.empty()) {
                    // println() with no args — print empty line
                    auto global = add_string_constant("");
                    auto ptr = next_reg();
                    output_ << "  " << ptr << " = getelementptr [1 x i8], [1 x i8]* "
                            << global << ", i32 0, i32 0\n";
                    auto reg = next_reg();
                    output_ << "  " << reg << " = call i32 @puts(i8* " << ptr << ")\n";
                    return reg;
                }
                auto arg = generate_expression(*call->args[0]);
                auto reg = next_reg();
                output_ << "  " << reg << " = call i32 @puts(i8* " << arg << ")\n";
                return reg;
            }
            // generic function call (not yet fully supported)
            throw std::runtime_error("unsupported function call: " + call->name);
        }

        throw std::runtime_error("unsupported expression type in codegen");
    }
};

std::string generate(const Program& program) {
    CodeGen codegen;
    return codegen.run(program);
}

}
