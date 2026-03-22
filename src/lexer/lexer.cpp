#include "lexer.hpp"

#include <iostream>
#include <stdexcept>
#include <unordered_map>

namespace mirage {

// keyword tablei

static const std::unordered_map<std::string, TokenType> keywords = {
    {"func",     TokenType::KW_FUNC},
    {"var",      TokenType::KW_VAR},
    {"const",    TokenType::KW_CONST},
    {"return",   TokenType::KW_RETURN},
    {"if",       TokenType::KW_IF},
    {"else",     TokenType::KW_ELSE},
    {"while",    TokenType::KW_WHILE},
    {"for",      TokenType::KW_FOR},
    {"in",       TokenType::KW_IN},
    {"match",    TokenType::KW_MATCH},
    {"break",    TokenType::KW_BREAK},
    {"continue", TokenType::KW_CONTINUE},
    {"struct",   TokenType::KW_STRUCT},
    {"enum",     TokenType::KW_ENUM},
    {"mod",      TokenType::KW_MOD},
    {"use",      TokenType::KW_USE},
    {"pub",      TokenType::KW_PUB},
    {"spawn",    TokenType::KW_SPAWN},
    {"unsafe",   TokenType::KW_UNSAFE},
    {"mut",      TokenType::KW_MUT},
    {"true",     TokenType::KW_TRUE},
    {"false",    TokenType::KW_FALSE},
    {"int",      TokenType::KW_INT},
    {"int8",     TokenType::KW_INT8},
    {"int16",    TokenType::KW_INT16},
    {"int32",    TokenType::KW_INT32},
    {"int64",    TokenType::KW_INT64},
    {"uint8",    TokenType::KW_UINT8},
    {"uint16",   TokenType::KW_UINT16},
    {"uint32",   TokenType::KW_UINT32},
    {"uint64",   TokenType::KW_UINT64},
    {"float",    TokenType::KW_FLOAT},
    {"float32",  TokenType::KW_FLOAT32},
    {"float64",  TokenType::KW_FLOAT64},
    {"bool",     TokenType::KW_BOOL},
    {"string",   TokenType::KW_STRING},
    {"byte",     TokenType::KW_BYTE},
};


class Lexer {
public:
    Lexer(const std::string& source, const std::string& filename)
        : source_(source), filename_(filename) {}

    std::vector<Token> run() {
        std::vector<Token> tokens;
        while (!at_end()) {
            skip_whitespace_and_comments();
            if (at_end()) break;
            tokens.push_back(next_token());
        }
        tokens.push_back(make_token(TokenType::EOF_TOKEN));
        return tokens;
    }

private:
    const std::string& source_;
    const std::string& filename_;
    size_t pos_  = 0;
    size_t line_ = 1;
    size_t col_  = 1;

    bool at_end() const { return pos_ >= source_.size(); }
    char current() const { return source_[pos_]; }

    char peek(size_t offset = 0) const {
        size_t idx = pos_ + offset;
        return idx < source_.size() ? source_[idx] : '\0';
    }

    char advance() {
        char c = source_[pos_++];
        if (c == '\n') { line_++; col_ = 1; } else { col_++; }
        return c;
    }

    bool match(char expected) {
        if (at_end() || source_[pos_] != expected) return false;
        advance();
        return true;
    }

    Token make_token(TokenType type, std::optional<std::string> value = std::nullopt) const {
        return Token{type, value, {line_, col_}};
    }

    Token make_token_at(TokenType type, SourceLocation loc,
                        std::optional<std::string> value = std::nullopt) const {
        return Token{type, value, loc};
    }

    [[noreturn]] void error(const std::string& message) const {
        throw std::runtime_error(
            filename_ + ":" + std::to_string(line_) + ":" +
            std::to_string(col_) + ": " + message);
    }

    void skip_whitespace_and_comments() {
        while (!at_end()) {
            char c = current();
            if (c == ' ' || c == '\t' || c == '\r' || c == '\n') { advance(); continue; }
            if (c == '/' && peek(1) == '/') {
                advance(); advance();
                while (!at_end() && current() != '\n') advance();
                continue;
            }
            if (c == '/' && peek(1) == '*') {
                advance(); advance();
                int depth = 1;
                while (!at_end() && depth > 0) {
                    if (current() == '/' && peek(1) == '*') { advance(); advance(); depth++; }
                    else if (current() == '*' && peek(1) == '/') { advance(); advance(); depth--; }
                    else { advance(); }
                }
                if (depth > 0) error("unterminated block comment");
                continue;
            }
            break;
        }
    }

    Token lex_identifier() {
        SourceLocation loc = {line_, col_};
        std::string word;
        while (!at_end() && (std::isalnum(current()) || current() == '_')) {
            word += advance();
        }
        auto it = keywords.find(word);
        if (it != keywords.end()) return make_token_at(it->second, loc, word);
        return make_token_at(TokenType::IDENTIFIER, loc, word);
    }

    Token lex_number() {
        SourceLocation loc = {line_, col_};
        std::string num;
        if (current() == '0' && (peek(1) == 'x' || peek(1) == 'X')) {
            num += advance(); num += advance();
            if (at_end() || !std::isxdigit(current())) error("expected hex digit after '0x'");
            while (!at_end() && (std::isxdigit(current()) || current() == '_')) {
                if (current() != '_') num += current(); advance();
            }
            return make_token_at(TokenType::INT_LITERAL, loc, num);
        }
        if (current() == '0' && (peek(1) == 'b' || peek(1) == 'B')) {
            num += advance(); num += advance();
            if (at_end() || (current() != '0' && current() != '1')) error("expected binary digit after '0b'");
            while (!at_end() && (current() == '0' || current() == '1' || current() == '_')) {
                if (current() != '_') num += current(); advance();
            }
            return make_token_at(TokenType::INT_LITERAL, loc, num);
        }
        bool is_float = false;
        while (!at_end() && (std::isdigit(current()) || current() == '_')) {
            if (current() != '_') num += current(); advance();
        }
        if (!at_end() && current() == '.' && peek(1) != '.') {
            is_float = true; num += advance();
            while (!at_end() && (std::isdigit(current()) || current() == '_')) {
                if (current() != '_') num += current(); advance();
            }
        }
        if (!at_end() && (current() == 'e' || current() == 'E')) {
            is_float = true; num += advance();
            if (!at_end() && (current() == '+' || current() == '-')) num += advance();
            if (at_end() || !std::isdigit(current())) error("expected digit in exponent");
            while (!at_end() && std::isdigit(current())) num += advance();
        }
        return make_token_at(is_float ? TokenType::FLOAT_LITERAL : TokenType::INT_LITERAL, loc, num);
    }

    Token lex_string() {
        SourceLocation loc = {line_, col_};
        if (peek(1) == '"' && peek(2) == '"') return lex_multiline_string(loc);
        advance();
        std::string value;
        while (!at_end() && current() != '"') {
            if (current() == '\n') error("unterminated string literal");
            if (current() == '\\') {
                advance();
                if (at_end()) error("unterminated escape sequence");
                switch (current()) {
                    case 'n':  value += '\n'; break;
                    case 't':  value += '\t'; break;
                    case 'r':  value += '\r'; break;
                    case '\\': value += '\\'; break;
                    case '"':  value += '"';  break;
                    case '0':  value += '\0'; break;
                    default: error(std::string("unknown escape sequence: \\") + current());
                }
                advance();
            } else { value += advance(); }
        }
        if (at_end()) error("unterminated string literal");
        advance();
        return make_token_at(TokenType::STRING_LITERAL, loc, value);
    }

    Token lex_multiline_string(SourceLocation loc) {
        advance(); advance(); advance();
        std::string value;
        while (!at_end()) {
            if (current() == '"' && peek(1) == '"' && peek(2) == '"') {
                advance(); advance(); advance();
                return make_token_at(TokenType::STRING_LITERAL, loc, value);
            }
            value += advance();
        }
        error("unterminated multiline string literal");
    }

    Token next_token() {
        SourceLocation loc = {line_, col_};
        char c = current();
        if (std::isalpha(c) || c == '_') return lex_identifier();
        if (std::isdigit(c)) return lex_number();
        if (c == '"') return lex_string();
        advance();
        switch (c) {
            case ';': return make_token_at(TokenType::SEMICOLON, loc, ";");
            case ',': return make_token_at(TokenType::COMMA, loc, ",");
            case '(': return make_token_at(TokenType::LPAREN, loc, "(");
            case ')': return make_token_at(TokenType::RPAREN, loc, ")");
            case '{': return make_token_at(TokenType::LBRACE, loc, "{");
            case '}': return make_token_at(TokenType::RBRACE, loc, "}");
            case '[': return make_token_at(TokenType::LBRACKET, loc, "[");
            case ']': return make_token_at(TokenType::RBRACKET, loc, "]");
            case '~': return make_token_at(TokenType::TILDE, loc, "~");
            case '?': return make_token_at(TokenType::QUESTION, loc, "?");
            case '.':
                if (match('.')) {
                    if (match('=')) return make_token_at(TokenType::DOT_DOT_EQ, loc, "..=");
                    return make_token_at(TokenType::DOT_DOT, loc, "..");
                }
                return make_token_at(TokenType::DOT, loc, ".");
            case ':':
                if (match(':')) return make_token_at(TokenType::COLON_COLON, loc, "::");
                return make_token_at(TokenType::COLON, loc, ":");
            case '+':
                if (match('+')) return make_token_at(TokenType::PLUS_PLUS, loc, "++");
                if (match('=')) return make_token_at(TokenType::PLUS_EQ, loc, "+=");
                return make_token_at(TokenType::PLUS, loc, "+");
            case '-':
                if (match('-')) return make_token_at(TokenType::MINUS_MINUS, loc, "--");
                if (match('=')) return make_token_at(TokenType::MINUS_EQ, loc, "-=");
                if (match('>')) return make_token_at(TokenType::ARROW, loc, "->");
                return make_token_at(TokenType::MINUS, loc, "-");
            case '*':
                if (match('=')) return make_token_at(TokenType::STAR_EQ, loc, "*=");
                return make_token_at(TokenType::STAR, loc, "*");
            case '/':
                if (match('=')) return make_token_at(TokenType::SLASH_EQ, loc, "/=");
                return make_token_at(TokenType::SLASH, loc, "/");
            case '%':
                if (match('=')) return make_token_at(TokenType::PERCENT_EQ, loc, "%=");
                return make_token_at(TokenType::PERCENT, loc, "%");
            case '=':
                if (match('=')) return make_token_at(TokenType::EQ_EQ, loc, "==");
                return make_token_at(TokenType::EQ, loc, "=");
            case '!':
                if (match('=')) return make_token_at(TokenType::BANG_EQ, loc, "!=");
                return make_token_at(TokenType::BANG, loc, "!");
            case '<':
                if (match('<')) {
                    if (match('=')) return make_token_at(TokenType::LESS_LESS_EQ, loc, "<<=");
                    return make_token_at(TokenType::LESS_LESS, loc, "<<");
                }
                if (match('=')) return make_token_at(TokenType::LESS_EQ, loc, "<=");
                return make_token_at(TokenType::LESS, loc, "<");
            case '>':
                if (match('>')) {
                    if (match('=')) return make_token_at(TokenType::GREATER_GREATER_EQ, loc, ">>=");
                    return make_token_at(TokenType::GREATER_GREATER, loc, ">>");
                }
                if (match('=')) return make_token_at(TokenType::GREATER_EQ, loc, ">=");
                return make_token_at(TokenType::GREATER, loc, ">");
            case '&':
                if (match('&')) return make_token_at(TokenType::AMP_AMP, loc, "&&");
                if (match('=')) return make_token_at(TokenType::AMP_EQ, loc, "&=");
                return make_token_at(TokenType::AMP, loc, "&");
            case '|':
                if (match('|')) return make_token_at(TokenType::PIPE_PIPE, loc, "||");
                if (match('=')) return make_token_at(TokenType::PIPE_EQ, loc, "|=");
                return make_token_at(TokenType::PIPE, loc, "|");
            case '^':
                if (match('=')) return make_token_at(TokenType::CARET_EQ, loc, "^=");
                return make_token_at(TokenType::CARET, loc, "^");
            default:
                error(std::string("unexpected character: '") + c + "'");
        }
    }
};

// to_string

std::string to_string(TokenType type) {
    switch (type) {
        case TokenType::EOF_TOKEN:          return "EOF";
        case TokenType::INT_LITERAL:        return "INT_LITERAL";
        case TokenType::FLOAT_LITERAL:      return "FLOAT_LITERAL";
        case TokenType::STRING_LITERAL:     return "STRING_LITERAL";
        case TokenType::BOOL_LITERAL:       return "BOOL_LITERAL";
        case TokenType::IDENTIFIER:         return "IDENTIFIER";
        case TokenType::PLUS:               return "PLUS";
        case TokenType::MINUS:              return "MINUS";
        case TokenType::STAR:               return "STAR";
        case TokenType::SLASH:              return "SLASH";
        case TokenType::PERCENT:            return "PERCENT";
        case TokenType::EQ_EQ:              return "EQ_EQ";
        case TokenType::BANG_EQ:            return "BANG_EQ";
        case TokenType::LESS:               return "LESS";
        case TokenType::GREATER:            return "GREATER";
        case TokenType::LESS_EQ:            return "LESS_EQ";
        case TokenType::GREATER_EQ:         return "GREATER_EQ";
        case TokenType::AMP_AMP:            return "AMP_AMP";
        case TokenType::PIPE_PIPE:          return "PIPE_PIPE";
        case TokenType::BANG:               return "BANG";
        case TokenType::AMP:                return "AMP";
        case TokenType::PIPE:               return "PIPE";
        case TokenType::CARET:              return "CARET";
        case TokenType::TILDE:              return "TILDE";
        case TokenType::LESS_LESS:          return "LESS_LESS";
        case TokenType::GREATER_GREATER:    return "GREATER_GREATER";
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
        case TokenType::PLUS_PLUS:          return "PLUS_PLUS";
        case TokenType::MINUS_MINUS:        return "MINUS_MINUS";
        case TokenType::ARROW:              return "ARROW";
        case TokenType::COLON_COLON:        return "COLON_COLON";
        case TokenType::QUESTION:           return "QUESTION";
        case TokenType::DOT_DOT:            return "DOT_DOT";
        case TokenType::DOT_DOT_EQ:         return "DOT_DOT_EQ";
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
