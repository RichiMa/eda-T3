#include <sstream>
#include <stdexcept>
#include <cctype>
#include "parser.hpp"
#include "stack.hpp"

namespace eda {

bool isOperatorChar(char c){
    return c=='+'||c=='-'||c=='*'||c=='/'||c=='^'||c=='=';
}

int precedence(const std::string& op){
    if (op=="^") return 4;               // potencia (asociatividad derecha)
    if (op=="*"||op=="/") return 3;
    if (op=="+"||op=="-") return 2;
    if (op=="=") return 1;               // solo para parseo general; asignación se maneja fuera
    return 0;
}
bool rightAssociative(const std::string& op){ return op=="^"; }
bool isFunctionName(const std::string& s){ return s=="sqrt"; }

// Tokenizador sin espacios
LinkedList<Token> tokenizeSmart(const std::string& line){
    LinkedList<Token> L;
    const char* s = line.c_str();
    size_t n = line.size();
    size_t i = 0;

    auto isIdentStart = [](char c){ return std::isalpha((unsigned char)c) || c=='_'; };
    auto isIdentChar  = [](char c){ return std::isalnum((unsigned char)c) || c=='_'; };

    while (i < n) {
        char c = s[i];
        if (std::isspace((unsigned char)c)) { ++i; continue; }

        if (c=='('){ L.push_back(Token("(",TokenType::LPAREN)); ++i; continue; }
        if (c==')'){ L.push_back(Token(")",TokenType::RPAREN)); ++i; continue; }

        if (isOperatorChar(c) && c!='='){ // '=' lo tratamos fuera
            std::string op(1,c);
            L.push_back(Token(op,TokenType::OPERATOR)); ++i; continue;
        }
        if (c=='='){ // igual: lo tokenizamos como operador, pero asignación se maneja en REPL
            L.push_back(Token("=",TokenType::OPERATOR)); ++i; continue;
        }

        if (std::isdigit((unsigned char)c) || ( (c=='+'||c=='-') && i+1<n && std::isdigit((unsigned char)s[i+1]) )) {
            // número (permite signo delante)
            size_t j = i;
            if (s[j]=='+' || s[j]=='-') ++j;
            bool dot = false;
            while (j<n && (std::isdigit((unsigned char)s[j]) || s[j]=='.')) {
                if (s[j]=='.'){ if (dot) break; dot=true; }
                ++j;
            }
            L.push_back(Token(line.substr(i,j-i), TokenType::OPERAND));
            i = j; continue;
        }

        if (isIdentStart(c)) {
            size_t j = i+1;
            while (j<n && isIdentChar(s[j])) ++j;
            std::string id = line.substr(i,j-i);
            if (isFunctionName(id)) L.push_back(Token(id, TokenType::FUNCTION));
            else L.push_back(Token(id, TokenType::IDENT));
            i = j; continue;
        }

        // carácter desconocido
        throw std::runtime_error(std::string("Carácter inválido: '")+c+"'");
    }
    return L;
}

// Versión por espacios (opcional)
LinkedList<Token> tokenizeBySpace(const std::string& line){
    LinkedList<Token> L;
    std::istringstream iss(line);
    std::string w;
    while (iss >> w){
        if (w=="(") L.push_back(Token(w,TokenType::LPAREN));
        else if (w==")") L.push_back(Token(w,TokenType::RPAREN));
        else if (isFunctionName(w)) L.push_back(Token(w,TokenType::FUNCTION));
        else if (w.size()==1 && isOperatorChar(w[0])) L.push_back(Token(w,TokenType::OPERATOR));
        else L.push_back(Token(w,TokenType::OPERAND));
    }
    return L;
}

// Shunting-yard (Infija→Posfija)
LinkedList<Token> infixToPostfix(const LinkedList<Token>& Ein){
    LinkedList<Token> Epos;
    Stack<Token> P;

    for (auto it=Ein.begin(); it!=Ein.end(); ++it){
        const Token& e = *it;
        if (e.type==TokenType::OPERAND || e.type==TokenType::IDENT){
            Epos.push_back(e);
        } else if (e.type==TokenType::FUNCTION){
            P.push(e);
        } else if (e.type==TokenType::OPERATOR && e.text!="="){
            while (!P.empty() && P.top().type!=TokenType::LPAREN &&
                   ((precedence(e.text) < precedence(P.top().text)) ||
                    (precedence(e.text) == precedence(P.top().text) && !rightAssociative(e.text)))){
                Epos.push_back(P.top()); P.pop();
            }
            P.push(e);
        } else if (e.type==TokenType::LPAREN){
            P.push(e);
        } else if (e.type==TokenType::RPAREN){
            while (!P.empty() && P.top().type!=TokenType::LPAREN){
                Epos.push_back(P.top()); P.pop();
            }
            if (P.empty()) throw std::runtime_error("Paréntesis desbalanceados");
            P.pop(); // '('
            if (!P.empty() && P.top().type==TokenType::FUNCTION){
                Epos.push_back(P.top()); P.pop();
            }
        }
        // ignoramos '=' aquí (la asignación se maneja fuera del shunting-yard)
    }
    while(!P.empty()){
        if (P.top().type==TokenType::LPAREN || P.top().type==TokenType::RPAREN)
            throw std::runtime_error("Paréntesis desbalanceados");
        Epos.push_back(P.top()); P.pop();
    }
    return Epos;
}

std::string tokensToLine(const LinkedList<Token>& L){
    std::ostringstream oss; bool first=true;
    for (auto it=L.begin(); it!=L.end(); ++it){
        if (!first) oss << ' ';
        first=false;
        oss << (*it).text;
    }
    return oss.str();
}

} // namespace eda
