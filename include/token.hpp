#pragma once
#include <string>
#include <stdexcept>
#include <cctype>

namespace eda {

enum class TokenType { OPERAND, OPERATOR, LPAREN, RPAREN, FUNCTION, IDENT };

struct Token {
    std::string text{};
    TokenType type{TokenType::OPERAND};

    Token() = default;
    Token(std::string t, TokenType tp) : text(std::move(t)), type(tp) {}

    bool isNumber() const {
        if (type != TokenType::OPERAND || text.empty()) return false;
        bool dot = false; size_t i = 0;
        if (text[0] == '+' || text[0] == '-') i = 1;
        for (; i < text.size(); ++i) {
            unsigned char ch = static_cast<unsigned char>(text[i]);
            if (text[i] == '.') { if (dot) return false; dot = true; }
            else if (!std::isdigit(ch)) return false;
        }
        return true;
    }
    double asNumber() const {
        if (!isNumber()) throw std::runtime_error("Token no numérico");
        return std::stod(text);
    }
    bool isOperator(char c) const {
        return type == TokenType::OPERATOR && text.size()==1 && text[0]==c;
    }
};

} // namespace eda
