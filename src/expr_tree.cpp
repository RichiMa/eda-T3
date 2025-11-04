#include <cmath>
#include <stdexcept>
#include "expr_tree.hpp"
#include "stack.hpp"

namespace eda {

void ExprTree::destroy(ExprNode* n){
    if(!n) return;
    destroy(n->left);
    destroy(n->right);
    delete n;
}

void ExprTree::toPrefix(const ExprNode* n, LinkedList<Token>& out){
    if(!n) return;
    out.push_back(n->tok);
    toPrefix(n->left,out);
    toPrefix(n->right,out);
}

void ExprTree::toPostfix(const ExprNode* n, LinkedList<Token>& out){
    if(!n) return;
    toPostfix(n->left,out);
    toPostfix(n->right,out);
    out.push_back(n->tok);
}

double ExprTree::eval(const ExprNode* n) const{
    if(!n) throw std::runtime_error("Árbol vacío");
    if (n->tok.type==TokenType::OPERAND) return n->tok.asNumber();
    if (n->tok.type==TokenType::IDENT){
        if (!resolver) throw std::runtime_error("No hay resolver de variables");
        return resolver(n->tok.text);
    }
    if (n->tok.type==TokenType::FUNCTION){
        double v = eval(n->right ? n->right : n->left);
        if (n->tok.text=="sqrt") return std::sqrt(v);
        throw std::runtime_error("Función no soportada: " + n->tok.text);
    }
    double a = eval(n->left);
    double b = eval(n->right);
    char op = n->tok.text[0];
    switch(op){
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': if (b==0) throw std::runtime_error("División por cero"); return a / b;
        case '^': return std::pow(a,b);
    }
    throw std::runtime_error("Operador no soportado");
}

void ExprTree::clear(){ destroy(root); root=nullptr; }

void ExprTree::buildFromPostfix(const LinkedList<Token>& E){
    clear();
    Stack<ExprNode*> st;
    for (auto it=E.begin(); it!=E.end(); ++it){
        const Token& t = *it;
        if (t.type==TokenType::OPERAND || t.type==TokenType::IDENT){
            st.push(new ExprNode(t));
        } else if (t.type==TokenType::FUNCTION){
            if (st.empty()) throw std::runtime_error("Postfija inválida (función)");
            ExprNode* a = st.top(); st.pop();
            auto* n = new ExprNode(t);
            n->right = a; // unaria
            st.push(n);
        } else if (t.type==TokenType::OPERATOR){
            if (st.size()<2) throw std::runtime_error("Postfija inválida (operador)");
            ExprNode* b = st.top(); st.pop();
            ExprNode* a = st.top(); st.pop();
            auto* n = new ExprNode(t);
            n->left = a; n->right = b;
            st.push(n);
        }
    }
    if (st.size()!=1) throw std::runtime_error("Postfija inválida (restos en pila)");
    root = st.top(); st.pop();
}

double ExprTree::evaluate() const { return eval(root); }
LinkedList<Token> ExprTree::toPrefix() const { LinkedList<Token> out; toPrefix(root,out); return out; }
LinkedList<Token> ExprTree::toPostfix() const { LinkedList<Token> out; toPostfix(root,out); return out; }

} // namespace eda
