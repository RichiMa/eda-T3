#pragma once
#include <functional>
#include "token.hpp"
#include "linked_list.hpp"

namespace eda {

struct ExprNode {
    Token tok;
    ExprNode* left{nullptr};
    ExprNode* right{nullptr};
    explicit ExprNode(const Token& t): tok(t) {}
};

class ExprTree {
    ExprNode* root{nullptr};
    // Resolver de identificadores (variables)
    std::function<double(const std::string&)> resolver;

    static void destroy(ExprNode* n);
    static void toPrefix(const ExprNode* n, LinkedList<Token>& out);
    static void toPostfix(const ExprNode* n, LinkedList<Token>& out);
    double eval(const ExprNode* n) const;

public:
    ~ExprTree(){ clear(); }
    void clear();
    void setResolver(std::function<double(const std::string&)> r){ resolver = std::move(r); }
    void buildFromPostfix(const LinkedList<Token>& E);
    double evaluate() const;
    LinkedList<Token> toPrefix() const;
    LinkedList<Token> toPostfix() const;
    const ExprNode* getRoot() const { return root; }
};

} // namespace eda
