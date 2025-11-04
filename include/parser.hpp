#pragma once
#include <string>
#include "token.hpp"
#include "linked_list.hpp"

namespace eda {

// funciones declaradas; definidas en parser.cpp
bool isOperatorChar(char c);
int precedence(const std::string& op);
bool rightAssociative(const std::string& op);
bool isFunctionName(const std::string& s);

// NUEVO: tokenizer que NO requiere espacios
LinkedList<Token> tokenizeSmart(const std::string& line);

// versión antigua por espacios (opcional)
LinkedList<Token> tokenizeBySpace(const std::string& line);

// shunting-yard
LinkedList<Token> infixToPostfix(const LinkedList<Token>& Ein);

std::string tokensToLine(const LinkedList<Token>& L);

} // namespace eda
