#include <iostream>
#include <string>
#include <unordered_map>
#include <sstream>
#include "token.hpp"
#include "parser.hpp"
#include "expr_tree.hpp"

using namespace eda;

static void printHelp(){
    std::cout << "Comandos:\n"
              << "  (expresión)            -> evalúa (usa ans y variables)\n"
              << "  x = (expresión)        -> asigna a variable x\n"
              << "  show x                 -> muestra valor de x\n"
              << "  postfix|posfix         -> imprime notación posfija de la última expresión\n"
              << "  prefix                 -> imprime notación prefija de la última expresión\n"
              << "  tree                   -> imprime árbol de la última expresión\n"
              << "  help                   -> ayuda\n"
              << "  exit                   -> salir\n";
}

// Impresión simple de árbol (estilo jerárquico con ramas)
static void printTreeRec(const ExprNode* n, const std::string& prefix, bool isLeft){
    if (!n) return;
    std::cout << prefix << (isLeft ? "├─ " : "└─ ") << n->tok.text << "\n";
    if (n->left || n->right) {
        std::string next = prefix + (isLeft ? "│  " : "   ");
        if (n->left)  printTreeRec(n->left,  next, true);
        if (n->right) printTreeRec(n->right, next, false);
    }
}

static void printTree(const ExprTree& t){
    const ExprNode* r = t.getRoot();
    if (!r){ std::cout << "(árbol vacío)\n"; return; }
    std::cout << r->tok.text << "\n";
    if (r->left)  printTreeRec(r->left,  "", true);
    if (r->right) printTreeRec(r->right, "", false);
}

int main(){
    std::cout << "Bienvenido a EdaCal\n";

    // memoria de variables
    std::unordered_map<std::string,double> vars;
    vars["ans"] = 0.0;

    LinkedList<Token> lastInfix;
    LinkedList<Token> lastPostfix;
    ExprTree tree;
    tree.setResolver([&](const std::string& name)->double{
        auto it = vars.find(name);
        if (it==vars.end()) throw std::runtime_error("Variable no definida: " + name);
        return it->second;
    });

    std::string line;
    while (true){
        std::cout << "≫ ";
        if (!std::getline(std::cin, line)) break;

        // trim
        auto trim = [](std::string& s){
            size_t a = s.find_first_not_of(" \t\r\n");
            size_t b = s.find_last_not_of(" \t\r\n");
            if (a==std::string::npos) { s.clear(); return; }
            s = s.substr(a, b-a+1);
        };
        trim(line);
        if (line.empty()) continue;
        if (line=="exit") break;
        if (line=="help") { printHelp(); continue; }

        // comandos puros
        if (line=="postfix" || line=="posfix"){
            try{
                auto L = tree.toPostfix();
                std::cout << tokensToLine(L) << "\n";
            } catch(const std::exception& ex){ std::cout << "ERROR: " << ex.what() << "\n"; }
            continue;
        }
        if (line=="prefix"){
            try{
                auto L = tree.toPrefix();
                std::cout << tokensToLine(L) << "\n";
            } catch(const std::exception& ex){ std::cout << "ERROR: " << ex.what() << "\n"; }
            continue;
        }
        if (line=="tree"){
            try{ printTree(tree); }
            catch(const std::exception& ex){ std::cout << "ERROR: " << ex.what() << "\n"; }
            continue;
        }
        if (line.rfind("show ",0)==0){
            std::string name = line.substr(5);
            trim(name);
            auto it = vars.find(name);
            if (it==vars.end()) { std::cout << "ERROR: variable no definida\n"; continue; }
            std::cout << name << " -> " << it->second << "\n";
            continue;
        }

        // asignación: x = expr
        size_t eq = line.find('=');
        if (eq != std::string::npos){
            std::string lhs = line.substr(0, eq);
            std::string rhs = line.substr(eq+1);
            trim(lhs); trim(rhs);
            if (lhs.empty()) { std::cout << "ERROR: LHS vacío\n"; continue; }

            // LHS debe ser identificador válido
            bool ok = std::isalpha((unsigned char)lhs[0]) || lhs[0]=='_';
            for (size_t i=1; ok && i<lhs.size(); ++i){
                ok = std::isalnum((unsigned char)lhs[i]) || lhs[i]=='_';
            }
            if (!ok){ std::cout << "ERROR: nombre de variable inválido\n"; continue; }

            try{
                lastInfix = tokenizeSmart(rhs);
                lastPostfix = infixToPostfix(lastInfix);
                tree.buildFromPostfix(lastPostfix);
                double v = tree.evaluate();
                vars[lhs] = v;
                vars["ans"] = v;
                std::cout << lhs << " -> " << v << "\n";
            } catch(const std::exception& ex){
                std::cout << "ERROR: " << ex.what() << "\n";
            }
            continue;
        }

        // expresión pura
        try{
            lastInfix = tokenizeSmart(line);
            lastPostfix = infixToPostfix(lastInfix);
            tree.buildFromPostfix(lastPostfix);
            double v = tree.evaluate();
            vars["ans"] = v;
            std::cout << "ans -> " << v << "\n";
        } catch(const std::exception& ex){
            std::cout << "ERROR: " << ex.what() << "\n";
        }
    }
    return 0;
}
