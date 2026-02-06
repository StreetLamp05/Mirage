#include "lexer.hpp"

#include <iostream>
#include <stdexcept>
#include <unordered_map>

namespace mirage {

std::string to_string(TokenType type) {
    switch (type) {
        case TokenType::EOF_TOKEN:          return "EOF";

        // literals
        case TokenType::INT_LITERAL:        return "INT_LITERAL";
        case TokenType::FLOAT_LITERAL:      return "FLOAT_LITERAL";
        case TokenType::STRING_LITERAL:     return "STRING_LITERAL";
        case TokenType::BOOL_LITERAL:       return "BOOL_LITERAL";

        // identifier
        case TokenType::IDENTIFIER:         return "IDENTIFIER";

        // arithmetic
        case TokenType::PLUS:               return "PLUS";
        case TokenType::MINUS:              return "MINUS";
        case TokenType::STAR:               return "STAR";
        case TokenType::SLASH:              return "SLASH";
        case TokenType::PERCENT:            return "PERCENT";

        // comparison
        case TokenType::EQ_EQ:              return "EQ_EQ";
        case TokenType::BANG_EQ:            return "BANG_EQ";
        case TokenType::LESS:               return "LESS";
        case TokenType::GREATER:            return "GREATER";
        case TokenType::LESS_EQ:            return "LESS_EQ";
        case TokenType::GREATER_EQ:         return "GREATER_EQ";

        // logical
        case TokenType::AMP_AMP:            return "AMP_AMP";
        case TokenType::PIPE_PIPE:          return "PIPE_PIPE";
        case TokenType::BANG:               return "BANG";

        // bitwise
        case TokenType::AMP:                return "AMP";
        case TokenType::PIPE:               return "PIPE";
        case TokenType::CARET:              return "CARET";
        case TokenType::TILDE:              return "TILDE";
        case TokenType::LESS_LESS:          return "LESS_LESS";
        case TokenType::GREATER_GREATER:    return "GREATER_GREATER";

        // assignment
        case TokenType::EQ:                 return "EQ";
        case TokenType::PLUS_EQ:            return "PLUS_EQ";
        case TokenType::MINUS_EQ:           return "MINUS_EQ";
        case TokenType::STAR_EQ:            return "STAR_EQ";
        case TokenType::SLASH_EQ:           return "SLASH_EQ";
        case TokenType::PERCENT_EQ:         return "PERCENT_EQ";
        case TokenType::AMP_EQ:             return "AMP_EQ";
        case TokenType::PIPE_EQ:            return "PIPE_EQ";
        case TokenType::CARET_EQ:           return "CARET_EQ";
        case TokenType::LESS_LESS_EQ:       return "LESS_LESS_EQ";
        case TokenType::GREATER_GREATER_EQ: return "GREATER_GREATER_EQ";

        // increment/decrement
        case TokenType::PLUS_PLUS:          return "PLUS_PLUS";
        case TokenType::MINUS_MINUS:        return "MINUS_MINUS";

        // special operators
        case TokenType::ARROW:              return "ARROW";
        case TokenType::COLON_COLON:        return "COLON_COLON";
        case TokenType::QUESTION:           return "QUESTION";
        case TokenType::DOT_DOT:            return "DOT_DOT";
        case TokenType::DOT_DOT_EQ:         return "DOT_DOT_EQ";

        // punctuators
        case TokenType::SEMICOLON:          return "SEMICOLON";
        case TokenType::COMMA:              return "COMMA";
        case TokenType::DOT:                return "DOT";
        case TokenType::COLON:              return "COLON";
        case TokenType::LPAREN:             return "LPAREN";
        case TokenType::RPAREN:             return "RPAREN";
        case TokenType::LBRACE:             return "LBRACE";
        case TokenType::RBRACE:             return "RBRACE";
        case TokenType::LBRACKET:           return "LBRACKET";
        case TokenType::RBRACKET:           return "RBRACKET";

        // type keywords
        case TokenType::KW_INT:             return "KW_INT";
        case TokenType::KW_INT8:            return "KW_INT8";
        case TokenType::KW_INT16:           return "KW_INT16";
        case TokenType::KW_INT32:           return "KW_INT32";
        case TokenType::KW_INT64:           return "KW_INT64";
        case TokenType::KW_UINT8:           return "KW_UINT8";
        case TokenType::KW_UINT16:          return "KW_UINT16";
        case TokenType::KW_UINT32:          return "KW_UINT32";
        case TokenType::KW_UINT64:          return "KW_UINT64";
        case TokenType::KW_FLOAT:           return "KW_FLOAT";
        case TokenType::KW_FLOAT32:         return "KW_FLOAT32";
        case TokenType::KW_FLOAT64:         return "KW_FLOAT64";
        case TokenType::KW_BOOL:            return "KW_BOOL";
        case TokenType::KW_STRING:          return "KW_STRING";
        case TokenType::KW_BYTE:            return "KW_BYTE";

        // general keywords
        case TokenType::KW_FUNC:            return "KW_FUNC";
        case TokenType::KW_VAR:             return "KW_VAR";
        case TokenType::KW_CONST:           return "KW_CONST";
        case TokenType::KW_RETURN:          return "KW_RETURN";
        case TokenType::KW_IF:              return "KW_IF";
        case TokenType::KW_ELSE:            return "KW_ELSE";
        case TokenType::KW_WHILE:           return "KW_WHILE";
        case TokenType::KW_FOR:             return "KW_FOR";
        case TokenType::KW_IN:              return "KW_IN";
        case TokenType::KW_MATCH:           return "KW_MATCH";
        case TokenType::KW_BREAK:           return "KW_BREAK";
        case TokenType::KW_CONTINUE:        return "KW_CONTINUE";
        case TokenType::KW_STRUCT:          return "KW_STRUCT";
        case TokenType::KW_ENUM:            return "KW_ENUM";
        case TokenType::KW_MOD:             return "KW_MOD";
        case TokenType::KW_USE:             return "KW_USE";
        case TokenType::KW_PUB:             return "KW_PUB";
        case TokenType::KW_SPAWN:           return "KW_SPAWN";
        case TokenType::KW_UNSAFE:          return "KW_UNSAFE";
        case TokenType::KW_MUT:             return "KW_MUT";
        case TokenType::KW_TRUE:            return "KW_TRUE";
        case TokenType::KW_FALSE:           return "KW_FALSE";
    }
    return "UNKNOWN";
}

// print_tokens

void print_tokens(const std::vector<Token>& tokens) {
    for (const auto& token : tokens) {
        std::cout << token.loc.line << ":" << token.loc.column << "  "
                  << to_string(token.type);
        if (token.value.has_value()) {
            std::cout << "  " << token.value.value();
        }
        std::cout << std::endl;
    }
}

// public API

std::vector<Token> tokenize(const std::string& source, const std::string& filename) {
    Lexer lexer(source, filename);
    return lexer.run();
}

}
